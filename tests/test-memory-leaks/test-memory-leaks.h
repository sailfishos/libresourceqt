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
