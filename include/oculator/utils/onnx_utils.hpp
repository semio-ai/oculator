#pragma once
#include <core/session/onnxruntime_cxx_api.h>
#include <functional_dag/dlpack.h>

namespace onnx_utils {

  typedef enum {
    CPU, CUDA, COREML
  } __provider;
  
  class __onnx_session_handles {
  public: 
    __onnx_session_handles();
    ~__onnx_session_handles();

    Ort::Env env;
    std::unique_ptr<Ort::Session> session;
    std::unique_ptr<Ort::MemoryInfo> memory_info;
    __provider provider;
  } ;
  
  /** 
   * Load a tensor from a file and return the result as a DLPack tensor
   */
  DLTensor loadTensorToDL(const char * const filename, const char * const paramName);

  /** 
   * Load a tensor from a file and return the result as a ONNX Runtime Value 
   */
  Ort::Value loadTensorToONNX(const char * const _filename, 
                              const char * const _paramName,
                              std::shared_ptr<__onnx_session_handles> _context);

  /**
   *  Load the weights and model from a file.
   *  
   *  @param filename The file path for the file to load
   *  @return The model that can be used for inference
   */
  std::shared_ptr<__onnx_session_handles> loadModel(std::string purpose, std::string file_path);

  /**
   *  Convert a DLPack Tensor to an ORT Tensor
   *  
   *  @param _tensor Tensor to convert
   *  @return That same tensor as an onnx runtime value
   */
  Ort::Value dlp_to_onnx(const DLTensor _tensor, 
                         const Ort::MemoryInfo &_memory_info, 
                         const std::vector<int64_t> *_shape_override = nullptr);
}
