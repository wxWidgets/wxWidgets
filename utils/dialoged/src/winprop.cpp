/////////////////////////////////////////////////////////////////////////////
// Name:        winprop.cpp
// Purpose:     Window properties
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "winprop.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(__WXMSW__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#ifdef __WXMSW__
#include <windows.h>
#endif

#include "reseditr.h"
#include "winprop.h"

// Causes immediate feedback.
void wxResourcePropertyListView::OnPropertyChanged(wxProperty *property)
{
  // Sets the value of the property back into the actual object,
  // IF the property value was modified.
  if (property->GetValue().GetModified())
  {
    propertyInfo->SetProperty(property->GetName(), property);
    property->GetValue().SetModified(FALSE);
  }
}

bool wxResourcePropertyListView::OnClose(void)
{
  int w, h, x, y;
  GetManagedWindow()->GetSize(& w, & h);
  GetManagedWindow()->GetPosition(& x, & y);

  wxResourceManager::GetCurrentResourceManager()->GetPropertyWindowSize().width = w;
  wxResourceManager::GetCurrentResourceManager()->GetPropertyWindowSize().height = h;
  wxResourceManager::GetCurrentResourceManager()->GetPropertyWindowSize().x = x;
  wxResourceManager::GetCurrentResourceManager()->GetPropertyWindowSize().y = y;

  return wxPropertyListView::OnClose();
}

wxWindow *wxPropertyInfo::sm_propertyWindow;

/*
 * wxDialogEditorPropertyListDialog
 */

wxDialogEditorPropertyListDialog::wxDialogEditorPropertyListDialog(wxPropertyListView *v, wxWindow *parent, const wxString& title,
        const wxPoint& pos, const wxSize& size,
        long style, const wxString& name):
            wxPropertyListDialog(v, parent, title, pos, size, style, name)
{
        m_propSheet = NULL;
        m_propInfo = NULL;
}

wxDialogEditorPropertyListDialog::~wxDialogEditorPropertyListDialog()
{
    delete m_propSheet;
    delete m_propInfo;
    wxPropertyInfo::sm_propertyWindow = NULL;
}

/*
 * wxPropertyInfo
 */

// Edit the information represented by this object, whatever that
// might be.
bool wxPropertyInfo::Edit(wxWindow *parent, const wxString& title)
{
    if (sm_propertyWindow)
        return FALSE;

  int width = wxResourceManager::GetCurrentResourceManager()->GetPropertyWindowSize().width;
  int height = wxResourceManager::GetCurrentResourceManager()->GetPropertyWindowSize().height;
  int x = wxResourceManager::GetCurrentResourceManager()->GetPropertyWindowSize().x;
  int y = wxResourceManager::GetCurrentResourceManager()->GetPropertyWindowSize().y;

  wxPropertySheet *propSheet = new wxPropertySheet;

  wxStringList propNames;
  GetPropertyNames(propNames);

  wxNode *node = propNames.First();
  while (node)
  {
    wxString name((char *)node->Data());
    wxProperty *prop = GetProperty(name);
    if (prop)
    {
      propSheet->AddProperty(prop);
    }
    node = node->Next();
  }
  
  // Reset 'modified' flags for all property values
  propSheet->SetAllModified(FALSE);

  wxResourcePropertyListView *view = new wxResourcePropertyListView(this, NULL,
#ifdef __XVIEW__
     wxPROP_BUTTON_OK | wxPROP_BUTTON_CANCEL |
#endif
     wxPROP_BUTTON_CHECK_CROSS|wxPROP_DYNAMIC_VALUE_FIELD|wxPROP_PULLDOWN|wxPROP_SHOWVALUES);

  wxDialogEditorPropertyListDialog *propDialog = new wxDialogEditorPropertyListDialog(view,
    wxResourceManager::GetCurrentResourceManager()->GetEditorFrame(), title, wxPoint(x, y),
    wxSize(width, height), wxDEFAULT_DIALOG_STYLE);
  sm_propertyWindow = propDialog;

  propDialog->m_registry.RegisterValidator((wxString)"real", new wxRealListValidator);
  propDialog->m_registry.RegisterValidator((wxString)"string", new wxStringListValidator);
  propDialog->m_registry.RegisterValidator((wxString)"integer", new wxIntegerListValidator);
  propDialog->m_registry.RegisterValidator((wxString)"bool", new wxBoolListValidator);
  propDialog->m_registry.RegisterValidator((wxString)"filename", new wxFilenameListValidator);
  propDialog->m_registry.RegisterValidator((wxString)"stringlist", new wxListOfStringsListValidator);
  propDialog->m_registry.RegisterValidator((wxString)"window_id", new wxResourceSymbolValidator);

  propDialog->m_propInfo = this;
  propDialog->m_propSheet = propSheet;

//  view->propertyWindow = propDialog;
  view->AddRegistry(&(propDialog->m_registry));
  view->ShowView(propSheet, propDialog);

  propDialog->Show(TRUE);
  return TRUE;
}

/*
 * wxWindowPropertyInfo
 */

wxWindowPropertyInfo::wxWindowPropertyInfo(wxWindow *win, wxItemResource *res)
{
  propertyWindow = win;
  propertyResource = res;
}

wxWindowPropertyInfo::~wxWindowPropertyInfo(void)
{
}

wxProperty *wxWindowPropertyInfo::GetFontProperty(wxString& name, wxFont *font)
{
  if (!font)
    return NULL;
    
  if (name.Contains("Points"))
    return new wxProperty(name, (long)font->GetPointSize(), "integer", new wxIntegerListValidator(1, 100));
  else if (name.Contains("Family"))
    return new wxProperty(name, font->GetFamilyString(), "string",
       new wxStringListValidator(new wxStringList("wxDECORATIVE", "wxROMAN", "wxSCRIPT", "wxSWISS", "wxMODERN",
          NULL)));
  else if (name.Contains("Style"))
    return new wxProperty(name, font->GetStyleString(), "string",
       new wxStringListValidator(new wxStringList("wxNORMAL", "wxITALIC", "wxSLANT", NULL)));
  else if (name.Contains("Weight"))
    return new wxProperty(name, font->GetWeightString(), "string",
       new wxStringListValidator(new wxStringList("wxNORMAL", "wxBOLD", "wxLIGHT", NULL)));
  else if (name.Contains("Underlined"))
    return new wxProperty(name, (bool)font->GetUnderlined(), "bool");
  else
    return NULL;
}

