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

void RegisterTCD::ushortToMat1c(const ushort * img, const int& width, const int& height, cv::Mat& out)
{
	ushort* p = new ushort[width * height * sizeof(ushort)];
	memcpy(p, img, width * height * sizeof(ushort));
	// Given a pointer img, and the size of the image, outputs a cv::Mat(height, width, CV_16UC1) object 
	out = cv::Mat(height, width, CV_16UC1, p)/*.clone()*/;
}

void RegisterTCD::ucharToMat3c(const uchar * img, const int& width, const int& height, cv::Mat& out)
{
	uchar* p = new uchar[width * height * sizeof(uchar) * 3];
	memcpy(p, img, width * height * sizeof(uchar) * 3);
	// Given a pointer img, and the size of the image, outputs a cv::Mat(height, width, CV_8UC3) object 
	out = cv::Mat(height, width, CV_8UC3, p)/*.clone()*/; // make a copy
}

void RegisterTCD::ucharToMat4c(const uchar * img, const int& width, const int& height, cv::Mat& out)
{
	uchar* p = new uchar[width * height * sizeof(uchar) * 4];
	memcpy(p, img, width * height * sizeof(uchar) * 4);
	// Given a pointer img, and the size of the image, outputs a cv::Mat(height, width, CV_8UC3) object 
	out = cv::Mat(height, width, CV_8UC4, p)/*.clone()*/; // make a copy
}

ushort* RegisterTCD::matToUshort(const cv::Mat& image)
{
	// Given a cv::Mat object returns it as a 16bit pointer
	int size = image.total();
	ushort* out = new ushort[size];  // destructor
	std::memcpy(out, image.data, size * sizeof(ushort));
	return out;
}

uchar* RegisterTCD::matToUchar(const cv::Mat& image)
{
	// Given a cv::Mat object returns it as a 8bit pointer
	int size = image.total() * image.elemSize();
	uchar* out = new uchar[size];  
	std::memcpy(out, image.data, size * sizeof(uchar));
	return out;
}

void RegisterTCD::makeAlpha(const cv::Mat& src, const cv::Mat& mask, const float& opacity, cv::Mat& dst)
{
	cv::Mat image, result;
	src.copyTo(image);
	std::vector<cv::Mat> src_channels;
	cv::split(image, src_channels);
	
	cv::Mat alpha;
	if (mask.empty() || mask.size().width != src.size().width || mask.size().height != src.size().height)
	{
		alpha = cv::Mat(image.size(), CV_8UC1, cv::Scalar(255));
	}
	else
	{
		mask.copyTo(alpha);
	}

	// Apply opacity
	alpha = alpha * opacity;

	src_channels.push_back(alpha);

	cv::merge(src_channels, dst);
}

void RegisterTCD::doColorMap(const cv::Mat& image, cv::Mat& image_out, const float& min, const float& max, const float& opacity, const int& cvColormap)
{
	// min, max & transparency must be between 0 and 1
	cv::Mat img, img_g;
	int mi = (int)255 * min;
	int ma = (int)255 * max;

	cv::Mat mask(image.size(), CV_8UC1, cv::Scalar(int(255 /** (1 - transparency)*/)));
	if (cvColormap != -1)
	{
		double imin, imax;
		cv::minMaxIdx(image, &imin, &imax);
		//image.convertTo(img, CV_8U, 255 / (imax - imin), -imin);
		image.convertTo(img, CV_8U, 1/256.0);
		//normalize(img, img, 255, 0, cv::NORM_MINMAX);

		// create the inverse mask
		for (int i = 0; i < img.size().height; i++)
		{
			for (int j = 0; j < img.size().width; j++)
			{
				if (!(mi <= img.at<uchar>(i, j) && img.at<uchar>(i, j) <= ma))
				{
					mask.at<uchar>(i, j) = 0;
				}
			}
		}
		applyColorMap(img, img, cvColormap);
	}
	else
	{
		image.copyTo(img);
		mask = mask /** (1 - transparency)*/;
	}

	//cv::cvtColor(img, img_g, CV_RGB2GRAY);

	makeAlpha(img, mask, opacity, image_out);	

}

void RegisterTCD::doColorMap(const ushort* image, uchar* image_out, const int image_width, const int image_height, const float min, const float max, const float opacity, const int cvColormap)
{
	cv::Mat img, img_out;
	ushortToMat1c(image, image_width, image_height, img);
	doColorMap(img, img_out, min, max, opacity, cvColormap);
	uchar * pimgage_out = matToUchar(img_out);
	std::memcpy(image_out, pimgage_out, image_width * image_height * sizeof(uchar) * 4);
	cv::Mat akan;
	ucharToMat4c(image_out, image_width, image_height, akan);
}

void RegisterTCD::overlapImages(const cv::Mat &background, const cv::Mat &foreground, cv::Mat &output, const int ox, const int oy, double opacity)
{
	background.copyTo(output);

	// start at the row indicated by location, or at row 0 if oy is negative.
	for (int y = std::max(oy, 0); y < background.rows; ++y) {
		int fY = y - oy; // because of the translation

		// we are done of we have processed all rows of the foreground image.
		if (fY >= foreground.rows)
			break;

		// start at the column indicated by location, or at column 0 if ox is negative.
		for (int x = std::max(ox, 0); x < background.cols; ++x) {
			int fX = x - ox; // because of the translation.

			// we are done with this row if the column is outside of the foreground image.
			if (fX >= foreground.cols)
				break;

			// determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
			double opacity_level = ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3]) / 255.;
			if (opacity >= 0.0 && opacity < 1.0)
				opacity_level *= opacity;

			// and now combine the background and foreground pixel, using the opacity, but only if opacity > 0.
			for (int c = 0; opacity_level > 0 && c < output.channels(); ++c) {
				unsigned char foregroundPx = foreground.data[fY * foreground.step + fX * foreground.channels() + c];
				unsigned char backgroundPx = background.data[y * background.step + x * background.channels() + c];
				output.data[y*output.step + output.channels()*x + c] = backgroundPx * (1. - opacity_level) + foregroundPx * opacity_level;
			}
		}
	}
}

