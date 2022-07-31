//
// Created by ditto on 28/06/2022.
//

#include "SceneShape.h"

// Layer ------>
Layer::Layer(bool isVisible, std::string layerName, std::vector<int> layerShapes) : isVisible(isVisible), layerName(layerName), layerShapes(layerShapes) {}

std::string Layer::getName()
{
    return this->layerName;
}

void Layer::setName(std::string name)
{
    this->layerName = name;
}

bool Layer::getIsVisible()
{
    return this->isVisible;
}

void Layer::setIsVisible(bool toSet)
{
    this->isVisible = toSet;
}
std::vector<int> Layer::getLayerShapes()
{
    return this->layerShapes;
}

void Layer::addShape(int shapeIndex) {
    this->layerShapes.push_back(shapeIndex);
}

void Layer::removeShape(int shapeIndex) {
    std::vector<int> layerShapes;
    for (int num : this->layerShapes) {
        if (num != shapeIndex)
            layerShapes.push_back(num);
    }
    this->layerShapes = layerShapes;

}
//END Layer ------->

SceneShape::SceneShape(std::string shapeName, igl::opengl::glfw::Viewer::shapes shapeType,
    Layer* layer, int index, igl::opengl::glfw::Viewer* viewer) : name(shapeName), type(shapeType),
    layer(layer),parent(nullptr), index(index), currentPosition(Eigen::Vector3f(0, 0, 0)), viewer(viewer)
{
    movement = std::vector<BizMovment>();
}

Layer* SceneShape::getLayer()
{
    return layer;
}
void SceneShape::changeLayer(Layer* layer)
{
    this->layer = layer;
}
int SceneShape::getIndex() {
    return index;
}


void SceneShape::addBiz(BizMovment biz, float *max_time) {
    this->movement.push_back(biz);
    if (biz.end_time > *max_time)
        *max_time = biz.end_time;
}

void SceneShape::clearBiz() {
    this->movement.clear();
}


Eigen::Vector3f SceneShape::getlastDrawnPosition() {
    return lastDrawnPosition;
}

void SceneShape::setlastDrawnPosition(Eigen::Vector3f pos) {
    lastDrawnPosition = pos;
}


SceneShape* SceneShape::getParent() {
    return parent;
}

void SceneShape::removeParent()
{
    currentPosition = getCurrentPositionAt(0);
    parent = nullptr;

}


void SceneShape::setParent(SceneShape* newParent) {
    parent = newParent;
    currentPosition = currentPosition - newParent->getCurrentPosition();
    parent->addChild(this);
}

std::vector<SceneShape*> SceneShape::getChildren() {
    return children;
}

Eigen::Vector3f SceneShape::getCurrentPositionAt(float time)
{
    if (parent == nullptr)
        return getPosition(time);
    else
        return (getPosition(time) + this->parent->getCurrentPositionAt(time));
}

Eigen::Vector3f SceneShape::getCurrentPosition() {
    return currentPosition;
}


void SceneShape::addChild(SceneShape* child) {
    children.push_back(child);
}

void SceneShape::removeChild(SceneShape* removedChild) {
    std::vector<SceneShape*> newChildren;
    for (SceneShape* child : children) {
        if (child != removedChild)
            newChildren.push_back(child);
    }
    children = newChildren;
}

///move object

Eigen::Vector3f SceneShape::getPosition(float time) {
    for (std::size_t i = 0; i < this->movement.size(); i++) {
        if (movement[i].start_time <= time && movement[i].end_time >= time) {
            return movement[i].getPosition(time);
        }
    }
    return lastDrawnPosition;
}
void SceneShape::Scale(double shiftSize, directions d) {
    switch (d)
    {
    case x:
        viewer->data_list[getIndex()]->MyScale(Eigen::Vector3d(shiftSize, 1, 1));
        break;
    case y:
        viewer->data_list[getIndex()]->MyScale(Eigen::Vector3d(1, shiftSize, 1));
        break;

    case z:
        viewer->data_list[getIndex()]->MyScale(Eigen::Vector3d(1, 1, shiftSize));
        break;

    case w:
        viewer->data_list[getIndex()]->MyScale(Eigen::Vector3d(shiftSize, shiftSize, shiftSize));
        break;
    }
}

void SceneShape::move(double shiftSize,directions d) {
    switch (d) {
        case x:
            viewer->data_list[getIndex()]->MyTranslate(Eigen::Vector3d(shiftSize, 0 , 0) , false);
            currentPosition += Eigen::Vector3f(shiftSize, 0 , 0);
            break;

        case y:
            viewer->data_list[getIndex()]->MyTranslate(Eigen::Vector3d(0, shiftSize, 0), false);
            currentPosition += Eigen::Vector3f(0, shiftSize, 0);
            break;

        case z:
            viewer->data_list[getIndex()]->MyTranslate(Eigen::Vector3d(0 , 0 , shiftSize) , false);
            currentPosition += Eigen::Vector3f(0 , 0 , shiftSize);
            break;
    }
    
}
