#pragma once

#include <QCommandLineParser>
#include <QString>

namespace cpprofiler
{

namespace cl_options
{
extern QCommandLineOption paths;
extern QCommandLineOption mzn;
extern QCommandLineOption save_search;
} // namespace cl_options

class CommandLineParser
{

    QCommandLineParser cl_parser;

  public:
    CommandLineParser();

    void process(const QCoreApplication &app);

    QString value(const QCommandLineOption &opt);

    bool isSet(const QCommandLineOption &opt);
};

} // namespace cpprofiler