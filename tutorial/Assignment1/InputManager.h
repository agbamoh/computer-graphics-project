#pragma once   //maybe should be static class
#include "./igl/opengl/glfw/Display.h"
#include "./igl/opengl/glfw/renderer.h"
#include "./Assignment1.h"
#include "./imgui/imgui.h"


	void glfw_mouse_callback(GLFWwindow* window,int button, int action, int mods)
	{	
		if (action == GLFW_PRESS)
		{
			
			Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
			Assignment1* scn = (Assignment1*)rndr->GetScene();
			double x2, y2;
			
			glfwGetCursorPos(window, &x2, &y2);
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				rndr->Pressed();
				scn->firstDrag = true;
			}
			rndr->UpdatePress(x2, y2);
			if (rndr->Picking((int)x2, (int)y2))
			{
				rndr->UpdatePosition(x2, y2);
				
					

			}
			else
			{
				rndr->UnPick(2);
			}
		
		}
		else
		{
			Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
			rndr->UnPick(2);
		}
	}
	
	void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Assignment1* scn = (Assignment1*)rndr->GetScene();

		scn->zoom(yoffset);
		scn->printAll();
		
	}
	
	void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Assignment1* scn = (Assignment1*)rndr->GetScene();

		rndr->UpdatePosition((float)xpos,(float)ypos);

		if (rndr->CheckViewport(xpos,ypos, 0))
		{
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{

				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				if (!scn->firstDrag) {
					scn->move(xpos - scn->lastPosx, ypos - scn-> lastPosy);
				}
				scn->firstDrag = false;
				scn->lastPosx = xpos;
				scn->lastPosy = ypos;

				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && rndr->IsPicked() && rndr->IsMany())
					rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);

		}
	}

	void glfw_window_size_callback(GLFWwindow* window, int width, int height)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);

        rndr->resize(window,width,height);
		
	}
	
	void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Assignment1* scn = (Assignment1*)rndr->GetScene();
		//rndr->FreeShapes(2);
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
				
			case GLFW_KEY_UP:
				scn->changePickedCoeff(true);
				break;
			case GLFW_KEY_DOWN:
				scn->changePickedCoeff(false);
				break;
			case GLFW_KEY_LEFT:
				scn->changeIterNum(false);
				break;
			case GLFW_KEY_RIGHT:
				scn->changeIterNum(true);
				break;
			case GLFW_KEY_1:
				scn->setCoeff(0);
				break;
			case GLFW_KEY_2:
				scn->setCoeff(1);
				break;
			case GLFW_KEY_3:
				scn->setCoeff(2);
				break;
			case GLFW_KEY_4:
				scn->setCoeff(3);
				break;
			default:
				break;
			}
			scn->printAll();
		}
	}


void Init(Display& display, igl::opengl::glfw::imgui::ImGuiMenu *menu)
{
    display.AddKeyCallBack(glfw_key_callback);
    display.AddMouseCallBacks(glfw_mouse_callback, glfw_scroll_callback, glfw_cursor_position_callback);
    display.AddResizeCallBack(glfw_window_size_callback);
    menu->init(&display);
}
