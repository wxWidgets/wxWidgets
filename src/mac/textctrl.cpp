/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textctrl.h"
#endif

#include "wx/defs.h"

#if wxUSE_TEXTCTRL

#ifdef __DARWIN__
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
#elif !defined(__MWERKS__) && !defined(__GNUWIN32) && !defined(__DARWIN__)
  #include <malloc.h>
#endif

#include "wx/mac/uma.h"

#define wxUSE_MLTE 0

#if wxUSE_MLTE == 0 // old textctrl implementation

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

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        wxASSERT_MSG( !(m_windowStyle & wxTE_PROCESS_ENTER),
                      wxT("wxTE_PROCESS_ENTER style is ignored for multiline text controls (they always process it)") );

        m_windowStyle |= wxTE_PROCESS_ENTER;
    }


    m_macControl = ::NewControl( parent->GetMacRootWindow() , &bounds , "\p" , true , 0 , 0 , 1, 
        ( style & wxTE_PASSWORD ) ? kControlEditTextPasswordProc : kControlEditTextProc , (long) this ) ;
    MacPostControlCreate() ;

    wxString value ;
    
    {
        TEHandle teH ;
        long size ;
   
        ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
        (*teH)->lineHeight = -1 ;
    }
    
    if( wxApp::s_macDefaultEncodingIsPC )
        value = wxMacMakeMacStringFromPC( st ) ;
    else
        value = st ;
    ::SetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , value.Length() , (char*) ((const char*)value) ) ;

  return TRUE;
}

wxString wxTextCtrl::GetValue() const
{
    Size actualsize;
    ::GetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
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
   
   ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;

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
    ::SetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , value.Length() , (char*) ((const char*)value) ) ;
    WindowRef window = GetMacRootWindow() ;
    if ( window )
    {
        wxWindow* win = wxFindWinFromMacWindow( window ) ;
        if ( win )
        {
            wxMacDrawingHelper help( win ) ;
            // the mac control manager always assumes to have the origin at 0,0
            SetOrigin( 0 , 0 ) ;
            
            bool            hasTabBehind = false ;
            wxWindow* parent = GetParent() ;
            while ( parent )
            {
                if( parent->MacGetWindowData() )
                {
                    ::SetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , kThemeBrushDialogBackgroundActive , false ) ;
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
            ::SetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , win->MacGetWindowData()->m_macWindowBackgroundTheme , false ) ;
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
   
         ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
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
   
        ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
        TECut( teH ) ;
#if TARGET_CARBON
        OSStatus err ;
        err = ClearCurrentScrap( );
#else
        OSErr err ;
        err = ZeroScrap( );
#endif
        TEToScrap() ;
        //  MacInvalidateControl() ;
    }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        TEHandle teH ;
        long size ;
   
        ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
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
                
                bool            hasTabBehind = false ;
                wxWindow* parent = GetParent() ;
                while ( parent )
                {
                    if( parent->MacGetWindowData() )
                    {
                        ::SetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , kThemeBrushDialogBackgroundActive , false ) ;
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
                ::SetThemeWindowBackground( win->MacGetWindowData()->m_macWindow , win->MacGetWindowData()->m_macWindowBackgroundTheme , false ) ;
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
        ScrapFlavorFlags    flavorFlags;
        Size                byteCount;
        
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
   
   ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
//   ::GetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection , &size ) ;
    return (**teH).selStart ;
}

long wxTextCtrl::GetLastPosition() const
{
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
//   ::GetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection , &size ) ;
    return (**teH).teLength ;
}

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
    TEHandle teH ;
    long size ;
   
    ControlEditTextSelectionRec selection ;
   
    selection.selStart = from ;
    selection.selEnd = to ;
    ::SetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
        ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
    TESetSelect( from , to  , teH ) ;
    TEDelete( teH ) ;
        TEInsert( value , value.Length() , teH ) ;
    Refresh() ;
}

void wxTextCtrl::Remove(long from, long to)
{
    TEHandle teH ;
    long size ;
   
    ControlEditTextSelectionRec selection ;
   
    selection.selStart = from ;
    selection.selEnd = to ;
    ::SetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
    ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
    TEDelete( teH ) ;
    Refresh() ;
}

void wxTextCtrl::SetSelection(long from, long to)
{
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
   selection.selStart = from ;
   selection.selEnd = to ;
   
   ::SetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
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
   
    ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
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
    ::SetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 0 , (char*) ((const char*)NULL) ) ;
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
    Size actualsize;
    ::GetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
    
    int count = 1;
    for (int i = 0; i < actualsize; i++)
    {
        if (wxBuffer[i] == '\r') count++;
    }
    
    return count;
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
    Size actualsize;
    ::GetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
    
    // Find line first
    int count = 0;
    for (int i = 0; i < actualsize; i++)
    {
        if (count == lineNo)
        {
            // Count chars in line then
            count = 0;
            for (int j = i; j < actualsize; j++)
            {
                count++;
                if (wxBuffer[j] == '\r') return count;
            }
            
            return count;
        }
        if (wxBuffer[i] == '\r') count++;
    }
    
    return 0;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    Size actualsize;
    ::GetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
    
    // Find line first
    int count = 0;
    for (int i = 0; i < actualsize; i++)
    {
        if (count == lineNo)
        {
            // Add chars in line then
            wxString tmp("");
            
            for (int j = i; j < actualsize; j++)
            {
                if (wxBuffer[j] == '\r')
                    return tmp;
                    
                tmp += wxBuffer[j];
            }
            
            return tmp;
        }
        if (wxBuffer[i] == '\r') count++;
    }
    
    return wxString("");
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

