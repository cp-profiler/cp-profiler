#include "structure.hh"

#include <iostream>
#include <exception>

namespace cpprofiler { namespace tree {

    Structure::Structure() {
        std::cerr << "Structure()\n";
        m_nodes.reserve(100);
    }

    void Structure::createRoot() {


        throw invalid_tree();
    }

}}