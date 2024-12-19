#include "MainWindow.h"
#include "ImageProcessor.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QDebug>
#include <QPixmap>
#include <QImageReader>
#include <QColorDialog>
#include <QApplication>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_processor(nullptr)
    , m_epsilon(10)
    , m_item(nullptr)
    , m_highlightColor(Qt::red)
{
    ui->setupUi(this);
    m_scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(m_scene);

    connect(ui->openImageButton, &QPushButton::clicked, this, &MainWindow::on_openImageButton_clicked);
    connect(ui->slider, &QSlider::valueChanged, this, &MainWindow::on_slider_valueChanged);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::on_resetButton_clicked);
    connect(ui->colorButton, &QPushButton::clicked, this, &MainWindow::on_colorButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_processor;
}

void MainWindow::on_openImageButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg);;All Files (*)");
    if (fileName.isEmpty())
        return;

    QImageReader reader(fileName);
    if (!reader.canRead()) {
        qDebug() << "QImageReader can't read: " << fileName << ". Error: " << reader.errorString();
        return;
    }
    if (!reader.read(&m_originalImage)) {
        qDebug() << "QImageReader failed to read: " << fileName << ". Error: " << reader.errorString();
        return;
    }
    if (m_originalImage.isNull()) {
        qDebug() << "QImage is null " << fileName;
        return;
    }
    if (m_processor) {
        delete m_processor;
        m_processor = nullptr;
    }
    m_processor = new ImageProcessor(m_originalImage);
    m_highlightMask.resize(m_originalImage.width() * m_originalImage.height());
    std::fill(m_highlightMask.begin(), m_highlightMask.end(), false);
    updatePixmap();
    qDebug() << "on_openImageButton_clicked: Image loaded and mask initialized.";
}

void MainWindow::on_slider_valueChanged(int value)
{
    m_epsilon = value;
    qDebug() << "on_slider_valueChanged: Epsilon value changed to" << m_epsilon;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!m_processor || m_originalImage.isNull() || !m_item) {
        qDebug() << "mousePressEvent: Image not loaded or item is null.";
        return;
    }
    // Получаем координаты клика мыши в координатах viewport
    QPoint viewportPos = ui->graphicsView->viewport()->mapFromGlobal(event->globalPosition().toPoint());

    // Получаем позицию QGraphicsPixmapItem в координатах QGraphicsView
    QPointF itemPos = ui->graphicsView->mapFromScene(m_item->pos());

    // Преобразуем координаты клика в координаты изображения
    QPoint imageClickPoint = (viewportPos - itemPos.toPoint());

    // Получаем абсолютные координаты для отладки
    QPoint globalPos = ui->graphicsView->mapToGlobal(event->pos());

    qDebug() << "mousePressEvent: Mouse click at widget position: " << viewportPos
             << ", global position: " << globalPos
             << ", item position: " << itemPos <<", image position: " << imageClickPoint ;


    if (imageClickPoint.x() >= 0 && imageClickPoint.x() < m_originalImage.width() &&
        imageClickPoint.y() >= 0 && imageClickPoint.y() < m_originalImage.height()) {
        qDebug() << "mousePressEvent: Image click point is valid: " << imageClickPoint;
        on_imageClicked(imageClickPoint);
    } else {
        qDebug() << "mousePressEvent: Image click point is invalid: " << imageClickPoint;
    }
}

void MainWindow::updatePixmap()
{
    if (m_originalImage.isNull())
        return;

    QImage displayedImage = m_originalImage.copy();
    int width = m_originalImage.width();
    int height = m_originalImage.height();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (m_highlightMask[y * width + x]) {
                displayedImage.setPixel(x, y, m_highlightColor.rgba()); // Изменено!
            }
        }
    }

    m_pixmap = QPixmap::fromImage(displayedImage);
    if (m_item) {
        m_scene->removeItem(m_item);
        delete m_item;
    }
    m_item = new QGraphicsPixmapItem(m_pixmap);
    m_scene->addItem(m_item);
    m_scene->setSceneRect(m_pixmap.rect());
    qDebug() << "updatePixmap: Pixmap updated.";
}

void MainWindow::on_imageClicked(QPointF point)
{
    if (!m_processor || m_originalImage.isNull()) {
        qDebug() << "on_imageClicked: Image not loaded";
        return;
    }

    // Использовать нелинейную зависимость для epsilon
    int adjustedEpsilon = std::pow(m_epsilon, 1.5);

    QVector<bool> newMask = m_processor->createMask(point.toPoint(), adjustedEpsilon/10);
    if (newMask.size() != m_highlightMask.size()) {
        qDebug() << "on_imageClicked: Mask size mismatch.";
        return;
    }

    for (size_t i = 0; i < static_cast<size_t>(m_highlightMask.size()); i++) {
        m_highlightMask[i] |= newMask[i];
    }
    qDebug() << "on_imageClicked: New mask applied";
    updatePixmap();
}

void MainWindow::on_resetButton_clicked()
{
    if (m_originalImage.isNull())
        return;

    std::fill(m_highlightMask.begin(), m_highlightMask.end(), false);
    updatePixmap();
    qDebug() << "on_resetButton_clicked: Mask reset.";
}

void MainWindow::on_colorButton_clicked()
{
    QColor color = QColorDialog::getColor(m_highlightColor, this, "Select Highlight Color");
    if (color.isValid()) {
        m_highlightColor = color;
        updatePixmap(); // Обновляем отображение с новым цветом
        qDebug() << "on_colorButton_clicked: Highlight color changed to" << m_highlightColor;
    }
}
