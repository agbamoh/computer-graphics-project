
#ifndef ASSIGNMENT1_CPP_SCENESHAPE_H
#define ASSIGNMENT1_CPP_SCENESHAPE_H

#include <Eigen/Core>
#include "igl/opengl/glfw/Viewer.h"

enum directions{
    x,y,z,w
};

class Layer {
public:
    Layer(bool isVisible, std::string layerName, std::vector<int> layerShapes);
    std::string getName();
    void setName(std::string name);
    bool getIsVisible();
    void setIsVisible(bool toSet);
    void addShape(int shapeIndex);
    void removeShape(int shapeIndex);
    std::vector<int> getLayerShapes();
private:
    bool isVisible;
    std::string layerName;
    std::vector<int> layerShapes;
};

class BizMovment {
public:
    BizMovment(std::vector<Eigen::Vector3f> movement, float start_time, float end_time) :
        points(movement), start_time(start_time), end_time(end_time) {};
    std::vector<Eigen::Vector3f> points;
    float start_time;
    float end_time;
    Eigen::Vector3f getPosition(float time) {
        size_t n = points.size();
        if (n == 1)
            return points[0];
        float normalizedTime = (time - start_time) / (end_time - start_time);
        Eigen::Vector3f pos = Eigen::Vector3f(0, 0, 0);
        for (std::size_t i = 0; i < n; i++) {
            pos += BinomialCoefficient(n, i) * pow((1-normalizedTime), n-i) * pow(normalizedTime, i) * points[i];
        }
        return pos;
    };

private:
    int BinomialCoefficient(const int n, const int k) {
        if (k == 0)
            return 1;
        std::vector<int> aSolutions(k);
        aSolutions[0] = n - k + 1;

        for (int i = 1; i < k; ++i) {
            aSolutions[i] = aSolutions[i - 1] * (n - k + 1 + i) / (i + 1);
        }

        return aSolutions[k - 1];
    }
};

class SceneShape {
public:
    SceneShape(std::string shapeName, igl::opengl::glfw::Viewer::shapes shapeType,
        Layer* layer, int index, igl::opengl::glfw::Viewer* viewer);
    Layer* getLayer();
    void changeLayer(Layer* layer);
    int getIndex();
    void addBiz(BizMovment biz, float *max_time);
    void clearBiz();
    Eigen::Vector3f getPosition(float time);
    Eigen::Vector3f getlastDrawnPosition();
    void setlastDrawnPosition(Eigen::Vector3f pos);
    std::string getName() { return name; };
    SceneShape* getParent();
    void removeParent();
    void setParent(SceneShape* newParent);
    void addChild(SceneShape* child);
    void removeChild(SceneShape* child);
    std::vector<SceneShape*> getChildren();
    Eigen::Vector3f getCurrentPositionAt(float time);
    Eigen::Vector3f getCurrentPosition();
    bool isTransparent = false;
    void SceneShape::Scale(double shiftSize, directions d);

    //edit mode movment:
    void move(double shiftSize,directions d);

private:
    std::string name;
    igl::opengl::glfw::Viewer::shapes type;
    std::vector<BizMovment> movement;
    Eigen::Vector3f currentPosition;
    igl::opengl::glfw::Viewer* viewer;
    Layer* layer;
    int index;
    Eigen::Vector3f lastDrawnPosition;
    SceneShape* parent;
    std::vector<SceneShape*> children;


};
#endif //ASSIGNMENT1_CPP_SCENESHAPE_H