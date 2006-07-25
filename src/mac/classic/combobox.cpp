/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/combobox.cpp
// Purpose:     wxComboBox class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/combobox.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/menu.h"
#endif

#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)

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
    void OnChar( wxKeyEvent& event )
    {
        if ( event.GetKeyCode() == WXK_RETURN )
        {
            wxString value = GetValue();

            if ( m_cb->GetCount() == 0 )
            {
                // make Enter generate "selected" event if there is only one item
                // in the combobox - without it, it's impossible to select it at
                // all!
                wxCommandEvent event( wxEVT_COMMAND_COMBOBOX_SELECTED, m_cb->GetId() );
                event.SetInt( 0 );
                event.SetString( value );
                event.SetEventObject( m_cb );
                m_cb->GetEventHandler()->ProcessEvent( event );
            }
            else
            {
                // add the item to the list if it's not there yet
                if ( m_cb->FindString(value) == wxNOT_FOUND )
                {
                    m_cb->Append(value);
                    m_cb->SetStringSelection(value);

                    // and generate the selected event for it
                    wxCommandEvent event( wxEVT_COMMAND_COMBOBOX_SELECTED, m_cb->GetId() );
                    event.SetInt( m_cb->GetCount() - 1 );
                    event.SetString( value );
                    event.SetEventObject( m_cb );
                    m_cb->GetEventHandler()->ProcessEvent( event );
                }

                // This will invoke the dialog default action, such
                // as the clicking the default button.

                wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
                if ( tlw && tlw->GetDefaultItem() )
                {
                    wxButton *def = wxDynamicCast(tlw->GetDefaultItem(), wxButton);
                    if ( def && def->IsEnabled() )
                    {
                        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
                        event.SetEventObject(def);
                        def->Command(event);
                        return ;
                   }
                }

                return;
            }
        }

        event.Skip();
    }

private:
    wxComboBox *m_cb;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxComboBoxText, wxTextCtrl)
    EVT_CHAR( wxComboBoxText::OnChar)
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
        wxString s = e.GetString();

        m_cb->DelegateChoice( s );
        wxCommandEvent event2(wxEVT_COMMAND_COMBOBOX_SELECTED, m_cb->GetId() );
        event2.SetInt(m_cb->GetSelection());
        event2.SetEventObject(m_cb);
        event2.SetString(m_cb->GetStringSelection());
        m_cb->ProcessCommand(event2);
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
    // delete client objects
    FreeData();

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

    if ( m_text != NULL )
    {
        wxSize  sizeText = m_text->GetBestSize();

        size.x = POPUPWIDTH + sizeText.x + MARGIN;
    }

    return size;
}

void wxComboBox::DoMoveWindow(int x, int y, int width, int height) {
    height = POPUPHEIGHT;

    wxControl::DoMoveWindow(x, y, width, height);

    if ( m_text == NULL )
    {
        m_choice->SetSize(0, 0 , width, -1);
    }
    else
    {
        wxCoord wText = width - POPUPWIDTH - MARGIN;
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
        return false;

    return true;
}

bool wxComboBox::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return false;

    return true;
}

void wxComboBox::SetFocus()
{
    if ( m_text != NULL) {
        m_text->SetFocus();
    }
}


void wxComboBox::DelegateTextChanged( const wxString& value )
{
    SetStringSelection( value );
}


void wxComboBox::DelegateChoice( const wxString& value )
{
    SetStringSelection( value );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    wxCArrayString chs( choices );

    return Create( parent, id, value, pos, size, chs.GetCount(),
                   chs.GetStrings(), style, validator, name );
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
    if ( !wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize, style ,
                            wxDefaultValidator, name) )
    {
        return false;
    }

    m_choice = new wxComboBoxChoice(this, style );

    wxSize csize = size;
    if ( style & wxCB_READONLY )
    {
        m_text = NULL;
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

    return true;
}

wxString wxComboBox::GetValue() const
{
    wxString result;

    if ( m_text == NULL )
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
    int s = FindString (value);
    if (s == wxNOT_FOUND && !HasFlag(wxCB_READONLY) )
    {
        m_choice->Append(value) ;
    }
    SetStringSelection( value ) ;
}

