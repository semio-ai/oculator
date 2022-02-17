#pragma once

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
  };
}