#include <iostream>
#include <thread>

#include <QApplication>

#include "cpprofiler/conductor.hh"

int main(int argc, char *argv[]) {

#ifdef QT_OPENGL_SUPPORT
  QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
#endif

    QApplication app(argc, argv);

    std::cerr << "main thread" << std::this_thread::get_id() << std::endl;

    cpprofiler::Conductor conductor;

    conductor.show();

    return app.exec();
}


/// Threads
// Main thread
// Receiver thread