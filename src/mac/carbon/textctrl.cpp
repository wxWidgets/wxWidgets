/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      Stefan Csomor
// Modified by: Ryan Norton (MLTE GetLineLength and GetLineText)
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "textctrl.h"
#endif

#include "wx/wxprec.h"

#if wxUSE_TEXTCTRL


#ifdef __DARWIN__
  #include <sys/types.h>
  #include <sys/stat.h>
#else
  #include <stat.h>
#endif

#include "wx/msgdlg.h"

#if wxUSE_STD_IOSTREAM
    #if wxUSE_IOSTREAMH
        #include <fstream.h>
    #else
        #include <fstream>
    #endif
#endif

#include "wx/app.h"
#include "wx/dc.h"
#include "wx/button.h"
#include "wx/toplevel.h"
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

#ifndef __DARWIN__
#include <Scrap.h>
#endif

// if this is set to 1 then under OSX 10.2 the 'classic' MLTE implementation will be used
// if set to 0 then the unicode textctrl will be used
#ifndef wxMAC_AWAYS_USE_MLTE
#define wxMAC_AWAYS_USE_MLTE 1
#endif

#include <MacTextEditor.h>
#include <ATSUnicode.h>
#include <TextCommon.h>
#include <TextEncodingConverter.h>
#include "wx/mac/uma.h"

class wxMacFunctor
{
public :
    wxMacFunctor(){}
    virtual ~wxMacFunctor() {}
    virtual void* operator()() = 0 ;
    static void* CallBackProc(void *param)
    {
        wxMacFunctor* f = (wxMacFunctor*) param ;
        void *result = (*f)() ;
        return result ;
    }
} ;

template<typename classtype,typename param1type>
class wxMacObjectFunctor1 : public wxMacFunctor
{
    typedef void (classtype::*function)( param1type p1 ) ;
    typedef void (classtype::*ref_function)( const param1type& p1 ) ;
public :
    wxMacObjectFunctor1( classtype *obj , function f , param1type p1 ) :
        wxMacFunctor(  )
    {
        m_object = obj ;
        m_function = f ;
        m_param1 = p1 ;
    }

    wxMacObjectFunctor1( classtype *obj , ref_function f , param1type p1 ) :
        wxMacFunctor(  )
    {
        m_object = obj ;
        m_refFunction = f ;
        m_param1 = p1 ;
    }

    ~wxMacObjectFunctor1() {}

    virtual void* operator()()
    {
        (m_object->*m_function)(m_param1) ;
        return NULL ;
    }
private :
    classtype* m_object ;
    param1type m_param1 ;
    union
    {
    function m_function ;
    ref_function m_refFunction ;
    } ;
} ;

template<typename classtype, typename param1type>
void* wxMacMPRemoteCall( classtype *object , void (classtype::*function)( param1type p1 ) , param1type p1 )
{
    wxMacObjectFunctor1<classtype,param1type> params(object,function,p1) ;
    void *result =
        MPRemoteCall( wxMacFunctor::CallBackProc , &params  , kMPOwningProcessRemoteContext ) ;
    return result ;
}

template<typename classtype, typename param1type>
void* wxMacMPRemoteCall( classtype *object , void (classtype::*function)( const param1type& p1 ) , param1type p1 )
{
    wxMacObjectFunctor1<classtype,param1type> params(object,function,p1) ;
    void *result =
        MPRemoteCall( wxMacFunctor::CallBackProc , &params  , kMPOwningProcessRemoteContext ) ;
    return result ;
}

template<typename classtype, typename param1type>
void* wxMacMPRemoteGUICall( classtype *object , void (classtype::*function)( param1type p1 ) , param1type p1 )
{
    wxMutexGuiLeave() ;
    void *result = wxMacMPRemoteCall( object , function , p1 ) ;
    wxMutexGuiEnter() ;
    return result ;
}

template<typename classtype, typename param1type>
void* wxMacMPRemoteGUICall( classtype *object , void (classtype::*function)( const param1type& p1 ) , param1type p1 )
{
    wxMutexGuiLeave() ;
    void *result = wxMacMPRemoteCall( object , function , p1 ) ;
    wxMutexGuiEnter() ;
    return result ;
}
// common interface for all implementations
class wxMacTextControl : public wxMacControl
{
public :
    wxMacTextControl() ;
    ~wxMacTextControl() ;

    virtual wxString GetStringValue() const = 0 ;
    virtual void SetStringValue( const wxString &val ) = 0 ;
    virtual void SetStyle(long start, long end, const wxTextAttr& style) ;
    virtual void Copy() ;
    virtual void Cut() ;
    virtual void Paste() ;
    virtual bool CanPaste() const ;
    virtual void SetEditable(bool editable) ;
    virtual wxTextPos GetLastPosition() const ;
    virtual void Replace( long from , long to , const wxString str ) ;
    virtual void Remove( long from , long to ) = 0 ;
    virtual void SetSelection( long from , long to ) = 0 ;
    virtual void GetSelection( long* from, long* to) const = 0 ;
    virtual void WriteText(const wxString& str) = 0 ;

    virtual void Clear() ;
    virtual bool CanUndo() const;
    virtual void Undo() ;
    virtual bool CanRedo() const;
    virtual void Redo() ;
    virtual int GetNumberOfLines() const ;
    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const ;
    virtual void ShowPosition( long WXUNUSED(pos) ) ;
    virtual int GetLineLength(long lineNo) const ;
    virtual wxString GetLineText(long lineNo) const ;
} ;

// common parts for implementations based on MLTE

class wxMacMLTEControl : public wxMacTextControl
{
public :
    virtual wxString GetStringValue() const ;
    virtual void SetStringValue( const wxString &str) ;

    static TXNFrameOptions FrameOptionsFromWXStyle( long wxStyle ) ;
    void    AdjustCreationAttributes( const wxColour& background , bool visible ) ;

    virtual void SetFont( const wxFont & font , const wxColour& foreground , long windowStyle ) ;
    virtual void SetBackground( const wxBrush &brush) ;
    virtual void SetStyle(long start, long end, const wxTextAttr& style) ;
    virtual void Copy() ;
    virtual void Cut() ;
    virtual void Paste() ;
    virtual bool CanPaste() const ;
    virtual void SetEditable(bool editable) ;
    virtual wxTextPos GetLastPosition() const ;
    virtual void Replace( long from , long to , const wxString str ) ;
    virtual void Remove( long from , long to )  ;
    virtual void GetSelection( long* from, long* to) const ;
    virtual void SetSelection( long from , long to ) ;

    virtual void WriteText(const wxString& str) ;
    virtual void Clear() ;

    virtual bool CanUndo() const ;
    virtual void Undo() ;
    virtual bool CanRedo()  const;
    virtual void Redo() ;
    virtual int GetNumberOfLines() const ;
    virtual long XYToPosition(long x, long y) const ;
    virtual bool PositionToXY(long pos, long *x, long *y) const ;
    virtual void ShowPosition( long pos ) ;
    virtual int GetLineLength(long lineNo) const ;
    virtual wxString GetLineText(long lineNo) const ;

    void SetTXNData( const wxString& st , TXNOffset start , TXNOffset end ) ;

protected :
    void TXNSetAttribute( const wxTextAttr& style , long from , long to ) ;
    TXNObject m_txn ;
} ;

#if TARGET_API_MAC_OSX

// implementation available under OSX

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2

class wxMacMLTEHIViewControl : public wxMacMLTEControl
{
public :
    wxMacMLTEHIViewControl( wxWindow *wxPeer,
                             const wxString& str,
                             const wxPoint& pos,
                             const wxSize& size, long style ) ;
    virtual OSStatus SetFocus( ControlFocusPart focusPart ) ;
    virtual bool HasFocus() const ;
    virtual bool NeedsFocusRect() const;
protected :
    HIViewRef m_scrollView ;
    HIViewRef m_textView ;
} ;

#endif

class wxMacUnicodeTextControl : public wxMacTextControl
{
public :
    wxMacUnicodeTextControl( wxWindow *wxPeer,
                             const wxString& str,
                             const wxPoint& pos,
                             const wxSize& size, long style ) ;
    ~wxMacUnicodeTextControl();
    virtual void VisibilityChanged(bool shown);
    virtual wxString GetStringValue() const ;
    virtual void SetStringValue( const wxString &str) ;
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual bool CanPaste() const;
    virtual void SetEditable(bool editable) ;
    virtual void Remove( long from , long to ) ;
    virtual void GetSelection( long* from, long* to) const ;
    virtual void SetSelection( long from , long to ) ;
    virtual void WriteText(const wxString& str) ;
protected :
    // contains the tag for the content (is different for password and non-password controls)
    OSType m_valueTag ;
} ;

#endif

// implementation available under classic

class STPTextPaneVars ;

class wxMacMLTEClassicControl : public wxMacMLTEControl
{
public :
    wxMacMLTEClassicControl( wxWindow *wxPeer,
                             const wxString& str,
                             const wxPoint& pos,
                             const wxSize& size, long style ) ;
    ~wxMacMLTEClassicControl() ;
    virtual void VisibilityChanged(bool shown) ;
    virtual bool NeedsFocusRect() const;
protected :
    OSStatus                 DoCreate();
public :
    // hack to make public until we have migrated all procs
    STPTextPaneVars*    m_macTXNvars ;
} ;

#define TE_UNLIMITED_LENGTH 0xFFFFFFFFUL

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
    EVT_ERASE_BACKGROUND( wxTextCtrl::OnEraseBackground )
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
void wxTextCtrl::Init()
{
  m_editable = true ;
  m_dirty = false;

  m_maxLength = TE_UNLIMITED_LENGTH ;
}

