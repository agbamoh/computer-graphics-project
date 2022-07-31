#pragma once
#include "./igl/opengl/glfw/Viewer.h"

class Assignment1 : public igl::opengl::glfw::Viewer
{
	int coeffPick;
	int iterNum;
	Eigen::Vector3cf roots;
	float time;
	
	Eigen::scomplex cPow(Eigen::scomplex num, int pow);
	Eigen::Vector3cf FindRootsOfReduceEquation(Eigen::Vector2cf reduceCoeffs);
	std::complex<float> Assignment1::NewtonCubicRoot(std::complex<float> num);

public:

	Eigen::Vector4cf coeffs;
	float x, y;
	float fxStart, fxEnd, fyStart, fyEnd;
	float lastPosx, lastPosy;
	bool firstDrag;
	Assignment1();
	//	Assignment1(float angle,float relationWH,float near, float far);
	void Init();
	void Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx);
	void WhenRotate();
	void WhenTranslate();
	void Animate() override;
	void ScaleAllShapes(float amt, int viewportIndx);
	void setCoeff(int num);
	void changePickedCoeff(bool isInc);
	void changeIterNum(bool isInc);
	void zoom(float offset);
	void move(float xoffset, float yoffset);
	void printAll();

	Eigen::Vector3cf FindCubicRoots();

	~Assignment1(void);
};

