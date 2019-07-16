// lib_register_tcd.cpp : Define las funciones exportadas de la aplicación DLL.
//
#include "stdafx.h"
#include "lib_register_tcd.h"

// UTILS


// CONSTRUCTORS
RegisterTCD::RegisterTCD()
{
	/*thermal_image_width;
	thermal_image_height;
	color_image_width;
	color_image_height;
	depth_scale;
	thermal_cameraMatrix;
	thermal_distCoeff;
	color_cameraMatrix;
	color_distCoeff;
	R;
	T;
	RT;*/
}

RegisterTCD::RegisterTCD(const char& file_path, bool invertRT)
{
	// Read config values
	cv::String path(&file_path);

	cv::FileStorage fs(path, cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		std::string msg;
		msg = "ERROR: Could not open the file: " + path;
		errorHandler(ERR_FILE_NF, msg);
	}

	fs["thermal_image_Width"] >> thermal_image_width;
	fs["thermal_image_Height"] >> thermal_image_height;
	fs["color_image_Width"] >> color_image_width;
	fs["color_image_Height"] >> color_image_height;
	fs["depth_scale"] >> depth_scale;

	fs["Thermal_Camera_Matrix"] >> thermal_cameraMatrix;
	fs["Thermal_Distortion_Coefficients"] >> thermal_distCoeff;

	fs["Color_Camera_Matrix"] >> color_cameraMatrix;
	fs["Color_Distortion_Coefficients"] >> color_distCoeff;
	fs["R"] >> R;
	fs["T"] >> T;
	fs.release();

	// Create Rt assuming that T is in mm 
	createRT(R, T, invertRT, RT);

}

RegisterTCD::RegisterTCD(const int& thermo_w, const int& thermo_h, const int& color_w, const int& color_h,
	const float& rsdepth_scale, const double* thermal_K33[9], const double* thermal_D15[5],
	const double* color_K33[9], const double* color_D15[5], const double* R33[9], const double* T31[3], bool& invertRT)
{
	thermal_image_width = thermo_w;
	thermal_image_height = thermo_h;
	color_image_width = color_w;
	color_image_height = color_h;
	depth_scale = rsdepth_scale;

	thermal_cameraMatrix = cv::Mat(3, 3, CV_32FC1, thermal_K33).clone();
	thermal_distCoeff = cv::Mat(1, 5, CV_32FC1, thermal_D15).clone();

	color_cameraMatrix = cv::Mat(3, 3, CV_32FC1, color_K33).clone();
	color_distCoeff = cv::Mat(1, 5, CV_32FC1, color_D15).clone();
	R = cv::Mat(3, 3, CV_32FC1, R33).clone();
	T = cv::Mat(3, 1, CV_32FC1, T31).clone();

}

// DESTRUCTORS
RegisterTCD::~RegisterTCD()
{
	/*delete[] & thermal_image_width;
	delete[] & thermal_image_height;
	delete[] & color_image_width;
	delete[] & color_image_height;
	delete[] & depth_scale;

	delete[] & thermal_cameraMatrix;
	delete[] & thermal_distCoeff;

	delete[] & color_cameraMatrix;
	delete[] & color_distCoeff;
	delete[] & R;
	delete[] & T;
	delete[] & RT;*/
}

void RegisterTCD::createRT(cv::Mat& R, cv::Mat& T, bool& invert, cv::Mat& RT)
{
	// Create Rt assuming that T is in mm 
	cv::Mat o = (cv::Mat_<double>(1, 4) << 0, 0, 0, 1);
	cv::Mat Tm = T.clone();
	Tm.at<double>(0, 0) = T.at<double>(0, 0) * 0.001;
	Tm.at<double>(1, 0) = T.at<double>(1, 0) * 0.001;
	Tm.at<double>(2, 0) = T.at<double>(2, 0) * 0.001;
	cv::hconcat(R, Tm, RT);
	cv::vconcat(RT, o, RT);
	// Invert RT matrix if defined so
	if (invert)
	{
		RT = RT.inv();
	}
}

