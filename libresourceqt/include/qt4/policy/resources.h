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

#ifndef RESOURCES_H
#define RESOURCES_H

#include <policy/resource.h>
#include <QString>

namespace ResourcePolicy
{

class AudioRecorderResource: public Resource
{
public:
    AudioRecorderResource();
    AudioRecorderResource(const AudioRecorderResource &other);
    virtual ~AudioRecorderResource();

    virtual ResourceType type() const;
private:
    QString group;
    quint32 pid;
    QString stream;
};

class BacklightResource: public Resource
{
public:
    BacklightResource();
    BacklightResource(const BacklightResource &other);
    virtual ~BacklightResource();

    virtual ResourceType type() const;
};

class LedsResource: public Resource
{
public:
    LedsResource();
    LedsResource(const LedsResource &other);
    virtual ~LedsResource();

    virtual ResourceType type() const;
};

class VibraResource: public Resource
{
public:
    VibraResource();
    VibraResource(const VibraResource &other);
    virtual ~VibraResource();

    virtual ResourceType type() const;
};

class VideoRecorderResource: public Resource
{
public:
    VideoRecorderResource();
    VideoRecorderResource(const VideoRecorderResource &other);
    virtual ~VideoRecorderResource();

    virtual ResourceType type() const;
};

class VideoResource: public Resource
{
public:
    VideoResource();
    VideoResource(const VideoResource &other);
    virtual ~VideoResource();

    virtual ResourceType type() const;
};

class SystemButtonResource: public Resource
{
public:
    SystemButtonResource();
    SystemButtonResource(const SystemButtonResource &other);
    virtual ~SystemButtonResource();

    virtual ResourceType type() const;
};

class LockButtonResource: public Resource
{
public:
    LockButtonResource();
    LockButtonResource(const LockButtonResource &other);
    virtual ~LockButtonResource();

    virtual ResourceType type() const;
};

class ScaleButtonResource: public Resource
{
public:
    ScaleButtonResource();
    ScaleButtonResource(const ScaleButtonResource &other);
    virtual ~ScaleButtonResource();

    virtual ResourceType type() const;
};

class SnapButtonResource: public Resource
{
public:
    SnapButtonResource();
    SnapButtonResource(const SnapButtonResource &other);
    virtual ~SnapButtonResource();

    virtual ResourceType type() const;
};

class LensCoverResource: public Resource
{
public:
    LensCoverResource();
    LensCoverResource(const LensCoverResource &other);
    virtual ~LensCoverResource();

    virtual ResourceType type() const;
};

class HeadsetButtonsResource: public Resource
{
public:
    HeadsetButtonsResource();
    HeadsetButtonsResource(const HeadsetButtonsResource &other);
    virtual ~HeadsetButtonsResource();

    virtual ResourceType type() const;
};

}
#endif

