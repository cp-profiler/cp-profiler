#include "../tree/node_tree.hh"
#include "../tree/structure.hh"

#include <QDebug>

namespace cpprofiler { namespace tests { namespace tree_test {


        static void binary_tree() {
            tree::NodeTree node_tree;

            auto& tree = node_tree.tree_structure();

            auto nid = tree.createRoot(2);

            auto n1 = tree.addChild(nid, 0, 2);
            auto n2 = tree.addChild(nid, 1, 2);

            qDebug() << "n1: " << n1;
            qDebug() << "n2: " << n2;

            qDebug() << "n1 control: " << tree.getChild(nid, 0);
            qDebug() << "n2 control: " << tree.getChild(nid, 1);

            auto n3 = tree.addChild(n1, 0, 0);
            auto n4 = tree.addChild(n1, 1, 0);

            qDebug() << "n3: " << n3;
            qDebug() << "n4: " << n4;

            try {
                auto n5 = tree.addChild(n2, 0, 0);
                auto n6 = tree.addChild(n2, 0, 0);
                qDebug() << "warning: adding a node in place of an existing one";
            } catch (std::exception& e) {}

            try {
                tree.addChild(n2, 2, 0);
                qDebug() << "bug: must not have room for this child";
            } catch (std::exception& e) {}
        }

        void run() {

            binary_tree();

        }

}}}