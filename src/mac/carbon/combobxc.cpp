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

#ifdef __GNUG__
#pragma implementation "combobox.h"
#endif

#include "wx/combobox.h"
#include "wx/button.h"
#include "wx/menu.h"
#include "wx/mac/uma.h"
#ifndef __HIVIEW__
	#include <HIToolbox/HIView.h>
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)
#endif

// composite combobox implementation by Dan "Bud" Keith bud@otsys.com

#define USE_HICOMBOBOX 1 //use hi combobox define

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
#if TARGET_API_MAC_OSX
static const int    POPUPWIDTH = 24;
#else
static const int    POPUPWIDTH = 18;
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
        wxString    s = e.GetString();

        m_cb->DelegateChoice( s );
        wxCommandEvent event2(wxEVT_COMMAND_COMBOBOX_SELECTED, m_cb->GetId() );
        event2.SetInt(m_cb->GetSelection());
        event2.SetEventObject(m_cb);
        event2.SetString(m_cb->GetStringSelection());
        m_cb->ProcessCommand(event2);
    }
    virtual wxSize DoGetBestSize() const
    {
        wxSize sz = wxChoice::DoGetBestSize() ;
        sz.x = POPUPWIDTH ;
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
#if USE_HICOMBOBOX
	return wxControl::DoGetBestSize();
#else
    wxSize size = m_choice->GetBestSize();
    
    if ( m_text != NULL )
    {
        wxSize  sizeText = m_text->GetBestSize();
        
        size.x = POPUPWIDTH + sizeText.x + MARGIN;
    }

    return size;
#endif
}

void wxComboBox::DoMoveWindow(int x, int y, int width, int height) {
#if USE_HICOMBOBOX
	wxControl::DoMoveWindow(x, y, width, height);
#else
    height = POPUPHEIGHT;
    
    wxControl::DoMoveWindow(x, y, width, height);

    if ( m_text == NULL )
    {
        // we might not be fully constructed yet, therefore watch out...
        if ( m_choice )
            m_choice->SetSize(0, 0 , width, -1);
    }
    else
    {
        wxCoord wText = width - POPUPWIDTH - MARGIN;
        m_text->SetSize(0, 0, wText, height);
        m_choice->SetSize(0 + wText + MARGIN, 0, POPUPWIDTH, -1);
    }
#endif    
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
#if USE_HICOMBOBOX
	wxControl::SetFocus();
#else
    if ( m_text != NULL) {
        m_text->SetFocus();
    }
#endif
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
    m_text = NULL;
    m_choice = NULL;
#if USE_HICOMBOBOX
    m_macIsUserPane = FALSE ;
#endif
    if ( !wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize, style ,
                            wxDefaultValidator, name) )
    {
        return FALSE;
    }
#if USE_HICOMBOBOX
    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    HIRect hiRect;
    
    hiRect.origin.x = 20; //bounds.left;
    hiRect.origin.y = 25; //bounds.top;
    hiRect.size.width = 120;// bounds.right - bounds.left;
    hiRect.size.height = 24; 
	
    //For some reason, this code causes the combo box not to be displayed at all.
    //hiRect.origin.x = bounds.left;
    //hiRect.origin.y = bounds.top;
    //hiRect.size.width = bounds.right - bounds.left;
    //hiRect.size.height = bounds.bottom - bounds.top;
    //printf("left = %d, right = %d, top = %d, bottom = %d\n", bounds.left, bounds.right, bounds.top, bounds.bottom);
	//printf("x = %d, y = %d, width = %d, height = %d\n", hibounds.origin.x, hibounds.origin.y, hibounds.size.width, hibounds.size.height);
    verify_noerr( HIComboBoxCreate( &hiRect, CFSTR(""), NULL, NULL, kHIComboBoxStandardAttributes, (HIViewRef*) &m_macControl) );

    SetControl32BitMinimum( (ControlRef) m_macControl , 0 ) ;
    SetControl32BitMaximum( (ControlRef) m_macControl , 100) ;
    if ( n > 0 )
        SetControl32BitValue( (ControlRef) m_macControl , 1 ) ;
    
    MacPostControlCreate(pos,size) ;
    
    for ( int i = 0 ; i < n ; i++ )
    {
        DoAppend( choices[ i ] );
    }
    
    HIViewSetVisible( (HIViewRef) m_macControl, true );
    SetSelection(0);
#else
    m_choice = new wxComboBoxChoice(this, style );

    m_choice = new wxComboBoxChoice(this, style );
    m_choice->SetSizeHints( wxSize( POPUPWIDTH , POPUPHEIGHT ) ) ;
    
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
    SetBestSize(csize);   // Needed because it is a wxControlWithItems
#endif

    return TRUE;
}