wxTextCtrl::~wxTextCtrl()
{
}


bool wxTextCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxString& str,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_macIsUserPane = false ;
    m_editable = true ;

    if ( ! ( style & wxNO_BORDER) )
        style = ( style & ~wxBORDER_MASK) | wxSUNKEN_BORDER ;

    if ( !wxTextCtrlBase::Create(parent, id, pos, size, style & ~(wxHSCROLL|wxVSCROLL), validator, name) )
        return false;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        wxASSERT_MSG( !(m_windowStyle & wxTE_PROCESS_ENTER),
                      wxT("wxTE_PROCESS_ENTER style is ignored for multiline text controls (they always process it)") );

        m_windowStyle |= wxTE_PROCESS_ENTER;
        style |= wxTE_PROCESS_ENTER ;
    }

#if TARGET_API_MAC_OSX
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
    if ( UMAGetSystemVersion() >= 0x1030 )
    {
        m_peer = new wxMacMLTEHIViewControl( this , str , pos , size , style ) ;
    }
#endif
#if !wxMAC_AWAYS_USE_MLTE
    if ( !m_peer )
    {
        m_peer = new wxMacUnicodeTextControl( this , str , pos , size , style ) ;
    }
#endif
#endif
    if ( !m_peer )
    {
        // this control draws the border itself
        if ( !HasFlag(wxNO_BORDER) )
        {
            m_windowStyle &= ~wxSUNKEN_BORDER ;
            bounds = wxMacGetBoundsForControl( this , pos , size ) ;
        }
        m_peer = new wxMacMLTEClassicControl( this , str , pos , size , style ) ;
    }

    MacPostControlCreate(pos,size) ;

    if ( m_windowStyle & wxTE_READONLY)
    {
        SetEditable( false ) ;
    }

    return true;
}

void wxTextCtrl::MacVisibilityChanged()
{
    GetPeer()->VisibilityChanged( MacIsReallyShown() ) ;
}

void wxTextCtrl::MacEnabledStateChanged()
{
}

wxString wxTextCtrl::GetValue() const
{
    return GetPeer()->GetStringValue() ;
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
    GetPeer()->GetSelection( from , to ) ;
}

void wxTextCtrl::SetValue(const wxString& str)
{
    // optimize redraws
    if ( GetValue() == str )
        return ;

    GetPeer()->SetStringValue(str) ;

    wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
    event.SetString( GetValue() ) ;
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);
}

void wxTextCtrl::SetMaxLength(unsigned long len)
{
    m_maxLength = len ;
}

bool wxTextCtrl::SetFont( const wxFont& font )
{
    if ( !wxTextCtrlBase::SetFont( font ) )
        return false ;

    GetPeer()->SetFont( font , GetForegroundColour() , GetWindowStyle() ) ;
    return true ;
}

bool wxTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    GetPeer()->SetStyle( start , end , style ) ;
    return true ;
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    wxTextCtrlBase::SetDefaultStyle( style ) ;
    SetStyle( kTXNUseCurrentSelection , kTXNUseCurrentSelection , GetDefaultStyle() ) ;
    return true ;
}

// Clipboard operations
void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
        GetPeer()->Copy() ;
    }
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
        GetPeer()->Cut() ;

        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
      }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        GetPeer()->Paste() ;
        // eventually we should add setting the default style again

        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
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
    if ( !IsEditable() )
    {
        return false ;
    }
    // Can cut if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanPaste() const
{
    if (!IsEditable())
        return false;

    return GetPeer()->CanPaste() ;
}

void wxTextCtrl::SetEditable(bool editable)
{
    if ( editable != m_editable )
    {
        m_editable = editable ;
        GetPeer()->SetEditable( editable ) ;
    }
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
    SetSelection( pos , pos ) ;
}

void wxTextCtrl::SetInsertionPointEnd()
{
    wxTextPos pos = GetLastPosition();
    SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
    long begin,end ;
    GetSelection( &begin , &end ) ;
    return begin ;
}

wxTextPos wxTextCtrl::GetLastPosition() const
{
    return GetPeer()->GetLastPosition( ) ;
}

void wxTextCtrl::Replace(long from, long to, const wxString& str)
{
    GetPeer()->Replace( from , to , str) ;
}

void wxTextCtrl::Remove(long from, long to)
{
    GetPeer()->Remove( from , to ) ;
}

void wxTextCtrl::SetSelection(long from, long to)
{
    GetPeer()->SetSelection( from , to ) ;
}

bool wxTextCtrl::LoadFile(const wxString& file)
{
    if ( wxTextCtrlBase::LoadFile(file) )
    {
        return true;
    }

    return false;
}

void wxTextCtrl::WriteText(const wxString& str)
{
    // TODO this MPRemoting will be moved into a remoting peer proxy for any command
    if ( !wxIsMainThread() )
    {
        // unfortunately CW 8 is not able to correctly deduce the template types, so we have
        // to instantiate explicitely
        wxMacMPRemoteGUICall<wxTextCtrl,wxString>( this , &wxTextCtrl::WriteText , str ) ;
        return ;
    }
    else
    {
        GetPeer()->WriteText( str ) ;
    }
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Clear()
{
    GetPeer()->Clear() ;
}

bool wxTextCtrl::IsModified() const
{
    return m_dirty;
}

bool wxTextCtrl::IsEditable() const
{
    return IsEnabled() && m_editable ;
}

bool wxTextCtrl::AcceptsFocus() const
{
    // we don't want focus if we can't be edited
    return /*IsEditable() && */ wxControl::AcceptsFocus();
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    int wText = 100 ;

    int hText;

    // these are the numbers from the HIG, we reduce them by the borders
    // first

    switch( m_windowVariant )
    {
        case wxWINDOW_VARIANT_NORMAL :
            hText = 22 - 6 ;
            break ;
        case wxWINDOW_VARIANT_SMALL :
            hText = 19 - 6 ;
            break ;
        case wxWINDOW_VARIANT_MINI :
            hText= 15 - 6 ;
            break ;
        default :
            hText = 22 - 6;
            break ;
    }

    // as the above numbers have some free space around the text
    // we get 5 lines like this anyway
    if ( m_windowStyle & wxTE_MULTILINE )
    {
         hText *= 5 ;
    }

    if ( !HasFlag(wxNO_BORDER) )
        hText += 6 ;

    return wxSize(wText, hText);
}

// ----------------------------------------------------------------------------
// Undo/redo
// ----------------------------------------------------------------------------

void wxTextCtrl::Undo()
{
    if (CanUndo())
    {
        GetPeer()->Undo() ;
    }
}

void wxTextCtrl::Redo()
{
    if (CanRedo())
    {
        GetPeer()->Redo() ;
    }
}

bool wxTextCtrl::CanUndo() const
{
    if ( !IsEditable() )
    {
        return false ;
    }
    return GetPeer()->CanUndo() ;
}

bool wxTextCtrl::CanRedo() const
{
    if ( !IsEditable() )
    {
        return false ;
    }
    return GetPeer()->CanRedo() ;
}

void wxTextCtrl::MarkDirty()
{
    m_dirty = true;
}

void wxTextCtrl::DiscardEdits()
{
    m_dirty = false;
}

int wxTextCtrl::GetNumberOfLines() const
{
    return GetPeer()->GetNumberOfLines() ;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    return GetPeer()->XYToPosition( x , y ) ;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    return GetPeer()->PositionToXY(pos , x , y ) ;
}

void wxTextCtrl::ShowPosition(long pos)
{
    return GetPeer()->ShowPosition(pos) ;
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    return GetPeer()->GetLineLength(lineNo) ;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    return GetPeer()->GetLineText(lineNo) ;
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

void wxTextCtrl::OnEraseBackground(wxEraseEvent& event)
{
    // all erasing should be done by the real mac control implementation
    // while this is true for MLTE under classic, the HITextView is somehow
    // transparent but background erase is not working correctly, so intercept
    // things while we can...
    event.Skip() ;
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    int key = event.GetKeyCode() ;
    bool eat_key = false ;

    if ( key == 'c' && event.MetaDown() )
    {
        if ( CanCopy() )
            Copy() ;
        return ;
    }

    if ( !IsEditable() && key != WXK_LEFT && key != WXK_RIGHT && key != WXK_DOWN && key != WXK_UP && key != WXK_TAB &&
        !( key == WXK_RETURN && ( (m_windowStyle & wxPROCESS_ENTER) || (m_windowStyle & wxTE_MULTILINE) ) )
/*        && key != WXK_PRIOR && key != WXK_NEXT && key != WXK_HOME && key != WXK_END */
        )
    {
        // eat it
        return ;
    }

    // Check if we have reached the max # of chars, but still allow navigation and deletion
    if ( !IsMultiLine() && GetValue().Length() >= m_maxLength &&
        key != WXK_LEFT && key != WXK_RIGHT && key != WXK_TAB &&
        key != WXK_BACK && !( key == WXK_RETURN && (m_windowStyle & wxPROCESS_ENTER) )
       )
    {
        // eat it, we don't want to add more than allowed # of characters
        return;
    }

    // assume that any key not processed yet is going to modify the control
    m_dirty = true;

    if ( key == 'v' && event.MetaDown() )
    {
        if ( CanPaste() )
            Paste() ;
        return ;
    }
    if ( key == 'x' && event.MetaDown() )
    {
        if ( CanCut() )
            Cut() ;
        return ;
    }
    switch ( key )
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

                // this will make wxWidgets eat the ENTER key so that
                // we actually prevent line wrapping in a single line
                // text control
                eat_key = true;
            }

            break;

        case WXK_TAB:
            if ( !(m_windowStyle & wxTE_PROCESS_TAB))
            {
                int flags = 0;
                if (!event.ShiftDown())
                    flags |= wxNavigationKeyEvent::IsForward ;
                if (event.ControlDown())
                    flags |= wxNavigationKeyEvent::WinChange ;
                Navigate(flags);
                return;
            }
            else
            {
                // This is necessary (don't know why) or the tab will not
                // be inserted.
                WriteText(wxT("\t"));
            }

            break;
    }

    if (!eat_key)
    {
        // perform keystroke handling
        if ( wxTheApp->MacGetCurrentEvent() != NULL && wxTheApp->MacGetCurrentEventHandlerCallRef() != NULL )
            CallNextEventHandler((EventHandlerCallRef)wxTheApp->MacGetCurrentEventHandlerCallRef() , (EventRef) wxTheApp->MacGetCurrentEvent() ) ;
        else
        {
            EventRecord rec ;
            if ( wxMacConvertEventToRecord(  (EventRef) wxTheApp->MacGetCurrentEvent() , &rec ) )
            {
                EventRecord *ev = &rec ;
                short keycode ;
                short keychar ;
                keychar = short(ev->message & charCodeMask);
                keycode = short(ev->message & keyCodeMask) >> 8 ;

                m_peer->HandleKey( keycode , keychar , ev->modifiers ) ;
            }
        }
    }
    if ( ( key >= 0x20 && key < WXK_START ) ||
         key == WXK_RETURN ||
         key == WXK_DELETE ||
         key == WXK_BACK)
    {
        wxCommandEvent event1(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event1.SetEventObject( this );
        wxPostEvent(GetEventHandler(),event1);
    }
}

// ----------------------------------------------------------------------------
// standard handlers for standard edit menu events
// ----------------------------------------------------------------------------

void wxTextCtrl::OnCut(wxCommandEvent& WXUNUSED(event))
{
    Cut();
}

void wxTextCtrl::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    Copy();
}

