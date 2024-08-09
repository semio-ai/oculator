
#include "functional_dag/lib_utils.h"
#include "oculator/utils/onnx_utils.hpp"

// torch::Tensor perform_deep_gaze_inference(torch::jit::script::Module model, 
//                                           torch::Tensor inference_img, 
//                                           torch::Tensor centerbias) {
//   // Append the input into a giant vector
//   std::vector<torch::jit::IValue> input;
//   input.push_back(inference_img);
//   input.push_back(centerbias);

//   // Perform the inference
//   auto output = model.forward(input);
  
//   //Extract the saliency map out
//   torch::Tensor saliency_map = output.toTensor().squeeze();
//   //Scale it back and project it into a byte-based greyscale image
//   saliency_map = -saliency_map / 25.0;
//   saliency_map = saliency_map * 255.0;
//   saliency_map = saliency_map.toType(at::ScalarType::Byte);

//   // and for some reason it's inverted
//   saliency_map = 255-saliency_map.t();

// #ifdef _DEBUG
//   torch::save(saliency_map, "/Users/drobotnik/test.tch");
// #endif
  
//   return saliency_map;
// }




// void updateViz(torch::Tensor new_image, Ui::MainWindow ui) {
//     ui.raw->setTensor(new_image, VIZ_RGB);
//     // First make it greyscale 
//     at::Tensor float_img = new_image.toType(at::ScalarType::Double);
//     float_img = torch::transpose(float_img, 0, 2);
//     float_img = torch::permute(float_img, {0, 2, 1});
//     at::Tensor saliency_input = at::upsample_bilinear2d(float_img.toType(at::ScalarType::Float).unsqueeze(0), {768, 1024}, false);

//     torch::Tensor saliency_map = perform_deep_gaze_inference(module, saliency_input, centerbias);
//     at::Tensor saliency_output = at::upsample_bilinear2d(saliency_map.toType(at::ScalarType::Double).unsqueeze(0).unsqueeze(0), {new_image.size(1), new_image.size(0)}, false);
//     saliency_output = saliency_output.toType(at::ScalarType::Byte).squeeze().t();
//     ui.saliency->setTensor(saliency_output, VIZ_HEATMAP);
// }

