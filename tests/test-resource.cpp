#include "test-resource.h"

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
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("expected");

    QTest::newRow("AudioPlayback") << "AudioPlayback"  << "AudioPlayback";
    QTest::newRow("VideoPlayback") << "VideoPlayback" << "VideoPlayback";
    QTest::newRow("AudioRecording") << "AudioRecording" << "AudioRecording";
    QTest::newRow("VideoRecording") << "VideoRecording" << "VideoRecording";
    QTest::newRow("Invalid") << "Invalid" << "Invalid";
    QTest::newRow("Empty") << "" << "";
    QTest::newRow("Null") << QString() << QString();
}
void TestResource::testType()
{
    QFETCH(QString, type);
    QFETCH(QString, expected);
    resource->setType(type);
    QString result = resource->type();

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

    resource->setId(identifier);
    quint32 result = resource->id();

    QCOMPARE(result, expected);
}

void TestResource::testCopy()
{
    Resource copy;
    resource->setOptional();

    copy = *resource;

    QCOMPARE(copy.id(), resource->id());
    QCOMPARE(copy.isOptional(), resource->isOptional());
    QCOMPARE(copy.isShared(), resource->isShared());
    QCOMPARE(copy.type(), resource->type());
}

void TestResource::testCopyConstructor()
{
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
    resource->setOptional();

    copy = *resource;

    QVERIFY(copy == *resource);
}

QTEST_MAIN(TestResource)