void wxTextCtrl::OnChar(wxKeyEvent& key_event)
{
    bool eat_key = FALSE;

    switch ( key_event.KeyCode() )
    {
        case WXK_RETURN:
            if (m_windowStyle & wxPROCESS_ENTER)
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                event.SetString( GetValue() );
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
                    wxButton *def = wxDynamicCast(panel->GetDefaultItem(),
                                                          wxButton);
                    if ( def && def->IsEnabled() )
                    {
                        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
                        event.SetEventObject(def);
                        def->Command(event);
                        return ;
                   }                              
                }
                
                // this will make wxWindows eat the ENTER key so that
                // we actually prevent line wrapping in a single line
                // text control
                eat_key = TRUE;
            }

            break;

        case WXK_TAB:
            // always produce navigation event - even if we process TAB
            // ourselves the fact that we got here means that the user code
            // decided to skip processing of this TAB - probably to let it
            // do its default job.
            {
                wxNavigationKeyEvent eventNav;
                eventNav.SetDirection(!key_event.ShiftDown());
                eventNav.SetWindowChange(key_event.ControlDown());
                eventNav.SetEventObject(this);

                if ( GetParent()->GetEventHandler()->ProcessEvent(eventNav) )
                    return;

                key_event.Skip() ;
                return;
            }
            break;
    }
    
    EventRecord *ev = wxTheApp->MacGetCurrentEvent();
    short keychar = short(ev->message & charCodeMask);
    if (!eat_key)
    {
        short keycode = short(ev->message & keyCodeMask) >> 8 ;
        ::HandleControlKey( m_macControl , keycode , keychar , ev->modifiers );
    }
    if ( keychar >= 0x20 ||
         key_event.KeyCode() == WXK_RETURN ||
         key_event.KeyCode() == WXK_DELETE || 
         key_event.KeyCode() == WXK_BACK)
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

#else

#if !TARGET_CARBON
#define GetControlOwner( control ) (**control).contrlOwner
#endif

//todo add access to global event record

EventRecord event ;

static EventRecord *GetCurrentEventRecord()
{
  return &event ;
}

// CS:We will replace the TextEdit by using the MultiLanguageTextEngine based on the following code written by apple

/*
    File: mUPControl.c
    
    Description:
        mUPControl implementation.

    Copyright:
        © Copyright 2000 Apple Computer, Inc. All rights reserved.
    
    Disclaimer:
        IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
        ("Apple") in consideration of your agreement to the following terms, and your
        use, installation, modification or redistribution of this Apple software
        constitutes acceptance of these terms.  If you do not agree with these terms,
        please do not use, install, modify or redistribute this Apple software.

        In consideration of your agreement to abide by the following terms, and subject
        to these terms, Apple grants you a personal, non-exclusive license, under AppleÕs
        copyrights in this original Apple software (the "Apple Software"), to use,
        reproduce, modify and redistribute the Apple Software, with or without
        modifications, in source and/or binary forms; provided that if you redistribute
        the Apple Software in its entirety and without modifications, you must retain
        this notice and the following text and disclaimers in all such redistributions of
        the Apple Software.  Neither the name, trademarks, service marks or logos of
        Apple Computer, Inc. may be used to endorse or promote products derived from the
        Apple Software without specific prior written permission from Apple.  Except as
        expressly stated in this notice, no other rights or licenses, express or implied,
        are granted by Apple herein, including but not limited to any patent rights that
        may be infringed by your derivative works or by other works in which the Apple
        Software may be incorporated.

        The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
        WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
        WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
        PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
        COMBINATION WITH YOUR PRODUCTS.

        IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
        CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
        GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
        ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
        OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
        (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
        ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Change History (most recent first):
        Fri, Jan 28, 2000 -- created
*/

#include "MacTextEditor.h"

/* part codes */

/* kmUPTextPart is the part code we return to indicate the user has clicked
    in the text area of our control */
#define kmUPTextPart 1

/* kmUPScrollPart is the part code we return to indicate the user has clicked
    in the scroll bar part of the control. */
#define kmUPScrollPart 2


/* routines for using existing user pane controls.
    These routines are useful for cases where you would like to use an
    existing user pane control in, say, a dialog window as a scrolling
    text edit field.*/
    
/* mUPOpenControl initializes a user pane control so it will be drawn
    and will behave as a scrolling text edit field inside of a window.
    This routine performs all of the initialization steps necessary,
    except it does not create the user pane control itself.  theControl
    should refer to a user pane control that you have either created
    yourself or extracted from a dialog's control heirarchy using
    the GetDialogItemAsControl routine.  */
OSStatus mUPOpenControl(ControlHandle theControl);

/* mUPCloseControl deallocates all of the structures allocated
    by mUPOpenControl.  */
OSStatus mUPCloseControl(ControlHandle theControl);



/* routines for creating new scrolling text user pane controls.
    These routines allow you to create new scrolling text
    user pane controls. */

/* mUPCreateControl creates a new user pane control and then it passes it
    to mUPOpenControl to initialize it as a scrolling text user pane control. */
OSStatus mUPCreateControl(WindowPtr theWindow, Rect *bounds, ControlHandle *theControl);

/* mUPDisposeControl calls mUPCloseControl and then it calls DisposeControl. */
OSStatus mUPDisposeControl(ControlHandle theControl);


/* Utility Routines */

    /* mUPSetText replaces the contents of the selection with the unicode
    text described by the text and count parameters:.
        text = pointer to unicode text buffer
        count = number of bytes in the buffer.  */
OSStatus mUPSetText(ControlHandle theControl, char* text, long count);

/* mUPGetText returns the current text data being displayed inside of
    the mUPControl.  When noErr is returned, *theText contain a new
    handle containing all of the Unicode text copied from the current
    selection.  It is the caller's responsibiliby to dispose of this handle. */
OSStatus mUPGetText(ControlHandle theControl, Handle *theText);


/* mUPSetSelection sets the text selection and autoscrolls the text view
    so either the cursor or the selction is in the view. */
void mUPSetSelection(ControlHandle theControl, long selStart, long selEnd);



/* IsmUPControl returns true if theControl is not NULL
    and theControl refers to a mUP Control.  */
Boolean IsmUPControl(ControlHandle theControl);



/* Edit commands for mUP Controls. */
enum {
    kmUPCut = 1,
    kmUPCopy = 2,
    kmUPPaste = 3,
    kmUPClear = 4
};


/* mUPDoEditCommand performs the editing command specified
    in the editCommand parameter.  The mUPControl's text
    and scroll bar are redrawn and updated as necessary. */
void mUPDoEditCommand(ControlHandle theControl, short editCommand);




/* mUPGetContents returns the entire contents of the control including the text
    and the formatting information. */
OSStatus mUPGetContents(ControlHandle theControl, Handle *theContents);
/* mUPSetContents replaces the contents of the selection with the data stored in the handle. */
OSStatus mUPSetContents(ControlHandle theControl, Handle theContents);

enum {
    kShiftKeyCode = 56
};

/* kUserClickedToFocusPart is a part code we pass to the SetKeyboardFocus
    routine.  In our focus switching routine this part code is understood
    as meaning 'the user has clicked in the control and we need to switch
    the current focus to ourselves before we can continue'. */
