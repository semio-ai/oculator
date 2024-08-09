#include "oculator/utils/onnx_utils.hpp"
#include "oculator/utils/libnpy.hpp"
#include <exception>
#include <vector>
#include <string>
#include <numeric>

namespace onnx_utils {

  // We need to declare the constructor and destructor to trigger the destructors of our members
  __onnx_session_handles::__onnx_session_handles() {}
  __onnx_session_handles::~__onnx_session_handles() {}

  // Load a raw DLPack tensor from a numpy file. 
  DLTensor loadTensorToDL(const char * const _filename, const char * const _paramName) {
    std::vector<unsigned long> shape {};
    bool fortran_order;
    std::vector<float> data;
    
    const std::string path {_filename};
    npy::LoadArrayFromNumpy(path, shape, fortran_order, data);
    
    // Stored in row major
    DLTensor dest_struct;
    long nbytes = data.size()*sizeof(float);
    dest_struct.data = malloc(nbytes);
    std::copy(data.begin(), data.end(), (float*)dest_struct.data);

    dest_struct.dtype.code = DLDataTypeCode::kDLFloat;
    dest_struct.dtype.bits = 32;
    dest_struct.dtype.lanes = 1;
    dest_struct.device.device_type = DLDeviceType::kDLCPU;
    dest_struct.ndim = shape.size();
    dest_struct.shape = new int64_t[dest_struct.ndim];
    for(size_t i = 0;i < dest_struct.ndim;i++) dest_struct.shape[i] = shape[i];
    dest_struct.strides = NULL;
    dest_struct.byte_offset = 0;
    
    return dest_struct;
  }

  // Load a an ONNX tensor from a numpy file. 
  Ort::Value loadTensorToONNX(const char * const _filename, 
                              const char * const _paramName,
                              std::shared_ptr<__onnx_session_handles> _context) {
    std::vector<unsigned long> shape {};
    bool fortran_order;
    std::vector<float> data;
    
    const std::string path {_filename};
    npy::LoadArrayFromNumpy(path, shape, fortran_order, data);
    
    // Stored in row major
    u_long nbytes = data.size() * sizeof(float);
    float *data_raw = (float*)malloc(nbytes);
    std::copy(data.begin(), data.end(), data_raw);

    size_t n_dim = shape.size();
    int64_t *raw_shape = new int64_t[n_dim];
    for(size_t i = 0;i < n_dim;i++) raw_shape[i] = shape[i];

    return Ort::Value::CreateTensor<float>(*_context->memory_info, data_raw, data.size(), raw_shape, n_dim);
  }

  /**
   *  Load the weights and model from a file.
   *  
   *  @param filename The file path for the file to load
   *  @return The model that can be used for inference
   */
  std::shared_ptr<__onnx_session_handles> loadModel(std::string purpose, std::string file_path) {
    std::shared_ptr<__onnx_session_handles> context = 
                  std::shared_ptr<__onnx_session_handles>(new __onnx_session_handles);

    /******* Create ORT environment *******/
    context->env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_FATAL, purpose.c_str());
    
    /******* Create ORT session *******/
    // Set up options for session
    Ort::SessionOptions sessionOptions;
    context->provider = __provider::CPU;

    // If we have a GPU, enable GPU
    // if(GPU) {
      // sessionOptions.AppendExecutionProvider_CUDA(OrtCUDAProviderOptions{});
      // sessionOptions.(OrtCUDAProviderOptions{});
      // context->provider = __provider::CUDA;
    // }

    // If we have CoreML, enable CoreML
    // if(CoreML) {
      // Ort::ThrowOnError(OrtSessionOptionsAppendExecutionProvider_CoreML(sessionOptions, coreml_flags));
      // context->provider = __provider::COREML;
    // }

    // If we have NPU, enable the NPU 
    // if(NPU) {
      // Enable NPU
    // }

    switch (context->provider) {
    case __provider::CPU:
      context->memory_info = std::make_unique<Ort::MemoryInfo>(Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault));
      break;
    default:
      std::cerr << "Don't know how to allocate memory on device: " << context->provider << std::endl;
    }

    // Sets graph optimization level (Here, enable all possible optimizations)
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    // Create session by loading the onnx model
    context->session = std::make_unique<Ort::Session>(Ort::Session(context->env, file_path.c_str(), sessionOptions));
    
    return context;
  }

  Ort::Value dlp_to_onnx(const DLTensor _tensor, 
                         const Ort::MemoryInfo &_memory_info, 
                         const std::vector<int64_t> *_shape_override) {
    size_t tensor_size = std::accumulate(_tensor.shape, _tensor.shape+_tensor.ndim, 1, std::multiplies<int>());

    const int64_t* shape = _tensor.shape;
    size_t ndims = _tensor.ndim;
    if( _shape_override != nullptr ) {
      shape = _shape_override->data();
      ndims = _shape_override->size();
    }

    switch(_tensor.dtype.code) {
    case DLDataTypeCode::kDLInt: // signed integer
      return Ort::Value::CreateTensor<int8_t>(_memory_info, (int8_t *)_tensor.data,
                                               tensor_size, shape, ndims);
    case DLDataTypeCode::kDLUInt: // unsigned integer
          return Ort::Value::CreateTensor<uint8_t>(_memory_info, (uint8_t *)_tensor.data,
                                                    tensor_size, shape, ndims);
    case DLDataTypeCode::kDLFloat:
      return Ort::Value::CreateTensor<float>(_memory_info, (float *)_tensor.data,
                                             tensor_size, shape, ndims);
    default:
      std::cerr << "Unknown type when converting from DLTensor to ONNX\n";
    }
    return Ort::Value(nullptr);
  }
}