/////////////////////////////////////////////////////////////////////////////
// Name:        resource.h
// Purpose:     Resource processing
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RESOURCEH__
#define _WX_RESOURCEH__

#ifdef __GNUG__
#pragma interface "resource.h"
#endif

#include "wx/setup.h"

#if wxUSE_WX_RESOURCES
#include <stdio.h>

// A few further types not in wx_types.h
#define wxRESOURCE_TYPE_SEPARATOR   1000
#define wxRESOURCE_TYPE_XBM_DATA    1001
#define wxRESOURCE_TYPE_XPM_DATA    1002

#define RESOURCE_PLATFORM_WINDOWS   1
#define RESOURCE_PLATFORM_X         2
#define RESOURCE_PLATFORM_MAC       3
#define RESOURCE_PLATFORM_ANY       4

// Extended styles: for resource usage only

// Use dialog units instead of pixels
#define wxRESOURCE_DIALOG_UNITS 1
// Use default system colour and font
#define wxRESOURCE_USE_DEFAULTS 2

// Macros to help use dialog units
#define wxDLG_POINT(x, y, parent, useDlgUnits) (useDlgUnits ? parent->ConvertDialogToPixel(wxPoint(x, y)) : wxPoint(x, y))
#define wxDLG_SIZE(x, y, parent, useDlgUnits) (useDlgUnits ? parent->ConvertDialogToPixel(wxSize(x, y)) : wxSize(x, y))

#ifdef FindResource
#undef FindResource
#endif

/*
 * Internal format for control/panel item
 */

class WXDLLEXPORT wxItemResource: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxItemResource)

 public:

  wxItemResource();
  ~wxItemResource();

  inline void SetType(const wxString& type) { m_itemType = type; }
  inline void SetStyle(long styl) { m_windowStyle = styl; }
  inline void SetId(int id) { m_windowId = id; }
  inline void SetBitmap(const wxBitmap& bm) { m_bitmap = bm; }
  inline wxBitmap& GetBitmap() const { return (wxBitmap&) m_bitmap; }
  inline void SetFont(const wxFont& font) { m_windowFont = font; }
  inline wxFont& GetFont() const { return (wxFont&) m_windowFont; }
  inline void SetSize(int xx, int yy, int ww, int hh)
  {  m_x = xx; m_y = yy; m_width = ww; m_height = hh; }
  inline void SetTitle(const wxString& title) { m_title = title; }
  inline void SetName(const wxString& name) { m_name = name; }
  inline void SetValue1(long v) { m_value1 = v; }
  inline void SetValue2(long v) { m_value2 = v; }
  inline void SetValue3(long v) { m_value3 = v; }
  inline void SetValue5(long v) { m_value5 = v; }
  inline void SetValue4(const wxString& v) { m_value4 = v; }
  inline void SetStringValues(const wxStringList& svalues) { m_stringValues = svalues; }

  inline const wxString& GetType() const { return m_itemType; }
  inline int GetX() const { return m_x; }
  inline int GetY() const { return m_y; }
  inline int GetWidth() const { return m_width; }
  inline int GetHeight() const { return m_height; }

  inline const wxString& GetTitle() const { return m_title; }
  inline const wxString& GetName() const { return m_name; }
  inline long GetStyle() const { return m_windowStyle; }
  inline int GetId() const { return m_windowId; }

  inline wxInt32 GetValue1() const { return m_value1; }
  inline wxInt32 GetValue2() const { return m_value2; }
  inline wxInt32 GetValue3() const { return m_value3; }
  inline wxInt32 GetValue5() const { return m_value5; }
  inline wxString GetValue4() const { return m_value4; }
  inline wxList& GetChildren() const { return (wxList&) m_children; }
  inline wxStringList& GetStringValues() const { return (wxStringList&) m_stringValues; }

  inline void SetBackgroundColour(const wxColour& col) { m_backgroundColour = col; }
  inline void SetLabelColour(const wxColour& col) { m_labelColour = col; }
  inline void SetButtonColour(const wxColour& col) { m_buttonColour = col; }
  
  inline wxColour& GetBackgroundColour() const { return (wxColour&) m_backgroundColour; }
  inline wxColour& GetLabelColour() const { return (wxColour&) m_labelColour; }
  inline wxColour& GetButtonColour() const { return (wxColour&) m_buttonColour; }

  inline void SetResourceStyle(long style) { m_exStyle = style; }
  inline wxInt32 GetResourceStyle() const { return m_exStyle; }

 protected:
  wxList        m_children;
  wxString      m_itemType;
  int           m_x, m_y, m_width, m_height;
  wxString      m_title;
  wxString      m_name;
  long          m_windowStyle;
  long          m_value1, m_value2, m_value3, m_value5;
  wxString      m_value4;
  int           m_windowId;
  wxStringList  m_stringValues; // Optional string values
  wxBitmap      m_bitmap;
  wxColour      m_backgroundColour;
  wxColour      m_labelColour;
  wxColour      m_buttonColour;
  wxFont        m_windowFont;
  long          m_exStyle; // Extended, resource-specific styles
};