void wxTextCtrl::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    Paste();
}

void wxTextCtrl::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    Undo();
}

void wxTextCtrl::OnRedo(wxCommandEvent& WXUNUSED(event))
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

bool wxTextCtrl::MacSetupCursor( const wxPoint& pt )
{
    return true ;
}
#if !TARGET_API_MAC_OSX
// user pane implementation

void wxTextCtrl::MacControlUserPaneDrawProc(wxInt16 part)
{
}

wxInt16 wxTextCtrl::MacControlUserPaneHitTestProc(wxInt16 x, wxInt16 y)
{
    return kControlNoPart ;
}

wxInt16 wxTextCtrl::MacControlUserPaneTrackingProc(wxInt16 x, wxInt16 y, void* actionProc)
{
    return kControlNoPart ;
}

void wxTextCtrl::MacControlUserPaneIdleProc()
{
}

wxInt16 wxTextCtrl::MacControlUserPaneKeyDownProc(wxInt16 keyCode, wxInt16 charCode, wxInt16 modifiers)
{
    return kControlNoPart ;
}

void wxTextCtrl::MacControlUserPaneActivateProc(bool activating)
{
}

wxInt16 wxTextCtrl::MacControlUserPaneFocusProc(wxInt16 action)
{
    return kControlNoPart ;
}

void wxTextCtrl::MacControlUserPaneBackgroundProc(void* info)
{
}
#endif
// ----------------------------------------------------------------------------
// implementation base class
// ----------------------------------------------------------------------------

wxMacTextControl::wxMacTextControl()
{
}

wxMacTextControl::~wxMacTextControl()
{
}

void wxMacTextControl::SetStyle(long start, long end, const wxTextAttr& style)
{
}

void wxMacTextControl::Copy()
{
}

void wxMacTextControl::Cut()
{
}

void wxMacTextControl::Paste()
{
}

bool wxMacTextControl::CanPaste() const
{
    return false ;
}

void wxMacTextControl::SetEditable(bool editable)
{
}

wxTextPos wxMacTextControl::GetLastPosition() const
{
    return GetStringValue().Length() ;
}

void wxMacTextControl::Replace( long from , long to , const wxString str )
{
}

void wxMacTextControl::Clear()
{
    SetStringValue( wxEmptyString ) ;
}

bool wxMacTextControl::CanUndo() const
{
    return false ;
}

void wxMacTextControl::Undo() { }

bool wxMacTextControl::CanRedo()  const
{
    return false ;
}

void wxMacTextControl::Redo()
{
}

long wxMacTextControl::XYToPosition(long x, long y) const
{
    return 0 ;
}

bool wxMacTextControl::PositionToXY(long pos, long *x, long *y) const
{
    return false ;
}

void wxMacTextControl::ShowPosition( long WXUNUSED(pos) )
{
}

int wxMacTextControl::GetNumberOfLines() const
{
    ItemCount lines = 0 ;
    wxString content = GetStringValue() ;
    lines = 1;
    for (size_t i = 0; i < content.Length() ; i++)
    {
        if (content[i] == '\r') lines++;
    }
    return lines ;
}

wxString wxMacTextControl::GetLineText(long lineNo) const
{
    // TODO change this if possible to reflect real lines
    wxString content = GetStringValue() ;

    // Find line first
    int count = 0;
    for (size_t i = 0; i < content.Length() ; i++)
    {
        if (count == lineNo)
        {
            // Add chars in line then
            wxString tmp;

            for (size_t j = i; j < content.Length(); j++)
            {
                if (content[j] == '\n')
                    return tmp;

                tmp += content[j];
            }

            return tmp;
        }
        if (content[i] == '\n') count++;
    }
    return wxEmptyString ;
}

int  wxMacTextControl::GetLineLength(long lineNo) const
{
    // TODO change this if possible to reflect real lines
    wxString content = GetStringValue() ;

    // Find line first
    int count = 0;
    for (size_t i = 0; i < content.Length() ; i++)
    {
        if (count == lineNo)
        {
            // Count chars in line then
            count = 0;
            for (size_t j = i; j < content.Length(); j++)
            {
                count++;
                if (content[j] == '\n') return count;
            }

            return count;
        }
        if (content[i] == '\n') count++;
    }
    return 0 ;
}

// ----------------------------------------------------------------------------
// standard unicode control implementation
// ----------------------------------------------------------------------------

#if TARGET_API_MAC_OSX

wxMacUnicodeTextControl::wxMacUnicodeTextControl( wxWindow *wxPeer,
                         const wxString& str,
                         const wxPoint& pos,
                         const wxSize& size, long style )
{
    m_font = wxPeer->GetFont() ;
    m_windowStyle = style ;
    Rect bounds = wxMacGetBoundsForControl( wxPeer , pos , size ) ;
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;
    wxMacCFStringHolder cf(st , m_font.GetEncoding()) ;
    CFStringRef cfr = cf ;
    Boolean isPassword = ( m_windowStyle & wxTE_PASSWORD ) != 0 ;
    m_valueTag = isPassword ? kControlEditTextPasswordCFStringTag : kControlEditTextCFStringTag ;
    CreateEditUnicodeTextControl( MAC_WXHWND(wxPeer->MacGetTopLevelWindowRef()), &bounds , cfr , isPassword , NULL , &m_controlRef ) ;

    if ( !(m_windowStyle & wxTE_MULTILINE) )
    {
        SetData<Boolean>( kControlEditTextPart , kControlEditTextSingleLineTag , true ) ;
    }
}

wxMacUnicodeTextControl::~wxMacUnicodeTextControl()
{
}

void wxMacUnicodeTextControl::VisibilityChanged(bool shown)
{
    if ( !(m_windowStyle & wxTE_MULTILINE) && shown )
    {
        // work around a refresh issue insofar as not always the entire content is shown even if this would be possible
        ControlEditTextSelectionRec sel ;
        CFStringRef value = NULL ;

        verify_noerr( GetData<ControlEditTextSelectionRec>( 0, kControlEditTextSelectionTag, &sel ) );
        verify_noerr( GetData<CFStringRef>( 0, m_valueTag , &value ) );
        verify_noerr( SetData<CFStringRef>( 0, m_valueTag, &value ) );
        verify_noerr( SetData<ControlEditTextSelectionRec>( 0, kControlEditTextSelectionTag, &sel ) );

        CFRelease( value ) ;
    }
}
wxString wxMacUnicodeTextControl::GetStringValue() const
{
    wxString result ;
    CFStringRef value = GetData<CFStringRef>(0,m_valueTag) ;
    if ( value )
    {
        wxMacCFStringHolder cf(value) ;
        result = cf.AsString() ;
    }
#if '\n' == 10
    wxMacConvertNewlines13To10( &result ) ;
#else
    wxMacConvertNewlines10To13( &result ) ;
#endif
    return result ;
}
void wxMacUnicodeTextControl::SetStringValue( const wxString &str)
{
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;
    wxMacCFStringHolder cf(st , m_font.GetEncoding() ) ;
    verify_noerr( SetData<CFStringRef>(  0, m_valueTag , cf ) ) ;
}
void wxMacUnicodeTextControl::Copy()
{
    SendHICommand( kHICommandCopy ) ;
}
void wxMacUnicodeTextControl::Cut()
{
    SendHICommand( kHICommandCut ) ;
}
void wxMacUnicodeTextControl::Paste()
{
    SendHICommand( kHICommandPaste ) ;
}
bool wxMacUnicodeTextControl::CanPaste() const
{
    return true ;
}
void wxMacUnicodeTextControl::SetEditable(bool editable)
{
    SetData<Boolean>( 0 , kControlEditTextLockedTag , (Boolean) !editable ) ;
}
void wxMacUnicodeTextControl::Remove( long from , long to )
{
}

