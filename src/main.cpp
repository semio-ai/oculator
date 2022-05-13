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
using namespace std::placeholders;

torch::jit::script::Module module;
torch::Tensor centerbias;

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
using namespace torch::indexing;
void updateViz(torch::Tensor new_image, Ui::MainWindow ui) {
  ui.raw->setTensor(new_image, VIZ_RGB);
  // at::Tensor image_a = new_image.index({Slice(), Slice(), 0});
  // at::Tensor image_b = at::upsample_nearest_exact2d(image_a, {768,1024}, 1.0);
  // at::Tensor image_b = at::upsample_linear2d(image_a, {768, 1024}, false);
  // at::Tensor image_b = new_image.resize_({1024, 768, 3});
  //  at::Tensor image_b = at::upsample_bilinear2d(new_image, {1024, 768,3}, false);
  //////// WORKING
  //  at::Tensor image_b = at::upsample_bilinear2d(new_image.toType(at::ScalarType::Double).unsqueeze(0), {1024, 768}, false);
  // First make it greyscale 
  at::Tensor float_img = new_image.toType(at::ScalarType::Double);
  float_img = torch::transpose(float_img, 0, 2);
  float_img = torch::permute(float_img, {0, 2, 1});
  // at::Tensor greyscale = torch::sum(float_img, {-1}) / (255.0*3.0);
  // at::Tensor grey_input = at::upsample_bilinear2d(greyscale.unsqueeze(0).unsqueeze(0), {1024, 768}, false);
  at::Tensor saliency_input = at::upsample_bilinear2d(float_img.toType(at::ScalarType::Float).unsqueeze(0), {768, 1024}, false);
  // float_img.index({Slice(), Slice(), 0});
    //  std::cout << "Shape: " << saliency_input.size(0) << " " << saliency_input.size(1) << " " 
    //         << saliency_input.size(2) << " " << saliency_input.size(3) 
    //         << std::endl;
  // at::Tensor image_b = at::upsample_nearest2d(new_image.unsqueeze(0), {1024, 768});
  // torch::nn::UpsampleImpl upsampler(torch::nn::UpsampleOptions().size(std::vector<int64_t>({1024,768})).mode(torch::kLinear).align_corners(false));
  // at::Tensor image_b = upsampler.forward(new_image);
  
  // image_b = image_b.index({0});

  //  at::Tensor image_b = at::upsample_nearest_exact2d(new_image, {1024, 768,3}, false);
   
  
  //torch.Size([1, 3, 768, 1024])
     
  // torch::Tensor inference_img = torch::transpose(image_b.clone(), 0, 2);
  // inference_img = inference_img.unsqueeze(0);

        std::cout << "Shape input: " << saliency_input.size(0) << " " << saliency_input.size(1) << " " 
            << saliency_input.size(2) << " " << saliency_input.size(3) 
            << std::endl;
        std::cout << "centerbias: " << centerbias.size(0) << " " << centerbias.size(1) << " " 
            << centerbias.size(2) << std::endl;
   torch::Tensor saliency_map = perform_deep_gaze_inference(module, saliency_input, centerbias);
    std::cout << "output Shape: " << saliency_map.size(0) << " " << saliency_map.size(1) << " " 
            << std::endl;
   at::Tensor saliency_output = at::upsample_bilinear2d(saliency_map.toType(at::ScalarType::Double).unsqueeze(0).unsqueeze(0), {new_image.size(1), new_image.size(0)}, false);
   saliency_output = saliency_output.toType(at::ScalarType::Byte).squeeze().t();
            std::cout << "output Shape resize: " << saliency_output.size(0) << " " << saliency_output.size(1) << " " 
            << std::endl;
   ui.saliency->setTensor(saliency_output, VIZ_HEATMAP);
 }

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

#if (VIZ == 1)
  // Construct the GUI 
  QApplication app(argc, argv);

  QMainWindow main_window;

  Ui::MainWindow ui;
  ui.setupUi(&main_window);

  std::cout<< "Show!\n";
  main_window.show();

  // QTimer timer;
  // timer.setInterval(10);
  // auto timer = new QTimer(&main_window);
  // timer->setInterval(10ms);
  // main_window.connect(timer, &QTimer::timeout, &main_window, []{});
  // timer->start();
#endif

  // Load the model and weights
  module = torch_utils::loadModel("/Users/drobotnik/projects/oculator/models/deepgaze.pth");
  
  
  // Load the bias input (it's a constant for DeepGaze)
  centerbias = torch_utils::loadTensor("/Users/drobotnik/projects/oculator/models/centerbias.pt", "cb");
  centerbias = centerbias.clone().unsqueeze(0);


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
    device = std::make_shared<DeviceReader>(filename, std::bind(updateViz, _1, ui));
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
      // device->waitForCompletion();
      break;
    case SINGLE_FRAME:
      {
        // Load the raw image to process
        torch::Tensor raw_image = image_utils::loadFile(filename.c_str());
        raw_image = raw_image.slice(1, 0,768).slice(0,0,1024);
        
        // Prepare the raw image for inference
        torch::Tensor inference_img = torch::transpose(raw_image.clone(), 0, 2);
        inference_img = inference_img.unsqueeze(0);
        
        // Perform the inference
        torch::Tensor saliency_map = perform_deep_gaze_inference(module, inference_img, centerbias);

        // Transpose the raw image for viewing
        raw_image = raw_image.clone();
        raw_image = torch::transpose(raw_image, 0,1);

#if (VIZ == 1)
        // Show the raw image
        ui.raw->setTensor(raw_image, VIZ_RGB);

        // Show the saliency image
        ui.saliency->setTensor(saliency_map, VIZ_HEATMAP);
        std::this_thread::sleep_for(1s);
        std::cout<< "Go\n";
#endif
      }
      break;    
    default:
      std::cerr << "Error: case not handled yet.\n";
  }
 
#if (VIZ == 1)
  return app.exec();
#else
  return 0;
#endif
}
