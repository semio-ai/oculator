
#include "functional_dag/lib_utils.h"
#include "oculator/utils/onnx_utils.hpp"


namespace oculator{
  class OpenPose : public fn_dag::module_transmit {
  public:
  OpenPose() {
    // Load the model and weights
    module = onnx_utils::loadModel("OpenPose Inference", 
                                   "/Users/drobotnik/projects/oculator/models/human-pose-estimation.onnx");
  }
  
  DLTensor *update(const DLTensor *image) {
    // size_t imagesize = image->shape[0]*image->shape[1]*image->shape[2];
    // int64_t shapeo[] = {1, image->shape[0], image->shape[1], image->shape[2]};
    // printf(" shape i: %d %d %d\n", image->shape[0], image->shape[1], image->shape[2]);
    // // Append the input into a giant vector
    // std::vector<Ort::Value> inputTensors;
    // Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
    // inputTensors.push_back(Ort::Value::CreateTensor<uint8_t>(memoryInfo, (uint8_t *)image->data,
    //                         imagesize,shapeo, 4));
    // inputTensors.push_back(Ort::Value::CreateTensor<float>(memoryInfo, (float *)centerbias.data,
    //                         768*1024,centerbias.shape, 1));


    // Ort::TypeInfo outputTypeInfo = module->GetOutputTypeInfo(0);
    // auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
    // ONNXTensorElementDataType outputType = outputTensorInfo.GetElementType();
    // std::vector<int64_t> mOutputDims = outputTensorInfo.GetShape();

    // std::vector<Ort::Value> outputTensors;
    // size_t outputTensorSize = mOutputDims[0]*mOutputDims[1]*mOutputDims[2];
    // std::vector<float> outputTensorValues(outputTensorSize);
    // outputTensors.push_back(Ort::Value::CreateTensor<float>(memoryInfo, outputTensorValues.data(), outputTensorSize,mOutputDims.data(), mOutputDims.size()));
    // std::vector<const char*> inputNames{"x", "centerbias"};
    // std::vector<const char*> outputNames{"saliency"};
    // // // Perform the inference
    // module->Run(Ort::RunOptions{nullptr}, 
    //             inputNames.data(),
    //             inputTensors.data(), 2, 
    //             outputNames.data(),
    //             outputTensors.data(), 1);
    
    //Extract the saliency map out
    // torch::Tensor saliency_map = output.toTensor().squeeze();
    //Scale it back and project it into a byte-based greyscale image
    // saliency_map = -saliency_map / 25.0;
    // saliency_map = saliency_map * 255.0;
    // saliency_map = saliency_map.toType(at::ScalarType::Byte);

    // // and for some reason it's inverted
    // saliency_map = 255-saliency_map.t();

// #ifdef _DEBUG
//   torch::save(saliency_map, "/Users/drobotnik/test.tch");
// #endif
  
    // return saliency_map;
    return nullptr;
  }
  private:
  std::shared_ptr<Ort::Session> module;

};


}

#pragma GCC diagnostic ignored "-Wreturn-type-c-linkage"

#define DL_EXPORT __attribute__ ((visibility ("default")))

extern "C" DL_EXPORT std::string get_simple_description() {
  return std::string("This package will perform bottom-up saliency on an image and output the saliency map.");
}

extern "C" DL_EXPORT std::string get_detailed_description() {
  return std::string("At the moment, this library performs deepgaze inference version 1. This is due to the heavy requirements of model 2 and the way we intend to handle the saliency map downstream.");
}

extern "C" DL_EXPORT std::string get_name() {
  return std::string("DeepGaze Version 1");
}

extern "C" DL_EXPORT long get_serial_guid() {
  return 952;
}

extern "C" DL_EXPORT bool is_source() {
  return false;
}

extern "C" DL_EXPORT shared_ptr<fn_dag::lib_options> get_options() {
  shared_ptr<fn_dag::lib_options> options(new fn_dag::lib_options());
  
  return options;
}

extern "C" DL_EXPORT fn_dag::module *get_module(const fn_dag::lib_options *options) {
  fn_dag::module_handler *viewer_out = new fn_dag::module_handler(new oculator::OpenPose());
  return (fn_dag::module *)viewer_out;
}