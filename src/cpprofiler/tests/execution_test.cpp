#include "execution_test.hh"

#include "../core.hh"
#include "../conductor.hh"
#include "../user_data.hh"

#include "../execution.hh"
#include "../tree/structure.hh"
#include "../utils/array.hh"
#include "../tree/node_info.hh"
#include "../execution_window.hh"
#include "../tree/traditional_view.hh"

#include <QDebug>
#include <iostream>

namespace cpprofiler { namespace tests { namespace execution {

    void copy_test(utils::Array<int> arr) {
        auto new_arr = arr;
    }

    void array_test() {

        utils::Array<int> arr(1);

        copy_test(arr);

    }

    void binary_tree_execution(Conductor& conductor) {

        auto ex = new Execution("test execution");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.createRootNew(2, "0");
        auto n1 =  tree.addNodeNew(root, 0, 2, tree::NodeStatus::BRANCH, "1");
        auto n2 =  tree.addNodeNew(root, 1, 2, tree::NodeStatus::BRANCH, "2");

        conductor.getExecutionWindow(ex).traditional_view().selectNode(n1);

        auto n3 =  tree.addNodeNew(n1, 0, 2, tree::NodeStatus::BRANCH, "3");
        auto n4 =  tree.addNodeNew(n1, 1, 2, tree::NodeStatus::BRANCH, "4");

        auto n5 =  tree.addNodeNew(n3, 0, 2, tree::NodeStatus::BRANCH, "7");
        auto n6 =  tree.addNodeNew(n3, 1, 2, tree::NodeStatus::BRANCH, "8");

        tree.addNodeNew(n5, 0, 0, tree::NodeStatus::SOLVED);
        tree.addNodeNew(n5, 1, 0, tree::NodeStatus::FAILED);

        tree.addNodeNew(n6, 0, 0, tree::NodeStatus::FAILED);
        tree.addNodeNew(n6, 1, 0, tree::NodeStatus::SKIPPED);

        auto n7 =  tree.addNodeNew(n4, 0, 0, tree::NodeStatus::FAILED);
        auto n8 =  tree.addNodeNew(n4, 1, 0, tree::NodeStatus::FAILED);

        auto n9 =  tree.addNodeNew(n2, 0, 2, tree::NodeStatus::BRANCH);
        auto n10 =  tree.addNodeNew(n2, 1, 0, tree::NodeStatus::FAILED);

        auto n11 =  tree.addNodeNew(n9, 0, 2, tree::NodeStatus::BRANCH);
        auto n12 =  tree.addNodeNew(n9, 1, 0, tree::NodeStatus::FAILED);

        auto n13 =  tree.addNodeNew(n11, 0, 2, tree::NodeStatus::BRANCH);
        auto n14 =  tree.addNodeNew(n11, 1, 0, tree::NodeStatus::FAILED);

        auto n15 =  tree.addNodeNew(n13, 0, 2, tree::NodeStatus::BRANCH);
        auto n16 =  tree.addNodeNew(n13, 1, 0, tree::NodeStatus::FAILED);

        auto n17 =  tree.addNodeNew(n15, 0, 2, tree::NodeStatus::BRANCH);
        auto n18 =  tree.addNodeNew(n15, 1, 0, tree::NodeStatus::FAILED);

        auto n19 =  tree.addNodeNew(n17, 0, 2, tree::NodeStatus::BRANCH);
        auto n20 =  tree.addNodeNew(n17, 1, 0, tree::NodeStatus::FAILED);

        auto n21 =  tree.addNodeNew(n19, 0, 2, tree::NodeStatus::BRANCH);
        auto n22 =  tree.addNodeNew(n19, 1, 0, tree::NodeStatus::FAILED);

        auto n23 =  tree.addNodeNew(n21, 0, 2, tree::NodeStatus::BRANCH);
        auto n24 =  tree.addNodeNew(n21, 1, 0, tree::NodeStatus::UNDETERMINED);

        auto n25 =  tree.addNodeNew(n23, 0, 2, tree::NodeStatus::BRANCH);
        auto n26 =  tree.addNodeNew(n23, 1, 0, tree::NodeStatus::FAILED);

        auto n27 =  tree.addNodeNew(n25, 0, 2, tree::NodeStatus::BRANCH);
        auto n28 =  tree.addNodeNew(n25, 1, 0, tree::NodeStatus::FAILED);

        auto n29 =  tree.addNodeNew(n27, 0, 2, tree::NodeStatus::BRANCH);
        auto n30 =  tree.addNodeNew(n27, 1, 0, tree::NodeStatus::FAILED);

        auto n31 =  tree.addNodeNew(n29, 0, 2, tree::NodeStatus::BRANCH);
        auto n32 =  tree.addNodeNew(n29, 1, 0, tree::NodeStatus::FAILED);

        auto n33 =  tree.addNodeNew(n31, 0, 2, tree::NodeStatus::BRANCH);
        auto n34 =  tree.addNodeNew(n31, 1, 0, tree::NodeStatus::FAILED);

        auto n35 =  tree.addNodeNew(n33, 0, 0, tree::NodeStatus::FAILED);
        auto n36 =  tree.addNodeNew(n33, 1, 0, tree::NodeStatus::FAILED);

        conductor.showTraditionalView(ex);
    }

