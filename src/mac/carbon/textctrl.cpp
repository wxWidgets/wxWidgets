/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textctrl.h"
#endif

#ifdef __UNIX__
  #include <sys/types.h>
  #include <sys/stat.h>
#else
  #include <stat.h>
#endif
#include <fstream.h>

#include "wx/app.h"
#include "wx/dc.h"
#include "wx/button.h"
#include "wx/panel.h"
#include "wx/textctrl.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"
#include "wx/settings.h"
#include "wx/filefn.h"
#include "wx/utils.h"

#if defined(__BORLANDC__) && !defined(__WIN32__)
  #include <alloc.h>
#elif !defined(__MWERKS__) && !defined(__GNUWIN32) && !defined(__WXMAC_X__)
  #include <malloc.h>
#endif

#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
	EVT_DROP_FILES(wxTextCtrl::OnDropFiles)
	EVT_CHAR(wxTextCtrl::OnChar)
    EVT_MENU(wxID_CUT, wxTextCtrl::OnCut)
    EVT_MENU(wxID_COPY, wxTextCtrl::OnCopy)
    EVT_MENU(wxID_PASTE, wxTextCtrl::OnPaste)
    EVT_MENU(wxID_UNDO, wxTextCtrl::OnUndo)
    EVT_MENU(wxID_REDO, wxTextCtrl::OnRedo)

    EVT_UPDATE_UI(wxID_CUT, wxTextCtrl::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxTextCtrl::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxTextCtrl::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxTextCtrl::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxTextCtrl::OnUpdateRedo)
END_EVENT_TABLE()
#endif

// Text item
wxTextCtrl::wxTextCtrl()
{
}

const short kVerticalMargin = 2 ;
const short kHorizontalMargin = 2 ;

bool wxTextCtrl::Create(wxWindow *parent, wxWindowID id,
		   const wxString& st,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    // base initialization
    if ( !CreateBase(parent, id, pos, size, style, validator, name) )
        return FALSE;

	wxSize mySize = size ;
	if ( UMAHasAppearance() )
	{
		m_macHorizontalBorder = 5 ; // additional pixels around the real control
		m_macVerticalBorder = 5 ;
	}
	else
	{
		m_macHorizontalBorder = 0 ; // additional pixels around the real control
		m_macVerticalBorder = 0 ;
	}


	Rect bounds ;
	Str255 title ;

	if ( mySize.y == -1 )
	{
		if ( UMAHasAppearance() )
			mySize.y = 13 ;
		else
			mySize.y = 24 ;
		
		mySize.y += 2 * m_macVerticalBorder ;
	}

	MacPreControlCreate( parent , id ,  "" , pos , mySize ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , "\p" , true , 0 , 0 , 1, 
	  	( style & wxTE_PASSWORD ) ? kControlEditTextPasswordProc : kControlEditTextProc , (long) this ) ;
	MacPostControlCreate() ;

	wxString value ;
	
	{
 		TEHandle teH ;
   		long size ;
   
   		UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
		(*teH)->lineHeight = -1 ;
	}
	
	if( wxApp::s_macDefaultEncodingIsPC )
		value = wxMacMakeMacStringFromPC( st ) ;
	else
		value = st ;
	UMASetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , value.Length() , (char*) ((const char*)value) ) ;

  return TRUE;
}

wxString wxTextCtrl::GetValue() const
{
	Size actualsize;
	UMAGetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
	wxBuffer[actualsize] = 0 ;
	if( wxApp::s_macDefaultEncodingIsPC )
		return wxMacMakePCStringFromMac( wxBuffer ) ;
	else
    	return wxString(wxBuffer);
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;

    *from = (**teH).selStart;
    *to = (**teH).selEnd;
}

void wxTextCtrl::SetValue(const wxString& st)
{
	wxString value ;
	
	if( wxApp::s_macDefaultEncodingIsPC )
		value = wxMacMakeMacStringFromPC( st ) ;
	else
		value = st ;
	UMASetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , value.Length() , (char*) ((const char*)value) ) ;
	WindowRef window = GetMacRootWindow() ;
	if ( window )
	{
		wxWindow* win = wxFindWinFromMacWindow( window ) ;
		if ( win )
		{
			wxMacDrawingHelper help( win ) ;
			// the mac control manager always assumes to have the origin at 0,0
			SetOrigin( 0 , 0 ) ;
			
			bool			hasTabBehind = false ;
			wxWindow* parent = GetParent() ;
			while ( parent )
			{
				if( parent->MacGetWindowData() )
				{
					UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , kThemeBrushDialogBackgroundActive , false ) ;
					break ;
				}
				
				if( parent->IsKindOf( CLASSINFO( wxNotebook ) ) ||  parent->IsKindOf( CLASSINFO( wxTabCtrl ) ))
				{
					if ( ((wxControl*)parent)->GetMacControl() )
						SetUpControlBackground( ((wxControl*)parent)->GetMacControl() , -1 , true ) ;
					break ;
				}
				
				parent = parent->GetParent() ;
			} 
			
			UMADrawControl( m_macControl ) ;
			UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , win->MacGetWindowData()->m_macWindowBackgroundTheme , false ) ;
			wxDC::MacInvalidateSetup() ;
		}
	}
}

