/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_toolbar_g.cpp
// Purpose:     wxMoToolBar class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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

#include "wx/mobile/generic/toolbar.h"

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
    m_buttonStrip.SetControlType(wxMoButtonStrip::ToolBar);
    m_buttonStrip.SetLayoutStyle(wxMoButtonStrip::LayoutStretchingSeparators );
    m_buttonStrip.SetSelectionStyle(wxMoButtonStrip::NoSelection);    
}

bool wxMoToolBar::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    style &= ~wxBORDER_MASK;
    style |= wxNO_BORDER;

    wxSize size2(size);
    if (size2.y == -1)
        size2.y = 44;

    // common initialisation
    if ( !wxToolBarBase::Create(parent, id, pos, size2, style, wxDefaultValidator, name) )
        return false;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    // Currently not necessary - buttons draw whatever they have, text or bitmap.
    // However we may want to make this more explicit in future so we can provide
    // both text and images, but choose which to display. This flexibility is not
    // so necessary on mobile devices, where there is not so much room for customisation.
#if 0
    if ((style & wxTB_TEXT) && (style & wxTB_NOICONS) == 0)
        m_buttonStrip.SetFaceType(wxMoButtonStrip::TextAndBitmapButton);
    else if (style & wxTB_TEXT) 
        m_buttonStrip.SetFaceType(wxMoButtonStrip::TextButton);
    else
        m_buttonStrip.SetFaceType(wxMoButtonStrip::BitmapButton);
#endif

    SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_TOOLBAR));

    if (style & wxTB_BLACK_OPAQUE_BG)
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_BLACK_OPAQUE_BG));
        SetButtonBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_BLACK_BUTTON_BG));
        m_buttonStrip.SetBorderColour(wxMO_COLOUR_TOOLBAR_BUTTON_BORDER);
    }
    else if (style & wxTB_BLACK_TRANSLUCENT_BG)
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_BLACK_TRANSLUCENT_BG));
        SetButtonBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_BLACK_BUTTON_BG));
        m_buttonStrip.SetBorderColour(wxMO_COLOUR_TOOLBAR_BUTTON_BORDER);
    }
    else
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_NORMAL_BG));
        SetButtonBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_NORMAL_BUTTON_BG));
        m_buttonStrip.SetBorderColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TOOLBAR_NORMAL_BUTTON_BG));
    }

    m_buttonStrip.SetButtonMarginY(4);

    SetInitialSize(size2);
    return true;
}

wxMoToolBar::~wxMoToolBar()
{
    // we must refresh the frame size when the toolbar is deleted but the frame
    // is not - otherwise toolbar leaves a hole in the place it used to occupy
    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);
    if ( frame && !frame->IsBeingDeleted() )
        frame->SendSizeEvent();
}

wxSize wxMoToolBar::DoGetBestSize() const
{
    // Note that a toolbar is typically 44 pixels high

    wxSize sz = m_buttonStrip.GetBestSize();
    CacheBestSize(sz);
    return sz;
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
    wxMoToolBarTool* tool = (wxMoToolBarTool*) wxMoToolBar::CreateTool(wxID_FIXEDSPACE,
                                         wxEmptyString,
                                         wxNullBitmap,
                                         wxNullBitmap,
                                         wxITEM_SEPARATOR,
                                         NULL,
                                         wxEmptyString,
                                         wxEmptyString);
    tool->SetWidth(width);
    AddTool(tool);
    return tool;
}

// New function to insert a fixed-width separator.
wxToolBarToolBase *wxMoToolBar::InsertFixedSeparator(size_t pos, int width)
{
    wxMoToolBarTool* tool = (wxMoToolBarTool*) wxMoToolBar::CreateTool(wxID_FIXEDSPACE,
                                         wxEmptyString,
                                         wxNullBitmap,
                                         wxNullBitmap,
                                         wxITEM_SEPARATOR,
                                         NULL,
                                         wxEmptyString,
                                         wxEmptyString);
    tool->SetWidth(width);
    InsertTool(pos, tool);
    return tool;
}

bool wxMoToolBar::DoInsertTool(size_t WXUNUSED(pos), wxToolBarToolBase *tool)
{
    // nothing special to do here - we really create the toolbar buttons in
    // Realize() later
    tool->Attach(this);

    InvalidateBestSize();
    return true;
}

