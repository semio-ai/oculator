#ifndef _QOCULATOR_IMAGE_VIEW_HPP_
#define _QOCULATOR_IMAGE_VIEW_HPP_

#include <QtWidgets/QLabel>
#include <opencv2/core.hpp>

namespace qoculator
{
  class ImageView : public QLabel
  {
    Q_OBJECT
  public:
    ImageView(QWidget *parent = nullptr);
    ~ImageView();

    void setMat(const cv::Mat &mat);
  };
}

#endif