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



#include "qt4/policy/resource-set.h"
#include <QObject>
#include <QList>
#include <cstdio>
#include <cstdlib>
#include <QCoreApplication>

using namespace ResourcePolicy;

class TestSecurity: public QObject
{
    Q_OBJECT

public:
    void testSecurity() {

      ResourceSet *resourceSet = new ResourceSet("call", this);
      resourceSet->addResource(ResourcePolicy::AudioRecorderType);

      connect(resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType>&)),
              this,        SLOT(resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>&)));

      connect(resourceSet, SIGNAL(errorCallback(quint32, const char*)), this, SLOT(errorCallback(quint32, const char*)));

      resourceSet->acquire();
    }

private slots:
    void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& /*grantedOptionalResList*/) {
      printf(">>> resource acquired\n");
      exit(0);
    }

    void errorCallback(quint32 code, const char* message) {
      printf(">>> error %u, %s\n", code, message);
      exit(1);
    }
};

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  TestSecurity *test = new TestSecurity();
  test->testSecurity();

  return app.exec();
}

#include "test-security.moc"
