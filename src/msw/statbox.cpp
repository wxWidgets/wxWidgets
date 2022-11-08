/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/statbox.cpp
// Purpose:     wxStaticBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_STATBOX

#include "wx/statbox.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
#endif

#include "wx/notebook.h"
#include "wx/sysopt.h"

#include "wx/msw/uxtheme.h"

#include <windowsx.h> // needed by GET_X_LPARAM and GET_Y_LPARAM macros

#include "wx/msw/private.h"
#include "wx/msw/missing.h"
#include "wx/msw/dc.h"
#include "wx/msw/private/winstyle.h"

namespace
{

// Offset of the first pixel of the label from the box left border.
//
// FIXME: value is hardcoded as this is what it is on my system, no idea if
//        it's true everywhere
const int LABEL_HORZ_OFFSET = 9;

// Extra borders around the label on left/right and bottom sides.
const int LABEL_HORZ_BORDER = 2;
const int LABEL_VERT_BORDER = 2;

// Offset of the box contents from left/right/bottom edge (top one is
// different, see GetBordersForSizer()). This one is completely arbitrary.
const int CHILDREN_OFFSET = 5;

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStaticBox
// ----------------------------------------------------------------------------

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    if ( !MSWCreateControl(wxT("BUTTON"), label, pos, size) )
        return false;

    return true;
}

bool wxStaticBox::Create(wxWindow* parent,
                         wxWindowID id,
                         wxWindow* labelWin,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    wxCHECK_MSG( labelWin, false, wxS("Label window can't be null") );

    if ( !Create(parent, id, wxString(), pos, size, style, name) )
        return false;

    m_labelWin = labelWin;
    m_labelWin->Reparent(this);

    PositionLabelWindow();

    return true;
}

void wxStaticBox::PositionLabelWindow()
{
    m_labelWin->SetSize(m_labelWin->GetBestSize());
    m_labelWin->Move(FromDIP(LABEL_HORZ_OFFSET), 0);
}

wxWindowList wxStaticBox::GetCompositeWindowParts() const
{
    wxWindowList parts;
    if ( m_labelWin )
        parts.push_back(m_labelWin);
    return parts;
}

WXDWORD wxStaticBox::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    long styleWin = wxStaticBoxBase::MSWGetStyle(style, exstyle);

    // no need for it anymore, must be removed for wxRadioBox child
    // buttons to be able to repaint themselves
    styleWin &= ~WS_CLIPCHILDREN;

    if ( exstyle )
    {
        // We may have children inside this static box, so use this style for
        // TAB navigation to work if we ever use IsDialogMessage() to implement
        // it (currently we don't because it's too buggy and implement TAB
        // navigation ourselves, but this could change in the future).
        *exstyle |= WS_EX_CONTROLPARENT;

        *exstyle |= WS_EX_TRANSPARENT;
    }

    styleWin |= BS_GROUPBOX;

    return styleWin;
}

wxSize wxStaticBox::DoGetBestSize() const
{
    wxSize best;

    // Calculate the size needed by the label
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, GetFont());

    int wBox;
    GetTextExtent(GetLabelText(wxGetWindowText(m_hWnd)), &wBox, &cy);

    wBox += 3*cx;
    int hBox = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    // If there is a sizer then the base best size is the sizer's minimum
    if (GetSizer() != nullptr)
    {
        wxSize cm(GetSizer()->CalcMin());
        best = ClientToWindowSize(cm);
        // adjust for a long label if needed
        best.x = wxMax(best.x, wBox);
    }
    // otherwise the best size falls back to the label size
    else
    {
        best = wxSize(wBox, hBox);
    }
    return best;
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    // Base class version doesn't leave enough space at the top when the label
    // is empty, so we can't use it here, even though the code is pretty
    // similar.
    if ( m_labelWin )
    {
        *borderTop = m_labelWin->GetSize().y;
    }
    else if ( !GetLabel().empty() )
    {
        *borderTop = GetCharHeight();
    }
    else // No label window nor text.
    {
        // This is completely arbitrary, but using the full char height in
        // this case too seems bad as it leaves too much space at the top
        // (although it does have the advantage of aligning the controls
        // inside static boxes with and without labels vertically).
        *borderTop = 2*FromDIP(CHILDREN_OFFSET);
    }

    *borderTop += FromDIP(LABEL_VERT_BORDER);

    *borderOther = FromDIP(CHILDREN_OFFSET);
}

bool wxStaticBox::SetBackgroundColour(const wxColour& colour)
{
    // Do _not_ call the immediate base class method, we don't need to set the
    // label window (which is the only sub-window of this composite window)
    // background explicitly because it will almost always be a wxCheckBox or
    // wxRadioButton which inherits its background from the box anyhow, so
    // setting it would be at best useless.
    return wxStaticBoxBase::SetBackgroundColour(colour);
}

bool wxStaticBox::SetForegroundColour(const wxColour& colour)
{
    if ( !base_type::SetForegroundColour(colour) )
        return false;

    if ( colour.IsOk() && !m_labelWin )
    {
        // Switch to using a custom label as we can't support custom colours
        // otherwise.
        m_labelWin = new wxStaticText(this, wxID_ANY, GetLabel());
        PositionLabelWindow();
    }

    return true;
}

bool wxStaticBox::SetFont(const wxFont& font)
{
    if ( !base_type::SetFont(font) )
        return false;

    // We need to reposition the label as its size may depend on the font.
    if ( m_labelWin )
    {
        PositionLabelWindow();
    }

    return true;
}

void wxStaticBox::SetLabel(const wxString& label)
{
    if ( m_labelWin )
    {
        // Ensure that GetLabel() returns the correct value.
        m_labelOrig = label;

        // And update the actually shown label.
        m_labelWin->SetLabel(label);
        PositionLabelWindow();
    }
    else
    {
        base_type::SetLabel(label);
    }
}

WXLRESULT wxStaticBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ( nMsg == WM_NCHITTEST )
    {
        // This code breaks some other processing such as enter/leave tracking
        // so it's off by default.

        static int s_useHTClient = -1;
        if (s_useHTClient == -1)
            s_useHTClient = wxSystemOptions::GetOptionInt(wxT("msw.staticbox.htclient"));
        if (s_useHTClient == 1)
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            ScreenToClient(&xPos, &yPos);

            // Make sure you can drag by the top of the groupbox, but let
            // other (enclosed) controls get mouse events also
            if ( yPos < 10 )
                return (long)HTCLIENT;
        }
    }

    if ( nMsg == WM_PRINTCLIENT )
    {
        // we have to process WM_PRINTCLIENT ourselves as otherwise child
        // windows' background (eg buttons in radio box) would never be drawn
        // unless we have a parent with non default background

        // so check first if we have one
        if ( !HandlePrintClient((WXHDC)wParam) )
        {
            // no, we don't, erase the background ourselves
            RECT rc;
            ::GetClientRect(GetHwnd(), &rc);

            HDC hdc = (HDC)wParam;
            HBRUSH hbr = MSWGetBgBrush(hdc);

            // if there is no special brush for painting this control, just use
            // the solid background colour
            wxBrush brush;
            if ( !hbr )
            {
                brush = wxBrush(GetParent()->GetBackgroundColour());
                hbr = GetHbrushOf(brush);
            }

            ::FillRect(hdc, &rc, hbr);
        }

        return 0;
    }

    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

#endif // wxUSE_STATBOX
