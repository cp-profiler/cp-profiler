#include "../tree/node_tree.hh"
#include "../tree/structure.hh"

#include "../utils/array.hh"

#include <QDebug>

namespace cpprofiler { namespace tests { namespace tree_test {


        static void binary_tree() {
            tree::NodeTree nt;

            auto nid = nt.createRoot_safe(3);

            auto n1 = nt.addChild_safe(nid, 0, 2);
            auto n2 = nt.addChild_safe(nid, 1, 2);
            auto n11 = nt.addChild_safe(nid, 2, 2);

            qDebug() << "n1: " << n1;
            qDebug() << "n2: " << n2;
            qDebug() << "n11: " << n11;

            qDebug() << "n1 control: " << nt.getChild_safe(nid, 0);
            qDebug() << "n2 control: " << nt.getChild_safe(nid, 1);

            auto n3 = nt.addChild_safe(n1, 0, 0);
            auto n4 = nt.addChild_safe(n1, 1, 0);

            qDebug() << "n3: " << n3;
            qDebug() << "n4: " << n4;

            try {
                auto n5 = nt.addChild_safe(n2, 0, 0);
                auto n6 = nt.addChild_safe(n2, 0, 0);
                qDebug() << "warning: adding a node in place of an existing one";
            } catch (std::exception& e) {}

            try {
                nt.addChild_safe(n2, 2, 0);
                qDebug() << "bug: must not have room for this child";
            } catch (std::exception& e) {}

            qDebug() << "siblings: " << nt.getNumberOfSiblings_safe(n1);
            qDebug() << "siblings: " << nt.getNumberOfSiblings_safe(n4);
        }


        class TestClass {
        private:
            int m_id;
            static int counter;
        public:
            TestClass() {
                counter++;
                m_id = counter;
                qDebug() << "TestClass:" << m_id;
            }

            ~TestClass() {
                qDebug() << "~TestClass" << m_id;
            }

            TestClass(const TestClass& other) {
                counter++;
                m_id = counter;
                qDebug() << "copy TestClass";
            }

            TestClass(TestClass&& other) {
                m_id = other.m_id;
                qDebug() << "move constructred TestClass";
            }

            TestClass& operator=(const TestClass& other)  {
                m_id = other.m_id;
                qDebug() << "copy assigning TestClass";
            }
            
        };

        int TestClass::counter = 0;

        void array_usage() {

            utils::Array<TestClass> arr(2);

            arr[0] = TestClass{};
            arr[1] = TestClass{};

            // auto other = arr;

            // TestClass a;
            // arr[0] = a;

        }

        void run() {

            // binary_tree();

            array_usage();

        }

}}}