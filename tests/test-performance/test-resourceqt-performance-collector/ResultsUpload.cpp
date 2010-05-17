/*
 * ResultsUpload.cpp
 *
 *  Created on: May 14, 2010
 *      Author: wlk
 */

#include "ResultsUpload.h"

ResultsUpload::ResultsUpload(QString fileToUpload)
{
    networkManager = new QNetworkAccessManager(this);
    uploadFile = new QFile(fileToUpload);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    uploadFile->open(QIODevice::ReadOnly);

/*    QNetworkAccessManager man;
    QFile* f = new QFile("/usr/lib/libresourceqt-tests/test-performance.ini");
    f->open(QIODevice::ReadOnly);
    man.post(QNetworkRequest(QUrl("http://policy.research.nokia.com/performance/upload.php")), f);*/
    networkManager->post(QNetworkRequest(QUrl("http://policy.research.nokia.com/performance/upload.php")), uploadFile);
}

ResultsUpload::~ResultsUpload()
{
    if( networkManager )
        delete networkManager;

    if( uploadFile )
    {
        uploadFile->close();
        delete uploadFile;
    }
}

void ResultsUpload::replyFinished(QNetworkReply* reply)
{
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
}
