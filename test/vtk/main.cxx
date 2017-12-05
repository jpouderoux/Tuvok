#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include "vtkTuvokMapper.h"

using namespace tuvok;

int main(int argc, const char *argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: vtkTuvok <data_file.uvf>" << std::endl;
    return EXIT_FAILURE;
  }
  std::string filename = argv[1];

  // Create a plane
  vtkNew<vtkPlaneSource> planeSource;
  planeSource->SetCenter(0.0, 0.0, 0.0);
  planeSource->SetNormal(0.0, 0.0, 1.0);
  planeSource->SetXResolution(40);
  planeSource->SetYResolution(40);
  planeSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(planeSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper.Get());
  actor->GetProperty()->EdgeVisibilityOn();
  //actor->GetProperty()->SetOpacity(0.9);

  vtkNew<vtkAxesActor> axes;
  vtkNew<vtkOrientationMarkerWidget> widget;
  widget->SetOrientationMarker(axes.Get());
  widget->SetViewport(0.0, 0.0, 0.2, 0.2);
  widget->InteractiveOff();

  vtkNew<vtkTuvokMapper> tuvokMapper;
  tuvokMapper->SetInputConnection(planeSource->GetOutputPort());
  tuvokMapper->SetFileName(filename.c_str());

  vtkNew<vtkVolume> volume;
  volume->SetMapper(tuvokMapper.Get());

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderWindow> renderWindow;
  vtkNew<vtkRenderer> renderer;
  renderWindow->AddRenderer(renderer.Get());
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow.Get());
  vtkNew<vtkInteractorStyleTrackballCamera> interactorStyle;
  renderWindowInteractor->SetInteractorStyle(interactorStyle.Get());
  widget->SetInteractor(renderWindowInteractor);
  widget->SetEnabled(1);
  renderer->SetBackground(.4, .5, .6);
  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderer->AddActor(actor.Get());
  renderer->ResetCamera();
  renderer->AddViewProp(volume.Get());
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
