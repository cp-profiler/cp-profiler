#include <iostream>
#include <thread>

#include <QApplication>

#include "cpprofiler/conductor.hh"

#include "cpprofiler/tests/tree_test.hh"

int main(int argc, char *argv[]) {

#ifdef QT_OPENGL_SUPPORT
  QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
#endif

    QApplication app(argc, argv);

    cpprofiler::tests::run();

    cpprofiler::Conductor conductor;

    conductor.show();

    return app.exec();
}


/// Threads
// Main thread
// Receiver thread