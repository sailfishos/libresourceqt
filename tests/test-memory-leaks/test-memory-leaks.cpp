#include "test-memory-leaks.h"
#include <cassert>

const int ITERATIONS = 20000;

void MemoryLeakTest::test() {
  set = new ResourceSet("player", this);

  iterations = ITERATIONS;

  AudioResource *audioResource = new ResourcePolicy::AudioResource("player");
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



int main(int argc, char* argv[]) {

  QCoreApplication app(argc, argv);

  MemoryLeakTest *leaks = new MemoryLeakTest();

  leaks->test();

  return app.exec();
  delete leaks;
}
