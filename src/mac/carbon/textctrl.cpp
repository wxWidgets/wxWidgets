/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      Stefan Csomor
// Modified by: Ryan Norton (MLTE GetLineLength and GetLineText)
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TEXTCTRL

#include "wx/textctrl.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/button.h"
    #include "wx/menu.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/toplevel.h"
#endif

#ifdef __DARWIN__
    #include <sys/types.h>
    #include <sys/stat.h>
#else
    #include <stat.h>
#endif

#if wxUSE_STD_IOSTREAM
    #if wxUSE_IOSTREAMH
        #include <fstream.h>
    #else
        #include <fstream>
    #endif
#endif

#include "wx/filefn.h"
#include "wx/sysopt.h"

#if defined(__BORLANDC__) && !defined(__WIN32__)
    #include <alloc.h>
#elif !defined(__MWERKS__) && !defined(__GNUWIN32) && !defined(__DARWIN__)
    #include <malloc.h>
#endif

#ifndef __DARWIN__
#include <Scrap.h>
#endif

#ifndef __DARWIN__
#include <MacTextEditor.h>
#include <ATSUnicode.h>
#include <TextCommon.h>
#include <TextEncodingConverter.h>
#endif

#include "wx/mac/uma.h"


// if this is set to 1 then under OSX 10.2 the 'classic' MLTE implementation will be used
// if set to 0 then the unicode textctrl will be used
#ifndef wxMAC_AWAYS_USE_MLTE
#define wxMAC_AWAYS_USE_MLTE 1
#endif

#ifndef __WXMAC_OSX__
enum
{
    kTXNVisibilityTag = 'visb' // set the visibility state of the object
};
#endif


class wxMacFunctor
{
public :
    wxMacFunctor() {}
    virtual ~wxMacFunctor() {}

    virtual void* operator()() = 0 ;

    static void* CallBackProc( void *param )
    {
        wxMacFunctor* f = (wxMacFunctor*) param ;
        void *result = (*f)() ;
        return result ;
    }
} ;

template<typename classtype, typename param1type>

class wxMacObjectFunctor1 : public wxMacFunctor
{
    typedef void (classtype::*function)( param1type p1 ) ;
    typedef void (classtype::*ref_function)( const param1type& p1 ) ;
public :
    wxMacObjectFunctor1( classtype *obj , function f , param1type p1 ) :
        wxMacFunctor()
    {
        m_object = obj ;
        m_function = f ;
        m_param1 = p1 ;
    }

    wxMacObjectFunctor1( classtype *obj , ref_function f , param1type p1 ) :
        wxMacFunctor()
    {
        m_object = obj ;
        m_refFunction = f ;
        m_param1 = p1 ;
    }

    virtual ~wxMacObjectFunctor1() {}

    virtual void* operator()()
    {
        (m_object->*m_function)( m_param1 ) ;
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
    wxMacObjectFunctor1<classtype, param1type> params(object, function, p1) ;
    void *result =
        MPRemoteCall( wxMacFunctor::CallBackProc , &params , kMPOwningProcessRemoteContext ) ;
    return result ;
}

template<typename classtype, typename param1type>
void* wxMacMPRemoteCall( classtype *object , void (classtype::*function)( const param1type& p1 ) , param1type p1 )
{
    wxMacObjectFunctor1<classtype,param1type> params(object, function, p1) ;
    void *result =
        MPRemoteCall( wxMacFunctor::CallBackProc , &params , kMPOwningProcessRemoteContext ) ;
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
    wxMacTextControl( wxTextCtrl *peer ) ;
    virtual ~wxMacTextControl() ;

    virtual wxString GetStringValue() const = 0 ;
    virtual void SetStringValue( const wxString &val ) = 0 ;
    virtual void SetSelection( long from, long to ) = 0 ;
    virtual void GetSelection( long* from, long* to ) const = 0 ;
    virtual void WriteText( const wxString& str ) = 0 ;

    virtual void SetStyle( long start, long end, const wxTextAttr& style ) ;
    virtual void Copy() ;
    virtual void Cut() ;
    virtual void Paste() ;
    virtual bool CanPaste() const ;
    virtual void SetEditable( bool editable ) ;
    virtual wxTextPos GetLastPosition() const ;
    virtual void Replace( long from, long to, const wxString &str ) ;
    virtual void Remove( long from, long to ) ;


    virtual bool HasOwnContextMenu() const
    { return false ; }

    virtual bool SetupCursor( const wxPoint& pt )
    { return false ; }

    virtual void Clear() ;
    virtual bool CanUndo() const;
    virtual void Undo() ;
    virtual bool CanRedo() const;
    virtual void Redo() ;
    virtual int GetNumberOfLines() const ;
    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const ;
    virtual void ShowPosition(long WXUNUSED(pos)) ;
    virtual int GetLineLength(long lineNo) const ;
    virtual wxString GetLineText(long lineNo) const ;

#ifndef __WXMAC_OSX__
    virtual void            MacControlUserPaneDrawProc(wxInt16 part) = 0 ;
    virtual wxInt16         MacControlUserPaneHitTestProc(wxInt16 x, wxInt16 y) = 0 ;
    virtual wxInt16         MacControlUserPaneTrackingProc(wxInt16 x, wxInt16 y, void* actionProc) = 0 ;
    virtual void            MacControlUserPaneIdleProc() = 0 ;
    virtual wxInt16         MacControlUserPaneKeyDownProc(wxInt16 keyCode, wxInt16 charCode, wxInt16 modifiers) = 0 ;
    virtual void            MacControlUserPaneActivateProc(bool activating) = 0 ;
    virtual wxInt16         MacControlUserPaneFocusProc(wxInt16 action) = 0 ;
    virtual void            MacControlUserPaneBackgroundProc(void* info) = 0 ;
#endif
} ;

// common parts for implementations based on MLTE

class wxMacMLTEControl : public wxMacTextControl
{
public :
    wxMacMLTEControl( wxTextCtrl *peer ) ;

    virtual wxString GetStringValue() const ;
    virtual void SetStringValue( const wxString &str ) ;

    static TXNFrameOptions FrameOptionsFromWXStyle( long wxStyle ) ;

    void AdjustCreationAttributes( const wxColour& background, bool visible ) ;

    virtual void SetFont( const wxFont & font, const wxColour& foreground, long windowStyle ) ;
    virtual void SetBackground( const wxBrush &brush ) ;
    virtual void SetStyle( long start, long end, const wxTextAttr& style ) ;
    virtual void Copy() ;
    virtual void Cut() ;
    virtual void Paste() ;
    virtual bool CanPaste() const ;
    virtual void SetEditable( bool editable ) ;
    virtual wxTextPos GetLastPosition() const ;
    virtual void Replace( long from, long to, const wxString &str ) ;
    virtual void Remove( long from, long to ) ;
    virtual void GetSelection( long* from, long* to ) const ;
    virtual void SetSelection( long from, long to ) ;

    virtual void WriteText( const wxString& str ) ;

    virtual bool HasOwnContextMenu() const
    {
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
        if ( UMAGetSystemVersion() >= 0x1040 )
        {
            TXNCommandEventSupportOptions options ;
            TXNGetCommandEventSupport( m_txn , & options ) ;
            return options & kTXNSupportEditCommandProcessing ;
        }
#endif

        return false ;
    }

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
    TXNObject GetTXNObject() { return m_txn ; }

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
    wxMacMLTEHIViewControl( wxTextCtrl *wxPeer,
                             const wxString& str,
                             const wxPoint& pos,
                             const wxSize& size, long style ) ;
    virtual ~wxMacMLTEHIViewControl() ;

    virtual OSStatus SetFocus( ControlFocusPart focusPart ) ;
    virtual bool HasFocus() const ;
    virtual void SetBackground( const wxBrush &brush) ;

protected :
    HIViewRef m_scrollView ;
    HIViewRef m_textView ;
    EventHandlerRef m_textEventHandlerRef ;
};

#endif

class wxMacUnicodeTextControl : public wxMacTextControl
{
public :
    wxMacUnicodeTextControl( wxTextCtrl *wxPeer,
                             const wxString& str,
                             const wxPoint& pos,
                             const wxSize& size, long style ) ;
    virtual ~wxMacUnicodeTextControl();

    virtual void VisibilityChanged(bool shown);
    virtual wxString GetStringValue() const ;
    virtual void SetStringValue( const wxString &str) ;
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual bool CanPaste() const;
    virtual void SetEditable(bool editable) ;
    virtual void GetSelection( long* from, long* to) const ;
    virtual void SetSelection( long from , long to ) ;
    virtual void WriteText(const wxString& str) ;

protected :
    // contains the tag for the content (is different for password and non-password controls)
    OSType m_valueTag ;

    // as the selection tag only works correctly when the control has the focus we have to mirror the
    // intended value
    EventHandlerRef m_focusHandlerRef ;
public :
    ControlEditTextSelectionRec m_selection ;
};

#endif

// 'classic' MLTE implementation

class wxMacMLTEClassicControl : public wxMacMLTEControl
{
public :
    wxMacMLTEClassicControl( wxTextCtrl *wxPeer,
                             const wxString& str,
                             const wxPoint& pos,
                             const wxSize& size, long style ) ;
    virtual ~wxMacMLTEClassicControl() ;

    virtual void VisibilityChanged(bool shown) ;
    virtual void SuperChangedPosition() ;

    virtual void            MacControlUserPaneDrawProc(wxInt16 part) ;
    virtual wxInt16         MacControlUserPaneHitTestProc(wxInt16 x, wxInt16 y) ;
    virtual wxInt16         MacControlUserPaneTrackingProc(wxInt16 x, wxInt16 y, void* actionProc) ;
    virtual void            MacControlUserPaneIdleProc() ;
    virtual wxInt16         MacControlUserPaneKeyDownProc(wxInt16 keyCode, wxInt16 charCode, wxInt16 modifiers) ;
    virtual void            MacControlUserPaneActivateProc(bool activating) ;
    virtual wxInt16         MacControlUserPaneFocusProc(wxInt16 action) ;
    virtual void            MacControlUserPaneBackgroundProc(void* info) ;

    virtual bool SetupCursor( const wxPoint& WXUNUSED(pt) )
    {
        MacControlUserPaneIdleProc();
        return true;
    }

    virtual void            SetRect( Rect *r ) ;

protected :
    OSStatus                 DoCreate();

