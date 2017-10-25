#include "StdTuvokDefines.h"
#include <cstdlib>
#include <iostream>

#include <Basics/SysTools.h>
#include <Controller/Controller.h>
#include <IO/IOManager.h>
#include <Renderer/AbstrRenderer.h>
#include <Renderer/ContextIdentification.h>
#include <Renderer/GL/GLContext.h>
#include <Renderer/GL/GLFBOTex.h>
#include <Renderer/GL/GLFrameCapture.h>
#include <Renderer/GL/GLRenderer.h>

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

using namespace tuvok;

int main(int argc, const char *argv[])
{
  std::string filename = argv[1];
  try {
    // Setup render window, renderer, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    vtkNew<vtkInteractorStyleTrackballCamera> interactorStyle;
    renderWindowInteractor->SetInteractorStyle(interactorStyle.Get());
    renderer->SetBackground(.4, .5, .6);
    renderWindow->SetSize(640, 480);
    renderWindow->Render();
    renderWindow->MakeCurrent();
    Controller::Instance().DebugOut()->SetOutput(true, true, true, true);

    // Convert the data into a UVF.
    std::string uvf_file;
    uvf_file = SysTools::RemoveExt(filename) + ".uvf";

    AbstrRenderer* absren = Controller::Instance().RequestNewVolumeRenderer(
      MasterController::OPENGL_SBVR, false, false, false, false);
    GLRenderer* ren = dynamic_cast<GLRenderer*>(absren);
    ren->LoadFile(uvf_file);
    ren->AddShaderPath("../../../Shaders");
    ren->Initialize(GLContext::Current(0));
    ren->Resize(UINTVECTOR2(640, 480));
    ren->SetRendererTarget(AbstrRenderer::RT_INTERACTIVE);
    cout << "HERE1" << std::endl;
    ren->Paint();

    GLRenderer* glren = dynamic_cast<GLRenderer*>(ren);
    GLFBOTex* fbo = glren->GetLastFBO();
    GLTargetBinder bind(&Controller::Instance());

    GLFrameCapture fc;
    fc.CaptureSingleFrame("test.png", fbo);

    cout << "PAINTED!" << std::endl;
    //renderWindowInteractor->Start();
    sleep(10);

    ren->Cleanup();
    Controller::Instance().ReleaseVolumeRenderer(ren);
  }
  catch(const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
