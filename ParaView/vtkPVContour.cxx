/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPVContour.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 1998-2000 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/
#include "vtkPVContour.h"
#include "vtkPVApplication.h"
#include "vtkStringList.h"

int vtkPVContourCommand(ClientData cd, Tcl_Interp *interp,
                        int argc, char *argv[]);

//----------------------------------------------------------------------------
vtkPVContour::vtkPVContour()
{
  this->CommandFunction = vtkPVContourCommand;
  
  this->ContourValuesLabel = vtkKWLabel::New();
  this->ContourValuesList = vtkKWListBox::New();
  this->NewValueFrame = vtkKWWidget::New();
  this->NewValueLabel = vtkKWLabel::New();
  this->NewValueEntry = vtkKWEntry::New();
  this->AddValueButton = vtkKWPushButton::New();
  this->DeleteValueButton = vtkKWPushButton::New();
  this->ComputeNormalsCheck = vtkKWCheckButton::New();
  this->ComputeGradientsCheck = vtkKWCheckButton::New();
  this->ComputeScalarsCheck = vtkKWCheckButton::New();
}

//----------------------------------------------------------------------------
vtkPVContour::~vtkPVContour()
{
  this->ContourValuesLabel->Delete();
  this->ContourValuesLabel = NULL;
  this->ContourValuesList->Delete();
  this->ContourValuesList = NULL;
  this->NewValueLabel->Delete();
  this->NewValueLabel = NULL;
  this->NewValueEntry->Delete();
  this->NewValueEntry = NULL;
  this->AddValueButton->Delete();
  this->AddValueButton = NULL;
  this->NewValueFrame->Delete();
  this->NewValueFrame = NULL;
  this->DeleteValueButton->Delete();
  this->DeleteValueButton = NULL;
  this->ComputeNormalsCheck->Delete();
  this->ComputeNormalsCheck = NULL;
  this->ComputeGradientsCheck->Delete();
  this->ComputeGradientsCheck = NULL;
  this->ComputeScalarsCheck->Delete();
  this->ComputeScalarsCheck = NULL;
}

//----------------------------------------------------------------------------
vtkPVContour* vtkPVContour::New()
{
  return new vtkPVContour();
}