#define kUserClickedToFocusPart 100


/* kmUPClickScrollDelayTicks is a time measurement in ticks used to
    slow the speed of 'auto scrolling' inside of our clickloop routine.
    This value prevents the text from wizzzzzing by while the mouse
    is being held down inside of the text area. */
#define kmUPClickScrollDelayTicks 3


/* STPTextPaneVars is a structure used for storing the the mUP Control's
    internal variables and state information.  A handle to this record is
    stored in the pane control's reference value field using the
    SetControlReference routine. */

typedef struct {
        /* OS records referenced */
    TXNObject fTXNRec; /* the txn record */
    TXNFrameID fTXNFrame; /* the txn frame ID */
    ControlHandle fUserPaneRec;  /* handle to the user pane control */
    WindowPtr fOwner; /* window containing control */
    GrafPtr fDrawingEnvironment; /* grafport where control is drawn */
        /* flags */
    Boolean fInFocus; /* true while the focus rect is drawn around the control */
    Boolean fIsActive; /* true while the control is drawn in the active state */
    Boolean fTEActive; /* reflects the activation state of the text edit record */ 
    Boolean fInDialogWindow; /* true if displayed in a dialog window */ 
        /* calculated locations */
    Rect fRTextArea; /* area where the text is drawn */
    Rect fRFocusOutline;  /* rectangle used to draw the focus box */
    Rect fRTextOutline; /* rectangle used to draw the border */
    RgnHandle fTextBackgroundRgn; /* background region for the text, erased before calling TEUpdate */
        /* our focus advance override routine */
    EventHandlerUPP handlerUPP;
    EventHandlerRef handlerRef;
} STPTextPaneVars;




/* Univerals Procedure Pointer variables used by the
    mUP Control.  These variables are set up
    the first time that mUPOpenControl is called. */
ControlUserPaneDrawUPP gTPDrawProc = NULL;
ControlUserPaneHitTestUPP gTPHitProc = NULL;
ControlUserPaneTrackingUPP gTPTrackProc = NULL;
ControlUserPaneIdleUPP gTPIdleProc = NULL;
ControlUserPaneKeyDownUPP gTPKeyProc = NULL;
ControlUserPaneActivateUPP gTPActivateProc = NULL;
ControlUserPaneFocusUPP gTPFocusProc = NULL;

    /* events handled by our focus advance override routine */
#if TARGET_CARBON
static const EventTypeSpec gMLTEEvents[] = { { kEventClassTextInput, kEventUnicodeForKeyEvent } };
#define kMLTEEventCount (sizeof( gMLTEEvents ) / sizeof( EventTypeSpec ))
#endif


/* TPActivatePaneText activates or deactivates the text edit record
    according to the value of setActive.  The primary purpose of this
    routine is to ensure each call is only made once. */
static void TPActivatePaneText(STPTextPaneVars **tpvars, Boolean setActive) {
    STPTextPaneVars *varsp;
    varsp = *tpvars;
    if (varsp->fTEActive != setActive) {
    
        varsp->fTEActive = setActive;
        
        TXNActivate(varsp->fTXNRec, varsp->fTXNFrame, varsp->fTEActive);
        
        if (varsp->fInFocus)
            TXNFocus( varsp->fTXNRec, varsp->fTEActive);
    }
}


/* TPFocusPaneText set the focus state for the text record. */
static void TPFocusPaneText(STPTextPaneVars **tpvars, Boolean setFocus) {
    STPTextPaneVars *varsp;
    varsp = *tpvars;
    if (varsp->fInFocus != setFocus) {
        varsp->fInFocus = setFocus;
        TXNFocus( varsp->fTXNRec, varsp->fInFocus);
    }
}


/* TPPaneDrawProc is called to redraw the control and for update events
    referring to the control.  This routine erases the text area's background,
    and redraws the text.  This routine assumes the scroll bar has been
    redrawn by a call to DrawControls. */
static pascal void TPPaneDrawProc(ControlRef theControl, ControlPartCode thePart) {
    STPTextPaneVars **tpvars, *varsp;
    char state;
    Rect bounds;
        /* set up our globals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
        varsp = *tpvars;
            
            /* save the drawing state */
        SetPort((**tpvars).fDrawingEnvironment);
            /* verify our boundary */
        GetControlBounds(theControl, &bounds);
        if ( ! EqualRect(&bounds, &varsp->fRTextArea) ) {
            SetRect(&varsp->fRFocusOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
            SetRect(&varsp->fRTextOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
            SetRect(&varsp->fRTextArea, bounds.left, bounds.top, bounds.right, bounds.bottom);
            RectRgn(varsp->fTextBackgroundRgn, &varsp->fRTextOutline);
            TXNSetFrameBounds(  varsp->fTXNRec, bounds.top, bounds.left, bounds.bottom, bounds.right, varsp->fTXNFrame);
        }

            /* update the text region */
        EraseRgn(varsp->fTextBackgroundRgn);
        TXNDraw(varsp->fTXNRec, NULL);
            /* restore the drawing environment */
            /* draw the text frame and focus frame (if necessary) */
        DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
        if ((**tpvars).fIsActive && varsp->fInFocus) DrawThemeFocusRect(&varsp->fRFocusOutline, true);
            /* release our globals */
        HSetState((Handle) tpvars, state);
    }
}


/* TPPaneHitTestProc is called when the control manager would
    like to determine what part of the control the mouse resides over.
    We also call this routine from our tracking proc to determine how
    to handle mouse clicks. */
static pascal ControlPartCode TPPaneHitTestProc(ControlHandle theControl, Point where) {
    STPTextPaneVars **tpvars;
    ControlPartCode result;
    char state;
        /* set up our locals and lock down our globals*/
    result = 0;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
            /* find the region where we clicked */
        if (PtInRect(where, &(**tpvars).fRTextArea)) {
            result = kmUPTextPart;
        } else result = 0;
            /* release oure globals */
        HSetState((Handle) tpvars, state);
    }
    return result;
}





/* TPPaneTrackingProc is called when the mouse is being held down
    over our control.  This routine handles clicks in the text area
    and in the scroll bar. */
