/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richeditctrl.cpp
// Purpose:     A rich edit control
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/menu.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/stopwatch.h"
#endif

#include "wx/textfile.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx/dcbuffer.h"
#include "wx/arrimpl.cpp"

// DLL options compatibility check:
#include "wx/app.h"
WX_CHECK_BUILD_OPTIONS("wxRichTextCtrl")

DEFINE_EVENT_TYPE(wxEVT_COMMAND_RICHTEXT_ITEM_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_RICHTEXT_ITEM_DESELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_RICHTEXT_LEFT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_RICHTEXT_RETURN)

#if wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE
IMPLEMENT_CLASS( wxRichTextCtrl, wxControl )
#else
IMPLEMENT_CLASS( wxRichTextCtrl, wxScrolledWindow )
#endif

IMPLEMENT_CLASS( wxRichTextEvent, wxNotifyEvent )

#if wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE
BEGIN_EVENT_TABLE( wxRichTextCtrl, wxControl )
#else
BEGIN_EVENT_TABLE( wxRichTextCtrl, wxScrolledWindow )
#endif
    EVT_PAINT(wxRichTextCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(wxRichTextCtrl::OnEraseBackground)
    EVT_IDLE(wxRichTextCtrl::OnIdle)
    EVT_SCROLLWIN(wxRichTextCtrl::OnScroll)
    EVT_LEFT_DOWN(wxRichTextCtrl::OnLeftClick)
    EVT_MOTION(wxRichTextCtrl::OnMoveMouse)
    EVT_LEFT_UP(wxRichTextCtrl::OnLeftUp)
    EVT_RIGHT_DOWN(wxRichTextCtrl::OnRightClick)
    EVT_MIDDLE_DOWN(wxRichTextCtrl::OnMiddleClick)
    EVT_LEFT_DCLICK(wxRichTextCtrl::OnLeftDClick)
    EVT_CHAR(wxRichTextCtrl::OnChar)
    EVT_SIZE(wxRichTextCtrl::OnSize)
    EVT_SET_FOCUS(wxRichTextCtrl::OnSetFocus)
    EVT_KILL_FOCUS(wxRichTextCtrl::OnKillFocus)
    EVT_CONTEXT_MENU(wxRichTextCtrl::OnContextMenu)

    EVT_MENU(wxID_UNDO, wxRichTextCtrl::OnUndo)
    EVT_UPDATE_UI(wxID_UNDO, wxRichTextCtrl::OnUpdateUndo)

    EVT_MENU(wxID_REDO, wxRichTextCtrl::OnRedo)
    EVT_UPDATE_UI(wxID_REDO, wxRichTextCtrl::OnUpdateRedo)

    EVT_MENU(wxID_COPY, wxRichTextCtrl::OnCopy)
    EVT_UPDATE_UI(wxID_COPY, wxRichTextCtrl::OnUpdateCopy)

    EVT_MENU(wxID_PASTE, wxRichTextCtrl::OnPaste)
    EVT_UPDATE_UI(wxID_PASTE, wxRichTextCtrl::OnUpdatePaste)

    EVT_MENU(wxID_CUT, wxRichTextCtrl::OnCut)
    EVT_UPDATE_UI(wxID_CUT, wxRichTextCtrl::OnUpdateCut)

    EVT_MENU(wxID_CLEAR, wxRichTextCtrl::OnClear)
    EVT_UPDATE_UI(wxID_CLEAR, wxRichTextCtrl::OnUpdateClear)

    EVT_MENU(wxID_SELECTALL, wxRichTextCtrl::OnSelectAll)
    EVT_UPDATE_UI(wxID_SELECTALL, wxRichTextCtrl::OnUpdateSelectAll)
END_EVENT_TABLE()

/*!
 * wxRichTextCtrl
 */

wxRichTextCtrl::wxRichTextCtrl()
#if wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE
    : wxScrollHelper(this)
#endif
{
    Init();
}

wxRichTextCtrl::wxRichTextCtrl( wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style)
#if wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE
    : wxScrollHelper(this)
#endif
{
    Init();
    Create(parent, id, value, pos, size, style);
}

/// Creation
bool wxRichTextCtrl::Create( wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style)
{
#if wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE
    if (!wxTextCtrlBase::Create(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE
        ))
        return false;
#else
    if (!wxScrolledWindow::Create(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE
        ))
        return false;
#endif

    if (!GetFont().Ok())
    {
        SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    }

    GetBuffer().SetRichTextCtrl(this);

    wxTextAttrEx attributes;
    attributes.SetFont(GetFont());
    attributes.SetTextColour(*wxBLACK);
    attributes.SetBackgroundColour(*wxWHITE);
    attributes.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
    attributes.SetFlags(wxTEXT_ATTR_ALL);

    SetDefaultStyle(attributes);
    SetBasicStyle(attributes);

    SetBackgroundColour(*wxWHITE);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    // Tell the sizers to use the given or best size
    SetBestFittingSize(size);

    // Create a buffer
    RecreateBuffer(size);

    SetCursor(wxCursor(wxCURSOR_IBEAM));

    if (!value.IsEmpty())
        SetValue(value);

    return true;
}

wxRichTextCtrl::~wxRichTextCtrl()
{
    delete m_contextMenu;
}

/// Member initialisation
void wxRichTextCtrl::Init()
{
    m_freezeCount = 0;
    m_contextMenu = NULL;
    m_caret = NULL;
    m_caretPosition = -1;
    m_selectionRange.SetRange(-2, -2);
    m_selectionAnchor = -2;
    m_editable = true;
    m_caretAtLineStart = false;
    m_dragging = false;
    m_fullLayoutRequired = false;
    m_fullLayoutTime = 0;
    m_fullLayoutSavedPosition = 0;
    m_delayedLayoutThreshold = wxRICHTEXT_DEFAULT_DELAYED_LAYOUT_THRESHOLD;
    m_caretPositionForDefaultStyle = -2;
}

/// Call Freeze to prevent refresh
void wxRichTextCtrl::Freeze()
{
    m_freezeCount ++;
}

/// Call Thaw to refresh
void wxRichTextCtrl::Thaw()
{
    m_freezeCount --;

    if (m_freezeCount == 0)
    {
        SetupScrollbars();
        Refresh(false);
    }
}

/// Clear all text
void wxRichTextCtrl::Clear()
{
    m_buffer.Reset();
    m_buffer.SetDirty(true);
    m_caretPosition = -1;
    m_caretPositionForDefaultStyle = -2;
    m_caretAtLineStart = false;
    m_selectionRange.SetRange(-2, -2);

    SetScrollbars(0, 0, 0, 0, 0, 0);

    if (m_freezeCount == 0)
    {
        SetupScrollbars();
        Refresh(false);
    }
    SendUpdateEvent();
}

/// Painting
void wxRichTextCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    if (GetCaret())
        GetCaret()->Hide();

    {
        wxBufferedPaintDC dc(this, m_bufferBitmap);
        //wxLogDebug(wxT("OnPaint"));

        PrepareDC(dc);

        if (m_freezeCount > 0)
            return;

        dc.SetFont(GetFont());

        // Paint the background
        PaintBackground(dc);

        wxRegion dirtyRegion = GetUpdateRegion();

        wxRect drawingArea(GetLogicalPoint(wxPoint(0, 0)), GetClientSize());
        wxRect availableSpace(GetClientSize());
        if (GetBuffer().GetDirty())
        {
            GetBuffer().Layout(dc, availableSpace, wxRICHTEXT_FIXED_WIDTH|wxRICHTEXT_VARIABLE_HEIGHT);
            GetBuffer().SetDirty(false);
            SetupScrollbars();
        }

        GetBuffer().Draw(dc, GetBuffer().GetRange(), GetInternalSelectionRange(), drawingArea, 0 /* descent */, 0 /* flags */);
    }

    if (GetCaret())
        GetCaret()->Show();

    PositionCaret();
}

// Empty implementation, to prevent flicker
void wxRichTextCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxRichTextCtrl::OnSetFocus(wxFocusEvent& WXUNUSED(event))
{
    wxCaret* caret = new wxCaret(this, wxRICHTEXT_DEFAULT_CARET_WIDTH, 16);
    SetCaret(caret);
    caret->Show();
    PositionCaret();

    if (!IsFrozen())
        Refresh(false);
}

void wxRichTextCtrl::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    SetCaret(NULL);

    if (!IsFrozen())
        Refresh(false);
}

