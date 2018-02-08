#include "structure.hh"

#include <iostream>
#include <exception>

namespace cpprofiler { namespace tree {

    Structure::Structure() {
        std::cerr << "Structure()\n";
        m_nodes.reserve(100);


    }

    NodeID Structure::createRoot(int kids) {

        if (m_nodes.size() > 0) {
            throw invalid_tree();
        }

        m_nodes.push_back(std::unique_ptr<Node>{new Node(kids)});

        return NodeID{0};

    }

    NodeID Structure::addChild(NodeID pid, int alt, int kids) {

        auto nid = NodeID{m_nodes.size()};
        m_nodes.push_back(std::unique_ptr<Node>{new Node(kids)});

        auto p_node = m_nodes[pid].get();
        p_node->setChild(nid, alt);

        return nid;


    }

    NodeID Structure::getChild(NodeID pid, int alt) {
        return m_nodes[pid]->getChild(alt);
    }

}}