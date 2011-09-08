/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2011 Nokia Corporation.

This library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation
version 2.1 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
USA.
*************************************************************************/

#ifndef TEST_LOOPING_H
#define TEST_LOOPING_H

#include <QObject>
#include <QList>
#include <QtTest/QTest>
#include <policy/resource-set.h>

class TestLooping: public QObject
{
    Q_OBJECT
private:

    void waitForSignal(const QObject *sender, const char *signal, quint32 timeout = 1000);

public:
    TestLooping();
    ~TestLooping();

private slots:

    void loopAcquireSend();

private:
    // Disabled since they fail
    void loopAcquireReleaseSend();
    void loopAcquireReleaseSend2();
    void loopReleaseSendNoWait();
    void loopReleaseSendWait();
    void loopReleaseSendWait2();
    void loopReleaseSendNoWait2();
};

#endif