/// Left-click
void wxRichTextCtrl::OnLeftClick(wxMouseEvent& event)
{
    SetFocus();

    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(GetFont());

    long position = 0;
    int hit = GetBuffer().HitTest(dc, event.GetLogicalPosition(dc), position);

    if (hit != wxRICHTEXT_HITTEST_NONE)
    {
        m_dragStart = event.GetLogicalPosition(dc);
        m_dragging = true;
        CaptureMouse();

        SelectNone();

        bool caretAtLineStart = false;

        if (hit & wxRICHTEXT_HITTEST_BEFORE)
        {
            // If we're at the start of a line (but not first in para)
            // then we should keep the caret showing at the start of the line
            // by showing the m_caretAtLineStart flag.
            wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(position);
            wxRichTextLine* line = GetBuffer().GetLineAtPosition(position);

            if (line && para && line->GetAbsoluteRange().GetStart() == position && para->GetRange().GetStart() != position)
                caretAtLineStart = true;
            position --;
        }

        MoveCaret(position, caretAtLineStart);
        SetDefaultStyleToCursorStyle();

#if 0
        wxWindow* p = GetParent();
        while (p && !p->IsKindOf(CLASSINFO(wxFrame)))
            p = p->GetParent();

        wxFrame* frame = wxDynamicCast(p, wxFrame);
        if (frame)
        {
            wxString msg = wxString::Format(wxT("Found position %ld"), position);
            frame->SetStatusText(msg, 1);
        }
#endif
    }

    event.Skip();
}

/// Left-up
void wxRichTextCtrl::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
    if (m_dragging)
    {
        m_dragging = false;
        if (GetCapture() == this)
            ReleaseMouse();
    }
}

/// Left-click
void wxRichTextCtrl::OnMoveMouse(wxMouseEvent& event)
{
    if (!event.Dragging())
    {
        event.Skip();
        return;
    }

    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(GetFont());

    long position = 0;
    wxPoint logicalPt = event.GetLogicalPosition(dc);
    int hit = GetBuffer().HitTest(dc, logicalPt, position);

    if (m_dragging && hit != wxRICHTEXT_HITTEST_NONE)
    {
        // TODO: test closeness

        bool caretAtLineStart = false;

        if (hit & wxRICHTEXT_HITTEST_BEFORE)
        {
            // If we're at the start of a line (but not first in para)
            // then we should keep the caret showing at the start of the line
            // by showing the m_caretAtLineStart flag.
            wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(position);
            wxRichTextLine* line = GetBuffer().GetLineAtPosition(position);

            if (line && para && line->GetAbsoluteRange().GetStart() == position && para->GetRange().GetStart() != position)
                caretAtLineStart = true;
            position --;
        }

        if (m_caretPosition != position)
        {
            bool extendSel = ExtendSelection(m_caretPosition, position, wxRICHTEXT_SHIFT_DOWN);

            MoveCaret(position, caretAtLineStart);
            SetDefaultStyleToCursorStyle();

            if (extendSel)
                Refresh(false);
        }
    }
}

/// Right-click
void wxRichTextCtrl::OnRightClick(wxMouseEvent& event)
{
    SetFocus();
    event.Skip();
}

/// Left-double-click
void wxRichTextCtrl::OnLeftDClick(wxMouseEvent& event)
{
    event.Skip();
}

/// Middle-click
void wxRichTextCtrl::OnMiddleClick(wxMouseEvent& event)
{
    event.Skip();
}

/// Key press
void wxRichTextCtrl::OnChar(wxKeyEvent& event)
{
    int flags = 0;
    if (event.ControlDown())
        flags |= wxRICHTEXT_CTRL_DOWN;
    if (event.ShiftDown())
        flags |= wxRICHTEXT_SHIFT_DOWN;
    if (event.AltDown())
        flags |= wxRICHTEXT_ALT_DOWN;

    if (event.GetKeyCode() == WXK_LEFT ||
        event.GetKeyCode() == WXK_RIGHT ||
        event.GetKeyCode() == WXK_UP ||
        event.GetKeyCode() == WXK_DOWN ||
        event.GetKeyCode() == WXK_HOME ||
        event.GetKeyCode() == WXK_PAGEUP ||
        event.GetKeyCode() == WXK_PAGEDOWN ||
        event.GetKeyCode() == WXK_END)
    {
        KeyboardNavigate(event.GetKeyCode(), flags);
        return;
    }

    // all the other keys modify the controls contents which shouldn't be
    // possible if we're read-only
    if ( !IsEditable() )
    {
        event.Skip();
        return;
    }

    if (event.GetKeyCode() == WXK_RETURN)
    {
        BeginBatchUndo(_("Insert Text"));

        long newPos = m_caretPosition;

        DeleteSelectedContent(& newPos);

        GetBuffer().InsertNewlineWithUndo(newPos+1, this);

        wxRichTextEvent cmdEvent(
            wxEVT_COMMAND_RICHTEXT_RETURN,
            GetId());
        cmdEvent.SetEventObject(this);
        cmdEvent.SetFlags(flags);
        GetEventHandler()->ProcessEvent(cmdEvent);

        EndBatchUndo();
        SetDefaultStyleToCursorStyle();

        ScrollIntoView(m_caretPosition, WXK_RIGHT);
    }
    else if (event.GetKeyCode() == WXK_BACK)
    {
        BeginBatchUndo(_("Delete Text"));

        // Submit range in character positions, which are greater than caret positions,
        // so subtract 1 for deleted character and add 1 for conversion to character position.
        if (m_caretPosition > -1 && !HasSelection())
        {
            GetBuffer().DeleteRangeWithUndo(wxRichTextRange(m_caretPosition, m_caretPosition),
                m_caretPosition,   // Current caret position
                m_caretPosition-1, // New caret position
                this);
        }
        else
            DeleteSelectedContent();

        EndBatchUndo();

        // Shouldn't this be in Do()?
        if (GetLastPosition() == -1)
        {
            GetBuffer().Reset();

            m_caretPosition = -1;
            PositionCaret();
            SetDefaultStyleToCursorStyle();
        }

        ScrollIntoView(m_caretPosition, WXK_LEFT);
    }
    else if (event.GetKeyCode() == WXK_DELETE)
    {
        BeginBatchUndo(_("Delete Text"));

        // Submit range in character positions, which are greater than caret positions,
        if (m_caretPosition < GetBuffer().GetRange().GetEnd()+1 && !HasSelection())
        {
            GetBuffer().DeleteRangeWithUndo(wxRichTextRange(m_caretPosition+1, m_caretPosition+1),
                m_caretPosition,   // Current caret position
                m_caretPosition+1, // New caret position
                this);
        }
        else
            DeleteSelectedContent();

        EndBatchUndo();

        // Shouldn't this be in Do()?
        if (GetLastPosition() == -1)
        {
            GetBuffer().Reset();

            m_caretPosition = -1;
            PositionCaret();
            SetDefaultStyleToCursorStyle();
        }
    }
    else
    {
        BeginBatchUndo(_("Insert Text"));

        long newPos = m_caretPosition;
        DeleteSelectedContent(& newPos);

        wxString str = (wxChar) event.GetKeyCode();
        GetBuffer().InsertTextWithUndo(newPos+1, str, this);

        EndBatchUndo();

        SetDefaultStyleToCursorStyle();
        ScrollIntoView(m_caretPosition, WXK_RIGHT);
    }
}

/// Delete content if there is a selection, e.g. when pressing a key.
bool wxRichTextCtrl::DeleteSelectedContent(long* newPos)
{
    if (HasSelection())
    {
        long pos = m_selectionRange.GetStart();
        GetBuffer().DeleteRangeWithUndo(m_selectionRange,
            m_caretPosition,       // Current caret position
            pos,    // New caret position
            this);
        m_selectionRange.SetRange(-2, -2);

        if (newPos)
            *newPos = pos-1;
        return true;
    }
    else
        return false;
}

/// Keyboard navigation

/*

Left:       left one character
Right:      right one character
Up:         up one line
Down:       down one line
Ctrl-Left:  left one word
Ctrl-Right: right one word
Ctrl-Up:    previous paragraph start
Ctrl-Down:  next start of paragraph
Home:       start of line
End:        end of line
Ctrl-Home:  start of document
Ctrl-End:   end of document
Page-Up:    Up a screen
Page-Down:  Down a screen

Maybe:

Ctrl-Alt-PgUp: Start of window
Ctrl-Alt-PgDn: End of window
F8:         Start selection mode
Esc:        End selection mode

Adding Shift does the above but starts/extends selection.


 */

bool wxRichTextCtrl::KeyboardNavigate(int keyCode, int flags)
{
    bool success = false;

    if (keyCode == WXK_RIGHT)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = WordRight(1, flags);
        else
            success = MoveRight(1, flags);
    }
    else if (keyCode == WXK_LEFT)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = WordLeft(1, flags);
        else
            success = MoveLeft(1, flags);
    }
    else if (keyCode == WXK_UP)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = MoveToParagraphStart(flags);
        else
            success = MoveUp(1, flags);
    }
    else if (keyCode == WXK_DOWN)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = MoveToParagraphEnd(flags);
        else
            success = MoveDown(1, flags);
    }
    else if (keyCode == WXK_PAGEUP)
    {
        success = PageUp(1, flags);
    }
    else if (keyCode == WXK_PAGEDOWN)
    {
        success = PageDown(1, flags);
    }
    else if (keyCode == WXK_HOME)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = MoveHome(flags);
        else
            success = MoveToLineStart(flags);
    }
    else if (keyCode == WXK_END)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = MoveEnd(flags);
        else
            success = MoveToLineEnd(flags);
    }

    if (success)
    {
        ScrollIntoView(m_caretPosition, keyCode);
        SetDefaultStyleToCursorStyle();
    }

    return success;
}

