// Test_lib_register.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#include "pch.h"

#include <iostream>
#include "lib_register_tcd.h"


uchar * matToUchar(cv::Mat image)
{
	int size = image.total() * image.elemSize();
	uchar * bytes = new uchar[size];  // you will have to delete[] that later
	std::memcpy(bytes, image.data, size * sizeof(uchar));
	return bytes;
}

cv::Mat ucharToMat(uchar * bytes, int width, int height)
{
	cv::Mat image = cv::Mat(height, width, CV_8UC3, bytes).clone(); // make a copy
	return image;
}

ushort * matToUshort(cv::Mat image)
{
	int size = image.total();
	ushort * bytes = new ushort[size];  // you will have to delete[] that later
	std::memcpy(bytes, image.data, size * sizeof(ushort));
	return bytes;
}

cv::Mat ushortToMat(ushort * bytes, int width, int height)
{
	cv::Mat image = cv::Mat(height, width, CV_16UC1, bytes).clone(); // make a copy
	return image;
}

int main()
{
	char path[] = "C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_imagesx640_80\\spaceing24_extrinsics_640_80.xml";

	cv::Mat t_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_imagesx640_80\\thermo\\t_2019711561964745974.png", cv::IMREAD_ANYDEPTH);
	cv::Mat c_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_imagesx640_80\\color\\c_2019711561964745974.png", cv::IMREAD_COLOR);
	cv::Mat d_img = cv::imread("C:\\Users\\aljuasin\\Desktop\\captura_tcd\\_imagenes\\patron\\output_imagesx640_80\\depth\\d_2019711561964745974.png", cv::IMREAD_ANYDEPTH);

	// Create RegisterTCD object
	RegisterTCD reg(*path, true);
	// Test conversions mat to pointer
	uchar* c = matToUchar(c_img);
	ushort* d = matToUshort(d_img);
	ushort* t = matToUshort(t_img);
	// Test conversions pointer to mat
	cv::Mat cout, dout, tout;
	cout = ucharToMat(c, 640, 480);
	dout = ushortToMat(d, 640, 480);
	tout = ushortToMat(t, 640, 480);
	// Test update
	//reg.update(t, c, d);
	// Test RegisterImages
	cv::Mat warpDepth, warpImage;

	




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
