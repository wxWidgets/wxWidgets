/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:     wxComboBox class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "combobox.h"
#endif

#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"
#include "wx/button.h"
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
#if TARGET_API_MAC_OSX
// margin should be bigger on OS X due to blue highlight
// around text control.
static const wxCoord MARGIN = 4;
// this is the border a focus rect on OSX is needing
static const int    TEXTFOCUSBORDER = 3 ;
#else
static const wxCoord MARGIN = 2;
static const int    TEXTFOCUSBORDER = 0 ;
#endif
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
        // Allows processing the tab key to go to the next control
        if (event.GetKeyCode() == WXK_TAB)
        {
            wxNavigationKeyEvent NavEvent;
            NavEvent.SetEventObject(this);
            NavEvent.SetDirection(true);
            NavEvent.SetWindowChange(false);

            // Get the parent of the combo and have it process the navigation?
            if (m_cb->GetParent()->GetEventHandler()->ProcessEvent(NavEvent))
                    return;
        }
        if ( event.GetKeyCode() == WXK_RETURN )
        {
            wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_cb->GetId());
            event.SetString( GetValue() );
            event.SetInt( m_cb->GetSelection() );
            event.SetEventObject( m_cb );

            // This will invoke the dialog default action, such
            // as the clicking the default button.

            if (!m_cb->GetEventHandler()->ProcessEvent( event ))
            {
                wxWindow *parent = GetParent();
                while( parent && !parent->IsTopLevel() && parent->GetDefaultItem() == NULL ) {
                    parent = parent->GetParent() ;
                }
                if ( parent && parent->GetDefaultItem() )
                {
                    wxButton *def = wxDynamicCast(parent->GetDefaultItem(),
                                                          wxButton);
                    if ( def && def->IsEnabled() )
                    {
                        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
                        event.SetEventObject(def);
                        def->Command(event);
                   }
                }

                return;
            }
        }

        event.Skip();
    }

    // Use the KeyUp as a naive approximation for TEXT_UPDATED, even though it is somewhat delayed
    // but this is less complicated than dealing with idle-ness, and is much better than nothing
    void OnKeyUp( wxKeyEvent& event )
    {
        if ( event.GetKeyCode() != WXK_RETURN && event.GetKeyCode() != WXK_TAB )
        {
            wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_cb->GetId());
            event.SetString( GetValue() );
            event.SetEventObject( m_cb );
            m_cb->GetEventHandler()->ProcessEvent(event);
        }
    }
private:
    wxComboBox *m_cb;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxComboBoxText, wxTextCtrl)
    EVT_CHAR( wxComboBoxText::OnChar)
    EVT_KEY_UP( wxComboBoxText::OnKeyUp)
END_EVENT_TABLE()