static pascal ControlPartCode TPPaneTrackingProc(ControlHandle theControl, Point startPt, ControlActionUPP actionProc) {
    STPTextPaneVars **tpvars, *varsp;
    char state;
    ControlPartCode partCodeResult;
        /* make sure we have some variables... */
    partCodeResult = 0;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
            /* lock 'em down */
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
        varsp = *tpvars;
            /* we don't do any of these functions unless we're in focus */
        if ( ! varsp->fInFocus) {
            WindowPtr owner;
            owner = GetControlOwner(theControl);
            ClearKeyboardFocus(owner);
            SetKeyboardFocus(owner, theControl, kUserClickedToFocusPart);
        }
            /* find the location for the click */
        switch (TPPaneHitTestProc(theControl, startPt)) {
                
                /* handle clicks in the text part */
            case kmUPTextPart:
                {   SetPort((**tpvars).fDrawingEnvironment);
                    TXNClick( varsp->fTXNRec, GetCurrentEventRecord());
                }
                break;
            
        }
        
        HSetState((Handle) tpvars, state);
    }
    return partCodeResult;
}


/* TPPaneIdleProc is our user pane idle routine.  When our text field
    is active and in focus, we use this routine to set the cursor. */
static pascal void TPPaneIdleProc(ControlHandle theControl) {
    STPTextPaneVars **tpvars, *varsp;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
            /* if we're not active, then we have nothing to say about the cursor */
        if ((**tpvars).fIsActive) {
            char state;
            Rect bounds;
            Point mousep;
                /* lock down the globals */
            state = HGetState((Handle) tpvars);
            HLock((Handle) tpvars);
            varsp = *tpvars;
                /* get the current mouse coordinates (in our window) */
#if TARGET_CARBON
            SetPort(GetWindowPort(GetControlOwner(theControl)));
#else
            SetPort((GrafPtr) GetWindowPort(GetControlOwner(theControl)));
#endif
            GetMouse(&mousep);
                /* there's a 'focus thing' and an 'unfocused thing' */
            if (varsp->fInFocus) {
                    /* flash the cursor */
                SetPort((**tpvars).fDrawingEnvironment);
                TXNIdle(varsp->fTXNRec);
                /* set the cursor */
                if (PtInRect(mousep, &varsp->fRTextArea)) {
                    RgnHandle theRgn;
                    RectRgn((theRgn = NewRgn()), &varsp->fRTextArea);
                    TXNAdjustCursor(varsp->fTXNRec, theRgn);
                    DisposeRgn(theRgn);
                 } else SetThemeCursor(kThemeArrowCursor);
            } else {
                /* if it's in our bounds, set the cursor */
                GetControlBounds(theControl, &bounds);
                if (PtInRect(mousep, &bounds))
                    SetThemeCursor(kThemeArrowCursor);
            }
            
            HSetState((Handle) tpvars, state);
        }
    }
}


/* TPPaneKeyDownProc is called whenever a keydown event is directed
    at our control.  Here, we direct the keydown event to the text
    edit record and redraw the scroll bar and text field as appropriate. */
static pascal ControlPartCode TPPaneKeyDownProc(ControlHandle theControl,
                            SInt16 keyCode, SInt16 charCode, SInt16 modifiers) {
    STPTextPaneVars **tpvars;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
        if ((**tpvars).fInFocus) {
                /* turn autoscrolling on and send the key event to text edit */
            SetPort((**tpvars).fDrawingEnvironment);
            TXNKeyDown( (**tpvars).fTXNRec, GetCurrentEventRecord());
        }
    }
    return kControlEntireControl;
}


/* TPPaneActivateProc is called when the window containing
    the user pane control receives activate events. Here, we redraw
    the control and it's text as necessary for the activation state. */
static pascal void TPPaneActivateProc(ControlHandle theControl, Boolean activating) {
    Rect bounds;
    STPTextPaneVars **tpvars, *varsp;
    char state;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
        varsp = *tpvars;
            /* de/activate the text edit record */
        SetPort((**tpvars).fDrawingEnvironment);
            GetControlBounds(theControl, &bounds);
            varsp->fIsActive = activating;
            TPActivatePaneText(tpvars, varsp->fIsActive && varsp->fInFocus);
            /* redraw the frame */
        DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
        if (varsp->fInFocus) DrawThemeFocusRect(&varsp->fRFocusOutline, varsp->fIsActive);
        HSetState((Handle) tpvars, state);
    }
}


/* TPPaneFocusProc is called when every the focus changes to or
    from our control.  Herein, switch the focus appropriately
    according to the parameters and redraw the control as
    necessary.  */
static pascal ControlPartCode TPPaneFocusProc(ControlHandle theControl, ControlFocusPart action) {
    ControlPartCode focusResult;
    STPTextPaneVars **tpvars, *varsp;
    char state;
        /* set up locals */
    focusResult = kControlFocusNoPart;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
        varsp = *tpvars;
            /* if kControlFocusPrevPart and kControlFocusNextPart are received when the user is
            tabbing forwards (or shift tabbing backwards) through the items in the dialog,
            and kControlFocusNextPart will be received.  When the user clicks in our field
            and it is not the current focus, then the constant kUserClickedToFocusPart will
            be received.  The constant kControlFocusNoPart will be received when our control
            is the current focus and the user clicks in another control.  In your focus routine,
            you should respond to these codes as follows:

            kControlFocusNoPart - turn off focus and return kControlFocusNoPart.  redraw
                the control and the focus rectangle as necessary.

            kControlFocusPrevPart or kControlFocusNextPart - toggle focus on or off
                depending on its current state.  redraw the control and the focus rectangle
                as appropriate for the new focus state.  If the focus state is 'off', return the constant
                kControlFocusNoPart, otherwise return a non-zero part code.
            kUserClickedToFocusPart - is a constant defined for this example.  You should
                define your own value for handling click-to-focus type events. */
            /* save the drawing state */
        SetPort((**tpvars).fDrawingEnvironment);
            /* calculate the next highlight state */
        switch (action) {
            default:
            case kControlFocusNoPart:
                TPFocusPaneText(tpvars, false);
                focusResult = kControlFocusNoPart;
                break;
            case kUserClickedToFocusPart:
                TPFocusPaneText(tpvars, true);
                focusResult = 1;
                break;
            case kControlFocusPrevPart:
            case kControlFocusNextPart:
                TPFocusPaneText(tpvars, ( ! varsp->fInFocus));
                focusResult = varsp->fInFocus ? 1 : kControlFocusNoPart;
                break;
        }
            TPActivatePaneText(tpvars, varsp->fIsActive && varsp->fInFocus);
            /* redraw the text fram and focus rectangle to indicate the
            new focus state */
        DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
        DrawThemeFocusRect(&varsp->fRFocusOutline, varsp->fIsActive && varsp->fInFocus);
            /* done */
        HSetState((Handle) tpvars, state);
    }
    return focusResult;
}











