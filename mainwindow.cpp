#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * Metodo para obtener la direccion del video
 * @brief MainWindow::SeleccionarVideo
 */
void MainWindow::SeleccionarVideo(){
    // Declara la variable con la ruta del archivo
    QString archivo = QFileDialog::getOpenFileName(this, tr("Abrir Video"),
                                                    "",
                                                    tr("Videos (*.avi *.mp4 *.mov)"));
    //Agrega la ruta del archivo
    ui->labelVideo->setText(archivo);
    ui->radioVideo->setChecked(true);
}

/**
 * Metodo para procesar el video frame a frame si ckecked==true
 * @brief MainWindow::ProcesarVideo
 * @param checked
 *
 */
void MainWindow::ProcesarVideo(bool checked){

    cv::destroyAllWindows(); // Para cerrar todas las ventanas

    cv::Mat frame, next, prvs, flujo_xy; // Frame como array multidimencional

    int s = 1;

    cv::cuda::GpuMat prvs_cuda, next_cuda, flujo_xy_cuda;
    cv::cuda::GpuMat prvs_cuda_o, next_cuda_o;
    cv::cuda::GpuMat prvs_cuda_c, next_cuda_c;

    if(!checked){ // Si !checked detiene el video si no lo procesa
        ui->play->setText("Iniciar video");
        cap.release();
    }else{
        ui->play->setText("Parar video");

        if(ui->radioVideo->isChecked()){ // si el "radio button" esta seleccionado ejecuta el video si no la webcam
            cap.open(ui->labelVideo->text().toStdString().c_str());
        }else{
            cap.open(0);
        }
        cap >> frame;

        //cuda upload, resize, color convert
        prvs_cuda_o.upload(frame);

        cv::cuda::resize(prvs_cuda_o, prvs_cuda_c, cv::Size(frame.size().width / s, frame.size().height / s));
        cv::cuda::cvtColor(prvs_cuda_c, prvs_cuda, CV_BGR2GRAY);


    }

    //dense optical flujo
    cv::Ptr< cv::cuda::FarnebackOpticalFlow > fbOF = cv::cuda::FarnebackOpticalFlow::create();



    while(checked) // bucle hasta que se precione "parar video"
    {
        cap >> frame; // obtiene un nuevo frame del video o camara
        if(frame.empty()) break; // detiene el bucle si elframe esta vacio

        //cuda upload, resize, color convert
        next_cuda_o.upload(frame);
        cv::cuda::resize(next_cuda_o, next_cuda_c, cv::Size(frame.size().width / s, frame.size().height / s));
        cv::cuda::cvtColor(next_cuda_c, next_cuda, CV_BGR2GRAY);

        //dense optical flujo
        fbOF->calc(prvs_cuda, next_cuda, flujo_xy_cuda);

        //copy for vector flujo drawing
        cv::Mat cflujo;
        cv::resize(frame, cflujo, cv::Size(frame.size().width / s, frame.size().height / s));
        flujo_xy_cuda.download(flujo_xy);

        mostrarFlujo(flujo_xy, cflujo, 10, CV_RGB(0, 255, 0));

        cv::namedWindow("Reproductor", cv::WINDOW_KEEPRATIO); // creamos una ventana la cual permita redimencionar
        cv::imshow("Reproductor", cflujo); // se muestran los frames

        next_cuda.download(next);
        prvs_cuda.download(prvs);
        prvs_cuda = next_cuda.clone();

        char key = (char) cv::waitKey(20); //espera 20ms por la tecla ESC
        if(key == 27) break; //detiene el bucle
    }

}


void  MainWindow::mostrarFlujo(const cv::Mat& flujo_xy, cv::Mat& cflujomap, int step, const cv::Scalar& color)
{

 for (int y = 0; y < cflujomap.rows; y += step)
  for (int x = 0; x < cflujomap.cols; x += step)
  {
   cv::Point2f fxy;
   fxy.x = cvRound(flujo_xy.at< cv::Vec2f >(y, x)[0] + x);
   fxy.y = cvRound(flujo_xy.at< cv::Vec2f >(y, x)[1] + y);


   cv::line(cflujomap, cv::Point(x, y), cv::Point(fxy.x, fxy.y), color);
   cv::circle(cflujomap, cv::Point(fxy.x, fxy.y), 1, color, -1);

  }

}

void MainWindow::on_toolButton_clicked()
{
    SeleccionarVideo();
}

void MainWindow::on_actionAbrir_Video_triggered()
{
    SeleccionarVideo();
}



void MainWindow::on_play_toggled(bool checked)
{
    ProcesarVideo(checked);
}
