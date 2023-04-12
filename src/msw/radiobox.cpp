/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/radiobox.cpp
// Purpose:     wxRadioBox implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_RADIOBOX

#include "wx/radiobox.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/brush.h"
    #include "wx/radiobut.h"
    #include "wx/settings.h"
    #include "wx/sizer.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

// TODO: wxCONSTRUCTOR
#if 0 // wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxRadioBoxStyle )

wxBEGIN_FLAGS( wxRadioBoxStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxRA_SPECIFY_COLS)
    wxFLAGS_MEMBER(wxRA_SPECIFY_ROWS)
wxEND_FLAGS( wxRadioBoxStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxRadioBox, wxControl, "wx/radiobox.h");

wxBEGIN_PROPERTIES_TABLE(wxRadioBox)
    wxEVENT_PROPERTY( Select , wxEVT_RADIOBOX , wxCommandEvent )
    wxPROPERTY_FLAGS( WindowStyle , wxRadioBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

#else
wxIMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl);
#endif

/*
    selection
    content
        label
        dimension
        item
*/

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxRadioBox creation
// ---------------------------------------------------------------------------

// Radio box item
void wxRadioBox::Init()
{
    m_selectedButton = wxNOT_FOUND;
}

bool wxRadioBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n,
                        const wxString choices[],
                        int majorDim,
                        long style,
                        const wxValidator& val,
                        const wxString& name)
{
    // common initialization
    if ( !wxStaticBox::Create(parent, id, title, pos, size, style, name) )
        return false;

    // the code elsewhere in this file supposes that either wxRA_SPECIFY_COLS
    // or wxRA_SPECIFY_ROWS is set, ensure that this is indeed the case
    if ( !(style & (wxRA_SPECIFY_ROWS | wxRA_SPECIFY_COLS)) )
        style |= wxRA_SPECIFY_COLS;

#if wxUSE_VALIDATORS
    SetValidator(val);
#else
    wxUnusedVar(val);
#endif // wxUSE_VALIDATORS/!wxUSE_VALIDATORS

    // Create all the buttons.
    m_radioButtons.reserve(n);

    for ( int i = 0; i < n; i++ )
    {
        // We need wxWANTS_CHARS to get the arrow key events and we also must
        // make the first button start of the group.
        auto rb = new wxRadioButton(this, wxID_ANY, choices[i],
                                    wxDefaultPosition, wxDefaultSize,
                                    (i == 0 ? wxRB_GROUP : 0) | wxWANTS_CHARS);

        m_radioButtons.push_back(rb);

        rb->Bind(wxEVT_RADIOBUTTON, &wxRadioBox::WXOnRadioButton, this);
        rb->Bind(wxEVT_KEY_DOWN, &wxRadioBox::WXOnRadioKeyDown, this);
    }

    // Call this to set the number of columns and rows before using them below.
    SetMajorDim(majorDim == 0 ? n : majorDim, style);

    const int numCols = GetColumnCount();

    // Create the sizer for buttons layout.

    // The gap is arbitrary, but this is simple and seems to work well.
    wxSize gapSize = GetTextExtent("X");
    gapSize.y /= 2;
    wxSizer* const sizerButtons = new wxGridSizer(numCols, gapSize);

    // This sizer can't be used directly for a couple of reasons: first, we
    // don't want it to expand to the total box area and spread out the buttons
    // if the box is too big. Second, we need to account for the static box
    // extra margins (and unfortunately we can't use wxStaticBoxSizer here as
    // this sizer can't be used with the box whose contents it's used to lay
    // out -- doing this would result in an infinite recursion).
    wxSizer* const sizerBox = new wxBoxSizer(wxVERTICAL);

    int borderTop, borderOther;
    GetBordersForSizer(&borderTop, &borderOther);

    if ( borderTop > borderOther )
        sizerBox->AddSpacer(borderTop - borderOther);

    borderOther += wxSizerFlags::GetDefaultBorder();
    sizerBox->Add(sizerButtons, wxSizerFlags().Border(wxALL, borderOther));

    SetSizer(sizerBox);

    const wxSizerFlags sizerFlags = wxSizerFlags().CentreVertical();

    // Arrange the buttons in the sizer in the right order depending on which
    // of wxRA_SPECIFY_XXX styles is specified.
    if ( style & wxRA_SPECIFY_COLS )
    {
        // This case is trivial, just add all the buttons in natural order.
        for ( auto button : m_radioButtons )
            sizerButtons->Add(button, sizerFlags);
    }
    else // row layout
    {
        const int numRows = GetRowCount();

        // This is slightly trickier, as we need to lay out the buttons in the
        // top to bottom order, but we can only add them to the sizer in the
        // left to right one, so we need to pick up the right button to add.
        for ( int row = 0; row < numRows; ++row )
        {
            for ( int col = 0; col < numCols; ++col )
            {
                const int i = row + numRows*col;
                if ( i >= n )
                {
                    // This can happen for the last column if there are not
                    // enough buttons to fill it.
                    continue;
                }

                sizerButtons->Add(m_radioButtons[i], sizerFlags);
            }
        }
    }

    // The first radio button is selected if we have any buttons at all.
    if ( n > 0 )
        m_selectedButton = 0;

    // Now that we have items determine what is the best size and set it.
    SetInitialSize(size);

    // The base wxStaticBox class never accepts focus, but we do because giving
    // focus to a wxRadioBox actually gives it to one of its buttons, which are
    // not visible at wx level and hence are not taken into account by the
    // logic in wxControlContainer code.
    m_container.EnableSelfFocus();

    return true;
}