// Clipboard operations
void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
   		TEHandle teH ;
   		long size ;
   
  		 UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
		TECopy( teH ) ;
#if TARGET_CARBON
		OSStatus err ;
		err = ClearCurrentScrap( );
#else
		OSErr err ;
		err = ZeroScrap( );
#endif
		TEToScrap() ;
	}
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
   		TEHandle teH ;
   		long size ;
   
   		UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
		TECut( teH ) ;
#if TARGET_CARBON
		OSStatus err ;
		err = ClearCurrentScrap( );
#else
		OSErr err ;
		err = ZeroScrap( );
#endif
		TEToScrap() ;
		//	MacInvalidateControl() ;
	}
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
   		TEHandle teH ;
   		long size ;
   
   		UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
		TEFromScrap() ;
		TEPaste( teH ) ;
		WindowRef window = GetMacRootWindow() ;
		if ( window )
		{
			wxWindow* win = wxFindWinFromMacWindow( window ) ;
			if ( win )
			{
				wxMacDrawingHelper help( win ) ;
				// the mac control manager always assumes to have the origin at 0,0
				SetOrigin( 0 , 0 ) ;
				
				bool			hasTabBehind = false ;
				wxWindow* parent = GetParent() ;
				while ( parent )
				{
					if( parent->MacGetWindowData() )
					{
						UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , kThemeBrushDialogBackgroundActive , false ) ;
						break ;
					}
					
					if( parent->IsKindOf( CLASSINFO( wxNotebook ) ) ||  parent->IsKindOf( CLASSINFO( wxTabCtrl ) ))
					{
						if ( ((wxControl*)parent)->GetMacControl() )
							SetUpControlBackground( ((wxControl*)parent)->GetMacControl() , -1 , true ) ;
						break ;
					}
					
					parent = parent->GetParent() ;
				} 
				
				UMADrawControl( m_macControl ) ;
				UMASetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , win->MacGetWindowData()->m_macWindowBackgroundTheme , false ) ;
				wxDC::MacInvalidateSetup() ;
			}
		}
	}
}

bool wxTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanCut() const
{
    // Can cut if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanPaste() const
{
    if (!IsEditable())
        return FALSE;

	long offset ;
#if TARGET_CARBON
	OSStatus err = noErr;
	ScrapRef scrapRef;
	
	err = GetCurrentScrap( &scrapRef );
	if ( err != noTypeErr && err != memFullErr )	
	{
		ScrapFlavorFlags	flavorFlags;
		Size				byteCount;
		
		if (( err = GetScrapFlavorFlags( scrapRef, 'TEXT', &flavorFlags )) == noErr)
		{
			if (( err = GetScrapFlavorSize( scrapRef, 'TEXT', &byteCount )) == noErr)
			{
				return TRUE ;
			}
		}
	}
	return FALSE;
	
#else
	if ( GetScrap( NULL , 'TEXT' , &offset ) > 0 )
	{
		return TRUE ;
	}
#endif
	return FALSE ;
}

void wxTextCtrl::SetEditable(bool editable)
{
    if ( editable )
    	UMAActivateControl( m_macControl ) ;
    else
    	UMADeactivateControl( m_macControl ) ;
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
	SetSelection( pos , pos ) ;
}

void wxTextCtrl::SetInsertionPointEnd()
{
    long pos = GetLastPosition();
    SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
//   UMAGetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection , &size ) ;
    return (**teH).selStart ;
}

long wxTextCtrl::GetLastPosition() const
{
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
//   UMAGetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection , &size ) ;
    return (**teH).teLength ;
}

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
   	TEHandle teH ;
   	long size ;
   
   	ControlEditTextSelectionRec selection ;
   
   	selection.selStart = from ;
   	selection.selEnd = to ;
   	UMASetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
		UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   	TESetSelect( from , to  , teH ) ;
   	TEDelete( teH ) ;
		TEInsert( value , value.Length() , teH ) ;
//	MacInvalidateControl() ;
}

void wxTextCtrl::Remove(long from, long to)
{
   	TEHandle teH ;
   	long size ;
   
   	ControlEditTextSelectionRec selection ;
   
   	selection.selStart = from ;
   	selection.selEnd = to ;
   	UMASetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
	UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   	TEDelete( teH ) ;
//	MacInvalidateControl() ;
}

void wxTextCtrl::SetSelection(long from, long to)
{
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
   selection.selStart = from ;
   selection.selEnd = to ;
   
   UMASetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
   TESetSelect( selection.selStart , selection.selEnd , teH ) ;
}

bool wxTextCtrl::LoadFile(const wxString& file)
{
    if ( wxTextCtrlBase::LoadFile(file) )
    {
        return TRUE;
    }

    return FALSE;
}