bool wxMoToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *tool)
{
    // Controls not supported
#if 0
    if ( tool->IsControl() )
    {
        tool->GetControl()->Destroy();
    }
#endif

    int idx = m_buttonStrip.FindIndexForId(tool->GetId());
    if (idx != -1)
        m_buttonStrip.DeleteButton(idx);

    tool->Detach();

    InvalidateBestSize();

    return true;
}

bool wxMoToolBar::Realize()
{
    // Add buttons

    m_buttonStrip.Clear();

    for (wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst(); node; node = node->GetNext())
    {
        wxMoToolBarTool *tool = wxDynamicCast(node->GetData(), wxMoToolBarTool);
        if (tool)
        {
            if (tool->IsSeparator())
            {
                if (tool->m_width == -1)
                    m_buttonStrip.AddFlexibleSeparator();
                else
                    m_buttonStrip.AddFixedSeparator(tool->m_width);
            }
            else
            {
                int idx = m_buttonStrip.AddBitmapButton(this, tool->GetId(), tool->GetNormalBitmap(), tool->GetLabel());
                wxMoBarButton* button = m_buttonStrip.GetButton(idx);
                button->SetFont(GetFont());

                // Make a text button look like an ordinary button
                if (!button->GetLabel().IsEmpty() && !button->GetNormalBitmap().Ok())
                {
                    button->SetMarginX(16);
                    button->SetStyle(wxBBU_BORDERED);
                    wxSize bestSize = button->GetBestSize();
                    bestSize.y = 30;
                    button->SetSize(bestSize);
                }
                else
                    button->SetStyle(wxBBU_PLAIN);

                if (button->GetId() == wxID_DONE)
                {
                    button->SetStyle(button->GetStyle()|wxBBU_DONE);
                    button->SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_DONE_BUTTON_BG));
                    button->SetSelectionColour(button->GetBackgroundColour());
                }
                else
                {
                    button->SetBackgroundColour(m_buttonStrip.GetButtonBackgroundColour());
                    button->SetSelectionColour(m_buttonStrip.GetButtonSelectionColour());
                }
                button->SetBorderColour(m_buttonStrip.GetBorderColour());
            }
        }        
    }
    
    m_buttonStrip.ComputeSizes(this);
    InvalidateBestSize();
    UpdateSize();

    return true;
}

// ----------------------------------------------------------------------------
// toolbar geometry
// ----------------------------------------------------------------------------

void wxMoToolBar::SetToolBitmapSize(const wxSize& size)
{
    wxToolBarBase::SetToolBitmapSize(size);
}

void wxMoToolBar::SetRows(int WXUNUSED(nRows))
{
}

// The button size is bigger than the bitmap size
wxSize wxMoToolBar::GetToolSize() const
{
    // Not implemented
    return wxSize(0, 0);
}

wxToolBarToolBase *wxMoToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    int idx = m_buttonStrip.HitTest(wxPoint(x, y));
    if (idx != -1)
    {
        wxMoBarButton* item = m_buttonStrip.GetButton(idx);
        if (item)
            return FindById(item->GetId());
    }
    return NULL;
}

void wxMoToolBar::UpdateSize()
{
    wxPoint pos = GetPosition();
    if (pos != GetPosition())
        Move(pos);

    // In case Realize is called after the initial display (IOW the programmer
    // may have rebuilt the toolbar) give the frame the option of resizing the
    // toolbar to full width again, but only if the parent is a frame and the
    // toolbar is managed by the frame.  Otherwise assume that some other
    // layout mechanism is controlling the toolbar size and leave it alone.
    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);
    if (frame)
    {
        frame->SendSizeEvent();
    }
}

// ----------------------------------------------------------------------------
// tool state
// ----------------------------------------------------------------------------

void wxMoToolBar::DoEnableTool(wxToolBarToolBase *tool, bool enable)
{
    int idx = m_buttonStrip.FindIndexForId(tool->GetId());
    if (idx != -1)
        m_buttonStrip.EnableButton(idx, enable);
}

