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

#if wxUSE_IOSTREAMH
#if defined(__WXMSW__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#else
#include <strstream>
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
    m_propertyInfo->SetProperty(property->GetName(), property);
    property->GetValue().SetModified(FALSE);
    wxResourceManager::GetCurrentResourceManager()->Modify(TRUE);
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

 /*
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
*/

wxDialogEditorPropertyListFrame::wxDialogEditorPropertyListFrame(wxPropertyListView *v, wxFrame *parent, const wxString& title,
        const wxPoint& pos, const wxSize& size,
        long style, const wxString& name):
            wxPropertyListFrame(v, parent, title, pos, size, style, name)
{
        m_propSheet = NULL;
        m_propInfo = NULL;
}

wxDialogEditorPropertyListFrame::~wxDialogEditorPropertyListFrame()
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
bool wxPropertyInfo::Edit(wxWindow *WXUNUSED(parent), const wxString& title)
{
    if (sm_propertyWindow)
    {
        sm_propertyWindow->Raise();
        return TRUE;
    }

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
     wxPROP_BUTTON_OK | wxPROP_BUTTON_CANCEL |
     wxPROP_BUTTON_CHECK_CROSS|wxPROP_DYNAMIC_VALUE_FIELD|wxPROP_PULLDOWN|wxPROP_SHOWVALUES);

  wxDialogEditorPropertyListFrame *propWin = new wxDialogEditorPropertyListFrame(view,
    wxResourceManager::GetCurrentResourceManager()->GetEditorFrame(), title, wxPoint(x, y),
    wxSize(width, height), wxDEFAULT_FRAME_STYLE);
  sm_propertyWindow = propWin;

  propWin->m_registry.RegisterValidator(wxString("real"), new wxRealListValidator);
  propWin->m_registry.RegisterValidator(wxString("string"), new wxStringListValidator);
  propWin->m_registry.RegisterValidator(wxString("integer"), new wxIntegerListValidator);
  propWin->m_registry.RegisterValidator(wxString("bool"), new wxBoolListValidator);
  propWin->m_registry.RegisterValidator(wxString("filename"), new wxFilenameListValidator);
  propWin->m_registry.RegisterValidator(wxString("stringlist"), new wxListOfStringsListValidator);
  propWin->m_registry.RegisterValidator(wxString("window_id"), new wxResourceSymbolValidator);

  propWin->m_propInfo = this;
  propWin->m_propSheet = propSheet;

//  view->m_propertyWindow = propWin;
  view->AddRegistry(&(propWin->m_registry));

  propWin->Initialize();
  view->ShowView(propSheet, propWin->GetPropertyPanel());

  propWin->Show(TRUE);
  return TRUE;
}

/*
 * wxWindowPropertyInfo
 */

wxWindowPropertyInfo::wxWindowPropertyInfo(wxWindow *win, wxItemResource *res)
{
  m_propertyWindow = win;
  m_propertyResource = res;
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
  wxItemResource* resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);

  wxFont *font = & m_propertyWindow->GetFont();
  if (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" ||
      name == "fontUnderlined")
    return GetFontProperty(name, font);
  else if (name == "name")
    return new wxProperty("name", m_propertyWindow->GetName(), "string");
  else if (name == "title")
    return new wxProperty("title", m_propertyWindow->GetTitle(), "string");
  else if (name == "x")
  {
    return new wxProperty("x", (long)resource->GetX(), "integer");
  }
  else if (name == "y")
  {
    return new wxProperty("y", (long)resource->GetY(), "integer");
  }
  else if (name == "width")
  {
    return new wxProperty("width", (long)resource->GetWidth(), "integer");
  }
  else if (name == "height")
  {
    return new wxProperty("height", (long)resource->GetHeight(), "integer");
  }
  else if (name == "id")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
    if (resource)
    {
        int id = resource->GetId();
        wxString idStr;
        idStr.Printf("%d", id);
        wxString symbolName = wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().GetSymbolForId(id);
        symbolName += "=";
        symbolName += idStr;
        // symbolName is now e.g. "ID_PANEL21=105"
        return new wxProperty("id", symbolName, "window_id");
    }
    else
        return NULL;
  }
  else if (name == "border")
  {
    wxString border("");
    if (m_propertyWindow->GetWindowStyleFlag() & wxSIMPLE_BORDER)
      border = "wxSIMPLE_BORDER";
    else if (m_propertyWindow->GetWindowStyleFlag() & wxRAISED_BORDER)
      border = "wxRAISED_BORDER";
    else if (m_propertyWindow->GetWindowStyleFlag() & wxSUNKEN_BORDER)
      border = "wxSUNKEN_BORDER";
    else if (m_propertyWindow->GetWindowStyleFlag() & wxDOUBLE_BORDER)
      border = "wxDOUBLE_BORDER";
    else if (m_propertyWindow->GetWindowStyleFlag() & wxSTATIC_BORDER)
      border = "wxSTATIC_BORDER";
    else
      border = "wxNO_BORDER";

    return new wxProperty("border", border, "string",
       new wxStringListValidator(new wxStringList("wxSIMPLE_BORDER", "wxRAISED_BORDER",
          "wxSUNKEN_BORDER", "wxDOUBLE_BORDER", "wxSTATIC_BORDER", "wxNO_BORDER", NULL)));
  }
  else
    return NULL;
}