//This our carbon event handler for unicode key downs
#if TARGET_CARBON
static pascal OSStatus FocusAdvanceOverride(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    WindowRef window;
    STPTextPaneVars **tpvars;
    OSStatus err;
    unsigned short mUnicodeText;
    ByteCount charCounts=0;
        /* get our window pointer */
    tpvars = (STPTextPaneVars **) userData;
    window = (**tpvars).fOwner;
        //find out how many bytes are needed
    err = GetEventParameter(event, kEventParamTextInputSendText,
                typeUnicodeText, NULL, 0, &charCounts, NULL);
    if (err != noErr) goto bail;
        /* we're only looking at single characters */
    if (charCounts != 2) { err = eventNotHandledErr; goto bail; }
        /* get the character */
    err = GetEventParameter(event, kEventParamTextInputSendText, 
                typeUnicodeText, NULL, sizeof(mUnicodeText),
                &charCounts, (char*) &mUnicodeText);
    if (err != noErr) goto bail;
        /* if it's not the tab key, forget it... */
    if ((mUnicodeText != '\t')) { err = eventNotHandledErr; goto bail; }
        /* advance the keyboard focus */
    AdvanceKeyboardFocus(window);
        /* noErr lets the CEM know we handled the event */
    return noErr;
bail:
    return eventNotHandledErr;
}
#endif


/* mUPOpenControl initializes a user pane control so it will be drawn
    and will behave as a scrolling text edit field inside of a window.
    This routine performs all of the initialization steps necessary,
    except it does not create the user pane control itself.  theControl
    should refer to a user pane control that you have either created
    yourself or extracted from a dialog's control heirarchy using
    the GetDialogItemAsControl routine.  */
