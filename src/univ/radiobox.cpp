/////////////////////////////////////////////////////////////////////////////
// Name:        univ/radiobox.cpp
// Purpose:     wxRadioBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univradiobox.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RADIOBOX

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/radiobox.h"
    #include "wx/radiobut.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/theme.h"
#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/colschem.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int BUTTON_BORDER_X = 0;
static const int BUTTON_BORDER_Y = 0;

static const int BOX_BORDER_X = 0;
static const int BOX_BORDER_Y = 0;

// ----------------------------------------------------------------------------
// wxRadioBox event handler
// ----------------------------------------------------------------------------

class wxRadioHookHandler : public wxEvtHandler
{
public:
    wxRadioHookHandler(wxRadioBox *radio) { m_radio = radio; }

    virtual bool ProcessEvent(wxEvent& event)
    {
        // we intercept the command events from radio buttons
        if ( event.GetEventType() == wxEVT_COMMAND_RADIOBUTTON_SELECTED )
        {
            m_radio->OnRadioButton(event);
        }

        // just pass it on
        return GetNextHandler()->ProcessEvent(event);
    }

private:
    wxRadioBox *m_radio;
};

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)

// ----------------------------------------------------------------------------
// wxRadioBox creation
// ----------------------------------------------------------------------------

void wxRadioBox::Init()
{
    m_selection = -1;
    m_majorDim = 0;
}

bool wxRadioBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n,
                        const wxString *choices,
                        int majorDim,
                        long style,
                        const wxValidator& val,
                        const wxString& name)
{
    if ( !wxStaticBox::Create(parent, id, title, pos, size, style, name) )
        return FALSE;

#if wxUSE_VALIDATORS
    SetValidator(val);
#endif // wxUSE_VALIDATORS

    Append(n, choices);

    SetMajorDim(majorDim);

    if ( size == wxDefaultSize )
    {
        SetClientSize(DoGetBestClientSize());
    }

    // radiobox should already have selection so select at least one item
    SetSelection(0);

    return TRUE;
}

wxRadioBox::~wxRadioBox()
{
    // remove the event handlers we pushed on them from all buttons
    size_t count = m_buttons.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_buttons[n]->PopEventHandler(TRUE /* delete it */);
    }
}

// ----------------------------------------------------------------------------
// wxRadioBox init
// ----------------------------------------------------------------------------

void wxRadioBox::SetMajorDim(int majorDim)
{
    m_majorDim = majorDim;

    int minorDim = (GetCount() + m_majorDim - 1) / m_majorDim;

    if ( GetWindowStyle() & wxRA_SPECIFY_COLS )
    {
        m_numCols = majorDim;
        m_numRows = minorDim;
    }
    else // wxRA_SPECIFY_ROWS
    {
        m_numCols = minorDim;
        m_numRows = majorDim;
    }
}