bool wxWindowPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxFont *font = & m_propertyWindow->GetFont();
  if (font && (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" || name == "fontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, font);
    if (newFont)
      m_propertyWindow->SetFont(* newFont);
    return TRUE;
  }
  else if (name == "name")
  {
    // Remove old name from resource table, if it's there.
    wxItemResource *oldResource = (wxItemResource *)wxResourceManager::GetCurrentResourceManager()->GetResourceTable().Delete(m_propertyWindow->GetName());
    if (oldResource)
    {
      // It's a top-level resource
      m_propertyWindow->SetName(property->GetValue().StringValue());
      oldResource->SetName(property->GetValue().StringValue());
      wxResourceManager::GetCurrentResourceManager()->GetResourceTable().Put(m_propertyWindow->GetName(), oldResource);
    }
    else
    {
      // It's a child of something; just set the name of the resource and the window.
      m_propertyWindow->SetName(property->GetValue().StringValue());
      m_propertyResource->SetName(property->GetValue().StringValue());
    }
    // Refresh the resource manager list, because the name changed.
    wxResourceManager::GetCurrentResourceManager()->UpdateResourceList();
    return TRUE;
  }
  else if (name == "title")
  {
    m_propertyWindow->SetTitle(property->GetValue().StringValue());
    return TRUE;
  }
  else if (name == "x")
  {
    int x, y;
    m_propertyWindow->GetPosition(&x, &y);
    int newX = (int)property->GetValue().IntegerValue();

    // We need to convert to pixels if this is not a dialog or panel, but
    // the parent resource specifies dialog units.
    if (m_propertyWindow->GetParent() && m_propertyWindow->IsKindOf(CLASSINFO(wxControl)))
    {
        wxItemResource* parentResource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow->GetParent());
        if (parentResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
        {
            wxPoint pt = m_propertyWindow->GetParent()->ConvertDialogToPixels(wxPoint(newX, y));
            newX = pt.x;
        }
    }
    else if (m_propertyWindow->IsKindOf(CLASSINFO(wxPanel)))
    {
        wxItemResource* resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
        if (resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
        {
            wxPoint pt = m_propertyWindow->ConvertDialogToPixels(wxPoint(newX, y));
            newX = pt.x;
        }
    }

    if (x != newX)
      m_propertyWindow->Move(newX, y);
    return TRUE;
  }
  else if (name == "y")
  {
    int x, y;
    m_propertyWindow->GetPosition(&x, &y);
    int newY = (int)property->GetValue().IntegerValue();

    // We need to convert to pixels if this is not a dialog or panel, but
    // the parent resource specifies dialog units.
    if (m_propertyWindow->GetParent() && m_propertyWindow->IsKindOf(CLASSINFO(wxControl)))
    {
        wxItemResource* parentResource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow->GetParent());
        if (parentResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
        {
            wxPoint pt = m_propertyWindow->GetParent()->ConvertDialogToPixels(wxPoint(x, newY));
            newY = pt.y;
        }
    }
    else if (m_propertyWindow->IsKindOf(CLASSINFO(wxPanel)))
    {
        wxItemResource* resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
        if (resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
        {
            wxPoint pt = m_propertyWindow->ConvertDialogToPixels(wxPoint(x, newY));
            newY = pt.y;
        }
    }

    if (y != newY)
      m_propertyWindow->Move(x, newY);
    return TRUE;
  }
  else if (name == "width")
  {
    int width, height;
    m_propertyWindow->GetSize(&width, &height);
    int newWidth = (int)property->GetValue().IntegerValue();

    // We need to convert to pixels if this is not a dialog or panel, but
    // the parent resource specifies dialog units.
    if (m_propertyWindow->GetParent() && m_propertyWindow->IsKindOf(CLASSINFO(wxControl)))
    {
        wxItemResource* parentResource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow->GetParent());
        if (parentResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
        {
            wxSize sz = m_propertyWindow->GetParent()->ConvertDialogToPixels(wxSize(newWidth, height));
            newWidth = sz.x;
        }
    }
    else if (m_propertyWindow->IsKindOf(CLASSINFO(wxPanel)))
    {
        wxItemResource* resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
        if (resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
        {
            wxSize sz = m_propertyWindow->ConvertDialogToPixels(wxSize(newWidth, height));
            newWidth = sz.x;
        }
    }

    if (width != newWidth)
    {
      m_propertyWindow->SetSize(newWidth, height);
    }
    return TRUE;
  }
  else if (name == "height")
  {
    int width, height;
    m_propertyWindow->GetSize(&width, &height);
    int newHeight = (int)property->GetValue().IntegerValue();

    // We need to convert to pixels if this is not a dialog or panel, but
    // the parent resource specifies dialog units.
    if (m_propertyWindow->GetParent() && m_propertyWindow->IsKindOf(CLASSINFO(wxControl)))
    {
        wxItemResource* parentResource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow->GetParent());
        if (parentResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
        {
            wxSize sz = m_propertyWindow->GetParent()->ConvertDialogToPixels(wxSize(width, newHeight));
            newHeight = sz.y;
        }
    }
    else if (m_propertyWindow->IsKindOf(CLASSINFO(wxPanel)))
    {
        wxItemResource* resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
        if (resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
        {
            wxSize sz = m_propertyWindow->ConvertDialogToPixels(wxSize(width, newHeight));
            newHeight = sz.y;
        }
    }

    if (height != newHeight)
    {
      m_propertyWindow->SetSize(width, newHeight);
    }
    return TRUE;
  }
  else if (name == "id")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
    if (resource)
    {
        wxString value = property->GetValue().StringValue();

        wxString strName = value.Before('=');
        wxString strId = value.After('=');
        int id = atoi(strId);

        wxString oldSymbolName = wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().GetSymbolForId(resource->GetId());
        int oldSymbolId = resource->GetId();

        if (strName != "")
        {
            // If we change the id for an existing symbol, we need to:
            // 1) Check if there are any other resources currently using the original id.
            //    If so, will need to change their id to the new id.
            // 2) Remove the old symbol, add the new symbol.
            // In this check, we don't have to do this, but we need to do it in SetProperty.

            if (strName == oldSymbolName && id != oldSymbolId)
            {
                wxASSERT( (!wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().IsStandardSymbol(oldSymbolName)) );

                // It's OK to change just the id. But we'll need to change all matching ids in all resources,
                // because ids are unique and changing one resource's id must change all identical ones.
                wxResourceManager::GetCurrentResourceManager()->ChangeIds(oldSymbolId, id);

                wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().RemoveSymbol(oldSymbolName);
                wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().AddSymbol(strName, id);
            }

            // If we change the name but not the id, we'll just need to remove and
            // re-add the symbol/id pair.
            if (strName != oldSymbolName && id == oldSymbolId)
            {
                wxASSERT( (!wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().IsStandardSymbol(oldSymbolName)) );

                wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().RemoveSymbol(oldSymbolName);

                if (!wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().SymbolExists(strName))
                {
                    wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().AddSymbol(strName, id);
                }
            }

            // What if we're changing both the name and the id?
            // - if there's no symbol of that name, just remove the old, add the new (in SetProperty)
            // - if there is a symbol of that name, if id matches, do nothing. If not, veto.

            if (strName != oldSymbolName && id != oldSymbolId)
            {
                // Remove old symbol if it's not being used
                if (!wxResourceManager::GetCurrentResourceManager()->IsSymbolUsed(resource, oldSymbolId) &&
                    !wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().IsStandardSymbol(oldSymbolName))
                {
                    wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().RemoveSymbol(oldSymbolName);
                }

                if (!wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().SymbolExists(strName))
                {
                    wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().AddSymbol(strName, id);
                }
            }
            resource->SetId(id);
        }

        return TRUE;
    }
    else
        return FALSE;
  }
  else if (name == "border")
  {
    long borderStyle = wxNO_BORDER;
    wxString val = property->GetValue().StringValue();

    if (val == "wxSIMPLE_BORDER")
        borderStyle = wxSIMPLE_BORDER;
    else if (val == "wxRAISED_BORDER")
        borderStyle = wxRAISED_BORDER;
    else if (val == "wxSUNKEN_BORDER")
        borderStyle = wxSUNKEN_BORDER;
    else if (val == "wxDOUBLE_BORDER")
        borderStyle = wxDOUBLE_BORDER;
    else if (val == "wxSTATIC_BORDER")
        borderStyle = wxSTATIC_BORDER;
    else
        borderStyle = wxNO_BORDER;

    SetWindowStyle(m_propertyWindow, wxSIMPLE_BORDER, FALSE);
    SetWindowStyle(m_propertyWindow, wxRAISED_BORDER, FALSE);
    SetWindowStyle(m_propertyWindow, wxSUNKEN_BORDER, FALSE);
    SetWindowStyle(m_propertyWindow, wxDOUBLE_BORDER, FALSE);
    SetWindowStyle(m_propertyWindow, wxSTATIC_BORDER, FALSE);
    SetWindowStyle(m_propertyWindow, wxNO_BORDER, FALSE);

    SetWindowStyle(m_propertyWindow, borderStyle, TRUE);

    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
    resource->SetStyle(m_propertyWindow->GetWindowStyleFlag());
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
  names.Add("border");
  if (!m_propertyWindow->IsKindOf(CLASSINFO(wxControl)))
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
//  resource->SetType(m_propertyWindow->GetClassInfo()->GetClassName());

//  resource->SetStyle(m_propertyWindow->GetWindowStyleFlag());
  wxString str(m_propertyWindow->GetName());
  resource->SetName(str);

#if 0
  int x, y, w, h;

  if (m_propertyWindow->IsKindOf(CLASSINFO(wxPanel)))
      m_propertyWindow->GetClientSize(&w, &h);
  else
      m_propertyWindow->GetSize(&w, &h);

  m_propertyWindow->GetPosition(&x, &y);

  // We need to convert to dialog units if this is not a dialog or panel, but
  // the parent resource specifies dialog units.
  if (m_propertyWindow->GetParent() && m_propertyWindow->IsKindOf(CLASSINFO(wxControl)))
  {
      wxItemResource* parentResource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow->GetParent());
      if (parentResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
      {
          wxPoint pt = m_propertyWindow->GetParent()->ConvertPixelsToDialog(wxPoint(x, y));
          x = pt.x; y = pt.y;
          wxSize sz = m_propertyWindow->GetParent()->ConvertPixelsToDialog(wxSize(w, h));
          w = sz.x; h = sz.y;
      }
  }
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxPanel)))
  {
      if (resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS)
      {
          wxPoint pt = m_propertyWindow->ConvertPixelsToDialog(wxPoint(x, y));
          x = pt.x; y = pt.y;
          wxSize sz = m_propertyWindow->ConvertPixelsToDialog(wxSize(w, h));
          w = sz.x; h = sz.y;
      }
  }

  resource->SetSize(x, y, w, h);
#endif

  return TRUE;
}

