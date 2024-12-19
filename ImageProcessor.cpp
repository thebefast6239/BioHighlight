// ImageProcessor.cpp
#include "ImageProcessor.h"
#include <QImage>
#include <QPoint>
#include <QDebug>
#include <QQueue>
#include <QVector>

ImageProcessor::ImageProcessor(const QImage &image)
    : m_image(image)
{
}

QVector<bool> ImageProcessor::createMask(const QPoint &startPoint, int epsilon)
{
    if (m_image.isNull())
    {
        qDebug() << "createMask: Image is null.";
        return QVector<bool>();
    }

    if (!m_image.rect().contains(startPoint)) {
        qDebug() << "createMask: Start point is outside image bounds.";
        return QVector<bool>(m_image.width() * m_image.height(), false); // Возвращаем пустую маску
    }

    int width = m_image.width();
    int height = m_image.height();


    QVector<bool> visited(width * height, false);
    QQueue<QPoint> queue;
    queue.enqueue(startPoint);
    visited[startPoint.y() * width + startPoint.x()] = true; // Mark the starting point

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};


    while (!queue.isEmpty()) {
        QPoint currentPoint = queue.dequeue();
        int x = currentPoint.x();
        int y = currentPoint.y();
        int intensity = qGray(m_image.pixel(currentPoint));


        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if(nx >= 0 && nx < width && ny >= 0 && ny < height){
                QPoint neighbor(nx, ny);
                int neighborIntensity = qGray(m_image.pixel(neighbor)); // интенсивность соседнего пикселя
                if (!visited[ny * width + nx] && qAbs(neighborIntensity - intensity) <= epsilon )
                {
                    queue.enqueue(neighbor);
                    visited[ny * width + nx] = true;
                }
            }
        }

    }
    qDebug() << "createMask: Mask created.";
    return visited;
}
