//#include <oculator/model/saliency.hpp>

#include <cstdlib>
//#include <opencv2/highgui.hpp>
//#include <opencv2/videoio.hpp>

#include <cxxopts.hpp>
#include <variant>
#include <optional>

//#include "oculator/model/MR-AIM/AIM.h"
//#include "oculator/model/MR-AIM/RunMR_AIM.h"

//class AIM;


#include <torch/torch.h>
#include <iostream>

int main() {
  torch::Tensor tensor = torch::rand({2, 3});
  std::cout << tensor << std::endl;
}
/**
int main(int argc, char *argv[])
{
  using namespace oculator::model::saliency;

  cxxopts::Options options("saliency", "Saliency example");
  options.add_options()
    ("d,device", "Open Video Device (webcam)", cxxopts::value<int>())
    ("u,uri", "Open Video URI (file, stream)", cxxopts::value<std::string>())
    ("x,resolution_x", "Attempt to set the X resolution", cxxopts::value<std::uint32_t>())
    ("y,resolution_y", "Attempt to set the Y resolution", cxxopts::value<std::uint32_t>())
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
  if (result.count("device") && result.count("uri"))
  {
    std::cerr << "Error: --device and --uri are mutually exclusive" << std::endl;
    return EXIT_FAILURE;
  }
  else if (result.count("device"))
  {
    target = result["device"].as<int>();
  }
  else if (result.count("uri"))
  {
    target = result["uri"].as<std::string>();
  }
  else
  {
    std::cerr << "Error: --device or --uri must be specified" << std::endl;
    return EXIT_FAILURE;
  }

  // Parse resolution from command line
  std::optional<std::uint32_t> resolution_x;
  if (result.count("resolution_x")) resolution_x = result["resolution_x"].as<std::uint32_t>();
  
  std::optional<std::uint32_t> resolution_y;
  if (result.count("resolution_y")) resolution_y = result["resolution_y"].as<std::uint32_t>();
  
  // Create saliency model
  //Parameters parameters;
  //Model model(parameters);


  // Create the cv::VideoCapture with either a device ID or URI
  std::unique_ptr<cv::VideoCapture> cap;
  if (std::holds_alternative<int>(target))
  {
    cap = std::make_unique<cv::VideoCapture>(std::get<int>(target));
  }
  else
  {
    cap = std::make_unique<cv::VideoCapture>(std::get<std::string>(target));
  }

  if (resolution_x) cap->set(cv::CAP_PROP_FRAME_WIDTH, *resolution_x);
  if (resolution_y) cap->set(cv::CAP_PROP_FRAME_HEIGHT, *resolution_y);
  

  cv::Mat Basis;
  cv::Mat frame;
  cv::Mat saliency;

  std::string basis_file_name = "data/basis.yml";
  AIM mkAIM(basis_file_name, 19);
	Basis = mkAIM.LoadBasis(basis_file_name);
  int num_pyramid_levels = 0;
  //while (cv::waitKey(1) < 0)
  for(int i = 0;i < 15;i++)
  {
    *cap >> frame;

    if (frame.empty()) break;
    
    //cv::imshow("frame", frame);
    saliency = RunMR_AIM(frame, Basis, mkAIM, num_pyramid_levels);

    //cv::Mat saliency = model(frame);
    //cv::imshow("saliency", saliency);
  }

  return EXIT_SUCCESS;
}
**/
