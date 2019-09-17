// Test_lib_register.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#include "pch.h"

#include <iostream>
#include "lib_register_tcd.h"


//uchar * matToUchar(cv::Mat image)
//{
//	int size = image.total() * image.elemSize();
//	uchar * bytes = new uchar[size];  // you will have to delete[] that later
//	std::memcpy(bytes, image.data, size * sizeof(uchar));
//	return bytes;
//}
//
//cv::Mat ucharToMat(uchar * bytes, int width, int height)
//{
//	cv::Mat image = cv::Mat(height, width, CV_8UC3, bytes).clone(); // make a copy
//	return image;
//}
//
//ushort * matToUshort(cv::Mat image)
//{
//	int size = image.total();
//	ushort * bytes = new ushort[size];  // you will have to delete[] that later
//	std::memcpy(bytes, image.data, size * sizeof(ushort));
//	return bytes;
//}
//
//cv::Mat ushortToMat(ushort * bytes, int width, int height)
//{
//	cv::Mat image = cv::Mat(height, width, CV_16UC1, bytes).clone(); // make a copy
//	return image;
//}

int main()
{
	char path[] = "C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_imagesx640_80\\spaceing24_extrinsics_640_80.xml";

	/*cv::Mat t_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_imagesx640_80\\thermo\\t_2019711561964745974.png", cv::IMREAD_ANYDEPTH);
	cv::Mat c_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_imagesx640_80\\color\\c_2019711561964745974.png", cv::IMREAD_COLOR);
	cv::Mat d_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_imagesx640_80\\depth\\d_2019711561964745974.png", cv::IMREAD_ANYDEPTH);*/

	/*cv::Mat t_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\jetson\\load\\thermo\\t_2019711561978893955.png", cv::IMREAD_ANYDEPTH);
	cv::Mat c_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\jetson\\load\\color\\c_2019711561978893955.png", cv::IMREAD_COLOR);
	cv::Mat d_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\jetson\\load\\depth\\d_2019711561978893955.png", cv::IMREAD_ANYDEPTH);*/

	cv::Mat t_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\jetson\\no_load\\thermo\\t_2019711561968282099.png", cv::IMREAD_ANYDEPTH);
	cv::Mat c_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\jetson\\no_load\\color\\c_2019711561968282099.png", cv::IMREAD_COLOR);
	cv::Mat d_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\jetson\\no_load\\depth\\d_2019711561968282099.png", cv::IMREAD_ANYDEPTH);

	

	// Create RegisterTCD object
	RegisterTCD reg(*path, true);
	// Test conversions mat to pointer
	uchar* c = reg.matToUchar(c_img);
	ushort* d = reg.matToUshort(d_img);
	ushort* t = reg.matToUshort(t_img);
	// Test conversions pointer to mat
	cv::Mat cout, dout, tout;
	reg.ucharToMat3c(c, 640, 480, cout);
	reg.ushortToMat1c(d, 640, 480, dout);
	reg.ushortToMat1c(t, 640, 480, tout);

	// Test read write
	// RGB
	uchar* prgb = reg.uchar3cImagePtr();
	prgb = reg.readRGBimage("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_images_640\\color\\c_20196201561021552305.png");
	cv::Mat test_1;
	reg.ucharToMat3c(prgb, 640, 480, test_1);
	delete prgb; prgb = NULL;
	// GREY
	ushort* pgrey = reg.ushort1cImagePtr();
	pgrey = reg.readGRAYimage("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_images_640\\thermo\\t_20196201561021552305.png");
	cv::Mat test_2;
	reg.ushortToMat1c(pgrey, 640, 480, test_2);
	delete pgrey; pgrey = NULL;


	
	// Test RegisterImages
	cv::Mat warpDepth, warpImage;

	cv::Mat alpha, colormap, mask, result;

	int im_width;
	int im_height;
	bool re = true;

	/*uchar *pwC = nullptr;
	ushort *pwD = nullptr;*/
	/*uchar *pwC = reg.uchar3cImagePtr();
	ushort *pwD = reg.ushort1cImagePtr();*/
	ushort *pwT = reg.ushort1cImagePtr();
	uchar *pwTCM = reg.uchar4cImagePtr();
	uchar *pwTOC = reg.uchar3cImagePtr();
	/*int a = 4;
	int * ax = nullptr;
	ax = &a;*/

	// Test update
	reg.update(t, c, d, true, 0, 1, 0.5, im_width, im_height, /*pwC, pwD,*/ pwT, pwTCM, pwTOC);
	cv::Mat exc, exd, ext, extcm, extoc;
	/*reg.ucharToMat3c(pwC, im_width, im_height, exc);
	reg.ushortToMat1c(pwD, im_width, im_height, exd);*/
	reg.ushortToMat1c(pwT, im_width, im_height, ext);
	reg.ucharToMat4c(pwTCM, im_width, im_height, extcm);
	reg.ucharToMat3c(pwTOC, im_width, im_height, extoc);

	// Test write
	reg.writeRGBimage(pwTOC, "testRGB.png", 640, 480);
	reg.writeRGBAimage(pwTCM, "testRGBA.png", 640, 480);
	reg.writeGRAYimage(pwT, "testGRAY.png", 640, 480);

	/*cv::Mat ovT;
	uchar * overlapT = reg.uchar3cImagePtr();
	reg.overlapImages(c, pwTCM, overlapT, im_width, im_height, 0, 0, 0.5);
	reg.ucharToMat3c(overlapT, im_width, im_height, ovT);
	cv::Mat d_imgCM, t_imgCM;*/
	/*reg.doColorMap(d_img, d_imgCM, true, 0, 1, 1, cv::COLORMAP_JET);
	reg.doColorMap(t_img, t_imgCM, false, 0, 1, 1, cv::COLORMAP_JET);

	uchar * pntr = reg.uchar4cImagePtr();
	reg.doColorMap(t, pntr, false, im_width, im_height, 0, 1, 1, cv::COLORMAP_JET);*/
	
	/*cv::imwrite("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\lib_register_tcd\\Demo_register_images\\color_jetson_no_load.png", c_img);
	cv::imwrite("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\lib_register_tcd\\Demo_register_images\\depth_jetson_no_load.png", d_img);
	cv::imwrite("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\lib_register_tcd\\Demo_register_images\\depthCM_jetson_no_load.png", d_imgCM);
	cv::imwrite("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\lib_register_tcd\\Demo_register_images\\thermo_jetson_no_load.png", t_img);
	cv::imwrite("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\lib_register_tcd\\Demo_register_images\\thermoCM_jetson_no_load.png", t_imgCM);
	cv::imwrite("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\lib_register_tcd\\Demo_register_images\\fusion_jetson_no_load.png", ovT);*/


	/*uchar *pCMT = reg.uchar4cImagePtr();
	reg.doColorMap(pwT, pCMT, im_width, im_height, 0, 1, 0.5, cv::COLORMAP_HOT);
	cv::Mat oT, ovT;
	reg.ucharToMat4c(pCMT, im_width, im_height, oT);

	uchar * overlapT = reg.uchar3cImagePtr();
	reg.overlapImages(c, pCMT, overlapT, im_width, im_height, 0, 0, 1);
	reg.ucharToMat3c(overlapT, im_width, im_height, ovT);*/

	//reg.doColorMap(dout, colormap, 0.2, 1, 0.5, cv::COLORMAP_JET);
	/*uchar *pCM = reg.uchar4cImagePtr();
	reg.doColorMap(t, pCM, im_width, im_height, 0, 1, 0.5, cv::COLORMAP_HOT);
	cv::Mat o, ov;
	reg.ucharToMat4c(pCM, im_width, im_height, o);

	uchar * overlap = reg.uchar3cImagePtr();
	reg.overlapImages(pwC, pCM, overlap, im_width, im_height, 0, 0, 0.5);
	reg.ucharToMat3c(overlap, im_width, im_height, ov);*/

	//reg.overlapImages(c_img, colormap, result);
	system("pause");
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