bool wxRadioBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos,
                        const wxSize& size,
                        const wxArrayString& choices,
                        int majorDim,
                        long style,
                        const wxValidator& val,
                        const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, title, pos, size, chs.GetCount(),
                  chs.GetStrings(), majorDim, style, val, name);
}

wxRadioBox::~wxRadioBox()
{
    SendDestroyEvent();
}

wxWindowList wxRadioBox::GetCompositeWindowParts() const
{
    wxWindowList parts;
    for ( auto button : m_radioButtons )
        parts.push_back(button);
    return parts;
}

// ----------------------------------------------------------------------------
// events generation
// ----------------------------------------------------------------------------

void wxRadioBox::WXOnRadioButton(wxCommandEvent& event)
{
    // Find the button which was clicked.
    auto sender = event.GetEventObject();
    const unsigned int n = GetCount();
    for ( unsigned int i = 0; i < n; i++ )
    {
        if ( sender == m_radioButtons[i] )
        {
            m_selectedButton = i;
            SendNotificationEvent();
            return;
        }
    }

    wxFAIL_MSG("alien button clicked");
}

void wxRadioBox::Command(wxCommandEvent & event)
{
    SetSelection (event.GetInt());
    SetFocus();
    ProcessCommand(event);
}

void wxRadioBox::SendNotificationEvent()
{
    wxCommandEvent event(wxEVT_RADIOBOX, m_windowId);
    event.SetInt( m_selectedButton );
    event.SetString(GetString(m_selectedButton));
    event.SetEventObject( this );
    ProcessCommand(event);
}

// ----------------------------------------------------------------------------
// simple accessors
// ----------------------------------------------------------------------------

unsigned int wxRadioBox::GetCount() const
{
    return m_radioButtons.size();
}

void wxRadioBox::SetString(unsigned int item, const wxString& label)
{
    wxCHECK_RET( IsValid(item), wxT("invalid radiobox index") );

    m_radioButtons[item]->SetLabel(label);

    InvalidateBestSize();
}

void wxRadioBox::SetSelection(int N)
{
    wxCHECK_RET( IsValid(N), wxT("invalid radiobox index") );

    m_radioButtons[N]->SetValue(true);

    m_selectedButton = N;
}

// Find string for position
wxString wxRadioBox::GetString(unsigned int item) const
{
    wxCHECK_MSG( IsValid(item), wxEmptyString,
                 wxT("invalid radiobox index") );

    return m_radioButtons[item]->GetLabel();
}