void wxMacUnicodeTextControl::GetSelection( long* from, long* to) const
{
    ControlEditTextSelectionRec sel ;
    verify_noerr(GetData<ControlEditTextSelectionRec>(  0, kControlEditTextSelectionTag, &sel ) ) ;
    if ( from ) *from = sel.selStart ;
    if ( to ) *to = sel.selEnd ;
}

void wxMacUnicodeTextControl::SetSelection( long from , long to )
{
    ControlEditTextSelectionRec sel ;
    sel.selStart = from ;
    sel.selEnd = to ;
    SetData<ControlEditTextSelectionRec>( 0 , kControlEditTextSelectionTag, &sel ) ;
}

void wxMacUnicodeTextControl::WriteText(const wxString& str)
{
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;
    #if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
        wxMacCFStringHolder cf(st , m_font.GetEncoding() ) ;
        CFStringRef value = cf ;
        SetData<CFStringRef>( 0, kControlEditTextInsertCFStringRefTag, &value );
    #else
        wxString val = GetStringValue() ;
        long start , end ;
        GetSelection( &start , &end ) ;
        val.Remove( start , end - start ) ;
        val.insert( start , str ) ;
        SetStringValue( val ) ;
        SetSelection( start + str.Length() , start + str.Length() ) ;
    #endif
}

#endif

// ----------------------------------------------------------------------------
// MLTE control implementation (common part)
// ----------------------------------------------------------------------------

#if TARGET_API_MAC_OSX == 0
// declaration needed because of one line in the code...
static void TPUpdateVisibility(ControlRef theControl) ;
#endif

// if mlte is on read only , no changes at all are allowed, not even from
// procedural API, in order to allow changes via API all the same we must undo
// the readonly status while we are executing, this class helps to do so

class EditHelper
{
public :
    EditHelper( TXNObject txn )
    {
        TXNControlTag tag[] = { kTXNIOPrivilegesTag } ;
        m_txn = txn ;
        TXNGetTXNObjectControls( m_txn , 1 , tag , m_data ) ;
        if ( m_data[0].uValue == kTXNReadOnly )
        {
            TXNControlData data[] = { { kTXNReadWrite } } ;
            TXNSetTXNObjectControls( m_txn , false , 1 , tag , data ) ;
        }
    }
    ~EditHelper()
    {
        TXNControlTag tag[] = { kTXNIOPrivilegesTag } ;
        if ( m_data[0].uValue == kTXNReadOnly )
        {
            TXNSetTXNObjectControls( m_txn , false , 1 , tag , m_data ) ;
        }
    }
    protected :
        TXNObject m_txn ;
        TXNControlData m_data[1] ;
} ;

wxString wxMacMLTEControl::GetStringValue() const
{
    wxString result ;
    OSStatus err ;
    Size actualSize = 0;
    {
#if wxUSE_UNICODE
        Handle theText ;
        err = TXNGetDataEncoded( m_txn , kTXNStartOffset, kTXNEndOffset, &theText , kTXNUnicodeTextData );
        // all done
        if ( err )
        {
            actualSize = 0 ;
        }
        else
        {
            actualSize = GetHandleSize( theText ) / sizeof( UniChar) ;
            if ( actualSize > 0 )
            {
                wxChar *ptr = NULL ;
#if SIZEOF_WCHAR_T == 2
                ptr = new wxChar[actualSize + 1 ] ;
                wxStrncpy( ptr , (wxChar*) *theText , actualSize ) ;
#else
                SetHandleSize( theText , ( actualSize + 1 ) * sizeof( UniChar ) ) ;
                HLock( theText ) ;
                (((UniChar*)*theText)[actualSize]) = 0 ;
                wxMBConvUTF16BE converter ;
                size_t noChars = converter.MB2WC( NULL , (const char*)*theText , 0 ) ;
                ptr = new wxChar[noChars + 1] ;

                noChars = converter.MB2WC( ptr , (const char*)*theText , noChars ) ;
                ptr[noChars] = 0 ;
                HUnlock( theText ) ;
#endif
                ptr[actualSize] = 0 ;
                result = wxString( ptr ) ;
                delete[] ptr ;
            }
            DisposeHandle( theText ) ;
        }
#else
        Handle theText ;
        err = TXNGetDataEncoded( m_txn , kTXNStartOffset, kTXNEndOffset, &theText , kTXNTextData );
        // all done
        if ( err )
        {
            actualSize = 0 ;
        }
        else
        {
            actualSize = GetHandleSize( theText ) ;
            if ( actualSize > 0 )
            {
                HLock( theText ) ;
                result = wxString( *theText , wxConvLocal , actualSize ) ;
                HUnlock( theText ) ;
            }
            DisposeHandle( theText ) ;
        }
#endif
    }
#if '\n' == 10
    wxMacConvertNewlines13To10( &result ) ;
#else
    wxMacConvertNewlines10To13( &result ) ;
#endif
    return result ;
}

void wxMacMLTEControl::SetStringValue( const wxString &str)
{
    wxString st = str ;

    wxMacConvertNewlines10To13( &st ) ;
    EditHelper help(m_txn) ;

    // wxMacWindowClipper c( this ) ;
#if !TARGET_API_MAC_OSX
    // otherwise scrolling might have problems ?
    TPUpdateVisibility( m_controlRef ) ;
#endif
    SetTXNData( st , kTXNStartOffset, kTXNEndOffset ) ;
    TXNSetSelection( m_txn, 0, 0);
    TXNShowSelection( m_txn, kTXNShowStart);
}

TXNFrameOptions wxMacMLTEControl::FrameOptionsFromWXStyle( long wxStyle )
{
    TXNFrameOptions frameOptions =
        kTXNDontDrawCaretWhenInactiveMask ;
    if ( ! ( wxStyle & wxTE_NOHIDESEL ) )
        frameOptions |= kTXNDontDrawSelectionWhenInactiveMask ;

    if ( wxStyle & wxTE_MULTILINE )
    {
        if ( ! ( wxStyle & wxTE_DONTWRAP ) )
            frameOptions |= kTXNAlwaysWrapAtViewEdgeMask ;
        else
        {
            frameOptions |= kTXNAlwaysWrapAtViewEdgeMask ;
            frameOptions |= kTXNWantHScrollBarMask ;
        }

        if ( !(wxStyle & wxTE_NO_VSCROLL ) )
            frameOptions |= kTXNWantVScrollBarMask ;
    }
    else
        frameOptions |= kTXNSingleLineOnlyMask ;

    if ( wxStyle & wxHSCROLL )
        frameOptions |= kTXNWantHScrollBarMask ;

    return frameOptions ;
}

void wxMacMLTEControl::AdjustCreationAttributes( const wxColour &background, bool visible )
{
    TXNControlTag iControlTags[3] = { kTXNDoFontSubstitution, kTXNWordWrapStateTag };
    TXNControlData iControlData[3] = { {false}, {kTXNNoAutoWrap} };
    int toptag = 2 ;
#if TARGET_API_MAC_OSX
    iControlTags[2] = kTXNVisibilityTag ;
    iControlData[2].uValue = visible ;
    toptag++ ;
#endif

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        if (m_windowStyle & wxTE_DONTWRAP)
            iControlData[1].uValue = kTXNNoAutoWrap ;
        else
            iControlData[1].uValue = kTXNAutoWrap ;
    }
    verify_noerr( TXNSetTXNObjectControls( m_txn, false, toptag,
                                        iControlTags, iControlData )) ;

    // setting the default font

    Str255 fontName ;
    SInt16 fontSize ;
    Style fontStyle ;

    GetThemeFont(kThemeSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;

    TXNTypeAttributes typeAttr[] =
    {
        {   kTXNQDFontNameAttribute , kTXNQDFontNameAttributeSize , { (void*) fontName } } ,
        {   kTXNQDFontSizeAttribute , kTXNFontSizeAttributeSize , { (void*) (fontSize << 16) } } ,
        {   kTXNQDFontStyleAttribute , kTXNQDFontStyleAttributeSize , {  (void*) normal } } ,
    } ;

    verify_noerr( TXNSetTypeAttributes (m_txn, sizeof( typeAttr ) / sizeof(TXNTypeAttributes) , typeAttr,
          kTXNStartOffset,
          kTXNEndOffset) );

    if ( m_windowStyle & wxTE_PASSWORD )
    {
        UniChar c = 0xA5 ;
        verify_noerr(TXNEchoMode( m_txn , c , 0 , true )) ;
    }

    TXNBackground tback;
    tback.bgType = kTXNBackgroundTypeRGB;
    tback.bg.color = MAC_WXCOLORREF( background.GetPixel() );
    TXNSetBackground( m_txn , &tback);
}

void wxMacMLTEControl::SetBackground( const wxBrush &brush )
{
    // currently only solid background are supported
    TXNBackground tback;
    tback.bgType = kTXNBackgroundTypeRGB;
    tback.bg.color = MAC_WXCOLORREF( brush.GetColour().GetPixel() );
    TXNSetBackground( m_txn , &tback);
}

