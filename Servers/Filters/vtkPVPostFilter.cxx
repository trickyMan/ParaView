/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkPVPostFilter.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVPostFilter.h"

#include "vtkArrayIteratorIncludes.h"
#include "vtkCellData.h"
#include "vtkCellDataToPointData.h"
#include "vtkCommand.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataArray.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointDataToCellData.h"
#include "vtkPVPostFilterExecutive.h"

#include <vtksys/SystemTools.hxx>
#include <vtkstd/string>
#include <assert.h>

namespace
{
  void DeMangleArrayName(vtkstd::string mangledName, vtkstd::string seperator,
    vtkstd::string& demangled_name, vtkstd::string& demagled_component_name)
    {
    size_t found = mangledName.rfind(seperator);
    if (found == vtkstd::string::npos)
      {
      // no vector conversion needed
      demangled_name = mangledName;
      return;
      }
    demagled_component_name = mangledName.substr(found+1, mangledName.size());
    demangled_name = mangledName.substr(0, found);
    }
}

vtkStandardNewMacro(vtkPVPostFilter);
//----------------------------------------------------------------------------
vtkPVPostFilter::vtkPVPostFilter()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkPVPostFilter::~vtkPVPostFilter()
{
}

//----------------------------------------------------------------------------
vtkExecutive* vtkPVPostFilter::CreateDefaultExecutive()
{
  return vtkPVPostFilterExecutive::New();
}
//----------------------------------------------------------------------------
int vtkPVPostFilter::RequestDataObject(
  vtkInformation*,
  vtkInformationVector** inputVector ,
  vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  if (!inInfo)
    {
    return 0;
    }
  vtkDataObject *input = inInfo->Get(vtkDataObject::DATA_OBJECT());
  if (input)
    {
    // for each output
    for(int i=0; i < this->GetNumberOfOutputPorts(); ++i)
      {
      vtkInformation* info = outputVector->GetInformationObject(i);
      vtkDataObject *output = info->Get(vtkDataObject::DATA_OBJECT());
      if (!output || !output->IsA(input->GetClassName()))
        {
        vtkDataObject* newOutput = input->NewInstance();
        newOutput->SetPipelineInformation(info);
        newOutput->Delete();
        }
      }
    return 1;
    }
  return 0;
}

//----------------------------------------------------------------------------
int vtkPVPostFilter::RequestData(vtkInformation *,
  vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

  //we need to just copy the data, so we can fixup the output as needed
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkDataObject* input= inInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkDataObject* output= outInfo->Get(vtkDataObject::DATA_OBJECT());
  if (output && input)
    {
    output->ShallowCopy(input);
    if (this->Information->Has(vtkPVPostFilterExecutive::POST_ARRAYS_TO_PROCESS()) )
      {
      this->DoAnyNeededConversions(output);
      }
    }
  return 1;
}

//----------------------------------------------------------------------------
int vtkPVPostFilter::DoAnyNeededConversions(vtkDataObject* output)
{
  //get the array to convert info
  vtkInformationVector* postVector =
    this->Information->Get(vtkPVPostFilterExecutive::POST_ARRAYS_TO_PROCESS());
  vtkInformation *postArrayInfo = postVector->GetInformationObject(0);

  const char* name = postArrayInfo->Get(vtkDataObject::FIELD_NAME());
  int fieldAssociation = postArrayInfo->Get(vtkDataObject::FIELD_ASSOCIATION());
  vtkstd::string demangled_name, demagled_component_name;

  DeMangleArrayName(name,
    postArrayInfo->Get(vtkPVPostFilterExecutive::POST_ARRAY_COMPONENT_KEY()),
    demangled_name, demagled_component_name);

  vtkCompositeDataSet* cd = vtkCompositeDataSet::SafeDownCast(output);
  if (cd)
    {
    vtkCompositeDataIterator* iter = cd->NewIterator();
    for (iter->InitTraversal(); !iter->IsDoneWithTraversal();
      iter->GoToNextItem())
      {
      vtkDataSet* dataset = vtkDataSet::SafeDownCast(iter->GetCurrentDataObject());
      if (dataset)
        {
        this->DoAnyNeededConversions(dataset, name, fieldAssociation,
          demangled_name.c_str(), demagled_component_name.c_str());
        }
      }
    iter->Delete();
    return 1;
    }

  return this->DoAnyNeededConversions(vtkDataSet::SafeDownCast(output),
    name, fieldAssociation, demangled_name.c_str(),
    demagled_component_name.c_str());
}