void wxMoToolBar::DoToggleTool(wxToolBarToolBase *WXUNUSED(tool), bool WXUNUSED(toggle))
{
    // Not implemented
}

void wxMoToolBar::DoSetToggle(wxToolBarToolBase *WXUNUSED(tool), bool WXUNUSED(toggle))
{
    // VZ: AFAIK, the button has to be created either with TBSTYLE_CHECK or
    //     without, so we really need to delete the button and recreate it here
    wxFAIL_MSG( _T("not implemented") );
}

void wxMoToolBar::SetToolNormalBitmap( int id, const wxBitmap& bitmap )
{
    wxMoToolBarTool* tool = wx_static_cast(wxMoToolBarTool*, FindById(id));
    if ( tool )
    {
        wxCHECK_RET( tool->IsButton(), wxT("Can only set bitmap on button tools."));

        tool->SetNormalBitmap(bitmap);
        Realize();
    }
}

void wxMoToolBar::SetToolDisabledBitmap( int id, const wxBitmap& bitmap )
{
    wxMoToolBarTool* tool = wx_static_cast(wxMoToolBarTool*, FindById(id));
    if ( tool )
    {
        wxCHECK_RET( tool->IsButton(), wxT("Can only set bitmap on button tools."));

        tool->SetDisabledBitmap(bitmap);
        Realize();
    }
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// Responds to colour changes, and passes event on to children.
void wxMoToolBar::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // TODO
    Refresh();

    // let the event propagate further
    event.Skip();
}

void wxMoToolBar::OnMouseEvent(wxMouseEvent& event)
{
    if ( event.RightDown() )
    {
        // find the tool under the mouse
        wxCoord x = 0, y = 0;
        event.GetPosition(&x, &y);

        wxToolBarToolBase *tool = FindToolForPosition(x, y);
        OnRightClick(tool ? tool->GetId() : -1, x, y);
    }
    else
    {
        wxRect rect = wxRect(wxPoint(0, 0), GetSize()); 
        int pressed = m_buttonStrip.ProcessMouseEvent(this, rect, event);
        if (pressed != -1)
        {
            wxMoBarButton* item = m_buttonStrip.GetButton(pressed);
            if (item)
                OnLeftClick(item->GetId(), false /* no check button allowed */);
        }
    }
}

void wxMoToolBar::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxMoToolBar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    wxRect rect(wxPoint(0, 0), GetSize());
    m_buttonStrip.Draw(this, rect, dc);
}

void wxMoToolBar::OnSize(wxSizeEvent& WXUNUSED(event))
{
    m_buttonStrip.ComputeSizes(this);
    Refresh();
}

void wxMoToolBar::SetBorderColour(const wxColour &colour)
{
    m_buttonStrip.SetBorderColour(colour);
    Refresh();
}

wxColour wxMoToolBar::GetBorderColour() const
{
    return m_buttonStrip.GetBorderColour();
}

bool wxMoToolBar::SetBackgroundColour(const wxColour &colour)
{
    wxControl::SetBackgroundColour(colour);
    m_buttonStrip.SetBackgroundColour(colour);
    Refresh();

    return true;
}

wxColour wxMoToolBar::GetBackgroundColour() const
{
    return m_buttonStrip.GetBackgroundColour();
}

bool wxMoToolBar::SetButtonBackgroundColour(const wxColour &colour)
{
    m_buttonStrip.SetButtonBackgroundColour(colour);
    m_buttonStrip.SetButtonSelectionColour(colour);

    return true;
}

wxColour wxMoToolBar::GetButtonBackgroundColour() const
{
    return m_buttonStrip.GetButtonBackgroundColour();
}

bool wxMoToolBar::SetForegroundColour(const wxColour &colour)
{
    wxControl::SetForegroundColour(colour);
    m_buttonStrip.SetLabelColour(colour);
    Refresh();

    return true;
}

wxColour wxMoToolBar::GetForegroundColour() const
{
    return GetForegroundColour();
}

bool wxMoToolBar::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);
    m_buttonStrip.SetLabelFont(font);
    InvalidateBestSize();
    Refresh();

    return true;
}

wxFont wxMoToolBar::GetFont() const
{
    return m_buttonStrip.GetLabelFont();
}
