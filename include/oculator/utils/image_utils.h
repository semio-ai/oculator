#pragma once
#include <torch/torch.h>

namespace image_utils {
  torch::Tensor loadFile(const char * const filename);
}
