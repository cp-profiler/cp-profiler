#include <QWidget>
#include <QLabel>


#include "tree/node_tree.hh"

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
    QHBoxLayout* hbl = new QHBoxLayout{this};
    hbl->setContentsMargins(0, 0, 0, 0);

    hbl->addWidget(new QLabel("Depth:"));
    depthLabel = new QLabel("0");
    hbl->addWidget(depthLabel);

    // hbl->addWidget(new NodeWidget(SOLVED));
    solvedLabel = new QLabel("0");
    hbl->addWidget(solvedLabel);

    // hbl->addWidget(new NodeWidget(FAILED));
    failedLabel = new QLabel("0");
    hbl->addWidget(failedLabel);

    // hbl->addWidget(new NodeWidget(BRANCH));
    choicesLabel = new QLabel("0");
    hbl->addWidget(choicesLabel);

    // hbl->addWidget(new NodeWidget(UNDETERMINED));
    openLabel = new QLabel("0");
    hbl->addWidget(openLabel);
  }

public slots:

  void update() {

      openLabel->setNum(stats.undetermined_count());
      solvedLabel->setNum(stats.solved_count());
      failedLabel->setNum(stats.failed_count());
      choicesLabel->setNum(stats.branch_count());

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