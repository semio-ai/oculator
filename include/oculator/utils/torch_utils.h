#pragma once
#include <torch/torch.h>

namespace torch_utils {
  /** 
   * Load a tensor from a file.
   */
  torch::Tensor loadTensor(const char * const filename, const char * const paramName);

  /** 
   * Load the model and weights from a file.
   */
  torch::jit::script::Module loadModel(const char * const filename);
}
