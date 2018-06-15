#ifndef CPPROFILER_GLOBAL_HH
#define CPPROFILER_GLOBAL_HH

#include <ostream>
#include "tree/node_id.hh"
#include "utils/debug_mutex.hh"
#include "utils/std_ext.hh"
#include "utils/debug.hh"

using cpprofiler::tree::NodeID;
using Label = std::string;
using ExecID = int;

namespace cpprofiler
{
namespace tree
{

enum class NodeStatus
{
    SOLVED = 0,
    FAILED = 1,
    BRANCH = 2,
    SKIPPED = 3,
    UNDETERMINED = 4,
    MERGED = 5
};

std::ostream &operator<<(std::ostream &os, const NodeStatus &ns);

} // namespace tree
} // namespace cpprofiler

namespace cpprofiler
{

class Nogood
{
    std::string orig_ng_;

  public:
    Nogood(const std::string &text) : orig_ng_(text) {}

    const std::string get() const { return orig_ng_; }

    static const Nogood empty;
};
} // namespace cpprofiler

#endif