/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_toolbar_n.mm
// Purpose:     wxMoToolBar class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/*

  How do we add normal-looking buttons to the toolbar? We could
  give any text-only button a bordered style, and take the regular button
  colouring into account.

 */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/native/toolbar.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/bitmap.h"
    #include "wx/dcmemory.h"
    #include "wx/control.h"
    #include "wx/image.h"
#endif

#include "wx/dcbuffer.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoToolBar, wxControl)

BEGIN_EVENT_TABLE(wxMoToolBar, wxToolBarBase)
    EVT_SIZE(wxMoToolBar::OnSize)
    EVT_PAINT(wxMoToolBar::OnPaint)
    EVT_MOUSE_EVENTS(wxMoToolBar::OnMouseEvent)
    EVT_SYS_COLOUR_CHANGED(wxMoToolBar::OnSysColourChanged)
    EVT_ERASE_BACKGROUND(wxMoToolBar::OnEraseBackground)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxMoToolBarTool : public wxToolBarToolBase
{
public:
    wxMoToolBarTool(wxMoToolBar *tbar,
                  int id,
                  const wxString& label,
                  const wxBitmap& bmpNormal,
                  const wxBitmap& bmpDisabled,
                  wxItemKind kind,
                  wxObject *clientData,
                  const wxString& shortHelp,
                  const wxString& longHelp)
        : wxToolBarToolBase(tbar, id, label, bmpNormal, bmpDisabled, kind,
                            clientData, shortHelp, longHelp)
    {
        m_width = -1;
    }

    wxMoToolBarTool(wxMoToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control, wxEmptyString)
    {
        m_width = -1;
    }

    virtual void SetLabel(const wxString& label)
    {
        if ( label == m_label )
            return;

        wxToolBarToolBase::SetLabel(label);
    }

    void SetWidth(int width) { m_width = width; }

    int m_width;
private:
    DECLARE_NO_COPY_CLASS(wxMoToolBarTool)
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMoToolBarTool
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxMoToolBar::CreateTool(int id,
                                         const wxString& label,
                                         const wxBitmap& bmpNormal,
                                         const wxBitmap& bmpDisabled,
                                         wxItemKind kind,
                                         wxObject *clientData,
                                         const wxString& shortHelp,
                                         const wxString& longHelp)
{
    return new wxMoToolBarTool(this, id, label, bmpNormal, bmpDisabled, kind,
                             clientData, shortHelp, longHelp);
}

wxToolBarToolBase *wxMoToolBar::CreateTool(wxControl *control)
{
    return new wxMoToolBarTool(this, control);
}

wxToolBarToolBase *wxMoToolBar::CreateTool(wxControl *control,
                                      const wxString& label)
{
    wxMoToolBarTool *toolbarTool = new wxMoToolBarTool(this, control);
    if (toolbarTool != NULL) {
        toolbarTool->SetLabel(label);
    }
    return toolbarTool;
}

// ----------------------------------------------------------------------------
// wxMoToolBar construction
// ----------------------------------------------------------------------------

void wxMoToolBar::Init()
{
    // FIXME stub
}

bool wxMoToolBar::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    // FIXME stub
    return true;
}

wxMoToolBar::~wxMoToolBar()
{
    // FIXME stub
}

wxSize wxMoToolBar::DoGetBestSize() const
{
    // FIXME stub

	wxSize empty(1, 1);
	return empty;
}

// ----------------------------------------------------------------------------
// adding/removing tools
// ----------------------------------------------------------------------------

// New function to add a flexible separator. AddSeparator is an alias
// for this.
wxToolBarToolBase *wxMoToolBar::AddFlexibleSeparator()
{
    return wxToolBarBase::AddSeparator();
}

// New function to add a flexible separator. InsertSeparator is an alias
// for this.
wxToolBarToolBase *wxMoToolBar::InsertFlexibleSeparator(size_t pos)
{
    return wxToolBarBase::InsertSeparator(pos);
}

// New function to add a fixed0width separator.
wxToolBarToolBase *wxMoToolBar::AddFixedSeparator(int width)
{
    // FIXME stub

	return NULL;
}

// New function to insert a fixed-width separator.
wxToolBarToolBase *wxMoToolBar::InsertFixedSeparator(size_t pos, int width)
{
    // FIXME stub

	return NULL;
}

bool wxMoToolBar::DoInsertTool(size_t WXUNUSED(pos), wxToolBarToolBase *tool)
{
    // FIXME stub

    return true;
}

bool wxMoToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *tool)
{
    // FIXME stub

    return true;
}

bool wxMoToolBar::Realize()
{
    // FIXME stub

    return true;
}

// ----------------------------------------------------------------------------
// toolbar geometry
// ----------------------------------------------------------------------------

void wxMoToolBar::SetToolBitmapSize(const wxSize& size)
{
    // FIXME stub
}

void wxMoToolBar::SetRows(int WXUNUSED(nRows))
{
}

// The button size is bigger than the bitmap size
wxSize wxMoToolBar::GetToolSize() const
{
	// FIXME stub
    // Not implemented
    return wxSize(0, 0);
}

wxToolBarToolBase *wxMoToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    // FIXME stub

    return NULL;
}

void wxMoToolBar::UpdateSize()
{
    // FIXME stub
}

// ----------------------------------------------------------------------------
// tool state
// ----------------------------------------------------------------------------

void wxMoToolBar::DoEnableTool(wxToolBarToolBase *tool, bool enable)
{
    // FIXME stub
}

void wxMoToolBar::DoToggleTool(wxToolBarToolBase *WXUNUSED(tool), bool WXUNUSED(toggle))
{
	// FIXME stub
    // Not implemented
}

void wxMoToolBar::DoSetToggle(wxToolBarToolBase *WXUNUSED(tool), bool WXUNUSED(toggle))
{
	// FIXME stub
    // VZ: AFAIK, the button has to be created either with TBSTYLE_CHECK or
    //     without, so we really need to delete the button and recreate it here
    wxFAIL_MSG( _T("not implemented") );
}

void wxMoToolBar::SetToolNormalBitmap( int id, const wxBitmap& bitmap )
{
    // FIXME stub
}

void wxMoToolBar::SetToolDisabledBitmap( int id, const wxBitmap& bitmap )
{
    // FIXME stub
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// Responds to colour changes, and passes event on to children.
void wxMoToolBar::OnSysColourChanged(wxSysColourChangedEvent& event)
{
	// FIXME stub
}

void wxMoToolBar::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}

void wxMoToolBar::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxMoToolBar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// FIXME stub
}

void wxMoToolBar::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoToolBar::SetBorderColour(const wxColour &colour)
{
    // FIXME stub
}

wxColour wxMoToolBar::GetBorderColour() const
{
    // FIXME stub

	wxColour empty;
	return empty;
}

bool wxMoToolBar::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

wxColour wxMoToolBar::GetBackgroundColour() const
{
    // FIXME stub

	wxColour empty;
	return empty;
}

bool wxMoToolBar::SetButtonBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

wxColour wxMoToolBar::GetButtonBackgroundColour() const
{
    // FIXME stub

	wxColour empty;
	return empty;
}

bool wxMoToolBar::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

wxColour wxMoToolBar::GetForegroundColour() const
{
    // FIXME stub

	wxColour empty;
	return empty;
}

bool wxMoToolBar::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

wxFont wxMoToolBar::GetFont() const
{
    // FIXME stub

	wxFont empty;
	return empty;
}
