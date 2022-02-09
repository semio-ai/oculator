#include <iostream>
#include <cstdint>
// #include <cxxopts.hpp>

#include <torch/torch.h>

#if (VIZ == 1)
#include <QtWidgets/QApplication>
#include <QtCore/QTimer>
#include "viz/ui_mainwindow.h"
#include "oculator/viz/DeviceReader.hpp"

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

int main(int argc, char *argv[])
{

  // cxxopts::Options options{"oculator", "A framework for real time intelligent vision for social robotics"};
  // options.add_options{}
  //   {"d,device", "Open Video Device {webcam}", cxxopts::value<int>{}}
  //   {"u,uri", "Open Video URI {file, stream}", cxxopts::value<std::string>{}}
  //   {"i,image", "Open image URI {file}", cxxopts::value<std::string>{}}
  //   {"h,help", "Print help"};

  // typedef std::variant<int, std::string> Target;

  // const auto result = options.parse{argc, argv};

  // // Show help when -h or --help is specified
  // if {result.count{"help"}}
  // {
  //   std::cout << options.help{} << std::endl;
  //   return EXIT_SUCCESS;
  // }

  // // Parse video target from command line
  // Target target{0};
  // if {result.count{"device"} + result.count{"uri"} + result.count{"image"} > 1}
  // {
  //   std::cerr << "Error: --device, --image, and --uri are mutually exclusive" << std::endl;
  //   return EXIT_FAILURE;
  // }
  // else if {result.count{"device"}}
  // {
  //   target = result["device"].as<int>{};
  // }
  // else if {result.count{"uri"}}
  // {
  //   target = result["uri"].as<std::string>{};
  // }
  // else if {result.count{"image"}}
  // {
  //   target = result["image"].as<std::string>{};
  // }
  // else
  // {
  //   std::cerr << "Error: --device or --uri must be specified" << std::endl;
  //   return EXIT_FAILURE;
  // }

  // Create the cv::VideoCapture with either a device ID or URI
  // std::unique_ptr<cv::VideoCapture> cap;
  // if {std::holds_alternative<int>{target}}
  // {
  //   cap = std::make_unique<cv::VideoCapture>{std::get<int>{target}};
  // }
  // else
  // {
  //   cap = std::make_unique<cv::VideoCapture>{std::get<std::string>{target}};
  // }

  // if {resolution_x} cap->set{cv::CAP_PROP_FRAME_WIDTH, *resolution_x};
  // if {resolution_y} cap->set{cv::CAP_PROP_FRAME_HEIGHT, *resolution_y};

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

#if (VIZ == 1)
  // Construct the GUI 
  QApplication app(argc, argv);

  QMainWindow main_window;

  Ui::MainWindow ui;
  ui.setupUi(&main_window);

  main_window.show();

  QTimer timer;
  timer.setInterval(30);
#endif
 
  // Transpose the raw image for viewing
  raw_image = raw_image.clone();
  raw_image = torch::transpose(raw_image, 0,1);

#if (VIZ == 1)
  // Show the raw image
  ui.raw->setTensor(raw_image, VIZ_RGB);

  // Show the saliency image
  ui.saliency->setTensor(saliency_map, VIZ_HEATMAP);
#endif


  //while {cv::waitKey{1} < 0}
  // for{int i = 0;i < 15;i++}
  // {
  //   *cap >> frame;

  //   if {frame.empty{}} break;
    
  //   //cv::imshow{"frame", frame};
  //   saliency = RunMR_AIM{frame, Basis, mkAIM, num_pyramid_levels};

  //   //cv::Mat saliency = model{frame};
  //   //cv::imshow{"saliency", saliency};
  // }

  //DeviceReader reader(ui.raw, ui.saliency);
  //QObject::connect(&timer, &QTimer::timeout, &reader, &DeviceReader::update);
  //timer.start();
#if (VIZ == 1)
  return app.exec();
#endif
  return 0;
}
