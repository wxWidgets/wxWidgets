///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/toolbar.h
// Purpose:     wxToolBar declaration
// Author:      Robert Roebling
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_TOOLBAR_H_
#define _WX_UNIV_TOOLBAR_H_

#ifdef __GNUG__
    #pragma interface "univtoolbar.h"
#endif

#include "wx/window.h"

// ----------------------------------------------------------------------------
// wxToolbar
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool( wxToolBarBase *tbar = (wxToolBarBase *)NULL,
                   int id = wxID_SEPARATOR,
                   const wxBitmap& bitmap1 = wxNullBitmap,
                   const wxBitmap& bitmap2 = wxNullBitmap,
                   bool toggle = FALSE,
                   wxObject *clientData = (wxObject *) NULL,
                   const wxString& shortHelpString = wxEmptyString,
                   const wxString& longHelpString = wxEmptyString ) :
        wxToolBarToolBase( tbar, id, bitmap1, bitmap2, toggle, clientData,
                           shortHelpString, longHelpString )
    {
        m_isDown = FALSE;
        m_x = -1;
        m_y = -1;
    }

public:
    bool        m_isDown;
    int         m_x;
    int         m_y;
};

class WXDLLEXPORT wxToolBar: public wxToolBarBase
{    
public:
    // construction/destruction
    wxToolBar() { Init(); }
    wxToolBar( wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxToolBarNameStr )
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxToolBarNameStr );
                 
#ifdef __DARWIN__
    virtual ~wxToolBar() { }
#endif

    virtual bool Realize();

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual void SetToolShortHelp(int id, const wxString& helpString);

protected:
    // common part of all ctors
    void Init();

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxBitmap& bitmap1,
                                          const wxBitmap& bitmap2,
                                          bool toggle,
                                          wxObject *clientData,
                                          const wxString& shortHelpString,
                                          const wxString& longHelpString);
    virtual wxToolBarToolBase *CreateTool(wxControl *control);
    
private:
    wxToolBarTool    *m_captured;
    
private:
    void OnMouse( wxMouseEvent &event );
    void RefreshTool( wxToolBarTool *tool );
    void DrawToolBarTool( wxToolBarTool *tool, wxDC &dc, bool down );
    void OnPaint( wxPaintEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

#endif // _WX_UNIV_TOOLBAR_H_
