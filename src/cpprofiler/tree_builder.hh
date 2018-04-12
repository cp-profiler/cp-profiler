#pragma once

#include <QObject>

namespace cpprofiler {

class Message;
class Execution;

class TreeBuilder : public QObject{
    Q_OBJECT

    Execution* m_execution = nullptr;

public:
    void startBuilding(Execution* e);

    void finishBuilding();

    void handleNode(Message* node);



};



}