void wxMacMLTEControl::TXNSetAttribute( const wxTextAttr& style , long from , long to)
{
    TXNTypeAttributes typeAttr[4] ;
    Str255 fontName = "\pMonaco" ;
    SInt16 fontSize = 12 ;
    Style fontStyle = normal ;
    RGBColor color ;
    int attrCounter = 0 ;
    if ( style.HasFont() )
    {
        const wxFont &font = style.GetFont() ;
        wxMacStringToPascal( font.GetFaceName() , fontName ) ;
        fontSize = font.GetPointSize() ;
        if ( font.GetUnderlined() )
            fontStyle |= underline ;
        if ( font.GetWeight() == wxBOLD )
            fontStyle |= bold ;
        if ( font.GetStyle() == wxITALIC )
            fontStyle |= italic ;

        typeAttr[attrCounter].tag = kTXNQDFontNameAttribute ;
        typeAttr[attrCounter].size = kTXNQDFontNameAttributeSize ;
        typeAttr[attrCounter].data.dataPtr = (void*) fontName ;
        typeAttr[attrCounter+1].tag = kTXNQDFontSizeAttribute ;
        typeAttr[attrCounter+1].size = kTXNFontSizeAttributeSize ;
        typeAttr[attrCounter+1].data.dataValue =  (fontSize << 16) ;
        typeAttr[attrCounter+2].tag = kTXNQDFontStyleAttribute ;
        typeAttr[attrCounter+2].size = kTXNQDFontStyleAttributeSize ;
        typeAttr[attrCounter+2].data.dataValue = fontStyle ;
        attrCounter += 3 ;
    }
    if ( style.HasTextColour() )
    {
        typeAttr[attrCounter].tag = kTXNQDFontColorAttribute ;
        typeAttr[attrCounter].size = kTXNQDFontColorAttributeSize ;
        typeAttr[attrCounter].data.dataPtr = (void*) &color ;
        color = MAC_WXCOLORREF(style.GetTextColour().GetPixel()) ;
        attrCounter += 1 ;
    }
    if ( attrCounter > 0 )
    {
        verify_noerr( TXNSetTypeAttributes ( m_txn , attrCounter , typeAttr, from , to) );
    }
}

void wxMacMLTEControl::SetFont( const wxFont & font , const wxColour& foreground , long windowStyle )
{
    EditHelper help(m_txn) ;
    TXNSetAttribute( wxTextAttr(foreground,wxNullColour,font) , kTXNStartOffset,kTXNEndOffset ) ;
}
void wxMacMLTEControl::SetStyle(long start, long end, const wxTextAttr& style)
{
    EditHelper help(m_txn) ;
    TXNSetAttribute( style , start,end ) ;
}

void wxMacMLTEControl::Copy()
{
    ClearCurrentScrap();
    TXNCopy(m_txn);
    TXNConvertToPublicScrap();
}

void wxMacMLTEControl::Cut()
{
    ClearCurrentScrap();
    TXNCut(m_txn);
    TXNConvertToPublicScrap();
}

void wxMacMLTEControl::Paste()
{
    TXNConvertFromPublicScrap();
    TXNPaste(m_txn);
}

bool wxMacMLTEControl::CanPaste() const
{
    return TXNIsScrapPastable() ;
}

void wxMacMLTEControl::SetEditable(bool editable)
{
    TXNControlTag tag[] = { kTXNIOPrivilegesTag } ;
    TXNControlData data[] = { { editable ? kTXNReadWrite : kTXNReadOnly } } ;
    TXNSetTXNObjectControls( m_txn , false , sizeof(tag) / sizeof (TXNControlTag) , tag , data ) ;
}

wxTextPos wxMacMLTEControl::GetLastPosition() const
{
    wxTextPos actualsize = 0 ;

    Handle theText ;
    OSErr err = TXNGetDataEncoded( m_txn, kTXNStartOffset, kTXNEndOffset, &theText , kTXNTextData );
    /* all done */
    if ( err )
    {
        actualsize = 0 ;
    }
    else
    {
        actualsize = GetHandleSize( theText ) ;
        DisposeHandle( theText ) ;
    }

    return actualsize ;
}

void wxMacMLTEControl::Replace( long from , long to , const wxString str )
{
    wxString value = str ;
    wxMacConvertNewlines10To13( &value ) ;

    EditHelper help( m_txn ) ;

    TXNSetSelection(m_txn , from , to ) ;
    TXNClear( m_txn ) ;
    SetTXNData( value , kTXNUseCurrentSelection, kTXNUseCurrentSelection ) ;
}

void wxMacMLTEControl::Remove( long from , long to )
{
    EditHelper help( m_txn ) ;

    TXNSetSelection(m_txn , from , to ) ;
    TXNClear( m_txn ) ;
}

void wxMacMLTEControl::GetSelection( long* from, long* to) const
{
    TXNGetSelection( m_txn , (TXNOffset*) from , (TXNOffset*) to ) ;
}

void wxMacMLTEControl::SetSelection( long from , long to )
{
    /* change the selection */
    if ((from == -1) && (to == -1))
        TXNSelectAll(m_txn);
    else
        TXNSetSelection( m_txn, from, to);
    TXNShowSelection( m_txn, kTXNShowStart);
}

void wxMacMLTEControl::WriteText(const wxString& str)
{
    EditHelper helper( m_txn ) ;
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;

    long start , end , dummy ;
    GetSelection( &start , &dummy ) ;
    SetTXNData( st , kTXNUseCurrentSelection, kTXNUseCurrentSelection ) ;
    GetSelection( &dummy , &end ) ;
    // TODO SetStyle( start , end , GetDefaultStyle() ) ;
}

void wxMacMLTEControl::Clear()
{
    EditHelper st(m_txn) ;
    TXNSetSelection( m_txn , kTXNStartOffset , kTXNEndOffset ) ;
    TXNClear(m_txn);
}

bool wxMacMLTEControl::CanUndo() const
{
    return TXNCanUndo( m_txn , NULL ) ;
}

void wxMacMLTEControl::Undo()
{
    TXNUndo( m_txn ) ;
}

bool wxMacMLTEControl::CanRedo()  const
{
    return TXNCanRedo( m_txn , NULL ) ;
}

void wxMacMLTEControl::Redo()
{
    TXNRedo( m_txn ) ;
}

int wxMacMLTEControl::GetNumberOfLines() const
{
    ItemCount lines = 0 ;
    TXNGetLineCount(m_txn, &lines ) ;
    return lines ;
}

long wxMacMLTEControl::XYToPosition(long x, long y) const
{
    Point curpt ;

    wxTextPos lastpos = GetLastPosition() ;

    // TODO find a better implementation : while we can get the
    // line metrics of a certain line, we don't get its starting
    // position, so it would probably be rather a binary search
    // for the start position
    long xpos = 0 ;
    long ypos = 0 ;
    int lastHeight = 0 ;

    ItemCount n ;
    for ( n = 0 ; n <= (ItemCount) lastpos ; ++n )
    {
        if ( y == ypos && x == xpos )
            return n ;

        TXNOffsetToPoint( m_txn ,  n , &curpt);

        if ( curpt.v > lastHeight )
        {
            xpos = 0 ;
            if ( n > 0 )
                ++ypos ;
            lastHeight = curpt.v ;
        }
        else
            ++xpos ;
    }
    return 0 ;
}

bool wxMacMLTEControl::PositionToXY(long pos, long *x, long *y) const
{
    Point curpt ;

    wxTextPos lastpos = GetLastPosition() ;

    if ( y ) *y = 0 ;
    if ( x ) *x = 0 ;

    if ( pos <= lastpos )
    {
        // TODO find a better implementation : while we can get the
        // line metrics of a certain line, we don't get its starting
        // position, so it would probably be rather a binary search
        // for the start position
        long xpos = 0 ;
        long ypos = 0 ;
        int lastHeight = 0 ;

        ItemCount n ;
        for ( n = 0 ; n <= (ItemCount) pos ; ++n )
        {
            TXNOffsetToPoint(m_txn ,  n , &curpt);

            if ( curpt.v > lastHeight )
            {
                xpos = 0 ;
                if ( n > 0 )
                    ++ypos ;
                lastHeight = curpt.v ;
            }
            else
                ++xpos ;
        }
        if ( y ) *y = ypos ;
        if ( x ) *x = xpos ;
    }

    return false ;
}

void wxMacMLTEControl::ShowPosition( long pos )
{
#if TARGET_RT_MAC_MACHO && defined(AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER)
    {
        Point current ;
        Point desired ;
        TXNOffset selstart , selend ;
        TXNGetSelection(  m_txn , &selstart , &selend) ;
        TXNOffsetToPoint( m_txn,  selstart , &current);
        TXNOffsetToPoint( m_txn,  pos , &desired);
        //TODO use HIPoints for 10.3 and above
        if ( (UInt32) TXNScroll != (UInt32) kUnresolvedCFragSymbolAddress )
        {
            OSErr theErr = noErr;
            SInt32 dv = desired.v - current.v ;
            SInt32 dh = desired.h - current.h ;
            TXNShowSelection( m_txn , true ) ;
            theErr = TXNScroll( m_txn, kTXNScrollUnitsInPixels , kTXNScrollUnitsInPixels , &dv , &dh );
            wxASSERT_MSG( theErr == noErr, _T("TXNScroll returned an error!") );
        }
    }
#endif
}

