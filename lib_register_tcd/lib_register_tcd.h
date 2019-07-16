#pragma once
#ifndef LIBREGISTERTCD_H
#define LIBREGISTERTCD_H

#ifndef LIBREGISTERTCD_EXPORTS

#define API_LIBREGISTERTCD __declspec(dllexport)
#else
#define API_LIBREGISTERTCD __declspec(dllimport)

#endif


#include "stdio.h" // C Standard Input/Output library.
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/rgbd.hpp"
#include "opencv2/photo.hpp"

#define ERR_IMG_READ 0
#define ERR_FILE_NF 1
#define ERR_CAN_NOT_REGISTER 2


class API_LIBREGISTERTCD RegisterTCD
{
public:
	RegisterTCD();
	/**	@brief 
	*	Creates the variables needed for the class with a .xml file (output of calibration_extrinsics_tcd app).
	*	- file_path: is the path of the intrinsics and extrinsics parameters.
	*	- invertRT: if set to true it will invert the RT matrix
	*/
	RegisterTCD(const char& file_path, bool invertRT);
	/**	@brief
	*	Creates the variables needed for the class with a manual input.
	*	- thermo_w: 
	*	- thermo_h: 
	*	- color_w:
	*	- color_h:
	*	- rsdepth_scale:
	*	- thermal_K33:
	*	- thermal_D15:
	*	- color_K33:
	*	- color_D15:
	*	- R33:
	*	- T31:
	*	- invertRT:
	*/
	RegisterTCD(const int& thermo_w, const int& thermo_h, const int& color_w, const int& color_h,
		const float& rsdepth_scale, const double* thermal_K33[9], const double* thermal_D15[5],
		const double* color_K33[9], const double* color_D15[5], const double* R33[9], const double* T31[3], bool& invertRT);
	~RegisterTCD();

	/**	@brief 
	*	Uploads the images to the class.
	*	- t_img: is the thermal image of 16 bits
	*	- c_img: is the color image in 8 bits
	*	- d_img: is the depth image in 16 bits
	*/
	void update(ushort* t_img, uchar* c_img, ushort* d_img, bool& registerImag, double& temp_min, double& temp_max, cv::Mat& warpedImage, cv::Mat& warpedDepth);
	
	/**	@brief 
	*	This function will fill the pixels with 0 value with the content-aware found inside the kernelSize filter
	*	- color: The image that needs to be filled
	*	- radius: radius of a circular neighborhood of each point inpainted
	*	- kernelSize: kernel size uset to dilate de image pixels to be able to create the mask that will define the inpainted pixels
	*	- result: the output image
	*/
	void refill(const cv::Mat& color, const double& radius, const int& kernelSize, cv::Mat& result);
	void showImages();

private:
	// Input file data variables
	int thermal_image_width;
	int thermal_image_height;
	int color_image_width;
	int color_image_height;
	float depth_scale;
	cv::Mat thermal_cameraMatrix , thermal_distCoeff;
	cv::Mat color_cameraMatrix, color_distCoeff;
	cv::Mat R, T, RT;
	cv::Mat t_frame, c_frame, d_frame;

	cv::Mat warpedImage, warpedDepth;

	// Functions
	void createRT(cv::Mat& R, cv::Mat& T, bool& invert, cv::Mat& RT);
	void ushortToMat(ushort * img, int& width, int& height, cv::Mat& out);
	void ucharToMat(uchar * img, int& width, int& height, cv::Mat& out);
	void matToUshort(cv::Mat& image, ushort* out);
	void matToUchar(cv::Mat& image, uchar* out);

	void registerImages(cv::Mat& warpedImage, cv::Mat& warpedDepth);

	void errorHandler(int err, std::string msg = "There is an error");

};

#endif // !LIBREGISTERTCD_H