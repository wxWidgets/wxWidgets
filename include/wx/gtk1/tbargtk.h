/////////////////////////////////////////////////////////////////////////////
// Name:        tbargtk.h
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __TBARGTKH__
#define __TBARGTKH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxToolBarTool;
class wxToolBar;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define wxTOOL_STYLE_BUTTON          1
#define wxTOOL_STYLE_SEPARATOR       2

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxToolBarNameStr;

//-----------------------------------------------------------------------------
// wxToolBarTool
//-----------------------------------------------------------------------------

class wxToolBarTool: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxToolBarTool)
  
  public:

    wxToolBarTool(void) {}; 
    wxToolBarTool( wxToolBar *owner, int theIndex = 0, 
      const wxBitmap& bitmap1 = wxNullBitmap, const wxBitmap& bitmap2 = wxNullBitmap,
      bool toggle = FALSE, wxObject *clientData = (wxObject *) NULL, 
      const wxString& shortHelpString = "", const wxString& longHelpString = "",
      GtkWidget *item = (GtkWidget *) NULL );
   ~wxToolBarTool(void);

  public:
  
    int                   m_toolStyle;
    wxObject             *m_clientData;
    int                   m_index;
    bool                  m_toggleState;
    bool                  m_isToggle;
    bool                  m_deleteSecondBitmap;
    bool                  m_enabled;
    wxBitmap              m_bitmap1;
    wxBitmap              m_bitmap2;
    bool                  m_isMenuCommand;
    wxString              m_shortHelpString;
    wxString              m_longHelpString;
    wxToolBar            *m_owner;
    GtkWidget            *m_item;
};

//-----------------------------------------------------------------------------
// wxToolBar
//-----------------------------------------------------------------------------

class wxToolBar: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxToolBar)
  
  public:

    wxToolBar(void);
    wxToolBar( wxWindow *parent, wxWindowID id, 
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = 0, const wxString& name = wxToolBarNameStr );
   ~wxToolBar(void);

   bool Create( wxWindow *parent, wxWindowID id, 
     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
     long style = 0, const wxString& name = wxToolBarNameStr);

    // Only allow toggle if returns TRUE. Call when left button up.
    virtual bool OnLeftClick(int toolIndex, bool toggleDown);

    // Call when right button down.
    virtual void OnRightClick(int toolIndex, float x, float y);

    // Called when the mouse cursor enters a tool bitmap.
    // Argument is -1 if mouse is exiting the toolbar.
    virtual void OnMouseEnter(int toolIndex);

    // If pushedBitmap is NULL, a reversed version of bitmap is
    // created and used as the pushed/toggled image.
    // If toggle is TRUE, the button toggles between the two states.
    virtual wxToolBarTool *AddTool( int toolIndex, const wxBitmap& bitmap, 
      const wxBitmap& pushedBitmap = wxNullBitmap, bool toggle = FALSE,
      float xPos = -1, float yPos = -1, wxObject *clientData = (wxObject *)NULL,
      const wxString& helpString1 = "", const wxString& helpString2 = "");
    virtual void AddSeparator(void);
    virtual void ClearTools(void);
    
    virtual bool Realize(void);

    virtual void EnableTool(int toolIndex, bool enable);
    virtual void ToggleTool(int toolIndex, bool toggle); // toggle is TRUE if toggled on
    virtual wxObject *GetToolClientData(int index) const;

    virtual bool GetToolState(int toolIndex) const;
    virtual bool GetToolEnabled(int toolIndex) const;

    virtual void SetMargins(int x, int y);
    void SetMargins(const wxSize& size) { SetMargins(size.x, size.y); };
    virtual wxSize GetToolMargins(void) { return wxSize(m_xMargin, m_yMargin); }
    
    virtual void SetToolPacking(int packing);
    virtual void SetToolSeparation(int separation);
    virtual int GetToolPacking();
    virtual int GetToolSeparation();
    
    virtual wxString GetToolLongHelp(int toolIndex);
    virtual wxString GetToolShortHelp(int toolIndex);
    
    virtual void SetToolLongHelp(int toolIndex, const wxString& helpString);
    virtual void SetToolShortHelp(int toolIndex, const wxString& helpString);

  // implementation
  
    GtkToolbar   *m_toolbar;
    int           m_separation;
    wxList        m_tools;
    
    GdkColor      *m_fg;
    GdkColor      *m_bg;
    int           m_xMargin;
    int           m_yMargin;
    bool          m_hasToolAlready;
};

#endif
    // __TBARGTKH__

