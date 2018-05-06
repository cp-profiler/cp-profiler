#include <QObject>

namespace cpprofiler { namespace tree {

class NodeStats : public QObject {
Q_OBJECT
    int branch_ = 0;
    int undet_ = 0;
    int failed_ = 0;
    int solved_ = 0;
    int max_depth_ = 0;

public:

    int branchCount() const { return branch_; }

    int undeterminedCount() const { return undet_; }

    int failedCount() const { return failed_; }

    int solvedCount() const { return solved_; }

    int maxDepth() const { return max_depth_; }

    void add_branch(int n) {
        branch_ = branch_ + n;
        emit stats_changed();
    }

    void subtract_undetermined(int n) {
        // undet_ = undet_ - n;
        emit stats_changed();
    }

    void add_undetermined(int n) {
        // undet_ = undet_ + n;
        emit stats_changed();
    }

    void add_failed(int n) {
        failed_ = failed_ + n;
        emit stats_changed();
    }

    void add_solved(int n) {
        solved_ = solved_ + n;
        emit stats_changed();
    }

    /// see if max depth needs to be updated to d
    void inform_depth(int d) {
        if (d > max_depth_) {
            max_depth_ = d;
            emit stats_changed();
        }
    }

signals:

    void stats_changed();


};


}}