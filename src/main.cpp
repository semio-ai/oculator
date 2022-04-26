/**
 *   ____  ____  _     _     ____  _____  ____  ____ 
 *  /  _ \/   _\/ \ /\/ \   /  _ \/__ __\/  _ \/  __\
 *  | / \||  /  | | ||| |   | / \|  / \  | / \||  \/|
 *  | \_/||  \_ | \_/|| |_/\| |-||  | |  | \_/||    /
 *  \____/\____/\____/\____/\_/ \|  \_/  \____/\_/\_\
 * 
 * Oculator's main entry point. Really this just triggers the model loading and the video feed. 
 * 
 * @author: ndepalma@alum.mit.edu
 * @license: MIT License
 */                                             



#include <iostream>
#include <cstdint>
#include <cxxopts.hpp>
#include <variant>
#include <chrono>
#include <thread>

#include <torch/torch.h>
#include "oculator/viz/DeviceReader.hpp"

#if (VIZ == 1)
#include <QtWidgets/QApplication>
#include <QtCore/QTimer>
#include "viz/ui_mainwindow.h"
#endif

#include "oculator/utils/image_utils.h"
#include "oculator/utils/torch_utils.h"

#if (VIZ == 1)
using namespace oculator;
#endif

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

using namespace std::chrono_literals;

typedef enum {
  LOOP_FOREVER,
  LOOP_UNTIL_COMPLETE,
  SINGLE_FRAME
} PLAY_MODE;

int main(int argc, char *argv[])
{
  PLAY_MODE how_long_to_run = SINGLE_FRAME;
  cxxopts::Options options{"oculator", "A framework for real time intelligent vision for social robotics"};
  options.add_options()
    ("d,device", "Open Video Device {webcam}", cxxopts::value<int>())
    ("u,uri", "Open Video URI {file, stream}", cxxopts::value<std::string>())
    ("i,image", "Open image URI {file}", cxxopts::value<std::string>())
    ("h,help", "Print help");

  typedef std::variant<int, std::string> Target;

  const auto result = options.parse(argc, argv);

  // Show help when -h or --help is specified
  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    return EXIT_SUCCESS;
  }

  // Parse video target from command line
  Target target(0);
  std::string filename;
  std::shared_ptr<DeviceReader> device;
  if (result.count("device") + result.count("uri") + result.count("image") > 1)
  {
    std::cerr << "Error: --device, --image, and --uri are mutually exclusive" << std::endl;
    return EXIT_FAILURE;
  }
  else if (result.count("device"))
  {
    target = result["device"].as<int>();
    how_long_to_run = LOOP_FOREVER;
  }
  else if (result.count("uri"))
  {
    target = result["uri"].as<std::string>();
    filename = std::get<std::string>(target);
    device = std::make_shared<DeviceReader>(filename);
    how_long_to_run = LOOP_UNTIL_COMPLETE;
  }
  else if (result.count("image"))
  {
    target = result["image"].as<std::string>();
    filename = std::get<std::string>(target);
    std::cout << "Opening image target " << filename << std::endl;
    how_long_to_run = SINGLE_FRAME;
  }
  else
  {
    std::cerr << "Error: --device or --uri must be specified" << std::endl;
    return EXIT_FAILURE;
  }

  switch(how_long_to_run) {
    case LOOP_UNTIL_COMPLETE:
      device->waitForCompletion();
      break;
    case SINGLE_FRAME:
      {
        // Load the raw image to process
        torch::Tensor raw_image = image_utils::loadFile(filename.c_str());
        raw_image = raw_image.slice(1, 0,768).slice(0,0,1024);
        
        // Prepare the raw image for inference
        torch::Tensor inference_img = torch::transpose(raw_image.clone(), 0, 2);
        inference_img = inference_img.unsqueeze(0);
      }
       break;    
    default:
      std::cerr << "Error: case not handled yet.\n";
  }

  // Load the model and weights
  // torch::jit::script::Module module = torch_utils::loadModel("/Users/drobotnik/projects/oculator/models/deepgaze.pth");
  
  
  // Load the bias input (it's a constant for DeepGaze)
  // torch::Tensor centerbias = torch_utils::loadTensor("/Users/drobotnik/projects/oculator/models/centerbias.pt", "cb");
  // centerbias = centerbias.clone().unsqueeze(0);

  

  // // Perform the inference
  // torch::Tensor saliency_map = perform_deep_gaze_inference(module, inference_img, centerbias);

#if (VIZ == 1)
  // Construct the GUI 
  // QApplication app(argc, argv);

  // QMainWindow main_window;

  // Ui::MainWindow ui;
  // ui.setupUi(&main_window);

  // main_window.show();

  // QTimer timer;
  // timer.setInterval(30);
#endif
 
  // Transpose the raw image for viewing
  // raw_image = raw_image.clone();
  // raw_image = torch::transpose(raw_image, 0,1);

#if (VIZ == 1)
  // // Show the raw image
  // ui.raw->setTensor(raw_image, VIZ_RGB);

  // // Show the saliency image
  // ui.saliency->setTensor(saliency_map, VIZ_HEATMAP);
  // std::this_thread::sleep_for(1s);
  // std::cout<< "Go\n";

#endif
#if (VIZ == 1)
  // return app.exec();
#endif
  return 0;
}