    void                    MacUpdatePosition() ;
    void                    MacActivatePaneText(bool setActive) ;
    void                    MacFocusPaneText(bool setFocus) ;
    void                    MacSetObjectVisibility(bool vis) ;

private :
    TXNFrameID              m_txnFrameID ;
    GrafPtr                 m_txnPort ;
    WindowRef               m_txnWindow ;
    // bounds of the control as we last did set the txn frames
    Rect                    m_txnControlBounds ;
    Rect                    m_txnVisBounds ;

#ifdef __WXMAC_OSX__
    static pascal void TXNScrollActionProc( ControlRef controlRef , ControlPartCode partCode ) ;
    static pascal void TXNScrollInfoProc(
        SInt32 iValue, SInt32 iMaximumValue,
        TXNScrollBarOrientation iScrollBarOrientation, SInt32 iRefCon ) ;

    ControlRef              m_sbHorizontal ;
    SInt32                  m_lastHorizontalValue ;
    ControlRef              m_sbVertical ;
    SInt32                  m_lastVerticalValue ;
#endif
};


IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxTextCtrlBase)

BEGIN_EVENT_TABLE(wxTextCtrl, wxTextCtrlBase)
    EVT_ERASE_BACKGROUND( wxTextCtrl::OnEraseBackground )
    EVT_DROP_FILES(wxTextCtrl::OnDropFiles)
    EVT_CHAR(wxTextCtrl::OnChar)
    EVT_MENU(wxID_CUT, wxTextCtrl::OnCut)
    EVT_MENU(wxID_COPY, wxTextCtrl::OnCopy)
    EVT_MENU(wxID_PASTE, wxTextCtrl::OnPaste)
    EVT_MENU(wxID_UNDO, wxTextCtrl::OnUndo)
    EVT_MENU(wxID_REDO, wxTextCtrl::OnRedo)
    EVT_MENU(wxID_CLEAR, wxTextCtrl::OnDelete)
    EVT_MENU(wxID_SELECTALL, wxTextCtrl::OnSelectAll)

    EVT_CONTEXT_MENU(wxTextCtrl::OnContextMenu)

    EVT_UPDATE_UI(wxID_CUT, wxTextCtrl::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxTextCtrl::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxTextCtrl::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxTextCtrl::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxTextCtrl::OnUpdateRedo)
    EVT_UPDATE_UI(wxID_CLEAR, wxTextCtrl::OnUpdateDelete)
    EVT_UPDATE_UI(wxID_SELECTALL, wxTextCtrl::OnUpdateSelectAll)
END_EVENT_TABLE()


void wxTextCtrl::Init()
{
    m_editable = true ;
    m_dirty = false;

    m_maxLength = 0;
    m_privateContextMenu = NULL;
    m_triggerOnSetValue = true ;
}

wxTextCtrl::~wxTextCtrl()
{
    delete m_privateContextMenu;
}

bool wxTextCtrl::Create( wxWindow *parent,
    wxWindowID id,
    const wxString& str,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name )
{
    m_macIsUserPane = false ;
    m_editable = true ;

    if ( ! (style & wxNO_BORDER) )
        style = (style & ~wxBORDER_MASK) | wxSUNKEN_BORDER ;

    if ( !wxTextCtrlBase::Create( parent, id, pos, size, style & ~(wxHSCROLL | wxVSCROLL), validator, name ) )
        return false;

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        // always turn on this style for multi-line controls
        m_windowStyle |= wxTE_PROCESS_ENTER;
        style |= wxTE_PROCESS_ENTER ;
    }

    bool forceMLTE = false ;

#if wxUSE_SYSTEM_OPTIONS
    if (wxSystemOptions::HasOption( wxMAC_TEXTCONTROL_USE_MLTE ) && (wxSystemOptions::GetOptionInt( wxMAC_TEXTCONTROL_USE_MLTE ) == 1))
    {
        forceMLTE = true ;
    }
#endif

#ifdef __WXMAC_OSX__
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
    if ( UMAGetSystemVersion() >= 0x1030 && !forceMLTE )
    {
        if ( m_windowStyle & wxTE_MULTILINE )
            m_peer = new wxMacMLTEHIViewControl( this , str , pos , size , style ) ;
    }
#endif

    if ( !m_peer )
    {
        if ( !(m_windowStyle & wxTE_MULTILINE) && !forceMLTE )
            m_peer = new wxMacUnicodeTextControl( this , str , pos , size , style ) ;
    }
#endif

    if ( !m_peer )
        m_peer = new wxMacMLTEClassicControl( this , str , pos , size , style ) ;

    MacPostControlCreate(pos, size) ;

    // only now the embedding is correct and we can do a positioning update

    MacSuperChangedPosition() ;

    if ( m_windowStyle & wxTE_READONLY)
        SetEditable( false ) ;

    SetCursor( wxCursor( wxCURSOR_IBEAM ) ) ;

    return true;
}

void wxTextCtrl::MacSuperChangedPosition()
{
    wxWindow::MacSuperChangedPosition() ;
    GetPeer()->SuperChangedPosition() ;
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

void wxTextCtrl::DoSetValue(const wxString& str, int flags)
{
    // optimize redraws
    if ( GetValue() == str )
        return;

    GetPeer()->SetStringValue( str ) ;

    if ( (flags & SetValue_SendEvent) && m_triggerOnSetValue )
    {
        SendTextUpdatedEvent();
    }
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
        GetPeer()->Copy() ;
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
        GetPeer()->Cut() ;

        wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, m_windowId );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event );
      }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        GetPeer()->Paste() ;

        // TODO: eventually we should add setting the default style again

        wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, m_windowId );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event );
    }
}

bool wxTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    long from, to;
    GetSelection( &from, &to );

    return (from != to);
}

bool wxTextCtrl::CanCut() const
{
    if ( !IsEditable() )
        return false;

    // Can cut if there's a selection
    long from, to;
    GetSelection( &from, &to );

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
    SetInsertionPoint( pos );
}

long wxTextCtrl::GetInsertionPoint() const
{
    long begin, end ;
    GetSelection( &begin , &end ) ;

    return begin ;
}

wxTextPos wxTextCtrl::GetLastPosition() const
{
    return GetPeer()->GetLastPosition() ;
}

void wxTextCtrl::Replace(long from, long to, const wxString& str)
{
    GetPeer()->Replace( from , to , str ) ;
}

void wxTextCtrl::Remove(long from, long to)
{
    GetPeer()->Remove( from , to ) ;
}

void wxTextCtrl::SetSelection(long from, long to)
{
    GetPeer()->SetSelection( from , to ) ;
}

void wxTextCtrl::WriteText(const wxString& str)
{
    // TODO: this MPRemoting will be moved into a remoting peer proxy for any command
    if ( !wxIsMainThread() )
    {
        // unfortunately CW 8 is not able to correctly deduce the template types,
        // so we have to instantiate explicitly
        wxMacMPRemoteGUICall<wxTextCtrl,wxString>( this , &wxTextCtrl::WriteText , str ) ;

        return ;
    }

    GetPeer()->WriteText( str ) ;
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText( text );
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
    int wText, hText;

    // these are the numbers from the HIG:
    // we reduce them by the borders first
    wText = 100 ;

    switch ( m_windowVariant )
    {
        case wxWINDOW_VARIANT_NORMAL :
            hText = 22 - 6 ;
            break ;

        case wxWINDOW_VARIANT_SMALL :
            hText = 19 - 6 ;
            break ;

        case wxWINDOW_VARIANT_MINI :
            hText = 15 - 6 ;
            break ;

        default :
            hText = 22 - 6;
            break ;
    }

    // as the above numbers have some free space around the text
    // we get 5 lines like this anyway
    if ( m_windowStyle & wxTE_MULTILINE )
         hText *= 5 ;

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
        GetPeer()->Undo() ;
}

void wxTextCtrl::Redo()
{
    if (CanRedo())
        GetPeer()->Redo() ;
}

bool wxTextCtrl::CanUndo() const
{
    if ( !IsEditable() )
        return false ;

    return GetPeer()->CanUndo() ;
}

bool wxTextCtrl::CanRedo() const
{
    if ( !IsEditable() )
        return false ;

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
    return GetPeer()->PositionToXY( pos , x , y ) ;
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

void wxTextCtrl::Command(wxCommandEvent & event)
{
    SetValue(event.GetString());
    ProcessCommand(event);
}

void wxTextCtrl::OnDropFiles(wxDropFilesEvent& event)
{
    // By default, load the first file into the text window.
    if (event.GetNumberOfFiles() > 0)
        LoadFile( event.GetFiles()[0] );
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

    if ( key == 'a' && event.MetaDown() )
    {
        SelectAll() ;

        return ;
    }

    if ( key == 'c' && event.MetaDown() )
    {
        if ( CanCopy() )
            Copy() ;

        return ;
    }

    if ( !IsEditable() && key != WXK_LEFT && key != WXK_RIGHT && key != WXK_DOWN && key != WXK_UP && key != WXK_TAB &&
        !( key == WXK_RETURN && ( (m_windowStyle & wxTE_PROCESS_ENTER) || (m_windowStyle & wxTE_MULTILINE) ) )
//        && key != WXK_PAGEUP && key != WXK_PAGEDOWN && key != WXK_HOME && key != WXK_END
        )
    {
        // eat it
        return ;
    }

    // Check if we have reached the max # of chars (if it is set), but still
    // allow navigation and deletion
    if ( !IsMultiLine() && m_maxLength && GetValue().length() >= m_maxLength &&
        key != WXK_LEFT && key != WXK_RIGHT && key != WXK_TAB &&
        key != WXK_BACK && !( key == WXK_RETURN && (m_windowStyle & wxTE_PROCESS_ENTER) )
       )
    {
        // eat it, we don't want to add more than allowed # of characters

        // TODO: generate EVT_TEXT_MAXLEN()
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
            if (m_windowStyle & wxTE_PROCESS_ENTER)
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                event.SetString( GetValue() );
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
            }

            if ( !(m_windowStyle & wxTE_MULTILINE) )
            {
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

                // this will make wxWidgets eat the ENTER key so that
                // we actually prevent line wrapping in a single line text control
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
                // This is necessary (don't know why);
                // otherwise the tab will not be inserted.
                WriteText(wxT("\t"));
                eat_key = true;
            }
            break;

        default:
            break;
    }

    if (!eat_key)
    {
        // perform keystroke handling
        event.Skip(true) ;
    }

    if ( ( key >= 0x20 && key < WXK_START ) ||
         ( key >= WXK_NUMPAD0 && key <= WXK_DIVIDE ) ||
         key == WXK_RETURN ||
         key == WXK_DELETE ||
         key == WXK_BACK)
    {
        wxCommandEvent event1(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event1.SetEventObject( this );
        wxPostEvent( GetEventHandler(), event1 );
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

void wxTextCtrl::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    long from, to;

    GetSelection( &from, &to );
    if (from != -1 && to != -1)
        Remove( from, to );
}

void wxTextCtrl::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
    SetSelection(-1, -1);
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

void wxTextCtrl::OnUpdateDelete(wxUpdateUIEvent& event)
{
    long from, to;

    GetSelection( &from, &to );
    event.Enable( from != -1 && to != -1 && from != to && IsEditable() ) ;
}

void wxTextCtrl::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
    event.Enable(GetLastPosition() > 0);
}

// CS: Context Menus only work with MLTE implementations or non-multiline HIViews at the moment

void wxTextCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    if ( GetPeer()->HasOwnContextMenu() )
    {
        event.Skip() ;
        return ;
    }

    if (m_privateContextMenu == NULL)
    {
        m_privateContextMenu = new wxMenu;
        m_privateContextMenu->Append(wxID_UNDO, _("&Undo"));
        m_privateContextMenu->Append(wxID_REDO, _("&Redo"));
        m_privateContextMenu->AppendSeparator();
        m_privateContextMenu->Append(wxID_CUT, _("Cu&t"));
        m_privateContextMenu->Append(wxID_COPY, _("&Copy"));
        m_privateContextMenu->Append(wxID_PASTE, _("&Paste"));
        m_privateContextMenu->Append(wxID_CLEAR, _("&Delete"));
        m_privateContextMenu->AppendSeparator();
        m_privateContextMenu->Append(wxID_SELECTALL, _("Select &All"));
    }

    if (m_privateContextMenu != NULL)
        PopupMenu(m_privateContextMenu);
}

