/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/textctrl_osx.cpp
// Purpose:     wxTextCtrl
// Author:      Stefan Csomor
// Modified by: Ryan Norton (MLTE GetLineLength and GetLineText)
// Created:     1998-01-01
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

#include "wx/filefn.h"
#include "wx/sysopt.h"
#include "wx/thread.h"

#include "wx/osx/private.h"

wxBEGIN_EVENT_TABLE(wxTextCtrl, wxTextCtrlBase)
    EVT_DROP_FILES(wxTextCtrl::OnDropFiles)
    EVT_CHAR(wxTextCtrl::OnChar)
    EVT_KEY_DOWN(wxTextCtrl::OnKeyDown)
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
wxEND_EVENT_TABLE()


void wxTextCtrl::Init()
{
    m_dirty = false;

    m_privateContextMenu = nullptr;
}

wxTextCtrl::~wxTextCtrl()
{
#if wxUSE_MENUS
    delete m_privateContextMenu;
#endif
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
    DontCreatePeer();
    m_editable = true ;

    if ( !wxTextCtrlBase::Create( parent, id, pos, size, style & ~(wxHSCROLL | wxVSCROLL), validator, name ) )
        return false;

    SetPeer(wxWidgetImpl::CreateTextControl( this, GetParent(), GetId(), str, pos, size, style, GetExtraStyle() ));

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
}

void wxTextCtrl::MacVisibilityChanged()
{
}

#if WXWIN_COMPATIBILITY_3_0 && wxUSE_SPELLCHECK
void wxTextCtrl::MacCheckSpelling(bool check)
{
    GetTextPeer()->CheckSpelling(check ? wxTextProofOptions::Default()
                                       : wxTextProofOptions::Disable());
}
#endif // WXWIN_COMPATIBILITY_3_0 && wxUSE_SPELLCHECK

void wxTextCtrl::OSXEnableNewLineReplacement(bool enable)
{
    GetTextPeer()->EnableNewLineReplacement(enable);
}

void wxTextCtrl::OSXEnableAutomaticQuoteSubstitution(bool enable)
{
    GetTextPeer()->EnableAutomaticQuoteSubstitution(enable);
}

void wxTextCtrl::OSXEnableAutomaticDashSubstitution(bool enable)
{
    GetTextPeer()->EnableAutomaticDashSubstitution(enable);
}

void wxTextCtrl::OSXDisableAllSmartSubstitutions()
{
    OSXEnableAutomaticDashSubstitution(false);
    OSXEnableAutomaticQuoteSubstitution(false);
}

wxTextSearchResult wxTextCtrl::SearchText(const wxTextSearch& search) const
{
    return GetTextPeer()->SearchText(search);
}

wxString wxTextCtrl::GetRTFValue() const
{
    return GetTextPeer()->GetRTFValue();
}

void wxTextCtrl::SetRTFValue(const wxString& val)
{
    GetTextPeer()->SetRTFValue(val);
}

bool wxTextCtrl::SetFont( const wxFont& font )
{
    if ( !wxTextCtrlBase::SetFont( font ) )
        return false ;

    GetPeer()->SetFont(font) ;

    return true ;
}

bool wxTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    if (GetTextPeer())
        GetTextPeer()->SetStyle( start , end , style ) ;

    return true ;
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    wxTextCtrlBase::SetDefaultStyle( style ) ;
    SetStyle( -1  /*current selection*/  , -1 /*current selection*/ , GetDefaultStyle() ) ;

    return true ;
}

bool wxTextCtrl::IsModified() const
{
    return m_dirty;
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    wxSize size;
    if (GetTextPeer())
        size = GetTextPeer()->GetBestSize();

    // Normally the width passed to GetSizeFromTextSize() is supposed to be
    // valid, i.e. positive, but we use our knowledge of its implementation
    // just below to rely on it returning the default size if we don't have
    // any valid best size in the peer and size remained default-initialized.
    return GetSizeFromTextSize(size);
}

