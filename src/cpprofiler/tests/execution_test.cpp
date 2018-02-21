#include "execution_test.hh"

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

    void run(Conductor& conductor) {

        auto ex = new Execution("test execution");

        conductor.addNewExecution(ex);

        // auto& tree = ex->tree_structure();

        // auto nid = tree.createRoot(2);

        // auto n1 = tree.getChild(nid, 0);
        // auto n2 = tree.getChild(nid, 1);

        // tree.resetNumberOfChildren(n1, 2);


        auto& tree = ex->tree();

        auto root = tree.addNode(tree::NodeID::NoNode, -1, 2, tree::NodeStatus::BRANCH);
        auto n1 =  tree.addNode(root, 0, 2, tree::NodeStatus::BRANCH);
        auto n2 =  tree.addNode(root, 1, 2, tree::NodeStatus::BRANCH);


        conductor.getExecutionWindow(ex).traditional_view().selectNode(n1);

        auto n3 =  tree.addNode(n1, 0, 2, tree::NodeStatus::BRANCH);
        auto n4 =  tree.addNode(n1, 1, 2, tree::NodeStatus::BRANCH);

        auto n5 =  tree.addNode(n3, 0, 2, tree::NodeStatus::BRANCH);
        auto n6 =  tree.addNode(n3, 1, 2, tree::NodeStatus::BRANCH);

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

        // conductor.showTraditionalView(ex);

        // auto n3 = tree.addChild(n1, 0, 0);
        // auto n4 = tree.addChild(n1, 1, 0);

        // auto post_order = tree::helper::postOrder(tree);

        // for (auto i : post_order) {
        //     std::cerr << i << " ";
        // }
        // std::cerr << std::endl;

        // auto pre_order = tree::helper::preOrder(tree);

        // for (auto i : pre_order) {
        //     std::cerr << i << " ";
        // }
        // std::cerr << std::endl;

        array_test();

    }

}}}
