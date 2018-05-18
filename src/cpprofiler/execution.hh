#ifndef CPPROFILER_EXECUTION_HH
#define CPPROFILER_EXECUTION_HH

#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include "tree/node_tree.hh"
#include "tree/node.hh"
#include "user_data.hh"
#include "solver_data.hh"

namespace cpprofiler {


class Execution {
    std::string m_name;

    std::unique_ptr<tree::NodeTree> m_tree;

    std::shared_ptr<SolverData> solver_data_;

    std::shared_ptr<const NameMap> name_map_;

    std::unique_ptr<UserData> user_data_;

    /// Whether the execution contains restarts
    bool m_is_restarts;

public:
    std::string name();

    explicit Execution(const std::string& name, bool restarts = false);

    void setNameMap(std::shared_ptr<const NameMap> nm);

    SolverData& solver_data();
    const SolverData& solver_data() const;

    inline UserData& userData() { return *user_data_; }
    inline const UserData& userData() const { return *user_data_; };

    tree::NodeTree& tree();
    const tree::NodeTree& tree() const;

    bool doesRestarts() const;
};

}



#endif

