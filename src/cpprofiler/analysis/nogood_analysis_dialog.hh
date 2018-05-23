#pragma once

#include <QDialog>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>

#include <memory>

#include "../core.hh"

namespace cpprofiler
{

namespace analysis
{

struct NgAnalysisItem
{
    NodeID nid;       /// node id of the pentagon
    int left;         /// number of nodes in the left subtree
    int right;        /// number of nodes in the right subtree
    const Nogood &ng; /// nogood associated with the reduction
};

class NogoodAnalysisDialog : public QDialog
{
    Q_OBJECT

  private:
    std::unique_ptr<QStandardItemModel> ng_model_;

    void init()
    {
        static constexpr int DEFAULT_WIDTH = 1000;
        static constexpr int DEFAULT_HEIGHT = 600;

        resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        auto layout = new QVBoxLayout(this);

        auto ng_table = new QTableView();
        layout->addWidget(ng_table);

        ng_model_.reset(new QStandardItemModel(0, 4));

        const QStringList headers{"NodeID", "Left", "Right",
                                  "Clause"};
        ng_model_->setHorizontalHeaderLabels(headers);
        ng_table->horizontalHeader()->setStretchLastSection(true);
        ng_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        ng_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

        ng_table->setModel(ng_model_.get());

        connect(ng_table, &QTableView::doubleClicked, [this](const QModelIndex &idx) {
            const auto nid = ng_model_->item(idx.row())->text().toInt();
            emit nogoodClicked(NodeID(nid));
        });
    }

  public:
    NogoodAnalysisDialog(const std::vector<NgAnalysisItem> &nga_data) : QDialog()
    {
        init();
        populate(nga_data);
    }

    void populate(const std::vector<NgAnalysisItem> &nga_data)
    {
        for (auto &item : nga_data)
        {
            const auto nid_i = new QStandardItem(QString::number(item.nid));
            const auto left_i = new QStandardItem(QString::number(item.left));
            const auto right_i = new QStandardItem(QString::number(item.right));
            const auto ng_i = new QStandardItem(item.ng.get().c_str());
            ng_model_->appendRow({nid_i, left_i, right_i, ng_i});
        }
    }

  signals:
    void nogoodClicked(NodeID nid);
};

} // namespace analysis

} // namespace cpprofiler