/// Extend the selection. Selections are in caret positions.
bool wxRichTextCtrl::ExtendSelection(long oldPos, long newPos, int flags)
{
    if (flags & wxRICHTEXT_SHIFT_DOWN)
    {
        // If not currently selecting, start selecting
        if (m_selectionRange.GetStart() == -2)
        {
            m_selectionAnchor = oldPos;

            if (oldPos > newPos)
                m_selectionRange.SetRange(newPos+1, oldPos);
            else
                m_selectionRange.SetRange(oldPos+1, newPos);
        }
        else
        {
            // Always ensure that the selection range start is greater than
            // the end.
            if (newPos > m_selectionAnchor)
                m_selectionRange.SetRange(m_selectionAnchor+1, newPos);
            else
                m_selectionRange.SetRange(newPos+1, m_selectionAnchor);
        }

        if (m_selectionRange.GetStart() > m_selectionRange.GetEnd())
        {
            wxLogDebug(wxT("Strange selection range"));
        }

        return true;
    }
    else
        return false;
}

/// Scroll into view, returning true if we scrolled.
/// This takes a _caret_ position.
bool wxRichTextCtrl::ScrollIntoView(long position, int keyCode)
{
    wxRichTextLine* line = GetVisibleLineForCaretPosition(position);

    if (!line)
        return false;

    int ppuX, ppuY;
    GetScrollPixelsPerUnit(& ppuX, & ppuY);

    int startXUnits, startYUnits;
    GetViewStart(& startXUnits, & startYUnits);
    int startY = startYUnits * ppuY;

    int sx = 0, sy = 0;
    GetVirtualSize(& sx, & sy);
    int sxUnits = 0;
    int syUnits = 0;
    if (ppuY != 0)
        syUnits = sy/ppuY;

    wxRect rect = line->GetRect();

    bool scrolled = false;

    wxSize clientSize = GetClientSize();

    // Going down
    if (keyCode == WXK_DOWN || keyCode == WXK_RIGHT || keyCode == WXK_END || keyCode == WXK_PAGEDOWN)
    {
        if ((rect.y + rect.height) > (clientSize.y + startY))
        {
            // Make it scroll so this item is at the bottom
            // of the window
            int y = rect.y - (clientSize.y - rect.height);
            int yUnits = (int) (0.5 + ((float) y)/(float) ppuY);

            // If we're still off the screen, scroll another line down
            if ((rect.y + rect.height) > (clientSize.y + (yUnits*ppuY)))
                yUnits ++;

            if (startYUnits != yUnits)
            {
                SetScrollbars(ppuX, ppuY, sxUnits, syUnits, 0, yUnits);
                scrolled = true;
            }
        }
        else if (rect.y < startY)
        {
            // Make it scroll so this item is at the top
            // of the window
            int y = rect.y ;
            int yUnits = (int) (0.5 + ((float) y)/(float) ppuY);

            if (startYUnits != yUnits)
            {
                SetScrollbars(ppuX, ppuY, sxUnits, syUnits, 0, yUnits);
                scrolled = true;
            }
        }
    }
    // Going up
    else if (keyCode == WXK_UP || keyCode == WXK_LEFT || keyCode == WXK_HOME || keyCode == WXK_PAGEUP )
    {
        if (rect.y < startY)
        {
            // Make it scroll so this item is at the top
            // of the window
            int y = rect.y ;
            int yUnits = (int) (0.5 + ((float) y)/(float) ppuY);

            if (startYUnits != yUnits)
            {
                SetScrollbars(ppuX, ppuY, sxUnits, syUnits, 0, yUnits);
                scrolled = true;
            }
        }
        else if ((rect.y + rect.height) > (clientSize.y + startY))
        {
            // Make it scroll so this item is at the bottom
            // of the window
            int y = rect.y - (clientSize.y - rect.height);
            int yUnits = (int) (0.5 + ((float) y)/(float) ppuY);

            // If we're still off the screen, scroll another line down
            if ((rect.y + rect.height) > (clientSize.y + (yUnits*ppuY)))
                yUnits ++;

            if (startYUnits != yUnits)
            {
                SetScrollbars(ppuX, ppuY, sxUnits, syUnits, 0, yUnits);
                scrolled = true;
            }
        }
    }
    PositionCaret();

    return scrolled;
}

/// Is the given position visible on the screen?
bool wxRichTextCtrl::IsPositionVisible(long pos) const
{
    wxRichTextLine* line = GetVisibleLineForCaretPosition(pos-1);

    if (!line)
        return false;

    int ppuX, ppuY;
    GetScrollPixelsPerUnit(& ppuX, & ppuY);

    int startX, startY;
    GetViewStart(& startX, & startY);
    startX = 0;
    startY = startY * ppuY;

    int sx = 0, sy = 0;
    GetVirtualSize(& sx, & sy);
    sx = 0;
    if (ppuY != 0)
        sy = sy/ppuY;

    wxRect rect = line->GetRect();

    wxSize clientSize = GetClientSize();

    return !(((rect.y + rect.height) > (clientSize.y + startY)) || rect.y < startY);
}

void wxRichTextCtrl::SetCaretPosition(long position, bool showAtLineStart)
{
    m_caretPosition = position;
    m_caretAtLineStart = showAtLineStart;
}

/// Move caret one visual step forward: this may mean setting a flag
/// and keeping the same position if we're going from the end of one line
/// to the start of the next, which may be the exact same caret position.
void wxRichTextCtrl::MoveCaretForward(long oldPosition)
{
    wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(oldPosition);

    // Only do the check if we're not at the end of the paragraph (where things work OK
    // anyway)
    if (para && (oldPosition != para->GetRange().GetEnd() - 1))
    {
        wxRichTextLine* line = GetBuffer().GetLineAtPosition(oldPosition);

        if (line)
        {
            wxRichTextRange lineRange = line->GetAbsoluteRange();

            // We're at the end of a line. See whether we need to
            // stay at the same actual caret position but change visual
            // position, or not.
            if (oldPosition == lineRange.GetEnd())
            {
                if (m_caretAtLineStart)
                {
                    // We're already at the start of the line, so actually move on now.
                    m_caretPosition = oldPosition + 1;
                    m_caretAtLineStart = false;
                }
                else
                {
                    // We're showing at the end of the line, so keep to
                    // the same position but indicate that we're to show
                    // at the start of the next line.
                    m_caretPosition = oldPosition;
                    m_caretAtLineStart = true;
                }
                SetDefaultStyleToCursorStyle();
                return;
            }
        }
    }
    m_caretPosition ++;
    SetDefaultStyleToCursorStyle();
}

/// Move caret one visual step backward: this may mean setting a flag
/// and keeping the same position if we're going from the end of one line
/// to the start of the next, which may be the exact same caret position.
void wxRichTextCtrl::MoveCaretBack(long oldPosition)
{
    wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(oldPosition);

    // Only do the check if we're not at the start of the paragraph (where things work OK
    // anyway)
    if (para && (oldPosition != para->GetRange().GetStart()))
    {
        wxRichTextLine* line = GetBuffer().GetLineAtPosition(oldPosition);

        if (line)
        {
            wxRichTextRange lineRange = line->GetAbsoluteRange();

            // We're at the start of a line. See whether we need to
            // stay at the same actual caret position but change visual
            // position, or not.
            if (oldPosition == lineRange.GetStart())
            {
                m_caretPosition = oldPosition-1;
                m_caretAtLineStart = true;
                return;
            }
            else if (oldPosition == lineRange.GetEnd())
            {
                if (m_caretAtLineStart)
                {
                    // We're at the start of the line, so keep the same caret position
                    // but clear the start-of-line flag.
                    m_caretPosition = oldPosition;
                    m_caretAtLineStart = false;
                }
                else
                {
                    // We're showing at the end of the line, so go back
                    // to the previous character position.
                    m_caretPosition = oldPosition - 1;
                }
                SetDefaultStyleToCursorStyle();
                return;
            }
        }
    }
    m_caretPosition --;
    SetDefaultStyleToCursorStyle();
}

