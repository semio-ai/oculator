#include <IL/il.h>
#include <IL/ilu.h>
#include <torch/torch.h>
#include <exception>

namespace image_utils {
    // Tracking whether OpenIL is already loaded
    static bool __IL_LOADED__ = false;
    /**
         Function to load an image from a file. Should support all the file formats that
        DevIL supports. For more info on DevIL, check out their website:
        http://openil.sourceforge.net/

        Github location (for bug fixes):
        https://github.com/DentonW/DevIL

    */
    torch::Tensor loadFile(const char * const filename) {
        int32_t width, height, depth;
        int size;

        if(!__IL_LOADED__) {
            ilInit();
            iluInit();
            __IL_LOADED__ = true;
        }

        // Initialize the image 
        ILuint ImgId = 0;
        ilGenImages(1, &ImgId);
        ilBindImage(ImgId);

        // Load the image
        ilLoadImage(filename);
        // Check the error 
        ILenum error = ilGetError();
        if(error != IL_NO_ERROR) {
            ilDeleteImage(ImgId);
            throw std::runtime_error(std::string("Unable to load file ") + filename);
        }

#ifdef _DEBUG
        std::cout << "error enum: " << error << std::endl;
        std::cout << "Error: " << iluErrorString(error) << std::endl;
#endif

        ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

#ifdef _DEBUG
        error = ilGetError();
        std::cout << "error enum: " << error << std::endl;
#endif
        // Get a pointer to the image data
        ILubyte *data = ilGetData();

        width = ilGetInteger(IL_IMAGE_WIDTH);
        height = ilGetInteger(IL_IMAGE_HEIGHT);
        depth = ilGetInteger(IL_IMAGE_DEPTH);
        size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);

#ifdef _DEBUG
        std::cout << "File has width: " << width << std::endl;
        std::cout << "File has height: "  << height << std::endl;
        std::cout << "File has depth: " << depth << std::endl;
        std::cout << "Data size: " << size << std::endl;
#endif
        // Create the torch tensor to return and fill it from OpenIL. 
        torch::Tensor imageData = torch::zeros({height,width,3}, torch::TensorOptions().dtype(torch::kUInt8));
        std::memcpy(imageData.data_ptr(), data, width*height*3);

#ifdef _DEBUG
        std::cout << "AFTER CPY" << std::endl;
#endif
        // Cleanup
        ilDeleteImage(ImgId);

        return imageData;
    }


}
