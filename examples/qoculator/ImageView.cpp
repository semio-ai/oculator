
#include "ImageView.hpp"

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

/*void ImageView::setMat(const   torch::Tensor &mat)
{
  //cv::Mat display;
  //cv::cvtColor(mat, display, cv::COLOR_BGR2RGB);
  u_int32 width = mat.shape[1];
  u_int32 height = mat.shape[0];
 
  QImage image(  mat[:,:,0:3].data, width, height, sizeof(float)*3, QImage::Format_RGB888);
  setPixmap(QPixmap::fromImage(image).scaled(size(), Qt::KeepAspectRatio));
  }*/

// void ImageView::setMat(const   torch::Tensor &mat) {
//   //cv::Mat display;
//   //cv::cvtColor(mat, display, cv::COLOR_BGR2RGB);
