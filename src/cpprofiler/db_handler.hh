
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

void readNodes(Execution& ex) const;

int countNodes() const;

void executeQuery(const char *sql, SQL_Callback cb = nullptr, void * arg = nullptr) const;

public:
    DB_Handler();
    ~DB_Handler();

    void create_db(const char* path);

    /// Opens a database file initializing db_handle_ on success
    void open_db(const char* path);

    void save_execution(Execution* ex) const;

    std::shared_ptr<Execution> loadExecution(const char* path);


};


}