#include "igl/opengl/glfw/renderer.h"
#include "Project.h"
#include "InputManager.h"
#include "MenuManager.h"

int main(int argc, char *argv[])
{
    const int DISPLAY_WIDTH = 1200;
    const int DISPLAY_HEIGHT = 800;
    const float CAMERA_ANGLE = 45.0f;
    const float NEAR = 1.0f;
    const float FAR = 120.0f;
    std::list<int> x, y;
    x.push_back(DISPLAY_WIDTH / 2);
    x.push_back(DISPLAY_WIDTH);
    y.push_back(DISPLAY_HEIGHT);
    Display disp = Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OPENGL");
    igl::opengl::glfw::imgui::ImGuiMenu *menu = new igl::opengl::glfw::imgui::ImGuiMenu();
    Renderer *rndr = new Renderer(CAMERA_ANGLE, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT / 2, NEAR, FAR);
    Project *scn = new Project(); // initializing scene

    rndr->AddCamera(Eigen::Vector3d(0, 0, 3), CAMERA_ANGLE, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT / 2, NEAR, FAR);


    Init(disp, menu);                         // adding callback functions
    scn->Init(DISPLAY_WIDTH, DISPLAY_HEIGHT); // adding shaders, textures, shapes to scene
    rndr->Init(scn, x, y, 1, menu);           // adding scene and viewports to the renderer
    scn->SetRenderer(rndr);
    disp.SetRenderer(rndr);

    rndr->AddViewport(0, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT);
    rndr->AddViewport(0, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT);
    rndr->AddViewport(0, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT); // <-------------------
    rndr->CopyDraw(2, rndr->viewport, 2);
    rndr->ClearDrawFlag(3, rndr->toClear | rndr->stencilTest);
    rndr->SetDrawFlag(3, rndr->blend | rndr->inAction2 | rndr->scissorTest);

    
    rndr->AddDraw(3, 0, 3, 0, rndr->blend | rndr->scaleAbit | rndr->depthTest | rndr->onPicking | rndr->stencil2 | rndr->stencilTest);
    rndr->AddDraw(4, 0, 3, 0, rndr->blend | rndr->scaleAbit | rndr->depthTest | rndr->onPicking); // <-------------------



    disp.launch_rendering(rndr);

    delete scn;
    delete rndr;
    delete menu;

    return 0;
}
