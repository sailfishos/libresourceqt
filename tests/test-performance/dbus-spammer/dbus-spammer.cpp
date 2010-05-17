#include <stdio.h>
#include <signal.h>
#include "dbus-services.h"

sig_atomic_t volatile g_exit = 0;

Maintainer::Maintainer(Threads* t) : QObject()
{
    allOnline = false;
    threads = t;
    timerID = startTimer(0);
}

Maintainer::~Maintainer()
{
    killTimer(timerID);
}

void Maintainer::timerEvent(QTimerEvent*)
{
    if( !allOnline )
    {
        int count = 0;
        Threads::iterator th;
        for( th = threads->begin(); th != threads->end(); ++th )
        {
            DbusSpammer* t = dynamic_cast<DbusSpammer*>(*th);
            if( t )
            {
                if( t->firstTime )
                    count++;
            }
        }

        allOnline = (count == 0) && (threads->count() > 0);

        if( allOnline )
            printf("All thread online and working!\n");
    }

    fflush(stdout);
    fflush(stderr);

    if( g_exit )
        QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
}

class CommandLineParser
{
public:
    CommandLineParser(int argc, char** argv)
    {
        threadCount = 10;
        dataSize = 512;

        parseArguments(argc, argv);
    }

public:
    int threadCount;
    int dataSize;

private:
    void parseArguments(int argc, char** argv)
    {
        int option, tmp;

        while( (option = getopt(argc, argv, "ht:s:")) != -1 )
        {
            switch( option )
            {
            case 'h':
                usage(0);
                return;
            case 't':
                tmp = atoi(optarg);
                if( tmp > 0 )
                {
                    threadCount = tmp;
                    printf("Thread count: %d\n", threadCount);
                }
                else
                {
                    printf("Invalid thread count, using default value %d!\n", threadCount);
                }
                break;
            case 's':
                tmp = atoi(optarg);
                if( tmp > 0 )
                {
                    dataSize = tmp;
                    printf("Data size: %d\n", dataSize);
                }
                else
                {
                    printf("Invalid data size, using default value %d!\n", dataSize);
                }
                break;
            default:
                usage(-1);
                return;
            }
        }
    }

    void usage(int theExitCode)
    {
        printf("usage: dbus-spammer [-h] [-t thread_count] [-s data_size]\n");
        printf("default values:\n");
        printf("  thread_count: %d\n", threadCount);
        printf("  data_size   : %d\n", dataSize);
        exit(theExitCode);
    }
};

void signal_handler(int signo, siginfo_t *info, void *data)
{
    switch( signo )
    {
    case SIGHUP:
    case SIGTERM:
    case SIGINT:
    default:
        puts("Caught exit signal!");
        g_exit = 1;
    }

    (void)info;
    (void)data;
}

bool installSignals()
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;

    if( sigaction(SIGHUP, &sa, NULL) < 0 || sigaction(SIGTERM, &sa, NULL) < 0 || sigaction(SIGINT, &sa, NULL) < 0 )
    {
        puts("Failed to install signal handlers");
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    CommandLineParser cmd(argc, argv);

    int count = cmd.threadCount;

    Threads threads;
    Servers servers;

    if( !installSignals() )
    {
        return -2;
    }

    if( !QDBusConnection::sessionBus().isConnected() )
    {
        puts("Cannot connect to the D-Bus session bus.");
        return -1;
    }

    Maintainer maintainer(&threads);

    printf("Starting pair: ");
    for( int i = 0; i < count; i++ )
    {
        printf("#%d", i);
        fflush(stdout);
        DbusServer* s = new DbusServer(i);
        s->start(QThread::NormalPriority);
        servers.push_back(s);
        usleep(100000);

        QThread* t = new DbusSpammer(i, s->getServiceName(), cmd.dataSize);
        threads.push_back(t);
        t->start(QThread::NormalPriority);
    }

    puts("");

    app.exec();

    Threads::iterator th;
    printf("Cleaning thread: ");
    for( th = threads.begin(); th != threads.end(); ++th )
    {
        DbusSpammer* t = dynamic_cast<DbusSpammer*>(*th);
        if( t )
        {
            printf("#%d", t->threadID);

            t->quit();
            int cnt = 0;
            while( t->isRunning() )
            {
                fflush(stdout);
                fflush(stderr);
                if( ++cnt == 1000 )
                {
                    printf("[T!]");
                    t->terminate();
                    break;
                }
                usleep(1000);
            }
            delete t;
        }
    }
    puts(" ... done!");

    Servers::iterator sv;
    printf("Cleaning service: ");
    for( sv = servers.begin(); sv != servers.end(); ++sv )
    {
        DbusServer* s = dynamic_cast<DbusServer*>(*sv);
        if( s )
        {
            printf("#%d", s->getServiceID());
            s->quit();
            while( !s->isFinished() )
                usleep(1000);

            delete s;
        }
    }

    puts(" ... done!");

    return 0;
}
