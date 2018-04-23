#include "string_utils.hh"

#include <sstream>
#include <iterator>

using std::vector;
using std::string;

namespace cpprofiler { namespace utils {

    vector<string> split(const string& str, char delim,
                         bool include_empty) {

      std::stringstream ss;
      ss.str(str);
      std::string item;

      vector<string> result;

      auto inserter = std::back_inserter(result);

      while (std::getline(ss, item, delim)) {
        if(!item.empty() || include_empty)
          *(inserter++) = item;
      }

      return result;
    }


}}