#pragma once
#include <torch/torch.h>

namespace image_utils {
  /** 
   * Load an image from a file and return a torch tensor.
   */
  torch::Tensor loadFile(const char * const filename);


}
