#ifndef _QOCULATOR_DEVICE_READER_HPP_
#define _QOCULATOR_DEVICE_READER_HPP_

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include "ImageView.hpp"

#include "oculator/model/MR-AIM/RunMR_AIM.h"


namespace qoculator
{
  struct DeviceReader : public QObject
  {
    Q_OBJECT
  public:
    DeviceReader(ImageView *const target, ImageView *const saliency, QObject *const parent = nullptr);
    void update();



  private:
    ImageView *target_;
    ImageView *saliency_;
    cv::VideoCapture capture_;

    cv::Mat basis_;

    std::unique_ptr<AIM> mk_aim_;
    
  };
}

#endif