// Set the window style
void wxWindowPropertyInfo::SetWindowStyle(wxWindow* win, long style, bool set)
{
    if (style == 0)
        return;

    if ((win->GetWindowStyleFlag() & style) == style)
    {
        if (!set)
        {
            win->SetWindowStyleFlag(win->GetWindowStyleFlag() - style);
        }
    }
    else
    {
        if (set)
        {
            win->SetWindowStyleFlag(win->GetWindowStyleFlag() | style);
        }
    }
}

/*
 * Controls
 */

wxProperty *wxItemPropertyInfo::GetProperty(wxString& name)
{
  wxControl *itemWindow = (wxControl *)m_propertyWindow; 
  wxFont *font = & itemWindow->GetFont();

  if (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" ||
      name == "fontUnderlined")
    return GetFontProperty(name, font);
  else if (name == "label" && itemWindow->GetLabel())
    return new wxProperty("label", m_propertyWindow->GetLabel(), "string");
  else
    return wxWindowPropertyInfo::GetProperty(name);
}

bool wxItemPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxControl *itemWindow = (wxControl *)m_propertyWindow; 
  wxFont *font = & itemWindow->GetFont();

  if (font && (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" || name == "fontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, font);
    if (newFont)
      itemWindow->SetLabelFont(* newFont);
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
  
  wxControl *item = (wxControl *)m_propertyWindow;
  wxString str(item->GetLabel());
  resource->SetTitle(str);
  if (item->GetFont().Ok())
    resource->SetFont(* wxTheFontList->FindOrCreateFont(item->GetFont().GetPointSize(),
		item->GetFont().GetFamily(), item->GetFont().GetStyle(), item->GetFont().GetWeight(),
		item->GetFont().GetUnderlined(), item->GetFont().GetFaceName()));
  return TRUE;
}

/*
 * Button
 */

wxProperty *wxButtonPropertyInfo::GetProperty(wxString& name)
{
  return wxItemPropertyInfo::GetProperty(name);
}

