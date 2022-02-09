#ifndef _OCULATOR_DEVICE_READER_HPP_
#define _OCULATOR_DEVICE_READER_HPP_

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include "ImageView.hpp"

namespace oculator
{
  struct DeviceReader : public QObject
  {
    Q_OBJECT
    
  public:
    DeviceReader(ImageView *const target, ImageView *const saliency, QObject *const parent = nullptr);
    ~DeviceReader();

    void update();



  private:
    ImageView *target_;
    ImageView *saliency_;
    //cv::VideoCapture capture_;

    //cv::Mat basis_;

    //std::unique_ptr<AIM> mk_aim_;
    
  };
}

#endif
