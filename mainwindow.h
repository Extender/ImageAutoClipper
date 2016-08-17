#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QStandardPaths>
#include <QGraphicsPixmapItem>
#include <QStringList>

#include "text.h"
#include "extcolordefs.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QFileDialog *dialog;
    QImage *image;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;
    int originalImageWidth;
    int originalImageHeight;
    uint32_t *originalImageData;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static uint32_t *qImageToBitmapData(QImage *image);
    static bool getColorFromHexString(const char *str, uint32_t &color);

public slots:
    void browseBtnClicked();
    void loadBtnClicked();
    void fitToWindow();
    void resetZoom();
    void dialogFileSelected(QString path);
    void resetBtnClicked();
    void autoClipBtnClicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
