#include "db_handler.hh"

#include <fstream>
#include "utils/debug.hh"
#include "../sqlite/sqlite3.h"
#include "execution.hh"
#include "utils/tree_utils.hh"
#include "utils/perf_helper.hh"

namespace cpprofiler
{
namespace db_handler
{

typedef int (*SQL_Callback)(void *, int, char **, char **);

struct NodeData
{
    NodeID nid;
    NodeID pid;
    int alt;
    int kids;
    tree::NodeStatus status;
    std::string label;
};

struct BookmarkItem
{
    NodeID nid;
    std::string text;
};

struct NogoodItem
{
    NodeID nid;
    std::string text;
};

struct CloseSqlStatement
{
    void operator()(sqlite3_stmt *stmt)
    {
        if (sqlite3_finalize(stmt) != SQLITE_OK)
        {
            debug("error") << "could not finalize a statement in db\n";
        }
    }
};

struct CloseDB
{
    void operator()(sqlite3 *db)
    {
        if (sqlite3_close(db) != SQLITE_OK)
        {
            debug("error") << "could not close db\n";
        }
    }
};

using SqlStatement = std::unique_ptr<sqlite3_stmt, CloseSqlStatement>;
using Sqlite3 = std::unique_ptr<sqlite3, CloseDB>;

/// Execute `sql` query; return true on success
static bool execute_query(sqlite3 *db, const char *query, SQL_Callback cb = nullptr, void *arg = nullptr)
{
    char *zErrMsg = 0;

    bool success = true;

    auto rc = sqlite3_exec(db, query, cb, arg, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        debug("error") << "SQL error: " << zErrMsg << '\n';
        success = false;
    }

    sqlite3_free(zErrMsg);
    return success;
}

static int count_nodes(sqlite3 *db)
{

    int count = 0;

    const auto query = "select count (*) from Nodes;";
    const auto success = execute_query(db, query, [](void *count, int ncols, char **cols, char **) -> int {
        int *val = reinterpret_cast<int *>(count);

        if (ncols == 1)
        {
            const auto str_val = cols[0];
            /// Note that atoi returns zero on error
            *val = atoi(str_val);
        }
        /// sqlite requires the return value of 0 to go on with the query
        return 0;
    },
                                       reinterpret_cast<void *>(&count));

    /// For now return zero on any error
    if (!success)
        count = 0;

    return count;
}

/// Prepare statement using `query`
static SqlStatement prepare_statement(sqlite3 *db, const char *query)
{
    const char *pzTest;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &stmt, &pzTest);
    if (rc != SQLITE_OK)
    {
        debug("error") << "SQL error: " << pzTest << '\n';
    }