wxString wxComboBox::GetValue() const
{
#if USE_HICOMBOBOX
    CFStringRef myString;
    HIComboBoxCopyTextItemAtIndex( (HIViewRef) m_macControl, (CFIndex)GetSelection(), &myString );
    return wxMacCFStringHolder( myString, m_font.GetEncoding() ).AsString();
#else
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
#endif
}

void wxComboBox::SetValue(const wxString& value)
{
#if USE_HICOMBOBOX
    
#else
    int s = FindString (value);
    if (s == wxNOT_FOUND && !HasFlag(wxCB_READONLY) )
    {
        m_choice->Append(value) ;
    }
    SetStringSelection( value ) ;
#endif
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

int wxComboBox::DoAppend(const wxString& item) 
{
#if USE_HICOMBOBOX
    CFIndex outIndex;
    HIComboBoxAppendTextItem( (HIViewRef) m_macControl, wxMacCFStringHolder( item, m_font.GetEncoding() ), &outIndex );
    //SetControl32BitMaximum( (HIViewRef) m_macControl, GetCount() );
    return (int) outIndex;
#else
    return m_choice->DoAppend( item ) ;
#endif
}

int wxComboBox::DoInsert(const wxString& item, int pos) 
{
#if USE_HICOMBOBOX
    HIComboBoxInsertTextItemAtIndex( (HIViewRef) m_macControl, (CFIndex)pos, wxMacCFStringHolder(item, m_font.GetEncoding()) );
    
    //SetControl32BitMaximum( (HIViewRef) m_macControl, GetCount() );
    
    return pos;
#else
    return m_choice->DoInsert( item , pos ) ;
#endif
}

void wxComboBox::DoSetItemClientData(int n, void* clientData) 
{
#if USE_HICOMBOBOX
    return; //TODO
#else
    return m_choice->DoSetItemClientData( n , clientData ) ;
#endif
}

void* wxComboBox::DoGetItemClientData(int n) const
{
#if USE_HICOMBOBOX
    return NULL; //TODO
#else
    return m_choice->DoGetItemClientData( n ) ;
#endif
}

void wxComboBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
#if USE_HICOMBOBOX
    return; //TODO
#else
    return m_choice->DoSetItemClientObject( n , clientData ) ;
#endif
}

wxClientData* wxComboBox::DoGetItemClientObject(int n) const 
{
#if USE_HICOMBOBOX
    return NULL;
#else
    return m_choice->DoGetItemClientObject( n ) ;
#endif
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

int wxComboBox::GetCount() const {
#if USE_HICOMBOBOX
	return (int) HIComboBoxGetItemCount( (HIViewRef) m_macControl );
#else
	return m_choice->GetCount() ; 
#endif
}

void wxComboBox::Delete(int n)
{
#if USE_HICOMBOBOX
    HIComboBoxRemoveItemAtIndex( (HIViewRef) m_macControl, (CFIndex)n );
#else
    // force client object deletion
    if( HasClientObjectData() )
        SetClientObject( n, NULL );
    m_choice->Delete( n );
#endif
}

void wxComboBox::Clear()
{
#if USE_HICOMBOBOX
    //TODO
#else
    FreeData();
    m_choice->Clear();
#endif
}

int wxComboBox::GetSelection() const
{
#if USE_HICOMBOBOX
    int result =  GetControl32BitValue( (HIViewRef) m_macControl ) -1;
    return result;
#else
    return m_choice->GetSelection();
#endif
}

void wxComboBox::SetSelection(int n)
{
#if USE_HICOMBOBOX
    SetControl32BitValue( (ControlRef) m_macControl , n + 1 ) ;
#else
    m_choice->SetSelection( n );
    
    if ( m_text != NULL )
    {
        m_text->SetValue( GetString( n ) );
    }
#endif
}

int wxComboBox::FindString(const wxString& s) const
{
#if USE_HICOMBOBOX
    for( int i = 0 ; i < GetCount() ; i++ )
    {
        if ( GetString( i ).IsSameAs(s, FALSE) )
            return i ;
    }
    return wxNOT_FOUND ;
#else
    return m_choice->FindString( s );
#endif
}

wxString wxComboBox::GetString(int n) const
{
#if USE_HICOMBOBOX
    CFStringRef itemText;
    HIComboBoxCopyTextItemAtIndex( (HIViewRef) m_macControl, (CFIndex)n, &itemText );
    return wxMacCFStringHolder(itemText).AsString();
#else
    return m_choice->GetString( n );
#endif
}

wxString wxComboBox::GetStringSelection() const
{
    int sel = GetSelection ();
    if (sel > -1)
        return wxString(this->GetString (sel));
    else
        return wxEmptyString;
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

void wxComboBox::SetString(int n, const wxString& s) 
{
#if USE_HICOMBOBOX

#else
    m_choice->SetString( n , s ) ;
#endif
}


wxInt32 wxComboBox::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) ) 
{
/*
    wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, m_windowId );
    event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(GetStringSelection());
    ProcessCommand(event);
    return noErr ;
*/
    return eventNotHandledErr ;
}