void wxTextCtrl::WriteText(const wxString& text)
{
    TEHandle teH ;
    long size ;
   
   	memcpy( wxBuffer, text , text.Length() ) ;
   	wxBuffer[text.Length() ] = 0 ;
//    wxMacConvertNewlines( wxBuffer , wxBuffer ) ;
   
    UMAGetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
		TEInsert( wxBuffer , strlen( wxBuffer) , teH ) ;
		Refresh() ;
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Clear()
{
	UMASetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 0 , (char*) ((const char*)NULL) ) ;
	Refresh() ;
}

bool wxTextCtrl::IsModified() const
{
    return TRUE;
}

bool wxTextCtrl::IsEditable() const
{
    return IsEnabled();
}

bool wxTextCtrl::AcceptsFocus() const
{
    // we don't want focus if we can't be edited
    return IsEditable() && wxControl::AcceptsFocus();
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    int wText = 100 ;
	
    int hText ;
		if ( UMAHasAppearance() )
			hText = 13 ;
		else
			hText = 24 ;
	hText += 2 * m_macHorizontalBorder ;
/*
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    int wText = DEFAULT_ITEM_WIDTH;

    int hText = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    return wxSize(wText, hText);
*/
    if ( m_windowStyle & wxTE_MULTILINE )
    {
        hText *= wxMin(GetNumberOfLines(), 5);
    }
    //else: for single line control everything is ok
    return wxSize(wText, hText);
}

// ----------------------------------------------------------------------------
// Undo/redo
// ----------------------------------------------------------------------------

void wxTextCtrl::Undo()
{
    if (CanUndo())
    {
    }
}

void wxTextCtrl::Redo()
{
    if (CanRedo())
    {
    }
}

bool wxTextCtrl::CanUndo() const
{
    return FALSE ;
}

bool wxTextCtrl::CanRedo() const
{
    return FALSE ;
}

// Makes 'unmodified'
void wxTextCtrl::DiscardEdits()
{
    // TODO
}

int wxTextCtrl::GetNumberOfLines() const
{
    // TODO
    return 0;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    // TODO
    return 0;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    return FALSE ;
}

void wxTextCtrl::ShowPosition(long pos)
{
    // TODO
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    return GetValue().Length();
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    return GetValue();
}

/*
 * Text item
 */
 
void wxTextCtrl::Command(wxCommandEvent & event)
{
    SetValue (event.GetString());
    ProcessCommand (event);
}

void wxTextCtrl::OnDropFiles(wxDropFilesEvent& event)
{
    // By default, load the first file into the text window.
    if (event.GetNumberOfFiles() > 0)
    {
        LoadFile(event.GetFiles()[0]);
    }
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case WXK_RETURN:
        	if (m_windowStyle & wxPROCESS_ENTER)
        	{
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
        	} 
            if ( !(m_windowStyle & wxTE_MULTILINE) )
            {
    			wxWindow *parent = GetParent();
    			wxPanel *panel = wxDynamicCast(parent, wxPanel);
    			while ( parent != NULL && panel == NULL )
    			{
    				parent = parent->GetParent() ;
    				panel = wxDynamicCast(parent, wxPanel);
    			}
           		if ( panel && panel->GetDefaultItem() )
           		{
           			wxButton *def = panel->GetDefaultItem() ;
    				wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
   					event.SetEventObject(def);
   					def->Command(event);
               		event.Skip() ;
                	return ;
           		}
            }
            //else: multiline controls need Enter for themselves

            break;

        case WXK_TAB:
            // always produce navigation event - even if we process TAB
            // ourselves the fact that we got here means that the user code
            // decided to skip processing of this TAB - probably to let it
            // do its default job.
            {
                wxNavigationKeyEvent eventNav;
                eventNav.SetDirection(!event.ShiftDown());
                eventNav.SetWindowChange(event.ControlDown());
                eventNav.SetEventObject(this);

                if ( GetParent()->GetEventHandler()->ProcessEvent(eventNav) )
                    return;
               	event.Skip() ;
                return ;
            }
            break;
    }

	EventRecord *ev = wxTheApp->MacGetCurrentEvent() ;
	short keycode ;
	short keychar ;
	keychar = short(ev->message & charCodeMask);
	keycode = short(ev->message & keyCodeMask) >> 8 ;
	UMAHandleControlKey( m_macControl , keycode , keychar , ev->modifiers ) ;
	if ( keychar >= 0x20 || event.KeyCode() == WXK_RETURN)
	{
        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event.SetString( GetValue() ) ;
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
	}

}

// ----------------------------------------------------------------------------
// standard handlers for standard edit menu events
// ----------------------------------------------------------------------------

void wxTextCtrl::OnCut(wxCommandEvent& event)
{
    Cut();
}

void wxTextCtrl::OnCopy(wxCommandEvent& event)
{
    Copy();
}

void wxTextCtrl::OnPaste(wxCommandEvent& event)
{
    Paste();
}

void wxTextCtrl::OnUndo(wxCommandEvent& event)
{
    Undo();
}

void wxTextCtrl::OnRedo(wxCommandEvent& event)
{
    Redo();
}

void wxTextCtrl::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxTextCtrl::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxTextCtrl::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxTextCtrl::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
}

void wxTextCtrl::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
}

