#include <iostream>
#include <thread>

#include <QApplication>

#include "cpprofiler/command_line_parser.hh"
#include "cpprofiler/conductor.hh"
#include "cpprofiler/options.hh"

#include "cpprofiler/tests/tree_test.hh"
#include "cpprofiler/tests/execution_test.hh"
#include "cpprofiler/utils/debug.hh"

int main(int argc, char *argv[])
{

    using namespace cpprofiler;

#ifdef QT_OPENGL_SUPPORT
    QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
#endif

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("CP-Profiler");
    debug("thread") << "main thread:" << std::this_thread::get_id() << std::endl;

    CommandLineParser cl_parser;
    cl_parser.process(app);

    Options options;

    {
        if (cl_parser.isSet(cl_options::paths))
        {
            options.paths = cl_parser.value(cl_options::paths).toStdString();
            debug("force") << "selected paths file: " << options.paths << std::endl;
        }

        if (cl_parser.isSet(cl_options::mzn))
        {
            options.mzn = cl_parser.value(cl_options::mzn).toStdString();
            debug("force") << "selected mzn file: " << options.mzn << std::endl;
        }
    }

    if (cl_parser.isSet(cl_options::save_search))
    {
        const auto path = cl_parser.value(cl_options::save_search).toStdString();
        options.save_search_path = path;
    }

    Conductor conductor(std::move(options));

    conductor.show();

    tests::execution::run(conductor);

    return app.exec();
}

/// Threads
// Main thread
// Receiver thread