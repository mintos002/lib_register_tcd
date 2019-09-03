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
	void update(const ushort* t_img, const uchar* c_img, const ushort* d_img, bool registerImag, const float& temp_min, const float& temp_max, int& width, int& height, uchar* warpedImage, ushort* warpedDepth);
	
	/**	@brief
	*	This function will return a pointer to a 8 bit 3 chanel color image size memory space reserved
	*/
	uchar * ucharImagePtr();

	/**	@brief
	*	This function will return a pointer to a 16 bit 1 chanel image size memory space reserved
	*/
	ushort * ushortImagePtr();

	/**	@brief 
	*	This function will fill the pixels with 0 value with the content-aware found inside the kernelSize filter
	*	- color: The image that needs to be filled
	*	- radius: radius of a circular neighborhood of each point inpainted
	*	- kernelSize: kernel size uset to dilate de image pixels to be able to create the mask that will define the inpainted pixels
	*	- result: the output image
	*/
	void refill(const cv::Mat& color, const double& radius, const int& kernelSize, cv::Mat& result);
	void showImages();




	void makeAlpha(const cv::Mat& src, const cv::Mat& mask, const float& opacity, cv::Mat& dst);
	void doColorMap(const cv::Mat& image, cv::Mat& image_out, const float& min, const float& max, const float& opacity, const int& cvColormap);
	void overlapImages(const cv::Mat &background, const cv::Mat &foreground, cv::Mat &output, cv::Point2i location = cv::Point2i(0,0), double opacity = 1.0);

	void ushortToMat(const ushort * img, const int& width, const int& height, cv::Mat& out);
	void ucharToMat(const uchar * img, const int& width, const int& height, cv::Mat& out);
	ushort* matToUshort(const cv::Mat& image);
	uchar* matToUchar(const cv::Mat& image);

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
	

	

	void registerImages(cv::Mat& warpedImage, cv::Mat& warpedDepth);
	

	void errorHandler(int err, std::string msg = "There is an error");

};

#endif // !LIBREGISTERTCD_H