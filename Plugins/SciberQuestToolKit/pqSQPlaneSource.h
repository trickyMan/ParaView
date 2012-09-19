/*
   ____    _ __           ____               __    ____
  / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
 _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
/___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_)

Copyright 2012 SciberQuest Inc.

*/
#ifndef pqSQPlaneSource_h
#define pqSQPlaneSource_h

#include "pqNamedObjectPanel.h"

#include "ui_pqSQPlaneSourceForm.h"
using Ui::pqSQPlaneSourceForm;

class pqProxy;
class pqPropertyLinks;
class vtkEventQtSlotConnect;
class QWidget;
class QContextMenuEvent;

class pqSQPlaneSource : public pqNamedObjectPanel
{
  Q_OBJECT
public:
  pqSQPlaneSource(pqProxy* proxy, QWidget* p = NULL);
  ~pqSQPlaneSource();

  // Description:
  // Set/Get values to/from the UI.
  void GetOrigin(double *o);
  void SetOrigin(double *o);

  void GetPoint1(double *p1);
  void SetPoint1(double *p1);

  void GetPoint2(double *p2);
  void SetPoint2(double *p2);

  void GetResolution(int *res);
  void SetResolution(int *res);

  void GetSpacing(double *dx);
  void SetSpacing(double *dx);

  void GetNormal(double *n);
  void SetNormal(double *n);

  int GetConstraint();
  void SetConstraint(int type);

  int GetDecompType();
  void SetDecompType(int type);

  void contextMenuEvent(QContextMenuEvent *event);

protected slots:
  // Description:
  // read state from disk.
  void Restore();
  void loadConfiguration();
  // Description:
  // write state to disk.
  void Save();
  void saveConfiguration();

  // Description:
  // check if cooridnates produce a good plane.
  int ValidateCoordinates();

  // Description:
  // calculate plane's dimensions for display. Retun 0 in case
  // an error occured.
  void DimensionsModified();

  // Description:
  // calculate the normal to the plane for display. Return 0
  // if the coordinate system is invalid.
  int CalculateNormal(double *n);

  // Description:
  // update and display computed values, and enforce aspect ratio lock.
  void SpacingModified();
  void ResolutionModified();

  // Description:
  // snap camera to the plane normal.
  void SnapViewToNormal();

  // Description:
  // restricts UI based on current constraints.
  void ApplyConstraint();

  // Description:
  // Update the UI with values from the server.
  void PullServerConfig();
  void PushServerConfig();

  // Description:
  // This is where we have to communicate our state to the server.
  virtual void accept();

  // Description:
  // Pull config from proxy
  virtual void reset();

  // Description:
  // Copy/Paste the UI state to the clip board
  void CopyConfiguration();
  void PasteConfiguration();

  // Description:
  // Apply a translation to the current coordinates.
  void ShowTranslateDialog();

private:
  double N[3];
  double Dims[2];
  double Dx[2];
  int Nx[2];

private:
  pqSQPlaneSourceForm *Form;
  vtkEventQtSlotConnect *VTKConnect;
  pqPropertyLinks *Links;
};

#endif
