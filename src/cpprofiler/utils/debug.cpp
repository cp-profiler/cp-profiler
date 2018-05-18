#include "debug.hh"

#include <vector>
#include <string>
#include <algorithm>

/// ingore everything sent there
static std::ostringstream oss;

namespace cpprofiler {

std::ostream& debug(std::string type) {

    static std::vector<std::string> to_print = {
        ""
        ,"force"
        ,"done"
        ,"error"
        ,"perf"
        // ,"layout"
        ,"msg:command"
        ,"msg:node"
    };

    if (std::find(to_print.begin(), to_print.end(), type) != to_print.end() ) {
        return std::cerr;
    } else {
        oss.seekp(0);
        return oss;
    }
}

std::ostream& operator<<(std::ostream& os, const QString& str) {
    return os << str.toStdString();
}



}