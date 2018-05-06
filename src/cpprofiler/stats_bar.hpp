#include <QWidget>
#include <QLabel>
#include <thread>


#include "tree/node_tree.hh"
#include "tree/node_widget.hh"

namespace cpprofiler {

class NodeStatsBar : public QWidget {

  const tree::NodeStats& stats;
  /// Status bar label for maximum depth indicator
  QLabel* depthLabel;
  /// Status bar label for number of solutions
  QLabel* solvedLabel;
  /// Status bar label for number of failures
  QLabel* failedLabel;
  /// Status bar label for number of choices
  QLabel* choicesLabel;
  /// Status bar label for number of open nodes
  QLabel* openLabel;

public:

  NodeStatsBar(QWidget* parent, const tree::NodeStats& ns) : QWidget(parent), stats(ns) {

    using namespace tree;

    QHBoxLayout* hbl = new QHBoxLayout{this};
    hbl->setContentsMargins(2, 1, 2, 1);

    hbl->addWidget(new QLabel("Depth:"));
    depthLabel = new QLabel("0");
    hbl->addWidget(depthLabel);

    solvedLabel = new QLabel("0");
    hbl->addWidget(new NodeWidget(NodeStatus::SOLVED));
    hbl->addWidget(solvedLabel);

    failedLabel = new QLabel("0");
    hbl->addWidget(new NodeWidget(NodeStatus::FAILED));
    hbl->addWidget(failedLabel);


    choicesLabel = new QLabel("0");
    hbl->addWidget(new NodeWidget(NodeStatus::BRANCH));
    hbl->addWidget(choicesLabel);

    // hbl->addWidget(new NodeWidget(UNDETERMINED));
    openLabel = new QLabel("0");
    hbl->addWidget(new NodeWidget(NodeStatus::UNDETERMINED));
    hbl->addWidget(openLabel);
  }

public slots:

  void update() {

      static bool done = false;

      if (!done) {
          debug("thread") << "StatsBar::update thread:" << std::this_thread::get_id() << std::endl;
          done = true;
      }

      depthLabel->setNum(stats.maxDepth());
      openLabel->setNum(stats.undeterminedCount());
      solvedLabel->setNum(stats.solvedCount());
      failedLabel->setNum(stats.failedCount());
      choicesLabel->setNum(stats.branchCount());

  }

//   void display(const Statistics& stats) {
//     depthLabel->setNum(stats.maxDepth);
//     solvedLabel->setNum(stats.solutions);
//     failedLabel->setNum(stats.failures);
//     choicesLabel->setNum(stats.choices);
//     openLabel->setNum(stats.undetermined);
//   }
};


}