wxFont *wxWindowPropertyInfo::SetFontProperty(wxString& name, wxProperty *property, wxFont *font)
{
  int pointSize = 12;
  int fontFamily = wxMODERN;
  int fontStyle = wxNORMAL;
  int fontWeight = wxNORMAL;
  bool fontUnderlined = FALSE;

  if (name.Contains("Points"))
  {
    pointSize = (int)property->GetValue().IntegerValue();
    if (font && (pointSize == font->GetPointSize()))
      return NULL; // No change
  }
  else if (font) pointSize = font->GetPointSize();

  if (name.Contains("Family"))
  {
    wxString val = property->GetValue().StringValue();
    fontFamily = wxStringToFontFamily(val);
    
    if (font && (fontFamily == font->GetFamily()))
      return NULL; // No change
  }
  else if (font) fontFamily = font->GetFamily();

  if (name.Contains("Style"))
  {
    wxString val = property->GetValue().StringValue();
    fontStyle = wxStringToFontStyle(val);
    
    if (font && (fontStyle == font->GetStyle()))
      return NULL; // No change
  }
  else if (font) fontStyle = font->GetStyle();
  if (name.Contains("Weight"))
  {
    wxString val = property->GetValue().StringValue();
    fontWeight = wxStringToFontWeight(val);
      
    if (font && (fontWeight == font->GetWeight()))
      return NULL; // No change
  }
  else if (font) fontWeight = font->GetWeight();
      
  if (name.Contains("Underlined"))
  {
    fontUnderlined = property->GetValue().BoolValue();

    if (font && (fontUnderlined == font->GetUnderlined()))
      return NULL; // No change
  }
  else if (font) fontUnderlined = font->GetUnderlined();

  wxFont *newFont = wxTheFontList->FindOrCreateFont(pointSize, fontFamily, fontStyle, fontWeight, fontUnderlined);
  if (newFont)
  {
    return newFont;
  }
  else
    return NULL;
}

wxProperty *wxWindowPropertyInfo::GetProperty(wxString& name)
{
  wxFont *font = propertyWindow->GetFont();
  if (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" ||
      name == "fontUnderlined")
    return GetFontProperty(name, font);
  else if (name == "name")
    return new wxProperty("name", propertyWindow->GetName(), "string");
  else if (name == "title")
    return new wxProperty("title", propertyWindow->GetTitle(), "string");
  else if (name == "x")
  {
    int x, y;
    propertyWindow->GetPosition(&x, &y);
    return new wxProperty("x", (long)x, "integer");
  }
  else if (name == "y")
  {
    int x, y;
    propertyWindow->GetPosition(&x, &y);
    return new wxProperty("y", (long)y, "integer");
  }
  else if (name == "width")
  {
    int width, height;
    propertyWindow->GetSize(&width, &height);
    return new wxProperty("width", (long)width, "integer");
  }
  else if (name == "height")
  {
    int width, height;
    propertyWindow->GetSize(&width, &height);
    return new wxProperty("height", (long)height, "integer");
  }
  else if (name == "id")
  {
    wxString symbolName("TODO");
    return new wxProperty("id", symbolName, "window_id");
  }
  else
    return NULL;
}

bool wxWindowPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxFont *font = propertyWindow->GetFont();
  if (font && (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" || name == "fontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, font);
    if (newFont)
      propertyWindow->SetFont(newFont);
    return TRUE;
  }
  else if (name == "name")
  {
    // Remove old name from resource table, if it's there.
    wxItemResource *oldResource = (wxItemResource *)wxResourceManager::GetCurrentResourceManager()->GetResourceTable().Delete(propertyWindow->GetName());
    if (oldResource)
    {
      // It's a top-level resource
      propertyWindow->SetName(property->GetValue().StringValue());
      oldResource->SetName(property->GetValue().StringValue());
      wxResourceManager::GetCurrentResourceManager()->GetResourceTable().Put(propertyWindow->GetName(), oldResource);
    }
    else
    {
      // It's a child of something; just set the name of the resource and the window.
      propertyWindow->SetName(property->GetValue().StringValue());
      propertyResource->SetName(property->GetValue().StringValue());
    }
    // Refresh the resource manager list, because the name changed.
    wxResourceManager::GetCurrentResourceManager()->UpdateResourceList();
    return TRUE;
  }
  else if (name == "title")
  {
    propertyWindow->SetTitle(property->GetValue().StringValue());
    return TRUE;
  }
  else if (name == "x")
  {
    int x, y;
    propertyWindow->GetPosition(&x, &y);
    int newX = (int)property->GetValue().IntegerValue();
    if (x != newX)
      propertyWindow->Move(newX, y);
    return TRUE;
  }
  else if (name == "y")
  {
    int x, y;
    propertyWindow->GetPosition(&x, &y);
    int newY = (int)property->GetValue().IntegerValue();
    if (y != newY)
      propertyWindow->Move(x, newY);
    return TRUE;
  }
  else if (name == "width")
  {
    int width, height;
    propertyWindow->GetSize(&width, &height);
    int newWidth = (int)property->GetValue().IntegerValue();
    if (width != newWidth)
    {
      propertyWindow->SetSize(newWidth, height);
/*
      if (propertyWindow->IsKindOf(CLASSINFO(wxPanel)) && !propertyWindow->IsKindOf(CLASSINFO(wxDialog)))
      {
        propertyWindow->GetParent()->SetClientSize(newWidth, height);
      }
*/
    }
    return TRUE;
  }
  else if (name == "height")
  {
    int width, height;
    propertyWindow->GetSize(&width, &height);
    int newHeight = (int)property->GetValue().IntegerValue();
    if (height != newHeight)
    {
      propertyWindow->SetSize(width, newHeight);
/*
      if (propertyWindow->IsKindOf(CLASSINFO(wxPanel)) && !propertyWindow->IsKindOf(CLASSINFO(wxDialog)))
      {
        propertyWindow->GetParent()->SetClientSize(width, newHeight);
      }
*/
    }
    return TRUE;
  }
  else if (name == "id")
  {
    // TODO
    return TRUE;
  }
  else
    return FALSE;
}

void wxWindowPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("id");
  names.Add("name");
  names.Add("x");
  names.Add("y");
  names.Add("width");
  names.Add("height");
  if (!propertyWindow->IsKindOf(CLASSINFO(wxControl)))
  {
    names.Add("fontPoints");
    names.Add("fontFamily");
    names.Add("fontStyle");
    names.Add("fontWeight");
    names.Add("fontUnderlined");
  }
}

// Fill in the wxItemResource members to mirror the current window settings
bool wxWindowPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  resource->SetType(propertyWindow->GetClassInfo()->GetClassName());
  
//  resource->SetStyle(propertyWindow->GetWindowStyleFlag());
  wxString str(propertyWindow->GetName());
  resource->SetName(WXSTRINGCAST str);
  int x, y, w, h;
  propertyWindow->GetSize(&w, &h);

  propertyWindow->GetPosition(&x, &y);
  resource->SetSize(x, y, w, h);
  return TRUE;
}


/*
 * Controls
 */

wxProperty *wxItemPropertyInfo::GetProperty(wxString& name)
{
  wxControl *itemWindow = (wxControl *)propertyWindow; 
  wxFont *font = itemWindow->GetFont();

  if (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" ||
      name == "fontUnderlined")
    return GetFontProperty(name, font);
  else if (name == "label" && itemWindow->GetLabel())
    return new wxProperty("label", propertyWindow->GetLabel(), "string");
  else
    return wxWindowPropertyInfo::GetProperty(name);
}

bool wxItemPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxControl *itemWindow = (wxControl *)propertyWindow; 
  wxFont *font = itemWindow->GetFont();

  if (font && (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" || name == "fontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, font);
    if (newFont)
      itemWindow->SetLabelFont(newFont);
    return TRUE;
  }
  else if (name == "label")
  {
    itemWindow->SetLabel(property->GetValue().StringValue());
    return TRUE;
  }
  else
    return wxWindowPropertyInfo::SetProperty(name, property);
}

void wxItemPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxWindowPropertyInfo::GetPropertyNames(names);
  
  names.Add("fontPoints");
  names.Add("fontFamily");
  names.Add("fontStyle");
  names.Add("fontWeight");
  names.Add("fontUnderlined");
}

bool wxItemPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxWindowPropertyInfo::InstantiateResource(resource);
  
  wxControl *item = (wxControl *)propertyWindow;
  wxString str(item->GetLabel());
  resource->SetTitle(WXSTRINGCAST str);
  if (item->GetFont() && item->GetFont()->Ok())
    resource->SetFont(wxTheFontList->FindOrCreateFont(item->GetFont()->GetPointSize(),
		item->GetFont()->GetFamily(), item->GetFont()->GetStyle(), item->GetFont()->GetWeight(),
		item->GetFont()->GetUnderlined(), item->GetFont()->GetFaceName()));
  return TRUE;
}

/*
 * Button
 */

wxProperty *wxButtonPropertyInfo::GetProperty(wxString& name)
{
  wxButton *button = (wxButton *)propertyWindow;
  return wxItemPropertyInfo::GetProperty(name);
}

bool wxButtonPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxButton *button = (wxButton *)propertyWindow;
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxButtonPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxButtonPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * wxBitmapButton
 */

wxProperty *wxBitmapButtonPropertyInfo::GetProperty(wxString& name)
{
  wxBitmapButton *button = (wxBitmapButton *)propertyWindow;
  if (name == "label")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(button);
    wxString str("none.bmp");
   
    if (resource)
    {
      char *filename = wxResourceManager::GetCurrentResourceManager()->FindBitmapFilenameForResource(resource);
      if (filename)
        str = filename;
    }
    return new wxProperty("label", str.GetData(), "string", new wxFilenameListValidator("Select a bitmap file", "*.bmp"));
  }
  else
    return wxButtonPropertyInfo::GetProperty(name);
}

bool wxBitmapButtonPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxBitmapButton *button = (wxBitmapButton *)propertyWindow;
  if (name == "label")
  {
    char *s = property->GetValue().StringValue();
    if (s && wxFileExists(s))
    {
      s = copystring(s);
      wxBitmap *bitmap = new wxBitmap(s, wxBITMAP_TYPE_BMP);
      if (!bitmap->Ok())
      {
        delete bitmap;
        delete[] s;
        return FALSE;
      }
      else
      {
        wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(button);
        if (resource)
        {
          wxString oldResource(resource->GetValue4());
          char *resName = wxResourceManager::GetCurrentResourceManager()->AddBitmapResource(s);
          resource->SetValue4(resName);
          
          if (!oldResource.IsNull())
            wxResourceManager::GetCurrentResourceManager()->PossiblyDeleteBitmapResource(WXSTRINGCAST oldResource);
        }

        button->SetLabel(bitmap);
        delete[] s;
        return TRUE;
      }
    }
    return FALSE;
  }
  else
    return wxButtonPropertyInfo::SetProperty(name, property);
}

void wxBitmapButtonPropertyInfo::GetPropertyNames(wxStringList& names)
{
//  names.Add("label");
  wxButtonPropertyInfo::GetPropertyNames(names);
}

bool wxBitmapButtonPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * wxStaticText
 */

wxProperty *wxStaticTextPropertyInfo::GetProperty(wxString& name)
{
  wxStaticText *message = (wxStaticText *)propertyWindow;
  return wxItemPropertyInfo::GetProperty(name);
}

bool wxStaticTextPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxStaticText *message = (wxStaticText *)propertyWindow;
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxStaticTextPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxStaticTextPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * wxStaticBitmap
 */

