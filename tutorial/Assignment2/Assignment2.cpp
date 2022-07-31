#include "Assignment2.h"
#include <iostream>


static void printMat(const Eigen::Matrix4d& mat)
{
	std::cout << " matrix:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << mat(j, i) << " ";
		std::cout << std::endl;
	}
}

Assignment2::Assignment2()
{
	SceneParser("C:\\Users\\ido\\Documents\\GitHub\\CG3DBasicEngine\\tutorial\\Assignment2\\scene5.txt", &scnData);
	xResolution = 800;
	yResolution = 800;
	sourceIndx = -1;
	isRightPressed = false;
	isPressed = false;
	xCamRot = 0;
	yCamRot = 0;
}

void Assignment2::Init()
{
	unsigned int texIDs[3] = { 0 , 1, 2 };
	unsigned int slots[3] = { 0 , 1, 2 };

	AddShader("shaders/pickingShader");
	AddShader("shaders/raytracingShader");

	AddTexture("textures/box0.bmp", 2);
	AddTexture("textures/grass.bmp", 2);

	AddMaterial(texIDs, slots, 1);
	AddMaterial(texIDs + 1, slots + 1, 1);

	AddShape(Plane, -1, TRIANGLES, 0);
	
	SetShapeShader(0, 1);
	SetShapeMaterial(0, 0);
	SetShapeStatic(0);
}

void Assignment2::Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx)
{
	Shader* s = shaders[shaderIndx];
	int r = ((shapeIndx + 1) & 0x000000FF) >> 0;
	int g = ((shapeIndx + 1) & 0x0000FF00) >> 8;
	int b = ((shapeIndx + 1) & 0x00FF0000) >> 16;

	s->Bind();

	s->SetUniformMat4f("Proj", Proj);
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Model", Model);
	
	s->SetUniform2f("camRot", xCamRot, yCamRot);
	s->SetUniform4fv("eye", &scnData.eye, 1);
	s->SetUniform4fv("ambient", &scnData.ambient, 1);
	s->SetUniform4fv("objects", &scnData.objects[0], scnData.objects.size());
	s->SetUniform4fv("objColors", &scnData.colors[0], scnData.colors.size());
	s->SetUniform4fv("lightsPosition", &scnData.lights[0], scnData.lights.size());
	s->SetUniform4fv("lightsDirection", &scnData.directions[0], scnData.directions.size());
	s->SetUniform4fv("lightsIntensity", &scnData.intensities[0], scnData.intensities.size());

	s->SetUniform4i("sizes", scnData.sizes[0], scnData.sizes[1], scnData.sizes[2], scnData.sizes[3]);
	if (data_list[shapeIndx]->GetMaterial() >= 0 && !materials.empty())
	{
		BindMaterial(s, data_list[shapeIndx]->GetMaterial());
	}
	s->Unbind();
}

void Assignment2::SetPosition(int x, int y)
{
	if (isPressed)
	{
		xRel = -xOldPos + (float)x / xResolution;
		yRel = yOldPos - (float)y / yResolution;
		if (isRightPressed)
		{
			if (sourceIndx >= 0)
			{
				scnData.objects[sourceIndx][0] += xRel * 2;
				scnData.objects[sourceIndx][1] += yRel * 2;
			}
		}
		else
		{
			scnData.eye[0] += xRel * 2;
			scnData.eye[1] += yRel * 2;
		}
	}
    xOldPos = ((float)x / xResolution);
    yOldPos = (float)(y) / yResolution;
}


void Assignment2::WhenRotate()
{
}

void Assignment2::WhenTranslate()
{

}

void Assignment2::Animate() {
}

void Assignment2::ScaleAllShapes(float amt, int viewportIndx)
{
	for (int i = 1; i < data_list.size(); i++)
	{
		if (data_list[i]->Is2Render(viewportIndx))
		{
			data_list[i]->MyScale(Eigen::Vector3d(amt, amt, amt));
		}
	}
}


Eigen::Matrix4f rotationMatrix(Eigen::Vector3f axis, float angle)
{
	axis = axis.normalized();
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;
	
	Eigen::Matrix4f m;
	m << oc * axis[0] * axis[0] + c, oc* axis[0]* axis[1] - axis[2] * s, oc* axis[2]* axis[0] + axis[1] * s, 0.0,
		oc* axis[0]* axis[1] + axis[2] * s, oc* axis[1]* axis[1] + c, oc* axis[1]* axis[2] - axis[0] * s, 0.0,
		oc* axis[2]* axis[0] - axis[1] * s, oc* axis[1]* axis[2] + axis[0] * s, oc* axis[2]* axis[2] + c, 0.0,
		0.0, 0.0, 0.0, 1.0;
	return m;
}

float Assignment2::Intersection(Eigen::Vector3f sourcePoint)
{
	sourcePoint = Eigen::Vector3f(scnData.eye[0] + sourcePoint[0], scnData.eye[1] + sourcePoint[1], scnData.eye[2] - 1.0f);

	sourcePoint = sourcePoint - scnData.eye.head(3);
	Eigen::Vector4f position(sourcePoint[0], sourcePoint[1], sourcePoint[2], 1.0f);
	position = rotationMatrix(Eigen::Vector3f(0, 1, 0), xCamRot) * rotationMatrix(Eigen::Vector3f(1, 0, 0), yCamRot) * position;
	sourcePoint = Eigen::Vector3f(position[0], position[1], position[2]) + scnData.eye.head(3);

	Eigen::Vector3f v = (sourcePoint - scnData.eye.head(3)).normalized();
	float tmin = 1.0e10;
	int indx = -1;
	for (int i = 0; i < scnData.sizes[0]; i++) //every object
	{
		if (scnData.objects[i][3] > 0) //sphere
		{
			Eigen::Vector3f p0o = (scnData.objects[i].head(3)) - sourcePoint;
			float r = scnData.objects[i][3];
			float b = v.dot(p0o);
			float delta = b * b - p0o.dot(p0o) + r * r;
			float t;
			if (delta >= 0)
			{
				if (b >= 0)
					t = b - sqrt(delta);
				else
					t = b + sqrt(delta);
				if (t < tmin && t>0)
				{
					tmin = t;
					indx = i;
				}
			}
		}
		else  //plane
		{
			Eigen::Vector3f n = ((scnData.objects[i]).head(3)).normalized();
			Eigen::Vector3f p0o = (-scnData.objects[i][3] * n) - scnData.objects[i].head(3);

			float t = (n.dot(p0o)) / (n.dot(v));
			if (t > 0 && t < tmin)
			{
				tmin = t;
				indx = i;
			}
		}
	}
	sourceIndx = indx;
	//std::cout<<"indx "<<indx<<std::endl;
	return tmin;
}

void Assignment2::RotateEye(float amt, bool upDown)
{
	float n = scnData.eye.norm();
	if (upDown) {
		yCamRot += amt;
	}
	else {
		xCamRot += amt;
	}

}

Assignment2::~Assignment2(void)
{
}