    return SqlStatement{stmt};
}

/// Reads all nodes from a database and builds the tree; returns `true` on success
static bool read_nodes(sqlite3 *db, Execution &ex)
{
    const auto query = "select * from Nodes;";

    auto select_stmt = prepare_statement(db, query);

    auto &tree = ex.tree();

    const auto total_nodes = count_nodes(db);

    tree.db_initialize(total_nodes);

    bool success = false;

    while (true)
    {

        int res = sqlite3_step(select_stmt.get());

        if (res == SQLITE_DONE)
        {
            print("sqlite done");
            success = true;
            break;
        }

        if (res == SQLITE_ERROR)
        {
            print("sqlite runtime ERROR!");
            break;
        }

        const auto nid = NodeID(sqlite3_column_int(select_stmt.get(), 0));
        const auto pid = NodeID(sqlite3_column_int(select_stmt.get(), 1));
        const auto alt = sqlite3_column_int(select_stmt.get(), 2);
        const auto kids = sqlite3_column_int(select_stmt.get(), 3);
        const auto status = tree::NodeStatus(sqlite3_column_int(select_stmt.get(), 4));
        const auto label = (const char *)sqlite3_column_text(select_stmt.get(), 5);

        if (pid == NodeID::NoNode)
        {
            tree.db_createRoot(nid, label);
        }
        else
        {
            tree.db_addChild(nid, pid, alt, status, label);
        }

        if (res != SQLITE_ROW)
            break;
    }

    return success;
}

/// Read all bookmarks from the database
static bool read_bookmarks(sqlite3 *db, Execution &ex)
{
    const auto query = "select * from Bookmarks;";
    auto select_bm_ = prepare_statement(db, query);

    bool success = false;

    auto &ud = ex.userData();

    while (true)
    {

        int res = sqlite3_step(select_bm_.get());

        if (res == SQLITE_DONE)
        {
            print("sqlite done");
            success = true;
            break;
        }

        if (res == SQLITE_ERROR)
        {
            print("sqlite runtime ERROR!");
            break;
        }

        const auto nid = NodeID(sqlite3_column_int(select_bm_.get(), 0));
        const auto bm_text = (const char *)(sqlite3_column_text(select_bm_.get(), 1));

        ud.setBookmark(nid, bm_text);

        if (res != SQLITE_ROW)
            break;
    }

    return success;
}

/// Read all nogoods from the database
static bool read_nogoods(sqlite3 *db, Execution &ex)
{
    const auto query = "select * from Nogoods;";
    auto select_ng_ = prepare_statement(db, query);

    bool success = false;

    auto &sd = ex.solver_data();

    while (true)
    {

        int res = sqlite3_step(select_ng_.get());

        if (res == SQLITE_DONE)
        {
            success = true;
            break;
        }

        if (res == SQLITE_ERROR)
        {
            print("sqlite runtime ERROR!");
            break;
        }

        const auto nid = NodeID(sqlite3_column_int(select_ng_.get(), 0));
        const auto ng_text = (const char *)(sqlite3_column_text(select_ng_.get(), 1));

        sd.setNogood(nid, {ng_text});

        if (res != SQLITE_ROW)
            break;
    }

    return success;
}

/// Opens a database file returing a handle on success (nullptr otherwise)
static Sqlite3 open_db(const char *path)
{

    sqlite3 *db;
    print("openDB: {}", path);
    int res = sqlite3_open_v2(path, &db, SQLITE_OPEN_READONLY, nullptr);
    if (res != SQLITE_OK)
    {
        debug("error") << "error opening a DB file\n";

        /// need to close the DB even when an error occurred
        if (sqlite3_close(db) != SQLITE_OK)
        {
            debug("error") << "could not close db\n";
        }

        return nullptr;
    }

    return Sqlite3{db};
}

static void insert_node(sqlite3_stmt *stmt, NodeData nd)
{

    if (sqlite3_reset(stmt) != SQLITE_OK)
    {
        debug("error") << "could not reset db statement\n";
    }

    sqlite3_bind_int(stmt, 1, nd.nid);
    sqlite3_bind_int(stmt, 2, nd.pid);
    sqlite3_bind_int(stmt, 3, nd.alt);
    sqlite3_bind_int(stmt, 4, nd.kids);
    sqlite3_bind_int(stmt, 5, static_cast<int>(nd.status));
    sqlite3_bind_text(stmt, 6, nd.label.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        debug("error") << "could not execute db statement\n";
    }
}

static void save_nodes(sqlite3 *db, const Execution *ex)
{

    // db.prepareInsert();

    const char *insert_query = "INSERT INTO Nodes \
                         (NodeID, ParentID, Alternative, NKids, Status, Label) \
                         VALUES (?,?,?,?,?,?);";

    auto insert_bm = prepare_statement(db, insert_query);

    const auto &tree = ex->tree();
    const auto order = utils::pre_order(tree);

    constexpr static int TRANSACTION_SIZE = 50000;

    bool success = 0;

    execute_query(db, "BEGIN;");
    for (auto i = 0u; i < order.size(); ++i)
    {
        const auto nid = order[i];
        const auto pid = tree.getParent(nid);
        const auto alt = tree.getAlternative(nid);
        const auto kids = tree.childrenCount(nid);
        const auto status = tree.getStatus(nid);
        const auto label = tree.getLabel(nid);

        insert_node(insert_bm.get(), {nid, pid, alt, kids, status, label});

        if (i % TRANSACTION_SIZE == TRANSACTION_SIZE - 1)
        {
            execute_query(db, "END;");
            execute_query(db, "BEGIN;");
        }
    }
    execute_query(db, "END;");
}

static void insert_bookmark(sqlite3_stmt *stmt, BookmarkItem bi)
{

    if (sqlite3_reset(stmt) != SQLITE_OK)
    {
        debug("error") << "could not reset db statement\n";
    }

    sqlite3_bind_int(stmt, 1, bi.nid);
    sqlite3_bind_text(stmt, 2, bi.text.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        debug("error") << "could not execute db statement\n";
    }
}

static void insert_nogood(sqlite3_stmt *stmt, NogoodItem ngi)
{

    if (sqlite3_reset(stmt) != SQLITE_OK)
    {
        debug("error") << "could not reset db statement\n";
    }

    sqlite3_bind_int(stmt, 1, ngi.nid);
    sqlite3_bind_text(stmt, 2, ngi.text.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        debug("error") << "could not execute db statement\n";
    }
}

static void save_nogoods(sqlite3 *db, const Execution *ex)
{

    print("saving nogoods");

    const char *query = "INSERT INTO Nogoods \
                         (NodeID, Nogood) \
                         VALUES (?,?);";

    auto insert_ng_stmt = prepare_statement(db, query);

    const auto &nt = ex->tree();
    const auto &sd = ex->solver_data();

    const auto nodes = utils::any_order(nt);

    execute_query(db, "BEGIN;");

    for (const auto n : nodes)
    {
        const auto &text = sd.getNogood(n).get();
        if (text != "")
        {
            insert_nogood(insert_ng_stmt.get(), {n, text});
        }
    }

    execute_query(db, "END;");
}

static void save_user_data(sqlite3 *db, const Execution *ex)
{

    const char *query = "INSERT INTO Bookmarks \
                         (NodeID, Bookmark) \
                         VALUES (?,?);";

    auto insert_bm_stmt = prepare_statement(db, query);

    const auto &ud = ex->userData();
    const auto nodes = ud.bookmarkedNodes();

    execute_query(db, "BEGIN;");

    for (const auto n : nodes)
    {
        const auto &text = ud.getBookmark(n);
        insert_bookmark(insert_bm_stmt.get(), {n, text});
    }

    execute_query(db, "END;");
}

/// Create a file at `path` (or overwrite it) and associate it with `db`
static Sqlite3 create_db(const char *path)
{
    debug("force") << "creating file: " << path << std::endl;
    std::ofstream file(path);
    file.close();

    sqlite3 *db;

    int res = sqlite3_open(path, &db);
    if (res != SQLITE_OK)
    {
        debug("error") << "error opening a DB file\n";
        return nullptr;
    }

    const auto success1 = execute_query(db, "CREATE TABLE Nodes( \
      NodeID INTEGER PRIMARY KEY, \
      ParentID int NOT NULL, \
      Alternative int NOT NULL, \
      NKids int NOT NULL, \
      Status int, \
      Label varchar(256) \
      );");

    const auto success2 = execute_query(db, "Create TABLE Bookmarks( \
        NodeID INTEGER PRIMARY KEY, \
        Bookmark varchar(8) \
        );");

    const auto success3 = execute_query(db, "CREATE TABLE Nogoods( \
        NodeID INTEGER PRIMARY KEY, \
        Nogood varchar(8) \
    );");

    if (success1 && success2 && success3)
    {
        return Sqlite3{db};
    }
    else
    {
        return nullptr;
    }
}

/// this takes (without nogoods) under 2 sec for a ~1.5M nodes (golomb 10)
void save_execution(const Execution *ex, const char *path)
{

    perfHelper.begin("save execution");

    auto db = create_db(path);

    save_nodes(db.get(), ex);

    save_user_data(db.get(), ex);

    const auto &sd = ex->solver_data();

    if (sd.hasNogoods())
    {
        save_nogoods(db.get(), ex);
    }

    perfHelper.end();
}

std::shared_ptr<Execution> load_execution(const char *path, ExecID eid)
{

    auto db = open_db(path);

    if (!db)
        return nullptr;

    auto ex = std::make_shared<Execution>(path, eid);

    read_nodes(db.get(), *ex);

    read_bookmarks(db.get(), *ex);

    read_nogoods(db.get(), *ex);

    ex->tree().setDone();

    return ex;
}
} // namespace db_handler
} // namespace cpprofiler