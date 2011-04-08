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

#ifndef TEST_RESOURCE_SET_H
#define TEST_RESOURCE_SET_H

#include <QObject>
#include <QList>
#include <QtTest/QTest>
#include <policy/resource-set.h>

class TestAcquire: public QObject
{
    Q_OBJECT
private:

    ResourcePolicy::AudioResource *audioResource;
    ResourcePolicy::AudioRecorderResource *audioRecorderResource;
    ResourcePolicy::Resource *videoResource;
    ResourcePolicy::Resource *videoRecorderResource;
    ResourcePolicy::Resource *vibraResource;
    ResourcePolicy::Resource *ledsResource;
    ResourcePolicy::Resource *backlightResource;
    ResourcePolicy::Resource *systemButtonResource;
    ResourcePolicy::Resource *lockButtonResource;
    ResourcePolicy::Resource *scaleButtonResource;
    ResourcePolicy::Resource *snapButtonResource;
    ResourcePolicy::Resource *lensCoverResource;
    ResourcePolicy::Resource *headsetButtonsResource;

    ResourcePolicy::Resource * resourceFromType(ResourcePolicy::ResourceType type);

    void waitForSignal(const QObject *sender, const char *signal, quint32 timeout = 500);

public:
    TestAcquire();
    ~TestAcquire();

private slots:

    void testAcquire();
    void testAcquiringAndDenyingResource();
    void testAcquiringAndDenyingResource2();

private:
    // Disabled tests - move to under 'private slots:' to enable again
    void testAcquiringAndLosingResource();

};

#endif
