#ifndef CPPROFILER_BUILDERTHREAD_HH
#define CPPROFILER_BUILDERTHREAD_HH

#include <QThread>
#include <memory>
#include "execution.hh"

namespace cpprofiler {

class Message;

class BuilderThread : public QThread {
    Q_OBJECT


    Execution* m_execution = nullptr;

    /// What is needed to build a tree

    // - node's parent UID, alt, kids, status



    // What gets added once a node is processed:

    // - its depth
    // - Gecode id (gid)
    // - node stats

    void run() override;

public:
    void startBuilding(Execution* e);

    void handleNode(Message* node);
    void finishBuilding();

    BuilderThread();
    ~BuilderThread();




};



}



#endif