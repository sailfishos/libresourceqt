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



#include <policy/resource-set.h>

using namespace ResourcePolicy;

class MemoryLeakTest : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(MemoryLeakTest)
public:
  MemoryLeakTest(QObject *parent = NULL) : QObject(parent)  {}

  void update_memory_stat();
  void test();

private:
  ResourceSet *set;
  int iterations;

private slots:
  void resourceAcquiredHandler(QList<ResourcePolicy::ResourceType> grantedResList);
  void resourceReleasedHandler();

};
