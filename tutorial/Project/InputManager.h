#pragma once // maybe should be static class
#include "igl/opengl/glfw/Display.h"
#include "igl/opengl/glfw/renderer.h"
#include "Project.h"
#include "imgui/imgui.h"
#include "SceneShape.h"
#include <math.h>

bool holdsLeft;
double xStart, yStart;
// 0=LEFT/UP, 1=RIGHT/DOWN
int ChosenScreen = 0;

float normelize(float num, int maxSize)
{
    return (((2 * num) / maxSize) - 1);
}

// Zoom in objects
Eigen::Vector3f FindCenterOfPickedObjects(Project *scn)
{
    float zoomInCenter = 4;
    int shapesCount = scn->pShapes.size();

    if (shapesCount > 0)
    {

        Eigen::Vector3f averagePos = Eigen::Vector3f(0, 0, 0);

        for (int shapeIndex : scn->pShapes)
        { // chaneg to picked shapes
            averagePos += scn->shapesGlobal[shapeIndex].getCurrentPosition();
        }
        averagePos /= shapesCount;
        averagePos -= Eigen::Vector3f(0, 0, zoomInCenter);

        return averagePos;
    }
    return Eigen::Vector3f(0, 0, 0);
}

void scalePickedObjects(double shiftSize, directions d, Project *scn)
{
    for (int i : scn->pShapes)
    {
        scn->shapesGlobal[i].Scale(shiftSize, d);
    }
}

void movePickedObjects(double shiftSize, directions d, Project *scn)
{
    for (int i : scn->pShapes)
    {
        scn->shapesGlobal[i].move(shiftSize, d);
    }
}

void glfw_mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {

        Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
        Project *scn = (Project *)rndr->GetScene();
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            rndr->UnPick(3);
            rndr->Pressed();
            glfwGetCursorPos(window, &xStart, &yStart);
            rndr->UnPick(3);
        }
        rndr->UpdatePress(xStart, yStart);
    }
    else
    {
        Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
        Project *scn = (Project *)rndr->GetScene();

        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            
            double xEnd, yEnd;
            glfwGetCursorPos(window, &xEnd, &yEnd);
            rndr->PickMany(3);
            rndr->Pressed();
        }
    }
}

void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
    Project *scn = (Project *)rndr->GetScene();

    // Set chosen camera
    // If chosen screen is 0, than we need viewport 0 so drawinfo 1=1+0 is still valid,
    // and if screen is 1 than we need draw info 2 = 1+1
    int ChosenCamera = scn->GetRenderer()->GetDrawCamera(ChosenScreen + 1);

    if (rndr->IsPicked())
    {
        std::cout << "picked!" << std::endl;
        rndr->UpdateZpos((int)yoffset);
        rndr->MouseProccessing(GLFW_MOUSE_BUTTON_MIDDLE);
    }
    else
    {
        rndr->MoveCamera(ChosenCamera, rndr->zTranslate, (float)yoffset);
    }
}

void glfw_cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
    Project *scn = (Project *)rndr->GetScene();
    double xStart, yStart;
    glfwGetCursorPos(window, &xStart, &yStart);

    rndr->UpdatePosition((float)xpos, (float)ypos);

    if (rndr->CheckViewport(xpos, ypos, 0))
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            rndr->UnPick(2);
            glfwGetCursorPos(window, &xStart, &yStart);
            if (!rndr->IsPressed())
            {
                rndr->Pressed();
            }
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && rndr->IsPressed())
        {
            double xEnd, yEnd;
            glfwGetCursorPos(window, &xEnd, &yEnd);
            rndr->PickMany(2);
            rndr->Pressed();
        }
    }
}

void glfw_window_size_callback(GLFWwindow *window, int width, int height)
{
    Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);

    rndr->resize(window, width, height);
}

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
    Project *scn = (Project *)rndr->GetScene();

    // Set chosen camera
    // If chosen screen is 0, than we need viewport 0 so drawinfo 1=1+0 is still valid,
    // and if screen is 1 than we need draw info 2 = 1+1
    int ChosenCamera = scn->GetRenderer()->GetDrawCamera(ChosenScreen + 1);

    // rndr->FreeShapes(2);
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
       

        case GLFW_KEY_UP:
            rndr->MoveCamera(ChosenCamera, scn->xRotate, 0.05f);

            break;
        case GLFW_KEY_DOWN:
            rndr->MoveCamera(ChosenCamera, scn->xRotate, -0.05f);
            break;
        case GLFW_KEY_LEFT:
            rndr->MoveCamera(ChosenCamera, scn->yRotate, 0.05f);
            break;
        case GLFW_KEY_RIGHT:
            rndr->MoveCamera(ChosenCamera, scn->yRotate, -0.05f);
            break;
        case GLFW_KEY_D:
            if (rndr->IsPicked())
                movePickedObjects(0.02, x, scn);
            else
                rndr->MoveCamera(ChosenCamera, scn->xTranslate, 0.25f);
            break;
        case GLFW_KEY_A:
            if (rndr->IsPicked())
                movePickedObjects(-0.02, x, scn);
            else
                rndr->MoveCamera(ChosenCamera, scn->xTranslate, -0.25f);
            break;
        case GLFW_KEY_W:
            if (rndr->IsPicked())
                movePickedObjects(0.02, y, scn);
            else
                rndr->MoveCamera(ChosenCamera, scn->yTranslate, 0.25f);
            break;
        case GLFW_KEY_S:
            if (rndr->IsPicked())
                movePickedObjects(-0.02, y, scn);
            else
                rndr->MoveCamera(ChosenCamera, scn->yTranslate, -0.25f);
            break;
        case GLFW_KEY_K:
            scn->NextCubeMap();
            break;
        
        case GLFW_KEY_O:
        {
            Eigen::Vector3f center = FindCenterOfPickedObjects(scn);
            rndr->MoveCamera(ChosenCamera, 100, 0);
            rndr->MoveCamera(ChosenCamera, scn->xTranslate, center.x());
            rndr->MoveCamera(ChosenCamera, scn->yTranslate, center.y());
            rndr->MoveCamera(ChosenCamera, scn->zTranslate, center.z());
            break;
        }
        case GLFW_KEY_G:
            movePickedObjects(+0.02, z, scn);
            break;
        case GLFW_KEY_F:
            movePickedObjects(-0.02, z, scn);
            break;

        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_SPACE:
            if (scn->IsActive())
                scn->Deactivate();
            else
                scn->Activate();
            break;
        case GLFW_KEY_LEFT_SHIFT:
            ChosenScreen = 0;
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            ChosenScreen = 1;
            break;
        default:
            std::cout << "you choosed a key that doesnt have an effect \n";
            break;
        }
    }
}

void Init(Display &display, igl::opengl::glfw::imgui::ImGuiMenu *menu)
{
    display.AddKeyCallBack(glfw_key_callback);
    display.AddMouseCallBacks(glfw_mouse_callback, glfw_scroll_callback, glfw_cursor_position_callback);
    display.AddResizeCallBack(glfw_window_size_callback);
    menu->init(&display);
}