/// Move right
bool wxRichTextCtrl::MoveRight(int noPositions, int flags)
{
    long endPos = GetBuffer().GetRange().GetEnd();

    if (m_caretPosition + noPositions < endPos)
    {
        long oldPos = m_caretPosition;
        long newPos = m_caretPosition + noPositions;

        bool extendSel = ExtendSelection(m_caretPosition, newPos, flags);
        if (!extendSel)
            SelectNone();

        // Determine by looking at oldPos and m_caretPosition whether
        // we moved from the end of a line to the start of the next line, in which case
        // we want to adjust the caret position such that it is positioned at the
        // start of the next line, rather than jumping past the first character of the
        // line.
        if (noPositions == 1 && !extendSel)
            MoveCaretForward(oldPos);
        else
            SetCaretPosition(newPos);

        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }
    else
        return false;
}

/// Move left
bool wxRichTextCtrl::MoveLeft(int noPositions, int flags)
{
    long startPos = -1;

    if (m_caretPosition > startPos - noPositions + 1)
    {
        long oldPos = m_caretPosition;
        long newPos = m_caretPosition - noPositions;
        bool extendSel = ExtendSelection(m_caretPosition, newPos, flags);
        if (!extendSel)
            SelectNone();

        if (noPositions == 1 && !extendSel)
            MoveCaretBack(oldPos);
        else
            SetCaretPosition(newPos);

        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }
    else
        return false;
}

/// Move up
bool wxRichTextCtrl::MoveUp(int noLines, int flags)
{
    return MoveDown(- noLines, flags);
}

