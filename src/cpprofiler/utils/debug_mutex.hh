
#ifndef CPPROFILER_UTILS_DEBUG_MUTEX_HH
#define CPPROFILER_UTILS_DEBUG_MUTEX_HH

#include <QMutex>
#include <QDebug>

namespace cpprofiler {namespace utils {

class DebugMutex : public QMutex {
public:

    DebugMutex() : QMutex() {}

    void lock() {
        // qDebug() << "lock";

        QMutex::lock();
    }

    bool tryLock() {

        // qDebug() << "try lock";
        return QMutex::tryLock();
    }

    void unlock() {
        // qDebug() << "unlock";
        QMutex::unlock();
        
    }

};

class DebugMutexLocker {

    DebugMutex* m_mutex;


public:
    DebugMutexLocker(DebugMutex* m) : m_mutex(m) {
        m_mutex->lock();
    }

    ~DebugMutexLocker() {
        m_mutex->unlock();
    }

};

// using Mutex = QMutex;
// using MutexLocker = QMutexLocker;
using Mutex = utils::DebugMutex;
using MutexLocker = utils::DebugMutexLocker;

}}


#endif