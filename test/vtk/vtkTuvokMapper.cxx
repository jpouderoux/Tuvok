/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTuvokMapper.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkTuvokMapper.h"

#include "vtkActor.h"
#include "vtkBoundingBox.h"
#include "vtkCamera.h"
#include "vtkDataSet.h"
#include "vtkExecutive.h"
#include "vtkGenericCell.h"
#include "vtkInformation.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkSmartPointer.h"

//#include "vtk_glew.h"

#include "StdTuvokDefines.h"
#include <Basics/SysTools.h>
#include <Controller/Controller.h>
#include <IO/IOManager.h>
#include <Renderer/AbstrRenderer.h>
#include <Renderer/ContextIdentification.h>
#include <Renderer/GL/GLContext.h>
#include <Renderer/GL/GLFBOTex.h>
#include <Renderer/GL/GLFrameCapture.h>
#include <Renderer/GL/GLRenderer.h>

#include <algorithm>
#include <vector>

#include <GL/glew.h>

using namespace tuvok;

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTuvokMapper)

//-----------------------------------------------------------------------------
vtkTuvokMapper::vtkTuvokMapper()
{
  this->FileName = nullptr;
  this->ARenderer = nullptr;
  Controller::Instance().DebugOut()->SetOutput(true, true, true, true);
}

//-----------------------------------------------------------------------------
vtkTuvokMapper::~vtkTuvokMapper()
{
  delete[] this->FileName;
  if (this->ARenderer)
  {
    this->ARenderer->Cleanup();
    Controller::Instance().ReleaseVolumeRenderer(this->ARenderer);
    delete this->ARenderer;
  }
}

//----------------------------------------------------------------------------
void vtkTuvokMapper::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Superclass::ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------------
void vtkTuvokMapper::Render(vtkRenderer *renderer, vtkVolume *vol)
{
  try
  {
    if (!ARenderer)
    {
      glewInit(); // Need to be done with OpenGL1!
      cout << "Initializing Tuvok Renderer at first rendering..." << endl;
      this->ARenderer = Controller::Instance().RequestNewVolumeRenderer(
        MasterController::OPENGL_RAYCASTER, false, false, false, false);
      GLRenderer* ren = dynamic_cast<GLRenderer*>(this->ARenderer);
      cout << "Loading " << this->FileName << endl;
      this->ARenderer->LoadFile(this->FileName);
      UINT64VECTOR3 sz = this->ARenderer->GetDataset().GetDomainSize();
      cout << "Domain extents: " << sz[0] << "x" << sz[1] << "x" << sz[2] << endl;
      this->ARenderer->AddShaderPath("../../../Shaders");
      this->ARenderer->SetRendererTarget(AbstrRenderer::RT_INVALID_MODE); //RT_INVALID_MODE
      this->ARenderer->Initialize(GLContext::Current(0));
      cout << "Done." << endl;
    }

    // Transfer VTK camera information to Tuvok
    vtkCamera* cam = renderer->GetActiveCamera();
    double* vPos = cam->GetPosition();
    double* vFocal = cam->GetFocalPoint();
    double* vUp = cam->GetViewUp();

    /*FLOATVECTOR3 pos = this->ARenderer->GetViewPos();
    FLOATVECTOR3 dir = this->ARenderer->GetViewDir();
    FLOATVECTOR3 up = this->ARenderer->GetUpDir();
    cout << "CAMERA Pos " << pos << endl;
    cout << "CAMERA Dir " << dir << endl;
    cout << "CAMERA Up " << up << endl;
    cout << "VTK CAMERA Pos " << vPos[0] << "\t" << vPos[1] << "\t" << vPos[2] << endl;
    cout << "VTK CAMERA Dir " << vFocal[0] << "\t" << vFocal[1] << "\t" << vFocal[2] << endl;
    cout << "VTK CAMERA Up " << vUp[0] << "\t" << vUp[1] << "\t" << vUp[2] << endl;*/

    this->ARenderer->SetViewPos(FLOATVECTOR3(vPos[0], vPos[1], vPos[2]));
    this->ARenderer->SetViewDir(FLOATVECTOR3(vFocal[0], vFocal[1], vFocal[2]) - FLOATVECTOR3(vPos[0], vPos[1], vPos[2]));
    this->ARenderer->SetUpDir(FLOATVECTOR3(vUp[0], vUp[1], vUp[2]));

    UINTVECTOR2 tuvokSize = this->ARenderer->GetSize();
    int* vtkSize = renderer->GetSize();
    if (tuvokSize[0] != vtkSize[0] || tuvokSize[1] != vtkSize[1])
    {
      // Update Tuvok size when vtkRenderer size changes
      this->ARenderer->Resize(UINTVECTOR2(vtkSize[0], vtkSize[1]));
    }

    this->ARenderer->Paint();
  }
  catch (const std::exception& e)
  {
    cerr << "Exception: " << e.what() << endl;
  }
}

//----------------------------------------------------------------------------
int vtkTuvokMapper::FillInputPortInformation(int port, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

//----------------------------------------------------------------------------
void vtkTuvokMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}