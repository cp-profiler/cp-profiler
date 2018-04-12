#include "../tree/node_tree.hh"
#include "../tree/structure.hh"

#include "../utils/array.hh"
#include "../utils/debug.hh"

namespace cpprofiler { namespace tests { namespace tree_test {


        static void binary_tree() {
            tree::NodeTree nt;

            auto nid = nt.createRoot(3);

            auto n1 = nt.addChild(nid, 0, 2);
            auto n2 = nt.addChild(nid, 1, 2);
            auto n11 = nt.addChild(nid, 2, 2);

            debug() << "n1: " << n1;
            debug() << "n2: " << n2;
            debug() << "n11: " << n11;

            debug() << "n1 control: " << nt.getChild(nid, 0);
            debug() << "n2 control: " << nt.getChild(nid, 1);

            auto n3 = nt.addChild(n1, 0, 0);
            auto n4 = nt.addChild(n1, 1, 0);

            debug() << "n3: " << n3;
            debug() << "n4: " << n4;

            try {
                auto n5 = nt.addChild(n2, 0, 0);
                auto n6 = nt.addChild(n2, 0, 0);
                debug() << "warning: adding a node in place of an existing one";
            } catch (std::exception& e) {}

            try {
                nt.addChild(n2, 2, 0);
                debug() << "bug: must not have room for this child";
            } catch (std::exception& e) {}

            debug() << "siblings: " << nt.getNumberOfSiblings(n1);
            debug() << "siblings: " << nt.getNumberOfSiblings(n4);
        }


        class TestClass {
        private:
            int m_id;
            static int counter;
        public:
            TestClass() {
                counter++;
                m_id = counter;
                debug("memory") << "TestClass:" << m_id;
            }

            ~TestClass() {
                debug("memory") << "~TestClass" << m_id;
            }

            TestClass(const TestClass& other) {
                counter++;
                m_id = counter;
                debug("memory") << "copy TestClass";
            }

            TestClass(TestClass&& other) {
                m_id = other.m_id;
                debug("memory") << "move constructred TestClass";
            }

            TestClass& operator=(const TestClass& other)  {
                m_id = other.m_id;
                debug("memory") << "copy assigning TestClass";
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