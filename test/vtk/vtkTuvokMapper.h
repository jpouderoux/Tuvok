/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTuvokMapper.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkTuvokMapper
 * @brief   Mapper for volume rendering with Tuvok.
 *
 * @par Thanks:
 * This class was written by Joachim Pouderoux, Kitware 2017
 *
 * @sa
 * vtkVolumeMapper
 */

#ifndef vtkTuvokMapper_h
#define vtkTuvokMapper_h

#include "vtkVolumeMapper.h"

class vtkActor;
class vtkDataSet;
class vtkRenderer;

namespace tuvok
{
  class AbstrRenderer;
}

class VTK_EXPORT vtkTuvokMapper : public vtkVolumeMapper
{
public:
  static vtkTuvokMapper* New();
  vtkTypeMacro(vtkTuvokMapper, vtkVolumeMapper);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  //@{
  /**
   * Specify the UVF file to display.
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

  /**
   * WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
   * DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS
   * Render the volume
   */
  void Render(vtkRenderer *ren, vtkVolume *vol) override;

  /**
   * WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
   * Release any graphics resources that are being consumed by this mapper.
   * The parameter window could be used to determine which graphic
   * resources to release.
   */
  void ReleaseGraphicsResources(vtkWindow *) override;

  /**
   * Shallow copy of an actor.
   */
  void ShallowCopy(vtkAbstractMapper *);

protected:
  vtkTuvokMapper();
  ~vtkTuvokMapper() override;

  int FillInputPortInformation(int, vtkInformation*) override;

  vtkTuvokMapper(const vtkTuvokMapper&) = delete;
  void operator=(const vtkTuvokMapper&) = delete;

  tuvok::AbstrRenderer* ARenderer;
  char* FileName;
};

#endif
