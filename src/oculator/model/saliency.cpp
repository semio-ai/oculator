#include "oculator/model/saliency.hpp"

using namespace oculator;
using namespace oculator::model;
using namespace oculator::model::saliency;

Model::Model(const Parameters &parameters)
  : parameters_(parameters)
{
}

Model::~Model()
{
}

cv::Mat Model::operator () (const cv::Mat &image)
{
  // Dummy implementation
  return image.clone();
}