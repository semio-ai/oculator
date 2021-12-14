#include <QtWidgets/QApplication>
#include <QtCore/QTimer>

#include "ui_mainwindow.h"

#include "DeviceReader.hpp"
#include "DeviceReader.hpp"

#include <opencv2/opencv.hpp>

using namespace qoculator;


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QMainWindow main_window;

  Ui::MainWindow ui;
  ui.setupUi(&main_window);

  main_window.show();

  QTimer timer;
  timer.setInterval(30);

  DeviceReader reader(ui.raw, ui.saliency);

  QObject::connect(&timer, &QTimer::timeout, &reader, &DeviceReader::update);

  timer.start();
  
  return app.exec();
}