// Clipboard operations
void wxComboBox::Copy()
{
    if ( m_text != NULL )
    {
        m_text->Copy();
    }
}

void wxComboBox::Cut()
{
    if ( m_text != NULL )
    {
        m_text->Cut();
    }
}

void wxComboBox::Paste()
{
    if ( m_text != NULL )
    {
        m_text->Paste();
    }
}

void wxComboBox::SetEditable(bool editable)
{
    if ( ( m_text == NULL ) && editable )
    {
        m_text = new wxComboBoxText( this );
    }
    else if ( ( m_text != NULL ) && !editable )
    {
        delete m_text;
        m_text = NULL;
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

wxTextPos wxComboBox::GetLastPosition() const
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

int wxComboBox::DoAppend(const wxString& item)
{
    return m_choice->DoAppend( item ) ;
}

int wxComboBox::DoInsert(const wxString& item, unsigned int pos)
{
    return m_choice->DoInsert( item , pos ) ;
}

void wxComboBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    return m_choice->DoSetItemClientData( n , clientData ) ;
}

void* wxComboBox::DoGetItemClientData(unsigned int n) const
{
    return m_choice->DoGetItemClientData( n ) ;
}

void wxComboBox::DoSetItemClientObject(unsigned int n, wxClientData* clientData)
{
    return m_choice->DoSetItemClientObject(n , clientData);
}

wxClientData* wxComboBox::DoGetItemClientObject(unsigned int n) const
{
    return m_choice->DoGetItemClientObject( n ) ;
}

void wxComboBox::FreeData()
{
    if ( HasClientObjectData() )
    {
        unsigned int count = GetCount();
        for ( unsigned int n = 0; n < count; n++ )
        {
            SetClientObject( n, NULL );
        }
    }
}

void wxComboBox::Delete(unsigned int n)
{
    // force client object deletion
    if( HasClientObjectData() )
        SetClientObject( n, NULL );
    m_choice->Delete( n );
}

void wxComboBox::Clear()
{
    FreeData();
    m_choice->Clear();
}

int wxComboBox::GetSelection() const
{
    return m_choice->GetSelection();
}

void wxComboBox::SetSelection(int n)
{
    m_choice->SetSelection( n );

    if ( m_text != NULL )
    {
        m_text->SetValue(GetString(n));
    }
}

int wxComboBox::FindString(const wxString& s, bool bCase ) const
{
    return m_choice->FindString( s , bCase );
}

wxString wxComboBox::GetString(unsigned int n) const
{
    return m_choice->GetString( n );
}

wxString wxComboBox::GetStringSelection() const
{
    int sel = GetSelection ();

    if (sel != wxNOT_FOUND)
        return wxString(this->GetString((unsigned int)sel));
    else
        return wxEmptyString;
}

void wxComboBox::SetString(unsigned int n, const wxString& s)
{
    m_choice->SetString( n , s );
}

bool wxComboBox::IsEditable() const
{
    return m_text != NULL && !HasFlag(wxCB_READONLY);
}

void wxComboBox::Undo()
{
    if (m_text != NULL)
        m_text->Undo();
}

void wxComboBox::Redo()
{
    if (m_text != NULL)
        m_text->Redo();
}

void wxComboBox::SelectAll()
{
    if (m_text != NULL)
        m_text->SelectAll();
}

bool wxComboBox::CanCopy() const
{
    if (m_text != NULL)
        return m_text->CanCopy();
    else
        return false;
}

bool wxComboBox::CanCut() const
{
    if (m_text != NULL)
        return m_text->CanCut();
    else
        return false;
}

bool wxComboBox::CanPaste() const
{
    if (m_text != NULL)
        return m_text->CanPaste();
    else
        return false;
}

bool wxComboBox::CanUndo() const
{
    if (m_text != NULL)
        return m_text->CanUndo();
    else
        return false;
}

bool wxComboBox::CanRedo() const
{
    if (m_text != NULL)
        return m_text->CanRedo();
    else
        return false;
}

void wxComboBox::MacHandleControlClick( WXWidget WXUNUSED(control) , wxInt16 WXUNUSED(controlpart) , bool WXUNUSED(mouseStillDown))
{
    wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, m_windowId );
    event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(GetStringSelection());
    ProcessCommand(event);
}
