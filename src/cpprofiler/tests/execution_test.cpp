#include "execution_test.hh"

#include "../conductor.hh"

#include "../execution.hh"
#include "../tree/structure.hh"
#include "../utils/array.hh"

#include <QDebug>
#include <iostream>

namespace cpprofiler { namespace tests { namespace execution {


    void array_test() {


        utils::Array<int> arr();


    }

    void run(Conductor& conductor) {

        auto ex = new Execution("test execution");

        conductor.addNewExecution(ex);

        auto& tree = ex->tree_structure();

        auto nid = tree.createRoot(2);

        auto n1 = tree.addChild(nid, 0, 2);
        auto n2 = tree.addChild(nid, 1, 0);

        auto n3 = tree.addChild(n1, 0, 0);
        auto n4 = tree.addChild(n1, 1, 0);

        auto post_order = tree::helper::postOrder(tree);

        for (auto i : post_order) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;

        auto pre_order = tree::helper::preOrder(tree);

        for (auto i : pre_order) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;

        array_test();

    }

}}}