bool wxTextCtrl::MacSetupCursor( const wxPoint& pt )
{
    if ( !GetPeer()->SetupCursor( pt ) )
        return wxWindow::MacSetupCursor( pt ) ;
    else
        return true ;
}

#if !TARGET_API_MAC_OSX

// user pane implementation

void wxTextCtrl::MacControlUserPaneDrawProc(wxInt16 part)
{
    GetPeer()->MacControlUserPaneDrawProc( part ) ;
}

wxInt16 wxTextCtrl::MacControlUserPaneHitTestProc(wxInt16 x, wxInt16 y)
{
    return GetPeer()->MacControlUserPaneHitTestProc( x , y ) ;
}

wxInt16 wxTextCtrl::MacControlUserPaneTrackingProc(wxInt16 x, wxInt16 y, void* actionProc)
{
    return GetPeer()->MacControlUserPaneTrackingProc( x , y , actionProc ) ;
}

void wxTextCtrl::MacControlUserPaneIdleProc()
{
    GetPeer()->MacControlUserPaneIdleProc( ) ;
}

wxInt16 wxTextCtrl::MacControlUserPaneKeyDownProc(wxInt16 keyCode, wxInt16 charCode, wxInt16 modifiers)
{
    return GetPeer()->MacControlUserPaneKeyDownProc( keyCode , charCode , modifiers ) ;
}

void wxTextCtrl::MacControlUserPaneActivateProc(bool activating)
{
    GetPeer()->MacControlUserPaneActivateProc( activating ) ;
}

wxInt16 wxTextCtrl::MacControlUserPaneFocusProc(wxInt16 action)
{
    return GetPeer()->MacControlUserPaneFocusProc( action ) ;
}

void wxTextCtrl::MacControlUserPaneBackgroundProc(void* info)
{
    GetPeer()->MacControlUserPaneBackgroundProc( info ) ;
}

#endif

// ----------------------------------------------------------------------------
// implementation base class
// ----------------------------------------------------------------------------

wxMacTextControl::wxMacTextControl(wxTextCtrl* peer) :
    wxMacControl( peer )
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
    return GetStringValue().length() ;
}

void wxMacTextControl::Replace( long from , long to , const wxString &val )
{
    SetSelection( from , to ) ;
    WriteText( val ) ;
}

void wxMacTextControl::Remove( long from , long to )
{
    SetSelection( from , to ) ;
    WriteText( wxEmptyString) ;
}

void wxMacTextControl::Clear()
{
    SetStringValue( wxEmptyString ) ;
}

bool wxMacTextControl::CanUndo() const
{
    return false ;
}

void wxMacTextControl::Undo()
{
}

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

    for (size_t i = 0; i < content.length() ; i++)
    {
        if (content[i] == '\r')
            lines++;
    }

    return lines ;
}

wxString wxMacTextControl::GetLineText(long lineNo) const
{
    // TODO: change this if possible to reflect real lines
    wxString content = GetStringValue() ;

    // Find line first
    int count = 0;
    for (size_t i = 0; i < content.length() ; i++)
    {
        if (count == lineNo)
        {
            // Add chars in line then
            wxString tmp;

            for (size_t j = i; j < content.length(); j++)
            {
                if (content[j] == '\n')
                    return tmp;

                tmp += content[j];
            }

            return tmp;
        }

        if (content[i] == '\n')
            count++;
    }

    return wxEmptyString ;
}

int wxMacTextControl::GetLineLength(long lineNo) const
{
    // TODO: change this if possible to reflect real lines
    wxString content = GetStringValue() ;

    // Find line first
    int count = 0;
    for (size_t i = 0; i < content.length() ; i++)
    {
        if (count == lineNo)
        {
            // Count chars in line then
            count = 0;
            for (size_t j = i; j < content.length(); j++)
            {
                count++;
                if (content[j] == '\n')
                    return count;
            }

            return count;
        }

        if (content[i] == '\n')
            count++;
    }

    return 0 ;
}

// ----------------------------------------------------------------------------
// standard unicode control implementation
// ----------------------------------------------------------------------------

#if TARGET_API_MAC_OSX

// the current unicode textcontrol implementation has a bug : only if the control
// is currently having the focus, the selection can be retrieved by the corresponding
// data tag. So we have a mirroring using a member variable
// TODO : build event table using virtual member functions for wxMacControl

static const EventTypeSpec unicodeTextControlEventList[] =
{
    { kEventClassControl , kEventControlSetFocusPart } ,
} ;

static pascal OSStatus wxMacUnicodeTextControlControlEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    wxMacUnicodeTextControl* focus = (wxMacUnicodeTextControl*) data ;
    wxMacCarbonEvent cEvent( event ) ;

    switch ( GetEventKind( event ) )
    {
        case kEventControlSetFocusPart :
        {
            ControlPartCode controlPart = cEvent.GetParameter<ControlPartCode>(kEventParamControlPart , typeControlPartCode );
            if ( controlPart == kControlFocusNoPart )
            {
                // about to loose focus -> store selection to field
                focus->GetData<ControlEditTextSelectionRec>( 0, kControlEditTextSelectionTag, &focus->m_selection );
            }
            result = CallNextEventHandler(handler,event) ;
            if ( controlPart != kControlFocusNoPart )
            {
                // about to gain focus -> set selection from field
                focus->SetData<ControlEditTextSelectionRec>( 0, kControlEditTextSelectionTag, &focus->m_selection );
            }
            break;
        }
        default:
            break ;
    }

    return result ;
}

static pascal OSStatus wxMacUnicodeTextControlEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    switch ( GetEventClass( event ) )
    {
        case kEventClassControl :
            result = wxMacUnicodeTextControlControlEventHandler( handler , event , data ) ;
            break ;

        default :
            break ;
    }
    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacUnicodeTextControlEventHandler )

wxMacUnicodeTextControl::wxMacUnicodeTextControl( wxTextCtrl *wxPeer,
    const wxString& str,
    const wxPoint& pos,
    const wxSize& size, long style )
    : wxMacTextControl( wxPeer )
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

    OSStatus err = CreateEditUnicodeTextControl(
        MAC_WXHWND(wxPeer->MacGetTopLevelWindowRef()), &bounds , cfr ,
        isPassword , NULL , &m_controlRef ) ;
    verify_noerr( err );

    if ( !(m_windowStyle & wxTE_MULTILINE) )
        SetData<Boolean>( kControlEditTextPart , kControlEditTextSingleLineTag , true ) ;

    InstallControlEventHandler( m_controlRef , GetwxMacUnicodeTextControlEventHandlerUPP(),
                                GetEventTypeCount(unicodeTextControlEventList), unicodeTextControlEventList, this,
                                &m_focusHandlerRef);
}

wxMacUnicodeTextControl::~wxMacUnicodeTextControl()
{
    ::RemoveEventHandler( m_focusHandlerRef );
}

void wxMacUnicodeTextControl::VisibilityChanged(bool shown)
{
    if ( !(m_windowStyle & wxTE_MULTILINE) && shown )
    {
        // work around a refresh issue insofar as not always the entire content is shown,
        // even if this would be possible
        ControlEditTextSelectionRec sel ;
        CFStringRef value = NULL ;

        verify_noerr( GetData<ControlEditTextSelectionRec>( 0, kControlEditTextSelectionTag, &sel ) );
        verify_noerr( GetData<CFStringRef>( 0, m_valueTag, &value ) );
        verify_noerr( SetData<CFStringRef>( 0, m_valueTag, &value ) );
        verify_noerr( SetData<ControlEditTextSelectionRec>( 0, kControlEditTextSelectionTag, &sel ) );

        CFRelease( value ) ;
    }
}

