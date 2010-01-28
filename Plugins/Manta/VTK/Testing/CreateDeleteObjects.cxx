#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkMantaActor.h"
#include "vtkMantaPolyDataMapper.h"
#include "vtkMantaRenderer.h"
#include "vtkMantaRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSphereSource.h"

// this program tests creating and deleting objects

bool useGL = false;

vtkPolyDataMapper *makeMapper()
{
  if (useGL)
    {
    return vtkPolyDataMapper::New();
    }
  else
    {
    return vtkMantaPolyDataMapper::New();
    }
}

vtkActor *makeActor()
{
  if (useGL)
    {
    return vtkActor::New();
    }
  else
    {
    return vtkMantaActor::New();
    }
}

vtkRenderer *makeRenderer()
{
  if (useGL)
    {
    return vtkRenderer::New();
    }
  else
    {
    return vtkMantaRenderer::New();
    }
}

vtkRenderWindow *makeRenderWindow()
{
  if (useGL)
    {
    return vtkRenderWindow::New();
    }
  else
    {
    return vtkMantaRenderWindow::New();
    }
}

//----------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  int     objRes = 12;
  double  objRad = 0.075;

  for (int i = 0; i < argc; i++)
    {
    if (!strcmp(argv[i], "-useGL"))
      {
      useGL = true;
      }
    }

  vtkRenderer * renderer = makeRenderer();
  renderer->SetBackground( 0.0, 0.0, 1.0 );

  vtkRenderWindow * renWin = makeRenderWindow();
  renWin->AddRenderer( renderer );
  renWin->SetSize( 400, 400 );

  // create cone
  vtkConeSource * cone= vtkConeSource::New();
  cone->SetRadius( objRad );
  cone->SetHeight( objRad * 2 );
  cone->SetResolution( objRes );

  vtkPolyDataMapper * coneMapper = makeMapper();
  coneMapper->SetInputConnection( cone->GetOutputPort() );

  vtkActor * coneActor = makeActor();
  coneActor->SetMapper( coneMapper );
  coneActor->AddPosition( 0.0, objRad * 2.0, 0.0 );
  coneActor->RotateZ( 90.0 );

  renderer->AddActor( coneActor );

  // create sphere
  vtkSphereSource * sphere = vtkSphereSource::New();
  sphere->SetCenter( 0.0, 0.0, 0.0 );
  sphere->SetRadius( objRad );
  sphere->SetThetaResolution( objRes );
  sphere->SetPhiResolution  ( objRes );

  vtkPolyDataMapper * sphereMapper = makeMapper();
  sphereMapper->SetInputConnection( sphere->GetOutputPort() );

  vtkActor * sphereActor = makeActor();
  sphereActor->SetMapper( sphereMapper );

  renderer->AddActor( sphereActor );

  // create cylinder
  vtkCylinderSource * cylinder = vtkCylinderSource::New();
  cylinder->SetCenter( 0.0, -objRad * 2, 0.0 );
  cylinder->SetRadius( objRad );
  cylinder->SetHeight( objRad * 2 );
  cylinder->SetResolution( objRes );

  vtkPolyDataMapper * cylinderMapper = makeMapper();
  cylinderMapper->SetInputConnection( cylinder->GetOutputPort() );

  vtkActor * cylinderActor = makeActor();
  cylinderActor->SetMapper( cylinderMapper );

  renderer->AddActor( cylinderActor );

  renWin->Render();

  // delete cone
  renderer->RemoveActor( coneActor );
  cone->Delete();
  coneMapper->Delete();
  coneActor->Delete();

  renWin->Render();

  // delete sphere
  renderer->RemoveActor( sphereActor );
  sphere->Delete();
  sphereMapper->Delete();
  sphereActor->Delete();

  renWin->Render();

  // delete cylinder
  renderer->RemoveActor( cylinderActor );
  cylinder->Delete();
  cylinderMapper->Delete();
  cylinderActor->Delete();

  renWin->Render();

  // re create sphere
  sphere = vtkSphereSource::New();
  sphere->SetCenter( 0.0, 0.0, 0.0 );
  sphere->SetRadius( objRad );
  sphere->SetThetaResolution( objRes );
  sphere->SetPhiResolution  ( objRes );

  sphereMapper = makeMapper();
  sphereMapper->SetInputConnection( sphere->GetOutputPort() );

  sphereActor = makeActor();
  sphereActor->SetMapper( sphereMapper );

  renderer->AddActor( sphereActor );

  renWin->Render();

  //re-delete sphere
  sphere->Delete();
  sphereMapper->Delete();
  sphereActor->Delete();

  renderer->Delete();
  renWin->Delete();

  return 0;
}
