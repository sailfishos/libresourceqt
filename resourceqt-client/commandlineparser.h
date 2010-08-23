#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H
#include <QString>
#include <QList>
#include <QSet>
#include <QHash>
#include <QTextStream>
#include <policy/resource.h>

class CommandLineParser
{
public:
    CommandLineParser();
    ~CommandLineParser();

public:
    bool parseArguments();

    static bool parseResourceList(const QString &resourceListStr,
                           QSet<ResourcePolicy::ResourceType> &resourceList);

    const QSet<ResourcePolicy::ResourceType>& resources() const;
    const QSet<ResourcePolicy::ResourceType>& optionalResources() const;
    QString resourceApplicationClass() const;
    bool shouldAutoRelease() const;
    bool shouldAlwaysReply() const;
    bool shouldBeVerbose() const;

private:
    QSet<ResourcePolicy::ResourceType> allResources;
    QSet<ResourcePolicy::ResourceType> optResources;
    static QHash<QString, ResourcePolicy::ResourceType> resourceValues;
    QString applicationClass;
    bool autoRelease;
    bool alwaysReply;
    bool verbose;
    bool allowUnkownResourceClass;
    QTextStream output;

    bool parseClassString(const QString &str);
    bool parseModeValues(const QString &modeListStr);

    void usage();

};
#endif

