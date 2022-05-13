#pragma once
/**
 *   ____  ____  _     _     ____  _____  ____  ____ 
 *  /  _ \/   _\/ \ /\/ \   /  _ \/__ __\/  _ \/  __\
 *  | / \||  /  | | ||| |   | / \|  / \  | / \||  \/|
 *  | \_/||  \_ | \_/|| |_/\| |-||  | |  | \_/||    /
 *  \____/\____/\____/\____/\_/ \|  \_/  \____/\_/\_\
 * 
 * ImageView displays RGB and heat maps for the data being streamed.
 * 
 * @author: ndepalma@alum.mit.edu
 * @license: MIT License
 */ 

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
    /**
     * Constructor for the visualizer
     * 
     * @param parent Parent widget (container)
     */
    ImageView(QWidget *parent = nullptr);

    /**
     * Deconstructor (used for cleanup)
     */
    ~ImageView();

    /**
     * setTensor
     * 
     * @param tensor The image to display
     * @param mode How to display it
     */
    void setTensor(const torch::Tensor &tensor, const VizMode mode);
  };
}

