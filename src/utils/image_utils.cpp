#include <IL/il.h>
#include <IL/ilu.h>
#include <torch/torch.h>

namespace image_utils {
  /**
     Function to load an image from a file. Should support all the file formats that
     DevIL supports. For more info on DevIL, check out their website:
     http://openil.sourceforge.net/

     Github location (for bug fixes):
     https://github.com/DentonW/DevIL

   */
  torch::Tensor loadFile(const char * const filename) {
    //torch::Tensor image;
    int32_t width, height, depth;
    int size;

    ilInit();
    iluInit();

    ILuint ImgId = 0;
    ilGenImages(1, &ImgId);
    ilBindImage(ImgId);

    ilLoadImage(filename);
    
    ILenum error = ilGetError();

#ifdef _DEBUG
    std::cout << "error enum: " << error << std::endl;
    std::cout << "Error: " << iluErrorString(error) << std::endl;
#endif

    ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
    error = ilGetError();

#ifdef _DEBUG
    std::cout << "error enum: " << error << std::endl;
#endif

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

    torch::Tensor imageData = torch::zeros({height,width,3}, torch::TensorOptions().dtype(torch::kUInt8));
    std::memcpy(imageData.data_ptr(), data, width*height*3);

#ifdef _DEBUG
    std::cout << "AFTER CPY" << std::endl;
#endif

    return imageData;
  }
}