    void nary_execution(Conductor& conductor) {

        auto ex = new Execution("n-ary execution");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.createRootNew(4);
        auto n1 =  tree.addNodeNew(root, 0, 2, tree::NodeStatus::BRANCH, "five");
        auto n2 =  tree.addNodeNew(root, 1, 2, tree::NodeStatus::BRANCH, "seven");
        auto n3 =  tree.addNodeNew(root, 2, 0, tree::NodeStatus::FAILED);
        auto n4 =  tree.addNodeNew(root, 3, 2, tree::NodeStatus::BRANCH);

        auto n5 =  tree.addNodeNew(n1, 0, 0, tree::NodeStatus::FAILED);
        auto n6 =  tree.addNodeNew(n1, 1, 0, tree::NodeStatus::FAILED);

        auto n7 =  tree.addNodeNew(n2, 0, 0, tree::NodeStatus::FAILED);
        auto n8 =  tree.addNodeNew(n2, 1, 2, tree::NodeStatus::BRANCH);

        auto n9 =  tree.addNodeNew(n4, 0, 0, tree::NodeStatus::FAILED);
        auto n10 =  tree.addNodeNew(n4, 1, 0, tree::NodeStatus::FAILED);

        auto n11 =  tree.addNodeNew(n8, 0, 0, tree::NodeStatus::FAILED);
        auto n12 =  tree.addNodeNew(n8, 1, 0, tree::NodeStatus::FAILED);
    }

    void larger_nary_execution(Conductor& conductor) {

        auto ex = new Execution("n-ary execution");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.createRootNew(4);
        auto n1 =  tree.addNodeNew(root, 0, 2, tree::NodeStatus::BRANCH);
        auto n2 =  tree.addNodeNew(root, 1, 2, tree::NodeStatus::BRANCH);
        auto n3 =  tree.addNodeNew(root, 2, 0, tree::NodeStatus::FAILED);
        auto n4 =  tree.addNodeNew(root, 3, 2, tree::NodeStatus::BRANCH);

        auto n5 =  tree.addNodeNew(n1, 0, 0, tree::NodeStatus::FAILED);
        auto n6 =  tree.addNodeNew(n1, 1, 4, tree::NodeStatus::BRANCH);

            auto n6a =  tree.addNodeNew(n6, 0, 0, tree::NodeStatus::FAILED);
            auto n6b =  tree.addNodeNew(n6, 1, 0, tree::NodeStatus::FAILED);
            auto n6c =  tree.addNodeNew(n6, 2, 0, tree::NodeStatus::FAILED);
            auto n6d =  tree.addNodeNew(n6, 3, 0, tree::NodeStatus::FAILED);

        auto n7 =  tree.addNodeNew(n2, 0, 0, tree::NodeStatus::FAILED);
        auto n8 =  tree.addNodeNew(n2, 1, 2, tree::NodeStatus::BRANCH);

        auto n9 =  tree.addNodeNew(n4, 0, 0, tree::NodeStatus::FAILED);
        auto n10 =  tree.addNodeNew(n4, 1, 0, tree::NodeStatus::FAILED);

        auto n11 =  tree.addNodeNew(n8, 0, 0, tree::NodeStatus::FAILED);
        auto n12 =  tree.addNodeNew(n8, 1, 0, tree::NodeStatus::FAILED);
    }

