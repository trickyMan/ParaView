/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkKWTextProperty.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2000-2001 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither the name of Kitware nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific 
   prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "vtkKWTextProperty.h"

#include "vtkKWApplication.h"
#include "vtkKWChangeColorButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWOptionMenu.h"
#include "vtkKWTkUtilities.h"
#include "vtkObjectFactory.h"
#include "vtkTextProperty.h"

#define VTK_KW_TEXT_PROP_ARIAL   "Arial"
#define VTK_KW_TEXT_PROP_COURIER "Courier"
#define VTK_KW_TEXT_PROP_TIMES   "Times"

/* 
 * This part was generated by ImageConvert from image:
 *    bold.png (zlib, base64)
 */
#define image_bold_width         16
#define image_bold_height        16
#define image_bold_pixel_size    4
#define image_bold_buffer_length 156

static unsigned char image_bold[] = 
  "eNr7//8/w/9BgIFACYjTgPgdEP+H4nIgngkVOwPlgwEW/TCwG0k/DBijmUmqfmTxdwOgH9"
  "n9aSToh4UfiH0XiF3ICD8lJDPekWg/MkCOVxcKwg/sLzL0n0EOQwrC/wwO/2NLv6uAuAMt"
  "/Qpi0z9QGAAGrJTx";

/* 
 * This part was generated by ImageConvert from image:
 *    italic.png (zlib, base64)
 */
#define image_italic_width         16
#define image_italic_height        16
#define image_italic_pixel_size    4
#define image_italic_buffer_length 144

static unsigned char image_italic[] = 
  "eNr7//8/w/9BgrEAFyC+C8QgyQ4gViJRPwjshpoBBmTofwfEM8nU7wJ1eyiZ+jug+gXJ1H"
  "8G6n8GMvQrQe0uJ1N/GlS/Epn6VyHHGxn630HDjxz9oejxRqR+QaieM8hpllj9A4EBZtrP"
  "lA==";

/* 
 * This part was generated by ImageConvert from image:
 *    shadow.png (zlib, base64)
 */
#define image_shadow_width         16
#define image_shadow_height        16
#define image_shadow_pixel_size    4
#define image_shadow_buffer_length 392

static unsigned char image_shadow[] = 
  "eNrN0j1LglEYxnEtpTQEtSgSQ4ieKCMaKnqBhqIhIZWmsFkMAqeCos1FoqFmv0JLk1OL+A"
  "n6Bo1OUeDQ+Pi/4Rqczlk78OPBl+uc+7nvE4ZhJPwntO7R0fMNTziNjC1HvqFMHFksoo8a"
  "FpDCpCP/gWfkcYBDPOBWNawh4cjb2T/6/wW2kMMVzrFrNTjy2/i1V0QXO1Yv0tjU+TOe/p"
  "XwpT2GuLGM9pjFlCdvPTrCO/60TxvzmMaEI79sv2MOx3jEAN+oYgkxR95mHtX8rN4NvKiG"
  "Fsr2Dp752dwyqjWmWfQ0xyYCz/w+8YpLXGvPVX22XhYdeTu3gDruNIu0+lJRD/Ke/tudPd"
  "P92VcP9nCCdc/9s5XQ/Q2womeg75LW3/HMCIV8hNo=";

/* 
 * This part was generated by ImageConvert from image:
 *    copy.png (zlib, base64)
 */
#define image_copy_width         16
#define image_copy_height        16
#define image_copy_pixel_size    4
#define image_copy_buffer_length 128

static unsigned char image_copy[] = 
  "eNqtkwEKwCAMA/f0/jwbOFitKU3phEMRkkNUABcGPAOMTj6OTkeWd7OUd853bRTFvzqM7s"
  "WO6P+wtNN3VP79XOdZOn7SL/vD29jyHM2f36vmr/ITfwWGf+8PblC4qYU=";

// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWTextProperty);
vtkCxxRevisionMacro(vtkKWTextProperty, "1.1.2.1");

int vtkKWTextPropertyCommand(ClientData cd, Tcl_Interp *interp,
                      int argc, char *argv[]);

