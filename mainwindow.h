// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QColor>

class ImageProcessor;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openImageButton_clicked();
    void on_slider_valueChanged(int value);
    void on_resetButton_clicked(); // Добавлен слот для кнопки сброса
    void on_colorButton_clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void on_imageClicked(QPointF point);
    void updatePixmap();


    Ui::MainWindow *ui;
    QGraphicsScene *m_scene;
    QImage m_originalImage; // Храним исходное изображение
    QPixmap m_pixmap;
    QVector<bool> m_highlightMask; // Храним маску выделения
    ImageProcessor *m_processor;
    int m_epsilon;
    QGraphicsPixmapItem *m_item;
    QColor m_highlightColor = QColor(255, 0, 0, 128); // Цвет выделения
};

#endif // MAINWINDOW_H
