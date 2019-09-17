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
	*	Reads the image path and returns a uchar pointer that contains the RGB image.
	*/
	uchar* readRGBimage(const char* path);
	/**	@brief
	*	Reads the image path and returns a ushort pointer that contains the 16bit image.
	*/
	ushort* readGRAYimage(const char* path);
	/**	@brief
	*	Writes the RGB image to the path defined.
	*/
	void writeRGBimage(const uchar* pimg, const char* filename, const int width, const int height);
	/**	@brief
	*	Writes the RGBA image to the path defined.
	*/
	void writeRGBAimage(const uchar* pimg, const char* filename, const int width, const int height);
	/**	@brief
	*	Writes the 16bit image to the path defined.
	*/
	void writeGRAYimage(const ushort* pimg, const char* filename, const int width, const int height);

	/**	@brief 
	*	Uploads the images to the class and outputs the registered images.
	*	- t_img: is the thermal image of 16 bits
	*	- c_img: is the color image in 8 bits
	*	- d_img: is the depth image in 16 bits
	*   - registerImag: set to true to register the images
	*   - temp_min
	*   - temp_max
	*   - width
	*   - height
	*   - warpedImage
	*   - warpedDepth
	*/
	void update(const ushort* t_img, const uchar* c_img, const ushort* d_img, 
		bool registerImag, const float& temp_min, const float& temp_max, const float& opacity, int& width, int& height, 
		/*uchar* warpedImage, ushort* warpedDepth,*/ ushort* warpedThermal, uchar* warpedThermalCM, uchar* thermoOverColor);
	
	/**	@brief
	*	This function will return a pointer to a 8 bit 3 chanel color image size memory space reserved
	*/
	uchar * uchar3cImagePtr();

	/**	@brief
	*	This function will return a pointer to a 8 bit 1 chanel color image size memory space reserved
	*/
	uchar * uchar1cImagePtr();

	/**	@brief
	*	This function will return a pointer to a 8 bit 4 chanel color image size memory space reserved
	*/
	uchar * uchar4cImagePtr();

	/**	@brief
	*	This function will return a pointer to a 16 bit 1 chanel image size memory space reserved
	*/
	ushort * ushort1cImagePtr();

	
	void doColorMapWithMask(const cv::Mat& image, cv::Mat& msk, cv::Mat& image_out, bool normalize, const float& min, const float& max, const float& opacity, const int& cvColormap);

	void overlapImages(const cv::Mat &background, const cv::Mat &foreground, cv::Mat &output, const int x = 0, const int y = 0, float opacity = 1.0);
	/**	@brief
	*	This function will apply a colormap to the single channel image pointed by the pointer.
	*	- image: Pointer to the single channel image we want the colormap to be applied.
	*	- image_out: Pointer to the 4 channel image output with the colormap added.
	*	- min & max: The mask of the output image is defined between min & max. 
	*                The range is between 0 & 1 being 0 the min pixel value and 1 the max pixel value of the input image.
	*   - opacity: Range between 0 & 1 that defines the total opacity of the output image mask.
	*   - cvColormap: Integer between 0 & 12 that follows opencv 3.4.5 colormap order.
	*                 cv::COLORMAP_AUTUMN = 0, cv::COLORMAP_BONE = 1,    cv::COLORMAP_JET = 2,
	*                 cv::COLORMAP_WINTER = 3, cv::COLORMAP_RAINBOW = 4, cv::COLORMAP_OCEAN = 5,
	*                 cv::COLORMAP_SUMMER = 6, cv::COLORMAP_SPRING = 7,  cv::COLORMAP_COOL = 8,
	*                 cv::COLORMAP_HSV = 9,    cv::COLORMAP_PINK = 10,   cv::COLORMAP_HOT = 11,
	*                 cv::COLORMAP_PARULA = 12.  
	*/
	void doColorMap(const ushort* image, uchar* image_out, bool normalize, const int image_width, const int image_height, const float min, const float max, const float opacity, const int cvColormap);
	void overlapImages(const uchar* background, const uchar* foreground, uchar* output, const int image_width, const int image_height, const int x = 0, const int y = 0, float opacity = 1.0);

	void ushortToMat1c(const ushort * img, const int& width, const int& height, cv::Mat& out);
	void ucharToMat1c(const uchar * img, const int& width, const int& height, cv::Mat& out);
	void ucharToMat3c(const uchar * img, const int& width, const int& height, cv::Mat& out);
	void ucharToMat4c(const uchar * img, const int& width, const int& height, cv::Mat& out);

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
	cv::Mat warpedImage, warpedDepth, warpedThermal;

	// Functions
	void createRT(cv::Mat& R, cv::Mat& T, bool& invert, cv::Mat& RT);

	/**	@brief
	*	This function will fill the pixels with 0 value with the content-aware found inside the kernelSize filter
	*	- color: The image that needs to be filled
	*	- radius: radius of a circular neighborhood of each point inpainted
	*	- kernelSize: kernel size uset to dilate de image pixels to be able to create the mask that will define the inpainted pixels
	*	- result: the output image
	*/
	void refill(const cv::Mat& color, const double& radius, const int& kernelSize, cv::Mat& result);
	/*void showImages();*/
	void makeAlpha(const cv::Mat& src, const cv::Mat& mask, const float& opacity, cv::Mat& dst);
	void doColorMap(const cv::Mat& image, cv::Mat& image_out, bool normalize, const float& min, const float& max, const float& opacity, const int& cvColormap);

	

	void registerImages(/*cv::Mat& warpedImage, cv::Mat& warpedDepth,*/ cv::Mat& warpedThermal, cv::Mat& out_thermalMask);	

	void errorHandler(int err, std::string msg = "There is an error");

	

};

#endif // !LIBREGISTERTCD_H