OSStatus mUPOpenControl(ControlHandle theControl) {
    Rect bounds;
    WindowPtr theWindow;
    STPTextPaneVars **tpvars, *varsp;
    OSStatus err;
    RGBColor rgbWhite = {0xFFFF, 0xFFFF, 0xFFFF};
    TXNBackground tback;
    
        /* set up our globals */
    if (gTPDrawProc == NULL) gTPDrawProc = NewControlUserPaneDrawUPP(TPPaneDrawProc);
    if (gTPHitProc == NULL) gTPHitProc = NewControlUserPaneHitTestUPP(TPPaneHitTestProc);
    if (gTPTrackProc == NULL) gTPTrackProc = NewControlUserPaneTrackingUPP(TPPaneTrackingProc);
    if (gTPIdleProc == NULL) gTPIdleProc = NewControlUserPaneIdleUPP(TPPaneIdleProc);
    if (gTPKeyProc == NULL) gTPKeyProc = NewControlUserPaneKeyDownUPP(TPPaneKeyDownProc);
    if (gTPActivateProc == NULL) gTPActivateProc = NewControlUserPaneActivateUPP(TPPaneActivateProc);
    if (gTPFocusProc == NULL) gTPFocusProc = NewControlUserPaneFocusUPP(TPPaneFocusProc);
        
        /* allocate our private storage */
    tpvars = (STPTextPaneVars **) NewHandleClear(sizeof(STPTextPaneVars));
    SetControlReference(theControl, (long) tpvars);
    HLock((Handle) tpvars);
    varsp = *tpvars;
        /* set the initial settings for our private data */
    varsp->fInFocus = false;
    varsp->fIsActive = true;
    varsp->fTEActive = false;
    varsp->fUserPaneRec = theControl;
    theWindow = varsp->fOwner = GetControlOwner(theControl);
#if TARGET_CARBON
    varsp->fDrawingEnvironment = GetWindowPort(varsp->fOwner);
#else
    varsp->fDrawingEnvironment = (GrafPtr) GetWindowPort(varsp->fOwner);
#endif
    varsp->fInDialogWindow = ( GetWindowKind(varsp->fOwner) == kDialogWindowKind );
        /* set up the user pane procedures */
    SetControlData(theControl, kControlEntireControl, kControlUserPaneDrawProcTag, sizeof(gTPDrawProc), &gTPDrawProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneHitTestProcTag, sizeof(gTPHitProc), &gTPHitProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneTrackingProcTag, sizeof(gTPTrackProc), &gTPTrackProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneIdleProcTag, sizeof(gTPIdleProc), &gTPIdleProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneKeyDownProcTag, sizeof(gTPKeyProc), &gTPKeyProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneActivateProcTag, sizeof(gTPActivateProc), &gTPActivateProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneFocusProcTag, sizeof(gTPFocusProc), &gTPFocusProc);
        /* calculate the rectangles used by the control */
    GetControlBounds(theControl, &bounds);
    SetRect(&varsp->fRFocusOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
    SetRect(&varsp->fRTextOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
    SetRect(&varsp->fRTextArea, bounds.left, bounds.top, bounds.right, bounds.bottom);
        /* calculate the background region for the text.  In this case, it's kindof
        and irregular region because we're setting the scroll bar a little ways inside
        of the text area. */
    RectRgn((varsp->fTextBackgroundRgn = NewRgn()), &varsp->fRTextOutline);

        /* set up the drawing environment */
    SetPort(varsp->fDrawingEnvironment);

        /* create the new edit field */
    TXNNewObject(NULL, varsp->fOwner, &varsp->fRTextArea,
        kTXNWantVScrollBarMask | kTXNAlwaysWrapAtViewEdgeMask,
        kTXNTextEditStyleFrameType,
        kTXNTextensionFile,
        kTXNSystemDefaultEncoding, 
        &varsp->fTXNRec, &varsp->fTXNFrame, (TXNObjectRefcon) tpvars);
        
        /* set the field's background */
    tback.bgType = kTXNBackgroundTypeRGB;
    tback.bg.color = rgbWhite;
    TXNSetBackground( varsp->fTXNRec, &tback);
    
        /* install our focus advance override routine */
#if TARGET_CARBON
    varsp->handlerUPP = NewEventHandlerUPP(FocusAdvanceOverride);
    err = InstallWindowEventHandler( varsp->fOwner, varsp->handlerUPP,
        kMLTEEventCount, gMLTEEvents, tpvars, &varsp->handlerRef );
#endif
        
        /* unlock our storage */
    HUnlock((Handle) tpvars);
        /* perform final activations and setup for our text field.  Here,
        we assume that the window is going to be the 'active' window. */
    TPActivatePaneText(tpvars, varsp->fIsActive && varsp->fInFocus);
        /* all done */
    return noErr;
}



/* mUPCloseControl deallocates all of the structures allocated
    by mUPOpenControl.  */
OSStatus mUPCloseControl(ControlHandle theControl) {
    STPTextPaneVars **tpvars;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
        /* release our sub records */
    TXNDeleteObject((**tpvars).fTXNRec);
        /* remove our focus advance override */
    RemoveEventHandler((**tpvars).handlerRef);
    DisposeEventHandlerUPP((**tpvars).handlerUPP);
        /* delete our private storage */
    DisposeHandle((Handle) tpvars);
        /* zero the control reference */
    SetControlReference(theControl, 0);
    return noErr;
}




    /* mUPSetText replaces the contents of the selection with the unicode
    text described by the text and count parameters:.
        text = pointer to unicode text buffer
        count = number of bytes in the buffer.  */
OSStatus mUPSetText(ControlHandle theControl, char* text, long count) {
    STPTextPaneVars **tpvars;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
        /* set the text in the record */
    return TXNSetData( (**tpvars).fTXNRec, kTXNUnicodeTextData, text, count,
        kTXNUseCurrentSelection, kTXNUseCurrentSelection);

    return noErr;
}


/* mUPSetSelection sets the text selection and autoscrolls the text view
    so either the cursor or the selction is in the view. */
void mUPSetSelection(ControlHandle theControl, long selStart, long selEnd) {
    STPTextPaneVars **tpvars;
        /* set up our locals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
        /* and our drawing environment as the operation
        may force a redraw in the text area. */
    SetPort((**tpvars).fDrawingEnvironment);
        /* change the selection */
    TXNSetSelection( (**tpvars).fTXNRec, selStart, selEnd);
}





/* mUPGetText returns the current text data being displayed inside of
    the mUPControl.  When noErr is returned, *theText contain a new
    handle containing all of the Unicode text copied from the current
    selection.  It is the caller's responsibiliby to dispose of this handle. */
OSStatus mUPGetText(ControlHandle theControl, Handle *theText) {
    STPTextPaneVars **tpvars;
    OSStatus err;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
        /* extract the text from the record */
    err = TXNGetData( (**tpvars).fTXNRec, kTXNUseCurrentSelection, kTXNUseCurrentSelection, theText);
        /* all done */
    return err;
}



/* mUPCreateControl creates a new user pane control and then it passes it
    to mUPOpenControl to initialize it as a scrolling text user pane control. */
OSStatus mUPCreateControl(WindowPtr theWindow, Rect *bounds, ControlHandle *theControl) {
    short featurSet;
        /* the following feature set can be specified in CNTL resources by using
        the value 1214.  When creating a user pane control, we pass this value
        in the 'value' parameter. */
    featurSet = kControlSupportsEmbedding | kControlSupportsFocus | kControlWantsIdle
            | kControlWantsActivate | kControlHandlesTracking | kControlHasSpecialBackground
            | kControlGetsFocusOnClick | kControlSupportsLiveFeedback;
        /* create the control */
    *theControl = NewControl(theWindow, bounds, "\p", true, featurSet, 0, featurSet, kControlUserPaneProc, 0);
        /* set up the mUP specific features and data */
    mUPOpenControl(*theControl);
        /* all done.... */
    return noErr;
}


/* mUPDisposeControl calls mUPCloseControl and then it calls DisposeControl. */
OSStatus mUPDisposeControl(ControlHandle theControl) {
        /* deallocate the mUP specific data */
    mUPCloseControl(theControl);
        /* deallocate the user pane control itself */
    DisposeControl(theControl);
    return noErr;
}




/* IsmUPControl returns true if theControl is not NULL
    and theControl refers to a mUP Control.  */
Boolean IsmUPControl(ControlHandle theControl) {
    Size theSize;
    ControlUserPaneFocusUPP localFocusProc;
        /* a NULL control is not a mUP control */
    if (theControl == NULL) return false;
        /* check if the control is using our focus procedure */
    theSize = sizeof(localFocusProc);
    if (GetControlData(theControl, kControlEntireControl, kControlUserPaneFocusProcTag,
        sizeof(localFocusProc), &localFocusProc, &theSize) != noErr) return false;
    if (localFocusProc != gTPFocusProc) return false;
        /* all tests passed, it's a mUP control */
    return true;
}


/* mUPDoEditCommand performs the editing command specified
    in the editCommand parameter.  The mUPControl's text
    and scroll bar are redrawn and updated as necessary. */
void mUPDoEditCommand(ControlHandle theControl, short editCommand) {
    STPTextPaneVars **tpvars;
        /* set up our locals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
        /* and our drawing environment as the operation
        may force a redraw in the text area. */
    SetPort((**tpvars).fDrawingEnvironment);
        /* perform the editing command */
    switch (editCommand) {
        case kmUPCut:
            ClearCurrentScrap();
            TXNCut((**tpvars).fTXNRec); 
            TXNConvertToPublicScrap();
            break;
        case kmUPCopy:
            ClearCurrentScrap();
            TXNCopy((**tpvars).fTXNRec);
            TXNConvertToPublicScrap();
            break;
        case kmUPPaste:
            TXNConvertFromPublicScrap();
            TXNPaste((**tpvars).fTXNRec);
            break;
        case kmUPClear:
            TXNClear((**tpvars).fTXNRec);
            break;
    }
}




/* mUPGetContents returns the entire contents of the control including the text
    and the formatting information. */
OSStatus mUPGetContents(ControlHandle theControl, Handle *theContents) {
    STPTextPaneVars **tpvars;
    OSStatus err;
    short vRefNum;
    long dirID;
    FSSpec tspec;
    short trefnum;
    Boolean texists;
    long bytecount;
    Handle localdata;
        /* set up locals */
    trefnum = 0;
    texists = false;
    localdata = NULL;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (theContents == NULL) return paramErr;
        /* create a temporary file */
    err = FindFolder(kOnSystemDisk, kTemporaryFolderType, true, &vRefNum, &dirID);
    if (err != noErr) goto bail;
    FSMakeFSSpec(vRefNum, dirID, "\pmUPGetContents", &tspec);
    err = FSpCreate(&tspec, 'trsh', 'trsh', smSystemScript);
    if (err != noErr) goto bail;
    texists = true;
        /* open the file */
    err = FSpOpenDF(&tspec, fsRdWrPerm, &trefnum);
    if (err != noErr) goto bail;
        /* save the data */
    err = TXNSave( (**tpvars).fTXNRec, kTXNTextensionFile, 0, kTXNSystemDefaultEncoding, &tspec, trefnum, 0);
    if (err != noErr) goto bail;
        /* get the file length and set the position */
    err = GetEOF(trefnum, &bytecount);
    if (err != noErr) goto bail;
    err = SetFPos(trefnum, fsFromStart, 0);
    if (err != noErr) goto bail;
        /* copy the data fork to a handle */
    localdata = NewHandle(bytecount);
    if (localdata == NULL) { err = memFullErr; goto bail; }
    HLock(localdata);
    err = FSRead(trefnum, &bytecount, *localdata);
    HUnlock(localdata);
    if (err != noErr) goto bail;
        /* store result */
    *theContents = localdata;
        /* clean up */
    FSClose(trefnum);
    FSpDelete(&tspec);
        /* all done */
    return noErr;
bail:
    if (trefnum != 0) FSClose(trefnum);
    if (texists) FSpDelete(&tspec);
    if (localdata != NULL) DisposeHandle(localdata);
    return err;
}




/* mUPSetContents replaces the contents of the selection with the data stored in the handle. */
OSStatus mUPSetContents(ControlHandle theControl, Handle theContents) {
    STPTextPaneVars **tpvars;
    OSStatus err;
    short vRefNum;
    long dirID;
    FSSpec tspec;
    short trefnum;
    Boolean texists;
    long bytecount;
    char state;
        /* set up locals */
    trefnum = 0;
    texists = false;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (theContents == NULL) return paramErr;
        /* create a temporary file */
    err = FindFolder(kOnSystemDisk,  kTemporaryFolderType, true, &vRefNum, &dirID);
    if (err != noErr) goto bail;
    FSMakeFSSpec(vRefNum, dirID, "\pmUPSetContents", &tspec);
    err = FSpCreate(&tspec, 'trsh', 'trsh', smSystemScript);
    if (err != noErr) goto bail;
    texists = true;
        /* open the file */
    err = FSpOpenDF(&tspec, fsRdWrPerm, &trefnum);
    if (err != noErr) goto bail;
        /* save the data to the temporary file */
    state = HGetState(theContents);
    HLock(theContents);
    bytecount = GetHandleSize(theContents);
    err = FSWrite(trefnum, &bytecount, *theContents);
    HSetState(theContents, state);
    if (err != noErr) goto bail;
        /* reset the file position */
    err = SetFPos(trefnum, fsFromStart, 0);
    if (err != noErr) goto bail;
        /* load the data */
    err = TXNSetDataFromFile((**tpvars).fTXNRec, trefnum, kTXNTextensionFile, bytecount, kTXNUseCurrentSelection, kTXNUseCurrentSelection);
    if (err != noErr) goto bail;
        /* clean up */
    FSClose(trefnum);
    FSpDelete(&tspec);
        /* all done */
    return noErr;
bail:
    if (trefnum != 0) FSClose(trefnum);
    if (texists) FSpDelete(&tspec);
    return err;
}

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

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        wxASSERT_MSG( !(m_windowStyle & wxTE_PROCESS_ENTER),
                      wxT("wxTE_PROCESS_ENTER style is ignored for multiline text controls (they always process it)") );

        m_windowStyle |= wxTE_PROCESS_ENTER;
    }


  if ( style & wxTE_PASSWORD )
  {
      m_macControl = ::NewControl( parent->GetMacRootWindow() , &bounds , "\p" , true , 0 , 0 , 1, 
        kControlEditTextPasswordProc , (long) this ) ;
    }
    else
    {
    if ( mUPCreateControl(parent->GetMacRootWindow(), &bounds, &m_macControl) != noErr ) 
      return FALSE ;
    }
    MacPostControlCreate() ;

    wxString value ;
        
    if( wxApp::s_macDefaultEncodingIsPC )
        value = wxMacMakeMacStringFromPC( st ) ;
    else
        value = st ;
        
    if ( style & wxTE_PASSWORD )
    {
      ::SetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , value.Length() , (char*) ((const char*)value) ) ;
  }
  else
  {
    STPTextPaneVars **tpvars;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(m_macControl);
        /* set the text in the record */
    TXNSetData( (**tpvars).fTXNRec, kTXNTextData,  (const char*)value, value.Length(),
      kTXNStartOffset, kTXNEndOffset);
  }
  
  return TRUE;
}

