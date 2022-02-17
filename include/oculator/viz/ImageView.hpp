#pragma once

#include <QtWidgets/QLabel>
#include <torch/torch.h>

namespace oculator
{
  typedef enum {
    VIZ_RGB, VIZ_HEATMAP
  } VizMode;

  class ImageView : public QLabel
  {
    Q_OBJECT
    
  public:
    ImageView(QWidget *parent = nullptr);
    ~ImageView();

    void setTensor(const torch::Tensor &tensor, VizMode mode);
  };
}

