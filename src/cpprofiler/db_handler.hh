
#pragma once

#include "core.hh"

class sqlite3;
class sqlite3_stmt;

typedef int(*SQL_Callback)(void*,int,char**,char**);

namespace cpprofiler {

class Execution;

struct NodeData {
    NodeID nid;
    NodeID pid; 
    int alt;
    int kids;
    tree::NodeStatus status;
    std::string label;
};

class DB_Handler {

sqlite3* db_handle_ = nullptr;
sqlite3_stmt* insert_stmt_ = nullptr;


void insertNode(NodeData data) const;

int countNodes() const;

/// Execute `sql` query; return true on success
bool executeQuery(const char *sql, SQL_Callback cb = nullptr, void * arg = nullptr) const;

/// Prepare an sql statement for insterting nodes; returns `true` on success
bool prepareInsert();

/// Create a file at `path` (or overwrite it) and associate it with `db_handle_`
bool createDB(const char* path);

/// Reads all nodes from a database and creates a tree; returns `true` on success
bool readNodes(Execution& ex) const;

/// Opens a database file returing `true` (and initializing db_handle_) on success 
bool openDB(const char* path);

DB_Handler();
~DB_Handler();

public:

    static void save_execution(Execution* ex, const char* path);

    static std::shared_ptr<Execution> load_execution(const char* path);


};


}