wxString wxTextCtrl::GetValue() const
{
    Size actualsize;
  if ( m_windowStyle & wxTE_PASSWORD )
  {
      ::GetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
  }
  else
  {
    STPTextPaneVars **tpvars;
    OSStatus err;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(m_macControl);
        /* extract the text from the record */
    Handle theText ;
    err = TXNGetDataEncoded( (**tpvars).fTXNRec, kTXNStartOffset, kTXNEndOffset, &theText , kTXNTextData );
        /* all done */
    if ( err )
    {
      actualsize = 0 ;
    }
    else
    {
      actualsize = GetHandleSize( theText ) ;
      strncpy( wxBuffer , *theText , actualsize ) ;
      DisposeHandle( theText ) ;
    }
  }
    wxBuffer[actualsize] = 0 ;
    if( wxApp::s_macDefaultEncodingIsPC )
        return wxMacMakePCStringFromMac( wxBuffer ) ;
    else
        return wxString(wxBuffer);
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
  if ( m_windowStyle & wxTE_PASSWORD )
  {
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;

    *from = (**teH).selStart;
    *to = (**teH).selEnd;
   }
   else
   {
        STPTextPaneVars **tpvars;

            /* set up locals */
        tpvars = (STPTextPaneVars **) GetControlReference(m_macControl);

        TXNGetSelection(  (**tpvars).fTXNRec , (TXNOffset*) from , (TXNOffset*) to ) ;

   }
}

