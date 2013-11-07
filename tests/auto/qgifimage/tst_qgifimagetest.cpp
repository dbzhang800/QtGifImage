#include "qgifimage.h"
#include <QtTest>

class QGifimageTest : public QObject
{
    Q_OBJECT

public:
    QGifimageTest();

private Q_SLOTS:
    void testGifFileLoad();
};

QGifimageTest::QGifimageTest()
{
}

void QGifimageTest::testGifFileLoad()
{
    QGifImage gif(SRCDIR"test.gif");

    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(QGifimageTest)

#include "tst_qgifimagetest.moc"
