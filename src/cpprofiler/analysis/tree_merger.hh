#pragma once


#include <QThread>

namespace cpprofiler {

namespace tree {
    class NodeTree;
}

    class Execution;
}

namespace cpprofiler { namespace analysis {


    class TreeMerger : public QThread {

        const Execution& ex_l;
        const Execution& ex_r;

        const tree::NodeTree& tree_l;
        const tree::NodeTree& tree_r;

        tree::NodeTree& res_tree;


protected:
        void run() override;
public:
        TreeMerger(const Execution& ex_l, const Execution& ex_r, tree::NodeTree& nt);
        ~TreeMerger();


    };



}}