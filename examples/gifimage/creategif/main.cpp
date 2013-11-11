#include "qgifimage.h"
#include <QPainter>
#include <QImage>
#include <QGuiApplication>
#include <QDebug>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QGifImage gif(QSize(300, 300));
    gif.setGlobalColorTable(QVector<QRgb>()<<qRgba(255,255,255, 0));

    QImage image(100, 100, QImage::Format_RGB32);
    image.fill(QColor(Qt::red));
    QPainter p(&image);
    p.drawRect(20, 20, 60, 60);

    for (int i=0; i<10; ++i) {
        gif.addFrame(image, QPoint(i*20, i*20), 100);
    }

    gif.save(SRCDIR"test.gif");

    return 0;
}
