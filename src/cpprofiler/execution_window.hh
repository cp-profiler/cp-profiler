#ifndef CPPROFILER_EXECUTION_WINDOW
#define CPPROFILER_EXECUTION_WINDOW

#include <memory>
#include <QMainWindow>
#include <QSlider>

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

class LanternMenu : public QWidget
{
  Q_OBJECT

private:
  QSlider *slider_;

public:
  LanternMenu();

  /// Get slider value
  int value() { return slider_->value(); }

signals:
  /// indicates that max size for a lantern node changed
  void limit_changed(int val);
};

class ExecutionWindow : public QMainWindow
{
  Q_OBJECT

  Execution &execution_;
  std::unique_ptr<tree::TraditionalView> traditional_view_;

  std::unique_ptr<pixel_tree::Canvas> pixel_tree_;

  /// Dockable widget for the pixel tree
  QDockWidget *pt_dock_ = nullptr;

  /// Settings widget for lantern tree
  LanternMenu *lantern_widget = nullptr;

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

  /// Toggle the tree lantern tree version of the visualisation
  void toggleLanternView(bool checked);

signals:

  void needToSaveSearch(Execution *e);
};

} // namespace cpprofiler

#endif