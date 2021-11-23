/*
 * RunMR_AIM.h
 *
 *  Created on: Jan 7, 2016
 *      Author: siddharthadvani
 */

#ifndef RUNMR_AIM_H_
#define RUNMR_AIM_H_

#include "opencv2/opencv.hpp"

using namespace cv;
#include "AIM.h"

class AIM;

//Mat RunMR_AIM(String ImgFileName, int NumPyrLevels);
Mat RunMR_AIM (Mat &OriginalInputImage, Mat &Basis, AIM &mkAIM, int NumPyrLevels);

#endif /* RUNMR_AIM_H_ */
