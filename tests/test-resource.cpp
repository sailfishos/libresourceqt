#include "test-resource.h"
#include <QSet>

using namespace ResourcePolicy;

TestResource::TestResource()
{
}

TestResource::~TestResource()
{
}

void TestResource::init()
{
    resource = new Resource();
}

void TestResource::cleanup()
{
    delete resource;
}

void TestResource::testType_data()
{
    QTest::addColumn<ResourceType>("type");
    QTest::addColumn<ResourceType>("expected");

    QTest::newRow("AudioPlayback") << AudioPlaybackResource << AudioPlaybackResource;
    QTest::newRow("VideoPlayback") << VideoPlaybackResource << VideoPlaybackResource;
    QTest::newRow("AudioRecorder") << AudioRecorderResource << AudioRecorderResource;
    QTest::newRow("VideoRecorder") << VideoRecorderResource << VideoRecorderResource;
    QTest::newRow("Invalid") << InvalidResource << InvalidResource;
}

void TestResource::testType()
{
    QFETCH(ResourceType, type);
    QFETCH(ResourceType, expected);
    bool setWasSuccessfull = resource->setType(type);
    ResourceType result = resource->type();

    if(expected == InvalidResource) {
	QEXPECT_FAIL("", "Expecting to fail since type == InvalidResource", Continue);
    }
    QVERIFY(setWasSuccessfull);
    QCOMPARE(result, expected);
}

void TestResource::testOptional_data()
{
    QTest::addColumn<bool>("optional");
    QTest::addColumn<bool>("expected");
    
    QTest::newRow("Resource is optional") << true << true;
    QTest::newRow("Resource is not optional") << false << false;
}

void TestResource::testOptional()
{
    QFETCH(bool, optional);
    QFETCH(bool, expected);

    resource->setType(AudioPlaybackResource);
    resource->setOptional(optional);
    bool result = resource->isOptional();

    QCOMPARE(result, expected);
}

void TestResource::testShared_data()
{
    QTest::addColumn<bool>("optional");
    QTest::addColumn<bool>("expected");
    
    QTest::newRow("Resource is shared") << true << true;
    QTest::newRow("Resource is not hhared") << false << false;
}

void TestResource::testShared()
{
    QFETCH(bool, optional);
    QFETCH(bool, expected);

    resource->setType(AudioPlaybackResource);
    resource->setShared(optional);
    bool result = resource->isShared();

    QCOMPARE(result, expected);
}

void TestResource::testIdentifier_data()
{
    QTest::addColumn<quint32>("identifier");
    QTest::addColumn<quint32>("expected");
    
    QTest::newRow("Set identifier") << (quint32)this << (quint32)this;
    QTest::newRow("Set to 0") << 0U << 0U;
}

void TestResource::testIdentifier()
{
    QFETCH(quint32, identifier);
    QFETCH(quint32, expected);

    resource->setType(AudioPlaybackResource);
    resource->setId(identifier);
    quint32 result = resource->id();

    QCOMPARE(result, expected);
}

void TestResource::testCopy()
{
    Resource copy;
    resource->setType(AudioPlaybackResource);
    resource->setOptional();

    copy = *resource;

    QCOMPARE(copy.id(), resource->id());
    QCOMPARE(copy.isOptional(), resource->isOptional());
    QCOMPARE(copy.isShared(), resource->isShared());
    QCOMPARE(copy.type(), resource->type());
}

void TestResource::testCopyConstructor()
{
    resource->setType(AudioPlaybackResource);
    resource->setOptional();
    Resource copy(*resource);

    QCOMPARE(copy.id(), resource->id());
    QCOMPARE(copy.isOptional(), resource->isOptional());
    QCOMPARE(copy.isShared(), resource->isShared());
    QCOMPARE(copy.type(), resource->type());
}

void TestResource::testEqualsOperator()
{
    Resource copy;
    resource->setType(AudioPlaybackResource);
    resource->setOptional();

    copy = *resource;

    QVERIFY(copy == *resource);
}

void TestResource::testHashFunctionWithCopies()
{
    resource->setType(AudioPlaybackResource);
    resource->setOptional();
    resource->setShared();
    Resource copy(*resource);

    uint originalHash = qHash(*resource);
    uint copyHash = qHash(copy);
    
    QCOMPARE(originalHash, copyHash);
}

void TestResource::testHashFunctionWithDifferentResources()
{
    resource->setType(AudioPlaybackResource);
    resource->setOptional();
    resource->setShared();
    Resource copy(*resource);
    copy.setType(VideoPlaybackResource);

    uint originalHash = qHash(*resource);
    uint copyHash = qHash(copy);
    
    QEXPECT_FAIL("", "Expecting to fail since they are different resources", Continue);
    QCOMPARE(originalHash, copyHash);
}

void TestResource::testHashFunctionWithDifferentOptionality()
{
    resource->setType(AudioPlaybackResource);
    resource->setOptional();
    resource->setShared();
    Resource copy(*resource);
    copy.setOptional(false);

    uint originalHash = qHash(*resource);
    uint copyHash = qHash(copy);
    
    QEXPECT_FAIL("", "Expecting to fail since one isOptional, the other isn't", Continue);
    QCOMPARE(originalHash, copyHash);
}

void TestResource::testHashFunctionWithDifferentSharedness()
{
    resource->setType(AudioPlaybackResource);
    resource->setOptional();
    resource->setShared();
    Resource copy(*resource);
    copy.setShared(false);

    uint originalHash = qHash(*resource);
    uint copyHash = qHash(copy);
    
    QEXPECT_FAIL("", "Expecting to fail since one isShared , the other isn't", Continue);
    QCOMPARE(originalHash, copyHash);
}

void TestResource::testHashFunctionWithIdentical()
{
    resource->setType(AudioPlaybackResource);
    resource->setOptional();
    resource->setShared();
    Resource copy;
    copy.setOptional();
    copy.setShared();
    copy.setType(AudioPlaybackResource);

    uint originalHash = qHash(*resource);
    uint copyHash = qHash(copy);
    
    QCOMPARE(originalHash, copyHash);
}

void TestResource::testQSetOfResource()
{
    Resource a,b,c;
    a.setType(AudioPlaybackResource);
    b.setType(VideoPlaybackResource);
    c.setType(AudioRecorderResource);
    
    QSet<Resource> set;
    set.insert(a);
    set.insert(b);
    set.insert(c);

    QVERIFY(set.contains(a));
    QVERIFY(set.contains(b));
    QVERIFY(set.contains(c));
}

QTEST_MAIN(TestResource)
