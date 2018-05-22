#pragma once

#include <QObject>

namespace cpprofiler
{

class Message;
class Execution;

class TreeBuilder : public QObject
{
    Q_OBJECT

    Execution &m_execution;

  public:
    TreeBuilder(Execution &ex);

    void startBuilding();

    void finishBuilding();

    void handleNode(Message *node);

  signals:

    void buildingDone();
};

} // namespace cpprofiler