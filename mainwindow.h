#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgproc.hpp>

#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaoptflow.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void SeleccionarVideo();
    void ProcesarVideo(bool checked);

    void mostrarFlujo(const cv::Mat &flow_xy, cv::Mat &cflowmap, int step, const cv::Scalar &color);
private slots:
    void on_toolButton_clicked();

    void on_actionAbrir_Video_triggered();

    void on_play_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    cv::VideoCapture cap;
};

#endif // MAINWINDOW_H