void RegisterTCD::overlapImages(const uchar* background, const uchar* foreground, uchar* output, const int image_width, const int image_height, const int ox, const int oy, double opacity)
{
	cv::Mat bcg, frg, out;
	ucharToMat3c(background, image_width, image_height, bcg);
	ucharToMat4c(foreground, image_width, image_height, frg);
	overlapImages(bcg, frg, out, ox, oy, opacity);
	uchar * pout = matToUchar(out);
	std::memcpy(output, pout, image_width * image_height * sizeof(uchar) * 3);
	cv::Mat akan;
	ucharToMat3c(output, image_width, image_height, akan);
}

uchar* RegisterTCD::uchar3cImagePtr()
{
	uchar *ptr = new uchar[color_image_width * color_image_height * sizeof(uchar) * 3];
	return ptr;
}

uchar* RegisterTCD::uchar4cImagePtr()
{
	uchar *ptr = new uchar[color_image_width * color_image_height * sizeof(uchar) * 4];
	return ptr;
}

ushort* RegisterTCD::ushort1cImagePtr()
{
	ushort *ptr = new ushort[color_image_width * color_image_height * sizeof(ushort)];
	return ptr;
}


void RegisterTCD::update(const ushort* t_img, const uchar* c_img, const ushort* d_img, bool registerImag, const float& temp_min, const float& temp_max, int& width, int& height, uchar* warpedImage, ushort* warpedDepth)
{
	// Delete possible content from the variables
	t_frame.release();
	c_frame.release();
	d_frame.release();

	// Read array images and turn them into cv::Mat objects
	ushortToMat1c(t_img, thermal_image_width, thermal_image_height, t_frame);
	ucharToMat3c(c_img, color_image_width, color_image_height, c_frame);
	ushortToMat1c(d_img, color_image_width, color_image_height, d_frame);

	// Throw error if images are empty
	if (t_frame.empty() || c_frame.empty() || c_frame.empty())
	{
		errorHandler(ERR_IMG_READ);
	}

	// Check if it's possible to register
	
	cv::Mat wI, wD;
	cv::Mat wT;
	if (RT.empty() && color_cameraMatrix.empty() && thermal_cameraMatrix.empty() && thermal_distCoeff.empty() && registerImag)
	{
		errorHandler(ERR_CAN_NOT_REGISTER, "Can't reconstruct useing this constructor.");
	}
	else
	{
		registerImages(wI, wD);
		registerImagesThermalToColor(wT);
		width = wI.size().width;
		height = wI.size().height;
	}

	if (wI.empty() || wD.empty())
	{
		errorHandler(ERR_CAN_NOT_REGISTER, "Can't register the images.");
	}

	uchar* pwI = matToUchar(wI);
	ushort* pwD = matToUshort(wD);

	memcpy(warpedImage, pwI, width * height * sizeof(uchar) * 3);
	memcpy(warpedDepth, pwD, width * height * sizeof(ushort));

	
	cv::Mat x, y;
	ucharToMat3c(warpedImage, 640, 480, x);
	ushortToMat1c(warpedDepth, 640, 480, y);

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
RegisterTCD::registerImagesThermalToColor(cv::Mat& out_warpedThermal)
{
	cv::Mat image = c_frame;
	cv::Mat depth = d_frame;
	cv::Mat thermo = t_frame;
	cv::Size imagePlanec2(thermal_image_width, thermal_image_height);
	cv::Size imagePlanec1(color_image_width, color_image_height);
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
	
	warpedDepth.create(imagePlanec2, CV_32FC1);
	warpedDepth = cv::Scalar(FLT_MAX);

	cv::Mat mask(imagePlanec1, CV_8UC1);
	mask = cv::Scalar(0);

	warpedThermal.create(imagePlanec1, CV_16UC1);
	warpedThermal = cv::Scalar(0);

	cv::Mat cloud;
	cv::rgbd::depthTo3d(depth, Kc1, cloud);
	cv::Mat cldSplit[3];
	split(cloud, cldSplit);

	cv::Mat warpedCloud, warpedImagePoints;
	perspectiveTransform(cloud, warpedCloud, Rt);
	projectPoints(warpedCloud.reshape(3, 1), cv::Mat(3, 1, CV_32FC1, cv::Scalar(0)), cv::Mat(3, 1, CV_32FC1, cv::Scalar(0)), Kc2, Dc2, warpedImagePoints);
	warpedImagePoints = warpedImagePoints.reshape(2, cloud.rows);
	//cv::Rect r(0, 0, imagePlanec2.width, imagePlanec2.height);
	cv::Rect r(0, 0, imagePlanec2.width, imagePlanec2.height);
	for (int y = 0; y < cloud.rows; y++)
	{
		for (int x = 0; x < cloud.cols; x++)
		{
			cv::Point p = warpedImagePoints.at<cv::Point2f>(y, x);
			if(!isnan(cldSplit[0].at<float>(y, x)))
			{
				if (r.contains(p))
				{
					warpedThermal.at<ushort>(y, x) = thermo.at<ushort>(p.y, p.x);
					mask.at<uchar>(y, x) = 255;
				}
			}
			
		}
	}

	cv::merge(result, warpedImage);
	warpedDepth.setTo(std::numeric_limits<float>::quiet_NaN(), warpedDepth > 100000);
	warpedDepth.convertTo(warpedDepth, CV_16UC1);

	out_warpedThermal = warpedThermal;

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