#ifndef CPPROFILER_EXECUTION_WINDOW
#define CPPROFILER_EXECUTION_WINDOW

#include <memory>
#include <QMainWindow>

#include "tree/node_id.hh"

namespace cpprofiler
{

namespace tree
{
class TraditionalView;
}

class Execution;

class ExecutionWindow : public QMainWindow
{
    Q_OBJECT

    Execution &execution_;
    std::unique_ptr<tree::TraditionalView> traditional_view_;

  public:
    tree::TraditionalView &traditional_view();

    /// Show a window with all bookmarks
    void showBookmarks() const;

    ExecutionWindow(Execution &ex);
    ~ExecutionWindow();

  public slots:

    /// Remove currently selected node; then select its parent
    void removeSelectedNode();

    void print_log(const std::string &str);

  signals:

    void needToSaveSearch(Execution *e);
};

} // namespace cpprofiler

#endif