    void simple_nary_execution(Conductor& conductor) {

        auto ex = new Execution("n-ary execution");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.createRootNew(4);
        auto n1 =  tree.addNodeNew(root, 0, 0, tree::NodeStatus::FAILED);
        auto n2 =  tree.addNodeNew(root, 1, 0, tree::NodeStatus::FAILED);
        auto n3 =  tree.addNodeNew(root, 2, 0, tree::NodeStatus::FAILED);
        auto n4 =  tree.addNodeNew(root, 3, 0, tree::NodeStatus::FAILED);
    }

    void binary_test_1_for_identical_subtrees(Conductor& conductor) {

        auto ex = new Execution("test for identical subtree algorithm");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.createRootNew(2);
        auto n1 =  tree.addNodeNew(root, 0, 2, tree::NodeStatus::BRANCH, "abcde");
        auto n2 =  tree.addNodeNew(root, 1, 2, tree::NodeStatus::BRANCH, "efghijk");

        auto n3 =  tree.addNodeNew(n1, 0, 2, tree::NodeStatus::BRANCH);
        auto n4 =  tree.addNodeNew(n1, 1, 2, tree::NodeStatus::BRANCH);
        auto n11 =  tree.addNodeNew(n2, 0, 0, tree::NodeStatus::FAILED);
        auto n12 =  tree.addNodeNew(n2, 1, 2, tree::NodeStatus::BRANCH);

        auto n5 =  tree.addNodeNew(n3, 0, 0, tree::NodeStatus::FAILED);
        auto n6 =  tree.addNodeNew(n3, 1, 2, tree::NodeStatus::BRANCH);
        auto n9 =  tree.addNodeNew(n4, 0, 0, tree::NodeStatus::FAILED);
        auto n10 =  tree.addNodeNew(n4, 1, 0, tree::NodeStatus::FAILED);
        auto n13 =  tree.addNodeNew(n12, 0, 0, tree::NodeStatus::FAILED);
        auto n14 =  tree.addNodeNew(n12, 1, 0, tree::NodeStatus::FAILED);

        auto n7 =  tree.addNodeNew(n6, 0, 0, tree::NodeStatus::FAILED);
        auto n8 =  tree.addNodeNew(n6, 1, 0, tree::NodeStatus::FAILED);

    }

    void binary_test_2_for_identical_subtrees(Conductor& conductor) {

        auto ex = new Execution("test for identical subtree algorithm");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.createRootNew(2, "0");
        auto n1 =  tree.addNodeNew(root, 0, 2, tree::NodeStatus::BRANCH, "1");
        auto n2 =  tree.addNodeNew(root, 1, 2, tree::NodeStatus::BRANCH, "2");

        auto n3 =  tree.addNodeNew(n1, 0, 2, tree::NodeStatus::BRANCH, "3");
        auto n4 =  tree.addNodeNew(n1, 1, 2, tree::NodeStatus::BRANCH, "4");
        auto n5 =  tree.addNodeNew(n2, 0, 2, tree::NodeStatus::BRANCH, "5");
        auto n6 =  tree.addNodeNew(n2, 1, 2, tree::NodeStatus::BRANCH, "6");

        auto n7 =  tree.addNodeNew(n3, 0, 0, tree::NodeStatus::FAILED, "7");
        auto n8 =  tree.addNodeNew(n3, 1, 0, tree::NodeStatus::FAILED, "8");
        auto n9 =  tree.addNodeNew(n4, 0, 0, tree::NodeStatus::FAILED, "9");
        auto n10 =  tree.addNodeNew(n4, 1, 2, tree::NodeStatus::BRANCH, "10");
        auto n13 =  tree.addNodeNew(n6, 0, 2, tree::NodeStatus::BRANCH, "13");
        auto n14 =  tree.addNodeNew(n6, 1, 2, tree::NodeStatus::BRANCH, "14");

        auto n15 =  tree.addNodeNew(n10, 0, 0, tree::NodeStatus::FAILED, "15");
        auto n16 =  tree.addNodeNew(n10, 1, 0, tree::NodeStatus::FAILED, "16");

        auto n11 =  tree.addNodeNew(n5, 0, 0, tree::NodeStatus::FAILED, "11");
        auto n12 =  tree.addNodeNew(n5, 1, 0, tree::NodeStatus::FAILED, "12");

        auto n17 =  tree.addNodeNew(n13, 0, 0, tree::NodeStatus::FAILED, "17");
        auto n18 =  tree.addNodeNew(n13, 1, 0, tree::NodeStatus::FAILED, "18");

        auto n19 =  tree.addNodeNew(n14, 0, 0, tree::NodeStatus::FAILED, "19");
        auto n20 =  tree.addNodeNew(n14, 1, 2, tree::NodeStatus::BRANCH, "20");

        auto n21 =  tree.addNodeNew(n20, 0, 0, tree::NodeStatus::FAILED, "21");
        auto n22 =  tree.addNodeNew(n20, 1, 0, tree::NodeStatus::FAILED, "22");

    }

