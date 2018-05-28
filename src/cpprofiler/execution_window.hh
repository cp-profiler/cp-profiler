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

namespace pixel_tree
{
class Canvas;
}

class Execution;

class ExecutionWindow : public QMainWindow
{
    Q_OBJECT

    Execution &execution_;
    std::unique_ptr<tree::TraditionalView> traditional_view_;

    std::unique_ptr<pixel_tree::Canvas> pixel_tree_;

    /// Dockable widget for the pixel tree
    QDockWidget *pt_dock_ = nullptr;

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

    /// Create and display pixel tree as a dockable widget
    void showPixelTree();

  signals:

    void needToSaveSearch(Execution *e);
};

} // namespace cpprofiler

#endif