bool wxButtonPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxButtonPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("label");
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
  wxBitmapButton *button = (wxBitmapButton *)m_propertyWindow;
  if (name == "bitmapFilename")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(button);
    wxString str("none.bmp");
   
    if (resource)
    {
      str = wxResourceManager::GetCurrentResourceManager()->FindBitmapFilenameForResource(resource);
    }
    return new wxProperty("bitmapFilename", str.GetData(), "string", new wxFilenameListValidator("Select a bitmap file", "*.bmp"));
  }
  else
    return wxButtonPropertyInfo::GetProperty(name);
}

bool wxBitmapButtonPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxBitmapButton *button = (wxBitmapButton *)m_propertyWindow;
  if (name == "bitmapFilename")
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
          wxString resName = wxResourceManager::GetCurrentResourceManager()->AddBitmapResource(s);
          resource->SetValue4(resName);
          
          if (!oldResource.IsNull())
            wxResourceManager::GetCurrentResourceManager()->PossiblyDeleteBitmapResource(oldResource);
        }

        button->SetLabel(* bitmap);
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
  wxButtonPropertyInfo::GetPropertyNames(names);
  names.Delete("label");
  names.Add("bitmapFilename");
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
  return wxItemPropertyInfo::GetProperty(name);
}

bool wxStaticTextPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxStaticTextPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("label");
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
  wxStaticBitmap *message = (wxStaticBitmap *)m_propertyWindow;
  if (name == "bitmapFilename")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(message);
    wxString str("none.bmp");
   
    if (resource)
    {
      str = wxResourceManager::GetCurrentResourceManager()->FindBitmapFilenameForResource(resource);
    }
    return new wxProperty("bitmapFilename", str.GetData(), "string", new wxFilenameListValidator("Select a bitmap file", "*.bmp"));
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxStaticBitmapPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxStaticBitmap *message = (wxStaticBitmap *)m_propertyWindow;
  if (name == "bitmapFilename")
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
          wxString resName = wxResourceManager::GetCurrentResourceManager()->AddBitmapResource(s);
          resource->SetValue4(resName);
          
          if (!oldResource.IsNull())
            wxResourceManager::GetCurrentResourceManager()->PossiblyDeleteBitmapResource(oldResource);
        }

        message->SetBitmap(* bitmap);
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
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("bitmapFilename");
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
  wxTextCtrl *text = (wxTextCtrl *)m_propertyWindow;
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
  wxTextCtrl *text = (wxTextCtrl *)m_propertyWindow;
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
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("value");
  names.Add("readonly");
  names.Add("password");
}

bool wxTextPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxTextCtrl *text = (wxTextCtrl *)m_propertyWindow;
  wxString str(text->GetValue());
  resource->SetValue4(str);
    
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Listbox item
 */

wxProperty *wxListBoxPropertyInfo::GetProperty(wxString& name)
{
  wxListBox *listBox = (wxListBox *)m_propertyWindow;
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
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(listBox);
    if (!resource)
      return NULL;

    char *mult = "wxLB_SINGLE";
      
    if ((listBox->GetWindowStyleFlag() & wxLB_MULTIPLE) != 0)
        mult = "wxLB_MULTIPLE";
    else if ((listBox->GetWindowStyleFlag() & wxLB_EXTENDED) != 0)
        mult = "wxLB_EXTENDED";
    else
        mult = "wxLB_SINGLE";

    return new wxProperty("multiple", mult, "string",
       new wxStringListValidator(new wxStringList("wxLB_SINGLE", "wxLB_MULTIPLE", "wxLB_EXTENDED",
          NULL)));
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxListBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxListBox *listBox = (wxListBox *)m_propertyWindow;
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
    SetWindowStyle(m_propertyWindow, wxLB_SINGLE, FALSE);
    SetWindowStyle(m_propertyWindow, wxLB_MULTIPLE, FALSE);
    SetWindowStyle(m_propertyWindow, wxLB_EXTENDED, FALSE);

    wxString str(property->GetValue().StringValue());
    if (str == "wxLB_MULTIPLE")
      SetWindowStyle(m_propertyWindow, wxLB_MULTIPLE, TRUE);
    else if (str == "wxLB_EXTENDED")
      SetWindowStyle(m_propertyWindow, wxLB_EXTENDED, TRUE);
    else
      SetWindowStyle(m_propertyWindow, wxLB_SINGLE, TRUE);

    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(listBox);
    if (resource)
      resource->SetStyle(m_propertyWindow->GetWindowStyleFlag());
    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(listBox, this);
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxListBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("values");
  names.Add("multiple");
}

bool wxListBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxListBox *lbox = (wxListBox *)m_propertyWindow;
  // This will be set for the wxItemResource on reading or in SetProperty
//  resource->SetValue1(lbox->GetSelectionMode());
  int i;
  if (lbox->Number() == 0)
    resource->SetStringValues(NULL);
  else
  {
    wxStringList slist;
    
    for (i = 0; i < lbox->Number(); i++)
      slist.Add(lbox->GetString(i));
      
    resource->SetStringValues(slist);
  }
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Choice item
 */

