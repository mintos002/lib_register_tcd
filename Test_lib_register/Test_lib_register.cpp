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

	cv::Mat t_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\jetson\\no_load\\thermo\\t_2019711561968234554.png", cv::IMREAD_ANYDEPTH);
	cv::Mat c_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\jetson\\no_load\\color\\c_2019711561968234554.png", cv::IMREAD_COLOR);
	cv::Mat d_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\jetson\\no_load\\depth\\d_2019711561968234554.png", cv::IMREAD_ANYDEPTH);

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
	// Test update
	//reg.update(t, c, d);
	// Test RegisterImages
	cv::Mat warpDepth, warpImage;

	cv::Mat alpha, colormap, mask, result;

	int im_width;
	int im_height;
	bool re = true;

	/*uchar *pwC = nullptr;
	ushort *pwD = nullptr;*/
	uchar *pwC = reg.uchar3cImagePtr();
	ushort *pwD = reg.ushort1cImagePtr();
	/*int a = 4;
	int * ax = nullptr;
	ax = &a;*/


	reg.update(t, c, d, true, 0.2, 1, im_width, im_height, pwC, pwD);
	cv::Mat exc, exd;
	reg.ucharToMat3c(pwC, im_width, im_height, exc);
	reg.ushortToMat1c(pwD, im_width, im_height, exd);

	//reg.doColorMap(dout, colormap, 0.2, 1, 0.5, cv::COLORMAP_JET);
	uchar *pCM = reg.uchar4cImagePtr();
	reg.doColorMap(t, pCM, im_width, im_height, 0.2, 1, 0.5, cv::COLORMAP_HOT);
	cv::Mat o, ov;
	reg.ucharToMat4c(pCM, im_width, im_height, o);

	uchar * overlap = reg.uchar3cImagePtr();
	reg.overlapImages(pwC, pCM, overlap, im_width, im_height, 0, 0, 1);
	reg.ucharToMat3c(overlap, im_width, im_height, ov);

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
