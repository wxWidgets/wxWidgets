/////////////////////////////////////////////////////////////////////////////
// Name:        resource.h
// Purpose:     Resource processing
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RESOURCEH__
#define _WX_RESOURCEH__

#ifdef __GNUG__
#pragma interface "resource.h"
#endif

#include "wx/setup.h"

#if USE_WX_RESOURCES
#include <stdio.h>

// A few further types not in wx_types.h
#define wxRESOURCE_TYPE_SEPARATOR   1000
#define wxRESOURCE_TYPE_XBM_DATA    1001
#define wxRESOURCE_TYPE_XPM_DATA    1002

#define RESOURCE_PLATFORM_WINDOWS   1
#define RESOURCE_PLATFORM_X         2
#define RESOURCE_PLATFORM_MAC       3
#define RESOURCE_PLATFORM_ANY       4

/*
 * Internal format for control/panel item
 */
 
class WXDLLEXPORT wxItemResource: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxItemResource)

 protected:
  wxList children;
  char *itemType;
  int x, y, width, height;
  char *title;
  char *name;
  long windowStyle;
  long value1, value2, value3, value5;
  char *value4;
  int   m_windowId;
  wxStringList *stringValues; // Optional string values
  wxBitmap *bitmap;
  wxColour *backgroundColour;
  wxColour *labelColour;
  wxColour *buttonColour;
  wxFont *windowFont;
 public:
 
  wxItemResource(void);
  ~wxItemResource(void);

  void SetType(char *typ);
  inline void SetStyle(long styl) { windowStyle = styl; }
  inline void SetId(int id) { m_windowId = id; }
  inline void SetBitmap(wxBitmap *bm) { bitmap = bm; }
  inline wxBitmap *GetBitmap(void) { return bitmap; }
  inline void SetFont(wxFont *font) { windowFont = font; }
  inline wxFont *GetFont(void) { return windowFont; }
  inline void SetSize(int xx, int yy, int ww, int hh)
  {  x = xx; y = yy; width = ww; height = hh; }
  void SetTitle(char *t);
  void SetName(char *n);
  inline void SetValue1(long v) { value1 = v; }
  inline void SetValue2(long v) { value2 = v; }
  inline void SetValue3(long v) { value3 = v; }
  inline void SetValue5(long v) { value5 = v; }
  void SetValue4(char *v);
  void SetStringValues(wxStringList *svalues);

  inline char *GetType(void) { return itemType; }
  inline int GetX(void) { return x; }
  inline int GetY(void) { return y; }
  inline int GetWidth(void) { return width; }
  inline int GetHeight(void) { return height; }

  inline char *GetTitle(void) { return title; }
  inline char *GetName(void) { return name; }
  inline long GetStyle(void) { return windowStyle; }
  inline int GetId(void) { return m_windowId; }

  inline long GetValue1(void) { return value1; }
  inline long GetValue2(void) { return value2; }
  inline long GetValue3(void) { return value3; }
  inline long GetValue5(void) { return value5; }
  inline char *GetValue4(void) { return value4; }
  inline wxList& GetChildren(void) { return children; }
  inline wxStringList *GetStringValues(void) { return stringValues; }
  
  inline void SetBackgroundColour(wxColour *col) { if (backgroundColour) delete backgroundColour; backgroundColour = col; }
  inline void SetLabelColour(wxColour *col) { if (labelColour) delete labelColour; labelColour = col; }
  inline void SetButtonColour(wxColour *col) { if (buttonColour) delete buttonColour; buttonColour = col; }
  
  inline wxColour *GetBackgroundColour(void) { return backgroundColour; }
  inline wxColour *GetLabelColour(void) { return labelColour; }
  inline wxColour *GetButtonColour(void) { return buttonColour; }
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
    
    wxResourceTable(void);
    ~wxResourceTable(void);
    
    virtual wxItemResource *FindResource(const wxString& name) const;
    virtual void AddResource(wxItemResource *item);
    virtual bool DeleteResource(const wxString& name);

    virtual bool ParseResourceFile(char *filename);
    virtual bool ParseResourceData(char *data);
    virtual bool SaveResource(char *filename);

    // Register XBM/XPM data
    virtual bool RegisterResourceBitmapData(char *name, char bits[], int width, int height);
    virtual bool RegisterResourceBitmapData(char *name, char **data);

    virtual wxControl *CreateItem(wxWindow *panel, wxItemResource *childResource) const;

    virtual void ClearTable(void);
};

extern void WXDLLEXPORT wxInitializeResourceSystem(void);
extern void WXDLLEXPORT wxCleanUpResourceSystem(void);

WXDLLEXPORT_DATA(extern wxResourceTable*) wxDefaultResourceTable;
extern long WXDLLEXPORT wxParseWindowStyle(char *style);

class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxIcon;
extern wxBitmap* WXDLLEXPORT wxResourceCreateBitmap(char *resource, wxResourceTable *table = (wxResourceTable *) NULL);
extern wxIcon* WXDLLEXPORT wxResourceCreateIcon(char *resource, wxResourceTable *table = (wxResourceTable *) NULL);
extern wxMenuBar* WXDLLEXPORT wxResourceCreateMenuBar(char *resource, wxResourceTable *table = (wxResourceTable *) NULL, wxMenuBar *menuBar = (wxMenuBar *) NULL);
extern wxMenu* WXDLLEXPORT wxResourceCreateMenu(char *resource, wxResourceTable *table = (wxResourceTable *) NULL);
extern bool WXDLLEXPORT wxResourceParseData(char *resource, wxResourceTable *table = (wxResourceTable *) NULL);
extern bool WXDLLEXPORT wxResourceParseFile(char *filename, wxResourceTable *table = (wxResourceTable *) NULL);
extern bool WXDLLEXPORT wxResourceParseString(char *s, wxResourceTable *table = (wxResourceTable *) NULL);
extern void WXDLLEXPORT wxResourceClear(wxResourceTable *table = (wxResourceTable *) NULL);
// Register XBM/XPM data
extern bool WXDLLEXPORT wxResourceRegisterBitmapData(char *name, char bits[], int width, int height, wxResourceTable *table = (wxResourceTable *) NULL);
extern bool WXDLLEXPORT wxResourceRegisterBitmapData(char *name, char **data, wxResourceTable *table = (wxResourceTable *) NULL);
#define wxResourceRegisterIconData wxResourceRegisterBitmapData

/*
 * Resource identifer code: #define storage
 */

extern bool WXDLLEXPORT wxResourceAddIdentifier(char *name, int value, wxResourceTable *table = (wxResourceTable *) NULL);
extern int WXDLLEXPORT wxResourceGetIdentifier(char *name, wxResourceTable *table = (wxResourceTable *) NULL);

#endif
#endif
    // _WX_RESOURCEH__
