/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:     wxComboBox class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "combobox.h"
#endif

#include "wx/combobox.h"
#include "wx/menu.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)
#endif

// composite combobox implementation by Dan "Bud" Keith bud@otsys.com


static int nextPopUpMenuId = 1000 ;
MenuHandle NewUniqueMenu() 
{
  MenuHandle handle = NewMenu( nextPopUpMenuId , "\pMenu" ) ;
  nextPopUpMenuId++ ;
  return handle ;
}


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the margin between the text control and the choice
static const wxCoord MARGIN = 2;
static const int    POPUPWIDTH = 18;
static const int    POPUPHEIGHT = 23;


// ----------------------------------------------------------------------------
// wxComboBoxText: text control forwards events to combobox
// ----------------------------------------------------------------------------

class wxComboBoxText : public wxTextCtrl
{
public:
    wxComboBoxText( wxComboBox * cb )
        : wxTextCtrl( cb , 1 )
    {
        m_cb = cb;
    }

protected:
    void OnTextChange( wxCommandEvent& event )
    {
        wxString    s = GetValue();
        
        if (!s.IsEmpty())
            m_cb->DelegateTextChanged( s );

        event.Skip();
    }

private:
    wxComboBox *m_cb;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxComboBoxText, wxTextCtrl)
    EVT_TEXT(-1, wxComboBoxText::OnTextChange)
END_EVENT_TABLE()

class wxComboBoxChoice : public wxChoice
{
public:
    wxComboBoxChoice(wxComboBox *cb, int style)
        : wxChoice( cb , 1 )
    {
        m_cb = cb;
    }

protected:
    void OnChoice( wxCommandEvent& e )
    {
        wxString    s = e.GetString();

        m_cb->DelegateChoice( s );
    }

private:
    wxComboBox *m_cb;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxComboBoxChoice, wxChoice)
    EVT_CHOICE(-1, wxComboBoxChoice::OnChoice)
END_EVENT_TABLE()

wxComboBox::~wxComboBox()
{
   // delete the controls now, don't leave them alive even though they would
    // still be eventually deleted by our parent - but it will be too late, the
    // user code expects them to be gone now
    if (m_text != NULL) {
        delete m_text;
        m_text = NULL;
    }
    if (m_choice != NULL) {
        delete m_choice;
        m_choice = NULL;
    }
}


// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize wxComboBox::DoGetBestSize() const
{
    wxSize size = m_choice->GetBestSize();
    
    if ( m_text != 0 )
    {
        wxSize  sizeText = m_text->GetBestSize();
        
        size.x = POPUPWIDTH + sizeText.x + MARGIN;
    }

    return size;
}

void wxComboBox::DoMoveWindow(int x, int y, int width, int height) {
    height = POPUPHEIGHT;
    
    wxControl::DoMoveWindow(x, y, width, height);

    if ( m_text == 0 )
    {
        m_choice->SetSize(0, 0 , width, -1);
    }
    else
    {
        wxCoord wText = width - POPUPWIDTH;
        m_text->SetSize(0, 0, wText, height);
        m_choice->SetSize(0 + wText + MARGIN, 0, POPUPWIDTH, -1);
    }    
}



// ----------------------------------------------------------------------------
// operations forwarded to the subcontrols
// ----------------------------------------------------------------------------

bool wxComboBox::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return FALSE;

    return TRUE;
}

bool wxComboBox::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return FALSE;

    return TRUE;
}

 void wxComboBox::SetFocus()
 {
    m_text->SetFocus();
 }


void wxComboBox::DelegateTextChanged( const wxString& value ) {
}


void wxComboBox::DelegateChoice( const wxString& value )
{
    SetStringSelection( value );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           long style,
           const wxValidator& validator,
           const wxString& name)
{

    Rect bounds ;
    Str255 title ;

    if ( !wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize, style ,
                            wxDefaultValidator, name) )
    {
        return FALSE;
    }

    m_choice = new wxComboBoxChoice(this, style );

    wxSize csize = size;
    if ( style & wxCB_READONLY )
    {
        m_text = 0;
    }
    else
    {
        m_text = new wxComboBoxText(this);
        if ( size.y == -1 ) {
          csize.y = m_text->GetSize().y ;
        }
    }
    
    DoSetSize(pos.x, pos.y, csize.x, csize.y);
    
    for ( int i = 0 ; i < n ; i++ )
    {
        m_choice->DoAppend( choices[ i ] );
    }

    return TRUE;
}

wxString wxComboBox::GetValue() const
{
    wxString        result;
    
    if ( m_text == 0 )
    {
        result = m_choice->GetString( m_choice->GetSelection() );
    }
    else
    {
        result = m_text->GetValue();
    }

    return result;
}

void wxComboBox::SetValue(const wxString& value)
{
    SetStringSelection( value ) ;
}

// Clipboard operations
void wxComboBox::Copy()
{
    if ( m_text != 0 )
    {
        m_text->Copy();
    }
}

void wxComboBox::Cut()
{
    if ( m_text != 0 )
    {
        m_text->Cut();
    }
}

void wxComboBox::Paste()
{
    if ( m_text != 0 )
    {
        m_text->Paste();
    }
}

void wxComboBox::SetEditable(bool editable)
{
    if ( ( m_text == 0 ) && editable )
    {
        m_text = new wxComboBoxText( this );
    }
    else if ( ( m_text != 0 ) && !editable )
    {
        delete m_text;
        m_text = 0;
    }

    int currentX, currentY;
    GetPosition( &currentX, &currentY );
    
    int currentW, currentH;
    GetSize( &currentW, &currentH );

    DoMoveWindow( currentX, currentY, currentW, currentH );
}

void wxComboBox::SetInsertionPoint(long pos)
{
    // TODO
}

void wxComboBox::SetInsertionPointEnd()
{
    // TODO
}

long wxComboBox::GetInsertionPoint() const
{
    // TODO
    return 0;
}

long wxComboBox::GetLastPosition() const
{
    // TODO
    return 0;
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
    // TODO
}

void wxComboBox::Remove(long from, long to)
{
    // TODO
}

void wxComboBox::SetSelection(long from, long to)
{
    // TODO
}

void wxComboBox::Append(const wxString& item)
{
    // I am not sure what other ports do,
    // but wxMac chokes on empty entries.

    if (!item.IsEmpty())
        m_choice->DoAppend( item );
}

void wxComboBox::Delete(int n)
{
    m_choice->Delete( n );
}

void wxComboBox::Clear()
{
    m_choice->Clear();
}

int wxComboBox::GetSelection() const
{
    return m_choice->GetSelection();
}

void wxComboBox::SetSelection(int n)
{
    m_choice->SetSelection( n );
    
    if ( m_text != 0 )
    {
        m_text->SetValue( GetString( n ) );
    }
}

int wxComboBox::FindString(const wxString& s) const
{
    return m_choice->FindString( s );
}

wxString wxComboBox::GetString(int n) const
{
    return m_choice->GetString( n );
}

wxString wxComboBox::GetStringSelection() const
{
    int sel = GetSelection ();
    if (sel > -1)
        return wxString(this->GetString (sel));
    else
        return wxString("");
}

bool wxComboBox::SetStringSelection(const wxString& sel)
{
    int s = FindString (sel);
    if (s > -1)
        {
            SetSelection (s);
            return TRUE;
        }
    else
        return FALSE;
}

void wxComboBox::MacHandleControlClick( WXWidget control , wxInt16 controlpart ) 
{
    wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, m_windowId );
    event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(GetStringSelection());
    ProcessCommand(event);
}

