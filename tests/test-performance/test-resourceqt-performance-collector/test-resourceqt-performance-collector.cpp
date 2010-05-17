#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QTextStream>
#include "ResultsUpload.h"
#include <stdlib.h>

class Reporter
{
public:
    Reporter(bool create)
    {
        QSettings settings("/usr/lib/libresourceqt-performance-tests/test-resourceqt-performance.ini", QSettings::IniFormat);

        settings.beginGroup("main");
        reportAcquire = settings.value("acqreport", "/tmp/acquire.csv").toString();
        reportRelease = settings.value("relreport", "/tmp/release.csv").toString();
        report = settings.value("report", "/tmp/resourceqt-performance.html").toString();
        reportTemplate = settings.value("template", "/usr/lib/libresourceqt-tests/performance/template.html").toString();
        timeLimit = settings.value("limit", 80.0).toDouble();
        settings.endGroup();

        if( create && QFile::exists(report) )
        {
            QFile::remove(report);
        }

        if( !QFile::exists(report) )
        {
            QFile::copy(reportTemplate, report);
        }
    }

    void updateReport(bool finalize)
    {
        QFile f(report);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QString data(f.readAll().data());
        f.close();

        QString replace, testName("name information missing"), testDesc("test information missing");
        char* env = getenv("TEST_DESC");
        if( env )
            testDesc = env;

        env = getenv("TEST_NAME");
        if( env )
            testName = env;

        QString results;
        QString resAcq, resRel, resHead;

        readReport(reportAcquire, "Acquire", resAcq);
        readReport(reportRelease, "Release", resRel);

        resHead = "<tr><th>Type</th><th>Count</th><th>Limit [ms]</th><th>Average [ms]</th><th>Minimum [ms]</th><th>Maximum [ms]</th><th>Total time [ms]</th>\n";
        results.sprintf("<table border=\"1\" cellpadding=\"6\" cellspacing=\"0\">%s%s%s</table>", qPrintable(resHead), qPrintable(resAcq), qPrintable(resRel));
        replace.sprintf("<h1>Testcase '%s'</h1>\n<p>%s</p>\n<h3>Test results</h3>\n%s\n<br /><br /><br />", qPrintable(testName), qPrintable(testDesc), qPrintable(results));
        if( !finalize )
            replace += "%%TESTRUNINFO%%";

        data.replace("%%TESTRUNINFO%%", replace);

        QFile::remove(report);
        f.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&f);
        out << data;
        f.close();
    }

public:
    QString report;

private:
    QString reportAcquire;
    QString reportRelease;
    QString reportTemplate;
    double  timeLimit;

    bool readReport(QString reportFile, QString name, QString& table)
    {
        QFile f(reportFile);

        if( !f.open(QIODevice::ReadOnly | QIODevice::Text) )
        {
            table.sprintf("<tr><td>%s</td><td>N/A</td><td>%.2f</td><td bgcolor=\"#FF0000\">N/A</td><td>N/A</td><td>N/A</td><td>N/A</td>\n", qPrintable(name), timeLimit);
            return false;
        }

        int tCount = 0;
        double tMin = 0, tMax = 0, tAvg = 0, tTotal = 0;

        for( int i = 0; i < 5; i++ )
        {
            char temp[1024];
            if( f.readLine(temp, 1024) )
            {
                QString qtemp(temp);
                QStringList l = qtemp.split(';');
                QString name = l[0];
                QString val = l[1];

                if( name == "Cnt" )
                    tCount = val.toInt();
                else if( name == "Tot")
                    tTotal = val.toDouble();
                else if( name == "Max" )
                    tMax = val.toDouble();
                else if( name == "Min" )
                    tMin = val.toDouble();
                else if( name == "Avg" )
                    tAvg = val.toDouble();
            }
        }

        QString color = "#00FF00";
        if( tAvg > timeLimit )
            color = "#FF0000";

        table.sprintf("<tr><td>%s</td><td>%d</td><td>%.2f</td><td bgcolor=\"%s\">%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td>\n", qPrintable(name), tCount, timeLimit, qPrintable(color), tAvg, tMin, tMax, tTotal);
        f.close();

        return true;
    }
};

int main(int argc, char** argv)
{
    bool createReport = false, finalizeReport = false;

    if( argc > 1 )
    {
        QString param;
        param = argv[1];
        if( param == "--create" )
            createReport = true;
        else if( param == "--finalize" )
            finalizeReport = true;
    }

    Reporter r(createReport);
    r.updateReport(finalizeReport);

    if( finalizeReport )
    {
        QCoreApplication app(argc, argv);
        ResultsUpload u(r.report);
        app.exec();
    }

    return 0;
}