void wxMacMLTEControl::SetTXNData( const wxString& st , TXNOffset start , TXNOffset end )
{
#if wxUSE_UNICODE
#if SIZEOF_WCHAR_T == 2
    size_t len = st.Len() ;
    TXNSetData( m_txn , kTXNUnicodeTextData,  (void*)st.wc_str(), len * 2,
      start, end);
#else
    wxMBConvUTF16BE converter ;
    ByteCount byteBufferLen = converter.WC2MB( NULL , st.wc_str() , 0 ) ;
    UniChar *unibuf = (UniChar*) malloc(byteBufferLen) ;
    converter.WC2MB( (char*) unibuf , st.wc_str() , byteBufferLen ) ;
    TXNSetData( m_txn , kTXNUnicodeTextData,  (void*)unibuf, byteBufferLen ,
      start, end);
    free( unibuf ) ;
#endif
#else
    wxCharBuffer text =  st.mb_str(wxConvLocal)  ;
    TXNSetData( m_txn , kTXNTextData,  (void*)text.data(), strlen( text ) ,
      start, end);
#endif
}


wxString wxMacMLTEControl::GetLineText(long lineNo) const
{
    wxString line ;

    if ( lineNo < GetNumberOfLines() )
    {
        long ypos = 0 ;

        Fixed   lineWidth,
                lineHeight,
                currentHeight = 0;

        // get the first possible position in the control
        Point firstPoint;
        TXNOffsetToPoint(m_txn, 0, &firstPoint);

        // Iterate through the lines until we reach the one we want,
        // adding to our current y pixel point position
        while (ypos < lineNo)
        {
            TXNGetLineMetrics(m_txn, ypos++, &lineWidth, &lineHeight);
            currentHeight += lineHeight;
        }

        Point thePoint = { firstPoint.v + (currentHeight >> 16), firstPoint.h + (0) };
        TXNOffset theOffset;
        TXNPointToOffset(m_txn, thePoint, &theOffset);

        wxString content = GetStringValue() ;
        Point currentPoint = thePoint;
        while(thePoint.v == currentPoint.v && theOffset < content.length())
        {
            line += content[theOffset];
            TXNOffsetToPoint(m_txn, ++theOffset, &currentPoint);
        }
    }
    return line ;
}

int  wxMacMLTEControl::GetLineLength(long lineNo) const
{
    int theLength = 0;

    if ( lineNo < GetNumberOfLines() )
    {
        long ypos = 0 ;

        Fixed   lineWidth,
                lineHeight,
                currentHeight = 0;

        // get the first possible position in the control
        Point firstPoint;
        TXNOffsetToPoint(m_txn, 0, &firstPoint);

        // Iterate through the lines until we reach the one we want,
        // adding to our current y pixel point position
        while (ypos < lineNo)
        {
            TXNGetLineMetrics(m_txn, ypos++, &lineWidth, &lineHeight);
            currentHeight += lineHeight;
        }

        Point thePoint = { firstPoint.v + (currentHeight >> 16), firstPoint.h + (0) };
        TXNOffset theOffset;
        TXNPointToOffset(m_txn, thePoint, &theOffset);

        wxString content = GetStringValue() ;
        Point currentPoint = thePoint;
        while(thePoint.v == currentPoint.v && theOffset < content.length())
        {
            ++theLength;
            TXNOffsetToPoint(m_txn, ++theOffset, &currentPoint);
        }
    }
    return theLength ;
}


// ----------------------------------------------------------------------------
// MLTE control implementation (classic part)
// ----------------------------------------------------------------------------

// CS:TODO we still have a problem getting properly at the text events of a control because under Carbon
// the MLTE engine registers itself for the key events thus the normal flow never occurs, the only measure for the
// moment is to avoid setting the true focus on the control, the proper solution at the end would be to have
// an alternate path for carbon key events that routes automatically into the same wx flow of events

/* part codes */

/* kmUPTextPart is the part code we return to indicate the user has clicked
in the text area of our control */
#define kmUPTextPart 1


/* routines for using existing user pane controls.
These routines are useful for cases where you would like to use an
existing user pane control in, say, a dialog window as a scrolling
text edit field.*/

/* Utility Routines */

/* kUserClickedToFocusPart is a part code we pass to the SetKeyboardFocus
routine.  In our focus switching routine this part code is understood
as meaning 'the user has clicked in the control and we need to switch
the current focus to ourselves before we can continue'. */
#define kUserClickedToFocusPart 100

/* STPTextPaneVars is a structure used for storing the the mUP Control's
internal variables and state information.  A handle to this record is
stored in the pane control's reference value field using the
SetControlReference routine. */

class STPTextPaneVars {
public :
    /* OS records referenced */
    TXNObject fTXNRec; /* the txn record */
    TXNFrameID fTXNFrame; /* the txn frame ID */
    ControlRef fUserPaneRec;  /* handle to the user pane control */
    WindowPtr fOwner; /* window containing control */
    GrafPtr fDrawingEnvironment; /* grafport where control is drawn */
    /* flags */
    Boolean fInFocus; /* true while the focus rect is drawn around the control */
    Boolean fIsActive; /* true while the control is drawn in the active state */
    Boolean fTXNObjectActive; /* reflects the activation state of the text edit record */
    Boolean fFocusDrawState; /* true if focus is drawn (default: true) */
    /* calculated locations */
    Rect fRBounds; /* control bounds */
    Rect fRTextArea; /* area where the text is drawn */
    Rect fRFocusOutline;  /* rectangle used to draw the focus box */
    Rect fRTextOutline; /* rectangle used to draw the border */
    RgnHandle fRTextOutlineRegion; /* background region for the text, erased before calling TEUpdate */
    /* our focus advance override routine */
    EventHandlerUPP handlerUPP;
    EventHandlerRef handlerRef;
    bool fNoBorders ;
    bool fMultiline ;
    bool fVisible ;
} ;

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

