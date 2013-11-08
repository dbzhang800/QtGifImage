/****************************************************************************
** Copyright (c) 2013 Debao Zhang <hello@debao.me>
** All right reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#ifndef QGIFIMAGE_H
#define QGIFIMAGE_H

#include "qgifglobal.h"
#include <QImage>
#include <QList>
#include <QVector>

class QGifImagePrivate;
class Q_GIFIMAGE_EXPORT QGifImage
{
    Q_DECLARE_PRIVATE(QGifImage)
public:
    QGifImage();
    QGifImage(QIODevice *device);
    QGifImage(const QString &fileName);
    QGifImage(const QSize &size);
    ~QGifImage();

    void setGlobalColorTable(QVector<QRgb> colors);
    void setDefaultDelayTime(int internal);
    bool addFrame(const QImage &frame, int delayTime=-1);

    int frameCount() const;
    QList<QImage> frames() const;

    bool load(QIODevice *device);
    bool load(const QString &fileName);
    bool save(QIODevice *device) const;
    bool save(const QString &fileName) const;

private:
    QGifImagePrivate * const d_ptr;
};

#endif // QGIFIMAGE_H