class wxComboBoxChoice : public wxChoice
{
public:
    wxComboBoxChoice(wxComboBox *cb, int style)
        : wxChoice( cb , 1 )
    {
        m_cb = cb;
    }
    int GetPopupWidth() const
    {
        switch ( GetWindowVariant() )
        {
            case wxWINDOW_VARIANT_NORMAL :
            case wxWINDOW_VARIANT_LARGE :
                return 24 ;
            default :
                return 21 ;
        }
    }

protected:
    void OnChoice( wxCommandEvent& e )
    {
        wxString    s = e.GetString();

        m_cb->DelegateChoice( s );
        wxCommandEvent event2(wxEVT_COMMAND_COMBOBOX_SELECTED, m_cb->GetId() );
        event2.SetInt(m_cb->GetSelection());
        event2.SetEventObject(m_cb);
        event2.SetString(m_cb->GetStringSelection());
        m_cb->ProcessCommand(event2);

        // For consistency with MSW and GTK, also send a text updated event
        // After all, the text is updated when a selection is made
        wxCommandEvent TextEvent( wxEVT_COMMAND_TEXT_UPDATED, m_cb->GetId() );
        TextEvent.SetString( m_cb->GetStringSelection() );
        TextEvent.SetEventObject( m_cb );
        m_cb->ProcessCommand( TextEvent );
    }
    virtual wxSize DoGetBestSize() const
    {
        wxSize sz = wxChoice::DoGetBestSize() ;
        if (! m_cb->HasFlag(wxCB_READONLY) )
            sz.x = GetPopupWidth() ;
        return sz ;
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
    if (!m_choice && !m_text)
        return GetSize();
    wxSize size = m_choice->GetBestSize();

    if ( m_text != NULL )
    {
        wxSize  sizeText = m_text->GetBestSize();
        if (sizeText.y > size.y)
            size.y = sizeText.y;
        size.x = m_choice->GetPopupWidth() + sizeText.x + MARGIN;
        size.x += TEXTFOCUSBORDER ;
        size.y += 2 * TEXTFOCUSBORDER ;
    }
    else
    {
        // clipping is too tight
        size.y += 1 ;
    }
    return size;
}

void wxComboBox::DoMoveWindow(int x, int y, int width, int height)
{
    wxControl::DoMoveWindow(x, y, width , height );

    if ( m_text == NULL )
    {
        // we might not be fully constructed yet, therefore watch out...
        if ( m_choice )
            m_choice->SetSize(0, 0 , width, -1);
    }
    else
    {
        wxCoord wText = width - m_choice->GetPopupWidth() - MARGIN;
        m_text->SetSize(TEXTFOCUSBORDER, TEXTFOCUSBORDER, wText, -1 );
        // put it at an inset of 1 to have outer area shadows drawn as well
        m_choice->SetSize(TEXTFOCUSBORDER + wText + MARGIN - 1 , TEXTFOCUSBORDER, m_choice->GetPopupWidth() , -1);
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
        if ( size.y == -1 )
        {
            csize.y = m_text->GetSize().y ;
            csize.y += 2 * TEXTFOCUSBORDER ;
        }
    }

    DoSetSize(pos.x, pos.y, csize.x, csize.y);

    for ( int i = 0 ; i < n ; i++ )
    {
        m_choice->DoAppend( choices[ i ] );
    }

    SetBestSize(size);   // Needed because it is a wxControlWithItems
    SetStringSelection(value);
    
    return true;
}

wxString wxComboBox::GetValue() const
{
    wxString        result;

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

int wxComboBox::GetCount() const
{
    return m_choice->GetCount() ;
}

void wxComboBox::SetValue(const wxString& value)
{
    if ( HasFlag(wxCB_READONLY) )
        SetStringSelection( value ) ;
    else
        m_text->SetValue( value );
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

int wxComboBox::DoInsert(const wxString& item, int pos)
{
    return m_choice->DoInsert( item , pos ) ;
}

void wxComboBox::DoSetItemClientData(int n, void* clientData)
{
    return m_choice->DoSetItemClientData( n , clientData ) ;
}

void* wxComboBox::DoGetItemClientData(int n) const
{
    return m_choice->DoGetItemClientData( n ) ;
}

void wxComboBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    return m_choice->DoSetItemClientObject( n , clientData ) ;
}

wxClientData* wxComboBox::DoGetItemClientObject(int n) const
{
    return m_choice->DoGetItemClientObject( n ) ;
}

void wxComboBox::FreeData()
{
    if ( HasClientObjectData() )
    {
        size_t count = GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            SetClientObject( n, NULL );
        }
    }
}

void wxComboBox::Delete(int n)
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
        return wxEmptyString;
}

void wxComboBox::SetString(int n, const wxString& s)
{
    m_choice->SetString( n , s ) ;
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

wxInt32 wxComboBox::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) )
{
    /* For consistency with other platforms, clicking in the text area does not constitute a selection
    wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, m_windowId );
    event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(GetStringSelection());
    ProcessCommand(event);*/
    return noErr ;
}

#endif