/// Move up
bool wxRichTextCtrl::MoveDown(int noLines, int flags)
{
    if (!GetCaret())
        return false;

    long lineNumber = GetBuffer().GetVisibleLineNumber(m_caretPosition, true, m_caretAtLineStart);
    wxPoint pt = GetCaret()->GetPosition();
    long newLine = lineNumber + noLines;

    if (lineNumber != -1)
    {
        if (noLines > 0)
        {
            long lastLine = GetBuffer().GetVisibleLineNumber(GetBuffer().GetRange().GetEnd());

            if (newLine > lastLine)
                return false;
        }
        else
        {
            if (newLine < 0)
                return false;
        }
    }

    wxRichTextLine* lineObj = GetBuffer().GetLineForVisibleLineNumber(newLine);
    if (lineObj)
    {
        pt.y = lineObj->GetAbsolutePosition().y + 2;
    }
    else
        return false;

    long newPos = 0;
    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(GetFont());

    int hitTest = GetBuffer().HitTest(dc, pt, newPos);

    if (hitTest != wxRICHTEXT_HITTEST_NONE)
    {
        // If end of previous line, and hitTest is wxRICHTEXT_HITTEST_BEFORE,
        // we want to be at the end of the last line but with m_caretAtLineStart set to true,
        // so we view the caret at the start of the line.
        bool caretLineStart = false;
        if (hitTest == wxRICHTEXT_HITTEST_BEFORE)
        {
            wxRichTextLine* thisLine = GetBuffer().GetLineAtPosition(newPos-1);
            wxRichTextRange lineRange;
            if (thisLine)
                lineRange = thisLine->GetAbsoluteRange();

            if (thisLine && (newPos-1) == lineRange.GetEnd())
            {
                newPos --;
                caretLineStart = true;
            }
            else
            {
                wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(newPos);
                if (para && para->GetRange().GetStart() == newPos)
                    newPos --;
            }
        }

        long newSelEnd = newPos;

        bool extendSel = ExtendSelection(m_caretPosition, newSelEnd, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(newPos, caretLineStart);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }

    return false;
}

/// Move to the end of the paragraph
bool wxRichTextCtrl::MoveToParagraphEnd(int flags)
{
    wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(m_caretPosition, true);
    if (para)
    {
        long newPos = para->GetRange().GetEnd() - 1;
        bool extendSel = ExtendSelection(m_caretPosition, newPos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(newPos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }

    return false;
}

/// Move to the start of the paragraph
bool wxRichTextCtrl::MoveToParagraphStart(int flags)
{
    wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(m_caretPosition, true);
    if (para)
    {
        long newPos = para->GetRange().GetStart() - 1;
        bool extendSel = ExtendSelection(m_caretPosition, newPos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(newPos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }

    return false;
}

/// Move to the end of the line
bool wxRichTextCtrl::MoveToLineEnd(int flags)
{
    wxRichTextLine* line = GetVisibleLineForCaretPosition(m_caretPosition);

    if (line)
    {
        wxRichTextRange lineRange = line->GetAbsoluteRange();
        long newPos = lineRange.GetEnd();
        bool extendSel = ExtendSelection(m_caretPosition, newPos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(newPos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }

    return false;
}

/// Move to the start of the line
bool wxRichTextCtrl::MoveToLineStart(int flags)
{
    wxRichTextLine* line = GetVisibleLineForCaretPosition(m_caretPosition);
    if (line)
    {
        wxRichTextRange lineRange = line->GetAbsoluteRange();
        long newPos = lineRange.GetStart()-1;

        bool extendSel = ExtendSelection(m_caretPosition, newPos, flags);
        if (!extendSel)
            SelectNone();

        wxRichTextParagraph* para = GetBuffer().GetParagraphForLine(line);

        SetCaretPosition(newPos, para->GetRange().GetStart() != lineRange.GetStart());
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }

    return false;
}

/// Move to the start of the buffer
bool wxRichTextCtrl::MoveHome(int flags)
{
    if (m_caretPosition != -1)
    {
        bool extendSel = ExtendSelection(m_caretPosition, -1, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(-1);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }
    else
        return false;
}

/// Move to the end of the buffer
bool wxRichTextCtrl::MoveEnd(int flags)
{
    long endPos = GetBuffer().GetRange().GetEnd()-1;

    if (m_caretPosition != endPos)
    {
        bool extendSel = ExtendSelection(m_caretPosition, endPos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(endPos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }
    else
        return false;
}

/// Move noPages pages up
bool wxRichTextCtrl::PageUp(int noPages, int flags)
{
    return PageDown(- noPages, flags);
}

/// Move noPages pages down
bool wxRichTextCtrl::PageDown(int noPages, int flags)
{
    // Calculate which line occurs noPages * screen height further down.
    wxRichTextLine* line = GetVisibleLineForCaretPosition(m_caretPosition);
    if (line)
    {
        wxSize clientSize = GetClientSize();
        int newY = line->GetAbsolutePosition().y + noPages*clientSize.y;

        wxRichTextLine* newLine = GetBuffer().GetLineAtYPosition(newY);
        if (newLine)
        {
            wxRichTextRange lineRange = newLine->GetAbsoluteRange();
            long pos = lineRange.GetStart()-1;
            if (pos != m_caretPosition)
            {
                wxRichTextParagraph* para = GetBuffer().GetParagraphForLine(newLine);

                bool extendSel = ExtendSelection(m_caretPosition, pos, flags);
                if (!extendSel)
                    SelectNone();

                SetCaretPosition(pos, para->GetRange().GetStart() != lineRange.GetStart());
                PositionCaret();
                SetDefaultStyleToCursorStyle();

                if (extendSel)
                    Refresh(false);
                return true;
            }
        }
    }

    return false;
}

// Finds the caret position for the next word
long wxRichTextCtrl::FindNextWordPosition(int direction) const
{
    long endPos = GetBuffer().GetRange().GetEnd();

    if (direction > 0)
    {
        long i = m_caretPosition+1+direction; // +1 for conversion to character pos

        // First skip current text to space
        while (i < endPos && i > -1)
        {
            // i is in character, not caret positions
            wxString text = GetBuffer().GetTextForRange(wxRichTextRange(i, i));
            if (text != wxT(" ") && !text.empty())
                i += direction;
            else
            {
                break;
            }
        }
        while (i < endPos && i > -1)
        {
            // i is in character, not caret positions
            wxString text = GetBuffer().GetTextForRange(wxRichTextRange(i, i));
            if (text.empty()) // End of paragraph, or maybe an image
                return wxMax(-1, i - 1);
            else if (text == wxT(" ") || text.empty())
                i += direction;
            else
            {
                // Convert to caret position
                return wxMax(-1, i - 1);
            }
        }
        if (i >= endPos)
            return endPos-1;
        return i-1;
    }
    else
    {
        long i = m_caretPosition;

        // First skip white space
        while (i < endPos && i > -1)
        {
            // i is in character, not caret positions
            wxString text = GetBuffer().GetTextForRange(wxRichTextRange(i, i));
            if (text.empty()) // End of paragraph, or maybe an image
                break;
            else if (text == wxT(" ") || text.empty())
                i += direction;
            else
                break;
        }
        // Next skip current text to space
        while (i < endPos && i > -1)
        {
            // i is in character, not caret positions
            wxString text = GetBuffer().GetTextForRange(wxRichTextRange(i, i));
            if (text != wxT(" ") /* && !text.empty() */)
                i += direction;
            else
            {
                return i;
            }
        }
        if (i < -1)
            return -1;
        return i;
    }
}

/// Move n words left
bool wxRichTextCtrl::WordLeft(int WXUNUSED(n), int flags)
{
    long pos = FindNextWordPosition(-1);
    if (pos != m_caretPosition)
    {
        wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(pos, true);

        bool extendSel = ExtendSelection(m_caretPosition, pos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(pos, para->GetRange().GetStart() != pos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }

    return false;
}

/// Move n words right
bool wxRichTextCtrl::WordRight(int WXUNUSED(n), int flags)
{
    long pos = FindNextWordPosition(1);
    if (pos != m_caretPosition)
    {
        wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(pos, true);

        bool extendSel = ExtendSelection(m_caretPosition, pos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(pos, para->GetRange().GetStart() != pos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        if (extendSel)
            Refresh(false);
        return true;
    }

    return false;
}

/// Sizing
void wxRichTextCtrl::OnSize(wxSizeEvent& event)
{
    // Only do sizing optimization for large buffers
    if (GetBuffer().GetRange().GetEnd() > m_delayedLayoutThreshold)
    {
        m_fullLayoutRequired = true;
        m_fullLayoutTime = wxGetLocalTimeMillis();
        m_fullLayoutSavedPosition = GetFirstVisiblePosition();
        LayoutContent(true /* onlyVisibleRect */);
    }
    else
        GetBuffer().Invalidate(wxRICHTEXT_ALL);

    RecreateBuffer();

    event.Skip();
}


/// Idle-time processing
void wxRichTextCtrl::OnIdle(wxIdleEvent& event)
{
    const int layoutInterval = wxRICHTEXT_DEFAULT_LAYOUT_INTERVAL;

    if (m_fullLayoutRequired && (wxGetLocalTimeMillis() > (m_fullLayoutTime + layoutInterval)))
    {
        m_fullLayoutRequired = false;
        m_fullLayoutTime = 0;
        GetBuffer().Invalidate(wxRICHTEXT_ALL);
        ShowPosition(m_fullLayoutSavedPosition);
        Refresh(false);
    }

    if (m_caretPositionForDefaultStyle != -2)
    {
        // If the caret position has changed, no longer reflect the default style
        // in the UI.
        if (GetCaretPosition() != m_caretPositionForDefaultStyle)
            m_caretPositionForDefaultStyle = -2;
    }

    event.Skip();
}

/// Scrolling
void wxRichTextCtrl::OnScroll(wxScrollWinEvent& event)
{
    // Not used
    event.Skip();
}

/// Set up scrollbars, e.g. after a resize
void wxRichTextCtrl::SetupScrollbars(bool atTop)
{
    if (m_freezeCount)
        return;

    if (GetBuffer().IsEmpty())
    {
        SetScrollbars(0, 0, 0, 0, 0, 0);
        return;
    }

    // TODO: reimplement scrolling so we scroll by line, not by fixed number
    // of pixels. See e.g. wxVScrolledWindow for ideas.
    int pixelsPerUnit = 5;
    wxSize clientSize = GetClientSize();

    int maxHeight = GetBuffer().GetCachedSize().y;

    // Round up so we have at least maxHeight pixels
    int unitsY = (int) (((float)maxHeight/(float)pixelsPerUnit) + 0.5);

    int startX = 0, startY = 0;
    if (!atTop)
        GetViewStart(& startX, & startY);

    int maxPositionX = 0; // wxMax(sz.x - clientSize.x, 0);
    int maxPositionY = (int) ((((float)(wxMax((unitsY*pixelsPerUnit) - clientSize.y, 0)))/((float)pixelsPerUnit)) + 0.5);

    // Move to previous scroll position if
    // possible
    SetScrollbars(0, pixelsPerUnit,
        0, unitsY,
        wxMin(maxPositionX, startX), wxMin(maxPositionY, startY));
}

/// Paint the background
void wxRichTextCtrl::PaintBackground(wxDC& dc)
{
    wxColour backgroundColour = GetBackgroundColour();
    if (!backgroundColour.Ok())
        backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    // Clear the background
    dc.SetBrush(wxBrush(backgroundColour));
    dc.SetPen(*wxTRANSPARENT_PEN);
    wxRect windowRect(GetClientSize());
    windowRect.x -= 2; windowRect.y -= 2;
    windowRect.width += 4; windowRect.height += 4;

    // We need to shift the rectangle to take into account
    // scrolling. Converting device to logical coordinates.
    CalcUnscrolledPosition(windowRect.x, windowRect.y, & windowRect.x, & windowRect.y);
    dc.DrawRectangle(windowRect);
}

/// Recreate buffer bitmap if necessary
bool wxRichTextCtrl::RecreateBuffer(const wxSize& size)
{
    wxSize sz = size;
    if (sz == wxDefaultSize)
        sz = GetClientSize();

    if (sz.x < 1 || sz.y < 1)
        return false;

    if (!m_bufferBitmap.Ok() || m_bufferBitmap.GetWidth() < sz.x || m_bufferBitmap.GetHeight() < sz.y)
        m_bufferBitmap = wxBitmap(sz.x, sz.y);
    return m_bufferBitmap.Ok();
}

// ----------------------------------------------------------------------------
// file IO functions
// ----------------------------------------------------------------------------

#if !wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE
bool wxRichTextCtrl::LoadFile(const wxString& filename, int fileType)
{
    return DoLoadFile(filename, fileType);    
}

bool wxRichTextCtrl::SaveFile(const wxString& filename, int fileType)
{
    wxString filenameToUse = filename.empty() ? m_filename : filename;
    if ( filenameToUse.empty() )
    {
        // what kind of message to give? is it an error or a program bug?
        wxLogDebug(wxT("Can't save textctrl to file without filename."));

        return false;
    }

    return DoSaveFile(filenameToUse, fileType);
}
#endif

bool wxRichTextCtrl::DoLoadFile(const wxString& filename, int fileType)
{
    bool success = GetBuffer().LoadFile(filename, fileType);
    if (success)
        m_filename = filename;

    DiscardEdits();
    SetInsertionPoint(0);
    LayoutContent();
    PositionCaret();
    SetupScrollbars(true);
    Refresh(false);
    SendUpdateEvent();

    if (success)
        return true;
    else
    {
        wxLogError(_("File couldn't be loaded."));

        return false;
    }
}

bool wxRichTextCtrl::DoSaveFile(const wxString& filename, int fileType)
{
    if (GetBuffer().SaveFile(filename, fileType))
    {
        m_filename = filename;

        DiscardEdits();

        return true;
    }

    wxLogError(_("The text couldn't be saved."));

    return false;
}

// ----------------------------------------------------------------------------
// wxRichTextCtrl specific functionality
// ----------------------------------------------------------------------------

/// Add a new paragraph of text to the end of the buffer
wxRichTextRange wxRichTextCtrl::AddParagraph(const wxString& text)
{
    return GetBuffer().AddParagraph(text);
}

/// Add an image
wxRichTextRange wxRichTextCtrl::AddImage(const wxImage& image)
{
    return GetBuffer().AddImage(image);
}

// ----------------------------------------------------------------------------
// selection and ranges
// ----------------------------------------------------------------------------

void wxRichTextCtrl::SelectAll()
{
    SetSelection(0, GetLastPosition()+1);
    m_selectionAnchor = -1;
}

/// Select none
void wxRichTextCtrl::SelectNone()
{
    if (!(GetSelectionRange() == wxRichTextRange(-2, -2)))
        SetSelection(-2, -2);
    m_selectionAnchor = -2;
}

wxString wxRichTextCtrl::GetStringSelection() const
{
    long from, to;
    GetSelection(&from, &to);

    return GetRange(from, to);
}

// do the window-specific processing after processing the update event
#if !wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE
void wxRichTextCtrl::DoUpdateWindowUI(wxUpdateUIEvent& event)
{
    // call inherited
    wxWindowBase::DoUpdateWindowUI(event);

    // update text
    if ( event.GetSetText() )
    {
        if ( event.GetText() != GetValue() )
            SetValue(event.GetText());
    }
}
#endif // !wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE

// ----------------------------------------------------------------------------
// hit testing
// ----------------------------------------------------------------------------

wxTextCtrlHitTestResult
wxRichTextCtrl::HitTest(const wxPoint& pt, wxTextCoord *x, wxTextCoord *y) const
{
    // implement in terms of the other overload as the native ports typically
    // can get the position and not (x, y) pair directly (although wxUniv
    // directly gets x and y -- and so overrides this method as well)
    long pos;
    wxTextCtrlHitTestResult rc = HitTest(pt, &pos);

    if ( rc != wxTE_HT_UNKNOWN )
    {
        PositionToXY(pos, x, y);
    }

    return rc;
}

wxTextCtrlHitTestResult
wxRichTextCtrl::HitTest(const wxPoint& pt,
                        long * pos) const
{
    wxClientDC dc((wxRichTextCtrl*) this);
    ((wxRichTextCtrl*)this)->PrepareDC(dc);

    // Buffer uses logical position (relative to start of buffer)
    // so convert
    wxPoint pt2 = GetLogicalPoint(pt);

    int hit = ((wxRichTextCtrl*)this)->GetBuffer().HitTest(dc, pt2, *pos);

    switch ( hit )
    {
        case wxRICHTEXT_HITTEST_BEFORE:
            return wxTE_HT_BEFORE;

        case wxRICHTEXT_HITTEST_AFTER:
            return wxTE_HT_BEYOND;

        case wxRICHTEXT_HITTEST_ON:
            return wxTE_HT_ON_TEXT;
    }

    return wxTE_HT_UNKNOWN;
}

// ----------------------------------------------------------------------------
// set/get the controls text
// ----------------------------------------------------------------------------

wxString wxRichTextCtrl::GetValue() const
{
    return GetBuffer().GetText();
}

wxString wxRichTextCtrl::GetRange(long from, long to) const
{
    // Public API for range is different from internals
    return GetBuffer().GetTextForRange(wxRichTextRange(from, to-1));
}

void wxRichTextCtrl::SetValue(const wxString& value)
{
    Clear();

    // if the text is long enough, it's faster to just set it instead of first
    // comparing it with the old one (chances are that it will be different
    // anyhow, this comparison is there to avoid flicker for small single-line
    // edit controls mostly)
    if ( (value.length() > 0x400) || (value != GetValue()) )
    {
        DoWriteText(value, false /* not selection only */);

        // for compatibility, don't move the cursor when doing SetValue()
        SetInsertionPoint(0);
    }
    else // same text
    {
        // still send an event for consistency
        SendUpdateEvent();
    }

    // we should reset the modified flag even if the value didn't really change

    // mark the control as being not dirty - we changed its text, not the
    // user
    DiscardEdits();
}

void wxRichTextCtrl::WriteText(const wxString& value)
{
    DoWriteText(value);
}

void wxRichTextCtrl::DoWriteText(const wxString& value, bool WXUNUSED(selectionOnly))
{
    GetBuffer().InsertTextWithUndo(m_caretPosition+1, value, this);
}

void wxRichTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();

    WriteText(text);
}

/// Write an image at the current insertion point
bool wxRichTextCtrl::WriteImage(const wxImage& image, int bitmapType)
{
    wxRichTextImageBlock imageBlock;

    wxImage image2 = image;
    if (imageBlock.MakeImageBlock(image2, bitmapType))
        return WriteImage(imageBlock);

    return false;
}

bool wxRichTextCtrl::WriteImage(const wxString& filename, int bitmapType)
{
    wxRichTextImageBlock imageBlock;

    wxImage image;
    if (imageBlock.MakeImageBlock(filename, bitmapType, image, false))
        return WriteImage(imageBlock);

    return false;
}

bool wxRichTextCtrl::WriteImage(const wxRichTextImageBlock& imageBlock)
{
    return GetBuffer().InsertImageWithUndo(m_caretPosition+1, imageBlock, this);
}

bool wxRichTextCtrl::WriteImage(const wxBitmap& bitmap, int bitmapType)
{
    if (bitmap.Ok())
    {
        wxRichTextImageBlock imageBlock;

        wxImage image = bitmap.ConvertToImage();
        if (image.Ok() && imageBlock.MakeImageBlock(image, bitmapType))
            return WriteImage(imageBlock);
    }

    return false;
}

/// Insert a newline (actually paragraph) at the current insertion point.
bool wxRichTextCtrl::Newline()
{
    return GetBuffer().InsertNewlineWithUndo(m_caretPosition+1, this);
}


// ----------------------------------------------------------------------------
// Clipboard operations
// ----------------------------------------------------------------------------

void wxRichTextCtrl::Copy()
{
    if (CanCopy())
    {
        wxRichTextRange range = GetSelectionRange();
        GetBuffer().CopyToClipboard(range);
    }
}

void wxRichTextCtrl::Cut()
{
    if (CanCut())
    {
        wxRichTextRange range = GetSelectionRange();
        GetBuffer().CopyToClipboard(range);

        DeleteSelectedContent();
        LayoutContent();
        Refresh(false);
    }
}

void wxRichTextCtrl::Paste()
{
    if (CanPaste())
    {
        BeginBatchUndo(_("Paste"));

        long newPos = m_caretPosition;
        DeleteSelectedContent(& newPos);

        GetBuffer().PasteFromClipboard(newPos);

        EndBatchUndo();
    }
}

void wxRichTextCtrl::DeleteSelection()
{
    if (CanDeleteSelection())
    {
        DeleteSelectedContent();
    }
}

bool wxRichTextCtrl::HasSelection() const
{
    return m_selectionRange.GetStart() != -2 && m_selectionRange.GetEnd() != -2;
}

bool wxRichTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    return HasSelection();
}

bool wxRichTextCtrl::CanCut() const
{
    return HasSelection() && IsEditable();
}

bool wxRichTextCtrl::CanPaste() const
{
    if ( !IsEditable() )
        return false;

    return GetBuffer().CanPasteFromClipboard();
}

bool wxRichTextCtrl::CanDeleteSelection() const
{
    return HasSelection() && IsEditable();
}


// ----------------------------------------------------------------------------
// Accessors
// ----------------------------------------------------------------------------

void wxRichTextCtrl::SetEditable(bool editable)
{
    m_editable = editable;
}

void wxRichTextCtrl::SetInsertionPoint(long pos)
{
    SelectNone();

    m_caretPosition = pos - 1;
}

void wxRichTextCtrl::SetInsertionPointEnd()
{
    long pos = GetLastPosition();
    SetInsertionPoint(pos);
}

long wxRichTextCtrl::GetInsertionPoint() const
{
    return m_caretPosition+1;
}

wxTextPos wxRichTextCtrl::GetLastPosition() const
{
    return GetBuffer().GetRange().GetEnd();
}

// If the return values from and to are the same, there is no
// selection.
void wxRichTextCtrl::GetSelection(long* from, long* to) const
{
    *from = m_selectionRange.GetStart();
    *to = m_selectionRange.GetEnd();
    if ((*to) != -1 && (*to) != -2)
        (*to) ++;
}

bool wxRichTextCtrl::IsEditable() const
{
    return m_editable;
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

void wxRichTextCtrl::SetSelection(long from, long to)
{
    // if from and to are both -1, it means (in wxWidgets) that all text should
    // be selected.
    if ( (from == -1) && (to == -1) )
    {
        from = 0;
        to = GetLastPosition()+1;
    }

    DoSetSelection(from, to);
}

void wxRichTextCtrl::DoSetSelection(long from, long to, bool WXUNUSED(scrollCaret))
{
    m_selectionAnchor = from;
    m_selectionRange.SetRange(from, to-1);
    Refresh(false);
    PositionCaret();
}

// ----------------------------------------------------------------------------
// Editing
// ----------------------------------------------------------------------------

void wxRichTextCtrl::Replace(long WXUNUSED(from), long WXUNUSED(to), const wxString& value)
{
    BeginBatchUndo(_("Replace"));

    DeleteSelectedContent();

    DoWriteText(value, true /* selection only */);

    EndBatchUndo();
}

void wxRichTextCtrl::Remove(long from, long to)
{
    SelectNone();

    GetBuffer().DeleteRangeWithUndo(wxRichTextRange(from, to),
        m_caretPosition,       // Current caret position
        from,                           // New caret position
        this);

    LayoutContent();
    if (!IsFrozen())
        Refresh(false);
}

bool wxRichTextCtrl::IsModified() const
{
    return m_buffer.IsModified();
}

void wxRichTextCtrl::MarkDirty()
{
    m_buffer.Modify(true);
}

void wxRichTextCtrl::DiscardEdits()
{
    m_caretPositionForDefaultStyle = -2;
    m_buffer.Modify(false);
    m_buffer.GetCommandProcessor()->ClearCommands();
}

int wxRichTextCtrl::GetNumberOfLines() const
{
    return GetBuffer().GetParagraphCount();
}

// ----------------------------------------------------------------------------
// Positions <-> coords
// ----------------------------------------------------------------------------

long wxRichTextCtrl::XYToPosition(long x, long y) const
{
    return GetBuffer().XYToPosition(x, y);
}

bool wxRichTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    return GetBuffer().PositionToXY(pos, x, y);
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

void wxRichTextCtrl::ShowPosition(long pos)
{
    if (!IsPositionVisible(pos))
        ScrollIntoView(pos-1, WXK_DOWN);
}

int wxRichTextCtrl::GetLineLength(long lineNo) const
{
    return GetBuffer().GetParagraphLength(lineNo);
}

wxString wxRichTextCtrl::GetLineText(long lineNo) const
{
    return GetBuffer().GetParagraphText(lineNo);
}

// ----------------------------------------------------------------------------
// Undo/redo
// ----------------------------------------------------------------------------

void wxRichTextCtrl::Undo()
{
    if (CanUndo())
    {
        GetCommandProcessor()->Undo();
    }
}

void wxRichTextCtrl::Redo()
{
    if (CanRedo())
    {
        GetCommandProcessor()->Redo();
    }
}

bool wxRichTextCtrl::CanUndo() const
{
    return GetCommandProcessor()->CanUndo();
}

bool wxRichTextCtrl::CanRedo() const
{
    return GetCommandProcessor()->CanRedo();
}

// ----------------------------------------------------------------------------
// implementation details
// ----------------------------------------------------------------------------

void wxRichTextCtrl::Command(wxCommandEvent& event)
{
    SetValue(event.GetString());
    GetEventHandler()->ProcessEvent(event);
}

void wxRichTextCtrl::OnDropFiles(wxDropFilesEvent& event)
{
    // By default, load the first file into the text window.
    if (event.GetNumberOfFiles() > 0)
    {
        LoadFile(event.GetFiles()[0]);
    }
}

// ----------------------------------------------------------------------------
// text control event processing
// ----------------------------------------------------------------------------

bool wxRichTextCtrl::SendUpdateEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, GetId());
    InitCommandEvent(event);

    return GetEventHandler()->ProcessEvent(event);
}

void wxRichTextCtrl::InitCommandEvent(wxCommandEvent& event) const
{
    event.SetEventObject((wxControlBase *)this);    // const_cast

    switch ( m_clientDataType )
    {
        case wxClientData_Void:
            event.SetClientData(GetClientData());
            break;

        case wxClientData_Object:
            event.SetClientObject(GetClientObject());
            break;

        case wxClientData_None:
            // nothing to do
            ;
    }
}


wxSize wxRichTextCtrl::DoGetBestSize() const
{
    return wxSize(10, 10);
}

// ----------------------------------------------------------------------------
// standard handlers for standard edit menu events
// ----------------------------------------------------------------------------

void wxRichTextCtrl::OnCut(wxCommandEvent& WXUNUSED(event))
{
    Cut();
}

void wxRichTextCtrl::OnClear(wxCommandEvent& WXUNUSED(event))
{
    DeleteSelection();
}

void wxRichTextCtrl::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    Copy();
}

void wxRichTextCtrl::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    Paste();
}

void wxRichTextCtrl::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    Undo();
}

void wxRichTextCtrl::OnRedo(wxCommandEvent& WXUNUSED(event))
{
    Redo();
}

void wxRichTextCtrl::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxRichTextCtrl::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxRichTextCtrl::OnUpdateClear(wxUpdateUIEvent& event)
{
    event.Enable( CanDeleteSelection() );
}

void wxRichTextCtrl::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxRichTextCtrl::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
    event.SetText( GetCommandProcessor()->GetUndoMenuLabel() );
}

void wxRichTextCtrl::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
    event.SetText( GetCommandProcessor()->GetRedoMenuLabel() );
}

void wxRichTextCtrl::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
    SelectAll();
}

void wxRichTextCtrl::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
    event.Enable(GetLastPosition() > 0);
}

void wxRichTextCtrl::OnContextMenu(wxContextMenuEvent& WXUNUSED(event))
{
    if (!m_contextMenu)
    {
        m_contextMenu = new wxMenu;
        m_contextMenu->Append(wxID_UNDO, _("&Undo"));
        m_contextMenu->Append(wxID_REDO, _("&Redo"));
        m_contextMenu->AppendSeparator();
        m_contextMenu->Append(wxID_CUT, _("Cu&t"));
        m_contextMenu->Append(wxID_COPY, _("&Copy"));
        m_contextMenu->Append(wxID_PASTE, _("&Paste"));
        m_contextMenu->Append(wxID_CLEAR, _("&Delete"));
        m_contextMenu->AppendSeparator();
        m_contextMenu->Append(wxID_SELECTALL, _("Select &All"));
    }
    PopupMenu(m_contextMenu);
    return;
}

bool wxRichTextCtrl::SetStyle(long start, long end, const wxTextAttrEx& style)
{
    return GetBuffer().SetStyle(wxRichTextRange(start, end-1), style);
}

bool wxRichTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    return GetBuffer().SetStyle(wxRichTextRange(start, end-1), wxTextAttrEx(style));
}

bool wxRichTextCtrl::SetStyle(const wxRichTextRange& range, const wxRichTextAttr& style)
{
    return GetBuffer().SetStyle(range.ToInternal(), style);
}

bool wxRichTextCtrl::SetDefaultStyle(const wxTextAttrEx& style)
{
    return GetBuffer().SetDefaultStyle(style);
}

bool wxRichTextCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    return GetBuffer().SetDefaultStyle(wxTextAttrEx(style));
}

const wxTextAttrEx& wxRichTextCtrl::GetDefaultStyleEx() const
{
    return GetBuffer().GetDefaultStyle();
}

const wxTextAttr& wxRichTextCtrl::GetDefaultStyle() const
{
    return GetBuffer().GetDefaultStyle();
}

bool wxRichTextCtrl::GetStyle(long position, wxTextAttr& style)
{
    wxTextAttrEx attr;
    if (GetBuffer().GetStyle(position, attr))
    {
        style = attr;
        return true;
    }
    else
        return false;
}

bool wxRichTextCtrl::GetStyle(long position, wxTextAttrEx& style)
{
    return GetBuffer().GetStyle(position, style);
}

bool wxRichTextCtrl::GetStyle(long position, wxRichTextAttr& style)
{
    return GetBuffer().GetStyle(position, style);
}

/// Set font, and also the buffer attributes
bool wxRichTextCtrl::SetFont(const wxFont& font)
{
#if wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE
    wxControl::SetFont(font);
#else
    wxScrolledWindow::SetFont(font);
#endif

    wxTextAttrEx attr = GetBuffer().GetAttributes();
    attr.SetFont(font);
    GetBuffer().SetBasicStyle(attr);
    GetBuffer().SetDefaultStyle(attr);

    return true;
}

/// Transform logical to physical
wxPoint wxRichTextCtrl::GetPhysicalPoint(const wxPoint& ptLogical) const
{
    wxPoint pt;
    CalcScrolledPosition(ptLogical.x, ptLogical.y, & pt.x, & pt.y);

    return pt;
}

/// Transform physical to logical
wxPoint wxRichTextCtrl::GetLogicalPoint(const wxPoint& ptPhysical) const
{
    wxPoint pt;
    CalcUnscrolledPosition(ptPhysical.x, ptPhysical.y, & pt.x, & pt.y);

    return pt;
}

/// Position the caret
void wxRichTextCtrl::PositionCaret()
{
    if (!GetCaret())
        return;

    //wxLogDebug(wxT("PositionCaret"));

    wxRect caretRect;
    if (GetCaretPositionForIndex(GetCaretPosition(), caretRect))
    {
        wxPoint originalPt = caretRect.GetPosition();
        wxPoint pt = GetPhysicalPoint(originalPt);
        if (GetCaret()->GetPosition() != pt)
        {
            GetCaret()->Move(pt);
            GetCaret()->SetSize(caretRect.GetSize());
        }
    }
}

/// Get the caret height and position for the given character position
bool wxRichTextCtrl::GetCaretPositionForIndex(long position, wxRect& rect)
{
    wxClientDC dc(this);
    dc.SetFont(GetFont());

    PrepareDC(dc);

    wxPoint pt;
    int height = 0;

    if (GetBuffer().FindPosition(dc, position, pt, & height, m_caretAtLineStart))
    {
        rect = wxRect(pt, wxSize(wxRICHTEXT_DEFAULT_CARET_WIDTH, height));
        return true;
    }

    return false;
}

/// Gets the line for the visible caret position. If the caret is
/// shown at the very end of the line, it means the next character is actually
/// on the following line. So let's get the line we're expecting to find
/// if this is the case.
wxRichTextLine* wxRichTextCtrl::GetVisibleLineForCaretPosition(long caretPosition) const
{
    wxRichTextLine* line = GetBuffer().GetLineAtPosition(caretPosition, true);
    wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(caretPosition, true);
    if (line)
    {
        wxRichTextRange lineRange = line->GetAbsoluteRange();
        if (caretPosition == lineRange.GetStart()-1 &&
            (para->GetRange().GetStart() != lineRange.GetStart()))
        {
            if (!m_caretAtLineStart)
                line = GetBuffer().GetLineAtPosition(caretPosition-1, true);
        }
    }
    return line;
}


/// Move the caret to the given character position
bool wxRichTextCtrl::MoveCaret(long pos, bool showAtLineStart)
{
    if (GetBuffer().GetDirty())
        LayoutContent();

    if (pos <= GetBuffer().GetRange().GetEnd())
    {
        SetCaretPosition(pos, showAtLineStart);

        PositionCaret();

        return true;
    }
    else
        return false;
}

/// Layout the buffer: which we must do before certain operations, such as
/// setting the caret position.
bool wxRichTextCtrl::LayoutContent(bool onlyVisibleRect)
{
    if (GetBuffer().GetDirty() || onlyVisibleRect)
    {
        wxRect availableSpace(GetClientSize());
        if (availableSpace.width == 0)
            availableSpace.width = 10;
        if (availableSpace.height == 0)
            availableSpace.height = 10;

        int flags = wxRICHTEXT_FIXED_WIDTH|wxRICHTEXT_VARIABLE_HEIGHT;
        if (onlyVisibleRect)
        {
            flags |= wxRICHTEXT_LAYOUT_SPECIFIED_RECT;
            availableSpace.SetPosition(GetLogicalPoint(wxPoint(0, 0)));
        }

        wxClientDC dc(this);
        dc.SetFont(GetFont());

        PrepareDC(dc);

        GetBuffer().Defragment();
        GetBuffer().UpdateRanges();     // If items were deleted, ranges need recalculation
        GetBuffer().Layout(dc, availableSpace, flags);
        GetBuffer().SetDirty(false);

        if (!IsFrozen())
            SetupScrollbars();
    }

    return true;
}

/// Is all of the selection bold?
bool wxRichTextCtrl::IsSelectionBold()
{
    if (HasSelection())
    {
        wxRichTextAttr attr;
        wxRichTextRange range = GetInternalSelectionRange();
        attr.SetFlags(wxTEXT_ATTR_FONT_WEIGHT);
        attr.SetFontWeight(wxBOLD);

        return HasCharacterAttributes(range, attr);
    }
    else
    {
        // If no selection, then we need to combine current style with default style
        // to see what the effect would be if we started typing.
        wxRichTextAttr attr;
        attr.SetFlags(wxTEXT_ATTR_FONT_WEIGHT);

        long pos = GetAdjustedCaretPosition(GetCaretPosition());
        if (GetStyle(pos, attr))
        {
            if (IsDefaultStyleShowing())
                wxRichTextApplyStyle(attr, GetDefaultStyleEx());
            return attr.GetFontWeight() == wxBOLD;
        }
    }
    return false;
}

/// Is all of the selection italics?
bool wxRichTextCtrl::IsSelectionItalics()
{
    if (HasSelection())
    {
        wxRichTextRange range = GetInternalSelectionRange();
        wxRichTextAttr attr;
        attr.SetFlags(wxTEXT_ATTR_FONT_ITALIC);
        attr.SetFontStyle(wxITALIC);

        return HasCharacterAttributes(range, attr);
    }
    else
    {
        // If no selection, then we need to combine current style with default style
        // to see what the effect would be if we started typing.
        wxRichTextAttr attr;
        attr.SetFlags(wxTEXT_ATTR_FONT_ITALIC);

        long pos = GetAdjustedCaretPosition(GetCaretPosition());
        if (GetStyle(pos, attr))
        {
            if (IsDefaultStyleShowing())
                wxRichTextApplyStyle(attr, GetDefaultStyleEx());
            return attr.GetFontStyle() == wxITALIC;
        }
    }
    return false;
}

/// Is all of the selection underlined?
bool wxRichTextCtrl::IsSelectionUnderlined()
{
    if (HasSelection())
    {
        wxRichTextRange range = GetInternalSelectionRange();
        wxRichTextAttr attr;
        attr.SetFlags(wxTEXT_ATTR_FONT_UNDERLINE);
        attr.SetFontUnderlined(true);

        return HasCharacterAttributes(range, attr);
    }
    else
    {
        // If no selection, then we need to combine current style with default style
        // to see what the effect would be if we started typing.
        wxRichTextAttr attr;
        attr.SetFlags(wxTEXT_ATTR_FONT_UNDERLINE);

        long pos = GetAdjustedCaretPosition(GetCaretPosition());
        if (GetStyle(pos, attr))
        {
            if (IsDefaultStyleShowing())
                wxRichTextApplyStyle(attr, GetDefaultStyleEx());
            return attr.GetFontUnderlined();
        }
    }
    return false;
}

/// Apply bold to the selection
bool wxRichTextCtrl::ApplyBoldToSelection()
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT_WEIGHT);
    attr.SetFontWeight(IsSelectionBold() ? wxNORMAL : wxBOLD);

    if (HasSelection())
        return SetStyle(GetSelectionRange(), attr);
    else
        SetAndShowDefaultStyle(attr);
    return true;
}

