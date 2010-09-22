/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2010 Nokia Corporation.

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



#ifndef TEST_RESOURCE_ENGINE_H
#define TEST_RESOURCE_ENGINE_H

#include <QtTest/QTest>
#include <QMetaType>

class TestResourceEngine;

#include "resource-engine.h"

Q_DECLARE_METATYPE(ResourcePolicy::ResourceType)

class TestResourceEngine: public QObject
{
    Q_OBJECT
private:
    ResourcePolicy::ResourceEngine *resourceEngine;
    ResourcePolicy::AudioResource *audioPlayback;
    ResourcePolicy::VideoResource *videoPlayback;
    ResourcePolicy::AudioRecorderResource *audioRecorder;
    ResourcePolicy::VideoRecorderResource *videoRecorder;
    bool libresourceInitialized;
    bool acquireOrDenyWasCalled;

public:
    ResourcePolicy::ResourceSet *resourceSet;

    TestResourceEngine();
    ~TestResourceEngine();
public slots:
    void connectedHandler();
    void handleDeny();
    void handleAcquire(quint32 bitmaskOfResources);

private slots:
    void init();
    void cleanup();

    void testConnect();
    void testDisconnect();

    void testStatusMessage();

    void testAcquire_data();
    void testAcquire();

    void testRelease_data();
    void testRelease();

    void testRegisterAudioProperties_data();
    void testRegisterAudioProperties();

    void testMultipleInstences();
};

#endif

