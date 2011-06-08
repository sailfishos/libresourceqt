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



#include "test-memory-leaks.h"
#include <cassert>

const int ITERATIONS = 20000;

void MemoryLeakTest::test() {
  set = new ResourceSet("player", this);

  iterations = ITERATIONS;

  AudioResource *audioResource = new ResourcePolicy::AudioResource();
  set->addResourceObject(audioResource);

  connect(set,  SIGNAL(resourcesGranted(QList<ResourcePolicy::ResourceType>)),
         this, SLOT(resourceAcquiredHandler(QList<ResourcePolicy::ResourceType>)));

  connect(set, SIGNAL(resourcesReleased()), this, SLOT(resourceReleasedHandler()));

  set->acquire();
}


void MemoryLeakTest::resourceAcquiredHandler(QList<ResourceType> notused)
{
  (void) notused;
  QList<Resource*> list = set->resources();
    if (list.count() == 0) {
        printf("Granted resource set is empty\n");
    }
    else {
      for (int i = 0; i < list.count(); i++) {
        //printf("Granted resource %d\n", (int)list[i]);
      }

      set->release();

    }
}

void MemoryLeakTest::resourceReleasedHandler()
{
    set->acquire();
    iterations--;
    printf("iteration %d\n", (ITERATIONS-iterations+1));
    if (iterations <= 0)  exit(0);

}

#define STATUS_BUF_SIZE 2047
static char status_buf[STATUS_BUF_SIZE+1];

void MemoryLeakTest::update_memory_stat()
{
    const int MAX_NAME = 100;
    char filename[MAX_NAME + 1];
    filename[MAX_NAME] = '\0';

    snprintf(filename, MAX_NAME, "/proc/%d/status", getpid());
    FILE* status = fopen(filename, "r");
    fread(status_buf, 1, STATUS_BUF_SIZE, status);
    fclose(status);

    char *ptr = strstr(status_buf, "VmSize:");

    int retcnt = 0;

    assert(ptr);
/*
    if (ptr)  retcnt = sscanf(ptr+7, "%d", &vm_size);

    if( retcnt != 1 )
        return FALSE;

    return TRUE;*/
}


int main(int argc, char* argv[]) {

  QCoreApplication app(argc, argv);

  MemoryLeakTest *leaks = new MemoryLeakTest();

  leaks->test();

  return app.exec();
  delete leaks;
}