/// Apply italic to the selection
bool wxRichTextCtrl::ApplyItalicToSelection()
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT_ITALIC);
    attr.SetFontStyle(IsSelectionItalics() ? wxNORMAL : wxITALIC);

    if (HasSelection())
        return SetStyle(GetSelectionRange(), attr);
    else
        SetAndShowDefaultStyle(attr);
    return true;
}

/// Apply underline to the selection
bool wxRichTextCtrl::ApplyUnderlineToSelection()
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT_UNDERLINE);
    attr.SetFontUnderlined(!IsSelectionUnderlined());

    if (HasSelection())
        return SetStyle(GetSelectionRange(), attr);
    else
        SetAndShowDefaultStyle(attr);
    return true;
}

/// Is all of the selection aligned according to the specified flag?
bool wxRichTextCtrl::IsSelectionAligned(wxTextAttrAlignment alignment)
{
    if (HasSelection())
    {
        wxRichTextRange range = GetInternalSelectionRange();
        wxRichTextAttr attr;
        attr.SetAlignment(alignment);

        return HasParagraphAttributes(range, attr);
    }
    else
    {
        // If no selection, then we need to get information from the current paragraph.
        wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(GetCaretPosition()+1);
        if (para)
            return para->GetAttributes().GetAlignment() == alignment;
    }
    return false;
}

