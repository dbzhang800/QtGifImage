#include "qgifimage.h"
#include <QPainter>
#include <QImage>
#include <QGuiApplication>
#include <QDebug>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QGifImage gif(QSize(300, 300));

    QImage image(100, 100, QImage::Format_RGB32);
    image.fill(QColor(Qt::red));
    QPainter p(&image);
    p.drawRect(20, 20, 60, 60);

    gif.addFrame(image);

    image.setOffset(QPoint(150, 150));
    gif.addFrame(image);

    gif.save(SRCDIR"test.gif");
    {
    QGifImage gif(SRCDIR"test.gif");
    for (int i=0; i<gif.frames().size(); ++i) {
        QImage image = gif.frames()[i];

        qDebug()<<QString("Frame %1: size %2X%3 at (%4, %5)").arg(i)
                  .arg(image.width()).arg(image.height())
                  .arg(image.offset().x()).arg(image.offset().y());

        image.save(QString(SRCDIR"test_%1.png").arg(i));
    }
    }
    return 0;
}
