#include <torch/torch.h>
#include <torch/script.h>
#include <exception>

namespace torch_utils {
    /**
        Load a raw tensor from a pytorch file. 
    */
    torch::Tensor loadTensor(const char * const filename, const char * const paramName) {
        torch::jit::script::Module container = torch::jit::load(filename);
        return container.attr(paramName).toTensor();
    }

    /**
     *  Load the weights and model from a file.
     *  
     *  @param filename The file path for the file to load
     *  @return The model that can be used for inference
     */
    torch::jit::script::Module loadModel(const char * const filename) {
        torch::jit::script::Module module;
        try {
            // Deserialize the ScriptModule from a file using torch::jit::load().
            module = torch::jit::load(filename);
        }
        catch (const c10::Error& e) {
            std::cerr<< e.msg() << std::endl;
            throw std::runtime_error(std::string("Unable to load model file ") + filename);
        }
        return module;
    }
}