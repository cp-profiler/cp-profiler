#include "debug.hh"

#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

/// ingore everything sent there
static std::ostringstream oss;

namespace cpprofiler {

std::ostream& debug(std::string type) {

    static std::vector<std::string> to_print = {"force", "done", "perf"};

    if (std::find(to_print.begin(), to_print.end(), type) != to_print.end() ) {
        return std::cerr;
    } else {
        return oss;
    }
}

std::ostream& operator<<(std::ostream& os, const QString& str) {
    return os << str.toStdString();
}



}