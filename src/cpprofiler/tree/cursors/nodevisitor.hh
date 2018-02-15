#ifndef CPPROFILER_TREE_CURSORS_NODE_VISITOR_HH
#define CPPROFILER_TREE_CURSORS_NODE_VISITOR_HH

namespace cpprofiler { namespace tree {

template<typename Cursor>
class NodeVisitor {

protected:
    Cursor m_cursor;

public:

    NodeVisitor(const Cursor& c);


};

template<typename Cursor>
class PreorderNodeVisitor : public NodeVisitor<Cursor> {

    using NodeVisitor<Cursor>::m_cursor;

protected:
    /// Move cursor to next node from a leaf
    bool backtrack();
    /// Move cursor to the next node, return true if succeeded
    bool next();

public:

    PreorderNodeVisitor(const Cursor& c);
    /// Execute visitor
    void run();
};

#include "nodevisitor.hpp"

}}


#endif