#ifndef _RESULTSUPLOAD_H_
#define _RESULTSUPLOAD_H_

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

class ResultsUpload : public QObject
{
    Q_OBJECT

public:
    ResultsUpload(QString fileToUpload);
    virtual ~ResultsUpload();

private slots:
    void replyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager*  networkManager;
    QFile*                  uploadFile;
};

#endif // _RESULTSUPLOAD_H_
