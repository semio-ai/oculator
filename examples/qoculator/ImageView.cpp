
#include "ImageView.hpp"

#include <opencv2/imgproc.hpp>

using namespace qoculator;

ImageView::ImageView(QWidget *parent)
  : QLabel(parent)
{
  setAlignment(Qt::AlignCenter);
  setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

ImageView::~ImageView()
{
}

void ImageView::setMat(const cv::Mat &mat)
{
  cv::Mat display;
  cv::cvtColor(mat, display, cv::COLOR_BGR2RGB);
  QImage image(display.data, display.cols, display.rows, display.step, QImage::Format_RGB888);
  setPixmap(QPixmap::fromImage(image).scaled(size(), Qt::KeepAspectRatio));
}