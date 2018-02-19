#include "execution_test.hh"

#include "../conductor.hh"
#include "../user_data.hh"

#include "../execution.hh"
#include "../tree/structure.hh"
#include "../utils/array.hh"
#include "../tree/node_info.hh"

#include <QDebug>
#include <iostream>

namespace cpprofiler { namespace tests { namespace execution {

    class Test {

public:
        Test() {
            qDebug() << "Test";
        }

        ~Test() {
            qDebug() << "~Test";
        }
    };

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

        ex->user_data().setSelectedNode(n1);

        auto n3 =  tree.addNode(n1, 0, 2, tree::NodeStatus::BRANCH);
        auto n4 =  tree.addNode(n1, 1, 2, tree::NodeStatus::BRANCH);

        auto n5 =  tree.addNode(n3, 0, 2, tree::NodeStatus::BRANCH);
        auto n6 =  tree.addNode(n3, 1, 2, tree::NodeStatus::BRANCH);

        auto n7 =  tree.addNode(n4, 0, 0, tree::NodeStatus::FAILED);
        auto n8 =  tree.addNode(n4, 1, 0, tree::NodeStatus::FAILED);


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
