
#include "command_line_parser.hh"

namespace cpprofiler {


namespace cl_options {
    QCommandLineOption paths{"paths", "Use symbol table from: <file_name>.", "file_name"};
    QCommandLineOption mzn{"mzn", "Use MiniZinc file for tying ids to expressions: <file_name>.", "file_name"};
}

    CommandLineParser::CommandLineParser() {

        cl_parser.addHelpOption();
        cl_parser.addOption(cl_options::paths);
        cl_parser.addOption(cl_options::mzn);

    }

    void CommandLineParser::process(const QCoreApplication& app) {
        cl_parser.process(app);
    }

    QString CommandLineParser::value(const QCommandLineOption& opt) {
        return cl_parser.value(opt);
    }

    bool CommandLineParser::isSet(const QCommandLineOption& opt) {
      return cl_parser.isSet(opt);
    }
}