void wxTextCtrl::SetValue(const wxString& st)
{
    wxString value ;
    
    if( wxApp::s_macDefaultEncodingIsPC )
        value = wxMacMakeMacStringFromPC( st ) ;
    else
        value = st ;
  if ( m_windowStyle & wxTE_PASSWORD )
  {
      ::SetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , value.Length() , (char*) ((const char*)value) ) ;
  }
  else
  {
    STPTextPaneVars **tpvars;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(m_macControl);
        /* set the text in the record */
    TXNSetData( (**tpvars).fTXNRec, kTXNTextData,  (const char*)value, value.Length(),
      kTXNStartOffset, kTXNEndOffset);
  }
    WindowRef window = GetMacRootWindow() ;
    if ( window )
    {
        wxWindow* win = wxFindWinFromMacWindow( window ) ;
        if ( win )
        {
            wxMacDrawingHelper help( win ) ;
            // the mac control manager always assumes to have the origin at 0,0
            SetOrigin( 0 , 0 ) ;
            
            bool            hasTabBehind = false ;
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
        }
    }
}

// Clipboard operations
void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
      if ( m_windowStyle & wxTE_PASSWORD )
      {
            TEHandle teH ;
            long size ;
       
             ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
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
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
      if ( m_windowStyle & wxTE_PASSWORD )
      {
            TEHandle teH ;
            long size ;
       
            ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
            TECut( teH ) ;
    #if TARGET_CARBON
            OSStatus err ;
            err = ClearCurrentScrap( );
    #else
            OSErr err ;
            err = ZeroScrap( );
    #endif
            TEToScrap() ;
            //  MacInvalidateControl() ;
    }
    }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
    if ( m_windowStyle & wxTE_PASSWORD )
    {
            TEHandle teH ;
            long size ;
     
            ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
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
                
                bool            hasTabBehind = false ;
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
            }
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
        ScrapFlavorFlags    flavorFlags;
        Size                byteCount;
        
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
  long begin,end ;
  GetSelection( &begin , &end ) ;
  return begin ;
}

long wxTextCtrl::GetLastPosition() const
{
    if ( m_windowStyle & wxTE_PASSWORD )
  {

   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
//   ::GetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection , &size ) ;
    return (**teH).teLength ;
  }
}

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
  if ( m_windowStyle & wxTE_PASSWORD )
  {
    TEHandle teH ;
    long size ;
   
    ControlEditTextSelectionRec selection ;
   
    selection.selStart = from ;
    selection.selEnd = to ;
    ::SetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
        ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
    TESetSelect( from , to  , teH ) ;
    TEDelete( teH ) ;
        TEInsert( value , value.Length() , teH ) ;
    }
    Refresh() ;
}

void wxTextCtrl::Remove(long from, long to)
{
  if ( m_windowStyle & wxTE_PASSWORD )
  {
    TEHandle teH ;
    long size ;
   
    ControlEditTextSelectionRec selection ;
   
    selection.selStart = from ;
    selection.selEnd = to ;
    ::SetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
    ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
    TEDelete( teH ) ;
  }
    Refresh() ;
}

void wxTextCtrl::SetSelection(long from, long to)
{
  if ( m_windowStyle & wxTE_PASSWORD )
  {
   ControlEditTextSelectionRec selection ;
   TEHandle teH ;
   long size ;
   
   ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
   
   selection.selStart = from ;
   selection.selEnd = to ;
   
   ::SetControlData( m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
   TESetSelect( selection.selStart , selection.selEnd , teH ) ;
  }
  else
  {
    STPTextPaneVars **tpvars;
        /* set up our locals */
    tpvars = (STPTextPaneVars **) GetControlReference(m_macControl);
        /* and our drawing environment as the operation
        may force a redraw in the text area. */
    SetPort((**tpvars).fDrawingEnvironment);
        /* change the selection */
    TXNSetSelection( (**tpvars).fTXNRec, from, to);
  }
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
    wxString value ;
    if( wxApp::s_macDefaultEncodingIsPC )
        value = wxMacMakeMacStringFromPC( text ) ;
    else
        value = text ;
    if ( m_windowStyle & wxTE_PASSWORD )
    {
      TEHandle teH ;
      long size ;
          
      ::GetControlData( m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*) &teH , &size ) ;
        TEInsert( value , value.Length() , teH ) ;
        }
        else
        {
        STPTextPaneVars **tpvars;
            /* set up locals */
        tpvars = (STPTextPaneVars **) GetControlReference(m_macControl);
            /* set the text in the record */
        TXNSetData( (**tpvars).fTXNRec, kTXNTextData,  (const char*)value, value.Length(),
          kTXNUseCurrentSelection, kTXNUseCurrentSelection);
        }
        Refresh() ;
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Clear()
{
  if ( m_windowStyle & wxTE_PASSWORD )
  {

    ::SetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 0 , (char*) ((const char*)NULL) ) ;
  }
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
  if ( m_windowStyle & wxTE_PASSWORD )
  {
    Size actualsize;
    ::GetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
    
    int count = 1;
    for (int i = 0; i < actualsize; i++)
    {
        if (wxBuffer[i] == '\r') count++;
    }
    
    return count;
  }
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
  if ( m_windowStyle & wxTE_PASSWORD )
  {
    Size actualsize;
    ::GetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
    
    // Find line first
    int count = 0;
    for (int i = 0; i < actualsize; i++)
    {
        if (count == lineNo)
        {
            // Count chars in line then
            count = 0;
            for (int j = i; j < actualsize; j++)
            {
                count++;
                if (wxBuffer[j] == '\r') return count;
            }
            
            return count;
        }
        if (wxBuffer[i] == '\r') count++;
    }
    }
    return 0;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
  if ( m_windowStyle & wxTE_PASSWORD )
  {
    Size actualsize;
    ::GetControlData( m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 32767 , wxBuffer , &actualsize) ;
    
    // Find line first
    int count = 0;
    for (int i = 0; i < actualsize; i++)
    {
        if (count == lineNo)
        {
            // Add chars in line then
            wxString tmp("");
            
            for (int j = i; j < actualsize; j++)
            {
                if (wxBuffer[j] == '\r')
                    return tmp;
                    
                tmp += wxBuffer[j];
            }
            
            return tmp;
        }
        if (wxBuffer[i] == '\r') count++;
    }
    }
    return wxString("");
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
                    wxButton *def = wxDynamicCast(panel->GetDefaultItem(),
                                                          wxButton);
                    if ( def && def->IsEnabled() )
                    {
                        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
                        event.SetEventObject(def);
                        def->Command(event);
                        return ;
                   }
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
    if ( keychar >= 0x20 || event.KeyCode() == WXK_RETURN || event.KeyCode() == WXK_DELETE || event.KeyCode() == WXK_BACK)
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

#endif

#endif
    // wxUSE_TEXTCTRL
