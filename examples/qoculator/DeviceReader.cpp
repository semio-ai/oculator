#include "DeviceReader.hpp"

#include <QtCore/QDebug>


using namespace qoculator;

DeviceReader::DeviceReader(ImageView *const target, ImageView *const saliency, QObject *const parent)
  : QObject(parent)
  , target_(target)
  , saliency_(saliency)
{
  capture_.open(0);
  capture_.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  capture_.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

  mk_aim_ = std::make_unique<AIM>("data/basis.yml", 19);
  basis_ = mk_aim_->LoadBasis("data/basis.yml");
}

void DeviceReader::update()
{
  if (!capture_.grab())
  {
    qWarning() << "Failed to grab frame";
    return;
  }

  cv::Mat frame;
  if (!capture_.retrieve(frame))
  {
    qWarning() << "Failed to retrieve frame";
    return;
  }

  target_->setMat(frame);

  saliency_->setMat(RunMR_AIM(frame, basis_, *mk_aim_, 0));

}