//----------------------------------------------------------------------------
void vtkPVContour::CreateProperties()
{
  vtkPVApplication* pvApp = this->GetPVApplication();
  
  this->vtkPVSource::CreateProperties();
  
  this->ContourValuesLabel->SetParent(this->GetParameterFrame()->GetFrame());
  this->ContourValuesLabel->Create(pvApp, "");
  this->ContourValuesLabel->SetLabel("Contour Values");
  
  this->ContourValuesList->SetParent(this->GetParameterFrame()->GetFrame());
  this->ContourValuesList->Create(pvApp, "");
  this->ContourValuesList->SetHeight(5);
  
  this->AcceptCommands->AddString("%s ContourValuesAcceptCallback",
                                  this->GetTclName());
  this->CancelCommands->AddString("%s ContourValuesCancelCallback",
                                  this->GetTclName());
  
  this->NewValueFrame->SetParent(this->GetParameterFrame()->GetFrame());
  this->NewValueFrame->Create(pvApp, "frame", "");
  
  this->Script("pack %s %s %s", this->ContourValuesLabel->GetWidgetName(),
               this->ContourValuesList->GetWidgetName(),
               this->NewValueFrame->GetWidgetName());
  
  this->NewValueLabel->SetParent(this->NewValueFrame);
  this->NewValueLabel->Create(pvApp, "");
  this->NewValueLabel->SetLabel("New Value:");
  
  this->NewValueEntry->SetParent(this->NewValueFrame);
  this->NewValueEntry->Create(pvApp, "");
  this->NewValueEntry->SetValue("");
  
  this->AddValueButton->SetParent(this->NewValueFrame);
  this->AddValueButton->Create(pvApp, "-text \"Add Value\"");
  this->AddValueButton->SetCommand(this, "AddValueCallback");
  
  this->Script("pack %s %s %s -side left",
               this->NewValueLabel->GetWidgetName(),
               this->NewValueEntry->GetWidgetName(),
               this->AddValueButton->GetWidgetName());
  
  this->DeleteValueButton->SetParent(this->GetParameterFrame()->GetFrame());
  this->DeleteValueButton->Create(pvApp, "-text \"Delete Value\"");
  this->DeleteValueButton->SetCommand(this, "DeleteValueCallback");
  
  this->ComputeNormalsCheck->SetParent(this->GetParameterFrame()->GetFrame());
  this->ComputeNormalsCheck->Create(pvApp, "-text \"Compute Normals\"");
  this->ComputeNormalsCheck->SetState(1);
  
  this->CancelCommands->AddString("%s SetState [%s %s]",
                                  this->ComputeNormalsCheck->GetTclName(),
                                  this->GetVTKSourceTclName(),
                                  "GetComputeNormals");
  this->AcceptCommands->AddString("%s AcceptHelper2 %s %s [%s GetState]",
                                  this->GetTclName(),
                                  this->GetVTKSourceTclName(),
                                  "SetComputeNormals",
                                  this->ComputeNormalsCheck->GetTclName());

  this->ComputeGradientsCheck->SetParent(this->GetParameterFrame()->GetFrame());
  this->ComputeGradientsCheck->Create(pvApp, "-text \"Compute Gradients\"");
  this->ComputeGradientsCheck->SetState(0);
  
  this->CancelCommands->AddString("%s SetState [%s %s]",
                                  this->ComputeGradientsCheck->GetTclName(),
                                  this->GetVTKSourceTclName(),
                                  "GetComputeGradients");
  this->AcceptCommands->AddString("%s AcceptHelper2 %s %s [%s GetState]",
                                  this->GetTclName(),
                                  this->GetVTKSourceTclName(),
                                  "SetComputeGradients",
                                  this->ComputeGradientsCheck->GetTclName());

  this->ComputeScalarsCheck->SetParent(this->GetParameterFrame()->GetFrame());
  this->ComputeScalarsCheck->Create(pvApp, "-text \"Compute Scalars\"");
  this->ComputeScalarsCheck->SetState(1);
  
  this->CancelCommands->AddString("%s SetState [%s %s]",
                                  this->ComputeScalarsCheck->GetTclName(),
                                  this->GetVTKSourceTclName(),
                                  "GetComputeScalars");
  this->AcceptCommands->AddString("%s AcceptHelper2 %s %s [%s GetState]",
                                  this->GetTclName(),
                                  this->GetVTKSourceTclName(),
                                  "SetComputeScalars",
                                  this->ComputeScalarsCheck->GetTclName());

  this->Script("pack %s %s %s %s -anchor w -padx 10",
               this->DeleteValueButton->GetWidgetName(),
               this->ComputeNormalsCheck->GetWidgetName(),
               this->ComputeGradientsCheck->GetWidgetName(),
               this->ComputeScalarsCheck->GetWidgetName());
}

void vtkPVContour::AddValueCallback()
{
  if (strcmp(this->NewValueEntry->GetValue(), "") == 0)
    {
    return;
    }

  this->ContourValuesList->AppendUnique(this->NewValueEntry->GetValue());
  this->NewValueEntry->SetValue("");
}

void vtkPVContour::DeleteValueCallback()
{
  int index;
  
  index = this->ContourValuesList->GetSelectionIndex();
  this->ContourValuesList->DeleteRange(index, index);
}

void vtkPVContour::ContourValuesAcceptCallback()
{
  int i;
  float value;
  int numContours = this->ContourValuesList->GetNumberOfItems();
  vtkPVApplication *pvApp = this->GetPVApplication();

  pvApp->BroadcastScript("%s SetNumberOfContours %d",
                         this->GetVTKSourceTclName(), numContours);
  
  for (i = 0; i < numContours; i++)
    {
    value = atof(this->ContourValuesList->GetItem(i));
    pvApp->BroadcastScript("%s SetValue %d %f",
                           this->GetVTKSourceTclName(),
                           i, value);
    }
}

void vtkPVContour::ContourValuesCancelCallback()
{
  int i;
  vtkKitwareContourFilter* contour =
    (vtkKitwareContourFilter*)this->GetVTKSource();
  int numContours = contour->GetNumberOfContours();
  char newValue[128];
  
  this->ContourValuesList->DeleteAll();
  
  for (i = 0; i < numContours; i++)
    {
    sprintf(newValue, "%f", contour->GetValue(i));
    this->ContourValuesList->AppendUnique(newValue);
    }
}

