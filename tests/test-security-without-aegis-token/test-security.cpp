#include "qt4/policy/resource-set.h"
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