wxSize wxTextCtrl::DoGetSizeFromTextSize(int xlen, int ylen) const
{
    static const int TEXTCTRL_BORDER_SIZE = 5;
    static const int TEXTCTRL_MAX_EMPTY_WIDTH = 5;

    // Compute the default height if not specified.
    int hText = ylen;
    if ( hText <= 0 )
    {
        // these are the numbers from the HIG:
        switch ( m_windowVariant )
        {
            case wxWINDOW_VARIANT_NORMAL :
                hText = 22;
                break ;

            case wxWINDOW_VARIANT_SMALL :
                hText = 19;
                break ;

            case wxWINDOW_VARIANT_MINI :
                hText = 15;
                break ;

            default :
                hText = 22;
                break ;
        }

        // the numbers above include the border size, so subtract it before
        // possibly adding it back below
        hText -= TEXTCTRL_BORDER_SIZE;

        // make the control 5 lines tall by default for consistently with how
        // the old code behaved
        if ( m_windowStyle & wxTE_MULTILINE )
            hText *= 5 ;
    }

    // Keep using the same default 100px width as was used previously in the
    // special case of having invalid width.
    // since this method is now called with native field widths, an empty field still
    // has small positive xlen, therefore don't compare just with > 0 anymore
    wxSize size(xlen > TEXTCTRL_MAX_EMPTY_WIDTH ? xlen : 100, hText);

    // Use extra margin size which works under macOS 10.15: note that we don't
    // need the vertical margin when using the automatically determined hText.
    if ( xlen > TEXTCTRL_MAX_EMPTY_WIDTH )
        size.x += 4;
    if ( ylen > 0 )
        size.y += 2;

    if ( !HasFlag(wxNO_BORDER) )
        size += wxSize(TEXTCTRL_BORDER_SIZE, TEXTCTRL_BORDER_SIZE) ;

    return size;
}

bool wxTextCtrl::GetStyle(long position, wxTextAttr& style)
{
    return GetTextPeer()->GetStyle(position, style);
}

void wxTextCtrl::MarkDirty()
{
    m_dirty = true;
}

void wxTextCtrl::DiscardEdits()
{
    m_dirty = false;
}

void wxTextCtrl::EmptyUndoBuffer()
{
    wxCHECK_RET( GetTextPeer(), "Must create the control first" );

    GetTextPeer()->EmptyUndoBuffer() ;
}

int wxTextCtrl::GetNumberOfLines() const
{
    return GetTextPeer()->GetNumberOfLines() ;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    return GetTextPeer()->XYToPosition( x , y ) ;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    return GetTextPeer()->PositionToXY( pos , x , y ) ;
}

void wxTextCtrl::ShowPosition(long pos)
{
    return GetTextPeer()->ShowPosition(pos) ;
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    return GetTextPeer()->GetLineLength(lineNo) ;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    return GetTextPeer()->GetLineText(lineNo) ;
}

void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
        wxClipboardTextEvent evt(wxEVT_TEXT_COPY, GetId());
        evt.SetEventObject(this);
        if (!GetEventHandler()->ProcessEvent(evt))
        {
            wxTextEntry::Copy();
        }
    }
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
        wxClipboardTextEvent evt(wxEVT_TEXT_CUT, GetId());
        evt.SetEventObject(this);
        if (!GetEventHandler()->ProcessEvent(evt))
        {
            wxTextEntry::Cut();

            SendTextUpdatedEvent();
        }
    }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        wxClipboardTextEvent evt(wxEVT_TEXT_PASTE, GetId());
        evt.SetEventObject(this);
        if (!GetEventHandler()->ProcessEvent(evt))
        {
            wxTextEntry::Paste();

            // TODO: eventually we should add setting the default style again
            SendTextUpdatedEvent();
        }
    }
}

#if wxUSE_SPELLCHECK

bool wxTextCtrl::EnableProofCheck(const wxTextProofOptions& options)
{
    GetTextPeer()->CheckSpelling(options);

    return true;
}