wxString wxMacUnicodeTextControl::GetStringValue() const
{
    wxString result ;
    CFStringRef value = GetData<CFStringRef>(0, m_valueTag) ;
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

void wxMacUnicodeTextControl::SetStringValue( const wxString &str )
{
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;
    wxMacCFStringHolder cf( st , m_font.GetEncoding() ) ;
    verify_noerr( SetData<CFStringRef>( 0, m_valueTag , cf ) ) ;
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
#if 0 // leads to problem because text cannot be selected anymore
    SetData<Boolean>( kControlEditTextPart , kControlEditTextLockedTag , (Boolean) !editable ) ;
#endif
}

void wxMacUnicodeTextControl::GetSelection( long* from, long* to ) const
{
    ControlEditTextSelectionRec sel ;
    if (HasFocus())
        verify_noerr( GetData<ControlEditTextSelectionRec>( 0, kControlEditTextSelectionTag, &sel ) ) ;
    else
        sel = m_selection ;

    if ( from )
        *from = sel.selStart ;
    if ( to )
        *to = sel.selEnd ;
}

void wxMacUnicodeTextControl::SetSelection( long from , long to )
{
    ControlEditTextSelectionRec sel ;
    wxString result ;
    int textLength = 0 ;
    CFStringRef value = GetData<CFStringRef>(0, m_valueTag) ;
    if ( value )
    {
        wxMacCFStringHolder cf(value) ;
        textLength = cf.AsString().length() ;
    }

    if ((from == -1) && (to == -1))
    {
        from = 0 ;
        to = textLength ;
    }
    else
    {
        from = wxMin(textLength,wxMax(from,0)) ;
        to = wxMax(0,wxMin(textLength,to)) ;
    }

    sel.selStart = from ;
    sel.selEnd = to ;
    if ( HasFocus() )
        SetData<ControlEditTextSelectionRec>( 0, kControlEditTextSelectionTag, &sel ) ;
    else
        m_selection = sel;
}

void wxMacUnicodeTextControl::WriteText( const wxString& str )
{
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
    if ( HasFocus() )
    {
        wxMacCFStringHolder cf(st , m_font.GetEncoding() ) ;
        CFStringRef value = cf ;
        SetData<CFStringRef>( 0, kControlEditTextInsertCFStringRefTag, &value );
    }
    else
#endif
    {
        wxString val = GetStringValue() ;
        long start , end ;
        GetSelection( &start , &end ) ;
        val.Remove( start , end - start ) ;
        val.insert( start , str ) ;
        SetStringValue( val ) ;
        SetSelection( start + str.length() , start + str.length() ) ;
    }
}

#endif

// ----------------------------------------------------------------------------
// MLTE control implementation (common part)
// ----------------------------------------------------------------------------

// if MTLE is read only, no changes at all are allowed, not even from
// procedural API, in order to allow changes via API all the same we must undo
// the readonly status while we are executing, this class helps to do so

class wxMacEditHelper
{
public :
    wxMacEditHelper( TXNObject txn )
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

    ~wxMacEditHelper()
    {
        TXNControlTag tag[] = { kTXNIOPrivilegesTag } ;
        if ( m_data[0].uValue == kTXNReadOnly )
            TXNSetTXNObjectControls( m_txn , false , 1 , tag , m_data ) ;
    }

protected :
    TXNObject m_txn ;
    TXNControlData m_data[1] ;
} ;

wxMacMLTEControl::wxMacMLTEControl( wxTextCtrl *peer )
    : wxMacTextControl( peer )
{
    SetNeedsFocusRect( true ) ;
}

wxString wxMacMLTEControl::GetStringValue() const
{
    wxString result ;
    OSStatus err ;
    Size actualSize = 0;

    {
#if wxUSE_UNICODE
        Handle theText ;
        err = TXNGetDataEncoded( m_txn, kTXNStartOffset, kTXNEndOffset, &theText, kTXNUnicodeTextData );

        // all done
        if ( err != noErr )
        {
            actualSize = 0 ;
        }
        else
        {
            actualSize = GetHandleSize( theText ) / sizeof(UniChar) ;
            if ( actualSize > 0 )
            {
                wxChar *ptr = NULL ;

#if SIZEOF_WCHAR_T == 2
                ptr = new wxChar[actualSize + 1] ;
                wxStrncpy( ptr , (wxChar*)(*theText) , actualSize ) ;
#else
                SetHandleSize( theText, (actualSize + 1) * sizeof(UniChar) ) ;
                HLock( theText ) ;
                (((UniChar*)*theText)[actualSize]) = 0 ;
                wxMBConvUTF16 converter ;
                size_t noChars = converter.MB2WC( NULL , (const char*)*theText , 0 ) ;
                wxASSERT_MSG( noChars != wxCONV_FAILED, _T("Unable to count the number of characters in this string!") );
                ptr = new wxChar[noChars + 1] ;

                noChars = converter.MB2WC( ptr , (const char*)*theText , noChars + 1 ) ;
                wxASSERT_MSG( noChars != wxCONV_FAILED, _T("Conversion of string failed!") );
                ptr[noChars] = 0 ;
                HUnlock( theText ) ;
#endif

                ptr[actualSize] = 0 ;
                result = wxString( ptr ) ;
                delete [] ptr ;
            }

            DisposeHandle( theText ) ;
        }
#else
        Handle theText ;
        err = TXNGetDataEncoded( m_txn , kTXNStartOffset, kTXNEndOffset, &theText, kTXNTextData );

        // all done
        if ( err != noErr )
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

void wxMacMLTEControl::SetStringValue( const wxString &str )
{
    wxString st = str;
    wxMacConvertNewlines10To13( &st );

    {
        wxMacWindowClipper c( m_peer );

        {
            wxMacEditHelper help( m_txn );
            SetTXNData( st, kTXNStartOffset, kTXNEndOffset );
        }

        TXNSetSelection( m_txn, 0, 0 );
        TXNShowSelection( m_txn, kTXNShowStart );
    }
}

TXNFrameOptions wxMacMLTEControl::FrameOptionsFromWXStyle( long wxStyle )
{
    TXNFrameOptions frameOptions = kTXNDontDrawCaretWhenInactiveMask;

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
    frameOptions |= kTXNDoFontSubstitutionMask;
#endif

    if ( ! (wxStyle & wxTE_NOHIDESEL) )
        frameOptions |= kTXNDontDrawSelectionWhenInactiveMask ;

    if ( wxStyle & (wxHSCROLL | wxTE_DONTWRAP) )
        frameOptions |= kTXNWantHScrollBarMask ;

    if ( wxStyle & wxTE_MULTILINE )
    {
        frameOptions |= kTXNAlwaysWrapAtViewEdgeMask ;

        if ( !(wxStyle & wxTE_NO_VSCROLL) )
        {
            frameOptions |= kTXNWantVScrollBarMask ;

            // The following code causes drawing problems on 10.4. Perhaps it can be restored for
            // older versions of the OS, but I'm not sure it's appropriate to put a grow icon here
            // anyways, as AFAIK users can't actually use it to resize the text ctrl.
//            if ( frameOptions & kTXNWantHScrollBarMask )
//                frameOptions |= kTXNDrawGrowIconMask ;
        }
    }
    else
    {
        frameOptions |= kTXNSingleLineOnlyMask ;
    }

    return frameOptions ;
}

void wxMacMLTEControl::AdjustCreationAttributes( const wxColour &background, bool visible )
{
    TXNControlTag iControlTags[] =
        {
            kTXNDoFontSubstitution,
            kTXNWordWrapStateTag ,
        };
    TXNControlData iControlData[] =
        {
            { true },
            { kTXNNoAutoWrap },
        };

    int toptag = WXSIZEOF( iControlTags ) ;

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        iControlData[1].uValue =
            (m_windowStyle & wxTE_DONTWRAP)
            ? kTXNNoAutoWrap
            : kTXNAutoWrap;
    }

    OSStatus err = TXNSetTXNObjectControls( m_txn, false, toptag, iControlTags, iControlData ) ;
    verify_noerr( err );

    // setting the default font:
    // under 10.2 this causes a visible caret, therefore we avoid it

    if ( UMAGetSystemVersion() >= 0x1030 )
    {
        Str255 fontName ;
        SInt16 fontSize ;
        Style fontStyle ;

        GetThemeFont( kThemeSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;

        TXNTypeAttributes typeAttr[] =
        {
            { kTXNQDFontNameAttribute , kTXNQDFontNameAttributeSize , { (void*) fontName } } ,
            { kTXNQDFontSizeAttribute , kTXNFontSizeAttributeSize , { (void*) (fontSize << 16) } } ,
            { kTXNQDFontStyleAttribute , kTXNQDFontStyleAttributeSize , { (void*) normal } } ,
        } ;

        err = TXNSetTypeAttributes(
            m_txn, sizeof(typeAttr) / sizeof(TXNTypeAttributes),
            typeAttr, kTXNStartOffset, kTXNEndOffset );
        verify_noerr( err );
    }

    if ( m_windowStyle & wxTE_PASSWORD )
    {
        UniChar c = 0x00A5 ;
        err = TXNEchoMode( m_txn , c , 0 , true );
        verify_noerr( err );
    }

    TXNBackground tback;
    tback.bgType = kTXNBackgroundTypeRGB;
    tback.bg.color = MAC_WXCOLORREF( background.GetPixel() );
    TXNSetBackground( m_txn , &tback );

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
    if ( UMAGetSystemVersion() >= 0x1040 )
    {
        TXNCommandEventSupportOptions options ;
        if ( TXNGetCommandEventSupport( m_txn, &options ) == noErr )
        {
            options |=
                kTXNSupportEditCommandProcessing
                | kTXNSupportEditCommandUpdating
                | kTXNSupportSpellCheckCommandProcessing
                | kTXNSupportSpellCheckCommandUpdating
                | kTXNSupportFontCommandProcessing
                | kTXNSupportFontCommandUpdating;

            TXNSetCommandEventSupport( m_txn , options ) ;
        }
    }
#endif
}

void wxMacMLTEControl::SetBackground( const wxBrush &brush )
{
    // currently only solid background are supported
    TXNBackground tback;

    tback.bgType = kTXNBackgroundTypeRGB;
    tback.bg.color = MAC_WXCOLORREF( brush.GetColour().GetPixel() );
    TXNSetBackground( m_txn , &tback );
}

void wxMacMLTEControl::TXNSetAttribute( const wxTextAttr& style , long from , long to )
{
    TXNTypeAttributes typeAttr[4] ;
    RGBColor color ;
    int attrCount = 0 ;

    if ( style.HasFont() )
    {
        const wxFont &font = style.GetFont() ;

#if 0 // old version
        Str255 fontName = "\pMonaco" ;
        SInt16 fontSize = 12 ;
        Style fontStyle = normal ;
        wxMacStringToPascal( font.GetFaceName() , fontName ) ;
        fontSize = font.GetPointSize() ;
        if ( font.GetUnderlined() )
            fontStyle |= underline ;
        if ( font.GetWeight() == wxBOLD )
            fontStyle |= bold ;
        if ( font.GetStyle() == wxITALIC )
            fontStyle |= italic ;

        typeAttr[attrCount].tag = kTXNQDFontNameAttribute ;
        typeAttr[attrCount].size = kTXNQDFontNameAttributeSize ;
        typeAttr[attrCount].data.dataPtr = (void*)fontName ;
        attrCount++ ;

        typeAttr[attrCount].tag = kTXNQDFontSizeAttribute ;
        typeAttr[attrCount].size = kTXNFontSizeAttributeSize ;
        typeAttr[attrCount].data.dataValue = (fontSize << 16) ;
        attrCount++ ;

        typeAttr[attrCount].tag = kTXNQDFontStyleAttribute ;
        typeAttr[attrCount].size = kTXNQDFontStyleAttributeSize ;
        typeAttr[attrCount].data.dataValue = fontStyle ;
        attrCount++ ;
#else
        typeAttr[attrCount].tag = kTXNATSUIStyle ;
        typeAttr[attrCount].size = kTXNATSUIStyleSize ;
        typeAttr[attrCount].data.dataValue = (UInt32)font.MacGetATSUStyle() ;
        attrCount++ ;
#endif
    }

    if ( style.HasTextColour() )
    {
        color = MAC_WXCOLORREF(style.GetTextColour().GetPixel()) ;

        typeAttr[attrCount].tag = kTXNQDFontColorAttribute ;
        typeAttr[attrCount].size = kTXNQDFontColorAttributeSize ;
        typeAttr[attrCount].data.dataPtr = (void*) &color ;
        attrCount++ ;
    }

    if ( attrCount > 0 )
    {
        verify_noerr( TXNSetTypeAttributes( m_txn , attrCount , typeAttr, from , to ) );
        // unfortunately the relayout is not automatic
        TXNRecalcTextLayout( m_txn );
    }
}

void wxMacMLTEControl::SetFont( const wxFont & font , const wxColour& foreground , long windowStyle )
{
    wxMacEditHelper help( m_txn ) ;
    TXNSetAttribute( wxTextAttr( foreground, wxNullColour, font ), kTXNStartOffset, kTXNEndOffset ) ;
}

void wxMacMLTEControl::SetStyle( long start, long end, const wxTextAttr& style )
{
    wxMacEditHelper help( m_txn ) ;
    TXNSetAttribute( style, start, end ) ;
}

void wxMacMLTEControl::Copy()
{
    ClearCurrentScrap();
    TXNCopy( m_txn );
    TXNConvertToPublicScrap();
}

void wxMacMLTEControl::Cut()
{
    ClearCurrentScrap();
    TXNCut( m_txn );
    TXNConvertToPublicScrap();
}

void wxMacMLTEControl::Paste()
{
    TXNConvertFromPublicScrap();
    TXNPaste( m_txn );
}

bool wxMacMLTEControl::CanPaste() const
{
    return TXNIsScrapPastable() ;
}

void wxMacMLTEControl::SetEditable(bool editable)
{
    TXNControlTag tag[] = { kTXNIOPrivilegesTag } ;
    TXNControlData data[] = { { editable ? kTXNReadWrite : kTXNReadOnly } } ;
    TXNSetTXNObjectControls( m_txn, false, WXSIZEOF(tag), tag, data ) ;
}

wxTextPos wxMacMLTEControl::GetLastPosition() const
{
    wxTextPos actualsize = 0 ;

    Handle theText ;
    OSErr err = TXNGetDataEncoded( m_txn, kTXNStartOffset, kTXNEndOffset, &theText, kTXNTextData );

    // all done
    if ( err == noErr )
    {
        actualsize = GetHandleSize( theText ) ;
        DisposeHandle( theText ) ;
    }
    else
    {
        actualsize = 0 ;
    }

    return actualsize ;
}

void wxMacMLTEControl::Replace( long from , long to , const wxString &str )
{
    wxString value = str ;
    wxMacConvertNewlines10To13( &value ) ;

    wxMacEditHelper help( m_txn ) ;
    wxMacWindowClipper c( m_peer ) ;

    TXNSetSelection( m_txn, from, to ) ;
    TXNClear( m_txn ) ;
    SetTXNData( value, kTXNUseCurrentSelection, kTXNUseCurrentSelection ) ;
}

void wxMacMLTEControl::Remove( long from , long to )
{
    wxMacWindowClipper c( m_peer ) ;
    wxMacEditHelper help( m_txn ) ;
    TXNSetSelection( m_txn , from , to ) ;
    TXNClear( m_txn ) ;
}

void wxMacMLTEControl::GetSelection( long* from, long* to) const
{
    TXNGetSelection( m_txn , (TXNOffset*) from , (TXNOffset*) to ) ;
}

void wxMacMLTEControl::SetSelection( long from , long to )
{
    wxMacWindowClipper c( m_peer ) ;

    // change the selection
    if ((from == -1) && (to == -1))
        TXNSelectAll( m_txn );
    else
        TXNSetSelection( m_txn, from, to );

    TXNShowSelection( m_txn, kTXNShowStart );
}

void wxMacMLTEControl::WriteText( const wxString& str )
{
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;

    long start , end , dummy ;

    GetSelection( &start , &dummy ) ;
    wxMacWindowClipper c( m_peer ) ;

    {
        wxMacEditHelper helper( m_txn ) ;
        SetTXNData( st, kTXNUseCurrentSelection, kTXNUseCurrentSelection ) ;
    }

    GetSelection( &dummy, &end ) ;

    // TODO: SetStyle( start , end , GetDefaultStyle() ) ;
}

void wxMacMLTEControl::Clear()
{
    wxMacWindowClipper c( m_peer ) ;
    wxMacEditHelper st( m_txn ) ;
    TXNSetSelection( m_txn , kTXNStartOffset , kTXNEndOffset ) ;
    TXNClear( m_txn ) ;
}

bool wxMacMLTEControl::CanUndo() const
{
    return TXNCanUndo( m_txn , NULL ) ;
}

void wxMacMLTEControl::Undo()
{
    TXNUndo( m_txn ) ;
}

bool wxMacMLTEControl::CanRedo() const
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
    TXNGetLineCount( m_txn, &lines ) ;

    return lines ;
}

long wxMacMLTEControl::XYToPosition(long x, long y) const
{
    Point curpt ;
    wxTextPos lastpos ;

    // TODO: find a better implementation : while we can get the
    // line metrics of a certain line, we don't get its starting
    // position, so it would probably be rather a binary search
    // for the start position
    long xpos = 0, ypos = 0 ;
    int lastHeight = 0 ;
    ItemCount n ;

    lastpos = GetLastPosition() ;
    for ( n = 0 ; n <= (ItemCount) lastpos ; ++n )
    {
        if ( y == ypos && x == xpos )
            return n ;

        TXNOffsetToPoint( m_txn, n, &curpt ) ;

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

bool wxMacMLTEControl::PositionToXY( long pos, long *x, long *y ) const
{
    Point curpt ;
    wxTextPos lastpos ;

    if ( y )
        *y = 0 ;
    if ( x )
        *x = 0 ;

    lastpos = GetLastPosition() ;
    if ( pos <= lastpos )
    {
        // TODO: find a better implementation - while we can get the
        // line metrics of a certain line, we don't get its starting
        // position, so it would probably be rather a binary search
        // for the start position
        long xpos = 0, ypos = 0 ;
        int lastHeight = 0 ;
        ItemCount n ;

        for ( n = 0 ; n <= (ItemCount) pos ; ++n )
        {
            TXNOffsetToPoint( m_txn, n, &curpt ) ;

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

        if ( y )
            *y = ypos ;
        if ( x )
            *x = xpos ;
    }

    return false ;
}

void wxMacMLTEControl::ShowPosition( long pos )
{
#if TARGET_RT_MAC_MACHO && defined(AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER)
    {
        Point current, desired ;
        TXNOffset selstart, selend;

        TXNGetSelection( m_txn, &selstart, &selend );
        TXNOffsetToPoint( m_txn, selstart, &current );
        TXNOffsetToPoint( m_txn, pos, &desired );

        // TODO: use HIPoints for 10.3 and above
        if ( (UInt32)TXNScroll != (UInt32)kUnresolvedCFragSymbolAddress )
        {
            OSErr theErr = noErr;
            SInt32 dv = desired.v - current.v;
            SInt32 dh = desired.h - current.h;
            TXNShowSelection( m_txn, kTXNShowStart ) ; // NB: should this be kTXNShowStart or kTXNShowEnd ??
            theErr = TXNScroll( m_txn, kTXNScrollUnitsInPixels, kTXNScrollUnitsInPixels, &dv, &dh );

            // there will be an error returned for classic MLTE implementation when the control is
            // invisible, but HITextView works correctly, so we don't assert that one
            // wxASSERT_MSG( theErr == noErr, _T("TXNScroll returned an error!") );
        }
    }
#endif
}

void wxMacMLTEControl::SetTXNData( const wxString& st, TXNOffset start, TXNOffset end )
{
#if wxUSE_UNICODE
#if SIZEOF_WCHAR_T == 2
    size_t len = st.length() ;
    TXNSetData( m_txn, kTXNUnicodeTextData, (void*)st.wc_str(), len * 2, start, end );
#else
    wxMBConvUTF16 converter ;
    ByteCount byteBufferLen = converter.WC2MB( NULL, st.wc_str(), 0 ) ;
    UniChar *unibuf = (UniChar*)malloc( byteBufferLen ) ;
    converter.WC2MB( (char*)unibuf, st.wc_str(), byteBufferLen ) ;
    TXNSetData( m_txn, kTXNUnicodeTextData, (void*)unibuf, byteBufferLen, start, end ) ;
    free( unibuf ) ;
#endif
#else
    wxCharBuffer text = st.mb_str( wxConvLocal ) ;
    TXNSetData( m_txn, kTXNTextData, (void*)text.data(), strlen( text ), start, end ) ;
#endif
}

wxString wxMacMLTEControl::GetLineText(long lineNo) const
{
    wxString line ;

    if ( lineNo < GetNumberOfLines() )
    {
        Point firstPoint;
        Fixed lineWidth, lineHeight, currentHeight;
        long ypos ;

        // get the first possible position in the control
        TXNOffsetToPoint(m_txn, 0, &firstPoint);

        // Iterate through the lines until we reach the one we want,
        // adding to our current y pixel point position
        ypos = 0 ;
        currentHeight = 0;
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
        while (thePoint.v == currentPoint.v && theOffset < content.length())
        {
            line += content[theOffset];
            TXNOffsetToPoint(m_txn, ++theOffset, &currentPoint);
        }
    }

    return line ;
}

int wxMacMLTEControl::GetLineLength(long lineNo) const
{
    int theLength = 0;

    if ( lineNo < GetNumberOfLines() )
    {
        Point firstPoint;
        Fixed lineWidth, lineHeight, currentHeight;
        long ypos;

        // get the first possible position in the control
        TXNOffsetToPoint(m_txn, 0, &firstPoint);

        // Iterate through the lines until we reach the one we want,
        // adding to our current y pixel point position
        ypos = 0;
        currentHeight = 0;
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
        while (thePoint.v == currentPoint.v && theOffset < content.length())
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

// OS X Notes : We still don't have a full replacement for MLTE, so this implementation
// has to live on. We have different problems coming from outdated implementations on the
// various OS X versions. Most deal with the scrollbars: they are not correctly embedded
// while this can be solved on 10.3 by reassigning them the correct place, on 10.2 there is
// no way out, therefore we are using our own implementation and our own scrollbars ....

#ifdef __WXMAC_OSX__

TXNScrollInfoUPP gTXNScrollInfoProc = NULL ;
ControlActionUPP gTXNScrollActionProc = NULL ;

pascal void wxMacMLTEClassicControl::TXNScrollInfoProc(
    SInt32 iValue, SInt32 iMaximumValue,
    TXNScrollBarOrientation iScrollBarOrientation, SInt32 iRefCon )
{
    wxMacMLTEClassicControl* mlte = (wxMacMLTEClassicControl*) iRefCon ;
    SInt32 value =  wxMax( iValue , 0 ) ;
    SInt32 maximum = wxMax( iMaximumValue , 0 ) ;

    if ( iScrollBarOrientation == kTXNHorizontal )
    {
        if ( mlte->m_sbHorizontal )
        {
            SetControl32BitValue( mlte->m_sbHorizontal , value ) ;
            SetControl32BitMaximum( mlte->m_sbHorizontal , maximum ) ;
            mlte->m_lastHorizontalValue = value ;
        }
    }
    else if ( iScrollBarOrientation == kTXNVertical )
    {
        if ( mlte->m_sbVertical )
        {
            SetControl32BitValue( mlte->m_sbVertical , value ) ;
            SetControl32BitMaximum( mlte->m_sbVertical , maximum ) ;
            mlte->m_lastVerticalValue = value ;
        }
    }
}

pascal void wxMacMLTEClassicControl::TXNScrollActionProc( ControlRef controlRef , ControlPartCode partCode )
{
    wxMacMLTEClassicControl* mlte = (wxMacMLTEClassicControl*) GetControlReference( controlRef ) ;
    if ( mlte == NULL )
        return ;

    if ( controlRef != mlte->m_sbVertical && controlRef != mlte->m_sbHorizontal )
        return ;

    OSStatus err ;
    bool isHorizontal = ( controlRef == mlte->m_sbHorizontal ) ;

    SInt32 minimum = 0 ;
    SInt32 maximum = GetControl32BitMaximum( controlRef ) ;
    SInt32 value = GetControl32BitValue( controlRef ) ;
    SInt32 delta = 0;

    switch ( partCode )
    {
        case kControlDownButtonPart :
            delta = 10 ;
            break ;

        case kControlUpButtonPart :
            delta = -10 ;
            break ;

        case kControlPageDownPart :
            delta = GetControlViewSize( controlRef ) ;
            break ;

        case kControlPageUpPart :
            delta = -GetControlViewSize( controlRef ) ;
            break ;

        case kControlIndicatorPart :
            delta = value - (isHorizontal ? mlte->m_lastHorizontalValue : mlte->m_lastVerticalValue) ;
            break ;

        default :
            break ;
    }

    if ( delta != 0 )
    {
        SInt32 newValue = value ;

        if ( partCode != kControlIndicatorPart )
        {
            if ( value + delta < minimum )
                delta = minimum - value ;
            if ( value + delta > maximum )
                delta = maximum - value ;

            SetControl32BitValue( controlRef , value + delta ) ;
            newValue = value + delta ;
        }

        SInt32 verticalDelta = isHorizontal ? 0 : delta ;
        SInt32 horizontalDelta = isHorizontal ? delta : 0 ;

        err = TXNScroll(
            mlte->m_txn, kTXNScrollUnitsInPixels, kTXNScrollUnitsInPixels,
            &verticalDelta, &horizontalDelta );
        verify_noerr( err );

        if ( isHorizontal )
            mlte->m_lastHorizontalValue = newValue ;
        else
            mlte->m_lastVerticalValue = newValue ;
    }
}
#endif

// make correct activations
void wxMacMLTEClassicControl::MacActivatePaneText(bool setActive)
{
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(m_controlRef);

    wxMacWindowClipper clipper( textctrl ) ;
    TXNActivate( m_txn, m_txnFrameID, setActive );

    ControlRef controlFocus = 0 ;
    GetKeyboardFocus( m_txnWindow , &controlFocus ) ;
    if ( controlFocus == m_controlRef )
        TXNFocus( m_txn, setActive );
}

void wxMacMLTEClassicControl::MacFocusPaneText(bool setFocus)
{
    TXNFocus( m_txn, setFocus );
}

// guards against inappropriate redraw (hidden objects drawing onto window)

void wxMacMLTEClassicControl::MacSetObjectVisibility(bool vis)
{
    ControlRef controlFocus = 0 ;
    GetKeyboardFocus( m_txnWindow , &controlFocus ) ;

    if ( !vis && (controlFocus == m_controlRef ) )
        SetKeyboardFocus( m_txnWindow , m_controlRef , kControlFocusNoPart ) ;

    TXNControlTag iControlTags[1] = { kTXNVisibilityTag };
    TXNControlData iControlData[1] = { { (UInt32)false } };

    verify_noerr( TXNGetTXNObjectControls( m_txn , 1, iControlTags, iControlData ) ) ;

    if ( iControlData[0].uValue != vis )
    {
        iControlData[0].uValue = vis ;
        verify_noerr( TXNSetTXNObjectControls( m_txn, false , 1, iControlTags, iControlData ) ) ;
    }

    // currently, we always clip as partial visibility (overlapped) visibility is also a problem,
    // if we run into further problems we might set the FrameBounds to an empty rect here
}

// make sure that the TXNObject is at the right position

void wxMacMLTEClassicControl::MacUpdatePosition()
{
    wxTextCtrl* textctrl = (wxTextCtrl*)GetControlReference( m_controlRef );
    if ( textctrl == NULL )
        return ;

    Rect bounds ;
    UMAGetControlBoundsInWindowCoords( m_controlRef, &bounds );

    wxRect visRect = textctrl->MacGetClippedClientRect() ;
    Rect visBounds = { visRect.y , visRect.x , visRect.y + visRect.height , visRect.x + visRect.width } ;
    int x , y ;
    x = y = 0 ;
    textctrl->MacWindowToRootWindow( &x , &y ) ;
    OffsetRect( &visBounds , x , y ) ;

    if ( !EqualRect( &bounds, &m_txnControlBounds ) || !EqualRect( &visBounds, &m_txnVisBounds ) )
    {
        m_txnControlBounds = bounds ;
        m_txnVisBounds = visBounds ;
        wxMacWindowClipper cl( textctrl ) ;

#ifdef __WXMAC_OSX__
        if ( m_sbHorizontal || m_sbVertical )
        {
            int w = bounds.right - bounds.left ;
            int h = bounds.bottom - bounds.top ;

            if ( m_sbHorizontal )
            {
                Rect sbBounds ;

                sbBounds.left = -1 ;
                sbBounds.top = h - 14 ;
                sbBounds.right = w + 1 ;
                sbBounds.bottom = h + 1 ;

                SetControlBounds( m_sbHorizontal , &sbBounds ) ;
                SetControlViewSize( m_sbHorizontal , w ) ;
            }

            if ( m_sbVertical )
            {
                Rect sbBounds ;

                sbBounds.left = w - 14 ;
                sbBounds.top = -1 ;
                sbBounds.right = w + 1 ;
                sbBounds.bottom = m_sbHorizontal ? h - 14 : h + 1 ;

                SetControlBounds( m_sbVertical , &sbBounds ) ;
                SetControlViewSize( m_sbVertical , h ) ;
            }
        }

        Rect oldviewRect ;
        TXNLongRect olddestRect ;
        TXNGetRectBounds( m_txn , &oldviewRect , &olddestRect , NULL ) ;

        Rect viewRect = { m_txnControlBounds.top, m_txnControlBounds.left,
            m_txnControlBounds.bottom - ( m_sbHorizontal ? 14 : 0 ) ,
            m_txnControlBounds.right - ( m_sbVertical ? 14 : 0 ) } ;
        TXNLongRect destRect = { m_txnControlBounds.top, m_txnControlBounds.left,
            m_txnControlBounds.bottom - ( m_sbHorizontal ? 14 : 0 ) ,
            m_txnControlBounds.right - ( m_sbVertical ? 14 : 0 ) } ;

        if ( olddestRect.right >= 10000 )
            destRect.right = destRect.left + 32000 ;

        if ( olddestRect.bottom >= 0x20000000 )
            destRect.bottom = destRect.top + 0x40000000 ;

        SectRect( &viewRect , &visBounds , &viewRect ) ;
        TXNSetRectBounds( m_txn , &viewRect , &destRect , true ) ;

#if 0
        TXNSetFrameBounds(
            m_txn,
            m_txnControlBounds.top,
            m_txnControlBounds.left,
            m_txnControlBounds.bottom - (m_sbHorizontal ? 14 : 0),
            m_txnControlBounds.right - (m_sbVertical ? 14 : 0),
            m_txnFrameID );
#endif
#else

        TXNSetFrameBounds(
            m_txn, m_txnControlBounds.top, m_txnControlBounds.left,
            wxMax( m_txnControlBounds.bottom, m_txnControlBounds.top ),
            wxMax( m_txnControlBounds.right, m_txnControlBounds.left ), m_txnFrameID );
#endif

        // the SetFrameBounds method under Classic sometimes does not correctly scroll a selection into sight after a
        // movement, therefore we have to force it

        // this problem has been reported in OSX as well, so we use this here once again

        TXNLongRect textRect ;
        TXNGetRectBounds( m_txn , NULL , NULL , &textRect ) ;
        if ( textRect.left < m_txnControlBounds.left )
            TXNShowSelection( m_txn , kTXNShowStart ) ;
    }
}

void wxMacMLTEClassicControl::SetRect( Rect *r )
{
    wxMacControl::SetRect( r ) ;
    MacUpdatePosition() ;
}

void wxMacMLTEClassicControl::MacControlUserPaneDrawProc(wxInt16 thePart)
{
    wxTextCtrl* textctrl = (wxTextCtrl*)GetControlReference( m_controlRef );
    if ( textctrl == NULL )
        return ;

    if ( textctrl->MacIsReallyShown() )
    {
        wxMacWindowClipper clipper( textctrl ) ;
        TXNDraw( m_txn , NULL ) ;
    }
}

wxInt16 wxMacMLTEClassicControl::MacControlUserPaneHitTestProc(wxInt16 x, wxInt16 y)
{
    Point where = { y , x } ;
    ControlPartCode result = kControlNoPart;

    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference( m_controlRef );
    if ( (textctrl != NULL) && textctrl->MacIsReallyShown() )
    {
        if (PtInRect( where, &m_txnControlBounds ))
        {
            result = kControlEditTextPart ;
        }
        else
        {
            // sometimes we get the coords also in control local coordinates, therefore test again
            int x = 0 , y = 0 ;
            textctrl->MacClientToRootWindow( &x , &y ) ;
            where.h += x ;
            where.v += y ;

            if (PtInRect( where, &m_txnControlBounds ))
                result = kControlEditTextPart ;
        }
    }

    return result;
}

wxInt16 wxMacMLTEClassicControl::MacControlUserPaneTrackingProc( wxInt16 x, wxInt16 y, void* actionProc )
{
    ControlPartCode result = kControlNoPart;

    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference( m_controlRef );
    if ( (textctrl != NULL) && textctrl->MacIsReallyShown() )
    {
        Point startPt = { y , x } ;

        // for compositing, we must convert these into toplevel window coordinates, because hittesting expects them
        int x = 0 , y = 0 ;
        textctrl->MacClientToRootWindow( &x , &y ) ;
        startPt.h += x ;
        startPt.v += y ;

        switch (MacControlUserPaneHitTestProc( startPt.h , startPt.v ))
        {
            case kControlEditTextPart :
            {
                wxMacWindowClipper clipper( textctrl ) ;
                EventRecord rec ;

                ConvertEventRefToEventRecord( (EventRef) wxTheApp->MacGetCurrentEvent() , &rec ) ;
                TXNClick( m_txn, &rec );
            }
                break;

            default :
                break;
        }
    }

    return result;
}

void wxMacMLTEClassicControl::MacControlUserPaneIdleProc()
{
    wxTextCtrl* textctrl = (wxTextCtrl*)GetControlReference( m_controlRef );
    if ( textctrl == NULL )
        return ;

    if (textctrl->MacIsReallyShown())
    {
        if (IsControlActive(m_controlRef))
        {
            Point mousep;

            wxMacWindowClipper clipper( textctrl ) ;
            GetMouse(&mousep);

            TXNIdle(m_txn);

            if (PtInRect(mousep, &m_txnControlBounds))
            {
                RgnHandle theRgn = NewRgn();
                RectRgn(theRgn, &m_txnControlBounds);
                TXNAdjustCursor(m_txn, theRgn);
                DisposeRgn(theRgn);
            }
        }
    }
}

wxInt16 wxMacMLTEClassicControl::MacControlUserPaneKeyDownProc (wxInt16 keyCode, wxInt16 charCode, wxInt16 modifiers)
{
    wxTextCtrl* textctrl = (wxTextCtrl*)GetControlReference( m_controlRef );
    if ( textctrl == NULL )
        return kControlNoPart;

    wxMacWindowClipper clipper( textctrl ) ;

    EventRecord ev ;
    memset( &ev , 0 , sizeof( ev ) ) ;
    ev.what = keyDown ;
    ev.modifiers = modifiers ;
    ev.message = ((keyCode << 8) & keyCodeMask) | (charCode & charCodeMask);
    TXNKeyDown( m_txn , &ev );

    return kControlEntireControl;
}

void wxMacMLTEClassicControl::MacControlUserPaneActivateProc(bool activating)
{
    MacActivatePaneText( activating );
}

wxInt16 wxMacMLTEClassicControl::MacControlUserPaneFocusProc(wxInt16 action)
{
    ControlPartCode focusResult = kControlFocusNoPart;

    wxTextCtrl* textctrl = (wxTextCtrl*)GetControlReference( m_controlRef );
    if ( textctrl == NULL )
        return focusResult;

    wxMacWindowClipper clipper( textctrl ) ;

    ControlRef controlFocus = NULL ;
    GetKeyboardFocus( m_txnWindow , &controlFocus ) ;
    bool wasFocused = ( controlFocus == m_controlRef ) ;

    switch (action)
    {
        case kControlFocusPrevPart:
        case kControlFocusNextPart:
            MacFocusPaneText( !wasFocused );
            focusResult = (!wasFocused ? (ControlPartCode) kControlEditTextPart : (ControlPartCode) kControlFocusNoPart);
            break;

        case kControlFocusNoPart:
        default:
            MacFocusPaneText( false );
            focusResult = kControlFocusNoPart;
            break;
    }

    return focusResult;
}

void wxMacMLTEClassicControl::MacControlUserPaneBackgroundProc( void *info )
{
}

wxMacMLTEClassicControl::wxMacMLTEClassicControl( wxTextCtrl *wxPeer,
    const wxString& str,
    const wxPoint& pos,
    const wxSize& size, long style )
    : wxMacMLTEControl( wxPeer )
{
    m_font = wxPeer->GetFont() ;
    m_windowStyle = style ;
    Rect bounds = wxMacGetBoundsForControl( wxPeer , pos , size ) ;

    short featureSet =
        kControlSupportsEmbedding | kControlSupportsFocus | kControlWantsIdle
        | kControlWantsActivate  | kControlHandlesTracking
//    | kControlHasSpecialBackground
        | kControlGetsFocusOnClick | kControlSupportsLiveFeedback;

   OSStatus err = ::CreateUserPaneControl(
        MAC_WXHWND(wxPeer->GetParent()->MacGetTopLevelWindowRef()),
        &bounds, featureSet, &m_controlRef );
    verify_noerr( err );

    DoCreate();

    AdjustCreationAttributes( *wxWHITE , true ) ;

    MacSetObjectVisibility( wxPeer->MacIsReallyShown() ) ;

    {
        wxString st = str ;
        wxMacConvertNewlines10To13( &st ) ;
        wxMacWindowClipper clipper( m_peer ) ;
        SetTXNData( st , kTXNStartOffset, kTXNEndOffset ) ;
        TXNSetSelection( m_txn, 0, 0 ) ;
    }
}

wxMacMLTEClassicControl::~wxMacMLTEClassicControl()
{
    TXNDeleteObject( m_txn );
    m_txn = NULL ;
}

void wxMacMLTEClassicControl::VisibilityChanged(bool shown)
{
    MacSetObjectVisibility( shown ) ;
    wxMacControl::VisibilityChanged( shown ) ;
}

void wxMacMLTEClassicControl::SuperChangedPosition()
{
    MacUpdatePosition() ;
    wxMacControl::SuperChangedPosition() ;
}

#ifdef __WXMAC_OSX__

ControlUserPaneDrawUPP gTPDrawProc = NULL;
ControlUserPaneHitTestUPP gTPHitProc = NULL;
ControlUserPaneTrackingUPP gTPTrackProc = NULL;
ControlUserPaneIdleUPP gTPIdleProc = NULL;
ControlUserPaneKeyDownUPP gTPKeyProc = NULL;
ControlUserPaneActivateUPP gTPActivateProc = NULL;
ControlUserPaneFocusUPP gTPFocusProc = NULL;

static pascal void wxMacControlUserPaneDrawProc(ControlRef control, SInt16 part)
{
    wxTextCtrl *textCtrl =  wxDynamicCast( wxFindControlFromMacControl(control) , wxTextCtrl ) ;
    wxMacMLTEClassicControl * win = textCtrl ? (wxMacMLTEClassicControl*)(textCtrl->GetPeer()) : NULL ;
    if ( win )
        win->MacControlUserPaneDrawProc( part ) ;
}

static pascal ControlPartCode wxMacControlUserPaneHitTestProc(ControlRef control, Point where)
{
    wxTextCtrl *textCtrl =  wxDynamicCast( wxFindControlFromMacControl(control) , wxTextCtrl ) ;
    wxMacMLTEClassicControl * win = textCtrl ? (wxMacMLTEClassicControl*)(textCtrl->GetPeer()) : NULL ;
    if ( win )
        return win->MacControlUserPaneHitTestProc( where.h , where.v ) ;
    else
        return kControlNoPart ;
}

static pascal ControlPartCode wxMacControlUserPaneTrackingProc(ControlRef control, Point startPt, ControlActionUPP actionProc)
{
    wxTextCtrl *textCtrl =  wxDynamicCast( wxFindControlFromMacControl(control) , wxTextCtrl ) ;
    wxMacMLTEClassicControl * win = textCtrl ? (wxMacMLTEClassicControl*)(textCtrl->GetPeer()) : NULL ;
    if ( win )
        return win->MacControlUserPaneTrackingProc( startPt.h , startPt.v , (void*) actionProc ) ;
    else
        return kControlNoPart ;
}

static pascal void wxMacControlUserPaneIdleProc(ControlRef control)
{
    wxTextCtrl *textCtrl =  wxDynamicCast( wxFindControlFromMacControl(control) , wxTextCtrl ) ;
    wxMacMLTEClassicControl * win = textCtrl ? (wxMacMLTEClassicControl*)(textCtrl->GetPeer()) : NULL ;
    if ( win )
        win->MacControlUserPaneIdleProc() ;
}

static pascal ControlPartCode wxMacControlUserPaneKeyDownProc(ControlRef control, SInt16 keyCode, SInt16 charCode, SInt16 modifiers)
{
    wxTextCtrl *textCtrl =  wxDynamicCast( wxFindControlFromMacControl(control) , wxTextCtrl ) ;
    wxMacMLTEClassicControl * win = textCtrl ? (wxMacMLTEClassicControl*)(textCtrl->GetPeer()) : NULL ;
    if ( win )
        return win->MacControlUserPaneKeyDownProc( keyCode, charCode, modifiers ) ;
    else
        return kControlNoPart ;
}

static pascal void wxMacControlUserPaneActivateProc(ControlRef control, Boolean activating)
{
    wxTextCtrl *textCtrl =  wxDynamicCast( wxFindControlFromMacControl(control) , wxTextCtrl ) ;
    wxMacMLTEClassicControl * win = textCtrl ? (wxMacMLTEClassicControl*)(textCtrl->GetPeer()) : NULL ;
    if ( win )
        win->MacControlUserPaneActivateProc( activating ) ;
}

static pascal ControlPartCode wxMacControlUserPaneFocusProc(ControlRef control, ControlFocusPart action)
{
    wxTextCtrl *textCtrl =  wxDynamicCast( wxFindControlFromMacControl(control) , wxTextCtrl ) ;
    wxMacMLTEClassicControl * win = textCtrl ? (wxMacMLTEClassicControl*)(textCtrl->GetPeer()) : NULL ;
    if ( win )
        return win->MacControlUserPaneFocusProc( action ) ;
    else
        return kControlNoPart ;
}

#if 0
static pascal void wxMacControlUserPaneBackgroundProc(ControlRef control, ControlBackgroundPtr info)
{
    wxTextCtrl *textCtrl =  wxDynamicCast( wxFindControlFromMacControl(control) , wxTextCtrl ) ;
    wxMacMLTEClassicControl * win = textCtrl ? (wxMacMLTEClassicControl*)(textCtrl->GetPeer()) : NULL ;
    if ( win )
        win->MacControlUserPaneBackgroundProc(info) ;
}
#endif

#endif // __WXMAC_OSX__

// TXNRegisterScrollInfoProc

OSStatus wxMacMLTEClassicControl::DoCreate()
{
    Rect bounds;
    OSStatus err = noErr ;

    // set up our globals
#ifdef __WXMAC_OSX__
    if (gTPDrawProc == NULL) gTPDrawProc = NewControlUserPaneDrawUPP(wxMacControlUserPaneDrawProc);
    if (gTPHitProc == NULL) gTPHitProc = NewControlUserPaneHitTestUPP(wxMacControlUserPaneHitTestProc);
    if (gTPTrackProc == NULL) gTPTrackProc = NewControlUserPaneTrackingUPP(wxMacControlUserPaneTrackingProc);
    if (gTPIdleProc == NULL) gTPIdleProc = NewControlUserPaneIdleUPP(wxMacControlUserPaneIdleProc);
    if (gTPKeyProc == NULL) gTPKeyProc = NewControlUserPaneKeyDownUPP(wxMacControlUserPaneKeyDownProc);
    if (gTPActivateProc == NULL) gTPActivateProc = NewControlUserPaneActivateUPP(wxMacControlUserPaneActivateProc);
    if (gTPFocusProc == NULL) gTPFocusProc = NewControlUserPaneFocusUPP(wxMacControlUserPaneFocusProc);

    if (gTXNScrollInfoProc == NULL ) gTXNScrollInfoProc = NewTXNScrollInfoUPP(TXNScrollInfoProc) ;
    if (gTXNScrollActionProc == NULL ) gTXNScrollActionProc = NewControlActionUPP(TXNScrollActionProc) ;
#endif

    // set the initial settings for our private data

    m_txnWindow = GetControlOwner(m_controlRef);
    m_txnPort = (GrafPtr) GetWindowPort(m_txnWindow);

#ifdef __WXMAC_OSX__
    // set up the user pane procedures
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneDrawProcTag, sizeof(gTPDrawProc), &gTPDrawProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneHitTestProcTag, sizeof(gTPHitProc), &gTPHitProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneTrackingProcTag, sizeof(gTPTrackProc), &gTPTrackProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneIdleProcTag, sizeof(gTPIdleProc), &gTPIdleProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneKeyDownProcTag, sizeof(gTPKeyProc), &gTPKeyProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneActivateProcTag, sizeof(gTPActivateProc), &gTPActivateProc);
    SetControlData(m_controlRef, kControlEntireControl, kControlUserPaneFocusProcTag, sizeof(gTPFocusProc), &gTPFocusProc);
#endif

    // calculate the rectangles used by the control
    UMAGetControlBoundsInWindowCoords( m_controlRef, &bounds );

    m_txnControlBounds = bounds ;
    m_txnVisBounds = bounds ;

    CGrafPtr origPort ;
    GDHandle origDev ;

    GetGWorld( &origPort, &origDev ) ;
    SetPort( m_txnPort );

    // create the new edit field
    TXNFrameOptions frameOptions = FrameOptionsFromWXStyle( m_windowStyle );

#ifdef __WXMAC_OSX__
    // the scrollbars are not correctly embedded but are inserted at the root:
    // this gives us problems as we have erratic redraws even over the structure area

    m_sbHorizontal = 0 ;
    m_sbVertical = 0 ;
    m_lastHorizontalValue = 0 ;
    m_lastVerticalValue = 0 ;

    Rect sb = { 0 , 0 , 0 , 0 } ;
    if ( frameOptions & kTXNWantVScrollBarMask )
    {
        CreateScrollBarControl( m_txnWindow, &sb, 0, 0, 100, 1, true, gTXNScrollActionProc, &m_sbVertical );
        SetControlReference( m_sbVertical, (SInt32)this );
        SetControlAction( m_sbVertical, gTXNScrollActionProc );
        ShowControl( m_sbVertical );
        EmbedControl( m_sbVertical , m_controlRef );
        frameOptions &= ~kTXNWantVScrollBarMask;
    }

    if ( frameOptions & kTXNWantHScrollBarMask )
    {
        CreateScrollBarControl( m_txnWindow, &sb, 0, 0, 100, 1, true, gTXNScrollActionProc, &m_sbHorizontal );
        SetControlReference( m_sbHorizontal, (SInt32)this );
        SetControlAction( m_sbHorizontal, gTXNScrollActionProc );
        ShowControl( m_sbHorizontal );
        EmbedControl( m_sbHorizontal, m_controlRef );
        frameOptions &= ~(kTXNWantHScrollBarMask | kTXNDrawGrowIconMask);
    }

#endif

    err = TXNNewObject(
        NULL, m_txnWindow, &bounds, frameOptions,
        kTXNTextEditStyleFrameType, kTXNTextensionFile, kTXNSystemDefaultEncoding,
        &m_txn, &m_txnFrameID, NULL );
    verify_noerr( err );

#if 0
    TXNControlTag iControlTags[] = { kTXNUseCarbonEvents };
    TXNControlData iControlData[] = { { (UInt32)&cInfo } };
    int toptag = WXSIZEOF( iControlTags ) ;
    TXNCarbonEventInfo cInfo ;
    cInfo.useCarbonEvents = false ;
    cInfo.filler = 0 ;
    cInfo.flags = 0 ;
    cInfo.fDictionary = NULL ;

    verify_noerr( TXNSetTXNObjectControls( m_txn, false, toptag, iControlTags, iControlData ) );
#endif

#ifdef __WXMAC_OSX__
    TXNRegisterScrollInfoProc( m_txn, gTXNScrollInfoProc, (SInt32)this );
#endif

    SetGWorld( origPort , origDev ) ;

    return err;
}

// ----------------------------------------------------------------------------
// MLTE control implementation (OSX part)
// ----------------------------------------------------------------------------

#if TARGET_API_MAC_OSX

#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2

// tiger multi-line textcontrols with no CR in the entire content
// don't scroll automatically, so we need a hack.
// This attempt only works 'before' the key (ie before CallNextEventHandler)
// is processed, thus the scrolling always occurs one character too late, but
// better than nothing ...

static const EventTypeSpec eventList[] =
{
    { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent } ,
} ;

static pascal OSStatus wxMacUnicodeTextEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;
    wxMacMLTEHIViewControl* focus = (wxMacMLTEHIViewControl*) data ;

    switch ( GetEventKind( event ) )
    {
        case kEventTextInputUnicodeForKeyEvent :
        {
            if ( UMAGetSystemVersion() >= 0x1040 )
            {
                TXNOffset from , to ;
                TXNGetSelection( focus->GetTXNObject() , &from , &to ) ;
                if ( from == to )
                    TXNShowSelection( focus->GetTXNObject() , kTXNShowStart );
            }
            result = CallNextEventHandler(handler,event);
            break;
        }
        default:
            break ;
    }

    return result ;
}

static pascal OSStatus wxMacTextControlEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    switch ( GetEventClass( event ) )
    {
        case kEventClassTextInput :
            result = wxMacUnicodeTextEventHandler( handler , event , data ) ;
            break ;

        default :
            break ;
    }
    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacTextControlEventHandler )

wxMacMLTEHIViewControl::wxMacMLTEHIViewControl( wxTextCtrl *wxPeer,
    const wxString& str,
    const wxPoint& pos,
    const wxSize& size, long style ) : wxMacMLTEControl( wxPeer )
{
    m_font = wxPeer->GetFont() ;
    m_windowStyle = style ;
    Rect bounds = wxMacGetBoundsForControl( wxPeer , pos , size ) ;
    wxString st = str ;
    wxMacConvertNewlines10To13( &st ) ;

    HIRect hr = {
        { bounds.left , bounds.top },
        { bounds.right - bounds.left, bounds.bottom - bounds.top } } ;

    m_scrollView = NULL ;
    TXNFrameOptions frameOptions = FrameOptionsFromWXStyle( style ) ;
    if ( frameOptions & (kTXNWantVScrollBarMask | kTXNWantHScrollBarMask) )
    {
        HIScrollViewCreate(
            (frameOptions & kTXNWantHScrollBarMask ? kHIScrollViewOptionsHorizScroll : 0)
            | (frameOptions & kTXNWantVScrollBarMask ? kHIScrollViewOptionsVertScroll : 0) ,
            &m_scrollView ) ;

        HIViewSetFrame( m_scrollView, &hr );
        HIViewSetVisible( m_scrollView, true );
    }

    m_textView = NULL ;
    HITextViewCreate( NULL , 0, frameOptions , &m_textView ) ;
    m_txn = HITextViewGetTXNObject( m_textView ) ;
    HIViewSetVisible( m_textView , true ) ;
    if ( m_scrollView )
    {
        HIViewAddSubview( m_scrollView , m_textView ) ;
        m_controlRef = m_scrollView ;
        wxPeer->MacInstallEventHandler( (WXWidget) m_textView ) ;
    }
    else
    {
        HIViewSetFrame( m_textView, &hr );
        m_controlRef = m_textView ;
    }

    AdjustCreationAttributes( *wxWHITE , true ) ;

    wxMacWindowClipper c( m_peer ) ;
    SetTXNData( st , kTXNStartOffset, kTXNEndOffset ) ;

    TXNSetSelection( m_txn, 0, 0 );
    TXNShowSelection( m_txn, kTXNShowStart );

    InstallControlEventHandler( m_textView , GetwxMacTextControlEventHandlerUPP(),
                                GetEventTypeCount(eventList), eventList, this,
                                &m_textEventHandlerRef);
}

wxMacMLTEHIViewControl::~wxMacMLTEHIViewControl()
{
    ::RemoveEventHandler( m_textEventHandlerRef ) ;
}

OSStatus wxMacMLTEHIViewControl::SetFocus( ControlFocusPart focusPart )
{
    return SetKeyboardFocus( GetControlOwner( m_textView ), m_textView, focusPart ) ;
}

bool wxMacMLTEHIViewControl::HasFocus() const
{
    ControlRef control ;
    GetKeyboardFocus( GetUserFocusWindow() , &control ) ;
    return control == m_textView ;
}

void wxMacMLTEHIViewControl::SetBackground( const wxBrush &brush )
{
    wxMacMLTEControl::SetBackground( brush ) ;

#if 0
    CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
    RGBColor col = MAC_WXCOLORREF(brush.GetColour().GetPixel()) ;

    float component[4] ;
    component[0] = col.red / 65536.0 ;
    component[1] = col.green / 65536.0 ;
    component[2] = col.blue / 65536.0 ;
    component[3] = 1.0 ; // alpha

    CGColorRef color = CGColorCreate( rgbSpace , component );
    HITextViewSetBackgroundColor( m_textView , color );
    CGColorSpaceRelease( rgbSpace );
#endif
}

#endif // MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2


#endif

#endif // wxUSE_TEXTCTRL