namespace oculator{
  class DeepGaze : public fn_dag::module_transmit {
  private:
    std::shared_ptr<onnx_utils::__onnx_session_handles> m_context;
    DLTensor m_centerbias;
    std::vector <int64_t> m_center_bias_shape;
    // Ort::Value m_centerbias;
    std::vector <int64_t> m_image_shape;
    
    
  public:
  DeepGaze(std::string model_name) {
    // Load the bias input (it's a constant for DeepGaze)
    m_centerbias = onnx_utils::loadTensorToDL("/Users/drobotnik/projects/oculator/models/centerbias.npy", "cb");
    
     
    // centerbias = centerbias.clone().unsqueeze(0);

    // Load the model and weights
    m_context = onnx_utils::loadModel(model_name, "/Users/drobotnik/projects/oculator/models/deepgaze.onnx");

     // input image 
    Ort::TypeInfo inputTypeInfo = m_context->session->GetInputTypeInfo(0);
    auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
    m_image_shape = inputTensorInfo.GetShape();

    inputTypeInfo = m_context->session->GetInputTypeInfo(1);
    inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
    m_center_bias_shape = inputTensorInfo.GetShape();
    // m_centerbias = onnx_utils::dlp_to_onnx(dlBias, *m_context->memory_info, &center_bias_shape);
  }
  
// node = onnx.helper.make_node(
//     "Resize",
//     inputs=["X", "", "scales"],
//     outputs=["Y"],
//     mode="nearest",
// )
// Ort::Op::Create 	( 	const OrtKernelInfo *  	info,
// 		const char *  	op_name,
// 		const char *  	domain,
// 		int  	version,
// 		const char **  	type_constraint_names,
// 		const ONNXTensorElementDataType *  	type_constraint_values,
// 		size_t  	type_constraint_count,
// 		const OpAttr *  	attr_values,
// 		size_t  	attr_count,
// 		size_t  	input_count,
// 		size_t  	output_count 
// 	) 	
  DLTensor *update(const DLTensor *image) {
    std::cout << "Got an image!\n";
    // Append the input into a giant vector
    std::vector<Ort::Value> inputTensors;

    // input image 
    // Ort::TypeInfo inputTypeInfo = m_context->session->GetInputTypeInfo(0);
    // auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
    // const std::vector <int64_t> t = inputTensorInfo.GetShape();
    inputTensors.push_back(onnx_utils::dlp_to_onnx(*image, *m_context->memory_info, &m_image_shape));

    // std::vector<int64_t> dism = inputTensorInfo.GetShape();
    // for(auto d : dism) {
    //   std::cout << "D: " << d << std::endl;
    // }

    // Centerbias
    // int64_t cbshape[] = {1, 768, 1024};
    // inputTensors.push_back(Ort::Value::CreateTensor<float>(*m_context->memory_info, (float *)m_centerbias.data,
    //                         768*1024,cbshape, 3));
    // inputTypeInfo = m_context->session->GetInputTypeInfo(1);
    // inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
    // const std::vector <int64_t> q = inputTensorInfo.GetShape();
    inputTensors.push_back(onnx_utils::dlp_to_onnx(m_centerbias, *m_context->memory_info, &m_center_bias_shape));
    // inputTensors.push_back(&m_centerbias);

    // Ort::Value inputTensors[] = {onnx_utils::dlp_to_onnx(*image, *m_context->memory_info, &m_image_shape), 
    //                             m_centerbias};

    Ort::TypeInfo outputTypeInfo = m_context->session->GetOutputTypeInfo(0);
    auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
    ONNXTensorElementDataType outputType = outputTensorInfo.GetElementType();
    std::vector<int64_t> mOutputDims = outputTensorInfo.GetShape();

    std::vector<Ort::Value> outputTensors;
    size_t outputTensorSize = mOutputDims[0]*mOutputDims[1]*mOutputDims[2];
    std::vector<float> outputTensorValues(outputTensorSize);
    outputTensors.push_back(Ort::Value::CreateTensor<float>(*m_context->memory_info, outputTensorValues.data(), outputTensorSize,mOutputDims.data(), mOutputDims.size()));
    std::vector<const char*> inputNames{"x", "onnx::Reshape_1"};
    std::vector<const char*> outputNames{"117"};

    // Perform the inference
    m_context->session->Run(Ort::RunOptions{nullptr}, 
                            inputNames.data(),
                            inputTensors.data(), 2, 
                            outputNames.data(),
                            outputTensors.data(), 1);
    //Extract the saliency map out
    // outputTensors.push_back(Ort::Value::CreateTensor<float>(*m_context->memory_info, outputTensorValues.data(), outputTensorSize,mOutputDims.data(), mOutputDims.size()));
    const float *saliency_map = outputTensors.at(0).GetTensorData<float>();

    DLTensor *outputImage = new DLTensor;
    // long nbytes = outputTensors.at(0).GetCount();

    std::cout << outputTensors.at(0).GetTensorTypeAndShapeInfo().GetElementType() << std::endl;
    std::vector<int64_t> shape = outputTensors.at(0).GetTensorTypeAndShapeInfo().GetShape();
    for(auto d : shape) {
      std::cout << "D: " << d << std::endl;
    }

    long nelements = outputTensors.at(0).GetTensorTypeAndShapeInfo().GetElementCount();
    outputImage->data = malloc(nelements);
    for( int i = 0;i < nelements;i++) {
      float f = saliency_map[i];
      f = -f / 25.0;
      f = f * 255.0;
      uint8_t b = (uint8_t)f;

      // and for some reason it's inverted
      ((uint8_t*)outputImage->data)[i] = 255-b;
    }
    // std::copy(saliency_map, saliency_map+nelements, (float*)outputImage->data);

    outputImage->dtype.code = DLDataTypeCode::kDLUInt;
    outputImage->dtype.bits = sizeof(uint8_t);
    outputImage->dtype.lanes = 1;
    outputImage->device.device_type = DLDeviceType::kDLCPU;
    outputImage->ndim = outputTensors.at(0).GetTensorTypeAndShapeInfo().GetDimensionsCount();
    outputImage->shape = new int64_t[outputImage->ndim];
    std::cout << "copying shape: ";

    for(size_t i = 0;i < outputImage->ndim;i++) {
      int64_t shape_i = outputTensors.at(0).GetTensorTypeAndShapeInfo().GetShape().at(i);
      std::cout << shape_i << " ";
      outputImage->shape[i] = shape_i;
    }
    std::cout << "\n";
    outputImage->strides = NULL;
    outputImage->byte_offset = 0;





    // outputTensors.at(0).
    // torch::Tensor saliency_map = output.toTensor().squeeze();
    // Scale it back and project it into a byte-based greyscale image
    // saliency_map = -saliency_map / 25.0;
    // saliency_map = saliency_map * 255.0;
    // saliency_map = saliency_map.toType(at::ScalarType::Byte);

    // // and for some reason it's inverted
    // saliency_map = 255-saliency_map.t();

// #ifdef _DEBUG
//   torch::save(saliency_map, "/Users/drobotnik/test.tch");
// #endif
  
    // return saliency_map;
    return outputImage;
  }


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
  return 951;
}

extern "C" DL_EXPORT bool is_source() {
  return false;
}

extern "C" DL_EXPORT shared_ptr<fn_dag::lib_options> get_options() {
  shared_ptr<fn_dag::lib_options> options(new fn_dag::lib_options());
  
  return options;
}

extern "C" DL_EXPORT fn_dag::module *get_module(const fn_dag::lib_options *options) {
  fn_dag::module_handler *viewer_out = new fn_dag::module_handler(new oculator::DeepGaze(get_name()));
  return (fn_dag::module *)viewer_out;
}