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


bool DB_Handler::executeQuery(const char *sql, SQL_Callback cb, void * arg) const
{
    char *zErrMsg = 0;

    bool success = true;

    auto rc = sqlite3_exec(db_handle_, sql, cb, arg, &zErrMsg);
    if (rc != SQLITE_OK) {
        debug("error") << "SQL error: " << zErrMsg << '\n';
        success = false;
    }

    sqlite3_free(zErrMsg);
    return success;
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

int DB_Handler::countNodes() const {

    const auto query = "select count (*) from Nodes;";

    int count = 0;

    const auto success = executeQuery(query, [](void* count, int ncols, char** cols, char**) -> int {
        int* val = reinterpret_cast<int*>(count);

        if (ncols == 1) {
            const auto str_val = cols[0];
            /// Note that atoi returns zero on error
            *val = atoi(str_val);
        }
        /// sqlite requires the return value of 0 to go on with the query
        return 0;
    }, reinterpret_cast<void*>(&count));

    /// For now return zero on any error
    if (!success) count = 0;

    return count;
}

bool DB_Handler::readNodes(Execution& ex) const {

    const auto query = "select * from Nodes";

    auto select_stmt = prepare_select(db_handle_);

    auto& tree = ex.tree();

    const auto total_nodes = countNodes();

    tree.db_initialize(total_nodes);

    bool success = false;

    while(true) {

        int res = sqlite3_step(select_stmt);

        if (res == SQLITE_DONE) {
            print("sqlite done");
            success = true;
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
            tree.db_createRoot(nid, label);
        } else {
            tree.db_addChild(nid, pid, alt, status, label);
        }

        if (res != SQLITE_ROW) break;
    }

    if (sqlite3_finalize(select_stmt) != SQLITE_OK) {
        debug("error") << "could not finalize a statement in db\n";
    }

    return success;
}

std::shared_ptr<Execution> DB_Handler::load_execution(const char* path) {

    DB_Handler db;

    if (!db.openDB(path)) return nullptr;

    auto ex = std::make_shared<Execution>(path);

    db.readNodes(*ex);

    return ex;
}

/// this takes under 2 sec for a ~1.5M nodes (golomb 10)
void DB_Handler::save_execution(Execution* ex, const char* path) {

    perfHelper.begin("save execution");

    DB_Handler db;

    db.createDB(path);
    db.prepareInsert();

    const auto& tree = ex->tree();
    const auto order = utils::pre_order(tree);

    constexpr static int TRANSACTION_SIZE = 50000;

    bool success = 0;

    db.executeQuery("BEGIN;");
    for (auto i = 0u; i < order.size(); ++i) {
        const auto nid = order[i];
        const auto pid = tree.getParent(nid);
        const auto alt = tree.getAlternative(nid);
        const auto kids = tree.childrenCount(nid);
        const auto status = tree.getStatus(nid);
        const auto label = tree.getLabel(nid);
        db.insertNode({nid, pid, alt, kids, status, label});

        if (i % TRANSACTION_SIZE == TRANSACTION_SIZE - 1) {
            db.executeQuery("END;");
            db.executeQuery("BEGIN;");
        }
    }
    db.executeQuery("END;");

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

bool DB_Handler::prepareInsert() {
    const char *pzTest;
    const char* query = "INSERT INTO Nodes \
                         (NodeID, ParentID, Alternative, NKids, Status, Label) \
                         VALUES (?,?,?,?,?,?);";

    int rc = sqlite3_prepare_v2(db_handle_, query, strlen(query) + 1, &insert_stmt_, &pzTest);
    if (rc != SQLITE_OK) {
        debug("error") << "SQL error: " << pzTest << '\n';
        return false;
    }

    return true;
}

bool DB_Handler::createDB(const char* path) {
    /// clear the file

    debug("force") << "creating file: " << path << std::endl;
    std::ofstream file(path);
    file.close();

    int res = sqlite3_open(path, &db_handle_);
    if (res !=  SQLITE_OK) {
        debug("error") << "error opening a DB file\n";
        return false;
    }

    const auto success = executeQuery("CREATE TABLE Nodes( \
      NodeID INTEGER PRIMARY KEY, \
      ParentID int NOT NULL, \
      Alternative int NOT NULL, \
      NKids int NOT NULL, \
      Status int, \
      Label varchar(256) \
      );"
    );

    return success;
}

bool DB_Handler::openDB(const char* path) {
    print("openDB: {}", path);
    int res = sqlite3_open_v2(path, &db_handle_, SQLITE_OPEN_READONLY, nullptr);
    if (res !=  SQLITE_OK) {
        debug("error") << "error opening a DB file\n";
        
        /// need to close the DB even when an error occurred
        if (sqlite3_close(db_handle_) != SQLITE_OK) {
            debug("error") << "could not close db\n";
        }

        db_handle_ = nullptr;
        return false;
    }

    return true;
}


}