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

#if wxUSE_TOOLBAR

#include "wx/control.h"
#include "wx/bitmap.h"
#include "wx/tbarbase.h"

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

extern const wxChar *wxToolBarNameStr;

//-----------------------------------------------------------------------------
// wxToolBar
//-----------------------------------------------------------------------------

class wxToolBar: public wxControl
{
public:
    wxToolBar();
    wxToolBar( wxWindow *parent, wxWindowID id, 
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = 0, const wxString& name = wxToolBarNameStr );
   ~wxToolBar();

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
      wxCoord xPos = -1, wxCoord yPos = -1, wxObject *clientData = (wxObject *)NULL,
      const wxString& helpString1 = "", const wxString& helpString2 = "");
    
    // Add arbitrary control
    virtual bool AddControl(wxControl *control);
      
    // Add space
    virtual void AddSeparator();
    
    virtual void ClearTools();
    
    virtual bool Realize();

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
    
    void OnIdle( wxIdleEvent &ievent );

  // implementation
  
    GtkToolbar   *m_toolbar;
    int           m_separation;
    wxList        m_tools;
    
    GdkColor      *m_fg;
    GdkColor      *m_bg;
    int           m_xMargin;
    int           m_yMargin;
    bool          m_hasToolAlready;
    bool          m_blockNextEvent;
    
    void OnInternalIdle();
    
private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

#endif

#endif
    // __TBARGTKH__
