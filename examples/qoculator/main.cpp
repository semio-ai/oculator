#include <iostream>
#include <cstdint>

#include <torch/torch.h>

#include <QtWidgets/QApplication>
#include <QtCore/QTimer>

#include "ui_mainwindow.h"
#include "DeviceReader.hpp"
#include "oculator/utils/image_utils.h"
#include "oculator/utils/torch_utils.h"

using namespace qoculator;

torch::Tensor perform_deep_gaze_inference(torch::jit::script::Module model, 
                                          torch::Tensor inference_img, 
                                          torch::Tensor centerbias) {
  // Append the input into a giant vector
  std::vector<torch::jit::IValue> input;
  input.push_back(inference_img);
  input.push_back(centerbias);

  // Perform the inference
  auto output = model.forward(input);
  
  //Extract the saliency map out
  torch::Tensor saliency_map = output.toTensor().squeeze();
  //Scale it back and project it into a byte-based greyscale image
  saliency_map = -saliency_map / 25.0;
  saliency_map = saliency_map * 255.0;
  saliency_map = saliency_map.toType(at::ScalarType::Byte);

  // and for some reason it's inverted
  saliency_map = 255-saliency_map.t();

#ifdef _DEBUG
  torch::save(saliency_map, "/Users/drobotnik/test.tch");
#endif
  
  return saliency_map;
}

int main(int argc, char *argv[])
{
  // Load the model and weights
  torch::jit::script::Module module = torch_utils::loadModel("/Users/drobotnik/projects/oculator/models/deepgaze.pth");
  
  // Load the raw image to process
  torch::Tensor raw_image = image_utils::loadFile("/Users/drobotnik/test.jpg");
  raw_image = raw_image.slice(1, 0,768).slice(0,0,1024);
  
  // Load the bias input (it's a constant for DeepGaze)
  torch::Tensor centerbias = torch_utils::loadTensor("/Users/drobotnik/projects/oculator/models/centerbias.pt", "cb");
  centerbias = centerbias.clone().unsqueeze(0);

  // Prepare the raw image for inference
  torch::Tensor inference_img = torch::transpose(raw_image.clone(), 0, 2);
  inference_img = inference_img.unsqueeze(0);

  // Perform the inference
  torch::Tensor saliency_map = perform_deep_gaze_inference(module, inference_img, centerbias);

  // Construct the GUI 
  QApplication app(argc, argv);

  QMainWindow main_window;

  Ui::MainWindow ui;
  ui.setupUi(&main_window);

  main_window.show();

  QTimer timer;
  timer.setInterval(30);

  // Transpose the raw image for viewing
  raw_image = raw_image.clone();
  raw_image = torch::transpose(raw_image, 0,1);

  // Show the raw image
  ui.raw->setTensor(raw_image, VIZ_RGB);

  // Show the saliency image
  ui.saliency->setTensor(saliency_map, VIZ_HEATMAP);

  //DeviceReader reader(ui.raw, ui.saliency);
  //QObject::connect(&timer, &QTimer::timeout, &reader, &DeviceReader::update);
  //timer.start();
  
  return app.exec();
}
