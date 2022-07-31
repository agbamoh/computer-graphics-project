#include "Project.h"

class MenuManager
{
public:
    enum CameraSplitMode
    {
        no_split,
        split_x,
        split_y
    };

    static void OnMaterialSelection(int material_inx, Project* scn)
    {
        for (size_t i = 0; i < scn->pShapes.size(); i++)
        {
            int shpIndex = scn->pShapes[i];
            scn->SetShapeMaterial(shpIndex, material_inx);

        }
    }

    static void OnTransparentToggled(bool isTransparent, Project* scn)
    {
        for (size_t i = 0; i < scn->pShapes.size(); i++)
        {
            int shapeIndx = scn->pShapes[i];
            scn->shapesGlobal[shapeIndx].isTransparent = isTransparent;
        }
    }

    static void OnBlurToggled(bool isBlurred, Project* scn)
    {
        for (size_t i = 0; i < scn->pShapes.size(); i++)
        {
            int shapeIndx = scn->pShapes[i];
            if (isBlurred == true)
            {
                scn->SetShapeShader(shapeIndx, scn->blurShaderIndx);
            }
            else
            {
                scn->SetShapeShader(shapeIndx, scn->basicShaderIndx);
            }
        }
    }

    static void OnCameraSplitSelection(int splitMode, Project *scn)
    {
        switch (splitMode)
        {
        case CameraSplitMode::no_split:
            // Set viewport 0 to all screen
            scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH, scn->DISPLAY_HEIGHT, 0);
            // Set viewport 1 to 0
            scn->GetRenderer()->SetViewport(0, 0, 0, 0, 1);
            // PICKING Viewports:
            // Set viewport 2 to all screen
            scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH, scn->DISPLAY_HEIGHT, 2);
            // Set viewport 3 to all screen
            scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH, scn->DISPLAY_HEIGHT, 3);
            // scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH, scn->DISPLAY_HEIGHT, 4);
            // Set camera relation
            for (size_t i = 0; i < scn->GetRenderer()->GetCameras().size(); i++)
            {
                scn->GetRenderer()->ChangeCameraRelation(i, (float)scn->DISPLAY_WIDTH / (float)scn->DISPLAY_HEIGHT);
            }
            break;
        case CameraSplitMode::split_x:
            // Set viewport 0 to all screen
            scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH / 2, scn->DISPLAY_HEIGHT, 0);
            // Set viewport 1 to 0
            scn->GetRenderer()->SetViewport(scn->DISPLAY_WIDTH / 2, 0, scn->DISPLAY_WIDTH / 2, scn->DISPLAY_HEIGHT, 1);
            // PICKING Viewports:
            // Set viewport 2 to all screen
            scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH / 2, scn->DISPLAY_HEIGHT, 2);
            // Set viewport 3 to all screen
            scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH / 2, scn->DISPLAY_HEIGHT, 3);
            // scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH / 2, scn->DISPLAY_HEIGHT, 4);
            // Set camera relation
            for (size_t i = 0; i < scn->GetRenderer()->GetCameras().size(); i++)
            {
                scn->GetRenderer()->ChangeCameraRelation(i, (float)scn->DISPLAY_WIDTH / (float)scn->DISPLAY_HEIGHT / 2);
            }
            break;
        case CameraSplitMode::split_y:
            // Set viewport 1 to 0
            scn->GetRenderer()->SetViewport(0, scn->DISPLAY_HEIGHT / 2, scn->DISPLAY_WIDTH, scn->DISPLAY_HEIGHT / 2, 0);
            // Set viewport 0 to all screen
            scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH, scn->DISPLAY_HEIGHT / 2, 1);
            // PICKING Viewports:
            // Set viewport 2 to all screen
            scn->GetRenderer()->SetViewport(0, 0, scn->DISPLAY_WIDTH, scn->DISPLAY_HEIGHT, 2);
            // Set viewport 3 to all screen
            scn->GetRenderer()->SetViewport(0, scn->DISPLAY_HEIGHT/2, scn->DISPLAY_WIDTH, scn->DISPLAY_HEIGHT / 2, 3);
            // scn->GetRenderer()->SetViewport(0, scn->DISPLAY_HEIGHT / 2, scn->DISPLAY_WIDTH, scn->DISPLAY_HEIGHT / 2, 4);
            // Set camera relation
            for (size_t i = 0; i < scn->GetRenderer()->GetCameras().size(); i++)
            {
                scn->GetRenderer()->ChangeCameraRelation(i, (float)scn->DISPLAY_WIDTH / ((float)scn->DISPLAY_HEIGHT / 2));
            }
            break;
        default:
            // error
            break;
        }
    }

    static void OnAddCamera(std::string camera_name, Project *scn,float* vals, float* pos)

    {
       
        scn->GetRenderer()->AddCamera(Eigen::Vector3d(pos[0], pos[1], pos[2]), vals[0], (float)scn->DISPLAY_WIDTH / (float)scn->DISPLAY_HEIGHT / 2, vals[1], vals[2]);
    }

    static void OnLayerSelection(std::string layer_name, Project* scn)
    {
        std::cout << "Set layer: " << layer_name << ", num of picked: " << scn->pShapes.size() << std::endl;
        Layer* changedLayer;
        for (size_t i = 0; i < scn->layers.size(); i++)
        {
            if (scn->layers[i]->getName() == layer_name)
            {
                changedLayer = scn->layers[i];
                break;
            }
        }
        for (int shapeIndex : scn->pShapes)
        {
            scn->shapesGlobal[shapeIndex].getLayer()->removeShape(shapeIndex);
            scn->shapesGlobal[shapeIndex].changeLayer(changedLayer);
            scn->shapesGlobal[shapeIndex].getLayer()->addShape(shapeIndex);
        }
    }

    static void OnLayerChange(std::string layer_name, bool is_visible, Project* scn)
    {
        for (Layer* layer : scn->layers)
        {
            if (layer->getName() == layer_name)
            {
                layer->setIsVisible(is_visible);
                break;
            }
        }
    }

    static void OnAddLayer(std::string layer_name, bool is_shown, Project *scn)
    {
        std::vector<int> layerShapes;
        Layer *layer = new Layer(is_shown, layer_name, layerShapes);
        scn->layers.push_back(layer);
    }

    static void OnCameraSelection(int screenIndx, int cameraIndx, Project* scn)
    {
        if (screenIndx == 0)
        {
            scn->GetRenderer()->SetDrawCamera(0, cameraIndx);
            scn->GetRenderer()->SetDrawCamera(1, cameraIndx);
        }
        else if (screenIndx == 1)
        {
            scn->GetRenderer()->SetDrawCamera(2, cameraIndx);
        }
    }

    static void OnPlayChanged(bool play, Project *scn)
    {
        scn->globalTime = 0;
    }

    static void onAddPoint(float point[3], Project* scn) {
        scn->bizPoints.push_back(Eigen::Vector3f(point[0], point[1], point[2]));
        std::cout << "point added: (" << point[0] << "," << point[1] << "," << point[2] << ")" << std::endl;
        scn->bizPoint[0] = 0;
        scn->bizPoint[1] = 0;
        scn->bizPoint[2] = 0;
    }

    static void onAddBiz(float times[2], Project* scn) {
        BizMovment movment = BizMovment(scn->bizPoints, times[0], times[1]);
        scn->bizPoints.clear();
        for (int i : scn->pShapes) {
            std::cout << "Bezier movment added to shape: " << i << std::endl;
            scn->shapesGlobal[i].addBiz(movment,&(scn->max_time));
        }
        scn->start_end_time[0] = 0;
        scn->start_end_time[1] = scn->max_time;
    }

    static void onRemoveBiz(Project* scn) {
        for (int i : scn->pShapes) {
            std::cout << "Bezier movment removed from shape: " << i << std::endl;
            scn->shapesGlobal[i].clearBiz();
        }
    }
};