//----------------------------------------------------------------------------
int vtkPVPostFilter::DoAnyNeededConversions(vtkDataSet* output,
  const char* requested_name, int fieldAssociation,
  const char* demangled_name, const char* demagled_component_name)
{
  vtkDataSetAttributes* dsa = NULL;
  vtkDataSetAttributes* pointData = output->GetPointData();
  vtkDataSetAttributes* cellData = output->GetCellData();

  switch (fieldAssociation)
    {
  case vtkDataObject::FIELD_ASSOCIATION_POINTS:
    dsa = pointData;
    break;

  case vtkDataObject::FIELD_ASSOCIATION_CELLS:
    dsa = cellData;
    break;

  case vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS:
    vtkWarningMacro("Case not handled");

  default:
    return 0;
    }

  if (dsa->GetAbstractArray(requested_name))
    {
    // requested array is present. Don't bother doing anything.
    return 0;
    }

  if (dsa->GetAbstractArray(demangled_name))
    {
    // demangled_name is present, implies that component extraction is needed.
    return this->ExtractComponent(dsa, requested_name,
      demangled_name, demagled_component_name);
    }

  if (fieldAssociation == vtkDataObject::FIELD_ASSOCIATION_POINTS)
    {
    if (cellData->GetAbstractArray(requested_name) ||
      cellData->GetAbstractArray(demangled_name))
      {
      this->CellDataToPointData(output);
      }
    }
  else if (fieldAssociation == vtkDataObject::FIELD_ASSOCIATION_CELLS)
    {
    if (pointData->GetAbstractArray(requested_name) ||
      pointData->GetAbstractArray(demangled_name))
      {
      this->PointDataToCellData(output);
      }
    }

  if (dsa->GetAbstractArray(requested_name))
    {
    // after the conversion the requested array is present. Don't bother doing
    // anything more.
    return 1;
    }

  if (dsa->GetAbstractArray(demangled_name))
    {
    // demangled_name is present, implies that component extraction is needed.
    return this->ExtractComponent(dsa, requested_name,
      demangled_name, demagled_component_name);
    }

  return 0;
}

//----------------------------------------------------------------------------
void vtkPVPostFilter::CellDataToPointData(vtkDataSet* output)
{
  vtkDataObject* clone = output->NewInstance();
  clone->ShallowCopy(output);

  vtkCellDataToPointData *converter = vtkCellDataToPointData::New();
  converter->SetInput(clone);
  converter->PassCellDataOn();
  converter->Update();
  output->ShallowCopy(converter->GetOutputDataObject(0));
  converter->Delete();
  clone->Delete();
}

//----------------------------------------------------------------------------
void vtkPVPostFilter::PointDataToCellData(vtkDataSet* output)
{
  vtkDataObject* clone = output->NewInstance();
  clone->ShallowCopy(output);

  vtkPointDataToCellData *converter = vtkPointDataToCellData::New();
  converter->SetInput(clone);
  converter->PassPointDataOn();
  converter->Update();
  output->ShallowCopy(converter->GetOutputDataObject(0));
  converter->Delete();
  clone->Delete();
}

//----------------------------------------------------------------------------
namespace
{
  template <class T>
  void CopyComponent(T* outIter, T* inIter, int compNo)
    {
    vtkIdType numTuples = inIter->GetNumberOfTuples();
    for (vtkIdType cc=0; cc < numTuples; cc++)
      {
      outIter->SetValue(cc, inIter->GetTuple(cc)[compNo]);
      }
    }
}

//----------------------------------------------------------------------------
int vtkPVPostFilter::ExtractComponent(vtkDataSetAttributes* dsa,
  const char* requested_name, const char* demangled_name,
  const char* demagled_component_name)
{
  vtkAbstractArray* array = dsa->GetAbstractArray(demangled_name);
  assert(array != NULL && demangled_name && demagled_component_name);

  int cIndex = -1;
  // demagled_component_name can be a real component name OR
  // X,Y,Z for the first 3 components or
  // 0,...N i.e. an integer for the index.
  // Now to the trick is to decide what way this particular request has been
  // made.
  for (int cc=0; cc < array->GetNumberOfComponents(); cc++)
    {
    const char* comp_name = array->GetComponentName(cc);
    if (comp_name && strcmp(comp_name, demagled_component_name) == 0)
      {
      cIndex = cc;
      break;
      }
    }
  if (cIndex == -1)
    {
    const char* default_names[3];
    default_names[0] = "x";
    default_names[1] = "y";
    default_names[2] = "z";
    for (int cc=0; cc < 3; cc++)
      {
      if (vtksys::SystemTools::Strucmp(demagled_component_name, default_names[cc]) == 0)
        {
        cIndex = cc;
        break;
        }
      }
    }
  if (cIndex == -1)
    {
    cIndex = atoi(demagled_component_name);
    }

  vtkAbstractArray* newArray = array->NewInstance();
  newArray->SetNumberOfComponents(1);
  newArray->SetNumberOfTuples(array->GetNumberOfTuples());
  newArray->SetName(requested_name);

  vtkArrayIterator* inIter = array->NewIterator();
  vtkArrayIterator* outIter = newArray->NewIterator();
  switch (array->GetDataType())
    {
    vtkArrayIteratorTemplateMacro(
      ::CopyComponent(static_cast<VTK_TT*>(outIter),
        static_cast<VTK_TT*>(inIter), cIndex);
    );
    }
  inIter->Delete();
  outIter->Delete();
  dsa->AddArray(newArray);
  newArray->FastDelete();
  return 1;
}

//----------------------------------------------------------------------------
void vtkPVPostFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