wxProperty *wxChoicePropertyInfo::GetProperty(wxString& name)
{
  wxChoice *choice = (wxChoice *)m_propertyWindow;
  if (name == "values")
  {
    wxStringList* stringList = new wxStringList;
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
  wxChoice *choice = (wxChoice *)m_propertyWindow;
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
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("values");
}

bool wxChoicePropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxChoice *choice = (wxChoice *)m_propertyWindow;
  int i;
  if (choice->Number() == 0)
    resource->SetStringValues(NULL);
  else
  {
    wxStringList slist;
    
    for (i = 0; i < choice->Number(); i++)
      slist.Add(choice->GetString(i));
      
    resource->SetStringValues(slist);
  }
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Choice item
 */

wxProperty *wxComboBoxPropertyInfo::GetProperty(wxString& name)
{
  wxComboBox *choice = (wxComboBox *)m_propertyWindow;
  if (name == "values")
  {
    wxStringList *stringList = new wxStringList;
    int i;
    for (i = 0; i < choice->Number(); i++)
      stringList->Add(choice->GetString(i));

    return new wxProperty(name, stringList, "stringlist");
  }
  else if (name == "sort")
  {
    bool sort = ((m_propertyWindow->GetWindowStyleFlag() & wxCB_SORT) == wxCB_SORT);
    return new wxProperty(name, sort, "bool");
  }
  else if (name == "style")
  {
    wxString styleStr("dropdown");
    if (m_propertyWindow->GetWindowStyleFlag() & wxCB_SIMPLE)
      styleStr = "simple";
    else if (m_propertyWindow->GetWindowStyleFlag() & wxCB_READONLY)
      styleStr = "readonly";
    else
      styleStr = "dropdown";

    return new wxProperty(name, styleStr, "string",
       new wxStringListValidator(new wxStringList("simple", "dropdown", "readonly",
          NULL)));
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxComboBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxComboBox *choice = (wxComboBox *)m_propertyWindow;
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
  else if (name == "sort")
  {
    SetWindowStyle(m_propertyWindow, wxCB_SORT, property->GetValue().BoolValue());

    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
    resource->SetStyle(m_propertyWindow->GetWindowStyleFlag());

    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(m_propertyWindow, this);
    return TRUE;
  }
  else if (name == "style")
  {
    SetWindowStyle(m_propertyWindow, wxCB_SIMPLE, FALSE);
    SetWindowStyle(m_propertyWindow, wxCB_DROPDOWN, FALSE);
    SetWindowStyle(m_propertyWindow, wxCB_READONLY, FALSE);

    wxString styleStr(property->GetValue().StringValue());
    if (styleStr == "simple")
      SetWindowStyle(m_propertyWindow, wxCB_SIMPLE, TRUE);
    else if (styleStr == "dropdown")
      SetWindowStyle(m_propertyWindow, wxCB_DROPDOWN, TRUE);
    else if (styleStr == "readonly")
      SetWindowStyle(m_propertyWindow, wxCB_READONLY, TRUE);

      // Necesary?
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
    resource->SetStyle(m_propertyWindow->GetWindowStyleFlag());

    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(m_propertyWindow, this);

    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxComboBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("values");
  names.Add("style");
  names.Add("sort");
}

bool wxComboBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxComboBox *choice = (wxComboBox *)m_propertyWindow;
  int i;
  if (choice->Number() == 0)
    resource->SetStringValues(NULL);
  else
  {
    wxStringList slist;
    
    for (i = 0; i < choice->Number(); i++)
      slist.Add(choice->GetString(i));
      
    resource->SetStringValues(slist);
  }
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Radiobox item
 */

wxProperty *wxRadioBoxPropertyInfo::GetProperty(wxString& name)
{
  wxRadioBox *radioBox = (wxRadioBox *)m_propertyWindow;
  if (name == "numberRowsOrCols")
  {
    return new wxProperty("numberRowsOrCols", (long)radioBox->GetNumberOfRowsOrCols(), "integer");
  }
  if (name == "orientation")
  {
    wxString orient;
    if (m_propertyWindow->GetWindowStyleFlag() & wxRA_SPECIFY_COLS)
      orient = "wxRA_SPECIFY_COLS";
    else
      orient = "wxRA_SPECIFY_ROWS";

    return new wxProperty("orientation", orient, "string",
       new wxStringListValidator(new wxStringList("wxRA_SPECIFY_COLS", "wxRA_SPECIFY_ROWS",
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
  wxRadioBox *radioBox = (wxRadioBox *)m_propertyWindow;
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
    if (val == "wxRA_SPECIFY_COLS")
    {
      if (windowStyle & wxRA_SPECIFY_ROWS)
        windowStyle -= wxRA_SPECIFY_ROWS;
      windowStyle |= wxRA_SPECIFY_COLS;
    }
    else
    {
      if (windowStyle & wxRA_SPECIFY_COLS)
        windowStyle -= wxRA_SPECIFY_COLS;
      windowStyle |= wxRA_SPECIFY_ROWS;
    }
    radioBox->SetWindowStyleFlag(windowStyle);
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(radioBox);
    resource->SetStyle(windowStyle);
    
    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(radioBox, this);
    return TRUE;
  }
  else if (name == "values")
  {
    // Set property into *resource*, not wxRadioBox, and then recreate
    // the wxRadioBox. This is because we can't dynamically set the strings
    // of a wxRadioBox.
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);
    if (!resource)
      return FALSE;
      
    wxStringList stringList;
    wxPropertyValue *expr = property->GetValue().GetFirst();
    while (expr)
    {
      char *s = expr->StringValue();
      if (s)
        stringList.Add(s);
      expr = expr->GetNext();
    }
    resource->SetStringValues(stringList);
    wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(radioBox, this);
    return TRUE;
  }
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxRadioBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("label");
  names.Add("values");
  names.Add("orientation");
  names.Add("numberRowsOrCols");
}

bool wxRadioBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxRadioBox *rbox = (wxRadioBox *)m_propertyWindow;
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
  return wxItemPropertyInfo::GetProperty(name);
}

bool wxGroupBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxGroupBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{  
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("label");
}

bool wxGroupBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Checkbox item
 */

wxProperty *wxCheckBoxPropertyInfo::GetProperty(wxString& name)
{
  wxCheckBox *checkBox = (wxCheckBox *)m_propertyWindow;
  if (name == "value")
    return new wxProperty("value", checkBox->GetValue(), "bool");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxCheckBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxCheckBox *checkBox = (wxCheckBox *)m_propertyWindow;
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
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("label");
  names.Add("value");
}

bool wxCheckBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxCheckBox *cbox = (wxCheckBox *)m_propertyWindow;
  resource->SetValue1(cbox->GetValue());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Radiobutton item
 */

wxProperty *wxRadioButtonPropertyInfo::GetProperty(wxString& name)
{
  wxRadioButton *checkBox = (wxRadioButton *)m_propertyWindow;
  if (name == "value")
    return new wxProperty("value", checkBox->GetValue(), "bool");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxRadioButtonPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxRadioButton *checkBox = (wxRadioButton *)m_propertyWindow;
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
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("label");
  names.Add("value");
}

bool wxRadioButtonPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxRadioButton *cbox = (wxRadioButton *)m_propertyWindow;
  resource->SetValue1(cbox->GetValue());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Slider item
 */

wxProperty *wxSliderPropertyInfo::GetProperty(wxString& name)
{
  wxSlider *slider = (wxSlider *)m_propertyWindow;
  if (name == "value")
    return new wxProperty("value", (long)slider->GetValue(), "integer");
  else if (name == "orientation")
  {
    char *pos = NULL;
    if (m_propertyWindow->GetWindowStyleFlag() & wxHORIZONTAL)
      pos = "wxHORIZONTAL";
    else
      pos = "wxVERTICAL";

    return new wxProperty("orientation", pos, "string",
       new wxStringListValidator(new wxStringList("wxHORIZONTAL", "wxVERTICAL",
          NULL)));
  }
  else if (name == "minValue")
    return new wxProperty("minValue", (long)slider->GetMin(), "integer");
  else if (name == "maxValue")
    return new wxProperty("maxValue", (long)slider->GetMax(), "integer");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxSliderPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxSlider *slider = (wxSlider *)m_propertyWindow;
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
  else if (name == "minValue")
  {
    slider->SetRange((int)property->GetValue().IntegerValue(), slider->GetMax());
    return TRUE;
  }
  else if (name == "maxValue")
  {
    slider->SetRange(slider->GetMin(), (int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxSliderPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("value");
  names.Add("orientation");
  names.Add("minValue");
  names.Add("maxValue");
}

bool wxSliderPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxSlider *slider = (wxSlider *)m_propertyWindow;
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
  wxGauge *gauge = (wxGauge *)m_propertyWindow;
  if (name == "value")
    return new wxProperty("value", (long)gauge->GetValue(), "integer");
  else if (name == "maxValue")
    return new wxProperty("maxValue", (long)gauge->GetRange(), "integer");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

bool wxGaugePropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxGauge *gauge = (wxGauge *)m_propertyWindow;
  if (name == "value")
  {
    gauge->SetValue((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "maxValue")
  {
    gauge->SetRange((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxGaugePropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("value");
  names.Add("maxValue");
}

bool wxGaugePropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxGauge *gauge = (wxGauge *)m_propertyWindow;
  resource->SetValue1(gauge->GetValue());
  resource->SetValue2(gauge->GetRange());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Scrollbar item
 */

wxProperty *wxScrollBarPropertyInfo::GetProperty(wxString& name)
{
  wxScrollBar *scrollBar = (wxScrollBar *)m_propertyWindow;
  if (name == "value")
    return new wxProperty("value", (long)scrollBar->GetValue(), "integer");
  else if (name == "orientation")
  {
    char *pos = NULL;
    if (m_propertyWindow->GetWindowStyleFlag() & wxHORIZONTAL)
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
  wxScrollBar *scrollBar = (wxScrollBar *)m_propertyWindow;
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
  wxItemPropertyInfo::GetPropertyNames(names);
  names.Add("orientation");
  names.Add("value");
  names.Add("pageSize");
  names.Add("viewLength");
  names.Add("objectLength");

  // Remove some properties we don't inherit
  names.Delete("fontPoints");
  names.Delete("fontFamily");
  names.Delete("fontStyle");
  names.Delete("fontWeight");
  names.Delete("fontUnderlined");
}

bool wxScrollBarPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxScrollBar *sbar = (wxScrollBar *)m_propertyWindow;

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
  wxPanel *panelWindow = (wxPanel *)m_propertyWindow; 

/*
  wxFont *labelFont = panelWindow->GetLabelFont();
  wxFont *buttonFont = panelWindow->GetButtonFont();
  
  if (name == "labelFontPoints" || name == "labelFontFamily" || name == "labelFontStyle" || name == "labelFontWeight" ||
      name == "labelFontUnderlined")
    return GetFontProperty(name, labelFont);
  else if (name == "buttonFontPoints" || name == "buttonFontFamily" || name == "buttonFontStyle" || name == "buttonFontWeight" ||
      name == "buttonFontUnderlined")
    return GetFontProperty(name, buttonFont);
*/

  if (name == "no3D")
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
  else if (name == "caption")
  {
    return new wxProperty(name, ((panelWindow->GetWindowStyleFlag() & wxCAPTION) == wxCAPTION),
        "bool");
  }
  else if (name == "systemMenu")
  {
    return new wxProperty(name, ((panelWindow->GetWindowStyleFlag() & wxSYSTEM_MENU) == wxSYSTEM_MENU),
        "bool");
  }
  else if (name == "thickFrame")
  {
    return new wxProperty(name, ((panelWindow->GetWindowStyleFlag() & wxTHICK_FRAME) == wxTHICK_FRAME),
        "bool");
  }
  else if (name == "useSystemDefaults")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    return new wxProperty(name, ((resource->GetResourceStyle() & wxRESOURCE_USE_DEFAULTS) == wxRESOURCE_USE_DEFAULTS),
        "bool");
  }
  else if (name == "useDialogUnits")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    return new wxProperty(name, ((resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) == wxRESOURCE_DIALOG_UNITS),
        "bool");
  }
  else
    return wxWindowPropertyInfo::GetProperty(name);
}

bool wxPanelPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxPanel *panelWindow = (wxPanel *)m_propertyWindow; 
/*
  wxFont *labelFont = panelWindow->GetLabelFont();
  wxFont *buttonFont = panelWindow->GetButtonFont();

  if (labelFont && (name == "labelFontPoints" || name == "labelFontFamily" || name == "labelFontStyle" || name == "labelFontWeight" || name == "labelFontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, labelFont);
    if (newFont)
      panelWindow->SetLabelFont(* newFont);
    return TRUE;
  }
  else if (buttonFont && (name == "buttonFontPoints" || name == "buttonFontFamily" || name == "buttonFontStyle" || name == "buttonFontWeight" || name == "buttonFontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, buttonFont);
    if (newFont)
      panelWindow->SetButtonFont(* newFont);
    return TRUE;
  }
*/

  if (name == "no3D")
  {
    bool userColours = property->GetValue().BoolValue();
    
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
  else if (name == "caption")
  {
    SetWindowStyle(panelWindow, wxCAPTION, property->GetValue().BoolValue());

    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    resource->SetStyle(panelWindow->GetWindowStyleFlag());
    return TRUE;
  }
  else if (name == "thickFrame")
  {
    SetWindowStyle(panelWindow, wxTHICK_FRAME, property->GetValue().BoolValue());

    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    resource->SetStyle(panelWindow->GetWindowStyleFlag());
    return TRUE;
  }
  else if (name == "systemMenu")
  {
    SetWindowStyle(panelWindow, wxSYSTEM_MENU, property->GetValue().BoolValue());

    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    resource->SetStyle(panelWindow->GetWindowStyleFlag());
    return TRUE;
  }
  else if (name == "useSystemDefaults")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    bool useDefaults = property->GetValue().BoolValue();
    long style = resource->GetResourceStyle();
    if (useDefaults)
    {
        if ((style & wxRESOURCE_USE_DEFAULTS) == 0)
            style |= wxRESOURCE_USE_DEFAULTS;
    }
    else
    {
        if ((style & wxRESOURCE_USE_DEFAULTS) != 0)
            style -= wxRESOURCE_USE_DEFAULTS;
    }
    resource->SetResourceStyle(style);
    panelWindow = (wxPanel *)wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(panelWindow, this);
    return TRUE;
  }
  else if (name == "useDialogUnits")
  {
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(panelWindow);
    bool useDialogUnits = property->GetValue().BoolValue();
    long style = resource->GetResourceStyle();
    if (useDialogUnits)
    {
        if ((style & wxRESOURCE_DIALOG_UNITS) == 0)
        {
            style |= wxRESOURCE_DIALOG_UNITS;
            ConvertDialogUnits(TRUE); // Convert all resources
        }
    }
    else
    {
        if ((style & wxRESOURCE_DIALOG_UNITS) != 0)
        {
            style -= wxRESOURCE_DIALOG_UNITS;
            ConvertDialogUnits(FALSE); // Convert all resources
        }
    }
    resource->SetResourceStyle(style);
    panelWindow = (wxPanel *)wxResourceManager::GetCurrentResourceManager()->RecreateWindowFromResource(panelWindow, this);
    // TODO: need to regenerate the width and height properties else they'll be inconsistent.
    return TRUE;
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
  names.Add("caption");
  names.Add("systemMenu");
  names.Add("thickFrame");
  names.Add("useSystemDefaults");
  names.Add("useDialogUnits");
}

bool wxPanelPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxPanel *panel = (wxPanel *)m_propertyWindow;
  if (panel->GetFont().Ok())
    resource->SetFont(* wxTheFontList->FindOrCreateFont(panel->GetFont().GetPointSize(),
		panel->GetFont().GetFamily(), panel->GetFont().GetStyle(), panel->GetFont().GetWeight(),
		panel->GetFont().GetUnderlined(), panel->GetFont().GetFaceName()));

  resource->SetBackgroundColour(wxColour(panel->GetBackgroundColour()));

  return wxWindowPropertyInfo::InstantiateResource(resource);
}

// Convert this dialog, and its children, to or from dialog units
void wxPanelPropertyInfo::ConvertDialogUnits(bool toDialogUnits)
{
    wxItemResource *resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(m_propertyWindow);

    wxPoint pt;
    wxSize sz;
    if (toDialogUnits)
    {
        sz = m_propertyWindow->ConvertPixelsToDialog(wxSize(resource->GetWidth(), resource->GetHeight()));
        pt = m_propertyWindow->ConvertPixelsToDialog(wxPoint(resource->GetX(), resource->GetY()));
    }
    else
    {
        sz = m_propertyWindow->ConvertDialogToPixels(wxSize(resource->GetWidth(), resource->GetHeight()));
        pt = m_propertyWindow->ConvertDialogToPixels(wxPoint(resource->GetX(), resource->GetY()));
    }
    resource->SetSize(pt.x, pt.y, sz.x, sz.y);

    wxNode* node = m_propertyWindow->GetChildren().First();
    while (node)
    {
        wxWindow* child = (wxWindow*) node->Data();
        if (child->IsKindOf(CLASSINFO(wxControl)))
        {
            resource = wxResourceManager::GetCurrentResourceManager()->FindResourceForWindow(child);
            if (toDialogUnits)
            {
                sz = m_propertyWindow->ConvertPixelsToDialog(wxSize(resource->GetWidth(), resource->GetHeight()));
                pt = m_propertyWindow->ConvertPixelsToDialog(wxPoint(resource->GetX(), resource->GetY()));
            }
            else
            {
                sz = m_propertyWindow->ConvertDialogToPixels(wxSize(resource->GetWidth(), resource->GetHeight()));
                pt = m_propertyWindow->ConvertDialogToPixels(wxPoint(resource->GetX(), resource->GetY()));
            }
            resource->SetSize(pt.x, pt.y, sz.x, sz.y);
        }
        node = node->Next();
    }
}

#if 0
/*
 * Dialog boxes
 */

wxProperty *wxDialogPropertyInfo::GetProperty(wxString& name)
{
  wxDialog *dialogWindow = (wxDialog *)m_propertyWindow; 
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
  wxDialog *dialogWindow = (wxDialog *)m_propertyWindow;

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
  wxPanelPropertyInfo::GetPropertyNames(names);
  names.Add("title");
  names.Add("modal");
}

bool wxDialogPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxDialog *dialog = (wxDialog *)m_propertyWindow;
  wxString str(dialog->GetTitle());
  resource->SetTitle(str);
    
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

bool wxResourceSymbolValidator::OnCheckValue(wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow))
{
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxResourceSymbolValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
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
bool wxResourceSymbolValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
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

bool wxResourceSymbolValidator::OnPrepareControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
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

  // Split name/id pair e.g. "IDC_TEXT=123"
  wxString value(property->GetValue().StringValue());

  wxString strName = value.Before('=');
  wxString strId = value.After('=');

  dialog->SetSymbol(strName);
  dialog->SetId(atoi(strId));

  dialog->Init();

  if (dialog->ShowModal() == wxID_OK)
  {
    wxString symbolName(dialog->GetSymbol());
    long id = dialog->GetId();
    dialog->Destroy();

    wxString str;
    str.Printf("%d", id);
    property->GetValue() = symbolName + wxString("=") + str;

    view->DisplayProperty(property);
    view->UpdatePropertyDisplayInList(property);
    view->OnPropertyChanged(property);
  }
}

BEGIN_EVENT_TABLE(wxResourceSymbolDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxResourceSymbolDialog::OnOK)
    EVT_COMBOBOX(ID_SYMBOLNAME_COMBOBOX, wxResourceSymbolDialog::OnComboBoxSelect)
    EVT_TEXT(ID_SYMBOLNAME_COMBOBOX, wxResourceSymbolDialog::OnSymbolNameUpdate)
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
        wxPoint(x, y), wxSize(200, -1), 0, NULL, wxCB_DROPDOWN|wxCB_SORT);

    y += 30;
    x = 5;

    (void) new wxStaticText(this, -1, "Id: ", wxPoint(x, y));

    x += 80;

    m_idCtrl = new wxTextCtrl(this, ID_SYMBOLID_TEXTCTRL, "",
        wxPoint(x, y), wxSize(200, -1));

    y += 30;
    x = 5;
    (void) new wxButton(this, wxID_OK, "OK", wxPoint(x, y), wxSize(80, -1));

    x += 100;
    (void) new wxButton(this, wxID_CANCEL, "Cancel", wxPoint(x, y), wxSize(80, -1));

    Fit();
    Centre();
}

void wxResourceSymbolDialog::Init()
{
    wxString defaultId;
    defaultId.Printf("%ld", m_symbolId);

    m_nameCtrl->SetValue(m_symbolName);
    m_idCtrl->SetValue(defaultId);

    wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().FillComboBox(m_nameCtrl);
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
    wxString nameStr(m_nameCtrl->GetValue());
    wxString idStr(m_idCtrl->GetValue());
    int id = atoi(idStr);

    if (id <= 0 )
    {
        wxMessageBox("Identifier cannot be missing or zero", "Dialog Editor", wxOK|wxICON_EXCLAMATION, this);
        return FALSE;
    }
    if (nameStr == "")
    {
        wxMessageBox("Please enter a symbol name", "Dialog Editor", wxOK|wxICON_EXCLAMATION, this);
        return FALSE;
    }
    if (nameStr.Contains(" "))
    {
        wxMessageBox("Symbol name cannot contain spaces.", "Dialog Editor", wxOK|wxICON_EXCLAMATION, this);
        return FALSE;
    }
    if (nameStr.Contains("="))
    {
        wxMessageBox("Symbol name cannot contain =.", "Dialog Editor", wxOK|wxICON_EXCLAMATION, this);
        return FALSE;
    }
    if (nameStr.IsNumber())
    {
        wxMessageBox("Symbol name cannot be a number.", "Dialog Editor", wxOK|wxICON_EXCLAMATION, this);
        return FALSE;
    }
    // TODO: other checks on the name syntax.

    if (!wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().IsStandardSymbol(nameStr))
    {
        // If we change the id for an existing symbol, we need to:
        // 1) Check if there are any other resources currently using the original id.
        //    If so, will need to change their id to the new id, in SetProperty.
        // 2) Remove the old symbol, add the new symbol.
        // In this check, we don't have to do this, but we need to do it in SetProperty.

        if (nameStr == GetSymbol() && id != GetId())
        {
            // It's OK to change the id. But we'll need to change all matching ids in all resources,
            // in SetProperty.
        }

        // If we change the name but not the id... we'll just need to remove and
        // re-add the symbol/id pair, in SetProperty.
        if (nameStr != GetSymbol() && id == GetId())
        {
        }

        // What if we're changing both the name and the id?
        // - if there's no symbol of that name, just remove the old, add the new (in SetProperty)
        // - if there is a symbol of that name, if id matches, do nothing. If not, veto.

        if (nameStr != GetSymbol() && id != GetId())
        {
            if (!wxResourceManager::GetCurrentResourceManager()->IsIdentifierOK(nameStr, id))
            {
                wxMessageBox("This integer id is already being used under a different name.\nPlease choose another.",
                    "Dialog Editor", wxOK|wxICON_EXCLAMATION, this);
                return FALSE;
            }
        }

    }

    SetSymbol(nameStr);
    SetId(id);

    return TRUE;
}

void wxResourceSymbolDialog::OnComboBoxSelect(wxCommandEvent& WXUNUSED(event))
{
    wxString str(m_nameCtrl->GetStringSelection());
    if (wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().IsStandardSymbol(str))
    {
        int id = wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().GetIdForSymbol(str);
        wxString str2;
        str2.Printf("%d", id);
        m_idCtrl->SetValue(str2);
        m_idCtrl->Enable(FALSE);
    }
    else
    {
        if (wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().SymbolExists(str))
        {
            int id = wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().GetIdForSymbol(str);
            wxString str2;
            str2.Printf("%d", id);
            m_idCtrl->SetValue(str2);
        }
        m_idCtrl->Enable(TRUE);
    }
}

void wxResourceSymbolDialog::OnSymbolNameUpdate(wxCommandEvent& WXUNUSED(event))
{
    wxString str(m_nameCtrl->GetValue());
    if (wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().IsStandardSymbol(str))
    {
        int id = wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().GetIdForSymbol(str);
        wxString str2;
        str2.Printf("%d", id);
        m_idCtrl->SetValue(str2);
        m_idCtrl->Enable(FALSE);
    }
    else
    {
        if (wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().SymbolExists(str))
        {
            int id = wxResourceManager::GetCurrentResourceManager()->GetSymbolTable().GetIdForSymbol(str);
            wxString str2;
            str2.Printf("%d", id);
            m_idCtrl->SetValue(str2);
        }
        m_idCtrl->Enable(TRUE);
    }
}