// one place for calculating all
static void TPCalculateBounds(STPTextPaneVars *varsp, const Rect& bounds)
{
    SetRect(&varsp->fRBounds, bounds.left, bounds.top, bounds.right, bounds.bottom);
    SetRect(&varsp->fRFocusOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
    // eventually make TextOutline inset 1,1
    SetRect(&varsp->fRTextOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
    if ( !varsp->fNoBorders )
    {
        SetRect(&varsp->fRTextArea, bounds.left + 2 , bounds.top + (varsp->fMultiline ? 0 : 2) ,
                bounds.right - (varsp->fMultiline ? 0 : 2), bounds.bottom - (varsp->fMultiline ? 0 : 2));
    }
    else
    {
        SetRect(&varsp->fRTextArea, bounds.left , bounds.top ,
                bounds.right, bounds.bottom);
    }
}

OSStatus MLTESetObjectVisibility( STPTextPaneVars *varsp, Boolean vis , long wxStyle)
{
    OSStatus err = noErr ;
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(varsp->fUserPaneRec);
    if ( textctrl )
    {
#if TARGET_API_MAC_OSX
        TXNControlTag iControlTags[1] = { kTXNVisibilityTag };
        TXNControlData iControlData[1] = {{ vis }};
        err = ::TXNSetTXNObjectControls( varsp->fTXNRec, false, 1, iControlTags, iControlData );
#endif
        Rect bounds ;
        UMAGetControlBoundsInWindowCoords( varsp->fUserPaneRec, &bounds);
        TPCalculateBounds( varsp , bounds ) ;
        if ( vis )
        {
            wxMacWindowClipper cl(textctrl) ;
            TXNSetFrameBounds( varsp->fTXNRec, varsp->fRTextArea.top, varsp->fRTextArea.left,
                               varsp->fRTextArea.bottom, varsp->fRTextArea.right, varsp->fTXNFrame);
            TXNShowSelection( varsp->fTXNRec, kTXNShowStart);
        }
        else
        {
#if TARGET_API_MAC_OSX
            // in 10.2 the scrollbars are still actively redrawn when using only the code above
            if ( UMAGetSystemVersion() < 0x1030 )
            {
                TXNSetFrameBounds( varsp->fTXNRec, varsp->fRTextArea.top + 20000 , varsp->fRTextArea.left + 20000 ,
                                   varsp->fRTextArea.bottom + 20000 , varsp->fRTextArea.right + 20000 , varsp->fTXNFrame);
            }
#endif
        }
    }
    return err ;
}

// make sure we don't miss changes as carbon events are not available for these under classic
static void TPUpdateVisibility(ControlRef theControl) {
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return ;

    STPTextPaneVars *varsp = (STPTextPaneVars *) ((wxMacMLTEClassicControl*)textctrl->GetPeer())->m_macTXNvars ;

    Rect bounds ;
    UMAGetControlBoundsInWindowCoords(theControl, &bounds);
    if ( textctrl->MacIsReallyShown() != varsp->fVisible )
    {
        // invalidate old position
        // InvalWindowRect( GetControlOwner( theControl ) , &varsp->fRBounds ) ;
        varsp->fVisible = textctrl->MacIsReallyShown() ;
    }
    if ( !EqualRect( &bounds , &varsp->fRBounds ) )
    {
        // old position
        Rect oldBounds = varsp->fRBounds ;
        TPCalculateBounds( varsp , bounds ) ;
        // we only recalculate when visible, otherwise scrollbars get drawn at incorrect places
        if ( varsp->fVisible )
        {
            wxMacWindowClipper cl(textctrl) ;
            TXNSetFrameBounds( varsp->fTXNRec, varsp->fRTextArea.top, varsp->fRTextArea.left,
                               varsp->fRTextArea.bottom, varsp->fRTextArea.right, varsp->fTXNFrame);
        }
        InvalWindowRect( GetControlOwner( theControl ) , &oldBounds ) ;
        InvalWindowRect( GetControlOwner( theControl ) , &varsp->fRBounds ) ;
    }
}

// make correct activations
static void TPActivatePaneText(STPTextPaneVars *varsp, Boolean setActive) {
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(varsp->fUserPaneRec);
    if (varsp->fTXNObjectActive != setActive && textctrl->MacIsReallyShown() )
    {
        varsp->fTXNObjectActive = setActive;
        TXNActivate(varsp->fTXNRec, varsp->fTXNFrame, varsp->fTXNObjectActive);
        if (varsp->fInFocus)
            TXNFocus( varsp->fTXNRec, varsp->fTXNObjectActive);
    }
}

// update focus outlines
static void TPRedrawFocusOutline(STPTextPaneVars *varsp) {
    /* state changed */
    if (varsp->fFocusDrawState != (varsp->fIsActive && varsp->fInFocus))
    {
        varsp->fFocusDrawState = (varsp->fIsActive && varsp->fInFocus);
        // DrawThemeFocusRect(&varsp->fRFocusOutline, varsp->fFocusDrawState);
    }
}

// update TXN focus state
static void TPFocusPaneText(STPTextPaneVars *varsp, Boolean setFocus) {
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(varsp->fUserPaneRec);

    if (varsp->fInFocus != setFocus && textctrl->MacIsReallyShown()) {
        varsp->fInFocus = setFocus;
        TXNFocus( varsp->fTXNRec, varsp->fInFocus);
    }
}

// draw the control
static pascal void TPPaneDrawProc(ControlRef theControl, ControlPartCode thePart) {
    /* set up our globals */

    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return ;
    TPUpdateVisibility( theControl ) ;

    STPTextPaneVars *varsp = (STPTextPaneVars *) ((wxMacMLTEClassicControl*)textctrl->GetPeer())->m_macTXNvars ;
    if ( textctrl->MacIsReallyShown() )
    {
        wxMacWindowClipper clipper( textctrl ) ;
        TXNDraw(varsp->fTXNRec, NULL);
        if ( !varsp->fNoBorders )
            DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
        TPRedrawFocusOutline( varsp ) ;
    }
}


/* TPPaneHitTestProc is called when the control manager would
like to determine what part of the control the mouse resides over.
We also call this routine from our tracking proc to determine how
to handle mouse clicks. */
static pascal ControlPartCode TPPaneHitTestProc(ControlRef theControl, Point where) {
    ControlPartCode result;
    /* set up our locals and lock down our globals*/
    result = 0;
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return 0 ;
    TPUpdateVisibility( theControl ) ;
    STPTextPaneVars *varsp = (STPTextPaneVars *) ((wxMacMLTEClassicControl*)textctrl->GetPeer())->m_macTXNvars ;
    if (textctrl->MacIsReallyShown() )
    {
        if (PtInRect(where, &varsp->fRBounds))
            result = kmUPTextPart;
        else
        {
            // sometimes we get the coords also in control local coordinates, therefore test again
            if ( textctrl->MacGetTopLevelWindow()->MacUsesCompositing() )
            {
                int x = 0 , y = 0 ;
                textctrl->MacClientToRootWindow( &x , &y ) ;
                where.h += x ;
                where.v += y ;
            }
            if (PtInRect(where, &varsp->fRBounds))
                result = kmUPTextPart;
            else
                result = 0;
        }
    }
    return result;
}





/* TPPaneTrackingProc is called when the mouse is being held down
over our control.  This routine handles clicks in the text area
and in the scroll bar. */
static pascal ControlPartCode TPPaneTrackingProc(ControlRef theControl, Point startPt, ControlActionUPP actionProc) {

    ControlPartCode partCodeResult;
    /* make sure we have some variables... */
    partCodeResult = 0;
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return 0;
    TPUpdateVisibility( theControl ) ;
    STPTextPaneVars *varsp = (STPTextPaneVars *) ((wxMacMLTEClassicControl*)textctrl->GetPeer())->m_macTXNvars ;
    if (textctrl->MacIsReallyShown() )
    {
        /* we don't do any of these functions unless we're in focus */
        if ( ! varsp->fInFocus) {
            WindowPtr owner;
            owner = GetControlOwner(theControl);
            ClearKeyboardFocus(owner);
            SetKeyboardFocus(owner, theControl, kUserClickedToFocusPart);
        }
        /* find the location for the click */
        // for compositing, we must convert these into toplevel window coordinates, because hittesting expects them
        if ( textctrl->MacGetTopLevelWindow()->MacUsesCompositing() )
        {
            int x = 0 , y = 0 ;
            textctrl->MacClientToRootWindow( &x , &y ) ;
            startPt.h += x ;
            startPt.v += y ;
        }

        switch (TPPaneHitTestProc(theControl, startPt))
        {
            /* handle clicks in the text part */
            case kmUPTextPart:
            {
                wxMacWindowClipper clipper( textctrl ) ;

                EventRecord rec ;
                ConvertEventRefToEventRecord( (EventRef) wxTheApp->MacGetCurrentEvent() , &rec ) ;
                TXNClick( varsp->fTXNRec, &rec );

            }
                break;

        }
    }
    return partCodeResult;
}


/* TPPaneIdleProc is our user pane idle routine.  When our text field
is active and in focus, we use this routine to set the cursor. */
static pascal void TPPaneIdleProc(ControlRef theControl) {
    /* set up locals */
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return ;
    TPUpdateVisibility( theControl ) ;
    STPTextPaneVars *varsp = (STPTextPaneVars *) ((wxMacMLTEClassicControl*)textctrl->GetPeer())->m_macTXNvars ;
    if (textctrl->MacIsReallyShown()) {
        /* if we're not active, then we have nothing to say about the cursor */
        if (varsp->fIsActive) {
            Rect bounds;
            Point mousep;

            wxMacWindowClipper clipper( textctrl ) ;
            GetMouse(&mousep);
            /* there's a 'focus thing' and an 'unfocused thing' */
            if (varsp->fInFocus) {
                /* flash the cursor */
                SetPort(varsp->fDrawingEnvironment);
                TXNIdle(varsp->fTXNRec);
                /* set the cursor */
                if (PtInRect(mousep, &varsp->fRTextArea)) {
                    RgnHandle theRgn;
                    RectRgn((theRgn = NewRgn()), &varsp->fRTextArea);
                    TXNAdjustCursor(varsp->fTXNRec, theRgn);
                    DisposeRgn(theRgn);
                }
                else
                {
                    // SetThemeCursor(kThemeArrowCursor);
                }
            } else {
                /* if it's in our bounds, set the cursor */
                UMAGetControlBoundsInWindowCoords(theControl, &bounds);
                if (PtInRect(mousep, &bounds))
                {
                    //    SetThemeCursor(kThemeArrowCursor);
                }
            }
        }
    }
}


/* TPPaneKeyDownProc is called whenever a keydown event is directed
at our control.  Here, we direct the keydown event to the text
edit record and redraw the scroll bar and text field as appropriate. */
static pascal ControlPartCode TPPaneKeyDownProc(ControlRef theControl,
                                                SInt16 keyCode, SInt16 charCode, SInt16 modifiers) {

    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return 0;
    TPUpdateVisibility( theControl ) ;

    STPTextPaneVars *varsp = (STPTextPaneVars *) ((wxMacMLTEClassicControl*)textctrl->GetPeer())->m_macTXNvars ;
    if (varsp->fInFocus)
    {
        /* turn autoscrolling on and send the key event to text edit */
        wxMacWindowClipper clipper( textctrl ) ;
        EventRecord ev ;
        memset( &ev , 0 , sizeof( ev ) ) ;
        ev.what = keyDown ;
        ev.modifiers = modifiers ;
        ev.message = (( keyCode << 8 ) & keyCodeMask ) + ( charCode & charCodeMask ) ;
        TXNKeyDown( varsp->fTXNRec, &ev);
    }
    return kControlEntireControl;
}


/* TPPaneActivateProc is called when the window containing
the user pane control receives activate events. Here, we redraw
the control and it's text as necessary for the activation state. */
static pascal void TPPaneActivateProc(ControlRef theControl, Boolean activating) {
    /* set up locals */
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);

    if ( textctrl == NULL )
        return ;
    TPUpdateVisibility( theControl ) ;

    STPTextPaneVars *varsp = (STPTextPaneVars *) ((wxMacMLTEClassicControl*)textctrl->GetPeer())->m_macTXNvars ;

    varsp->fIsActive = activating;
    wxMacWindowClipper clipper( textctrl ) ;
    TPActivatePaneText(varsp, varsp->fIsActive && varsp->fInFocus);
    /* redraw the frame */
    if ( textctrl->MacIsReallyShown() )
    {
        if ( !varsp->fNoBorders )
            DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
        TPRedrawFocusOutline( varsp ) ;
    }
}


/* TPPaneFocusProc is called when every the focus changes to or
from our control.  Herein, switch the focus appropriately
according to the parameters and redraw the control as
necessary.  */
static pascal ControlPartCode TPPaneFocusProc(ControlRef theControl, ControlFocusPart action) {
    ControlPartCode focusResult;

    focusResult = kControlFocusNoPart;
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return 0;
    TPUpdateVisibility( theControl ) ;
    STPTextPaneVars *varsp = (STPTextPaneVars *) ((wxMacMLTEClassicControl*)textctrl->GetPeer())->m_macTXNvars ;
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
    /* calculate the next highlight state */
    switch (action) {
        default:
        case kControlFocusNoPart:
            TPFocusPaneText(varsp, false);
            focusResult = kControlFocusNoPart;
            break;
        case kUserClickedToFocusPart:
            TPFocusPaneText(varsp, true);
            focusResult = 1;
            break;
        case kControlFocusPrevPart:
        case kControlFocusNextPart:
            TPFocusPaneText(varsp, ( ! varsp->fInFocus));
            focusResult = varsp->fInFocus ? 1 : kControlFocusNoPart;
            break;
    }
    TPActivatePaneText(varsp, varsp->fIsActive && varsp->fInFocus);
    /* redraw the text fram and focus rectangle to indicate the
        new focus state */
    if ( textctrl->MacIsReallyShown() )
    {
        wxMacWindowClipper c( textctrl ) ;
        if ( !varsp->fNoBorders )
            DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
        TPRedrawFocusOutline( varsp ) ;
    }
    return focusResult;
}

wxMacMLTEClassicControl::wxMacMLTEClassicControl( wxWindow *wxPeer,
                         const wxString& str,
                         const wxPoint& pos,
                         const wxSize& size, long style )
{
    m_font = wxPeer->GetFont() ;
    m_windowStyle = style ;
    Rect bounds = wxMacGetBoundsForControl( wxPeer , pos , size ) ;
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;

    short featurSet;

    featurSet = kControlSupportsEmbedding | kControlSupportsFocus  | kControlWantsIdle
            | kControlWantsActivate | kControlHandlesTracking | kControlHasSpecialBackground
            | kControlGetsFocusOnClick | kControlSupportsLiveFeedback;
        /* create the control */

    verify_noerr( ::CreateUserPaneControl( MAC_WXHWND(wxPeer->GetParent()->MacGetTopLevelWindowRef()), &bounds, featurSet, &m_controlRef ) );

    {
//        wxMacWindowClipper c(wxPeer) ;
        DoCreate();
    }

    if ( wxPeer->MacIsReallyShown() )
        MLTESetObjectVisibility( (STPTextPaneVars*) m_macTXNvars, true , style ) ;

    {
 //         wxMacWindowClipper clipper( wxPeer ) ;

        TPUpdateVisibility( m_controlRef ) ;

        SetTXNData( st , kTXNStartOffset, kTXNEndOffset ) ;

        TXNSetSelection( m_txn, 0, 0);
        TXNShowSelection( m_txn, kTXNShowStart);
    }

    AdjustCreationAttributes( *wxWHITE , true ) ;
}

wxMacMLTEClassicControl::~wxMacMLTEClassicControl()
{
//    SetControlReference(m_controlRef , 0) ;
    TXNDeleteObject(m_txn);
    free(m_macTXNvars);
}

void wxMacMLTEClassicControl::VisibilityChanged(bool shown)
{
    MLTESetObjectVisibility((STPTextPaneVars*) m_macTXNvars , shown , m_windowStyle ) ;
    if ( !shown )
        InvalWindowRect( GetControlOwner( m_controlRef ) , &((STPTextPaneVars *)m_macTXNvars)->fRBounds ) ;
}

bool wxMacMLTEClassicControl::NeedsFocusRect() const
{
    return true;
}

OSStatus wxMacMLTEClassicControl::DoCreate()
{
    Rect bounds;
    WindowRef theWindow;

    OSStatus err = noErr ;

    /* set up our globals */
    if (gTPDrawProc == NULL) gTPDrawProc = NewControlUserPaneDrawUPP(TPPaneDrawProc);
    if (gTPHitProc == NULL) gTPHitProc = NewControlUserPaneHitTestUPP(TPPaneHitTestProc);
    if (gTPTrackProc == NULL) gTPTrackProc = NewControlUserPaneTrackingUPP(TPPaneTrackingProc);
    if (gTPIdleProc == NULL) gTPIdleProc = NewControlUserPaneIdleUPP(TPPaneIdleProc);
    if (gTPKeyProc == NULL) gTPKeyProc = NewControlUserPaneKeyDownUPP(TPPaneKeyDownProc);
    if (gTPActivateProc == NULL) gTPActivateProc = NewControlUserPaneActivateUPP(TPPaneActivateProc);
    if (gTPFocusProc == NULL) gTPFocusProc = NewControlUserPaneFocusUPP(TPPaneFocusProc);

    /* allocate our private storage */
    m_macTXNvars = (STPTextPaneVars *) malloc(sizeof(STPTextPaneVars));

    /* set the initial settings for our private data */
    m_macTXNvars->fMultiline = m_windowStyle & wxTE_MULTILINE ;
    m_macTXNvars->fNoBorders = m_windowStyle & wxNO_BORDER ;
    m_macTXNvars->fInFocus = false;
    m_macTXNvars->fIsActive = true;
    m_macTXNvars->fTXNObjectActive = false;
    m_macTXNvars->fFocusDrawState = false ;
    m_macTXNvars->fUserPaneRec = m_controlRef ;
    m_macTXNvars->fVisible = true ;

    theWindow = m_macTXNvars->fOwner = GetControlOwner(m_controlRef);

    m_macTXNvars->fDrawingEnvironment = (GrafPtr)  GetWindowPort(theWindow);

    /* set up the user pane procedures */
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneDrawProcTag, sizeof(gTPDrawProc), &gTPDrawProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneHitTestProcTag, sizeof(gTPHitProc), &gTPHitProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneTrackingProcTag, sizeof(gTPTrackProc), &gTPTrackProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneIdleProcTag, sizeof(gTPIdleProc), &gTPIdleProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneKeyDownProcTag, sizeof(gTPKeyProc), &gTPKeyProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneActivateProcTag, sizeof(gTPActivateProc), &gTPActivateProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneFocusProcTag, sizeof(gTPFocusProc), &gTPFocusProc);

    /* calculate the rectangles used by the control */
    UMAGetControlBoundsInWindowCoords(m_controlRef, &bounds);
    m_macTXNvars->fRTextOutlineRegion = NewRgn() ;
    TPCalculateBounds( m_macTXNvars , bounds ) ;

    /* set up the drawing environment */
    SetPort(m_macTXNvars->fDrawingEnvironment);

    /* create the new edit field */

    TXNFrameOptions frameOptions = FrameOptionsFromWXStyle( m_windowStyle ) ;

    verify_noerr(TXNNewObject(NULL, m_macTXNvars->fOwner, &m_macTXNvars->fRTextArea,
                              frameOptions ,
                              kTXNTextEditStyleFrameType,
                              kTXNTextensionFile,
                              kTXNSystemDefaultEncoding,
                              &m_macTXNvars->fTXNRec, &m_macTXNvars->fTXNFrame, (TXNObjectRefcon) m_macTXNvars));
    m_txn = m_macTXNvars->fTXNRec ;

    /* perform final activations and setup for our text field.  Here,
        we assume that the window is going to be the 'active' window. */
    TPActivatePaneText(m_macTXNvars, m_macTXNvars->fIsActive && m_macTXNvars->fInFocus);
    /* all done */
    return err;
}