    void build_for_comparison_a(tree::NodeTree& tree) {

        auto root = tree.createRootNew(2, "0");

        auto n1 = tree.addNodeNew(root, 0, 2, tree::NodeStatus::BRANCH, "1");
        auto n2 = tree.addNodeNew(root, 1, 2, tree::NodeStatus::BRANCH, "2");

        auto n3 = tree.addNodeNew(n1, 0, 0, tree::NodeStatus::FAILED, "3");
        auto n4 = tree.addNodeNew(n1, 1, 2, tree::NodeStatus::BRANCH, "4");

        auto n7 = tree.addNodeNew(n4, 0, 0, tree::NodeStatus::FAILED, "7");
        auto n8 = tree.addNodeNew(n4, 1, 0, tree::NodeStatus::FAILED, "8");

        auto n5 = tree.addNodeNew(n2, 0, 0, tree::NodeStatus::SOLVED, "5");
        auto n6 = tree.addNodeNew(n2, 1, 0, tree::NodeStatus::FAILED, "6");
    }

    void build_for_comparison_b(tree::NodeTree& tree) {

        auto root = tree.createRootNew(2, "0");

        auto n1 = tree.addNodeNew(root, 0, 2, tree::NodeStatus::BRANCH, "1");
        auto n2 = tree.addNodeNew(root, 1, 0, tree::NodeStatus::FAILED, "2");

        auto n3 = tree.addNodeNew(n1, 0, 0, tree::NodeStatus::FAILED, "3");
        auto n4 = tree.addNodeNew(n1, 1, 2, tree::NodeStatus::BRANCH, "4");

        auto n7 = tree.addNodeNew(n4, 0, 2, tree::NodeStatus::BRANCH, "7");
        auto n8 = tree.addNodeNew(n4, 1, 0, tree::NodeStatus::FAILED, "8");

        auto n9 = tree.addNodeNew(n7, 0, 0, tree::NodeStatus::FAILED, "9");
        auto n10 = tree.addNodeNew(n7, 1, 0, tree::NodeStatus::FAILED, "10");
    }

    void comparison(Conductor& c) {

        auto ex1 = new Execution("Execution A");
        c.addNewExecution(ex1);
        build_for_comparison_a(ex1->tree());

        auto ex2 = new Execution("Execution B");
        c.addNewExecution(ex2);
        build_for_comparison_b(ex2->tree());

        c.mergeTrees(ex1, ex2);
    }

    void tree_building(Conductor& c) {

        /// create a dummy root node

        auto ex = new Execution("test tree");

        c.addNewExecution(ex);

        auto& tree = ex->tree();

        // auto root = tree.createRootNew(2, "root");
        auto root = tree.createDummyRoot();

        // tree.transformNode(root, 2, tree::NodeStatus::BRANCH);
        auto n1 = tree.addNodeNew(NodeID::NoNode, -1, 0, tree::NodeStatus::FAILED, "1");
    //     auto n2 = tree.addNode(root, 1, 0, tree::NodeStatus::FAILED, "2");

    //     auto n3 = tree.addNode(n1, 0, 0, tree::NodeStatus::FAILED, "3");
    //     auto n4 = tree.addNode(n1, 1, 0, tree::NodeStatus::FAILED, "4");
    }

    void run(Conductor& conductor) {

        // binary_test_1_for_identical_subtrees(conductor);
        // binary_test_2_for_identical_subtrees(conductor);
        binary_tree_execution(conductor);
        // simple_nary_execution(conductor);
        // nary_execution(conductor);
        // larger_nary_execution(conductor);

        // comparison(conductor);


        // tree_building(conductor);

    }

}}}
