#include <iostream>
#include <cstdint>

#include <torch/torch.h>
#include <torch/script.h>

#include <QtWidgets/QApplication>
#include <QtCore/QTimer>
#include <QtWidgets/QLabel>
#include <QPainter> 

#include "ui_mainwindow.h"
#include "DeviceReader.hpp"
#include "oculator/utils/image_utils.h"

using namespace qoculator;

int main(int argc, char *argv[])
{
  torch::jit::script::Module module;
  try {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    module = torch::jit::load("/Users/drobotnik/projects/oculator/models/deepgaze.pth");
  }
  catch (const c10::Error& e) {
    std::cerr<< e.msg() << std::endl;
    std::cerr << "error loading the model\n";
    return -1;
  }

  torch::jit::script::Module container = torch::jit::load("/Users/drobotnik/projects/oculator/models/centerbias.pt");
  //container.dump(true, true, true);
  /*auto attributes = container.named_modules();
  for(const auto &attribute : attributes) {
    std::cout << "Attribute: " << attribute.name << std::endl;
    auto v = attribute.value;
    //    std::cout << v << std::endl;
  }
  auto centerbias = (*container._ivalue()).getAttr("cb");//container.attr("forward").toTensor();
  std::cout << "IVal: " << centerbias << std::endl;*/
  torch::Tensor centerbias = container.attr("cb").toTensor();
  
  centerbias = centerbias.clone().unsqueeze(0);

  torch::Tensor raw_image = image_utils::loadFile("/Users/drobotnik/test.jpg");
//#ifdef _DEBUG
  std::cout << "Loaded image of size:\n" << raw_image.sizes() << std::endl;

  raw_image = raw_image.slice(1, 0,768).slice(0,0,1024);
  std::cout << "Snipped image to:\n" << raw_image.sizes() << std::endl;

  torch::Tensor inference_img = torch::transpose(raw_image.clone(), 0, 2);
  inference_img = inference_img.unsqueeze(0);
  std::cout << "Inf img of size:\n" << inference_img.sizes() << std::endl;

  raw_image = torch::transpose(raw_image, 0,1);
  std::cout << "Transpose raw:\n" << raw_image.sizes() << std::endl;
  
  //torch::save(image, "/home/nd/img.tch");
//#endif
  raw_image = raw_image.clone();
  std::vector<torch::jit::IValue> input;

  
  input.push_back(inference_img);
  input.push_back(centerbias);

  std::cout << "Centerbias of size:\n" << centerbias.sizes() << std::endl;
  auto output = module.forward(input);

  // std::cout << "Output : " << output << std::endl;

  QApplication app(argc, argv);

  QMainWindow main_window;

  Ui::MainWindow ui;
  ui.setupUi(&main_window);

  main_window.show();

  QTimer timer;
  timer.setInterval(30);

  torch::Tensor saliency_map = output.toTensor().squeeze();
  saliency_map = -saliency_map / 25.0;
  saliency_map = saliency_map * 256.0;
  saliency_map = saliency_map.toType(at::ScalarType::Byte);
  std::cout << "MIN: " << saliency_map.min() << " and MAX: " << saliency_map.max() << " -- shape: " << saliency_map.sizes() << std::endl;
  //ui.raw->setTensor(frame);
  // torch::save(saliency_map, "/Users/drobotnik/test.tch");
  saliency_map = 255-saliency_map.t();

  int width = raw_image.size(0);
  int height = raw_image.size(1);
  unsigned char* img_data = raw_image.data_ptr<unsigned char>();
  std::cout << "Width: " << width << "; Height: " << height << "; data: " << reinterpret_cast<std::uintptr_t>(img_data) << std::endl;
  QImage image3(img_data, width, height, sizeof(unsigned char)*3*width, QImage::Format_RGB888);
  // image2.save("/Users/drobotnik/blah.jpg");

  ui.raw->setPixmap(QPixmap::fromImage(image3).scaled(ui.raw->size(), Qt::KeepAspectRatio));

  width = saliency_map.size(0);
  height = saliency_map.size(1);
  // QImage image2 = QImage(width, height, QImage::Format_Grayscale8);
  // std::cout<< "Image2 bytes per line: " << image2.bytesPerLine() << std::endl;
  unsigned char* src_data = saliency_map.data_ptr<unsigned char>();
  // quint8 *dst = (quint8*)(image2.bits());
  // memcpy(dst, src_data, width*height);
  // short* img_data = saliency_map.data_ptr<short>();
  std::cout << "Width: " << width << "; Height: " << height << "; data: " << reinterpret_cast<std::uintptr_t>(src_data) << std::endl;
  QImage image2(src_data, width, height, sizeof(unsigned char)*width, QImage::Format_Grayscale8);


  // QImage target(QSize(image2.height(), image2.width()), QImage::Format_Grayscale8);
  // QPainter painter(&target);
  // QTransform transf = painter.transform();
  // // transf.scale(-1, 1);
  // transf.rotate(90.0);
  // painter.setTransform(transf);
  // painter.drawImage(-image2.width(), 0, image2);
  QTransform myTransform;
  myTransform.rotate(90);
  QImage target = image2.transformed(myTransform).mirrored(true, false);

  // target.save("/Users/drobotnik/blah2.jpg");

  ui.saliency->setPixmap(QPixmap::fromImage(target).scaled(ui.saliency->size(), Qt::KeepAspectRatio));


//DeviceReader reader(ui.raw, ui.saliency);
  //QObject::connect(&timer, &QTimer::timeout, &reader, &DeviceReader::update);
  //timer.start();
  
  return app.exec();
}