// ----------------------------------------------------------------------------
// MLTE control implementation (OSX part)
// ----------------------------------------------------------------------------

#if TARGET_API_MAC_OSX

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2

wxMacMLTEHIViewControl::wxMacMLTEHIViewControl( wxWindow *wxPeer,
                         const wxString& str,
                         const wxPoint& pos,
                         const wxSize& size, long style )
{
    m_font = wxPeer->GetFont() ;
    m_windowStyle = style ;
    Rect bounds = wxMacGetBoundsForControl( wxPeer , pos , size ) ;
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;

    HIRect hr = { bounds.left , bounds.top , bounds.right - bounds.left , bounds.bottom- bounds.top } ;

    m_scrollView = NULL ;
    TXNFrameOptions frameOptions = FrameOptionsFromWXStyle( style ) ;
    if ( frameOptions & (kTXNWantVScrollBarMask|kTXNWantHScrollBarMask) )
    {
        HIScrollViewCreate(( frameOptions & kTXNWantHScrollBarMask ? kHIScrollViewOptionsHorizScroll : 0) |
            ( frameOptions & kTXNWantVScrollBarMask ? kHIScrollViewOptionsVertScroll: 0 ) , &m_scrollView ) ;

        HIViewSetFrame( m_scrollView, &hr );
        HIViewSetVisible( m_scrollView, true );
    }

    m_textView = NULL ;
    HITextViewCreate( NULL , 0, frameOptions , &m_textView ) ;
    m_txn = HITextViewGetTXNObject( m_textView) ;
    HIViewSetVisible( m_textView , true ) ;
    if ( m_scrollView )
    {
        HIViewAddSubview( m_scrollView , m_textView ) ;
        m_controlRef = m_scrollView ;
        wxPeer->MacInstallEventHandler( (WXWidget) m_textView  ) ;
    }
    else
    {
        HIViewSetFrame( m_textView, &hr );
        m_controlRef = m_textView ;
    }

    SetTXNData( st , kTXNStartOffset, kTXNEndOffset ) ;

    TXNSetSelection( m_txn, 0, 0);
    TXNShowSelection( m_txn, kTXNShowStart);

    AdjustCreationAttributes( *wxWHITE , true ) ;
}

OSStatus wxMacMLTEHIViewControl::SetFocus( ControlFocusPart focusPart )
{
    return SetKeyboardFocus(  GetControlOwner( m_textView )  ,
        m_textView , focusPart ) ;
}

bool wxMacMLTEHIViewControl::HasFocus() const
{
    ControlRef control ;
    GetKeyboardFocus( GetUserFocusWindow() , &control ) ;
    return control == m_textView ;
}

bool wxMacMLTEHIViewControl::NeedsFocusRect() const
{
    return m_windowStyle & wxNO_BORDER ? false : true;
}

#endif // MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2


#endif

#endif // wxUSE_TEXTCTRL
