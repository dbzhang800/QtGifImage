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
#include "qgifimage.h"
#include "qgifimage_p.h"
#include <QFile>
#include <QImage>
#include <QDebug>
#include <QScopedPointer>

namespace
{
int writeToIODevice(GifFileType *gifFile, const GifByteType *data, int maxSize)
{
    return static_cast<QIODevice *>(gifFile->UserData)->write(reinterpret_cast<const char *>(data), maxSize);
}

int readFromIODevice(GifFileType *gifFile, GifByteType *data, int maxSize)
{
    return static_cast<QIODevice *>(gifFile->UserData)->read(reinterpret_cast<char *>(data), maxSize);
}
}

QGifImagePrivate::QGifImagePrivate(QGifImage *p)
    :canvasWidth(-1), canvasHeight(-1), delayTime(0), q_ptr(p)
{

}

QGifImagePrivate::~QGifImagePrivate()
{

}

QVector<QRgb> QGifImagePrivate::colorTableFromColorMapObject(ColorMapObject *colorMap, int transColorIndex)
{
    QVector<QRgb> colorTable;
    if (colorMap) {
        for (int idx=0; idx<colorMap->ColorCount; ++idx) {
            GifColorType gifColor = colorMap->Colors[idx];
            QRgb color = gifColor.Blue | (gifColor.Green << 8) | (gifColor.Red << 16);
            // For non-transparent color, set the alpha to opaque.
            if (idx != transColorIndex)
                color |= 0xff << 24;
            colorTable.append(color);
        }
    }
    return colorTable;
}

bool QGifImagePrivate::load(QIODevice *device)
{
    static int interlacedOffset[] = { 0, 4, 2, 1 }; /* The way Interlaced image should. */
    static int interlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */

    GifFileType *gifFile = DGifOpen(device, readFromIODevice, 0);
    if (!gifFile)
        return false;

    if (DGifSlurp(gifFile) == GIF_ERROR)
        return false;

    canvasWidth = gifFile->SWidth;
    canvasHeight = gifFile->SHeight;
    QColor backgroundColor;
    if (gifFile->SColorMap) {
        globalColorTable = colorTableFromColorMapObject(gifFile->SColorMap);
        backgroundColor = globalColorTable[gifFile->SBackGroundColor];
    }

    for (int idx=0; idx<gifFile->ImageCount; ++idx) {
        SavedImage gifImage = gifFile->SavedImages[idx];
        int top = gifImage.ImageDesc.Top;
        int left = gifImage.ImageDesc.Left;
        int width = gifImage.ImageDesc.Width;
        int height = gifImage.ImageDesc.Height;

        QScopedPointer<GraphicsControlBlock> gcb(new GraphicsControlBlock);
        DGifSavedExtensionToGCB(gifFile, idx, gcb.data());
        int transColorIndex = gcb->TransparentColor;

        QVector<QRgb> colorTable;
        if (gifImage.ImageDesc.ColorMap)
            colorTable = colorTableFromColorMapObject(gifImage.ImageDesc.ColorMap, transColorIndex);
        else if (transColorIndex != -1)
            colorTable = colorTableFromColorMapObject(gifFile->SColorMap, transColorIndex);
        else
            colorTable = globalColorTable;

        QImage image(width, height, QImage::Format_Indexed8);
        image.setOffset(QPoint(left, top));
        image.setColorTable(colorTable);
        if (transColorIndex != -1)
            image.fill(transColorIndex);
        else if (!globalColorTable.isEmpty())
            image.fill(gifFile->SBackGroundColor); //!ToDo

        if (gifImage.ImageDesc.Interlace) {
            int line = 0;
            for (int i = 0; i < 4; i++) {
                for (int row = interlacedOffset[i]; row <height; row += interlacedJumps[i]) {
                    memcpy(image.scanLine(row), gifImage.RasterBits+line*width, width);
                    line ++;
                }
            }
        } else {
            for (int row = 0; row < height; row++) {
                memcpy(image.scanLine(row), gifImage.RasterBits+row*width, width);
            }
        }


        //Extract other data for the image.
//        for (int i=0; i<gifImage->ExtensionBlockCount; ++i) {
//            ExtensionBlock *extBlock = gifImage->ExtensionBlocks[i];
//            if (extBlock->Function == GRAPHICS_EXT_FUNC_CODE) {
//            }
//        }

        frames.append(image);
    }

    DGifCloseFile(gifFile);
}

bool QGifImagePrivate::save(QIODevice *device)
{
    return false;
}


/*!
    \class QGifImage
    \brief Class used to read/wirte .gif files.
*/

/*!
    Constructs a gif image
*/
QGifImage::QGifImage()
    :d_ptr(new QGifImagePrivate(this))
{

}

/*!
    Constructs a gif image and tries to load the image from the
    file with the given \a fileName
*/
QGifImage::QGifImage(const QString &fileName)
    :d_ptr(new QGifImagePrivate(this))
{
    load(fileName);
}

/*!
    Destroys the gif image and cleans up.
*/
QGifImage::~QGifImage()
{
    delete d_ptr;
}

int QGifImage::frameCount() const
{
    Q_D(const QGifImage);
    return d->frames.count();
}

QList<QImage> QGifImage::frames() const
{
    Q_D(const QGifImage);
    return d->frames;
}

/*!
    Saves the gif image to the file with the given \a fileName.
    Returns \c true if the image was successfully saved; otherwise
    returns \c false.
*/
bool QGifImage::save(const QString &fileName) const
{
    return false;
}

/*!
    \overload

    This function writes a QImage to the given \a device.
*/
bool QGifImage::save(QIODevice *device) const
{
    return false;
}

/*!
    Loads an gif image from the file with the given \a fileName. Returns \c true if
    the image was successfully loaded; otherwise invalidates the image
    and returns \c false.
*/
bool QGifImage::load(const QString &fileName)
{
    Q_D(QGifImage);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
        return d->load(&file);

    return false;
}

/*!
    \overload

    This function reads a gif image from the given \a device. This can,
    for example, be used to load an image directly into a QByteArray.
*/
bool QGifImage::load(QIODevice *device)
{
    Q_D(QGifImage);
    if (device->openMode() | QIODevice::ReadOnly)
        return d->load(device);

    return false;
}