void vtkPVContour::Save(ofstream* file)
{
  char sourceTclName[256];
  char* tempName;
  int i;
  vtkKitwareContourFilter *source =
    (vtkKitwareContourFilter*)this->GetVTKSource();

  if (this->DefaultScalarsName)
    {
    *file << "vtkSimpleFieldDataToAttributeDataFilter "
          << this->ChangeScalarsFilterTclName << "\n\t"
          << this->ChangeScalarsFilterTclName << " SetInput [";
    if (strncmp(this->GetNthPVInput(0)->GetVTKDataTclName(),
                "EnSight", 7) == 0)
      {
      char *charFound;
      int pos;
      char *dataName = this->GetNthPVInput(0)->GetVTKDataTclName();
      
      sprintf(sourceTclName, "EnSightReader");
      tempName = strtok(dataName, "O");
      strcat(sourceTclName, tempName+7);
      *file << sourceTclName << " GetOutput ";
      charFound = strrchr(dataName, 't');
      pos = charFound - dataName + 1;
      *file << dataName+pos << "]\n\t";
      }
    else if (strncmp(this->GetNthPVInput(0)->GetVTKDataTclName(),
                     "DataSet", 7) == 0)
      {
      sprintf(sourceTclName, "DataSetReader");
      tempName = strtok(this->GetNthPVInput(0)->GetVTKDataTclName(), "O");
      strcat(sourceTclName, tempName+7);
      *file << sourceTclName << " GetOutput]\n\t";
      }
    else
      {
      *file << this->GetNthPVInput(0)->GetPVSource()->GetVTKSourceTclName()
            << " GetOutput]\n\t";
      }
    *file << this->ChangeScalarsFilterTclName << " SetFieldName "
          << this->DefaultScalarsName << "\n\n";
    }

  *file << this->VTKSource->GetClassName() << " "
        << this->VTKSourceTclName << "\n";

  *file << "\t" << this->VTKSourceTclName << " SetInput [";

  if (!this->DefaultScalarsName)
    {
    if (strncmp(this->GetNthPVInput(0)->GetVTKDataTclName(),
                "EnSight", 7) == 0)
      {
      char *charFound;
      int pos;
      char *dataName = this->GetNthPVInput(0)->GetVTKDataTclName();
      
      sprintf(sourceTclName, "EnSightReader");
      tempName = strtok(dataName, "O");
      strcat(sourceTclName, tempName+7);
      *file << sourceTclName << " GetOutput ";
      charFound = strrchr(dataName, 't');
      pos = charFound - dataName + 1;
      *file << dataName+pos << "]\n\t";
      }
    else if (strncmp(this->GetNthPVInput(0)->GetVTKDataTclName(),
                     "DataSet", 7) == 0)
      {
      sprintf(sourceTclName, "DataSetReader");
      tempName = strtok(this->GetNthPVInput(0)->GetVTKDataTclName(), "O");
      strcat(sourceTclName, tempName+7);
      *file << sourceTclName << " GetOutput]\n\t";
      }
    else
      {
      *file << this->GetNthPVInput(0)->GetPVSource()->GetVTKSourceTclName()
            << " GetOutput]\n\t";
      }
    }
  else
    {
    *file << this->ChangeScalarsFilterTclName << " GetOutput]\n\t";
    }
  
  *file << this->VTKSourceTclName << " SetNumberOfContours "
        << source->GetNumberOfContours() << "\n\t";
  
  for (i = 0; i < source->GetNumberOfContours(); i++)
    {
    *file << this->VTKSourceTclName << " SetValue " << i << " "
          << source->GetValue(i) << "\n\t";
    }

  *file << this->VTKSourceTclName << " SetComputeNormals "
        << this->ComputeNormalsCheck->GetState() << "\n\t"
        << this->VTKSourceTclName << " SetComputeGradients "
        << this->ComputeGradientsCheck->GetState() << "\n\t"
        << this->VTKSourceTclName << " SetComputeScalars "
        << this->ComputeScalarsCheck->GetState() << "\n\n";
  
  this->GetPVOutput(0)->Save(file, this->VTKSourceTclName);
}
