#pragma once

#include <QMainWindow>
#include "../tree/node_tree.hh"
#include "merging/merge_result.hh"


namespace cpprofiler { namespace tree {
    class TraditionalView;
}}

namespace cpprofiler { namespace analysis {

class PentagonCounter;
class PentagonListWidget;

class MergeWindow : public QMainWindow {
Q_OBJECT

    tree::NodeTree m_nt;

    std::unique_ptr<tree::TraditionalView> m_view;

    MergeResult m_merge_result;

    PentagonCounter* pentagon_bar;

    PentagonListWidget* pent_list;

public:
    MergeWindow();
    ~MergeWindow();


    tree::NodeTree& getTree();

    MergeResult& mergeResult();

public slots:

    /// update status bar to show the final result (petagons)
    void finalize();

};


}}