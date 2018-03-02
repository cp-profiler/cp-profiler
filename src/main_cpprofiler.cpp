#include <iostream>
#include <thread>

#include <QApplication>

#include "cpprofiler/conductor.hh"

#include "cpprofiler/tests/tree_test.hh"
#include "cpprofiler/tests/execution_test.hh"

int main(int argc, char *argv[]) {

#ifdef QT_OPENGL_SUPPORT
  QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
#endif

    QApplication app(argc, argv);

    cpprofiler::tests::tree_test::run();

    std::cerr << "main thread:" << std::this_thread::get_id() << std::endl;

    cpprofiler::Conductor conductor;

    conductor.show();

    cpprofiler::tests::execution::run(conductor);

    return app.exec();
}


/// Threads
// Main thread
// Receiver thread