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



#ifndef TEST_RESOURCE_H
#define TEST_RESOURCE_H

#include <QtTest/QTest>
#include <QObject>
#include <QMetaType>
#include <policy/resources.h>
#include <policy/audio-resource.h>

Q_DECLARE_METATYPE(ResourcePolicy::ResourceType)

using namespace ResourcePolicy;

class TestResource: public QObject
{
    Q_OBJECT
private:
    AudioResource *audioResource;
    AudioRecorderResource *audioRecorderResource;
    Resource *videoResource;
    Resource *videoRecorderResource;
    Resource *vibraResource;
    Resource *ledsResource;
    Resource *backlightResource;
    Resource *systemButtonResource;
    Resource *lockButtonResource;
    Resource *scaleButtonResource;
    Resource *snapButtonResource;
    Resource *lensCoverResource;
    Resource *headsetButtonsResource;

    Resource * resourceFromType(ResourceType type);
    const char * stringFromType(ResourceType type);
public:
    TestResource();
    ~TestResource();

private slots:

    void init();
    void cleanup();

    void testType();

    void testOptional_data();
    void testOptional();
};

#endif