wxTextProofOptions wxTextCtrl::GetProofCheckOptions() const
{
    return GetTextPeer()->GetCheckingOptions();
}

#endif // wxUSE_SPELLCHECK

void wxTextCtrl::OnDropFiles(wxDropFilesEvent& event)
{
    // By default, load the first file into the text window.
    if (event.GetNumberOfFiles() > 0)
        LoadFile( event.GetFiles()[0] );
}

void wxTextCtrl::OnKeyDown(wxKeyEvent& event)
{
    if ( event.ControlDown() )
    {
        switch( event.GetKeyCode() )
        {
            case 'A':
                SelectAll();
                return;
            case 'C':
                if ( CanCopy() )
                    Copy() ;
                return;
            case 'V':
                if ( CanPaste() )
                    Paste() ;
                return;
            case 'X':
                if ( CanCut() )
                    Cut() ;
                return;
            case 'Z':
                if ( !event.ShiftDown() )
                {
                    if ( CanUndo() )
                        Undo() ;
                    return;
                }
                // else fall through to Redo
                wxFALLTHROUGH;
            case 'Y':
                if ( CanRedo() )
                    Redo() ;
                return;
            default:
                break;
        }
    }
    // no, we didn't process it
    event.Skip();
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    int key = event.GetKeyCode() ;
    bool eat_key = false ;
    long from, to;

    if ( !IsEditable() &&
        !event.IsKeyInCategory(WXK_CATEGORY_ARROW | WXK_CATEGORY_TAB) &&
        !( (key == WXK_RETURN || key == WXK_NUMPAD_ENTER) &&
        ( (m_windowStyle & wxTE_PROCESS_ENTER) || (m_windowStyle & wxTE_MULTILINE) ) )
//        && key != WXK_PAGEUP && key != WXK_PAGEDOWN && key != WXK_HOME && key != WXK_END
        )
    {
        // eat it
        return ;
    }

    if ( !GetTextPeer()->CanClipMaxLength() )
    {
        // Check if we have reached the max # of chars (if it is set), but still
        // allow navigation and deletion
        GetSelection( &from, &to );
        if ( !IsMultiLine() && m_maxLength && GetValue().length() >= m_maxLength &&
            !event.IsKeyInCategory(WXK_CATEGORY_ARROW | WXK_CATEGORY_TAB | WXK_CATEGORY_CUT) &&
            !( (key == WXK_RETURN || key == WXK_NUMPAD_ENTER) &&
            (m_windowStyle & wxTE_PROCESS_ENTER) ) &&
            from == to )
        {
            // eat it, we don't want to add more than allowed # of characters

            // TODO: generate EVT_TEXT_MAXLEN()
            return;
        }
    }

    // assume that any key not processed yet is going to modify the control
    m_dirty = true;

    switch ( key )
    {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            if (m_windowStyle & wxTE_PROCESS_ENTER)
            {
                wxCommandEvent evt(wxEVT_TEXT_ENTER, m_windowId);
                evt.SetEventObject(this);
                evt.SetString(GetValue());
                if (HandleWindowEvent(evt))
                    return;
            }

            if ( GetTextPeer()->GetNewLineReplacement() )
            {
                wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
                if ( tlw && tlw->GetDefaultItem() )
                {
                    wxButton *def = wxDynamicCast(tlw->GetDefaultItem(), wxButton);
                    if ( def && def->IsEnabled() )
                    {
                        wxCommandEvent evt(wxEVT_BUTTON, def->GetId());
                        evt.SetEventObject(def);
                        def->Command(evt);

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

    // osx_cocoa sends its event upon insertText
}

void wxTextCtrl::Command(wxCommandEvent & event)
{
    SetValue(event.GetString());
    ProcessCommand(event);
}

void wxTextCtrl::SetWindowStyleFlag(long style)
{
    long styleOld = GetWindowStyleFlag();

    wxTextCtrlBase::SetWindowStyleFlag(style);

    static const long flagsAlign = wxTE_LEFT | wxTE_CENTRE | wxTE_RIGHT;
    if ( (style & flagsAlign) != (styleOld & flagsAlign) )
        GetTextPeer()->SetJustification();
}

// ----------------------------------------------------------------------------
// standard handlers for standard edit menu events
// ----------------------------------------------------------------------------

// CS: Context Menus only work with MLTE implementations or non-multiline HIViews at the moment

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

void wxTextCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    if ( GetTextPeer()->HasOwnContextMenu() )
    {
        event.Skip() ;
        return ;
    }

#if wxUSE_MENUS
    if (m_privateContextMenu == nullptr)
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

    PopupMenu(m_privateContextMenu);
#endif
}

bool wxTextCtrl::MacSetupCursor( const wxPoint& pt )
{
    if ( !GetTextPeer()->SetupCursor( pt ) )
        return wxWindow::MacSetupCursor( pt ) ;
    else
        return true ;
}

// ----------------------------------------------------------------------------
// implementation base class
// ----------------------------------------------------------------------------

bool wxTextWidgetImpl::GetStyle(long WXUNUSED(position),
                                wxTextAttr& WXUNUSED(style))
{
    return false;
}

void wxTextWidgetImpl::SetStyle(long WXUNUSED(start),
                                long WXUNUSED(end),
                                const wxTextAttr& WXUNUSED(style))
{
}

void wxTextWidgetImpl::Copy()
{
}

void wxTextWidgetImpl::Cut()
{
}

void wxTextWidgetImpl::Paste()
{
}

bool wxTextWidgetImpl::CanPaste() const
{
    return false ;
}

void wxTextWidgetImpl::SetEditable(bool WXUNUSED(editable))
{
}

long wxTextWidgetImpl::GetLastPosition() const
{
    return GetStringValue().length() ;
}

void wxTextWidgetImpl::Replace( long from , long to , const wxString &val )
{
    SetSelection( from , to ) ;
    WriteText( val ) ;
}

void wxTextWidgetImpl::Remove( long from , long to )
{
    SetSelection( from , to ) ;
    WriteText( wxEmptyString) ;
}

void wxTextWidgetImpl::Clear()
{
    SetStringValue( wxEmptyString ) ;
}

bool wxTextWidgetImpl::CanUndo() const
{
    return false ;
}

void wxTextWidgetImpl::Undo()
{
}

bool wxTextWidgetImpl::CanRedo()  const
{
    return false ;
}

void wxTextWidgetImpl::Redo()
{
}

void wxTextWidgetImpl::EmptyUndoBuffer()
{
}

long wxTextWidgetImpl::XYToPosition(long WXUNUSED(x), long WXUNUSED(y)) const
{
    return 0 ;
}

bool wxTextWidgetImpl::PositionToXY(long WXUNUSED(pos),
                                    long *WXUNUSED(x),
                                    long *WXUNUSED(y)) const
{
    return false ;
}

void wxTextWidgetImpl::ShowPosition( long WXUNUSED(pos) )
{
}

int wxTextWidgetImpl::GetNumberOfLines() const
{
    wxString content = GetStringValue() ;
    ItemCount lines = 1;

    for (size_t i = 0; i < content.length() ; i++)
    {
#if wxOSX_USE_COCOA
        if (content[i] == '\n')
#else
        if (content[i] == '\r')
#endif
            lines++;
    }

    return lines ;
}

wxString wxTextWidgetImpl::GetLineText(long lineNo) const
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

int wxTextWidgetImpl::GetLineLength(long lineNo) const
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
                if (content[j] == '\n')
                    return count;

                count++;
            }

            return count;
        }

        if (content[i] == '\n')
            count++;
    }

    return -1 ;
}

#if wxUSE_SPELLCHECK

wxTextProofOptions wxTextWidgetImpl::GetCheckingOptions() const
{
    return wxTextProofOptions::Disable();
}

#endif // wxUSE_SPELLCHECK

void wxTextWidgetImpl::SetJustification()
{
}
#endif // wxUSE_TEXTCTRL
