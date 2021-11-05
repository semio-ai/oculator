#ifndef _OCULATOR_MODEL_SALIENCY_HPP_
#define _OCULATOR_MODEL_SALIENCY_HPP_

#include <opencv2/core/core.hpp>

namespace oculator
{
  namespace model
  {
    namespace saliency
    {
      class Parameters
      {

      };

      class Model
      {
      public:
        Model(const Parameters &parameters);
        ~Model();

        cv::Mat operator () (const cv::Mat &image);

      private:
        Parameters parameters_;
      };
    }
  }
}

#endif