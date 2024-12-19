// ImageProcessor.h
#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QPoint>
#include <QVector>

class ImageProcessor
{
public:
    ImageProcessor(const QImage &image);
    QVector<bool> createMask(const QPoint &startPoint, int epsilon);

private:
    QImage m_image;
};

#endif // IMAGEPROCESSOR_H
