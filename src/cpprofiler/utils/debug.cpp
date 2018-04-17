#include "debug.hh"

#include <sstream>

/// ingore everything sent there
static std::ostringstream oss;

namespace cpprofiler {


std::ostream& debug(std::string type) {

    if (type == "memory") {
        return oss;
    } else if (type == "force") {
        return std::cerr;
    } else if (type == "node") {
        return oss;
    } else if (type == "done") {
        return std::cerr;
    } else if (type == "build") {
        return std::cerr;
    } else {
        return oss;
    }
    
}

// std::ostream& no_debug() {
    // return oss;
// }



}