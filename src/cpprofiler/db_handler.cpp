#include "db_handler.hh"

#include <fstream>
#include "utils/debug.hh"
#include "../sqlite/sqlite3.h"
#include "execution.hh"
#include "utils/tree_utils.hh"
#include "utils/perf_helper.hh"

namespace cpprofiler {

DB_Handler::DB_Handler() {

    
}

DB_Handler::~DB_Handler() {
    if (sqlite3_finalize(insert_stmt_) != SQLITE_OK) {
        debug("error") << "could not finalize a statement in db\n";
    }

    if (sqlite3_close(db_handle_) != SQLITE_OK) {
        debug("error") << "could not close db\n";
    }
}

static int nodeCallback(void*,int ncolumns,char** columns,char** col_names) {
    qDebug() << "node callback";
    qDebug() << "columns:" << ncolumns;

    for (auto i = 0u; i < ncolumns; ++i) {
        if (columns[i]) {
            debug("force") << "col " << i << ":" << "name: " << col_names[i] << " ";
            debug("force") << "val:" << columns[i] << "\n";
        }
    }

    /// sqlite requires the return value of 0 to go on with the query
    return 0;
}


void DB_Handler::executeQuery(const char *sql, SQL_Callback cb, void * arg) const
{
    char *zErrMsg = 0;

    auto rc = sqlite3_exec(db_handle_, sql, cb, arg, &zErrMsg);
    if (rc != SQLITE_OK) {
        debug("error") << "SQL error: " << zErrMsg << '\n';
    }

    sqlite3_free(zErrMsg);
}

/// returns a nullptr on failure
static sqlite3_stmt* prepare_select(sqlite3* db) {
    const char *pzTest;
    const char* query = "select * from nodes;";

    sqlite3_stmt* select_stmt;
    int rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &select_stmt, &pzTest);
    if (rc != SQLITE_OK) {
        debug("error") << "SQL error: " << pzTest << '\n';
    }

    return select_stmt;
}

void DB_Handler::readNodes(Execution& ex) const {

    const auto query = "select * from Nodes";

    auto select_stmt = prepare_select(db_handle_);

    auto& tree = ex.tree();

    while(true) {

        int res = sqlite3_step(select_stmt);

        if (res == SQLITE_DONE) {
            print("sqlite done");
            break;
        }

        if (res == SQLITE_ERROR) {
            print("sqlite runtime ERROR!");
            break;
        }

        const auto nid = NodeID(sqlite3_column_int(select_stmt, 0));
        const auto pid = NodeID(sqlite3_column_int(select_stmt, 1));
        const auto alt = sqlite3_column_int(select_stmt, 2);
        const auto kids = sqlite3_column_int(select_stmt, 3);
        const auto status = tree::NodeStatus(sqlite3_column_int(select_stmt, 4));
        const auto label = (const char*)sqlite3_column_text(select_stmt, 5);

        if (pid == NodeID::NoNode) {
            tree.createRoot(kids, label);
        } else {
            print("promote node, nid: {}, pid: {}, alt: {}, kids: {}, status: {}, label: {}", nid, pid, alt, kids, status, label);
            tree.promoteNode(pid, alt, kids, status, label);
            // tree.promoteNode(nid, kids, status, label);
        }

        if (res != SQLITE_ROW) break;
    }

    if (sqlite3_finalize(select_stmt) != SQLITE_OK) {
        debug("error") << "could not finalize a statement in db\n";
    }

    // executeQuery(query, nodeCallback);
}

std::shared_ptr<Execution> DB_Handler::loadExecution(const char* path) {

    open_db(path);

    if (!db_handle_) return nullptr;

    auto ex = std::make_shared<Execution>(path);

    readNodes(*ex);

    return ex;
}

/// this takes under 2 sec for a ~1.5M nodes (golomb 10)
void DB_Handler::save_execution(Execution* ex) const {

    perfHelper.begin("save execution");

    auto& tree = ex->tree();

    auto order = utils::preOrder(tree);

    constexpr static int TRANSACTION_SIZE = 50000;

    executeQuery("BEGIN;");
    for (auto i = 0u; i < order.size(); ++i) {
        const auto nid = order[i];
        const auto pid = tree.getParent(nid);
        const auto alt = tree.getAlternative(nid);
        const auto kids = tree.childrenCount(nid);
        const auto status = tree.getStatus(nid);
        const auto label = tree.getLabel(nid);
        insertNode({nid, pid, alt, kids, status, label});

        if (i % TRANSACTION_SIZE == TRANSACTION_SIZE - 1) {
            executeQuery("END;");
            executeQuery("BEGIN;");
        }
    }
    executeQuery("END;");

    perfHelper.end();

}

void DB_Handler::insertNode(NodeData nd) const {

    if (sqlite3_reset(insert_stmt_) != SQLITE_OK) {
        debug("error") << "could not reset db statement\n";
    }

    sqlite3_bind_int(insert_stmt_, 1, nd.nid);
    sqlite3_bind_int(insert_stmt_, 2, nd.pid);
    sqlite3_bind_int(insert_stmt_, 3, nd.alt);
    sqlite3_bind_int(insert_stmt_, 4, nd.kids);
    sqlite3_bind_int(insert_stmt_, 5, static_cast<int>(nd.status));
    sqlite3_bind_text(insert_stmt_, 6, nd.label.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(insert_stmt_) != SQLITE_DONE) {
        debug("error") << "could not execute db statement\n";
    }

}

static sqlite3_stmt* prepareInsert(sqlite3* db) {
    const char *pzTest;
    const char* query = "INSERT INTO Nodes \
                         (NodeID, ParentID, Alternative, NKids, Status, Label) \
                         VALUES (?,?,?,?,?,?);";


    sqlite3_stmt* insert_stmt;
    int rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &insert_stmt, &pzTest);
    if (rc != SQLITE_OK) {
        debug("error") << "SQL error: " << pzTest << '\n';
    }

    return insert_stmt;
}

void DB_Handler::create_db(const char* path) {
    /// clear the file

    debug("force") << "creating file: " << path << std::endl;
    std::ofstream file(path);
    file.close();

    int res = sqlite3_open(path, &db_handle_);
    if (res !=  SQLITE_OK) {
        debug("error") << "error opening a DB file\n";
        return;
    }

    executeQuery("CREATE TABLE Nodes( \
      NodeID INTEGER PRIMARY KEY, \
      ParentID int NOT NULL, \
      Alternative int NOT NULL, \
      NKids int NOT NULL, \
      Status int, \
      Label varchar(256) \
      );"
    );

    insert_stmt_ = prepareInsert(db_handle_);
}

void DB_Handler::open_db(const char* path) {
    debug("force") << path << std::endl;
    int res = sqlite3_open_v2(path, &db_handle_, SQLITE_OPEN_READONLY, nullptr);
    if (res !=  SQLITE_OK) {
        debug("error") << "error opening a DB file\n";
        
        /// need to close the DB even when an error occurred
        if (sqlite3_close(db_handle_) != SQLITE_OK) {
            debug("error") << "could not close db\n";
        }

        db_handle_ = nullptr;
    }
}


}