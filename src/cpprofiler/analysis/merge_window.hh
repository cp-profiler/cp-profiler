#pragma once

#include <QMainWindow>
#include "../tree/node_tree.hh"


namespace cpprofiler { namespace tree {
    class TraditionalView;
}}

namespace cpprofiler { namespace analysis {


class MergeWindow : public QMainWindow {
Q_OBJECT

    tree::NodeTree m_nt;

    std::unique_ptr<tree::TraditionalView> m_view;

public:
    MergeWindow();
    ~MergeWindow();


    tree::NodeTree& getTree();

};


}}