void wxRadioBox::Append(int count, const wxString *choices)
{
    if ( !count )
        return;

    wxWindow *parent = GetParent();
    m_buttons.Alloc(count);
    for ( int n = 0; n < count; n++ )
    {
        // make the first button in the box the start of new group by giving it
        // wxRB_GROUP style
        wxRadioButton *btn = new wxRadioButton(parent, -1, choices[n],
                                               wxDefaultPosition,
                                               wxDefaultSize,
                                               n == 0 ? wxRB_GROUP : 0);

        // we want to get the events from the buttons to translate it into
        btn->PushEventHandler(new wxRadioHookHandler(this));
        m_buttons.Add(btn);
    }
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

void wxRadioBox::SetSelection(int n)
{
    wxCHECK_RET( IsValid(n), _T("invalid index in wxRadioBox::SetSelection") );

    m_selection = n;

    // this will also unselect the previously selected button in our group
    m_buttons[n]->SetValue(TRUE);
}

int wxRadioBox::GetSelection() const
{
    return m_selection;
}

void wxRadioBox::OnRadioButton(wxEvent& event)
{
    int n = m_buttons.Index((wxRadioButton *)event.GetEventObject());
    wxCHECK_RET( n != wxNOT_FOUND, _T("click from alien radio button") );

    m_selection = n;

    wxCommandEvent event2(wxEVT_COMMAND_RADIOBOX_SELECTED, GetId());
    InitCommandEvent(event2);
    event2.SetInt(n);
    event2.SetString(GetString(n));
    Command(event2);
}

// ----------------------------------------------------------------------------
// methods forwarded to the buttons
// ----------------------------------------------------------------------------

wxString wxRadioBox::GetString(int n) const
{
    wxCHECK_MSG( IsValid(n), _T(""),
                 _T("invalid index in wxRadioBox::GetString") );

    return m_buttons[n]->GetLabel();
}

void wxRadioBox::SetString(int n, const wxString& label)
{
    wxCHECK_RET( IsValid(n), _T("invalid index in wxRadioBox::SetString") );

    m_buttons[n]->SetLabel(label);
}

void wxRadioBox::Enable(int n, bool enable)
{
    wxCHECK_RET( IsValid(n), _T("invalid index in wxRadioBox::Enable") );

    m_buttons[n]->Enable(enable);
}

void wxRadioBox::Show(int n, bool show)
{
    wxCHECK_RET( IsValid(n), _T("invalid index in wxRadioBox::Show") );

    m_buttons[n]->Show(show);
}

// ----------------------------------------------------------------------------
// methods forwarded to the static box
// ----------------------------------------------------------------------------

bool wxRadioBox::Enable(bool enable)
{
    return wxStaticBox::Enable(enable);
}

bool wxRadioBox::Show(bool show)
{
    return wxStaticBox::Show(show);
}

wxString wxRadioBox::GetLabel() const
{
    return wxStaticBox::GetLabel();
}

void wxRadioBox::SetLabel(const wxString& label)
{
    wxStaticBox::SetLabel(label);
}

// ----------------------------------------------------------------------------
// buttons positioning
// ----------------------------------------------------------------------------

wxSize wxRadioBox::GetMaxButtonSize() const
{
    int widthMax, heightMax, width, height;
    widthMax = heightMax = 0;

    int count = GetCount();
    for ( int n = 0; n < count; n++ )
    {
        m_buttons[n]->GetSize(&width, &height);

        if ( width > widthMax )
            widthMax = width;
        if ( height > heightMax )
            heightMax = height;
    }

    // add a intra button border
    widthMax += BUTTON_BORDER_X;
    heightMax += BUTTON_BORDER_Y;

    return wxSize(widthMax, heightMax);
}

/*
   Remember that wxRA_SPECIFY_COLS means that the buttons go from top to
   bottom and from left to right while wxRA_SPECIFY_ROWS means that they are
   laid out from left to right and then from top to bottom
*/

wxSize wxRadioBox::DoGetBestClientSize() const
{
    wxSize sizeBtn = GetMaxButtonSize();
    sizeBtn.x *= m_numCols;
    sizeBtn.y *= m_numRows;

    // add a border around all buttons
    sizeBtn.x += 2*BOX_BORDER_X;
    sizeBtn.y += 2*BOX_BORDER_Y;

    // account for the area taken by static box
    wxRect rect = GetBorderGeometry();
    sizeBtn.x += rect.x + rect.width;
    sizeBtn.y += rect.y + rect.height;

    return sizeBtn;
}

void wxRadioBox::DoMoveWindow(int x0, int y0, int width, int height)
{
    wxStaticBox::DoMoveWindow(x0, y0, width, height);

    wxSize sizeBtn = GetMaxButtonSize();
    wxPoint ptOrigin = GetBoxAreaOrigin();

    x0 += ptOrigin.x + BOX_BORDER_X;
    y0 += ptOrigin.y + BOX_BORDER_Y;

    int x = x0,
        y = y0;

    int count = GetCount();
    for ( int n = 0; n < count; n++ )
    {
        m_buttons[n]->SetSize(x, y, sizeBtn.x, sizeBtn.y);

        if ( GetWindowStyle() & wxRA_SPECIFY_COLS )
        {
            // from top to bottom
            if ( (n + 1) % m_numRows )
            {
                // continue in this column
                y += sizeBtn.y;
            }
            else
            {
                // start a new column
                x += sizeBtn.x;
                y = y0;
            }
        }
        else // wxRA_SPECIFY_ROWS: mirror the code above
        {
            // from left to right
            if ( (n + 1) % m_numCols )
            {
                // continue in this row
                x += sizeBtn.x;
            }
            else
            {
                // start a new row
                y += sizeBtn.y;
                x = x0;
            }
        }
    }
}

#endif // wxUSE_RADIOBOX