/*
 * Resource table (normally only one of these)
 */
 
class WXDLLEXPORT wxResourceTable: public wxHashTable
{
  DECLARE_DYNAMIC_CLASS(wxResourceTable)

  protected:
    
  public:
    wxHashTable identifiers;
    
    wxResourceTable();
    ~wxResourceTable();

    virtual wxItemResource *FindResource(const wxString& name) const;
    virtual void AddResource(wxItemResource *item);
    virtual bool DeleteResource(const wxString& name);

    virtual bool ParseResourceFile(const wxString& filename);
    virtual bool ParseResourceData(const wxString& data);
    virtual bool SaveResource(const wxString& filename);

    // Register XBM/XPM data
    virtual bool RegisterResourceBitmapData(const wxString& name, char bits[], int width, int height);
    virtual bool RegisterResourceBitmapData(const wxString& name, char **data);

    virtual wxControl *CreateItem(wxWindow *panel, const wxItemResource* childResource, const wxItemResource* parentResource) const;

    virtual void ClearTable();
};

WXDLLEXPORT extern void wxInitializeResourceSystem();
WXDLLEXPORT extern void wxCleanUpResourceSystem();

WXDLLEXPORT_DATA(extern wxResourceTable*) wxDefaultResourceTable;
WXDLLEXPORT extern long wxParseWindowStyle(const wxString& style);

class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxIcon;
WXDLLEXPORT extern wxBitmap wxResourceCreateBitmap(const wxString& resource, wxResourceTable *table = (wxResourceTable *) NULL);
WXDLLEXPORT extern wxIcon wxResourceCreateIcon(const wxString& resource, wxResourceTable *table = (wxResourceTable *) NULL);
WXDLLEXPORT extern wxMenuBar* wxResourceCreateMenuBar(const wxString& resource, wxResourceTable *table = (wxResourceTable *) NULL, wxMenuBar *menuBar = (wxMenuBar *) NULL);
WXDLLEXPORT extern wxMenu* wxResourceCreateMenu(const wxString& resource, wxResourceTable *table = (wxResourceTable *) NULL);
WXDLLEXPORT extern bool wxResourceParseData(const wxString& resource, wxResourceTable *table = (wxResourceTable *) NULL);
WXDLLEXPORT extern bool wxResourceParseFile(const wxString& filename, wxResourceTable *table = (wxResourceTable *) NULL);
WXDLLEXPORT extern bool wxResourceParseString(char* s, wxResourceTable *table = (wxResourceTable *) NULL);
WXDLLEXPORT extern void wxResourceClear(wxResourceTable *table = (wxResourceTable *) NULL);
// Register XBM/XPM data
WXDLLEXPORT extern bool wxResourceRegisterBitmapData(const wxString& name, char bits[], int width, int height, wxResourceTable *table = (wxResourceTable *) NULL);
WXDLLEXPORT extern bool wxResourceRegisterBitmapData(const wxString& name, char **data, wxResourceTable *table = (wxResourceTable *) NULL);
#define wxResourceRegisterIconData wxResourceRegisterBitmapData

/*
 * Resource identifer code: #define storage
 */

WXDLLEXPORT extern bool wxResourceAddIdentifier(const wxString& name, int value, wxResourceTable *table = (wxResourceTable *) NULL);
WXDLLEXPORT extern int wxResourceGetIdentifier(const wxString& name, wxResourceTable *table = (wxResourceTable *) NULL);

#endif
#endif
    // _WX_RESOURCEH__
