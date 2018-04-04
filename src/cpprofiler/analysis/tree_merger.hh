#pragma once


#include <QThread>

#include "merging/merge_result.hh"

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
        MergeResult& merge_result;


protected:
        void run() override;
public:
        TreeMerger(const Execution& ex_l, const Execution& ex_r, tree::NodeTree& nt, MergeResult& res);
        ~TreeMerger();


    };



}}