void wxRadioBox::SetFocus()
{
    int n = m_selectedButton;
    if ( n == wxNOT_FOUND && !m_radioButtons.empty() )
        n = 0;

    if ( n != wxNOT_FOUND )
    {
        m_radioButtons[n]->SetFocus();
    }
}

bool wxRadioBox::CanBeFocused() const
{
    // If the control itself is hidden or disabled, no need to check anything
    // else.
    if ( !wxStaticBox::CanBeFocused() )
        return false;

    // Otherwise, check if we have any buttons that can be focused.
    for ( size_t item = 0; item < GetCount(); item++ )
    {
        if ( IsItemEnabled(item) && IsItemShown(item) )
            return true;
    }

    // We didn't find any items that can accept focus, so neither can we as a
    // whole accept it.
    return false;
}

// Enable the entire radiobox or a specific button
bool wxRadioBox::Enable(bool enable)
{
    if ( !wxStaticBoxBase::Enable(enable) )
        return false;

    for ( auto button : m_radioButtons )
        button->Enable(enable);

    return true;
}

bool wxRadioBox::Enable(unsigned int item, bool enable)
{
    wxCHECK_MSG( IsValid(item), false,
                 wxT("invalid item in wxRadioBox::Enable()") );

    return m_radioButtons[item]->Enable(enable);
}

bool wxRadioBox::IsItemEnabled(unsigned int item) const
{
    wxCHECK_MSG( IsValid(item), false,
                 wxT("invalid item in wxRadioBox::IsItemEnabled()") );

    return m_radioButtons[item]->IsThisEnabled();
}

// Show the entire radiobox or a specific button
bool wxRadioBox::Show(bool show)
{
    if ( !wxStaticBoxBase::Show(show) )
        return false;

    for ( auto button : m_radioButtons )
        button->Show(show);

    return true;
}

bool wxRadioBox::Show(unsigned int item, bool show)
{
    wxCHECK_MSG( IsValid(item), false,
                 wxT("invalid item in wxRadioBox::Show()") );

    if ( !m_radioButtons[item]->Show(show) )
        return false;

    InvalidateBestSize();

    return true;
}

bool wxRadioBox::IsItemShown(unsigned int item) const
{
    wxCHECK_MSG( IsValid(item), false,
                 wxT("invalid item in wxRadioBox::IsItemShown()") );

    return m_radioButtons[item]->IsShown();
}

#if wxUSE_TOOLTIPS

bool wxRadioBox::HasToolTips() const
{
    return wxStaticBox::HasToolTips() || wxRadioBoxBase::HasItemToolTips();
}

void wxRadioBox::DoSetItemToolTip(unsigned int item, wxToolTip *tooltip)
{
    // we have already checked for the item to be valid in wxRadioBoxBase
    if ( tooltip )
        m_radioButtons[item]->SetToolTip(tooltip->GetTip());
    else
        m_radioButtons[item]->UnsetToolTip();
}

#endif // wxUSE_TOOLTIPS

int wxRadioBox::GetItemFromPoint(const wxPoint& pt) const
{
    const unsigned int count = GetCount();
    for ( unsigned int i = 0; i < count; i++ )
    {
        if ( m_radioButtons[i]->GetRect().Contains(pt) )
            return i;
    }

    return wxNOT_FOUND;
}

// ---------------------------------------------------------------------------
// keyboard navigation
// ---------------------------------------------------------------------------

void wxRadioBox::WXOnRadioKeyDown(wxKeyEvent& event)
{
    wxDirection dir;
    switch ( event.GetKeyCode() )
    {
        case WXK_UP:
            dir = wxUP;
            break;

        case WXK_LEFT:
            dir = wxLEFT;
            break;

        case WXK_DOWN:
            dir = wxDOWN;
            break;

        case WXK_RIGHT:
            dir = wxRIGHT;
            break;

        default:
            event.Skip();
            return;
    }

    const int selNew = GetNextItem(m_selectedButton, dir, GetWindowStyle());
    if ( selNew != m_selectedButton )
    {
        SetSelection(selNew);

        SendNotificationEvent();
    }
}

#endif // wxUSE_RADIOBOX