void RegisterTCD::ushortToMat(ushort * img, int& width, int& height, cv::Mat& out)
{
	// Given a pointer img, and the size of the image, outputs a cv::Mat(height, width, CV_16UC1) object 
	out = cv::Mat(height, width, CV_16UC1, img).clone();
}

void RegisterTCD::ucharToMat(uchar * img, int& width, int& height, cv::Mat& out)
{
	// Given a pointer img, and the size of the image, outputs a cv::Mat(height, width, CV_8UC3) object 
	out = cv::Mat(height, width, CV_8UC3, img).clone(); // make a copy
}

void RegisterTCD::matToUshort(cv::Mat& image, ushort* out)
{
	// Given a cv::Mat object returns it as a 16bit pointer
	int size = image.total();
	out = new ushort[size];  // destructor
	std::memcpy(out, image.data, size * sizeof(ushort));
}

void RegisterTCD::matToUchar(cv::Mat& image, uchar* out)
{
	// Given a cv::Mat object returns it as a 8bit pointer
	int size = image.total() * image.elemSize();
	out = new uchar[size];  
	std::memcpy(out, image.data, size * sizeof(uchar));
}


void 
RegisterTCD::update(ushort* t_img, uchar* c_img, ushort* d_img, bool& registerImag, double& temp_min, double& temp_max, cv::Mat& warpedImage, cv::Mat& warpedDepth)
{
	// Delete possible content from the variables
	t_frame.release();
	c_frame.release();
	d_frame.release();

	// Read array images and turn them into cv::Mat objects
	ushortToMat(t_img, thermal_image_width, thermal_image_height, t_frame);
	ucharToMat(c_img, color_image_width, color_image_height, c_frame);
	ushortToMat(d_img, color_image_width, color_image_height, d_frame);

	// Throw error if images are empty
	if (t_frame.empty() || c_frame.empty() || c_frame.empty())
	{
		errorHandler(ERR_IMG_READ);
	}

	// Check if it's possible to register
	
	cv::Mat wI, wD;
	if (RT.empty() && color_cameraMatrix.empty() && thermal_cameraMatrix.empty() && thermal_distCoeff.empty() && registerImag)
	{
		errorHandler(ERR_CAN_NOT_REGISTER, "Can't reconstruct useing this constructor.");
	}
	else
	{
		registerImages(wI, wD);
	}
	
}

