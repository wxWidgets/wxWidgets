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

#if wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxBitmap& bitmap1,
                  const wxBitmap& bitmap2,
                  bool toggle,
                  wxObject *clientData,
                  const wxString& shortHelpString,
                  const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, bitmap1, bitmap2, toggle,
                            clientData, shortHelpString, longHelpString)
    {
        Init();
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
        Init();
    }

    GtkWidget            *m_item;
    GtkWidget            *m_pixmap;

protected:
    void Init();
};

// ----------------------------------------------------------------------------
// wxToolBar
// ----------------------------------------------------------------------------

class wxToolBar : public wxToolBarBase
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
                 const wxString& name = wxToolBarNameStr);

    virtual ~wxToolBar();

    // override base class virtuals
    virtual void SetMargins(int x, int y);
    virtual void SetToolSeparation(int separation);

    virtual wxToolBarTool *FindToolForPosition(wxCoord x, wxCoord y) const;

    // implementation from now on
    // --------------------------

    GtkToolbar   *m_toolbar;

    GdkColor     *m_fg;
    GdkColor     *m_bg;

    bool          m_blockNextEvent;

    void OnInternalIdle();

protected:
    // common part of all ctors
    void Init();

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarTool *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarTool *tool);

    virtual void DoEnableTool(wxToolBarTool *tool, bool enable);
    virtual void DoToggleTool(wxToolBarTool *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarTool *tool, bool toggle);

private:
    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_GTK_TBARGTK_H_