// ----------------------------------------------------------------------------
vtkKWTextProperty::vtkKWTextProperty()
{
  this->TextProperty = NULL;

  this->ShowColor = 1;
  this->ChangeColorButton = vtkKWChangeColorButton::New();

  this->ShowFontFamily = 1;
  this->FontFamilyOptionMenu = vtkKWOptionMenu::New();

  this->ShowStyles = 1;
  this->StylesFrame = vtkKWWidget::New();
  this->BoldCheckButton = vtkKWCheckButton::New();
  this->ItalicCheckButton = vtkKWCheckButton::New();
  this->ShadowCheckButton = vtkKWCheckButton::New();

  this->ShowHorizontalJustification = 1;
  this->HorizontalJustificationOptionMenu = vtkKWOptionMenu::New();

  this->ShowVerticalJustification = 1;
  this->VerticalJustificationOptionMenu = vtkKWOptionMenu::New();

  this->OnChangeCommand = NULL;

  this->ShowCopy = 0;
  this->CopyButton = vtkKWPushButton::New();
}

// ----------------------------------------------------------------------------
vtkKWTextProperty::~vtkKWTextProperty()
{
  this->SetTextProperty(NULL);

  this->ChangeColorButton->Delete();
  this->ChangeColorButton = NULL;

  this->FontFamilyOptionMenu->Delete();
  this->FontFamilyOptionMenu = NULL;

  this->StylesFrame->Delete();
  this->StylesFrame = NULL;
  this->BoldCheckButton->Delete();
  this->BoldCheckButton = NULL;
  this->ItalicCheckButton->Delete();
  this->ItalicCheckButton = NULL;
  this->ShadowCheckButton->Delete();
  this->ShadowCheckButton = NULL;

  this->HorizontalJustificationOptionMenu->Delete();
  this->HorizontalJustificationOptionMenu = NULL;

  this->VerticalJustificationOptionMenu->Delete();
  this->VerticalJustificationOptionMenu = NULL;

  this->SetOnChangeCommand(NULL);

  this->CopyButton->Delete();
  this->CopyButton = NULL;
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::Create(vtkKWApplication *app)
{
  // Set application

  if (this->Application)
    {
    vtkErrorMacro("vtkKWTextProperty already created");
    return;
    }
  this->SetApplication(app);

  // Create the top level widget

  const char *wname = this->GetWidgetName();
  this->Script("frame %s -borderwidth 0 -relief flat", wname);

  // Color

  this->ChangeColorButton->SetParent(this);
  this->ChangeColorButton->SetText("");
  this->ChangeColorButton->Create(this->Application, "");
  this->ChangeColorButton->SetCommand(this, "ChangeColorButtonCallback");
  this->ChangeColorButton->SetBalloonHelpString("Select the text color.");

  // Font Family

  this->FontFamilyOptionMenu->SetParent(this);
  this->FontFamilyOptionMenu->Create(
    this->Application, "-width 7 -indicator 0 -padx 0 -pady 0");
  this->FontFamilyOptionMenu->SetBalloonHelpString("Select the font family.");
  this->FontFamilyOptionMenu->AddEntryWithCommand(
    VTK_KW_TEXT_PROP_ARIAL, this, "FontFamilyOptionMenuCallback");
  this->FontFamilyOptionMenu->AddEntryWithCommand(
    VTK_KW_TEXT_PROP_COURIER, this, "FontFamilyOptionMenuCallback");
  this->FontFamilyOptionMenu->AddEntryWithCommand(
    VTK_KW_TEXT_PROP_TIMES, this, "FontFamilyOptionMenuCallback");

  // Styles (bold, italic, shadow)

  this->StylesFrame->SetParent(this);
  this->StylesFrame->Create(
    this->Application, "frame", "-borderwidth 0 -relief flat");

  const char *styles_options = "-indicator 0 -padx 0 -pady 0";
  this->BoldCheckButton->SetParent(this->StylesFrame);
  this->BoldCheckButton->Create(this->Application, styles_options);
  this->BoldCheckButton->SetCommand(this, "BoldCheckButtonCallback");
  this->BoldCheckButton->SetBalloonHelpString("Select bold style.");
  ostrstream bold;
  bold << this->BoldCheckButton->GetWidgetName() << ".boldimg" << ends;
  this->Script("image create photo %s", bold.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     bold.str(), 
                                     image_bold, 
                                     image_bold_width, 
                                     image_bold_height, 
                                     image_bold_pixel_size,
                                     image_bold_buffer_length,
                                     this->BoldCheckButton->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (bold)");
    this->BoldCheckButton->SetText("Bold");
    }
  else
    {
    this->Script("%s configure -image %s", 
                 this->BoldCheckButton->GetWidgetName(), bold.str());
    }
  bold.rdbuf()->freeze(0);
  ostrstream bold_s;
  bold_s << this->BoldCheckButton->GetWidgetName() << ".bold_simg" << ends;
  this->Script("image create photo %s", bold_s.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     bold_s.str(), 
                                     image_bold, 
                                     image_bold_width, 
                                     image_bold_height, 
                                     image_bold_pixel_size,
                                     image_bold_buffer_length,
                                     this->BoldCheckButton->GetWidgetName(),
                                     "-selectcolor"))
    {
    vtkWarningMacro(<< "Error creating photo (bold)");
    }
  else
    {
    this->Script("%s configure -selectimage %s", 
                 this->BoldCheckButton->GetWidgetName(), bold_s.str());
    }
  bold_s.rdbuf()->freeze(0);

  this->ItalicCheckButton->SetParent(this->StylesFrame);
  this->ItalicCheckButton->Create(this->Application,  styles_options);
  this->ItalicCheckButton->SetCommand(this, "ItalicCheckButtonCallback");
  this->ItalicCheckButton->SetBalloonHelpString("Select italic style.");
  ostrstream italic;
  italic << this->ItalicCheckButton->GetWidgetName() << ".italicimg" << ends;
  this->Script("image create photo %s", italic.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     italic.str(), 
                                     image_italic, 
                                     image_italic_width, 
                                     image_italic_height, 
                                     image_italic_pixel_size,
                                     image_italic_buffer_length,
                                     this->ItalicCheckButton->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (italic)");
    this->ItalicCheckButton->SetText("Italic");
    }
  else
    {
    this->Script("%s configure -image %s", 
                 this->ItalicCheckButton->GetWidgetName(), italic.str());
    }
  italic.rdbuf()->freeze(0);
  ostrstream italic_s;
  italic_s << this->ItalicCheckButton->GetWidgetName() 
           << ".italic_simg" << ends;
  this->Script("image create photo %s", italic_s.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     italic_s.str(), 
                                     image_italic, 
                                     image_italic_width, 
                                     image_italic_height, 
                                     image_italic_pixel_size,
                                     image_italic_buffer_length,
                                     this->ItalicCheckButton->GetWidgetName(),
                                     "-selectcolor"))
    {
    vtkWarningMacro(<< "Error creating photo (italic)");
    }
  else
    {
    this->Script("%s configure -selectimage %s", 
                 this->ItalicCheckButton->GetWidgetName(), italic_s.str());
    }
  italic_s.rdbuf()->freeze(0);

  this->ShadowCheckButton->SetParent(this->StylesFrame);
  this->ShadowCheckButton->Create(this->Application,  styles_options);
  this->ShadowCheckButton->SetCommand(this, "ShadowCheckButtonCallback");
  this->ShadowCheckButton->SetBalloonHelpString("Select shadow style.");
  ostrstream shadow;
  shadow << this->ShadowCheckButton->GetWidgetName() << ".shadowimg" << ends;
  this->Script("image create photo %s", shadow.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     shadow.str(), 
                                     image_shadow, 
                                     image_shadow_width, 
                                     image_shadow_height, 
                                     image_shadow_pixel_size,
                                     image_shadow_buffer_length,
                                     this->ShadowCheckButton->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (shadow)");
    this->ShadowCheckButton->SetText("Shadow");
    }
  else
    {
    this->Script("%s configure -image %s", 
                 this->ShadowCheckButton->GetWidgetName(), shadow.str());
    }
  shadow.rdbuf()->freeze(0);
  ostrstream shadow_s;
  shadow_s << this->ShadowCheckButton->GetWidgetName() 
           << ".shadow_simg" << ends;
  this->Script("image create photo %s", shadow_s.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     shadow_s.str(), 
                                     image_shadow, 
                                     image_shadow_width, 
                                     image_shadow_height, 
                                     image_shadow_pixel_size,
                                     image_shadow_buffer_length,
                                     this->ShadowCheckButton->GetWidgetName(),
                                     "-selectcolor"))
    {
    vtkWarningMacro(<< "Error creating photo (shadow)");
    }
  else
    {
    this->Script("%s configure -selectimage %s", 
                 this->ShadowCheckButton->GetWidgetName(), shadow_s.str());
    }
  shadow_s.rdbuf()->freeze(0);

  this->Script("pack %s %s %s -side left -fill both",
               this->BoldCheckButton->GetWidgetName(),
               this->ItalicCheckButton->GetWidgetName(),
               this->ShadowCheckButton->GetWidgetName());

  // Copy button

  this->CopyButton->SetParent(this);
  this->CopyButton->Create(this->Application, "");
  ostrstream copy;
  copy << this->CopyButton->GetWidgetName() << ".copyimg" << ends;
  this->Script("image create photo %s", copy.str());
  if (!vtkKWTkUtilities::UpdatePhoto(this->Application->GetMainInterp(),
                                     copy.str(), 
                                     image_copy, 
                                     image_copy_width, 
                                     image_copy_height, 
                                     image_copy_pixel_size,
                                     image_copy_buffer_length,
                                     this->CopyButton->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (copy)");
    this->CopyButton->SetLabel("Copy");
    }
  else
    {
    this->Script("%s configure -image %s", 
                 this->CopyButton->GetWidgetName(), copy.str());
    }
  copy.rdbuf()->freeze(0);

  // Pack

  this->Script("grid %s %s %s %s -sticky news -padx 1",
               this->ChangeColorButton->GetWidgetName(),
               this->FontFamilyOptionMenu->GetWidgetName(),
               this->StylesFrame->GetWidgetName(),
               this->CopyButton->GetWidgetName());

  // Update

  this->UpdateInterface();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateInterface()
{
  this->UpdateColorButton();
  this->UpdateFontFamilyOptionMenu();
  this->UpdateStylesFrame();
  this->UpdateCopyButton();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetTextProperty(vtkTextProperty *_arg)
{
  if (this->TextProperty == _arg)
    {
    return;
    }

  if (this->TextProperty != NULL) 
    { 
    // this->TextProperty->UnRegister(this); 
    }

  this->TextProperty = _arg;
  
  if (this->TextProperty != NULL) 
    { 
    //this->TextProperty->Register(this); 
    this->UpdateInterface();
    }

  this->Modified();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowColor(int _arg)
{
  if (this->ShowColor == _arg)
    {
    return;
    }
  this->ShowColor = _arg;
  this->Modified();

  this->UpdateColorButton();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetColor(float r, float g, float b) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetColor(r, g, b);
    }

  this->UpdateColorButton();

  this->AddTraceEntry("$kw(%s) SetColor %f %f %f", this->GetTclName(), r,g,b);

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateColorButton()
{
  if (this->ChangeColorButton->IsCreated() && this->TextProperty)
    {
    this->ChangeColorButton->SetColor(this->TextProperty->GetColor());
    this->Script("grid %s %s",
                 (this->ShowColor ? "" : "remove"), 
                 this->ChangeColorButton->GetWidgetName());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::ChangeColorButtonCallback(float r, float g, float b) 
{
  this->SetColor(r, g, b);
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowFontFamily(int _arg)
{
  if (this->ShowFontFamily == _arg)
    {
    return;
    }
  this->ShowFontFamily = _arg;
  this->Modified();

  this->UpdateFontFamilyOptionMenu();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetFontFamily(int v) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetFontFamily(v);
    }

  this->UpdateFontFamilyOptionMenu();

  this->AddTraceEntry("$kw(%s) SetFontFamily %d", this->GetTclName(), v);

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateFontFamilyOptionMenu()
{
  if (this->FontFamilyOptionMenu->IsCreated() && this->TextProperty)
    {
    switch (this->TextProperty->GetFontFamily())
      {
      case VTK_ARIAL:
        this->FontFamilyOptionMenu->SetValue(VTK_KW_TEXT_PROP_ARIAL);
        break;
      case VTK_COURIER:
        this->FontFamilyOptionMenu->SetValue(VTK_KW_TEXT_PROP_COURIER);
        break;
      case VTK_TIMES:
        this->FontFamilyOptionMenu->SetValue(VTK_KW_TEXT_PROP_TIMES);
        break;
      }
    this->Script("grid %s %s",
                 (this->ShowFontFamily ? "" : "remove"), 
                 this->FontFamilyOptionMenu->GetWidgetName());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::FontFamilyOptionMenuCallback() 
{
  if (this->FontFamilyOptionMenu->IsCreated())
    {
    char *value = this->FontFamilyOptionMenu->GetValue();
    if (!strcmp(value, VTK_KW_TEXT_PROP_ARIAL))
      {
      this->SetFontFamily(VTK_ARIAL);
      }
    else if (!strcmp(value, VTK_KW_TEXT_PROP_COURIER))
      {
      this->SetFontFamily(VTK_COURIER);
      }
    else if (!strcmp(value, VTK_KW_TEXT_PROP_TIMES))
      {
      this->SetFontFamily(VTK_TIMES);
      }
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowStyles(int _arg)
{
  if (this->ShowStyles == _arg)
    {
    return;
    }
  this->ShowStyles = _arg;
  this->Modified();

  this->UpdateStylesFrame();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateStylesFrame()
{
  if (this->StylesFrame->IsCreated())
    {
    this->Script("grid %s %s",
                 (this->ShowStyles ? "" : "remove"), 
                 this->StylesFrame->GetWidgetName());
    }

  this->UpdateBoldCheckButton();
  this->UpdateItalicCheckButton();
  this->UpdateShadowCheckButton();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetBold(int v) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetBold(v);
    }

  this->UpdateBoldCheckButton();

  this->AddTraceEntry("$kw(%s) SetBold %d", this->GetTclName(), v);

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateBoldCheckButton()
{
  if (this->BoldCheckButton->IsCreated() && this->TextProperty)
    {
    this->BoldCheckButton->SetState(this->TextProperty->GetBold());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::BoldCheckButtonCallback() 
{
  if (this->BoldCheckButton->IsCreated())
    {
    this->SetBold(this->BoldCheckButton->GetState());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetItalic(int v) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetItalic(v);
    }

  this->UpdateItalicCheckButton();

  this->AddTraceEntry("$kw(%s) SetItalic %d", this->GetTclName(), v);

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateItalicCheckButton()
{
  if (this->ItalicCheckButton->IsCreated() && this->TextProperty)
    {
    this->ItalicCheckButton->SetState(this->TextProperty->GetItalic());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::ItalicCheckButtonCallback() 
{
  if (this->ItalicCheckButton->IsCreated())
    {
    this->SetItalic(this->ItalicCheckButton->GetState());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShadow(int v) 
{
  if (this->TextProperty)
    {
    this->TextProperty->SetShadow(v);
    }

  this->UpdateShadowCheckButton();

  this->AddTraceEntry("$kw(%s) SetShadow %d", this->GetTclName(), v);

  if (this->OnChangeCommand)
    {
    this->Script("eval %s", this->OnChangeCommand);
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateShadowCheckButton()
{
  if (this->ShadowCheckButton->IsCreated() && this->TextProperty)
    {
    this->ShadowCheckButton->SetState(this->TextProperty->GetShadow());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::ShadowCheckButtonCallback() 
{
  if (this->ShadowCheckButton->IsCreated())
    {
    this->SetShadow(this->ShadowCheckButton->GetState());
    }
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowHorizontalJustification(int _arg)
{
  if (this->ShowHorizontalJustification == _arg)
    {
    return;
    }
  this->ShowHorizontalJustification = _arg;
  this->Modified();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowVerticalJustification(int _arg)
{
  if (this->ShowVerticalJustification == _arg)
    {
    return;
    }
  this->ShowVerticalJustification = _arg;
  this->Modified();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::SetShowCopy(int _arg)
{
  if (this->ShowCopy == _arg)
    {
    return;
    }
  this->ShowCopy = _arg;
  this->Modified();

  this->UpdateCopyButton();
}

// ----------------------------------------------------------------------------
void vtkKWTextProperty::UpdateCopyButton()
{
  if (this->CopyButton->IsCreated())
    {
    this->Script("grid %s %s",
                 (this->ShowCopy ? "" : "remove"), 
                 this->CopyButton->GetWidgetName());
    }
}

void vtkKWTextProperty::CopyValuesFrom(vtkKWTextProperty *widget)
{
  if (widget)
    {
    vtkTextProperty *tprop = widget->GetTextProperty();
    if (tprop)
      {
      float *rgb = tprop->GetColor();
      this->SetColor(rgb[0], rgb[1], rgb[2]);
      this->SetFontFamily(tprop->GetFontFamily());
      this->SetBold(tprop->GetBold());
      this->SetItalic(tprop->GetItalic());
      this->SetShadow(tprop->GetShadow());
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWTextProperty::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if (this->TextProperty)
    {
    os << indent << "TextProperty:\n";
    this->TextProperty->PrintSelf(os, indent.GetNextIndent());
    }
  else
    {
    os << indent << "TextProperty: None" << endl;
    }
  os << indent << "ShowColor: " << (this->ShowColor ? "On" : "Off") << endl;
  os << indent << "ShowFontFamily: " 
     << (this->ShowFontFamily ? "On" : "Off") << endl;
  os << indent << "ShowStyles: " 
     << (this->ShowStyles ? "On" : "Off") << endl;
  os << indent << "ShowHorizontalJustification: " 
     << (this->ShowHorizontalJustification ? "On" : "Off") << endl;
  os << indent << "ShowVerticalJustification: " 
     << (this->ShowVerticalJustification ? "On" : "Off") << endl;
  os << indent << "ShowCopy: " << (this->ShowCopy ? "On" : "Off") << endl;
  os << indent << "OnChangeCommand: " 
     << (this->OnChangeCommand ? this->OnChangeCommand : "None") << endl;
  os << indent << "CopyButton: " << this->CopyButton << endl;
}
