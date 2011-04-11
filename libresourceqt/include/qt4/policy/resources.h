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
/**
* \file resources.h
* \brief Declaration of The different resource classes: 
* ResourcePolicy::AudioRecorderResource
* ResourcePolicy::BacklightResource
* ResourcePolicy::LedsResource
* ResourcePolicy::VibraResource
* ResourcePolicy::VideoRecorderResource
* ResourcePolicy::VideoResource
* ResourcePolicy::LockButtonResource
* ResourcePolicy::ScaleButtonResource
* ResourcePolicy::SystemButtonResource
* ResourcePolicy::LensCoverResource
* ResourcePolicy::HeadsetButtonsResource
*
* \copyright Copyright (C) 2010 Nokia Corporation.
* \author Wolf Bergenheim
* \par License
* @license LGPL
* This file is part of libresourceqt
* \par
* Copyright (C) 2010 Nokia Corporation.
* \par
* This library is free software; you can redistribute
* it and/or modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation
* version 2.1 of the License.
* \par
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
* \par
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
* USA.
*/

#ifndef RESOURCES_H
#define RESOURCES_H

#include <policy/resource.h>
#include <QString>

namespace ResourcePolicy
{

/**
* The AudioRecorderResource class represents the audio recorder device.
*/
class AudioRecorderResource: public Resource
{
public:
	/**
	* The constructor
	*/
	AudioRecorderResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	AudioRecorderResource(const AudioRecorderResource &other);

	/**
	* The destructor
	*/
	virtual ~AudioRecorderResource();

	/**
	* \return The resource type
	*/
	virtual ResourceType type() const;
};

/**
* The BacklightResource is used by applications wanting to control the
* backlight
*/
class BacklightResource: public Resource
{
public:
	/**
	* The constructor
	*/
	BacklightResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	BacklightResource(const BacklightResource &other);

	/**
	* The destructor
	*/
	virtual ~BacklightResource();

	/**
	* \return The resource type
	*/
	virtual ResourceType type() const;
};

/**
* Resource to be used when the application wants to fiddle with the leds.
*/
class LedsResource: public Resource
{
public:
	/**
	* The constructor
	*/
	LedsResource();
	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	LedsResource(const LedsResource &other);

	/**
	* The destructor
	*/
	virtual ~LedsResource();

	/**
	* \return the resource type
	*/
	virtual ResourceType type() const;
};

/**
* Resource to be used when the application wants to use the vibra.
*/
class VibraResource: public Resource
{
public:
	/**
	* The constructor
	*/
	VibraResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	VibraResource(const VibraResource &other);

	/**
	* The destructor
	*/
	virtual ~VibraResource();

	/**
	* \return The resource type
	*/
	virtual ResourceType type() const;
};

/**
* Resource to be used when the application wants to record video.
*/
class VideoRecorderResource: public Resource
{
public:
	/**
	* The constructor
	*/
	VideoRecorderResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	VideoRecorderResource(const VideoRecorderResource &other);

	/**
	* The destructor
	*/
	virtual ~VideoRecorderResource();

	/**
	* \return The resource type
	*/
	virtual ResourceType type() const;
};

/**
* Resource to be used when the application wants to render video with the
* DSP decoder to render the viewfinder.
*/
class VideoResource: public QObject, public Resource
{
	Q_OBJECT
public:
	/**
	* The constructor
	*/
	VideoResource(quint32 pid);

	VideoResource():pid(0){} ;

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	VideoResource(const VideoResource &other);

	/**
	* The destructor
	*/
	virtual ~VideoResource();

	/**
	* \return the resource type
	*/
	virtual ResourceType type() const;

	quint32 processID() const { return pid; }

	/**
	* Set the process ID of the video resource.
	*/
	void setProcessID(quint32 newPID);

signals:
	void videoPropertiesChanged(quint32 pid);
private:
	quint32 pid;

};

/**
* Resource to be used when the application wants to control the system button.
*/
class SystemButtonResource: public Resource
{
public:
	/**
	* The constructor
	*/
	SystemButtonResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	SystemButtonResource(const SystemButtonResource &other);

	/**
	* The destructor
	*/
	virtual ~SystemButtonResource();

	/**
	* \return The resource type
	*/
	virtual ResourceType type() const;
};

/**
* Resource to be used when the application wants to control the lock button.
*/
class LockButtonResource: public Resource
{
public:
	/**
	* The constructor
	*/
	LockButtonResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	LockButtonResource(const LockButtonResource &other);

	/**
	* The destructor
	*/
	virtual ~LockButtonResource();

	/**
	* \return The resource type
	*/
	virtual ResourceType type() const;
};

/**
* Resource to be used when the application wants to control
* the volume/scale button.
*/
class ScaleButtonResource: public Resource
{
public:
	/**
	* The constructor
	*/
	ScaleButtonResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	ScaleButtonResource(const ScaleButtonResource &other);

	/**
	* The destructor
	*/
	virtual ~ScaleButtonResource();

	/**
	* \return The resource type
	*/
	virtual ResourceType type() const;
};

/**
* Resource to be used when the application wants to take still pictures.
*/
class SnapButtonResource: public Resource
{
public:
	/**
	* The constructor
	*/
	SnapButtonResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	SnapButtonResource(const SnapButtonResource &other);

	/**
	* The destructor
	*/
	virtual ~SnapButtonResource();

	/**
	* \return The resource type
	*/
	virtual ResourceType type() const;
};

class LensCoverResource: public Resource
{
public:
	/**
	* The constructor.
	*/
	LensCoverResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	LensCoverResource(const LensCoverResource &other);

	/**
	* The destructor
	*/
	virtual ~LensCoverResource();

	/**
	* \return The resource type
	*/
	virtual ResourceType type() const;
};

/**
* Resource to be used when the application wants to control the
* headset buttons.
*/
class HeadsetButtonsResource: public Resource
{
public:
	/**
	* The constructor
	*/
	HeadsetButtonsResource();

	/**
	* The copy constructor
	* \param other The resource to copy from
	*/
	HeadsetButtonsResource(const HeadsetButtonsResource &other);

	/**
	* The destructor
	*/
	virtual ~HeadsetButtonsResource();

	/**
	* \return the resource type
	*/
	virtual ResourceType type() const;
};

}
#endif

