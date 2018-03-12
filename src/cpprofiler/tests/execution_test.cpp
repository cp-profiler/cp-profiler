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



        auto root = tree.addNode(tree::NodeID::NoNode, -1, 2, tree::NodeStatus::BRANCH, "0");
        auto n1 =  tree.addNode(root, 0, 2, tree::NodeStatus::BRANCH, "1");
        auto n2 =  tree.addNode(root, 1, 2, tree::NodeStatus::BRANCH, "2");

        conductor.getExecutionWindow(ex).traditional_view().selectNode(n1);

        auto n3 =  tree.addNode(n1, 0, 2, tree::NodeStatus::BRANCH, "3");
        auto n4 =  tree.addNode(n1, 1, 2, tree::NodeStatus::BRANCH, "4");

        auto n5 =  tree.addNode(n3, 0, 2, tree::NodeStatus::BRANCH, "7");
        auto n6 =  tree.addNode(n3, 1, 2, tree::NodeStatus::BRANCH, "8");

        tree.addNode(n5, 0, 0, tree::NodeStatus::SOLVED);
        tree.addNode(n5, 1, 0, tree::NodeStatus::FAILED);

        tree.addNode(n6, 0, 0, tree::NodeStatus::FAILED);
        tree.addNode(n6, 1, 0, tree::NodeStatus::SKIPPED);

        auto n7 =  tree.addNode(n4, 0, 0, tree::NodeStatus::FAILED);
        auto n8 =  tree.addNode(n4, 1, 0, tree::NodeStatus::FAILED);

        auto n9 =  tree.addNode(n2, 0, 2, tree::NodeStatus::BRANCH);
        auto n10 =  tree.addNode(n2, 1, 0, tree::NodeStatus::FAILED);

        auto n11 =  tree.addNode(n9, 0, 2, tree::NodeStatus::BRANCH);
        auto n12 =  tree.addNode(n9, 1, 0, tree::NodeStatus::FAILED);

        auto n13 =  tree.addNode(n11, 0, 2, tree::NodeStatus::BRANCH);
        auto n14 =  tree.addNode(n11, 1, 0, tree::NodeStatus::FAILED);

        auto n15 =  tree.addNode(n13, 0, 2, tree::NodeStatus::BRANCH);
        auto n16 =  tree.addNode(n13, 1, 0, tree::NodeStatus::FAILED);

        auto n17 =  tree.addNode(n15, 0, 2, tree::NodeStatus::BRANCH);
        auto n18 =  tree.addNode(n15, 1, 0, tree::NodeStatus::FAILED);

        auto n19 =  tree.addNode(n17, 0, 2, tree::NodeStatus::BRANCH);
        auto n20 =  tree.addNode(n17, 1, 0, tree::NodeStatus::FAILED);

        auto n21 =  tree.addNode(n19, 0, 2, tree::NodeStatus::BRANCH);
        auto n22 =  tree.addNode(n19, 1, 0, tree::NodeStatus::FAILED);

        auto n23 =  tree.addNode(n21, 0, 2, tree::NodeStatus::BRANCH);
        auto n24 =  tree.addNode(n21, 1, 0, tree::NodeStatus::FAILED);

        auto n25 =  tree.addNode(n23, 0, 2, tree::NodeStatus::BRANCH);
        auto n26 =  tree.addNode(n23, 1, 0, tree::NodeStatus::FAILED);

        auto n27 =  tree.addNode(n25, 0, 2, tree::NodeStatus::BRANCH);
        auto n28 =  tree.addNode(n25, 1, 0, tree::NodeStatus::FAILED);

        auto n29 =  tree.addNode(n27, 0, 2, tree::NodeStatus::BRANCH);
        auto n30 =  tree.addNode(n27, 1, 0, tree::NodeStatus::FAILED);

        auto n31 =  tree.addNode(n29, 0, 2, tree::NodeStatus::BRANCH);
        auto n32 =  tree.addNode(n29, 1, 0, tree::NodeStatus::FAILED);

        conductor.showTraditionalView(ex);
    }

    void nary_execution(Conductor& conductor) {

        auto ex = new Execution("n-ary execution");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.addNode(tree::NodeID::NoNode, -1, 4, tree::NodeStatus::BRANCH);
        auto n1 =  tree.addNode(root, 0, 2, tree::NodeStatus::BRANCH, "five");
        auto n2 =  tree.addNode(root, 1, 2, tree::NodeStatus::BRANCH, "seven");
        auto n3 =  tree.addNode(root, 2, 0, tree::NodeStatus::FAILED);
        auto n4 =  tree.addNode(root, 3, 2, tree::NodeStatus::BRANCH);

        auto n5 =  tree.addNode(n1, 0, 0, tree::NodeStatus::FAILED);
        auto n6 =  tree.addNode(n1, 1, 0, tree::NodeStatus::FAILED);

        auto n7 =  tree.addNode(n2, 0, 0, tree::NodeStatus::FAILED);
        auto n8 =  tree.addNode(n2, 1, 2, tree::NodeStatus::BRANCH);

        auto n9 =  tree.addNode(n4, 0, 0, tree::NodeStatus::FAILED);
        auto n10 =  tree.addNode(n4, 1, 0, tree::NodeStatus::FAILED);

        auto n11 =  tree.addNode(n8, 0, 0, tree::NodeStatus::FAILED);
        auto n12 =  tree.addNode(n8, 1, 0, tree::NodeStatus::FAILED);
    }

    void larger_nary_execution(Conductor& conductor) {

        auto ex = new Execution("n-ary execution");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.addNode(tree::NodeID::NoNode, -1, 4, tree::NodeStatus::BRANCH);
        auto n1 =  tree.addNode(root, 0, 2, tree::NodeStatus::BRANCH);
        auto n2 =  tree.addNode(root, 1, 2, tree::NodeStatus::BRANCH);
        auto n3 =  tree.addNode(root, 2, 0, tree::NodeStatus::FAILED);
        auto n4 =  tree.addNode(root, 3, 2, tree::NodeStatus::BRANCH);

        auto n5 =  tree.addNode(n1, 0, 0, tree::NodeStatus::FAILED);
        auto n6 =  tree.addNode(n1, 1, 4, tree::NodeStatus::BRANCH);

            auto n6a =  tree.addNode(n6, 0, 0, tree::NodeStatus::FAILED);
            auto n6b =  tree.addNode(n6, 1, 0, tree::NodeStatus::FAILED);
            auto n6c =  tree.addNode(n6, 2, 0, tree::NodeStatus::FAILED);
            auto n6d =  tree.addNode(n6, 3, 0, tree::NodeStatus::FAILED);

        auto n7 =  tree.addNode(n2, 0, 0, tree::NodeStatus::FAILED);
        auto n8 =  tree.addNode(n2, 1, 2, tree::NodeStatus::BRANCH);

        auto n9 =  tree.addNode(n4, 0, 0, tree::NodeStatus::FAILED);
        auto n10 =  tree.addNode(n4, 1, 0, tree::NodeStatus::FAILED);

        auto n11 =  tree.addNode(n8, 0, 0, tree::NodeStatus::FAILED);
        auto n12 =  tree.addNode(n8, 1, 0, tree::NodeStatus::FAILED);
    }

    void simple_nary_execution(Conductor& conductor) {

        auto ex = new Execution("n-ary execution");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.addNode(tree::NodeID::NoNode, -1, 4, tree::NodeStatus::BRANCH);
        auto n1 =  tree.addNode(root, 0, 0, tree::NodeStatus::FAILED);
        auto n2 =  tree.addNode(root, 1, 0, tree::NodeStatus::FAILED);
        auto n3 =  tree.addNode(root, 2, 0, tree::NodeStatus::FAILED);
        auto n4 =  tree.addNode(root, 3, 0, tree::NodeStatus::FAILED);
    }

    void binary_test_1_for_identical_subtrees(Conductor& conductor) {

        auto ex = new Execution("test for identical subtree algorithm");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.addNode(tree::NodeID::NoNode, -1, 2, tree::NodeStatus::BRANCH);
        auto n1 =  tree.addNode(root, 0, 2, tree::NodeStatus::BRANCH, "abcde");
        auto n2 =  tree.addNode(root, 1, 2, tree::NodeStatus::BRANCH, "efghijk");

        auto n3 =  tree.addNode(n1, 0, 2, tree::NodeStatus::BRANCH);
        auto n4 =  tree.addNode(n1, 1, 2, tree::NodeStatus::BRANCH);
        auto n11 =  tree.addNode(n2, 0, 0, tree::NodeStatus::FAILED);
        auto n12 =  tree.addNode(n2, 1, 2, tree::NodeStatus::BRANCH);

        auto n5 =  tree.addNode(n3, 0, 0, tree::NodeStatus::FAILED);
        auto n6 =  tree.addNode(n3, 1, 2, tree::NodeStatus::BRANCH);
        auto n9 =  tree.addNode(n4, 0, 0, tree::NodeStatus::FAILED);
        auto n10 =  tree.addNode(n4, 1, 0, tree::NodeStatus::FAILED);
        auto n13 =  tree.addNode(n12, 0, 0, tree::NodeStatus::FAILED);
        auto n14 =  tree.addNode(n12, 1, 0, tree::NodeStatus::FAILED);

        auto n7 =  tree.addNode(n6, 0, 0, tree::NodeStatus::FAILED);
        auto n8 =  tree.addNode(n6, 1, 0, tree::NodeStatus::FAILED);

    }

    void binary_test_2_for_identical_subtrees(Conductor& conductor) {

        auto ex = new Execution("test for identical subtree algorithm");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree();

        auto root = tree.addNode(tree::NodeID::NoNode, -1, 2, tree::NodeStatus::BRANCH, "0");
        auto n1 =  tree.addNode(root, 0, 2, tree::NodeStatus::BRANCH, "1");
        auto n2 =  tree.addNode(root, 1, 2, tree::NodeStatus::BRANCH, "2");

        auto n3 =  tree.addNode(n1, 0, 2, tree::NodeStatus::BRANCH, "3");
        auto n4 =  tree.addNode(n1, 1, 2, tree::NodeStatus::BRANCH, "4");
        auto n5 =  tree.addNode(n2, 0, 2, tree::NodeStatus::BRANCH, "5");
        auto n6 =  tree.addNode(n2, 1, 2, tree::NodeStatus::BRANCH, "6");

        auto n7 =  tree.addNode(n3, 0, 0, tree::NodeStatus::FAILED, "7");
        auto n8 =  tree.addNode(n3, 1, 0, tree::NodeStatus::FAILED, "8");
        auto n9 =  tree.addNode(n4, 0, 0, tree::NodeStatus::FAILED, "9");
        auto n10 =  tree.addNode(n4, 1, 2, tree::NodeStatus::BRANCH, "10");
        auto n13 =  tree.addNode(n6, 0, 2, tree::NodeStatus::BRANCH, "13");
        auto n14 =  tree.addNode(n6, 1, 2, tree::NodeStatus::BRANCH, "14");

        auto n15 =  tree.addNode(n10, 0, 0, tree::NodeStatus::FAILED, "15");
        auto n16 =  tree.addNode(n10, 1, 0, tree::NodeStatus::FAILED, "16");

        auto n11 =  tree.addNode(n5, 0, 0, tree::NodeStatus::FAILED, "11");
        auto n12 =  tree.addNode(n5, 1, 0, tree::NodeStatus::FAILED, "12");

        auto n17 =  tree.addNode(n13, 0, 0, tree::NodeStatus::FAILED, "17");
        auto n18 =  tree.addNode(n13, 1, 0, tree::NodeStatus::FAILED, "18");

        auto n19 =  tree.addNode(n14, 0, 0, tree::NodeStatus::FAILED, "19");
        auto n20 =  tree.addNode(n14, 1, 2, tree::NodeStatus::BRANCH, "20");

        auto n21 =  tree.addNode(n20, 0, 0, tree::NodeStatus::FAILED, "21");
        auto n22 =  tree.addNode(n20, 1, 0, tree::NodeStatus::FAILED, "22");

    }

    void run(Conductor& conductor) {

        // binary_test_1_for_identical_subtrees(conductor);
        binary_test_2_for_identical_subtrees(conductor);
        // binary_tree_execution(conductor);
        // simple_nary_execution(conductor);
        // nary_execution(conductor);
        // larger_nary_execution(conductor);

    }

}}}
