/////////////////////////////////////////////////////////////////////////////
// Name:        tbargtk.h
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TBARGTK_H_
#define _WX_GTK_TBARGTK_H_

#ifdef __GNUG__
    #pragma interface "tbargtk.h"
#endif

#include "wx/defs.h"

#if wxUSE_TOOLBAR

#include "wx/control.h"
#include "wx/bitmap.h"
#include "wx/tbarbase.h"

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBar            *m_owner;
    GtkWidget            *m_item;
    GtkWidget            *m_pixmap;
};

// ----------------------------------------------------------------------------
// wxToolBar
// ----------------------------------------------------------------------------

class wxToolBar : public wxToolBarBase
{
public:
    // construction/destruction
    wxToolBar();
    wxToolBar( wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxToolBarNameStr );
    virtual ~wxToolBar();

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxToolBarNameStr);

    // implement base class virtuals
    virtual wxToolBarTool *AddTool(int id,
                                   const wxBitmap& bitmap,
                                   const wxBitmap& pushedBitmap = wxNullBitmap,
                                   bool toggle = FALSE,
                                   wxCoord xPos = -1, wxCoord yPos = -1,
                                   wxObject *clientData = (wxObject *)NULL,
                                   const wxString& helpString1 = wxEmptyString,
                                   const wxString& helpString2 = wxEmptyString);

    virtual wxToolBarTool *InsertTool(size_t pos,
                                      int id,
                                      const wxBitmap& bitmap,
                                      const wxBitmap& pushedBitmap = wxNullBitmap,
                                      bool toggle = FALSE,
                                      wxCoord xPos = -1, wxCoord yPos = -1,
                                      wxObject *clientData = (wxObject *)NULL,
                                      const wxString& helpString1 = wxEmptyString,
                                      const wxString& helpString2 = wxEmptyString);

    virtual bool AddControl(wxControl *control);
    virtual bool InsertControl(size_t pos, wxControl *control);

    virtual void AddSeparator();
    virtual void InsertSeparator(size_t pos);

    virtual bool DeleteToolByPos(size_t pos);
    virtual bool DeleteTool(int id);

    virtual void ClearTools();

    // Has to be called after adding tools or changing something
    virtual bool Realize();

    virtual void EnableTool(int id, bool enable);
    virtual void ToggleTool(int id, bool toggle);
    virtual wxObject *GetToolClientData(int index) const;

    virtual bool GetToolState(int id) const;
    virtual bool GetToolEnabled(int id) const;

    virtual void SetMargins(int x, int y);
    void SetMargins(const wxSize& size) { SetMargins(size.x, size.y); };
    virtual wxSize GetToolMargins(void) { return wxSize(m_xMargin, m_yMargin); }

    virtual void SetToolPacking(int packing);
    virtual void SetToolSeparation(int separation);
    virtual int GetToolPacking();
    virtual int GetToolSeparation();

    virtual wxString GetToolLongHelp(int id);
    virtual wxString GetToolShortHelp(int id);

    virtual void SetToolLongHelp(int id, const wxString& helpString);
    virtual void SetToolShortHelp(int id, const wxString& helpString);

    void OnIdle( wxIdleEvent &ievent );

    // Only allow toggle if returns TRUE. Call when left button up.
    virtual bool OnLeftClick(int id, bool toggleDown);

    // Call when right button down.
    virtual void OnRightClick(int id, float x, float y);

    // Called when the mouse cursor enters a tool bitmap.
    // Argument is -1 if mouse is exiting the toolbar.
    virtual void OnMouseEnter(int id);

    // implementation from now on
    // --------------------------

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
    // _WX_GTK_TBARGTK_H_