/// Apply alignment to the selection
bool wxRichTextCtrl::ApplyAlignmentToSelection(wxTextAttrAlignment alignment)
{
    wxRichTextAttr attr;
    attr.SetAlignment(alignment);
    if (HasSelection())
        return SetStyle(GetSelectionRange(), attr);
    else
    {
        wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(GetCaretPosition()+1);
        if (para)
            return SetStyle(para->GetRange().FromInternal(), attr);
    }
    return true;
}

/// Apply a named style to the selection
void wxRichTextCtrl::ApplyStyle(wxRichTextStyleDefinition* def)
{
    // Flags are defined within each definition, so only certain
    // attributes are applied.
    wxRichTextAttr attr(def->GetStyle());

    // Make sure the attr has the style name
    if (def->IsKindOf(CLASSINFO(wxRichTextParagraphStyleDefinition)))
        attr.SetParagraphStyleName(def->GetName());
    else
        attr.SetCharacterStyleName(def->GetName());

    if (HasSelection())
        SetStyle(GetSelectionRange(), attr);
    else
        SetAndShowDefaultStyle(attr);
}

/// Sets the default style to the style under the cursor
bool wxRichTextCtrl::SetDefaultStyleToCursorStyle()
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_CHARACTER);

    // If at the start of a paragraph, use the next position.
    long pos = GetAdjustedCaretPosition(GetCaretPosition());

    if (GetStyle(pos, attr))
    {
        SetDefaultStyle(attr);
        return true;
    }

    return false;
}