wxProperty *wxStaticBitmapPropertyInfo::GetProperty(wxString& name)
{
  wxStaticBitmap *message = (wxStaticBitmap *)propertyWindow;
  if (name == "label")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(message);
    wxString str("none.bmp");
   
    if (resource)
    {
      char *filename = wxResourceManager::GetCurrentResourceManager()->FindBitmapFilenameForResource(resource);
      if (filename)
        str = filename;
    }
    return new wxProperty("label", str.GetData(), "string", new wxFilenameListValidator("Select a bitmap file", "*.bmp"));
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxStaticBitmapPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxStaticBitmap *message = (wxStaticBitmap *)propertyWindow;
  if (name == "label")
  {
    char *s = property->GetValue().StringValue();
    if (s && wxFileExists(s))
    {
      s = copystring(s);
      
      wxBitmap *bitmap = new wxBitmap(s, wxBITMAP_TYPE_BMP);
      if (!bitmap->Ok())
      {
        delete bitmap;
        delete[] s;
        return FALSE;
      }
      else
      {
        wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(message);
        if (resource)
        {
          wxString oldResource(resource->GetValue4());
          char *resName = wxResourceManager::GetCurrentResourceManager()->AddBitmapResource(s);
          resource->SetValue4(resName);
          
          if (!oldResource.IsNull())
            wxResourceManager::GetCurrentResourceManager()->PossiblyDeleteBitmapResource(WXSTRINGCAST oldResource);
        }

        message->SetBitmap(bitmap);
        delete[] s;
        return TRUE;
      }
    }
    return FALSE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxStaticBitmapPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxStaticBitmapPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Text item
 */

wxProperty *wxTextPropertyInfo::GetProperty(wxString& name)
{
  wxTextCtrl *text = (wxTextCtrl *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", text->GetValue(), "string");
  else if (name == "password")
  {
    bool isPassword = ((text->GetWindowStyleFlag() & wxTE_PASSWORD) == wxTE_PASSWORD);
    return new wxProperty("password", isPassword, "bool");
  }
  else if (name == "readonly")
  {
    bool isReadOnly = ((text->GetWindowStyleFlag() & wxTE_READONLY) == wxTE_READONLY);
    return new wxProperty("readonly", isReadOnly, "bool");
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxTextPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxTextCtrl *text = (wxTextCtrl *)propertyWindow;
  if (name == "value")
  {
    text->SetValue(property->GetValue().StringValue());
    return TRUE;
  }
  else if (name == "password")
  {
    long flag = text->GetWindowStyleFlag();
    if (property->GetValue().BoolValue())
    {
      if ((flag & wxTE_PASSWORD) != wxTE_PASSWORD)
        flag |= wxTE_PASSWORD;
    }
    else
    {
      if ((flag & wxTE_PASSWORD) == wxTE_PASSWORD)
        flag -= wxTE_PASSWORD;
    }
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(text);
    resource->SetStyle(flag);

    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(text, this);
    return TRUE;
  }
  else if (name == "readonly")
  {
    long flag = text->GetWindowStyleFlag();
    if (property->GetValue().BoolValue())
    {
      if ((flag & wxTE_READONLY) != wxTE_READONLY)
        flag |= wxTE_READONLY;
    }
    else
    {
      if ((flag & wxTE_READONLY) == wxTE_READONLY)
        flag -= wxTE_READONLY;
    }
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(text);
    resource->SetStyle(flag);

    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(text, this);
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxTextPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("value");
  names.Add("readonly");
  names.Add("password");
  wxItemPropertyInfo::GetPropertyNames(names);  
}

bool wxTextPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxTextCtrl *text = (wxTextCtrl *)propertyWindow;
  wxString str(text->GetValue());
  resource->SetValue4(WXSTRINGCAST str);
    
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Listbox item
 */

wxProperty *wxListBoxPropertyInfo::GetProperty(wxString& name)
{
  wxListBox *listBox = (wxListBox *)propertyWindow;
  if (name == "values")
  {
    wxStringList *stringList = new wxStringList;
    int i;
    for (i = 0; i < listBox->Number(); i++)
      stringList->Add(listBox->GetString(i));

    return new wxProperty(name, stringList, "stringlist");
  }
  else if (name == "multiple")
  {
    char *pos = NULL;
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(listBox);
    if (!resource)
      return NULL;
      
    char *mult = "wxSINGLE";
      
    switch (resource->GetValue1())
    {
      case wxLB_MULTIPLE:
        mult = "wxLB_MULTIPLE";
        break;
      case wxLB_EXTENDED:
        mult = "wxLB_EXTENDED";
        break;
      default:
      case wxLB_SINGLE:
        mult = "wxLB_SINGLE";
        break;
    }
    return new wxProperty("multiple", mult, "string",
       new wxStringListValidator(new wxStringList("wxLB_SINGLE", "wxLB_MULTIPLE", "wxLB_EXTENDED",
          NULL)));
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxListBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxListBox *listBox = (wxListBox *)propertyWindow;
  if (name == "values")
  {
    listBox->Clear();
    wxPropertyValue *expr = property->GetValue().GetFirst();
    while (expr)
    {
      char *s = expr->StringValue();
      if (s)
        listBox->Append(s);
      expr = expr->GetNext();
    }
    return TRUE;
  }
  else if (name == "multiple")
  {
    int mult = wxLB_SINGLE;
    wxString str(property->GetValue().StringValue());
    if (str == "wxLB_MULTIPLE")
      mult = wxLB_MULTIPLE;
    else if (str == "wxLB_EXTENDED")
      mult = wxLB_EXTENDED;
    else
      mult = wxLB_SINGLE;
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(listBox);
    if (resource)
      resource->SetValue1(mult);
    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(listBox, this);
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxListBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("values");
  names.Add("multiple");
  wxItemPropertyInfo::GetPropertyNames(names);  
}

bool wxListBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxListBox *lbox = (wxListBox *)propertyWindow;
  // This will be set for the wxItemResource on reading or in SetProperty
//  resource->SetValue1(lbox->GetSelectionMode());
  int i;
  if (lbox->Number() == 0)
    resource->SetStringValues(NULL);
  else
  {
    wxStringList *slist = new wxStringList;
    
    for (i = 0; i < lbox->Number(); i++)
      slist->Add(lbox->GetString(i));
      
    resource->SetStringValues(slist);
  }
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Choice item
 */

wxProperty *wxChoicePropertyInfo::GetProperty(wxString& name)
{
  wxChoice *choice = (wxChoice *)propertyWindow;
  if (name == "values")
  {
    wxStringList *stringList = new wxStringList;
    int i;
    for (i = 0; i < choice->Number(); i++)
      stringList->Add(choice->GetString(i));

    return new wxProperty(name, stringList, "stringlist");
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxChoicePropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxChoice *choice = (wxChoice *)propertyWindow;
  if (name == "values")
  {
    choice->Clear();
    wxPropertyValue *expr = property->GetValue().GetFirst();
    while (expr)
    {
      char *s = expr->StringValue();
      if (s)
        choice->Append(s);
      expr = expr->GetNext();
    }
    if (choice->Number() > 0)
      choice->SetSelection(0);
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxChoicePropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("values");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxChoicePropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxChoice *choice = (wxChoice *)propertyWindow;
  int i;
  if (choice->Number() == 0)
    resource->SetStringValues(NULL);
  else
  {
    wxStringList *slist = new wxStringList;
    
    for (i = 0; i < choice->Number(); i++)
      slist->Add(choice->GetString(i));
      
    resource->SetStringValues(slist);
  }
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Radiobox item
 */

wxProperty *wxRadioBoxPropertyInfo::GetProperty(wxString& name)
{
  wxRadioBox *radioBox = (wxRadioBox *)propertyWindow;
  if (name == "numberRowsOrCols")
  {
    return new wxProperty("numberRowsOrCols", (long)radioBox->GetNumberOfRowsOrCols(), "integer");
  }
  if (name == "orientation")
  {
    char *pos = NULL;
    if (propertyWindow->GetWindowStyleFlag() & wxHORIZONTAL)
      pos = "wxHORIZONTAL";
    else
      pos = "wxVERTICAL";

    return new wxProperty("orientation", pos, "string",
       new wxStringListValidator(new wxStringList("wxHORIZONTAL", "wxVERTICAL",
          NULL)));
  }
  else if (name == "values")
  {
    wxStringList *stringList = new wxStringList;
    int i;
    for (i = 0; i < radioBox->Number(); i++)
      stringList->Add(radioBox->GetString(i));

    return new wxProperty(name, stringList, "stringlist");
  }
  return wxItemPropertyInfo::GetProperty(name);
}

bool wxRadioBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxRadioBox *radioBox = (wxRadioBox *)propertyWindow;
  if (name == "numberRowsOrCols")
  {
    radioBox->SetNumberOfRowsOrCols((int)property->GetValue().IntegerValue());
    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(radioBox, this);
    return TRUE;
  }
  else if (name == "orientation")
  {
    long windowStyle = radioBox->GetWindowStyleFlag();
    wxString val(property->GetValue().StringValue());
    if (val == "wxHORIZONTAL")
    {
      if (windowStyle & wxVERTICAL)
        windowStyle -= wxVERTICAL;
      windowStyle |= wxHORIZONTAL;
    }
    else
    {
      if (windowStyle & wxHORIZONTAL)
        windowStyle -= wxHORIZONTAL;
      windowStyle |= wxVERTICAL;
    }
    radioBox->SetWindowStyleFlag(windowStyle);
    
    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(radioBox, this);
    return TRUE;
  }
  else if (name == "values")
  {
    // Set property into *resource*, not wxRadioBox, and then recreate
    // the wxRadioBox. This is because we can't dynamically set the strings
    // of a wxRadioBox.
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(propertyWindow);
    if (!resource)
      return FALSE;
      
    wxStringList *stringList = resource->GetStringValues();
    if (!stringList)
    {
      stringList = new wxStringList;
      resource->SetStringValues(stringList);
    }
    stringList->Clear();
      
    wxPropertyValue *expr = property->GetValue().GetFirst();
    while (expr)
    {
      char *s = expr->StringValue();
      if (s)
        stringList->Add(s);
      expr = expr->GetNext();
    }
    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(radioBox, this);
    return TRUE;
  }
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxRadioBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("values");
  names.Add("orientation");
  names.Add("numberRowsOrCols");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxRadioBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxRadioBox *rbox = (wxRadioBox *)propertyWindow;
  // Take strings from resource instead
/*
  int i;
  if (rbox->Number() == 0)
    resource->SetStringValues(NULL);
  else
  {
    wxStringList *slist = new wxStringList;
    
    for (i = 0; i < rbox->Number(); i++)
      slist->Add(rbox->GetString(i));
      
    resource->SetStringValues(slist);
  }
*/
  resource->SetValue1(rbox->GetNumberOfRowsOrCols());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Groupbox item
 */

wxProperty *wxGroupBoxPropertyInfo::GetProperty(wxString& name)
{
  wxStaticBox *groupBox = (wxStaticBox *)propertyWindow;
  return wxItemPropertyInfo::GetProperty(name);
}

bool wxGroupBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxGroupBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{  
  names.Add("label");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxGroupBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxStaticBox *gbox = (wxStaticBox *)propertyWindow;
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Checkbox item
 */

wxProperty *wxCheckBoxPropertyInfo::GetProperty(wxString& name)
{
  wxCheckBox *checkBox = (wxCheckBox *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", checkBox->GetValue(), "bool");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxCheckBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxCheckBox *checkBox = (wxCheckBox *)propertyWindow;
  if (name == "value")
  {
    checkBox->SetValue((bool)property->GetValue().BoolValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxCheckBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("value");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxCheckBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxCheckBox *cbox = (wxCheckBox *)propertyWindow;
  resource->SetValue1(cbox->GetValue());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Radiobutton item
 */

wxProperty *wxRadioButtonPropertyInfo::GetProperty(wxString& name)
{
  wxRadioButton *checkBox = (wxRadioButton *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", checkBox->GetValue(), "bool");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxRadioButtonPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxRadioButton *checkBox = (wxRadioButton *)propertyWindow;
  if (name == "value")
  {
    checkBox->SetValue((bool)property->GetValue().BoolValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxRadioButtonPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("value");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxRadioButtonPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxRadioButton *cbox = (wxRadioButton *)propertyWindow;
  resource->SetValue1(cbox->GetValue());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Slider item
 */

wxProperty *wxSliderPropertyInfo::GetProperty(wxString& name)
{
  wxSlider *slider = (wxSlider *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", (long)slider->GetValue(), "integer");
  else if (name == "orientation")
  {
    char *pos = NULL;
    if (propertyWindow->GetWindowStyleFlag() & wxHORIZONTAL)
      pos = "wxHORIZONTAL";
    else
      pos = "wxVERTICAL";

    return new wxProperty("orientation", pos, "string",
       new wxStringListValidator(new wxStringList("wxHORIZONTAL", "wxVERTICAL",
          NULL)));
  }
  else if (name == "min_value")
    return new wxProperty("min_value", (long)slider->GetMin(), "integer");
  else if (name == "max_value")
    return new wxProperty("max_value", (long)slider->GetMax(), "integer");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxSliderPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxSlider *slider = (wxSlider *)propertyWindow;
  if (name == "value")
  {
    slider->SetValue((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "orientation")
  {
    long windowStyle = slider->GetWindowStyleFlag();
    long oldWindowStyle = windowStyle;
    wxString val(property->GetValue().StringValue());
    if (val == "wxHORIZONTAL")
    {
      if (windowStyle & wxVERTICAL)
        windowStyle -= wxVERTICAL;
      windowStyle |= wxHORIZONTAL;
    }
    else
    {
      if (windowStyle & wxHORIZONTAL)
        windowStyle -= wxHORIZONTAL;
      windowStyle |= wxVERTICAL;
    }
    
    if (oldWindowStyle == windowStyle)
      return TRUE;
      
    slider->SetWindowStyleFlag(windowStyle);
    
    // If the window style has changed, we swap the width and height parameters.
    int w, h;
    slider->GetSize(&w, &h);
    
    slider = (wxSlider *)wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(slider, this);
    slider->SetSize(-1, -1, h, w);
    
    return TRUE;
  }
  else if (name == "min_value")
  {
    slider->SetRange((int)property->GetValue().IntegerValue(), slider->GetMax());
    return TRUE;
  }
  else if (name == "max_value")
  {
    slider->SetRange(slider->GetMin(), (int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxSliderPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("value");
  names.Add("orientation");
  names.Add("min_value");
  names.Add("max_value");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxSliderPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxSlider *slider = (wxSlider *)propertyWindow;
  resource->SetValue1(slider->GetValue());
  resource->SetValue2(slider->GetMin());
  resource->SetValue3(slider->GetMax());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Gauge item
 */

wxProperty *wxGaugePropertyInfo::GetProperty(wxString& name)
{
  wxGauge *gauge = (wxGauge *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", (long)gauge->GetValue(), "integer");
  else if (name == "max_value")
    return new wxProperty("max_value", (long)gauge->GetRange(), "integer");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxGaugePropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxGauge *gauge = (wxGauge *)propertyWindow;
  if (name == "value")
  {
    gauge->SetValue((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "max_value")
  {
    gauge->SetRange((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxGaugePropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("value");
  names.Add("max_value");
  wxItemPropertyInfo::GetPropertyNames(names);
}

bool wxGaugePropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxGauge *gauge = (wxGauge *)propertyWindow;
  resource->SetValue1(gauge->GetValue());
  resource->SetValue2(gauge->GetRange());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Scrollbar item
 */

wxProperty *wxScrollBarPropertyInfo::GetProperty(wxString& name)
{
  wxScrollBar *scrollBar = (wxScrollBar *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", (long)scrollBar->GetValue(), "integer");
  else if (name == "orientation")
  {
    char *pos = NULL;
    if (propertyWindow->GetWindowStyleFlag() & wxHORIZONTAL)
      pos = "wxHORIZONTAL";
    else
      pos = "wxVERTICAL";

    return new wxProperty("orientation", pos, "string",
       new wxStringListValidator(new wxStringList("wxHORIZONTAL", "wxVERTICAL",
          NULL)));
  }
  else if (name == "pageSize")
  {
    int viewStart, pageLength, objectLength, viewLength;
    scrollBar->GetValues(&viewStart, &viewLength, &objectLength, &pageLength);
    
    return new wxProperty("pageSize", (long)pageLength, "integer");
  }
  else if (name == "viewLength")
  {
    int viewStart, pageLength, objectLength, viewLength;
    scrollBar->GetValues(&viewStart, &viewLength, &objectLength, &pageLength);
    
    return new wxProperty("viewLength", (long)viewLength, "integer");
  }
  else if (name == "objectLength")
  {
    int viewStart, pageLength, objectLength, viewLength;
    scrollBar->GetValues(&viewStart, &viewLength, &objectLength, &pageLength);
    
    return new wxProperty("objectLength", (long)objectLength, "integer");
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxScrollBarPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxScrollBar *scrollBar = (wxScrollBar *)propertyWindow;
  if (name == "value")
  {
    scrollBar->SetValue((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "orientation")
  {
    long windowStyle = scrollBar->GetWindowStyleFlag();
    long oldWindowStyle = windowStyle;
    wxString val(property->GetValue().StringValue());
    if (val == "wxHORIZONTAL")
    {
      if (windowStyle & wxVERTICAL)
        windowStyle -= wxVERTICAL;
      windowStyle |= wxHORIZONTAL;
    }
    else
    {
      if (windowStyle & wxHORIZONTAL)
        windowStyle -= wxHORIZONTAL;
      windowStyle |= wxVERTICAL;
    }
    
    if (oldWindowStyle == windowStyle)
      return TRUE;
      
    scrollBar->SetWindowStyleFlag(windowStyle);
    
    // If the window style has changed, we swap the width and height parameters.
    int w, h;
    scrollBar->GetSize(&w, &h);
    
    scrollBar = (wxScrollBar *)wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(scrollBar, this);
    scrollBar->SetSize(-1, -1, h, w);
    
    return TRUE;
  }
  else if (name == "pageSize")
  {
    scrollBar->SetPageSize((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "viewLength")
  {
    scrollBar->SetViewLength((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "objectLength")
  {
    scrollBar->SetObjectLength((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxScrollBarPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("orientation");
  names.Add("value");
  names.Add("pageSize");
  names.Add("viewLength");
  names.Add("objectLength");
  wxItemPropertyInfo::GetPropertyNames(names);

  // Remove some properties we don't inherit
  names.Delete("fontPoints");
  names.Delete("fontFamily");
  names.Delete("fontStyle");
  names.Delete("fontWeight");
  names.Delete("fontUnderlined");
}

bool wxScrollBarPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxScrollBar *sbar = (wxScrollBar *)propertyWindow;
  
  resource->SetValue1(sbar->GetValue());

  int viewStart, pageLength, objectLength, viewLength;
  sbar->GetValues(&viewStart, &viewLength, &objectLength, &pageLength);
  
  resource->SetValue2(pageLength);
  resource->SetValue3(objectLength);
  resource->SetValue5(viewLength);

  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Panels
 */

wxProperty *wxPanelPropertyInfo::GetProperty(wxString& name)
{
  wxPanel *panelWindow = (wxPanel *)propertyWindow; 
  wxFont *labelFont = panelWindow->GetLabelFont();
  wxFont *buttonFont = panelWindow->GetButtonFont();
  
  if (name == "labelFontPoints" || name == "labelFontFamily" || name == "labelFontStyle" || name == "labelFontWeight" ||
      name == "labelFontUnderlined")
    return GetFontProperty(name, labelFont);
  else if (name == "buttonFontPoints" || name == "buttonFontFamily" || name == "buttonFontStyle" || name == "buttonFontWeight" ||
      name == "buttonFontUnderlined")
    return GetFontProperty(name, buttonFont);
  else if (name == "no3D")
  {
    bool userColours;
    if (panelWindow->GetWindowStyleFlag() & wxNO_3D)
      userColours = TRUE;
    else
      userColours = FALSE;
      
    return new wxProperty(name, (bool)userColours, "bool");
  }
  else if (name == "backgroundColour")
  {
    wxColour col(panelWindow->GetBackgroundColour());
    char buf[7];
    wxDecToHex(col.Red(), buf);
    wxDecToHex(col.Green(), buf+2);
    wxDecToHex(col.Blue(), buf+4);

    return new wxProperty(name, buf, "string", new wxColourListValidator);
  }
  else if (name == "title")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    if (resource)
        return new wxProperty(name, resource->GetTitle(), "string");
    else
        return new wxProperty(name, "Could not get title", "string");
  }
  else
    return wxWindowPropertyInfo::GetProperty(name);
}

bool wxPanelPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxPanel *panelWindow = (wxPanel *)propertyWindow; 
  wxFont *labelFont = panelWindow->GetLabelFont();
  wxFont *buttonFont = panelWindow->GetButtonFont();
  
  if (labelFont && (name == "labelFontPoints" || name == "labelFontFamily" || name == "labelFontStyle" || name == "labelFontWeight" || name == "labelFontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, labelFont);
    if (newFont)
      panelWindow->SetLabelFont(newFont);
    return TRUE;
  }
  else if (buttonFont && (name == "buttonFontPoints" || name == "buttonFontFamily" || name == "buttonFontStyle" || name == "buttonFontWeight" || name == "buttonFontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, buttonFont);
    if (newFont)
      panelWindow->SetButtonFont(newFont);
    return TRUE;
  }
  else if (name == "no3D")
  {
    bool userColours = property->GetValue().BoolValue();
    long flag = panelWindow->GetWindowStyleFlag();
    
    if (userColours)
    {
      if ((panelWindow->GetWindowStyleFlag() & wxNO_3D) != wxNO_3D)
        panelWindow->SetWindowStyleFlag(panelWindow->GetWindowStyleFlag() | wxNO_3D);
    }
    else
    {
      if ((panelWindow->GetWindowStyleFlag() & wxNO_3D) == wxNO_3D)
        panelWindow->SetWindowStyleFlag(panelWindow->GetWindowStyleFlag() - wxNO_3D);
    }
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    resource->SetStyle(panelWindow->GetWindowStyleFlag());

    panelWindow = (wxPanel *)wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(panelWindow, this);
    return TRUE;
  }
  else if (name == "backgroundColour")
  {
    char *hex = property->GetValue().StringValue();
    int r = wxHexToDec(hex);
    int g = wxHexToDec(hex+2);
    int b = wxHexToDec(hex+4);
    
    wxColour col(r,g,b);
    panelWindow->SetBackgroundColour(col);
    panelWindow = (wxPanel *)wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(panelWindow, this);
   return TRUE;
  }
  else if (name == "title")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    if (resource)
    {
        resource->SetTitle(property->GetValue().StringValue());
        return TRUE;
    }
    else
        return FALSE;
  }
  else
    return wxWindowPropertyInfo::SetProperty(name, property);
}

void wxPanelPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxWindowPropertyInfo::GetPropertyNames(names);
  
  names.Add("title");
  names.Add("no3D");
  names.Add("backgroundColour");
}

bool wxPanelPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxPanel *panel = (wxPanel *)propertyWindow;
  if (panel->GetFont())
    resource->SetFont(wxTheFontList->FindOrCreateFont(panel->GetFont()->GetPointSize(),
		panel->GetFont()->GetFamily(), panel->GetFont()->GetStyle(), panel->GetFont()->GetWeight(),
		panel->GetFont()->GetUnderlined(), panel->GetFont()->GetFaceName()));

  resource->SetBackgroundColour(new wxColour(panel->GetBackgroundColour()));

  return wxWindowPropertyInfo::InstantiateResource(resource);
}

#if 0
/*
 * Dialog boxes
 */

wxProperty *wxDialogPropertyInfo::GetProperty(wxString& name)
{
  wxDialog *dialogWindow = (wxDialog *)propertyWindow; 
  if (name == "modal")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(dialogWindow);
    if (!resource)
      return NULL;

    bool modal = (resource->GetValue1() != 0);
    return new wxProperty(name, modal, "bool");
  }
  else  
    return wxPanelPropertyInfo::GetProperty(name);
}

bool wxDialogPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxDialog *dialogWindow = (wxDialog *)propertyWindow;

  if (name == "modal")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(dialogWindow);
    if (!resource)
      return FALSE;

    resource->SetValue1(property->GetValue().BoolValue());
    return TRUE;
  }
  else
    return wxPanelPropertyInfo::SetProperty(name, property);
}

void wxDialogPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("title");
  names.Add("modal");

  wxPanelPropertyInfo::GetPropertyNames(names);
}

bool wxDialogPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxDialog *dialog = (wxDialog *)propertyWindow;
  wxString str(dialog->GetTitle());
  resource->SetTitle(WXSTRINGCAST str);
    
  return wxPanelPropertyInfo::InstantiateResource(resource);
}
#endif

/*
 * Utilities
 */
 
int wxStringToFontWeight(wxString& val)
{
  if (val == "wxBOLD") return wxBOLD;
  else if (val == "wxLIGHT") return wxLIGHT;
  else return wxNORMAL;
}

int wxStringToFontStyle(wxString& val)
{
  if (val == "wxITALIC") return wxITALIC;
  else if (val == "wxSLANT") return wxSLANT;
  else return wxNORMAL;
}

int wxStringToFontFamily(wxString& val)
{
  if (val == "wxDECORATIVE") return wxDECORATIVE;
  else if (val == "wxROMAN") return wxROMAN;
  else if (val == "wxSCRIPT") return wxSCRIPT;
  else if (val == "wxMODERN") return wxMODERN;
  else if (val == "wxTELETYPE") return wxTELETYPE;
  else return wxSWISS;
}

///
/// Resource symbol validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxResourceSymbolValidator, wxPropertyListValidator)

wxResourceSymbolValidator::wxResourceSymbolValidator(long flags):
  wxPropertyListValidator(flags)
{
}

wxResourceSymbolValidator::~wxResourceSymbolValidator(void)
{
}

bool wxResourceSymbolValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxResourceSymbolValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());
  property->GetValue() = value ;
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxResourceSymbolValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue(str);
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxResourceSymbolValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  OnEdit(property, view, parentWindow);
  return TRUE;
}

bool wxResourceSymbolValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(TRUE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(TRUE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);
  if (view->GetValueText())
    view->GetValueText()->Enable((GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == wxPROP_ALLOW_TEXT_EDITING);
  return TRUE;
}

void wxResourceSymbolValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return;

  wxResourceSymbolDialog* dialog = new wxResourceSymbolDialog(parentWindow, -1, "Edit Symbol");

  dialog->SetSymbol(property->GetValue().StringValue());

  // TODO: split name/id pair e.g. "IDC_TEXT=123" or get from symbol table - which?
  dialog->SetId(1234);

  dialog->Init();

  if (dialog->ShowModal())
  {
    wxString symbolName(dialog->GetSymbol());
    long id = dialog->GetId();
    dialog->Destroy();

    // TODO: set id somewhere
    property->GetValue() = wxString(symbolName);

    view->DisplayProperty(property);
    view->UpdatePropertyDisplayInList(property);
    view->OnPropertyChanged(property);
  }

#if 0
  char *s = wxFileSelector(
     filenameMessage.GetData(),
     wxPathOnly(property->GetValue().StringValue()),
     wxFileNameFromPath(property->GetValue().StringValue()),
     NULL,
     filenameWildCard.GetData(),
     0,
     parentWindow);
  if (s)
  {
    property->GetValue() = wxString(s);
    view->DisplayProperty(property);
    view->UpdatePropertyDisplayInList(property);
    view->OnPropertyChanged(property);
  }
#endif
}

BEGIN_EVENT_TABLE(wxResourceSymbolDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxResourceSymbolDialog::OnOK)
END_EVENT_TABLE()

wxResourceSymbolDialog::wxResourceSymbolDialog(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, style)
{
    int x = 5;
    int y = 5;

    (void) new wxStaticText(this, -1, "Name: ", wxPoint(x, y));

    x += 80;

    m_nameCtrl = new wxComboBox(this, ID_SYMBOLNAME_COMBOBOX, "",
        wxPoint(x, y), wxSize(200, -1), 0, NULL, wxCB_DROPDOWN);

    y += 30;
    x = 5;

    (void) new wxStaticText(this, -1, "Id: ", wxPoint(x, y));

    x += 80;

    m_idCtrl = new wxTextCtrl(this, ID_SYMBOLID_TEXTCTRL, "",
        wxPoint(x, y), wxSize(200, -1));

    y += 30;
    x = 5;
    (void) new wxButton(this, wxID_OK, "OK", wxPoint(x, y), wxSize(90, -1));

    x += 120;
    (void) new wxButton(this, wxID_CANCEL, "Cancel", wxPoint(x, y), wxSize(90, -1));

    Fit();
    Centre();
}

void wxResourceSymbolDialog::Init()
{
    wxString defaultId;
    defaultId.Printf("%ld", m_symbolId);

    m_nameCtrl->SetValue(m_symbolName);
    m_idCtrl->SetValue(defaultId);
}

void wxResourceSymbolDialog::OnOK(wxCommandEvent& event)
{
    if (CheckValues())
    {
        wxDialog::OnOK(event);
    }
}

bool wxResourceSymbolDialog::CheckValues()
{
    return TRUE;
}