void
RegisterTCD::registerImages(cv::Mat& out_warpedImage, cv::Mat& out_warpedDepth)
{
	cv::Mat image = c_frame;
	cv::Mat depth = d_frame;
	cv::Size imagePlanec2(thermal_image_width, thermal_image_height);
	cv::Mat Rt = RT;
	cv::Mat Kc1 = color_cameraMatrix;
	cv::Mat Kc2 = thermal_cameraMatrix;
	cv::Mat Dc2 = thermal_distCoeff;

	CV_Assert(!image.empty());
	CV_Assert(image.type() == CV_8UC3);

	CV_Assert(depth.size() == image.size());
	CV_Assert(depth.type() == CV_16UC1);

	warpedImage.create(imagePlanec2, CV_8UC3);
	warpedImage = cv::Scalar(0);

	cv::Mat bgr[3], cb, cg, cr;
	split(image, bgr);

	cb = cv::Mat::zeros(imagePlanec2, CV_8UC1);
	cg = cv::Mat::zeros(imagePlanec2, CV_8UC1);
	cr = cv::Mat::zeros(imagePlanec2, CV_8UC1);

	std::vector<cv::Mat> result;
	result.push_back(cb);
	result.push_back(cg);
	result.push_back(cr);

	cv::Mat mask(imagePlanec2, CV_8UC1);
	mask = cv::Scalar(0);

	warpedDepth.create(imagePlanec2, CV_32FC1);
	warpedDepth = cv::Scalar(FLT_MAX);

	cv::Mat cloud;
	cv::rgbd::depthTo3d(depth, Kc1, cloud);

	cv::Mat warpedCloud, warpedImagePoints;
	perspectiveTransform(cloud, warpedCloud, Rt);
	projectPoints(warpedCloud.reshape(3, 1), cv::Mat(3, 1, CV_32FC1, cv::Scalar(0)), cv::Mat(3, 1, CV_32FC1, cv::Scalar(0)), Kc2, Dc2, warpedImagePoints);
	warpedImagePoints = warpedImagePoints.reshape(2, cloud.rows);
	cv::Rect r(0, 0, imagePlanec2.width, imagePlanec2.height);
	for (int y = 0; y < cloud.rows; y++)
	{
		for (int x = 0; x < cloud.cols; x++)
		{
			cv::Point p = warpedImagePoints.at<cv::Point2f>(y, x);
			if (r.contains(p))
			{
				float curDepth = warpedDepth.at<float>(p.y, p.x);
				float newDepth = warpedCloud.at<cv::Point3f>(y, x).z;
				if (newDepth < curDepth && newDepth > 0)
				{
					//warpedImage.at<uchar>(p.y, p.x) = image.at<uchar>(y, x);
					result[0].at<uchar>(p.y, p.x) = bgr[0].at<uchar>(y, x);
					result[1].at<uchar>(p.y, p.x) = bgr[1].at<uchar>(y, x);
					result[2].at<uchar>(p.y, p.x) = bgr[2].at<uchar>(y, x);

					warpedDepth.at<float>(p.y, p.x) = newDepth * 1000;

					mask.at<uchar>(p.y, p.x) = 255;

				}
			}
		}
	}
	cv::merge(result, warpedImage);
	warpedDepth.setTo(std::numeric_limits<float>::quiet_NaN(), warpedDepth > 100000);
	warpedDepth.convertTo(warpedDepth, CV_16UC1);

	out_warpedDepth = warpedDepth;
	out_warpedImage = warpedImage;

}

void
RegisterTCD::showImages()
{

}

void 
RegisterTCD::refill(const cv::Mat& color, const double& radius, const int& kernelSize, cv::Mat& result)
{
	CV_Assert(!color.empty());
	CV_Assert(color.type() == CV_8UC3 || color.type() == CV_8UC1);
	cv::Mat image, image_gray, blur;
	color.copyTo(image);
	// create border
	cv::copyMakeBorder(image, image, 1, 1, 1, 1, cv::BORDER_REPLICATE);

	image.copyTo(image_gray);
	// If image is rgb convert to single channel
	if (image.type() == CV_8UC3)
	{
		cv::cvtColor(image, image_gray, cv::COLOR_RGB2GRAY);
	}

	cv::Mat kernel(kernelSize, kernelSize, CV_32F);
	//cv::Mat kernel(5, 5, CV_32F);
	kernel = cv::Scalar(1);

	cv::morphologyEx(image_gray, blur, cv::MORPH_DILATE, kernel);

	cv::Mat mask(image_gray.size(), CV_8UC1);
	mask = cv::Scalar(0);

	for (int i = 0; i < image_gray.size().height; i++)
	{
		for (int j = 0; j < image_gray.size().width; j++)
		{
			if (image_gray.at<uchar>(i, j) == 0 && !blur.at<uchar>(i, j) == 0)
			{
				mask.at<uchar>(i, j) = 255;
			}
		}
	}
	cv::Mat out;
	inpaint(image, mask, out, radius, CV_INPAINT_TELEA);
	cv::Rect r(1, 1, out.cols - 2, out.rows - 2);
	result = out(r);
}

// ERROR HANDLER
void RegisterTCD::errorHandler(int err, std::string msg)
{
	printf("%s\n", msg.c_str());
	switch (err)
	{
	case ERR_IMG_READ:
		throw "Could not read the images propertly.";
		break;
	case ERR_FILE_NF:
		throw "File not found.";
		break;
	case ERR_CAN_NOT_REGISTER:
		throw "Could not register the images";
		break;

	default:
		break;
	}
}