/// Returns the first visible position in the current view
long wxRichTextCtrl::GetFirstVisiblePosition() const
{
    wxRichTextLine* line = GetBuffer().GetLineAtYPosition(GetLogicalPoint(wxPoint(0, 0)).y);
    if (line)
        return line->GetAbsoluteRange().GetStart();
    else
        return 0;
}

/// The adjusted caret position is the character position adjusted to take
/// into account whether we're at the start of a paragraph, in which case
/// style information should be taken from the next position, not current one.
long wxRichTextCtrl::GetAdjustedCaretPosition(long caretPos) const
{
    wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(caretPos+1);

    if (para && (caretPos+1 == para->GetRange().GetStart()))
        caretPos ++;
    return caretPos;
}

/// Get/set the selection range in character positions. -1, -1 means no selection.
/// The range is in API convention, i.e. a single character selection is denoted
/// by (n, n+1)
wxRichTextRange wxRichTextCtrl::GetSelectionRange() const
{
    wxRichTextRange range = GetInternalSelectionRange();
    if (range != wxRichTextRange(-2,-2) && range != wxRichTextRange(-1,-1))
        range.SetEnd(range.GetEnd() + 1);
    return range;
}

void wxRichTextCtrl::SetSelectionRange(const wxRichTextRange& range)
{
    wxRichTextRange range1(range);
    if (range1 != wxRichTextRange(-2,-2) && range1 != wxRichTextRange(-1,-1) )
        range1.SetEnd(range1.GetEnd() - 1);

    wxASSERT( range1.GetStart() > range1.GetEnd() );
    
    SetInternalSelectionRange(range1);
}

#endif
    // wxUSE_RICHTEXT
