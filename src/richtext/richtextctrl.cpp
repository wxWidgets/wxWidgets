/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextctrl.cpp
// Purpose:     A rich edit control
// Author:      Julian Smart
// Created:     2005-09-30
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_RICHTEXT

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/settings.h"
#endif

#include "wx/timer.h"
#include "wx/textfile.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx/dcbuffer.h"
#include "wx/arrimpl.cpp"
#include "wx/fontenum.h"
#include "wx/accel.h"

#if defined (__WXGTK__) || defined(__WXX11__)
#define wxHAVE_PRIMARY_SELECTION 1
#else
#define wxHAVE_PRIMARY_SELECTION 0
#endif

#if wxUSE_CLIPBOARD && wxHAVE_PRIMARY_SELECTION
#include "wx/clipbrd.h"
#endif

// DLL options compatibility check:
#include "wx/app.h"
WX_CHECK_BUILD_OPTIONS("wxRichTextCtrl")

wxDEFINE_EVENT( wxEVT_RICHTEXT_LEFT_CLICK, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_MIDDLE_CLICK, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_RIGHT_CLICK, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_LEFT_DCLICK, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_RETURN, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_CHARACTER, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_CONSUMING_CHARACTER, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_DELETE, wxRichTextEvent );

wxDEFINE_EVENT( wxEVT_RICHTEXT_STYLESHEET_REPLACING, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_STYLESHEET_REPLACED, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_STYLESHEET_CHANGING, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_STYLESHEET_CHANGED, wxRichTextEvent );

wxDEFINE_EVENT( wxEVT_RICHTEXT_CONTENT_INSERTED, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_CONTENT_DELETED, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_STYLE_CHANGED, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_PROPERTIES_CHANGED, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_SELECTION_CHANGED, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_BUFFER_RESET, wxRichTextEvent );
wxDEFINE_EVENT( wxEVT_RICHTEXT_FOCUS_OBJECT_CHANGED, wxRichTextEvent );

#if wxRICHTEXT_USE_OWN_CARET

/*!
 * wxRichTextCaret
 *
 * This implements a non-flashing cursor in case there
 * are platform-specific problems with the generic caret.
 * wxRICHTEXT_USE_OWN_CARET is set in richtextbuffer.h.
 */

class wxRichTextCaret;
class wxRichTextCaretTimer: public wxTimer
{
  public:
    wxRichTextCaretTimer(wxRichTextCaret* caret)
    {
        m_caret = caret;
    }
    virtual void Notify() override;
    wxRichTextCaret* m_caret;
};

class wxRichTextCaret: public wxCaret
{
public:
    // ctors
    // -----
        // default - use Create()
    wxRichTextCaret(): m_timer(this)  { Init(); }
        // creates a block caret associated with the given window
    wxRichTextCaret(wxRichTextCtrl *window, int width, int height)
        : wxCaret(window, width, height), m_timer(this) { Init(); m_richTextCtrl = window; }
    wxRichTextCaret(wxRichTextCtrl *window, const wxSize& size)
        : wxCaret(window, size), m_timer(this) { Init(); m_richTextCtrl = window; }

    virtual ~wxRichTextCaret();

    // implementation
    // --------------

    // called by wxWindow (not using the event tables)
    virtual void OnSetFocus() override;
    virtual void OnKillFocus() override;

    // draw the caret on the given DC
    void DoDraw(wxDC *dc);

    // get the visible count
    int GetVisibleCount() const { return m_countVisible; }

    // delay repositioning
    bool GetNeedsUpdate() const { return m_needsUpdate; }
    void SetNeedsUpdate(bool needsUpdate = true ) { m_needsUpdate = needsUpdate; }

    void Notify();

    bool GetRefreshEnabled() const { return m_refreshEnabled; }
    void EnableRefresh(bool b) { m_refreshEnabled = b; }

protected:
    virtual void DoShow() override;
    virtual void DoHide() override;
    virtual void DoMove() override;
    virtual void DoSize() override;

    // refresh the caret
    void Refresh();

private:
    void Init();

    int           m_xOld,
                  m_yOld;
    bool          m_hasFocus;       // true => our window has focus
    bool          m_needsUpdate;    // must be repositioned
    bool          m_flashOn;
    wxRichTextCaretTimer m_timer;
    wxRichTextCtrl* m_richTextCtrl;
    bool          m_refreshEnabled;
    wxPen         m_caretPen;
    wxBrush       m_caretBrush;
};
#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextCtrl, wxControl);

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextEvent, wxNotifyEvent);

wxBEGIN_EVENT_TABLE(wxRichTextCtrl, wxControl)
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
    EVT_KEY_DOWN(wxRichTextCtrl::OnChar)
    EVT_SIZE(wxRichTextCtrl::OnSize)
    EVT_SET_FOCUS(wxRichTextCtrl::OnSetFocus)
    EVT_KILL_FOCUS(wxRichTextCtrl::OnKillFocus)
    EVT_MOUSE_CAPTURE_LOST(wxRichTextCtrl::OnCaptureLost)
    EVT_CONTEXT_MENU(wxRichTextCtrl::OnContextMenu)
    EVT_SYS_COLOUR_CHANGED(wxRichTextCtrl::OnSysColourChanged)
    EVT_TIMER(wxID_ANY, wxRichTextCtrl::OnTimer)

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

    EVT_MENU(wxID_RICHTEXT_PROPERTIES1, wxRichTextCtrl::OnProperties)
    EVT_UPDATE_UI(wxID_RICHTEXT_PROPERTIES1, wxRichTextCtrl::OnUpdateProperties)

    EVT_MENU(wxID_RICHTEXT_PROPERTIES2, wxRichTextCtrl::OnProperties)
    EVT_UPDATE_UI(wxID_RICHTEXT_PROPERTIES2, wxRichTextCtrl::OnUpdateProperties)

    EVT_MENU(wxID_RICHTEXT_PROPERTIES3, wxRichTextCtrl::OnProperties)
    EVT_UPDATE_UI(wxID_RICHTEXT_PROPERTIES3, wxRichTextCtrl::OnUpdateProperties)

wxEND_EVENT_TABLE()

/*!
 * wxRichTextCtrl
 */

wxArrayString wxRichTextCtrl::sm_availableFontNames;

wxRichTextCtrl::wxRichTextCtrl()
              : wxScrollHelper(this)
{
    Init();
}

wxRichTextCtrl::wxRichTextCtrl(wxWindow* parent,
                               wxWindowID id,
                               const wxString& value,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxValidator& validator,
                               const wxString& name)
              : wxScrollHelper(this)
{
    Init();
    Create(parent, id, value, pos, size, style, validator, name);
}

/// Creation
bool wxRichTextCtrl::Create( wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style,
                             const wxValidator& validator, const wxString& name)
{
    style |= wxVSCROLL;

    // If read-only, the programmer probably wants to retain dialog keyboard navigation.
    // If you don't, then pass wxWANTS_CHARS explicitly.
    if ((style & wxTE_READONLY) == 0)
        style |= wxWANTS_CHARS;

    if (!wxControl::Create(parent, id, pos, size,
                           style|wxFULL_REPAINT_ON_RESIZE,
                           validator, name))
        return false;

    if (!GetFont().IsOk())
    {
        SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    }

    // No physical scrolling, so we can preserve margins
    EnableScrolling(false, false);

    if (style & wxTE_READONLY)
        SetEditable(false);

    // The base attributes must all have default values
    wxRichTextAttr attributes;
    attributes.SetFont(GetFont());
    attributes.SetTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    attributes.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
    attributes.SetLineSpacing(10);
    attributes.SetParagraphSpacingAfter(10);
    attributes.SetParagraphSpacingBefore(0);
    SetBasicStyle(attributes);

    int margin = 5;
    SetMargins(margin, margin);

    // The default attributes will be merged with base attributes, so
    // can be empty to begin with
    wxRichTextAttr defaultAttributes;
    SetDefaultStyle(defaultAttributes);

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    GetBuffer().Reset();
    GetBuffer().SetRichTextCtrl(this);

#if wxRICHTEXT_USE_OWN_CARET
    SetCaret(new wxRichTextCaret(this, wxRICHTEXT_DEFAULT_CARET_WIDTH, 16));
#else
    SetCaret(new wxCaret(this, wxRICHTEXT_DEFAULT_CARET_WIDTH, 16));
#endif

    // Tell the sizers to use the given or best size
    SetInitialSize(size);

#if wxRICHTEXT_BUFFERED_PAINTING
    // Create a buffer
    RecreateBuffer(size);
#endif

    m_textCursor = wxCursor(wxCURSOR_IBEAM);
    m_urlCursor = wxCursor(wxCURSOR_HAND);

    SetCursor(m_textCursor);

    if (!value.IsEmpty())
        SetValue(value);

    GetBuffer().AddEventHandler(this);

#if wxUSE_ACCEL
    // Accelerators
    wxAcceleratorEntry entries[6];

    entries[0].Set(wxACCEL_CTRL,   (int) 'C',       wxID_COPY);
    entries[1].Set(wxACCEL_CTRL,   (int) 'X',       wxID_CUT);
    entries[2].Set(wxACCEL_CTRL,   (int) 'V',       wxID_PASTE);
    entries[3].Set(wxACCEL_CTRL,   (int) 'A',       wxID_SELECTALL);
    entries[4].Set(wxACCEL_CTRL,   (int) 'Z',       wxID_UNDO);
    entries[5].Set(wxACCEL_CTRL,   (int) 'Y',       wxID_REDO);

    wxAcceleratorTable accel(6, entries);
    SetAcceleratorTable(accel);
#endif // wxUSE_ACCEL

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
    m_contextMenu->AppendSeparator();
    m_contextMenu->Append(wxID_RICHTEXT_PROPERTIES1, _("&Properties"));

#if wxUSE_DRAG_AND_DROP
    SetDropTarget(new wxRichTextDropTarget(this));
#endif
    SetModified( false );
    return true;
}

wxRichTextCtrl::~wxRichTextCtrl()
{
    SetFocusObject(& GetBuffer(), false);
    GetBuffer().RemoveEventHandler(this);

    delete m_contextMenu;

    m_delayedImageProcessingTimer.Stop();
}

/// Member initialisation
void wxRichTextCtrl::Init()
{
    m_contextMenu = nullptr;
    m_caret = nullptr;
    m_caretPosition = -1;
    m_selectionAnchor = -2;
    m_selectionAnchorObject = nullptr;
    m_selectionState = wxRichTextCtrlSelectionState_Normal;
    m_editable = true;
    m_useVirtualAttributes = false;
    m_verticalScrollbarEnabled = true;
    m_caretAtLineStart = false;
    m_dragging = false;
#if wxUSE_DRAG_AND_DROP
    m_preDrag = false;
#endif
    m_fullLayoutRequired = false;
    m_fullLayoutTime = 0;
    m_fullLayoutSavedPosition = 0;
    m_delayedLayoutThreshold = wxRICHTEXT_DEFAULT_DELAYED_LAYOUT_THRESHOLD;
    m_caretPositionForDefaultStyle = -2;
    m_focusObject = & m_buffer;
    m_scale = 1.0;

    // Scrollbar hysteresis detection
    m_setupScrollbarsCount = 0;
    m_setupScrollbarsCountInOnSize = 0;

    m_enableImages = true;

    m_enableDelayedImageLoading = false;
    m_delayedImageProcessingRequired = false;
    m_delayedImageProcessingTime = 0;

    // Line height in pixels
    m_lineHeight = 5;
}

void wxRichTextCtrl::DoThaw()
{
    if (GetBuffer().IsDirty())
        LayoutContent();
    else
        SetupScrollbars();

    wxWindow::DoThaw();
}

/// Clear all text
void wxRichTextCtrl::Clear()
{
    if (GetFocusObject() == & GetBuffer())
    {
        m_buffer.ResetAndClearCommands();
        m_buffer.Invalidate(wxRICHTEXT_ALL);
    }
    else
    {
        GetFocusObject()->Reset();
    }

    m_caretPosition = -1;
    m_caretPositionForDefaultStyle = -2;
    m_caretAtLineStart = false;
    m_selection.Reset();
    m_selectionState = wxRichTextCtrlSelectionState_Normal;

    Scroll(0,0);

    if (!IsFrozen())
    {
        LayoutContent();
        Refresh(false);
    }

    wxTextCtrl::SendTextUpdatedEvent(this);
}

/// Painting
void wxRichTextCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#if !wxRICHTEXT_USE_OWN_CARET
    if (GetCaret() && !IsFrozen())
        GetCaret()->Hide();
#else
    // Stop the caret refreshing the control from within the
    // paint handler
    if (GetCaret())
        ((wxRichTextCaret*) GetCaret())->EnableRefresh(false);
#endif

    {
#if wxRICHTEXT_BUFFERED_PAINTING
        wxBufferedPaintDC dc(this, m_bufferBitmap);
#else
        wxPaintDC dc(this);
#endif

        if (IsFrozen())
            return;

        PrepareDC(dc);

        dc.SetFont(GetFont());

        wxRect drawingArea(GetUpdateRegion().GetBox());
        drawingArea.SetPosition(GetUnscaledPoint(GetLogicalPoint(drawingArea.GetPosition())));
        drawingArea.SetSize(GetUnscaledSize(drawingArea.GetSize()));

        wxRect availableSpace(GetUnscaledSize(GetClientSize()));
        wxRichTextDrawingContext context(& GetBuffer());
        if (GetBuffer().IsDirty())
        {
            dc.SetUserScale(GetScale(), GetScale());

            GetBuffer().Defragment(context);
            GetBuffer().UpdateRanges();     // If items were deleted, ranges need recalculation

            DoLayoutBuffer(GetBuffer(), dc, context, availableSpace, availableSpace, wxRICHTEXT_FIXED_WIDTH|wxRICHTEXT_VARIABLE_HEIGHT);

            GetBuffer().Invalidate(wxRICHTEXT_NONE);

            dc.SetUserScale(1.0, 1.0);

            SetupScrollbars(false, true /* from OnPaint */);
        }

        // Paint the background
        PaintBackground(dc);

        wxRect clipRect(availableSpace);
        clipRect.x += GetBuffer().GetLeftMargin();
        clipRect.y += GetBuffer().GetTopMargin();
        clipRect.width -= (GetBuffer().GetLeftMargin() + GetBuffer().GetRightMargin());
        clipRect.height -= (GetBuffer().GetTopMargin() + GetBuffer().GetBottomMargin());

        clipRect = GetScaledRect(clipRect);
        clipRect.SetPosition(GetLogicalPoint(clipRect.GetPosition()));

        dc.SetClippingRegion(clipRect);

        int flags = 0;
        if ((GetExtraStyle() & wxRICHTEXT_EX_NO_GUIDELINES) == 0)
            flags |= wxRICHTEXT_DRAW_GUIDELINES;

        dc.SetUserScale(GetScale(), GetScale());

        GetBuffer().Draw(dc, context, GetBuffer().GetOwnRange(), GetSelection(), drawingArea, 0 /* descent */, flags);

        dc.DestroyClippingRegion();

        // Other user defined painting after everything else (i.e. all text) is painted
        PaintAboveContent(dc);

#if wxRICHTEXT_USE_OWN_CARET
        if (GetCaret()->IsVisible())
        {
            PositionCaret();
            ((wxRichTextCaret*) GetCaret())->DoDraw(& dc);
        }
#endif

        dc.SetUserScale(1.0, 1.0);
    }

#if !wxRICHTEXT_USE_OWN_CARET
    if (GetCaret())
        GetCaret()->Show();
    PositionCaret();
#else
    if (GetCaret())
        ((wxRichTextCaret*) GetCaret())->EnableRefresh(true);
#endif
}

// Empty implementation, to prevent flicker
void wxRichTextCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxRichTextCtrl::OnSetFocus(wxFocusEvent& WXUNUSED(event))
{
    if (GetCaret())
    {
#if !wxRICHTEXT_USE_OWN_CARET
        PositionCaret();
#endif
        if (!GetCaret()->IsVisible())
            GetCaret()->Show();
    }

#if defined(__WXGTK__) && !wxRICHTEXT_USE_OWN_CARET
    // Work around dropouts when control is focused
    if (!IsFrozen())
    {
        Refresh(false);
    }
#endif
}

void wxRichTextCtrl::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    if (GetCaret() && GetCaret()->IsVisible())
        GetCaret()->Hide();

#if defined(__WXGTK__) && !wxRICHTEXT_USE_OWN_CARET
    // Work around dropouts when control is focused
    if (!IsFrozen())
    {
        Refresh(false);
    }
#endif
}

void wxRichTextCtrl::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    m_dragging = false;
}

// Set up the caret for the given position and container, after a mouse click
bool wxRichTextCtrl::SetCaretPositionAfterClick(wxRichTextParagraphLayoutBox* container, long position, int hitTestFlags, bool extendSelection)
{
    bool caretAtLineStart = false;

    if (hitTestFlags & wxRICHTEXT_HITTEST_BEFORE)
    {
        // If we're at the start of a line (but not first in para)
        // then we should keep the caret showing at the start of the line
        // by showing the m_caretAtLineStart flag.
        wxRichTextParagraph* para = container->GetParagraphAtPosition(position);
        wxRichTextLine* line = container->GetLineAtPosition(position);

        if (line && para && line->GetAbsoluteRange().GetStart() == position && para->GetRange().GetStart() != position)
            caretAtLineStart = true;
        position --;
    }

    if (extendSelection && (m_caretPosition != position))
        ExtendSelection(m_caretPosition, position, wxRICHTEXT_SHIFT_DOWN);

    MoveCaret(position, caretAtLineStart);
    SetDefaultStyleToCursorStyle();

    return true;
}

/// Left-click
void wxRichTextCtrl::OnLeftClick(wxMouseEvent& event)
{
    SetFocus();

    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(GetFont());

    // TODO: detect change of focus object
    long position = 0;
    wxRichTextObject* hitObj = nullptr;
    wxRichTextObject* contextObj = nullptr;
    wxRichTextDrawingContext context(& GetBuffer());
    int hit = GetBuffer().HitTest(dc, context, GetUnscaledPoint(event.GetLogicalPosition(dc)), position, & hitObj, & contextObj, wxRICHTEXT_HITTEST_HONOUR_ATOMIC);

#if wxUSE_DRAG_AND_DROP
    // If there's no selection, or we're not inside it, this isn't an attempt to initiate Drag'n'Drop
    if (IsEditable() && HasSelection() && GetSelectionRange().ToInternal().Contains(position))
    {
        // This might be an attempt at initiating Drag'n'Drop. So set the time & flags
        m_preDrag = true;
        m_dragStartPoint = event.GetPosition();   // No need to worry about logical positions etc, we only care about the distance from the original pt

#if wxUSE_DATETIME
        m_dragStartTime = wxDateTime::UNow();
#endif // wxUSE_DATETIME

        // Preserve behaviour of clicking on an object within the selection
        if (hit != wxRICHTEXT_HITTEST_NONE && hitObj)
            m_dragging = true;

        return; // Don't skip the event, else the selection will be lost
    }
#endif // wxUSE_DRAG_AND_DROP

    if (hit != wxRICHTEXT_HITTEST_NONE && hitObj)
    {
        wxRichTextParagraphLayoutBox* oldFocusObject = GetFocusObject();
        wxRichTextParagraphLayoutBox* container = wxDynamicCast(contextObj, wxRichTextParagraphLayoutBox);
        bool needsCaretSet = false;
        if (container && container != GetFocusObject() && container->AcceptsFocus())
        {
            SetFocusObject(container, false /* don't set caret position yet */);
            needsCaretSet = true;
        }

        m_dragging = true;
        CaptureMouse();

        // Don't change the caret position if we clicked on a floating objects such as an image,
        // unless we changed the focus object.
        if (wxRichTextBuffer::GetFloatingLayoutMode() && hitObj && hitObj->IsFloating() && !hitObj->AcceptsFocus())
        {
            if (needsCaretSet)
                SetInsertionPoint(0);
        }
        else
        {
            long oldCaretPos = m_caretPosition;

            SetCaretPositionAfterClick(container, position, hit);

            // For now, don't handle shift-click when we're selecting multiple objects.
            if (event.ShiftDown() && GetFocusObject() == oldFocusObject && m_selectionState == wxRichTextCtrlSelectionState_Normal)
                ExtendSelection(oldCaretPos, m_caretPosition, wxRICHTEXT_SHIFT_DOWN);
            else
                SelectNone();
        }
    }

    event.Skip();
}

/// Left-up
void wxRichTextCtrl::OnLeftUp(wxMouseEvent& event)
{
    if (m_dragging)
    {
        m_dragging = false;
        if (GetCapture() == this)
            ReleaseMouse();

        // See if we clicked on a URL
        wxClientDC dc(this);
        PrepareDC(dc);
        dc.SetFont(GetFont());

        long position = 0;
        wxPoint logicalPt = event.GetLogicalPosition(dc);
        wxRichTextObject* hitObj = nullptr;
        wxRichTextObject* contextObj = nullptr;
        wxRichTextDrawingContext context(& GetBuffer());
        // Only get objects at this level, not nested, because otherwise we couldn't swipe text at a single level.
        int hit = GetFocusObject()->HitTest(dc, context, GetUnscaledPoint(logicalPt), position, & hitObj, & contextObj, wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS|wxRICHTEXT_HITTEST_HONOUR_ATOMIC);

#if wxUSE_DRAG_AND_DROP
        if (m_preDrag)
        {
            // Preserve the behaviour that would have happened without drag-and-drop detection, in OnLeftClick
            m_preDrag = false; // Tell DnD not to happen now: we are processing Left Up ourselves.

            // Do the actions that would have been done in OnLeftClick if we hadn't tried to drag
            long prePosition = 0;
            wxRichTextObject* preHitObj = nullptr;
            wxRichTextObject* preContextObj = nullptr;
            int preHit = GetBuffer().HitTest(dc, context, GetUnscaledPoint(event.GetLogicalPosition(dc)), prePosition, & preHitObj, & preContextObj, wxRICHTEXT_HITTEST_HONOUR_ATOMIC);
            wxRichTextParagraphLayoutBox* oldFocusObject = GetFocusObject();
            wxRichTextParagraphLayoutBox* container = wxDynamicCast(preContextObj, wxRichTextParagraphLayoutBox);
            bool needsCaretSet = false;
            if (container && container != GetFocusObject() && container->AcceptsFocus())
            {
                SetFocusObject(container, false /* don't set caret position yet */);
                needsCaretSet = true;
            }

            if (wxRichTextBuffer::GetFloatingLayoutMode() && preHitObj && preHitObj->IsFloating() && !preHitObj->AcceptsFocus())
            {
                if (needsCaretSet)
                    SetInsertionPoint(0);
            }
            else
            {
                long oldCaretPos = m_caretPosition;

                SetCaretPositionAfterClick(container, prePosition, preHit);

                // For now, don't handle shift-click when we're selecting multiple objects.
                if (event.ShiftDown() && GetFocusObject() == oldFocusObject && m_selectionState == wxRichTextCtrlSelectionState_Normal)
                    ExtendSelection(oldCaretPos, m_caretPosition, wxRICHTEXT_SHIFT_DOWN);
                else
                    SelectNone();
            }
        }
#endif

        // Don't process left click if there was a selection, which implies that a selection may just have been
        // extended
        if ((hit != wxRICHTEXT_HITTEST_NONE) && !(hit & wxRICHTEXT_HITTEST_OUTSIDE) && !HasSelection())
        {
            wxRichTextEvent cmdEvent(
                wxEVT_RICHTEXT_LEFT_CLICK,
                GetId());
            cmdEvent.SetEventObject(this);
            cmdEvent.SetPosition(position);
            if (hitObj)
                cmdEvent.SetContainer(hitObj->GetContainer());

            if (!GetEventHandler()->ProcessEvent(cmdEvent))
            {
                wxRichTextAttr attr;
                if (GetStyle(position, attr))
                {
                    if (attr.HasFlag(wxTEXT_ATTR_URL))
                    {
                        wxString urlTarget = attr.GetURL();
                        if (!urlTarget.IsEmpty())
                        {
                            wxMouseEvent mouseEvent(event);

                            long startPos = 0, endPos = 0;
                            wxRichTextObject* obj = GetFocusObject()->GetLeafObjectAtPosition(position);
                            if (obj)
                            {
                                startPos = obj->GetRange().GetStart();
                                endPos = obj->GetRange().GetEnd();
                            }

                            wxTextUrlEvent urlEvent(GetId(), mouseEvent, startPos, endPos);
                            InitCommandEvent(urlEvent);

                            urlEvent.SetString(urlTarget);

                            GetEventHandler()->ProcessEvent(urlEvent);
                        }
                    }
                }
            }
        }
    }

#if wxUSE_DRAG_AND_DROP
    m_preDrag = false;
#endif // wxUSE_DRAG_AND_DROP

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ && wxHAVE_PRIMARY_SELECTION
    if (HasSelection() && GetFocusObject() && GetFocusObject()->GetBuffer())
    {
        // Put the selection in PRIMARY, if it exists
        wxTheClipboard->UsePrimarySelection(true);

        wxRichTextRange range = GetInternalSelectionRange();
        GetFocusObject()->GetBuffer()->CopyToClipboard(range);

        wxTheClipboard->UsePrimarySelection(false);
    }
#endif
}

/// Mouse-movements
void wxRichTextCtrl::OnMoveMouse(wxMouseEvent& event)
{
    if (!event.Dragging() && m_dragging)
    {
        // We may have accidentally lost a mouse-up event, especially on Linux
        m_dragging = false;
        if (GetCapture() == this)
            ReleaseMouse();
    }

#if wxUSE_DRAG_AND_DROP
    size_t distance = 0;
    if (m_preDrag || m_dragging)
    {
        int x = m_dragStartPoint.x - event.GetPosition().x;
        int y = m_dragStartPoint.y - event.GetPosition().y;
        distance = abs(x) + abs(y);
    }

    // See if we're starting Drag'n'Drop
    if (m_preDrag)
    {
#if wxUSE_DATETIME
        wxTimeSpan diff = wxDateTime::UNow() - m_dragStartTime;
#endif
        if ((distance > 10)
#if wxUSE_DATETIME
             && (diff.GetMilliseconds() > 100)
#endif
           )
        {
            m_dragging = false;

            // Start drag'n'drop
            wxRichTextRange range = GetInternalSelectionRange();
            if (range == wxRICHTEXT_NONE)
            {
              // Don't try to drag an empty range
              m_preDrag = false;
              return;
            }

            // Cache the current situation, to be restored if Drag'n'Drop is cancelled
            long oldPos = GetCaretPosition();
            wxRichTextParagraphLayoutBox* oldFocus = GetFocusObject();

            wxDataObjectComposite* compositeObject = new wxDataObjectComposite();
            wxString text = GetFocusObject()->GetTextForRange(range);
#ifdef __WXMSW__
            text = wxTextFile::Translate(text, wxTextFileType_Dos);
#endif
            compositeObject->Add(new wxTextDataObject(text), false /* not preferred */);

            wxRichTextBuffer* richTextBuf = new wxRichTextBuffer;
            GetFocusObject()->CopyFragment(range, *richTextBuf);
            compositeObject->Add(new wxRichTextBufferDataObject(richTextBuf), true /* preferred */);

            wxRichTextDropSource source(*compositeObject, this);
            // Use wxDrag_DefaultMove, not because it's the likelier choice but because pressing Ctrl for Copy obeys the principle of least surprise
            // The alternative, wxDrag_DefaultCopy, requires the user to know that Move needs the Shift key pressed
            BeginBatchUndo(_("Drag"));
            switch (source.DoDragDrop(wxDrag_AllowMove | wxDrag_DefaultMove))
            {
                case wxDragMove:
                case wxDragCopy:  break;

                case wxDragError:
                    wxLogError(wxT("An error occurred during drag and drop operation"));
                    wxFALLTHROUGH;
                case wxDragNone:
                case wxDragCancel:
                    Refresh(); // This is needed in wxMSW, otherwise resetting the position doesn't 'take'
                    SetCaretPosition(oldPos);
                    SetFocusObject(oldFocus, false);
                    wxFALLTHROUGH;
                default: break;
            }
            EndBatchUndo();

            m_preDrag = false;
            return;
        }
    }
#endif // wxUSE_DRAG_AND_DROP

    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(GetFont());

    long position = 0;
    wxPoint logicalPt = event.GetLogicalPosition(dc);
    wxRichTextObject* hitObj = nullptr;
    wxRichTextObject* contextObj = nullptr;

    int flags = 0;

    // If we're dragging, let's only consider positions at this level; otherwise
    // selecting a range is not going to work.
    wxRichTextParagraphLayoutBox* container = & GetBuffer();
    if (m_dragging)
    {
        flags = wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS;
        container = GetFocusObject();
    }
    wxRichTextDrawingContext context(& GetBuffer());
    int hit = container->HitTest(dc, context, GetUnscaledPoint(logicalPt), position, & hitObj, & contextObj, flags);

    // See if we need to change the cursor

    {
        if (hit != wxRICHTEXT_HITTEST_NONE && !(hit & wxRICHTEXT_HITTEST_OUTSIDE) && hitObj)
        {
            wxRichTextParagraphLayoutBox* actualContainer = wxDynamicCast(contextObj, wxRichTextParagraphLayoutBox);
            if (actualContainer)
                ProcessMouseMovement(actualContainer, hitObj, position, logicalPt);
        }
        else
            SetCursor(m_textCursor);
    }

    if (!event.Dragging())
    {
        event.Skip();
        return;
    }

    if (m_dragging
#if wxUSE_DRAG_AND_DROP
        && !m_preDrag
        && (distance > 4)
#endif
        )
    {
        wxRichTextParagraphLayoutBox* commonAncestor = nullptr;
        wxRichTextParagraphLayoutBox* otherContainer = nullptr;
        wxRichTextParagraphLayoutBox* firstContainer = nullptr;

        // Check for dragging across multiple containers
        long position2 = 0;
        wxRichTextObject* hitObj2 = nullptr, *contextObj2 = nullptr;
        int hit2 = GetBuffer().HitTest(dc, context, GetUnscaledPoint(logicalPt), position2, & hitObj2, & contextObj2, 0);
        if (hit2 != wxRICHTEXT_HITTEST_NONE && hitObj2 && hitObj != hitObj2)
        {
            // See if we can find a common ancestor
            if (m_selectionState == wxRichTextCtrlSelectionState_Normal)
            {
                firstContainer = GetFocusObject();
                commonAncestor = wxDynamicCast(firstContainer->GetParent(), wxRichTextParagraphLayoutBox);
            }
            else
            {
                firstContainer = wxDynamicCast(m_selectionAnchorObject, wxRichTextParagraphLayoutBox);
                //commonAncestor = GetFocusObject(); // when the selection state is not normal, the focus object (e.g. table)
                                                   // is the common ancestor.
                commonAncestor = wxDynamicCast(firstContainer->GetParent(), wxRichTextParagraphLayoutBox);
            }

            if (commonAncestor && commonAncestor->HandlesChildSelections())
            {
                wxRichTextObject* p = hitObj2;
                while (p)
                {
                    if (p->GetParent() == commonAncestor)
                    {
                        otherContainer = wxDynamicCast(p, wxRichTextParagraphLayoutBox);
                        break;
                    }
                    p = p->GetParent();
                }
            }

            if (commonAncestor && firstContainer && otherContainer)
            {
                // We have now got a second container that shares a parent with the current or anchor object.
                if (m_selectionState == wxRichTextCtrlSelectionState_Normal)
                {
                    // Don't go into common-ancestor selection mode if we still have the same
                    // container.
                    if (otherContainer != firstContainer)
                    {
                        m_selectionState = wxRichTextCtrlSelectionState_CommonAncestor;
                        m_selectionAnchorObject = firstContainer;
                        m_selectionAnchor = firstContainer->GetRange().GetStart();

                        // The common ancestor, such as a table, returns the cell selection
                        // between the anchor and current position.
                        m_selection = commonAncestor->GetSelection(m_selectionAnchor, otherContainer->GetRange().GetStart());
                    }
                }
                else
                {
                    m_selection = commonAncestor->GetSelection(m_selectionAnchor, otherContainer->GetRange().GetStart());
                }

                Refresh();

                if (otherContainer->AcceptsFocus())
                    SetFocusObject(otherContainer, false /* don't set caret and clear selection */);
                MoveCaret(-1, false);
                SetDefaultStyleToCursorStyle();
            }
        }
    }

    if (hitObj && m_dragging && hit != wxRICHTEXT_HITTEST_NONE && m_selectionState == wxRichTextCtrlSelectionState_Normal
#if wxUSE_DRAG_AND_DROP
        && !m_preDrag
        && (distance > 4)
#endif
        // Don't select to the end of the container when going outside the window
        // For analysis, see https://github.com/wxWidgets/wxWidgets/issues/15714
        && (! (hitObj == (& m_buffer) && ((hit & wxRICHTEXT_HITTEST_OUTSIDE) != 0)))
        )
    {
        SetCaretPositionAfterClick(container, position, hit, true /* extend selection */);
    }
}

/// Right-click
void wxRichTextCtrl::OnRightClick(wxMouseEvent& event)
{
    SetFocus();

    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(GetFont());

    long position = 0;
    wxPoint logicalPt = event.GetLogicalPosition(dc);
    wxRichTextObject* hitObj = nullptr;
    wxRichTextObject* contextObj = nullptr;
    wxRichTextDrawingContext context(& GetBuffer());
    int hit = GetFocusObject()->HitTest(dc, context, GetUnscaledPoint(logicalPt), position, & hitObj, & contextObj, wxRICHTEXT_HITTEST_HONOUR_ATOMIC);

    if (hitObj && hitObj->GetContainer() != GetFocusObject())
    {
        wxRichTextParagraphLayoutBox* actualContainer = wxDynamicCast(contextObj, wxRichTextParagraphLayoutBox);
        if (actualContainer && actualContainer->AcceptsFocus())
        {
            SetFocusObject(actualContainer, false /* don't set caret position yet */);
            SetCaretPositionAfterClick(actualContainer, position, hit);
        }
    }

    wxRichTextEvent cmdEvent(
        wxEVT_RICHTEXT_RIGHT_CLICK,
        GetId());
    cmdEvent.SetEventObject(this);
    cmdEvent.SetPosition(position);
    if (hitObj)
        cmdEvent.SetContainer(hitObj->GetContainer());

    if (!GetEventHandler()->ProcessEvent(cmdEvent))
        event.Skip();
}

/// Left-double-click
void wxRichTextCtrl::OnLeftDClick(wxMouseEvent& event)
{
    wxRichTextEvent cmdEvent(
        wxEVT_RICHTEXT_LEFT_DCLICK,
        GetId());
    cmdEvent.SetEventObject(this);
    cmdEvent.SetPosition(m_caretPosition+1);
    cmdEvent.SetContainer(GetFocusObject());

    if (!GetEventHandler()->ProcessEvent(cmdEvent))
    {
        bool okToSelectWord = true;
        // Don't try to select a word if we clicked on a floating objects such as an image.
        // Instead, select or deselect the object.
        if (wxRichTextBuffer::GetFloatingLayoutMode())
        {
            wxClientDC dc(this);
            PrepareDC(dc);
            dc.SetFont(GetFont());

            long position = 0;
            wxPoint logicalPt = event.GetLogicalPosition(dc);
            wxRichTextObject* hitObj = nullptr;
            wxRichTextObject* contextObj = nullptr;
            wxRichTextDrawingContext context(& GetBuffer());
            int hit = GetFocusObject()->HitTest(dc, context, GetUnscaledPoint(logicalPt), position, & hitObj, & contextObj, wxRICHTEXT_HITTEST_HONOUR_ATOMIC);
            wxUnusedVar(hit);
            if (hitObj && hitObj->IsFloating() && !hitObj->AcceptsFocus())
            {
                if ((GetFocusObject() == m_selection.GetContainer()) && m_selection.WithinSelection(hitObj->GetRange().GetStart()))
                {
                }
                else
                {
                    int from = hitObj->GetRange().GetStart();
                    int to = hitObj->GetRange().GetStart()+1;

                    wxRichTextSelection oldSelection = m_selection;
                    m_selectionAnchor = from-1;
                    m_selectionAnchorObject = nullptr;
                    m_selection.Set(wxRichTextRange(from, to-1), GetFocusObject());
                    RefreshForSelectionChange(oldSelection, m_selection);
                }
                okToSelectWord = false;
            }
        }
        if (okToSelectWord)
        {
            SelectWord(GetCaretPosition()+1);
        }
    }
}

/// Middle-click
void wxRichTextCtrl::OnMiddleClick(wxMouseEvent& event)
{
    wxRichTextEvent cmdEvent(
        wxEVT_RICHTEXT_MIDDLE_CLICK,
        GetId());
    cmdEvent.SetEventObject(this);
    cmdEvent.SetPosition(m_caretPosition+1);
    cmdEvent.SetContainer(GetFocusObject());

    if (!GetEventHandler()->ProcessEvent(cmdEvent))
        event.Skip();

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ && wxHAVE_PRIMARY_SELECTION
    // Paste any PRIMARY selection, if it exists
    wxTheClipboard->UsePrimarySelection(true);
    Paste();
    wxTheClipboard->UsePrimarySelection(false);
#endif
}

/// Key press
void wxRichTextCtrl::OnChar(wxKeyEvent& event)
{
    int flags = 0;
    if (event.CmdDown())
        flags |= wxRICHTEXT_CTRL_DOWN;
    if (event.ShiftDown())
        flags |= wxRICHTEXT_SHIFT_DOWN;
    if (event.AltDown())
        flags |= wxRICHTEXT_ALT_DOWN;

    if (event.GetEventType() == wxEVT_KEY_DOWN)
    {
        if (event.IsKeyInCategory(WXK_CATEGORY_NAVIGATION))
        {
            KeyboardNavigate(event.GetKeyCode(), flags);
            return;
        }

        long keycode = event.GetKeyCode();
        switch ( keycode )
        {
            case WXK_ESCAPE:
            case WXK_START:
            case WXK_LBUTTON:
            case WXK_RBUTTON:
            case WXK_CANCEL:
            case WXK_MBUTTON:
            case WXK_CLEAR:
            case WXK_SHIFT:
            case WXK_ALT:
            case WXK_CONTROL:
            case WXK_PAUSE:
            case WXK_CAPITAL:
            case WXK_END:
            case WXK_HOME:
            case WXK_LEFT:
            case WXK_UP:
            case WXK_RIGHT:
            case WXK_DOWN:
            case WXK_SELECT:
            case WXK_PRINT:
            case WXK_EXECUTE:
            case WXK_SNAPSHOT:
            case WXK_INSERT:
            case WXK_HELP:
            case WXK_F1:
            case WXK_F2:
            case WXK_F3:
            case WXK_F4:
            case WXK_F5:
            case WXK_F6:
            case WXK_F7:
            case WXK_F8:
            case WXK_F9:
            case WXK_F10:
            case WXK_F11:
            case WXK_F12:
            case WXK_F13:
            case WXK_F14:
            case WXK_F15:
            case WXK_F16:
            case WXK_F17:
            case WXK_F18:
            case WXK_F19:
            case WXK_F20:
            case WXK_F21:
            case WXK_F22:
            case WXK_F23:
            case WXK_F24:
            case WXK_NUMLOCK:
            case WXK_SCROLL:
            case WXK_PAGEUP:
            case WXK_PAGEDOWN:
            case WXK_NUMPAD_F1:
            case WXK_NUMPAD_F2:
            case WXK_NUMPAD_F3:
            case WXK_NUMPAD_F4:
            case WXK_NUMPAD_HOME:
            case WXK_NUMPAD_LEFT:
            case WXK_NUMPAD_UP:
            case WXK_NUMPAD_RIGHT:
            case WXK_NUMPAD_DOWN:
            case WXK_NUMPAD_PAGEUP:
            case WXK_NUMPAD_PAGEDOWN:
            case WXK_NUMPAD_END:
            case WXK_NUMPAD_BEGIN:
            case WXK_NUMPAD_INSERT:
            case WXK_WINDOWS_LEFT:
            case WXK_BROWSER_BACK:
            case WXK_BROWSER_FORWARD:
            case WXK_BROWSER_REFRESH:
            case WXK_BROWSER_STOP:
            case WXK_BROWSER_SEARCH:
            case WXK_BROWSER_FAVORITES:
            case WXK_BROWSER_HOME:
            case WXK_VOLUME_MUTE:
            case WXK_VOLUME_DOWN:
            case WXK_VOLUME_UP:
            case WXK_MEDIA_NEXT_TRACK:
            case WXK_MEDIA_PREV_TRACK:
            case WXK_MEDIA_STOP:
            case WXK_MEDIA_PLAY_PAUSE:
            case WXK_LAUNCH_MAIL:
            case WXK_LAUNCH_APP1:
            case WXK_LAUNCH_APP2:
            {
                return;
            }
            case WXK_MENU:
            {
                // Necessary for the context menu to work on wxGTK
                event.Skip();
                return;
            }
            default:
            {
            }
        }

        // Must process this before translation, otherwise it's translated into a WXK_DELETE event.
        if (event.CmdDown() && event.GetKeyCode() == WXK_BACK)
        {
            if (!ProcessBackKey(event, flags))
                return;
        }
        else
            event.Skip();

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
        if (!CanInsertContent(* GetFocusObject(), m_caretPosition+1))
            return;

        long newPos = m_caretPosition;

        if (HasSelection() && !CanDeleteRange(* GetFocusObject(), GetSelectionRange()))
        {
            return;
        }

        BeginBatchUndo(_("Insert Text"));

        DeleteSelectedContent(& newPos);

        if (event.ShiftDown())
        {
            wxString text;
            text = wxRichTextLineBreakChar;
            GetFocusObject()->InsertTextWithUndo(& GetBuffer(), newPos+1, text, this);
            m_caretAtLineStart = true;
            PositionCaret();
        }
        else
            GetFocusObject()->InsertNewlineWithUndo(& GetBuffer(), newPos+1, this, wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE|wxRICHTEXT_INSERT_INTERACTIVE);

        // Automatically renumber list
        bool isNumberedList = false;
        wxRichTextRange numberedListRange = FindRangeForList(newPos+1, isNumberedList);
        if (isNumberedList && numberedListRange != wxRichTextRange(-1, -1))
        {
            NumberList(numberedListRange, nullptr, wxRICHTEXT_SETSTYLE_RENUMBER|wxRICHTEXT_SETSTYLE_WITH_UNDO);
        }

        EndBatchUndo();
        SetDefaultStyleToCursorStyle();

        ScrollIntoView(m_caretPosition, WXK_RIGHT);

        wxRichTextEvent cmdEvent(
            wxEVT_RICHTEXT_RETURN,
            GetId());
        cmdEvent.SetEventObject(this);
        cmdEvent.SetFlags(flags);
        cmdEvent.SetPosition(newPos+1);
        cmdEvent.SetContainer(GetFocusObject());

        if (!GetEventHandler()->ProcessEvent(cmdEvent))
        {
            // Generate conventional event
            wxCommandEvent textEvent(wxEVT_TEXT_ENTER, GetId());
            InitCommandEvent(textEvent);

            GetEventHandler()->ProcessEvent(textEvent);
        }
        Update();
    }
    else if (event.GetKeyCode() == WXK_BACK)
    {
        ProcessBackKey(event, flags);
    }
    else if (event.GetKeyCode() == WXK_DELETE)
    {
        long newPos = m_caretPosition;

        if (HasSelection() && !CanDeleteRange(* GetFocusObject(), GetSelectionRange()))
        {
            return;
        }

        BeginBatchUndo(_("Delete Text"));

        bool processed = DeleteSelectedContent(& newPos);

        // Submit range in character positions, which are greater than caret positions,
        if (newPos < GetFocusObject()->GetOwnRange().GetEnd()+1)
        {
            if (event.CmdDown())
            {
                long pos = wxRichTextCtrl::FindNextWordPosition(1);
                if (pos != -1 && (pos > newPos))
                {
                    wxRichTextRange range(newPos+1, pos);
                    if (CanDeleteRange(* GetFocusObject(), range.FromInternal()))
                    {
                        GetFocusObject()->DeleteRangeWithUndo(range, this, & GetBuffer());
                    }
                    processed = true;
                }
            }

            if (!processed && newPos < (GetLastPosition()-1))
            {
                wxRichTextRange range(newPos+1, newPos+1);
                if (CanDeleteRange(* GetFocusObject(), range.FromInternal()))
                {
                    GetFocusObject()->DeleteRangeWithUndo(range, this, & GetBuffer());
                }
            }
        }

        EndBatchUndo();

        if (GetLastPosition() == -1)
        {
            GetFocusObject()->Reset();

            m_caretPosition = -1;
            PositionCaret();
            SetDefaultStyleToCursorStyle();
        }

        ScrollIntoView(m_caretPosition, WXK_LEFT);

        // Always send this event; wxEVT_RICHTEXT_CONTENT_DELETED will be sent only if there is an actual deletion.
        {
            wxRichTextEvent cmdEvent(
                wxEVT_RICHTEXT_DELETE,
                GetId());
            cmdEvent.SetEventObject(this);
            cmdEvent.SetFlags(flags);
            cmdEvent.SetPosition(m_caretPosition+1);
            cmdEvent.SetContainer(GetFocusObject());
            GetEventHandler()->ProcessEvent(cmdEvent);
        }

        Update();
    }
    else
    {
        long keycode = event.GetKeyCode();
        switch ( keycode )
        {
            case WXK_ESCAPE:
            case WXK_MENU: // Necessary for the context menu to work on wxGTK
            {
                event.Skip();
                return;
            }

            default:
            {
#ifdef __WXMAC__
                if (event.CmdDown())
#else
                // Fixes AltGr+key with European input languages on Windows
                if ((event.CmdDown() && !event.AltDown()) || (event.AltDown() && !event.CmdDown()))
#endif
                {
                    event.Skip();
                    return;
                }

                wxRichTextEvent cmdEvent1(
                    wxEVT_RICHTEXT_CONSUMING_CHARACTER,
                    GetId());
                cmdEvent1.SetEventObject(this);
                cmdEvent1.SetFlags(flags);
                cmdEvent1.SetCharacter(event.GetUnicodeKey());
                cmdEvent1.SetPosition(m_caretPosition+1);
                cmdEvent1.SetContainer(GetFocusObject());
                if (GetEventHandler()->ProcessEvent(cmdEvent1) && !cmdEvent1.IsAllowed())
                {
                    return;
                }

                wxRichTextEvent cmdEvent(
                    wxEVT_RICHTEXT_CHARACTER,
                    GetId());
                cmdEvent.SetEventObject(this);
                cmdEvent.SetFlags(flags);
                cmdEvent.SetCharacter(event.GetUnicodeKey());
                cmdEvent.SetPosition(m_caretPosition+1);
                cmdEvent.SetContainer(GetFocusObject());

                if (keycode == wxT('\t'))
                {
                    // See if we need to promote or demote the selection or paragraph at the cursor
                    // position, instead of inserting a tab.
                    long pos = GetAdjustedCaretPosition(GetCaretPosition());
                    wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(pos);
                    if (para && para->GetRange().GetStart() == pos && para->GetAttributes().HasListStyleName())
                    {
                        wxRichTextRange range;
                        if (HasSelection())
                            range = GetSelectionRange();
                        else
                            range = para->GetRange().FromInternal();

                        int promoteBy = event.ShiftDown() ? 1 : -1;

                        PromoteList(promoteBy, range, nullptr);

                        GetEventHandler()->ProcessEvent(cmdEvent);

                        return;
                    }
                }

                if (!CanInsertContent(* GetFocusObject(), m_caretPosition+1))
                    return;

                if (HasSelection() && !CanDeleteRange(* GetFocusObject(), GetSelectionRange()))
                    return;

                BeginBatchUndo(_("Insert Text"));

                long newPos = m_caretPosition;
                DeleteSelectedContent(& newPos);

                wxString str = event.GetUnicodeKey();
                GetFocusObject()->InsertTextWithUndo(& GetBuffer(), newPos+1, str, this, 0);

                EndBatchUndo();

                SetDefaultStyleToCursorStyle();
                ScrollIntoView(m_caretPosition, WXK_RIGHT);

                cmdEvent.SetPosition(m_caretPosition);
                GetEventHandler()->ProcessEvent(cmdEvent);

                Update();
            }
        }
    }
}

bool wxRichTextCtrl::ProcessMouseMovement(wxRichTextParagraphLayoutBox* container, wxRichTextObject* WXUNUSED(obj), long position, const wxPoint& WXUNUSED(pos))
{
    wxRichTextAttr attr;
    if (container && GetStyle(position, attr, container))
    {
        if (attr.HasFlag(wxTEXT_ATTR_URL))
        {
            SetCursor(m_urlCursor);
        }
        else if (!attr.HasFlag(wxTEXT_ATTR_URL))
        {
            SetCursor(m_textCursor);
        }
        return true;
    }
    else
        return false;
}

// Processes the back key
bool wxRichTextCtrl::ProcessBackKey(wxKeyEvent& event, int flags)
{
    if (!IsEditable())
    {
        return false;
    }

    if (HasSelection() && !CanDeleteRange(* GetFocusObject(), GetSelectionRange()))
    {
        return false;
    }

    wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(m_caretPosition, true);

    // If we're at the start of a list item with a bullet, let's 'delete' the bullet, i.e.
    // make it a continuation paragraph.
    if (!HasSelection() && para && ((m_caretPosition+1) == para->GetRange().GetStart()) &&
        para->GetAttributes().HasBulletStyle() && (para->GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_CONTINUATION) == 0)
    {
        wxRichTextParagraph* newPara = wxDynamicCast(para->Clone(), wxRichTextParagraph);
        newPara->GetAttributes().SetBulletStyle(newPara->GetAttributes().GetBulletStyle() | wxTEXT_ATTR_BULLET_STYLE_CONTINUATION);

        wxRichTextAction* action = new wxRichTextAction(nullptr, _("Remove Bullet"), wxRICHTEXT_CHANGE_STYLE, & GetBuffer(), GetFocusObject(), this);
        action->SetRange(newPara->GetRange());
        action->SetPosition(GetCaretPosition());
        action->GetNewParagraphs().AppendChild(newPara);
        // Also store the old ones for Undo
        action->GetOldParagraphs().AppendChild(new wxRichTextParagraph(*para));

        GetBuffer().Invalidate(para->GetRange());
        GetBuffer().SubmitAction(action);

        // Automatically renumber list
        bool isNumberedList = false;
        wxRichTextRange numberedListRange = FindRangeForList(m_caretPosition, isNumberedList);
        if (isNumberedList && numberedListRange != wxRichTextRange(-1, -1))
        {
            NumberList(numberedListRange, nullptr, wxRICHTEXT_SETSTYLE_RENUMBER|wxRICHTEXT_SETSTYLE_WITH_UNDO);
        }

        Update();
    }
    else
    {
        BeginBatchUndo(_("Delete Text"));

        long newPos = m_caretPosition;

        bool processed = DeleteSelectedContent(& newPos);

        // Submit range in character positions, which are greater than caret positions,
        // so subtract 1 for deleted character and add 1 for conversion to character position.
        if (newPos > -1)
        {
            if (event.CmdDown())
            {
                long pos = wxRichTextCtrl::FindNextWordPosition(-1);
                if (pos < newPos)
                {
                    wxRichTextRange range(pos+1, newPos);
                    if (CanDeleteRange(* GetFocusObject(), range.FromInternal()))
                    {
                        GetFocusObject()->DeleteRangeWithUndo(range, this, & GetBuffer());
                    }
                    processed = true;
                }
            }

            if (!processed)
            {
                wxRichTextRange range(newPos, newPos);
                if (CanDeleteRange(* GetFocusObject(), range.FromInternal()))
                {
                    GetFocusObject()->DeleteRangeWithUndo(range, this, & GetBuffer());
                }
            }
        }

        EndBatchUndo();

        if (GetLastPosition() == -1)
        {
            GetFocusObject()->Reset();

            m_caretPosition = -1;
            PositionCaret();
            SetDefaultStyleToCursorStyle();
        }

        ScrollIntoView(m_caretPosition, WXK_LEFT);

        // Always send this event; wxEVT_RICHTEXT_CONTENT_DELETED will be sent only if there is an actual deletion.
        {
            wxRichTextEvent cmdEvent(
                wxEVT_RICHTEXT_DELETE,
                GetId());
            cmdEvent.SetEventObject(this);
            cmdEvent.SetFlags(flags);
            cmdEvent.SetPosition(m_caretPosition+1);
            cmdEvent.SetContainer(GetFocusObject());
            GetEventHandler()->ProcessEvent(cmdEvent);
        }

        Update();
    }

    return true;
}

/// Delete content if there is a selection, e.g. when pressing a key.
bool wxRichTextCtrl::DeleteSelectedContent(long* newPos)
{
    if (HasSelection())
    {
        long pos = m_selection.GetRange().GetStart();
        wxRichTextRange range = m_selection.GetRange();

        // SelectAll causes more to be selected than doing it interactively,
        // and causes a new paragraph to be inserted. So for multiline buffers,
        // don't delete the final position.
        if (range.GetEnd() == GetLastPosition() && GetNumberOfLines() > 0)
            range.SetEnd(range.GetEnd()-1);

        GetFocusObject()->DeleteRangeWithUndo(range, this, & GetBuffer());
        m_selection.Reset();
        m_selectionState = wxRichTextCtrlSelectionState_Normal;

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

    if (keyCode == WXK_RIGHT || keyCode == WXK_NUMPAD_RIGHT)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = WordRight(1, flags);
        else
            success = MoveRight(1, flags);
    }
    else if (keyCode == WXK_LEFT || keyCode == WXK_NUMPAD_LEFT)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = WordLeft(1, flags);
        else
            success = MoveLeft(1, flags);
    }
    else if (keyCode == WXK_UP || keyCode == WXK_NUMPAD_UP)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = MoveToParagraphStart(flags);
        else
            success = MoveUp(1, flags);
    }
    else if (keyCode == WXK_DOWN || keyCode == WXK_NUMPAD_DOWN)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = MoveToParagraphEnd(flags);
        else
            success = MoveDown(1, flags);
    }
    else if (keyCode == WXK_PAGEUP || keyCode == WXK_NUMPAD_PAGEUP)
    {
        success = PageUp(1, flags);
    }
    else if (keyCode == WXK_PAGEDOWN || keyCode == WXK_NUMPAD_PAGEDOWN)
    {
        success = PageDown(1, flags);
    }
    else if (keyCode == WXK_HOME || keyCode == WXK_NUMPAD_HOME)
    {
        if (flags & wxRICHTEXT_CTRL_DOWN)
            success = MoveHome(flags);
        else
            success = MoveToLineStart(flags);
    }
    else if (keyCode == WXK_END || keyCode == WXK_NUMPAD_END)
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
        if (oldPos == newPos)
            return false;

        wxRichTextSelection oldSelection = m_selection;

        m_selection.SetContainer(GetFocusObject());

        wxRichTextRange oldRange;
        if (m_selection.IsValid())
            oldRange = m_selection.GetRange();
        else
            oldRange = wxRICHTEXT_NO_SELECTION;
        wxRichTextRange newRange;

        // If not currently selecting, start selecting
        if (oldRange.GetStart() == -2)
        {
            m_selectionAnchor = oldPos;

            if (oldPos > newPos)
                newRange.SetRange(newPos+1, oldPos);
            else
                newRange.SetRange(oldPos+1, newPos);
        }
        else
        {
            // Always ensure that the selection range start is greater than
            // the end.
            if (newPos > m_selectionAnchor)
                newRange.SetRange(m_selectionAnchor+1, newPos);
            else if (newPos == m_selectionAnchor)
                newRange = wxRichTextRange(-2, -2);
            else
                newRange.SetRange(newPos+1, m_selectionAnchor);
        }

        m_selection.SetRange(newRange);

        RefreshForSelectionChange(oldSelection, m_selection);

        if (newRange.GetStart() > newRange.GetEnd())
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
    if (!m_verticalScrollbarEnabled)
        return false;

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

    wxRect rect = GetScaledRect(line->GetRect());

    bool scrolled = false;

    wxSize clientSize = GetClientSize();

    int leftMargin, rightMargin, topMargin, bottomMargin;

    {
        wxClientDC dc(this);
        wxRichTextObject::GetTotalMargin(dc, & GetBuffer(), GetBuffer().GetAttributes(), leftMargin, rightMargin,
            topMargin, bottomMargin);
    }
    clientSize.y -= (int) (0.5 + bottomMargin * GetScale());

    if (GetWindowStyle() & wxRE_CENTRE_CARET)
    {
        int y = rect.y - GetClientSize().y/2;
        int yUnits = (y + ppuY - 1) / ppuY;
        if (y >= 0 && (y + clientSize.y) < (int) (0.5 + GetBuffer().GetCachedSize().y * GetScale()))
        {
            if (startYUnits != yUnits)
            {
                SetScrollbars(ppuX, ppuY, sxUnits, syUnits, 0, yUnits);
                scrolled = true;
            }
#if !wxRICHTEXT_USE_OWN_CARET
            if (scrolled)
#endif
                PositionCaret();

            return scrolled;
        }
    }

    // Going down
    if (keyCode == WXK_DOWN || keyCode == WXK_NUMPAD_DOWN ||
        keyCode == WXK_RIGHT || keyCode == WXK_NUMPAD_RIGHT ||
        keyCode == WXK_END || keyCode == WXK_NUMPAD_END ||
        keyCode == WXK_PAGEDOWN || keyCode == WXK_NUMPAD_PAGEDOWN)
    {
        if ((rect.y + rect.height) > (clientSize.y + startY))
        {
            // Make it scroll so this item is at the bottom
            // of the window
            int y = rect.y - (clientSize.y - rect.height);
            int yUnits = (y + ppuY - 1) / ppuY;

            // If we're still off the screen, scroll another line down
            if ((rect.y + rect.height) > (clientSize.y + (yUnits*ppuY)))
                yUnits ++;

            if (startYUnits != yUnits)
            {
                SetScrollbars(ppuX, ppuY, sxUnits, syUnits, 0, yUnits);
                scrolled = true;
            }
        }
        else if (rect.y < (startY + (int) (0.5 + GetBuffer().GetTopMargin() * GetScale())))
        {
            // Make it scroll so this item is at the top
            // of the window
            int y = rect.y - (int) (0.5 + GetBuffer().GetTopMargin() * GetScale());
            int yUnits = (y + ppuY - 1) / ppuY;

            if (startYUnits != yUnits)
            {
                SetScrollbars(ppuX, ppuY, sxUnits, syUnits, 0, yUnits);
                scrolled = true;
            }
        }
    }
    // Going up
    else if (keyCode == WXK_UP  || keyCode == WXK_NUMPAD_UP ||
             keyCode == WXK_LEFT || keyCode == WXK_NUMPAD_LEFT ||
             keyCode == WXK_HOME || keyCode == WXK_NUMPAD_HOME ||
             keyCode == WXK_PAGEUP || keyCode == WXK_NUMPAD_PAGEUP )
    {
        if (rect.y < (startY + (int) (0.5 + GetBuffer().GetBottomMargin() * GetScale())))
        {
            // Make it scroll so this item is at the top
            // of the window
            int y = rect.y - (int) (0.5 + GetBuffer().GetTopMargin() * GetScale());
            int yUnits = (y + ppuY - 1) / ppuY;

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
            int yUnits = (y + ppuY - 1) / ppuY;

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

#if !wxRICHTEXT_USE_OWN_CARET
    if (scrolled)
#endif
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

    wxRect rect = GetScaledRect(line->GetRect());
    wxSize clientSize = GetClientSize();

    int topMargin = (int) (0.5 + GetBuffer().GetTopMargin() * GetScale());
    int bottomMargin = (int) (0.5 + GetBuffer().GetBottomMargin() * GetScale());

    return (rect.GetTop() >= (startY + topMargin)) &&
           (rect.GetBottom() <= (startY + clientSize.y - bottomMargin));
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
    wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(oldPosition);

    // Only do the check if we're not at the end of the paragraph (where things work OK
    // anyway)
    if (para && (oldPosition != para->GetRange().GetEnd() - 1))
    {
        wxRichTextLine* line = GetFocusObject()->GetLineAtPosition(oldPosition);

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
    wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(oldPosition);

    // Only do the check if we're not at the start of the paragraph (where things work OK
    // anyway)
    if (para && (oldPosition != para->GetRange().GetStart()))
    {
        wxRichTextLine* line = GetFocusObject()->GetLineAtPosition(oldPosition);

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
    // Test for continuing table selection
    if (flags & wxRICHTEXT_SHIFT_DOWN)
    {
        if (m_selection.GetContainer() && m_selection.GetContainer()->IsKindOf(CLASSINFO(wxRichTextTable)))
        {
            wxRichTextTable* table = wxDynamicCast(m_selection.GetContainer(), wxRichTextTable);
            if (GetFocusObject() && GetFocusObject()->GetParent() == m_selection.GetContainer())
            {
                ExtendCellSelection(table, 0, noPositions);
                return true;
            }
        }
    }

    long startPos = -1;
    long endPos = GetFocusObject()->GetOwnRange().GetEnd();

    bool beyondBottom = (noPositions > 0 && (m_caretPosition + noPositions >= endPos));
    bool beyondTop = (noPositions < 0 && (m_caretPosition <= startPos + noPositions + 1));

    if (beyondBottom || beyondTop)
    {
        wxPoint pt = GetLogicalPoint(GetCaret()->GetPosition());

        int hitTestFlags = wxRICHTEXT_HITTEST_NO_FLOATING_OBJECTS|wxRICHTEXT_HITTEST_HONOUR_ATOMIC;

        if (beyondBottom)
            pt.x = GetFocusObject()->GetPosition().x + GetFocusObject()->GetCachedSize().x + 2;
        else
            pt.x = GetFocusObject()->GetPosition().x - 2;

        pt.y += 2;

        long newPos = 0;
        wxClientDC dc(this);
        PrepareDC(dc);
        dc.SetFont(GetFont());

        wxRichTextObject* hitObj = nullptr;
        wxRichTextObject* contextObj = nullptr;
        wxRichTextDrawingContext context(& GetBuffer());
        int hitTest = GetBuffer().HitTest(dc, context, pt, newPos, & hitObj, & contextObj, hitTestFlags);

        if (hitObj &&
            ((hitTest & wxRICHTEXT_HITTEST_NONE) == 0) &&
            (! (hitObj == (& m_buffer) && ((hitTest & wxRICHTEXT_HITTEST_OUTSIDE) != 0))) // outside the buffer counts as 'do nothing'
            )
        {
            wxRichTextParagraphLayoutBox* actualContainer = wxDynamicCast(contextObj, wxRichTextParagraphLayoutBox);
            if (actualContainer && actualContainer != GetFocusObject() && actualContainer->AcceptsFocus() && actualContainer->IsShown())
            {
                if ((flags & wxRICHTEXT_SHIFT_DOWN) &&
                    GetFocusObject()->IsKindOf(CLASSINFO(wxRichTextCell)) &&
                    actualContainer->IsKindOf(CLASSINFO(wxRichTextCell)) &&
                    GetFocusObject()->GetParent() == actualContainer->GetParent())
                {
                    // Start selecting cells in a table
                    wxRichTextTable* table = wxDynamicCast(actualContainer->GetParent(), wxRichTextTable);
                    if (table)
                    {
                        StartCellSelection(table, actualContainer);
                        return true;
                    }
                }

                // If the new container is a cell, go to the top or bottom of it.
                if (actualContainer->IsKindOf(CLASSINFO(wxRichTextCell)))
                {
                    if (beyondBottom)
                        newPos = 0;
                    else
                        newPos = actualContainer->GetOwnRange().GetEnd()-1;
                }

                SetFocusObject(actualContainer, false /* don't set caret position yet */);
                bool caretLineStart = true;
                long caretPosition = FindCaretPositionForCharacterPosition(newPos, hitTest, actualContainer, caretLineStart);

                SelectNone();

                SetCaretPosition(caretPosition, caretLineStart);
                PositionCaret();
                SetDefaultStyleToCursorStyle();

                return true;
            }
        }
    }
    else if (!beyondTop && !beyondBottom)
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
        if (noPositions == 1)
            MoveCaretForward(oldPos);
        else if (noPositions == -1)
            MoveCaretBack(oldPos);
        else
            SetCaretPosition(newPos);

        PositionCaret();
        SetDefaultStyleToCursorStyle();

        return true;
    }
    return false;
}

/// Move left
bool wxRichTextCtrl::MoveLeft(int noPositions, int flags)
{
    return MoveRight(-noPositions, flags);
}

// Find the caret position for the combination of hit-test flags and character position.
// Returns the caret position and also an indication of where to place the caret (caretLineStart)
// since this is ambiguous (same position used for end of line and start of next).
long wxRichTextCtrl::FindCaretPositionForCharacterPosition(long position, int hitTestFlags, wxRichTextParagraphLayoutBox* container,
                                                   bool& caretLineStart)
{
    // If end of previous line, and hitTest is wxRICHTEXT_HITTEST_BEFORE,
    // we want to be at the end of the last line but with m_caretAtLineStart set to true,
    // so we view the caret at the start of the line.
    caretLineStart = false;
    long caretPosition = position;

    if (hitTestFlags & wxRICHTEXT_HITTEST_BEFORE)
    {
        wxRichTextLine* thisLine = container->GetLineAtPosition(position-1);
        wxRichTextRange lineRange;
        if (thisLine)
            lineRange = thisLine->GetAbsoluteRange();

        if (thisLine && (position-1) == lineRange.GetEnd())
        {
            caretPosition --;
            caretLineStart = true;
        }
        else
        {
            wxRichTextParagraph* para = container->GetParagraphAtPosition(position);
            if (para && para->GetRange().GetStart() == position)
                caretPosition --;
        }
    }
    return caretPosition;
}

/// Move up
bool wxRichTextCtrl::MoveDown(int noLines, int flags)
{
    if (!GetCaret())
        return false;

    // Test for continuing table selection
    if (flags & wxRICHTEXT_SHIFT_DOWN)
    {
        if (m_selection.GetContainer() && m_selection.GetContainer()->IsKindOf(CLASSINFO(wxRichTextTable)))
        {
            wxRichTextTable* table = wxDynamicCast(m_selection.GetContainer(), wxRichTextTable);
            if (GetFocusObject() && GetFocusObject()->GetParent() == m_selection.GetContainer())
            {
                ExtendCellSelection(table, noLines, 0);
                return true;
            }
        }
    }

    long lineNumber = GetFocusObject()->GetVisibleLineNumber(m_caretPosition, true, m_caretAtLineStart);
    wxPoint pt = GetLogicalPoint(GetCaret()->GetPosition());
    long newLine = lineNumber + noLines;
    bool notInThisObject = false;

    if (lineNumber != -1)
    {
        if (noLines > 0)
        {
            long lastLine = GetFocusObject()->GetVisibleLineNumber(GetFocusObject()->GetOwnRange().GetEnd());
            if (newLine > lastLine)
                notInThisObject = true;
        }
        else
        {
            if (newLine < 0)
                notInThisObject = true;
        }
    }

    wxRichTextParagraphLayoutBox* container = GetFocusObject();
    int hitTestFlags = wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS|wxRICHTEXT_HITTEST_NO_FLOATING_OBJECTS|wxRICHTEXT_HITTEST_HONOUR_ATOMIC;

    bool lineIsEmpty = false;
    if (notInThisObject)
    {
        // If we know we're navigating out of the current object,
        // try to find an object anywhere in the buffer at the new position (up or down a bit)
        container = & GetBuffer();
        hitTestFlags &= ~wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS;

        if (noLines > 0) // going down
        {
            pt.y = GetFocusObject()->GetPosition().y + GetFocusObject()->GetCachedSize().y + 2;
        }
        else // going up
        {
            pt.y = GetFocusObject()->GetPosition().y - 2;
        }
    }
    else
    {
        wxRichTextLine* lineObj = GetFocusObject()->GetLineForVisibleLineNumber(newLine);
        if (lineObj)
        {
            pt.y = lineObj->GetAbsolutePosition().y + 2;
            if (lineObj->GetRange().GetStart() == lineObj->GetRange().GetEnd())
                lineIsEmpty = true;
        }
        else
            return false;
    }

    long newPos = 0;
    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(GetFont());

    wxRichTextObject* hitObj = nullptr;
    wxRichTextObject* contextObj = nullptr;
    wxRichTextDrawingContext context(& GetBuffer());
    int hitTest = container->HitTest(dc, context, pt, newPos, & hitObj, & contextObj, hitTestFlags);

    if (hitObj &&
        ((hitTest & wxRICHTEXT_HITTEST_NONE) == 0) &&
        (! (hitObj == (& m_buffer) && ((hitTest & wxRICHTEXT_HITTEST_OUTSIDE) != 0))) // outside the buffer counts as 'do nothing'
        )
    {
        if (notInThisObject)
        {
            wxRichTextParagraphLayoutBox* actualContainer = wxDynamicCast(contextObj, wxRichTextParagraphLayoutBox);
            if (actualContainer && actualContainer != GetFocusObject() && actualContainer->AcceptsFocus())
            {
                if ((flags & wxRICHTEXT_SHIFT_DOWN) &&
                    GetFocusObject()->IsKindOf(CLASSINFO(wxRichTextCell)) &&
                    actualContainer->IsKindOf(CLASSINFO(wxRichTextCell)) &&
                    GetFocusObject()->GetParent() == actualContainer->GetParent())
                {
                    // Start selecting cells in a table
                    wxRichTextTable* table = wxDynamicCast(actualContainer->GetParent(), wxRichTextTable);
                    if (table)
                    {
                        StartCellSelection(table, actualContainer);
                        return true;
                    }
                }

                SetFocusObject(actualContainer, false /* don't set caret position yet */);

                container = actualContainer;
            }
        }

        bool caretLineStart = true;

        // If the line is empty, there is only one possible position for the caret,
        // so force the 'before' state so FindCaretPositionForCharacterPosition doesn't
        // just return the same position.
        if (lineIsEmpty)
        {
            hitTest &= ~wxRICHTEXT_HITTEST_AFTER;
            hitTest |= wxRICHTEXT_HITTEST_BEFORE;
        }
        long caretPosition = FindCaretPositionForCharacterPosition(newPos, hitTest, container, caretLineStart);
        long newSelEnd = caretPosition;
        bool extendSel;

        if (notInThisObject)
            extendSel = false;
        else
            extendSel = ExtendSelection(m_caretPosition, newSelEnd, flags);

        if (!extendSel)
            SelectNone();

        SetCaretPosition(caretPosition, caretLineStart);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        return true;
    }

    return false;
}

/// Extend a table selection in the given direction
bool wxRichTextCtrl::ExtendCellSelection(wxRichTextTable* table, int noRowSteps, int noColSteps)
{
    int thisRow = -1;
    int thisCol = -1;
    int r, c;
    for (r = 0; r < table->GetRowCount(); r ++)
    {
        for (c = 0; c < table->GetColumnCount(); c++)
        {
            wxRichTextCell* cell = table->GetCell(r, c);
            if (cell == GetFocusObject())
            {
                thisRow = r;
                thisCol = c;
            }
        }
    }
    if (thisRow != -1)
    {
        int newRow = wxMax(0, wxMin((thisRow + noRowSteps), table->GetRowCount()-1));
        int newCol = wxMax(0, wxMin((thisCol + noColSteps), table->GetColumnCount()-1));

        if (newRow != thisRow || newCol != thisCol)
        {
            // Make sure we're on a visible row or column
            r = newRow;
            c = newCol;
            int rowInc = noRowSteps > 0 ? 1 : -1;
            int colInc = noColSteps > 0 ? 1 : -1;
            bool visibleRow = false;
            bool visibleCol = false;
            if (noRowSteps != 0)
            {
                while (r >= 0 && r < table->GetRowCount())
                {
                    wxRichTextCell* cell = table->GetCell(r, newCol);
                    if (cell->IsShown())
                    {
                        newRow = r;
                        visibleRow = true;
                        break;
                    }
                    else
                    {
                        r += rowInc;
                    }
                }
                // No change if the cell would not be visible
                if (!visibleRow)
                    return true;
            }

            if (noColSteps != 0)
            {
                while (c >= 0 && c < table->GetColumnCount())
                {
                    wxRichTextCell* cell = table->GetCell(newRow, c);
                    if (cell->IsShown())
                    {
                        newCol = c;
                        visibleCol = true;
                        break;
                    }
                    else
                    {
                        c += colInc;
                    }
                }
                // No change if the cell would not be visible
                if (!visibleCol)
                    return true;
            }

            wxRichTextCell* newCell = table->GetCell(newRow, newCol);
            if (newCell)
            {
                m_selection = table->GetSelection(m_selectionAnchor, newCell->GetRange().GetStart());
                Refresh();
                if (newCell->AcceptsFocus())
                    SetFocusObject(newCell, false);
                MoveCaret(-1, false);
                SetDefaultStyleToCursorStyle();
            }
        }
    }
    return true;
}

/// Start selecting cells
bool wxRichTextCtrl::StartCellSelection(wxRichTextTable* table, wxRichTextParagraphLayoutBox* newCell)
{
    // Start selecting cells in a table
    m_selectionState = wxRichTextCtrlSelectionState_CommonAncestor;
    m_selectionAnchorObject = GetFocusObject();
    m_selectionAnchor = GetFocusObject()->GetRange().GetStart();

    // The common ancestor, such as a table, returns the cell selection
    // between the anchor and current position.
    m_selection = table->GetSelection(m_selectionAnchor, newCell->GetRange().GetStart());
    Refresh();

    if (newCell->AcceptsFocus())
        SetFocusObject(newCell, false /* don't set caret and clear selection */);
    MoveCaret(-1, false);
    SetDefaultStyleToCursorStyle();

    return true;
}

/// Move up
bool wxRichTextCtrl::MoveUp(int noLines, int flags)
{
    return MoveDown(- noLines, flags);
}

/// Move to the end of the paragraph
bool wxRichTextCtrl::MoveToParagraphEnd(int flags)
{
    wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(m_caretPosition, true);
    if (para)
    {
        long newPos = para->GetRange().GetEnd() - 1;
        bool extendSel = ExtendSelection(m_caretPosition, newPos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(newPos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        return true;
    }

    return false;
}

/// Move to the start of the paragraph
bool wxRichTextCtrl::MoveToParagraphStart(int flags)
{
    wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(m_caretPosition, true);
    if (para)
    {
        long newPos = para->GetRange().GetStart() - 1;
        bool extendSel = ExtendSelection(m_caretPosition, newPos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(newPos, true);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

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

        wxRichTextParagraph* para = GetFocusObject()->GetParagraphForLine(line);

        SetCaretPosition(newPos, para->GetRange().GetStart() != lineRange.GetStart());
        PositionCaret();
        SetDefaultStyleToCursorStyle();

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

        return true;
    }
    else
        return false;
}

/// Move to the end of the buffer
bool wxRichTextCtrl::MoveEnd(int flags)
{
    long endPos = GetFocusObject()->GetOwnRange().GetEnd()-1;

    if (m_caretPosition != endPos)
    {
        bool extendSel = ExtendSelection(m_caretPosition, endPos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(endPos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

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
        int topMargin = GetBuffer().GetTopMargin();
        int bottomMargin = GetBuffer().GetBottomMargin();
        int height = int( 0.5 + ((clientSize.y - topMargin - bottomMargin) / GetScale()));
        int newY = line->GetAbsolutePosition().y + noPages*height;

        wxRichTextLine* newLine = GetFocusObject()->GetLineAtYPosition(newY);
        if (newLine)
        {
            wxRichTextRange lineRange = newLine->GetAbsoluteRange();
            long pos = lineRange.GetStart()-1;
            if (pos != m_caretPosition)
            {
                wxRichTextParagraph* para = GetFocusObject()->GetParagraphForLine(newLine);

                bool extendSel = ExtendSelection(m_caretPosition, pos, flags);
                if (!extendSel)
                    SelectNone();

                SetCaretPosition(pos, para->GetRange().GetStart() != lineRange.GetStart());
                PositionCaret();
                SetDefaultStyleToCursorStyle();

                return true;
            }
        }
    }

    return false;
}

static bool wxRichTextCtrlIsWhitespace(const wxString& str)
{
    return str == wxT(" ") || str == wxT("\t") || (!str.empty() && (str[0] == (wxChar) 160));
}

// Finds the caret position for the next word
long wxRichTextCtrl::FindNextWordPosition(int direction) const
{
    long endPos = GetFocusObject()->GetOwnRange().GetEnd();

    if (direction > 0)
    {
        long i = m_caretPosition+1+direction; // +1 for conversion to character pos

        // First skip current text to space
        while (i < endPos && i > -1)
        {
            // i is in character, not caret positions
            wxString text = GetFocusObject()->GetTextForRange(wxRichTextRange(i, i));
            wxRichTextLine* line = GetFocusObject()->GetLineAtPosition(i, false);
            if (line && (i == line->GetAbsoluteRange().GetEnd()))
            {
                break;
            }
            else if (!wxRichTextCtrlIsWhitespace(text) && !text.empty())
                i += direction;
            else
            {
                break;
            }
        }
        while (i < endPos && i > -1)
        {
            // i is in character, not caret positions
            wxString text = GetFocusObject()->GetTextForRange(wxRichTextRange(i, i));
            wxRichTextLine* line = GetFocusObject()->GetLineAtPosition(i, false);
            if (line && (i == line->GetAbsoluteRange().GetEnd()))
                return wxMax(-1, i);

            if (text.empty()) // End of paragraph, or maybe an image
                return wxMax(-1, i - 1);
            else if (wxRichTextCtrlIsWhitespace(text))
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
            wxString text = GetFocusObject()->GetTextForRange(wxRichTextRange(i, i));
            wxRichTextLine* line = GetFocusObject()->GetLineAtPosition(i, false);

            if (text.empty() || (line && (i == line->GetAbsoluteRange().GetStart()))) // End of paragraph, or maybe an image
                break;
            else if (wxRichTextCtrlIsWhitespace(text) || text.empty())
                i += direction;
            else
                break;
        }
        // Next skip current text to space
        while (i < endPos && i > -1)
        {
            // i is in character, not caret positions
            wxString text = GetFocusObject()->GetTextForRange(wxRichTextRange(i, i));
            wxRichTextLine* line = GetFocusObject()->GetLineAtPosition(i, false);
            if (line && line->GetAbsoluteRange().GetStart() == i)
                return i-1;

            if (!wxRichTextCtrlIsWhitespace(text) /* && !text.empty() */)
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
        wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(pos, true);

        bool extendSel = ExtendSelection(m_caretPosition, pos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(pos, para->GetRange().GetStart() != pos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

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
        wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(pos, true);

        bool extendSel = ExtendSelection(m_caretPosition, pos, flags);
        if (!extendSel)
            SelectNone();

        SetCaretPosition(pos, para->GetRange().GetStart() != pos);
        PositionCaret();
        SetDefaultStyleToCursorStyle();

        return true;
    }

    return false;
}

/// Sizing
void wxRichTextCtrl::OnSize(wxSizeEvent& event)
{
    // Only do sizing optimization for large buffers
    if (GetBuffer().GetOwnRange().GetEnd() > m_delayedLayoutThreshold)
    {
        m_fullLayoutRequired = true;
        m_fullLayoutTime = wxGetLocalTimeMillis();
        m_fullLayoutSavedPosition = GetFirstVisiblePosition();
        LayoutContent(true /* onlyVisibleRect */);
    }
    else
        GetBuffer().Invalidate(wxRICHTEXT_ALL);

#if wxRICHTEXT_BUFFERED_PAINTING
    RecreateBuffer();
#endif

    // Anti-hysteresis code: a way to determine whether a combination of OnPaint and
    // OnSize was the source of a scrollbar change.
    m_setupScrollbarsCountInOnSize = m_setupScrollbarsCount;

    if (GetDelayedImageLoading())
        RequestDelayedImageProcessing();

    event.Skip();
}

// Force any pending layout due to large buffer
void wxRichTextCtrl::ForceDelayedLayout()
{
    if (m_fullLayoutRequired)
    {
        m_fullLayoutRequired = false;
        m_fullLayoutTime = 0;
        GetBuffer().Invalidate(wxRICHTEXT_ALL);
        ShowPosition(m_fullLayoutSavedPosition);
        Refresh(false);
        Update();
    }
}

/// Idle-time processing
void wxRichTextCtrl::OnIdle(wxIdleEvent& event)
{
#if wxRICHTEXT_USE_OWN_CARET
    if (((wxRichTextCaret*) GetCaret())->GetNeedsUpdate())
    {
        ((wxRichTextCaret*) GetCaret())->SetNeedsUpdate(false);
        PositionCaret();
        if (!GetCaret()->IsVisible())
            GetCaret()->Show();
    }
#endif

    const int layoutInterval = wxRICHTEXT_DEFAULT_LAYOUT_INTERVAL;

    if (m_fullLayoutRequired && (wxGetLocalTimeMillis() > (m_fullLayoutTime + layoutInterval)))
    {
        m_fullLayoutRequired = false;
        m_fullLayoutTime = 0;
        GetBuffer().Invalidate(wxRICHTEXT_ALL);
        ShowPosition(m_fullLayoutSavedPosition);
        Refresh(false);
    }

    const int imageProcessingInterval = wxRICHTEXT_DEFAULT_DELAYED_IMAGE_PROCESSING_INTERVAL;

    if (m_enableDelayedImageLoading && m_delayedImageProcessingRequired && (wxGetLocalTimeMillis() > (m_delayedImageProcessingTime + imageProcessingInterval)))
    {
        m_delayedImageProcessingTimer.Stop();
        m_delayedImageProcessingRequired = false;
        m_delayedImageProcessingTime = 0;
        ProcessDelayedImageLoading(true);
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
#if wxRICHTEXT_USE_OWN_CARET
    if (!((wxRichTextCaret*) GetCaret())->GetNeedsUpdate())
    {
        if (GetCaret()->IsVisible())
            GetCaret()->Hide();
        ((wxRichTextCaret*) GetCaret())->SetNeedsUpdate();
    }
#endif

    event.Skip();
}

/// Set up scrollbars, e.g. after a resize
void wxRichTextCtrl::SetupScrollbars(bool atTop, bool fromOnPaint)
{
    if (IsFrozen())
        return;

    if (GetBuffer().IsEmpty() || !m_verticalScrollbarEnabled)
    {
        SetScrollbars(0, 0, 0, 0, 0, 0);
        return;
    }

    // TODO: reimplement scrolling so we scroll by line, not by fixed number
    // of pixels. See e.g. wxVScrolledWindow for ideas.
    int pixelsPerUnit = GetLineHeight();
    wxSize clientSize = GetClientSize();

    int maxHeight = (int) (0.5 + GetScale() * (GetBuffer().GetCachedSize().y + GetBuffer().GetTopMargin()));

    // Round up so we have at least maxHeight pixels
    int unitsY = (maxHeight + pixelsPerUnit - 1) / pixelsPerUnit;

    int startX = 0, startY = 0;
    if (!atTop)
        GetViewStart(& startX, & startY);

    int maxPositionX = 0;
    int maxPositionY = (wxMax(unitsY * pixelsPerUnit - clientSize.y, 0) + pixelsPerUnit - 1) / pixelsPerUnit;

    int newStartX = wxMin(maxPositionX, startX);
    int newStartY = wxMin(maxPositionY, startY);

    int oldPPUX, oldPPUY;
    int oldStartX, oldStartY;
    int oldVirtualSizeX = 0, oldVirtualSizeY = 0;
    GetScrollPixelsPerUnit(& oldPPUX, & oldPPUY);
    GetViewStart(& oldStartX, & oldStartY);
    GetVirtualSize(& oldVirtualSizeX, & oldVirtualSizeY);
    if (oldPPUY > 0)
        oldVirtualSizeY /= oldPPUY;

    if (oldPPUX == 0 && oldPPUY == pixelsPerUnit && oldVirtualSizeY == unitsY && oldStartX == newStartX && oldStartY == newStartY)
        return;

    // Don't set scrollbars if there were none before, and there will be none now.
    if (oldPPUY != 0 && (oldVirtualSizeY*oldPPUY < clientSize.y) && (unitsY*pixelsPerUnit < clientSize.y))
        return;

    // Hysteresis detection. If an object width is relative to the window width, then there can be
    // interaction between image width and total content height, causing the scrollbar to appear
    // and disappear rapidly. We need to see if we're getting this looping via OnSize/OnPaint,
    // and if so, keep the scrollbar shown. We use a counter to see whether the SetupScrollbars
    // call is caused by OnSize, versus any other operation such as editing.
    // There may still be some flickering when editing at the boundary of scrollbar/no scrollbar
    // states, but looping will be avoided.
    bool doSetScrollbars = true;
    wxSize windowSize = GetSize();
    if (fromOnPaint)
    {
        if ((windowSize == m_lastWindowSize) && (m_setupScrollbarsCountInOnSize == m_setupScrollbarsCount))
        {
            // If we will be going from scrollbar to no scrollbar, we're now probably in hysteresis.
            // So don't set the scrollbars this time.
            if ((oldPPUY != 0) && (oldVirtualSizeY*oldPPUY > clientSize.y) && (unitsY*pixelsPerUnit <= clientSize.y))
                doSetScrollbars = false;
        }
    }

    m_lastWindowSize = windowSize;
    m_setupScrollbarsCount ++;
    if (m_setupScrollbarsCount > 32000)
        m_setupScrollbarsCount = 0;

    // Move to previous scroll position if
    // possible
    if (doSetScrollbars)
        SetScrollbars(0, pixelsPerUnit, 0, unitsY, newStartX, newStartY);
}

/// Paint the background
void wxRichTextCtrl::PaintBackground(wxDC& dc)
{
    wxColour backgroundColour = GetBackgroundColour();
    if (!backgroundColour.IsOk())
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

#if wxRICHTEXT_BUFFERED_PAINTING
/// Recreate buffer bitmap if necessary
bool wxRichTextCtrl::RecreateBuffer(const wxSize& size)
{
    wxSize sz = size;
    if (sz == wxDefaultSize)
        sz = GetClientSize();

    if (sz.x < 1 || sz.y < 1)
        return false;

    if (!m_bufferBitmap.IsOk() || m_bufferBitmap.GetLogicalWidth() < sz.x || m_bufferBitmap.GetLogicalHeight() < sz.y)
    {
        // As per https://github.com/wxWidgets/wxWidgets/issues/14403, prevent very inefficient fix to alpha bits of
        // destination by making the backing bitmap 24-bit. Note that using 24-bit depth breaks painting of
        // scrolled areas on wxWidgets 2.8.
        int depth = -1;
#if defined(__WXMSW__)
        depth = 24;
#endif
        m_bufferBitmap.CreateWithLogicalSize(sz, GetDPIScaleFactor(), depth);
    }
    return m_bufferBitmap.IsOk();
}
#endif

// ----------------------------------------------------------------------------
// file IO functions
// ----------------------------------------------------------------------------
#if wxUSE_FFILE && wxUSE_STREAMS
bool wxRichTextCtrl::DoLoadFile(const wxString& filename, int fileType)
{
    SetFocusObject(& GetBuffer(), true);

    bool success = GetBuffer().LoadFile(filename, (wxRichTextFileType)fileType);
    if (success)
        m_filename = filename;

    DiscardEdits();
    SetInsertionPoint(0);
    LayoutContent();
    PositionCaret();
    SetupScrollbars(true);
    Refresh(false);
    wxTextCtrl::SendTextUpdatedEvent(this);

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
    if (GetBuffer().SaveFile(filename, (wxRichTextFileType)fileType))
    {
        m_filename = filename;

        DiscardEdits();

        return true;
    }

    wxLogError(_("The text couldn't be saved."));

    return false;
}
#endif // wxUSE_FFILE && wxUSE_STREAMS

// ----------------------------------------------------------------------------
// wxRichTextCtrl specific functionality
// ----------------------------------------------------------------------------

/// Add a new paragraph of text to the end of the buffer
wxRichTextRange wxRichTextCtrl::AddParagraph(const wxString& text)
{
    wxRichTextRange range = GetFocusObject()->AddParagraph(text);
    GetBuffer().Invalidate();
    LayoutContent();
    return range;
}

/// Add an image
wxRichTextRange wxRichTextCtrl::AddImage(const wxImage& image)
{
    wxRichTextRange range = GetFocusObject()->AddImage(image);
    GetBuffer().Invalidate();
    LayoutContent();
    return range;
}

// ----------------------------------------------------------------------------
// selection and ranges
// ----------------------------------------------------------------------------

/// Select none
void wxRichTextCtrl::SelectNone()
{
    if (m_selection.IsValid())
    {
        wxRichTextSelection oldSelection = m_selection;

        m_selection.Reset();

        RefreshForSelectionChange(oldSelection, m_selection);
    }
    m_selectionAnchor = -2;
    m_selectionAnchorObject = nullptr;
    m_selectionState = wxRichTextCtrlSelectionState_Normal;
}

static bool wxIsWordDelimiter(const wxString& text)
{
    return !text.IsEmpty() && !wxIsalnum(text[0]);
}

/// Select the word at the given character position
bool wxRichTextCtrl::SelectWord(long position)
{
    if (position < 0 || position > GetFocusObject()->GetOwnRange().GetEnd())
        return false;

    wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(position);
    if (!para)
        return false;

    if (position == para->GetRange().GetEnd())
        position --;

    long positionStart = position;
    long positionEnd = position;

    for (positionStart = position; positionStart >= para->GetRange().GetStart(); positionStart --)
    {
        wxString text = GetFocusObject()->GetTextForRange(wxRichTextRange(positionStart, positionStart));
        if (wxIsWordDelimiter(text))
        {
            positionStart ++;
            break;
        }
    }
    if (positionStart < para->GetRange().GetStart())
        positionStart = para->GetRange().GetStart();

    for (positionEnd = position; positionEnd < para->GetRange().GetEnd(); positionEnd ++)
    {
        wxString text = GetFocusObject()->GetTextForRange(wxRichTextRange(positionEnd, positionEnd));
        if (wxIsWordDelimiter(text))
        {
            positionEnd --;
            break;
        }
    }
    if (positionEnd >= para->GetRange().GetEnd())
        positionEnd = para->GetRange().GetEnd()-1;

    if (positionEnd < positionStart)
        return false;

    SetSelection(positionStart, positionEnd+1);

    if (positionStart >= 0)
    {
        MoveCaret(positionStart-1, true);
        SetDefaultStyleToCursorStyle();
    }

    return true;
}

wxString wxRichTextCtrl::GetStringSelection() const
{
    long from, to;
    GetSelection(&from, &to);

    return GetRange(from, to);
}

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
    wxClientDC dc(const_cast<wxRichTextCtrl*>(this));
    const_cast<wxRichTextCtrl*>(this)->PrepareDC(dc);

    // Buffer uses logical position (relative to start of buffer)
    // so convert
    wxPoint pt2 = GetLogicalPoint(pt);

    wxRichTextObject* hitObj = nullptr;
    wxRichTextObject* contextObj = nullptr;
    wxRichTextDrawingContext context(const_cast<wxRichTextBuffer*>(&GetBuffer()));
    int hit = const_cast<wxRichTextCtrl*>(this)->GetFocusObject()->HitTest(dc, context, pt2, *pos, &hitObj, &contextObj, wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS);

    if ((hit & wxRICHTEXT_HITTEST_BEFORE) && (hit & wxRICHTEXT_HITTEST_OUTSIDE))
        return wxTE_HT_BEFORE;
    else if ((hit & wxRICHTEXT_HITTEST_AFTER) && (hit & wxRICHTEXT_HITTEST_OUTSIDE))
        return wxTE_HT_BEYOND;
    else if (hit & (wxRICHTEXT_HITTEST_BEFORE|wxRICHTEXT_HITTEST_AFTER))
        return wxTE_HT_ON_TEXT;

    return wxTE_HT_UNKNOWN;
}

wxRichTextParagraphLayoutBox*
wxRichTextCtrl::FindContainerAtPoint(const wxPoint& pt, long& position, int& hit, wxRichTextObject* hitObj, int flags/* = 0*/)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(GetFont());

    wxPoint logicalPt = GetLogicalPoint(pt);

    wxRichTextObject* contextObj = nullptr;
    wxRichTextDrawingContext context(& GetBuffer());
    hit = GetBuffer().HitTest(dc, context, GetUnscaledPoint(logicalPt), position, &hitObj, &contextObj, flags);
    wxRichTextParagraphLayoutBox* container = wxDynamicCast(contextObj, wxRichTextParagraphLayoutBox);

    return container;
}


// ----------------------------------------------------------------------------
// set/get the controls text
// ----------------------------------------------------------------------------

wxString wxRichTextCtrl::DoGetValue() const
{
    return GetBuffer().GetText();
}

wxString wxRichTextCtrl::GetRange(long from, long to) const
{
    // Public API for range is different from internals
    return GetFocusObject()->GetTextForRange(wxRichTextRange(from, to-1));
}

void wxRichTextCtrl::DoSetValue(const wxString& value, int flags)
{
    // Don't call Clear here, since it always sends a text updated event
    m_buffer.ResetAndClearCommands();
    m_buffer.Invalidate(wxRICHTEXT_ALL);
    m_caretPosition = -1;
    m_caretPositionForDefaultStyle = -2;
    m_caretAtLineStart = false;
    m_selection.Reset();
    m_selectionState = wxRichTextCtrlSelectionState_Normal;

    Scroll(0,0);

    if (!IsFrozen())
    {
        LayoutContent();
        Refresh(false);
    }

    if (!value.IsEmpty())
    {
        // Remove empty paragraph
        GetBuffer().Clear();
        DoWriteText(value, flags);

        // for compatibility, don't move the cursor when doing SetValue()
        SetInsertionPoint(0);
    }
    else
    {
        // still send an event for consistency
        if (flags & SetValue_SendEvent)
            wxTextCtrl::SendTextUpdatedEvent(this);
    }
    DiscardEdits();
}

void wxRichTextCtrl::WriteText(const wxString& value)
{
    DoWriteText(value);
}

void wxRichTextCtrl::DoWriteText(const wxString& value, int flags)
{
    wxString valueUnix = wxTextFile::Translate(value, wxTextFileType_Unix);

    GetFocusObject()->InsertTextWithUndo(& GetBuffer(), m_caretPosition+1, valueUnix, this, wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE);
    if (!IsFrozen())
    {
        wxRichTextDrawingContext context(& GetBuffer());
        GetBuffer().Defragment(context);
    }

    if ( flags & SetValue_SendEvent )
        wxTextCtrl::SendTextUpdatedEvent(this);
}

void wxRichTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();

    WriteText(text);
}

/// Write an image at the current insertion point
bool wxRichTextCtrl::WriteImage(const wxImage& image, wxBitmapType bitmapType, const wxRichTextAttr& textAttr)
{
    wxRichTextImageBlock imageBlock;

    wxImage image2 = image;
    if (imageBlock.MakeImageBlock(image2, bitmapType))
        return WriteImage(imageBlock, textAttr);

    return false;
}

bool wxRichTextCtrl::WriteImage(const wxString& filename, wxBitmapType bitmapType, const wxRichTextAttr& textAttr)
{
    wxRichTextImageBlock imageBlock;

    wxImage image;
    if (imageBlock.MakeImageBlock(filename, bitmapType, image, false))
        return WriteImage(imageBlock, textAttr);

    return false;
}

bool wxRichTextCtrl::WriteImage(const wxRichTextImageBlock& imageBlock, const wxRichTextAttr& textAttr)
{
    return GetFocusObject()->InsertImageWithUndo(& GetBuffer(), m_caretPosition+1, imageBlock, this, 0, textAttr);
}

bool wxRichTextCtrl::WriteImage(const wxBitmap& bitmap, wxBitmapType bitmapType, const wxRichTextAttr& textAttr)
{
    if (bitmap.IsOk())
    {
        wxRichTextImageBlock imageBlock;

        wxImage image = bitmap.ConvertToImage();
        if (image.IsOk() && imageBlock.MakeImageBlock(image, bitmapType))
            return WriteImage(imageBlock, textAttr);
    }

    return false;
}

// Write a text box at the current insertion point.
wxRichTextBox* wxRichTextCtrl::WriteTextBox(const wxRichTextAttr& textAttr)
{
    wxRichTextBox* textBox = new wxRichTextBox;
    textBox->SetAttributes(textAttr);
    textBox->SetParent(& GetBuffer()); // set parent temporarily for AddParagraph to use correct style
    textBox->AddParagraph(wxEmptyString);
    textBox->SetParent(nullptr);

    // If the box has an invalid foreground colour, its text will mimic any upstream value (see #15224)
    if (!textBox->GetAttributes().GetTextColour().IsOk())
    {
        textBox->GetAttributes().SetTextColour(GetBasicStyle().GetTextColour());
    }

    // The object returned is the one actually inserted into the buffer,
    // while the original one is deleted.
    wxRichTextObject* obj = GetFocusObject()->InsertObjectWithUndo(& GetBuffer(), m_caretPosition+1, textBox, this, wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE);
    wxRichTextBox* box = wxDynamicCast(obj, wxRichTextBox);
    return box;
}

wxRichTextField* wxRichTextCtrl::WriteField(const wxString& fieldType, const wxRichTextProperties& properties,
                            const wxRichTextAttr& textAttr)
{
    return GetFocusObject()->InsertFieldWithUndo(& GetBuffer(), m_caretPosition+1, fieldType, properties,
                this, wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE, textAttr);
}

// Write a table at the current insertion point, returning the table.
wxRichTextTable* wxRichTextCtrl::WriteTable(int rows, int cols, const wxRichTextAttr& tableAttr, const wxRichTextAttr& cellAttr)
{
    wxASSERT(rows > 0 && cols > 0);

    if (rows <= 0 || cols <= 0)
        return nullptr;

    wxRichTextTable* table = new wxRichTextTable;
    table->SetAttributes(tableAttr);
    table->SetParent(& GetBuffer()); // set parent temporarily for AddParagraph to use correct style
    table->SetBasicStyle(GetBasicStyle());

    table->CreateTable(rows, cols);

    table->SetParent(nullptr);

    // If cells have an invalid foreground colour, their text will mimic any upstream value (see #15224)
    wxRichTextAttr attr = cellAttr;
    if (!attr.GetTextColour().IsOk())
    {
        attr.SetTextColour(GetBasicStyle().GetTextColour());
    }

    int i, j;
    for (j = 0; j < rows; j++)
    {
        for (i = 0; i < cols; i++)
        {
            table->GetCell(j, i)->GetAttributes() = attr;
        }
    }

    // The object returned is the one actually inserted into the buffer,
    // while the original one is deleted.
    wxRichTextObject* obj = GetFocusObject()->InsertObjectWithUndo(& GetBuffer(), m_caretPosition+1, table, this, wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE);
    wxRichTextTable* tableResult = wxDynamicCast(obj, wxRichTextTable);
    return tableResult;
}


/// Insert a newline (actually paragraph) at the current insertion point.
bool wxRichTextCtrl::Newline()
{
    return GetFocusObject()->InsertNewlineWithUndo(& GetBuffer(), m_caretPosition+1, this, wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE);
}

/// Insert a line break at the current insertion point.
bool wxRichTextCtrl::LineBreak()
{
    wxString text;
    text = wxRichTextLineBreakChar;
    return GetFocusObject()->InsertTextWithUndo(& GetBuffer(), m_caretPosition+1, text, this);
}

// ----------------------------------------------------------------------------
// Clipboard operations
// ----------------------------------------------------------------------------

void wxRichTextCtrl::Copy()
{
    if (CanCopy())
    {
        wxRichTextRange range = GetInternalSelectionRange();
        GetBuffer().CopyToClipboard(range);
    }
}

void wxRichTextCtrl::Cut()
{
    if (CanCut())
    {
        wxRichTextRange range = GetInternalSelectionRange();
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
    return (m_selection.IsValid() && m_selection.GetContainer() == GetFocusObject());
}

bool wxRichTextCtrl::HasUnfocusedSelection() const
{
    return m_selection.IsValid();
}

bool wxRichTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    return HasSelection();
}

bool wxRichTextCtrl::CanCut() const
{
    return CanDeleteSelection();
}

bool wxRichTextCtrl::CanPaste() const
{
    if ( !IsEditable() || !GetFocusObject() || !CanInsertContent(* GetFocusObject(), m_caretPosition+1))
        return false;

    return GetBuffer().CanPasteFromClipboard();
}

bool wxRichTextCtrl::CanDeleteSelection() const
{
    return HasSelection() && IsEditable() && CanDeleteRange(* GetFocusObject(), GetSelectionRange());
}


// ----------------------------------------------------------------------------
// Accessors
// ----------------------------------------------------------------------------

void wxRichTextCtrl::SetContextMenu(wxMenu* menu)
{
    if (m_contextMenu && m_contextMenu != menu)
        delete m_contextMenu;
    m_contextMenu = menu;
}

void wxRichTextCtrl::SetEditable(bool editable)
{
    m_editable = editable;
}

void wxRichTextCtrl::SetInsertionPoint(long pos)
{
    SelectNone();

    m_caretPosition = pos - 1;
    m_caretAtLineStart = true;

    PositionCaret();

    SetDefaultStyleToCursorStyle();
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
    return GetFocusObject()->GetOwnRange().GetEnd();
}

// If the return values from and to are the same, there is no
// selection.
void wxRichTextCtrl::GetSelection(long* from, long* to) const
{
    if (m_selection.IsValid())
    {
        *from = m_selection.GetRange().GetStart();
        *to = m_selection.GetRange().GetEnd();
        (*to) ++;
    }
    else
    {
        *from = -2;
        *to = -2;
    }
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

    if (from == to)
    {
        SelectNone();
    }
    else
    {
        wxRichTextSelection oldSelection = m_selection;

        m_selectionAnchor = from-1;
        m_selectionAnchorObject = nullptr;
        m_selection.Set(wxRichTextRange(from, to-1), GetFocusObject());

        m_caretPosition = wxMax(-1, to-1);

        RefreshForSelectionChange(oldSelection, m_selection);
        PositionCaret();
    }
}

// ----------------------------------------------------------------------------
// Editing
// ----------------------------------------------------------------------------

void wxRichTextCtrl::Replace(long from, long to,
                             const wxString& value)
{
    BeginBatchUndo(_("Replace"));

    SetSelection(from, to);

    wxRichTextAttr attr(GetDefaultStyle());

    DeleteSelectedContent();

    SetDefaultStyle(attr);

    if (!value.IsEmpty())
        DoWriteText(value, SetValue_SelectionOnly);

    EndBatchUndo();
}

void wxRichTextCtrl::Remove(long from, long to)
{
    SelectNone();

    GetFocusObject()->DeleteRangeWithUndo(wxRichTextRange(from, to-1), this, & GetBuffer());

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
    return GetFocusObject()->GetParagraphCount();
}

// ----------------------------------------------------------------------------
// Positions <-> coords
// ----------------------------------------------------------------------------

long wxRichTextCtrl::XYToPosition(long x, long y) const
{
    return GetFocusObject()->XYToPosition(x, y);
}

bool wxRichTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    return GetFocusObject()->PositionToXY(pos, x, y);
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
    return GetFocusObject()->GetParagraphLength(lineNo);
}

wxString wxRichTextCtrl::GetLineText(long lineNo) const
{
    return GetFocusObject()->GetParagraphText(lineNo);
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
    return GetCommandProcessor()->CanUndo() && IsEditable();
}

bool wxRichTextCtrl::CanRedo() const
{
    return GetCommandProcessor()->CanRedo() && IsEditable();
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

wxSize wxRichTextCtrl::DoGetBestSize() const
{
    return FromDIP(wxSize(10, 10));
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
    if (GetLastPosition() > 0)
        SelectAll();
}

void wxRichTextCtrl::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
    event.Enable(GetLastPosition() > 0);
}

void wxRichTextCtrl::OnProperties(wxCommandEvent& event)
{
    int idx = event.GetId() - wxID_RICHTEXT_PROPERTIES1;
    if (idx >= 0 && idx < m_contextMenuPropertiesInfo.GetCount())
    {
        wxRichTextObject* obj = m_contextMenuPropertiesInfo.GetObject(idx);
        if (obj && CanEditProperties(obj))
            EditProperties(obj, this);

        m_contextMenuPropertiesInfo.Clear();
    }
}

void wxRichTextCtrl::OnUpdateProperties(wxUpdateUIEvent& event)
{
    int idx = event.GetId() - wxID_RICHTEXT_PROPERTIES1;
    event.Enable(idx >= 0 && idx < m_contextMenuPropertiesInfo.GetCount());
}

void wxRichTextCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    if (event.GetEventObject() != this)
    {
        event.Skip();
        return;
    }

    ShowContextMenu(m_contextMenu, event.GetPosition());
}

// Prepares the context menu, adding appropriate property-editing commands.
// Returns the number of property commands added.
int wxRichTextCtrl::PrepareContextMenu(wxMenu* menu, const wxPoint& pt, bool addPropertyCommands)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(GetFont());

    m_contextMenuPropertiesInfo.Clear();

    long position = 0;
    wxRichTextObject* hitObj = nullptr;
    wxRichTextObject* contextObj = nullptr;
    if (pt != wxDefaultPosition)
    {
        wxPoint logicalPt = GetLogicalPoint(ScreenToClient(pt));
        wxRichTextDrawingContext context(& GetBuffer());
        int hit = GetBuffer().HitTest(dc, context, GetUnscaledPoint(logicalPt), position, & hitObj, & contextObj);

        if (hit == wxRICHTEXT_HITTEST_ON || hit == wxRICHTEXT_HITTEST_BEFORE || hit == wxRICHTEXT_HITTEST_AFTER)
        {
            wxRichTextParagraphLayoutBox* actualContainer = wxDynamicCast(contextObj, wxRichTextParagraphLayoutBox);
            if (hitObj && actualContainer)
            {
                if (actualContainer->AcceptsFocus())
                {
                    SetFocusObject(actualContainer, false /* don't set caret position yet */);
                    SetCaretPositionAfterClick(actualContainer, position, hit);
                }

                if (addPropertyCommands)
                    m_contextMenuPropertiesInfo.AddItems(this, actualContainer, hitObj);
            }
            else
            {
                if (addPropertyCommands)
                    m_contextMenuPropertiesInfo.AddItems(this, GetFocusObject(), hitObj);
            }
        }
        else
        {
            if (addPropertyCommands)
                m_contextMenuPropertiesInfo.AddItems(this, GetFocusObject(), nullptr);
        }
    }
    else
    {
        // Invoked from the keyboard, so don't set the caret position and don't use the event
        // position
        hitObj = GetFocusObject()->GetLeafObjectAtPosition(m_caretPosition+1);
        if (hitObj)
            contextObj = hitObj->GetParentContainer();
        else
            contextObj = GetFocusObject();

        wxRichTextParagraphLayoutBox* actualContainer = wxDynamicCast(contextObj, wxRichTextParagraphLayoutBox);
        if (hitObj && actualContainer)
        {
            if (addPropertyCommands)
                m_contextMenuPropertiesInfo.AddItems(this, actualContainer, hitObj);
        }
        else
        {
            if (addPropertyCommands)
                m_contextMenuPropertiesInfo.AddItems(this, GetFocusObject(), nullptr);
        }
    }

    if (menu)
    {
        if (addPropertyCommands)
            m_contextMenuPropertiesInfo.AddMenuItems(menu);
        return m_contextMenuPropertiesInfo.GetCount();
    }
    else
        return 0;
}

// Shows the context menu, adding appropriate property-editing commands
bool wxRichTextCtrl::ShowContextMenu(wxMenu* menu, const wxPoint& pt, bool addPropertyCommands)
{
    if (menu)
    {
        PrepareContextMenu(menu, pt, addPropertyCommands);
        PopupMenu(menu);
        return true;
    }
    else
        return false;
}

bool wxRichTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    return GetFocusObject()->SetStyle(wxRichTextRange(start, end-1), wxRichTextAttr(style));
}

bool wxRichTextCtrl::SetStyle(long start, long end, const wxRichTextAttr& style)
{
    return GetFocusObject()->SetStyle(wxRichTextRange(start, end-1), style);
}

bool wxRichTextCtrl::SetStyle(const wxRichTextRange& range, const wxTextAttr& style)
{
    return GetFocusObject()->SetStyle(range.ToInternal(), wxRichTextAttr(style));
}

bool wxRichTextCtrl::SetStyle(const wxRichTextRange& range, const wxRichTextAttr& style)
{
    return GetFocusObject()->SetStyle(range.ToInternal(), style);
}

void wxRichTextCtrl::SetStyle(wxRichTextObject *obj, const wxRichTextAttr& textAttr, int flags)
{
    GetFocusObject()->SetStyle(obj, textAttr, flags);
}

// extended style setting operation with flags including:
// wxRICHTEXT_SETSTYLE_WITH_UNDO, wxRICHTEXT_SETSTYLE_OPTIMIZE, wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY.
// see richtextbuffer.h for more details.

bool wxRichTextCtrl::SetStyleEx(const wxRichTextRange& range, const wxRichTextAttr& style, int flags)
{
    return GetFocusObject()->SetStyle(range.ToInternal(), style, flags);
}

bool wxRichTextCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    return GetBuffer().SetDefaultStyle(style);
}

bool wxRichTextCtrl::SetDefaultStyle(const wxRichTextAttr& style)
{
    wxRichTextAttr attr1(style);
    attr1.GetTextBoxAttr().Reset();
    return GetBuffer().SetDefaultStyle(attr1);
}

const wxRichTextAttr& wxRichTextCtrl::GetDefaultStyleEx() const
{
    return GetBuffer().GetDefaultStyle();
}

bool wxRichTextCtrl::GetStyle(long position, wxTextAttr& style)
{
    wxRichTextAttr attr;
    if (GetFocusObject()->GetStyle(position, attr))
    {
        style = attr;
        return true;
    }
    else
        return false;
}

bool wxRichTextCtrl::GetStyle(long position, wxRichTextAttr& style)
{
    return GetFocusObject()->GetStyle(position, style);
}

bool wxRichTextCtrl::GetStyle(long position, wxRichTextAttr& style, wxRichTextParagraphLayoutBox* container)
{
    wxRichTextAttr attr;
    if (container->GetStyle(position, attr))
    {
        style = attr;
        return true;
    }
    else
        return false;
}

// get the common set of styles for the range
bool wxRichTextCtrl::GetStyleForRange(const wxRichTextRange& range, wxTextAttr& style)
{
    wxRichTextAttr attr;
    if (GetFocusObject()->GetStyleForRange(range.ToInternal(), attr))
    {
        style = attr;
        return true;
    }
    else
        return false;
}

bool wxRichTextCtrl::GetStyleForRange(const wxRichTextRange& range, wxRichTextAttr& style)
{
    return GetFocusObject()->GetStyleForRange(range.ToInternal(), style);
}

bool wxRichTextCtrl::GetStyleForRange(const wxRichTextRange& range, wxRichTextAttr& style, wxRichTextParagraphLayoutBox* container)
{
    return container->GetStyleForRange(range.ToInternal(), style);
}

/// Get the content (uncombined) attributes for this position.
bool wxRichTextCtrl::GetUncombinedStyle(long position, wxRichTextAttr& style)
{
    return GetFocusObject()->GetUncombinedStyle(position, style);
}

/// Get the content (uncombined) attributes for this position.
bool wxRichTextCtrl::GetUncombinedStyle(long position, wxRichTextAttr& style, wxRichTextParagraphLayoutBox* container)
{
    return container->GetUncombinedStyle(position, style);
}

bool wxRichTextCtrl::SetProperties(const wxRichTextRange& range, const wxRichTextProperties& properties, int flags)
{
    return GetFocusObject()->SetProperties(range.ToInternal(), properties, flags);
}

/// Set font, and also the buffer attributes
bool wxRichTextCtrl::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);

    wxRichTextAttr attr = GetBuffer().GetAttributes();
    attr.SetFont(font);
    GetBuffer().SetBasicStyle(attr);

    GetBuffer().Invalidate(wxRICHTEXT_ALL);
    Refresh(false);

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
void wxRichTextCtrl::PositionCaret(wxRichTextParagraphLayoutBox* container)
{
    if (!GetCaret())
        return;

    //wxLogDebug(wxT("PositionCaret"));

    wxRect caretRect;
    if (GetCaretPositionForIndex(GetCaretPosition(), caretRect, container))
    {
#if !wxRICHTEXT_USE_OWN_CARET
        caretRect = GetScaledRect(caretRect);
#endif
        int topMargin = (int) (0.5 + GetScale()*GetBuffer().GetTopMargin());
        int bottomMargin = (int) (0.5 + GetScale()*GetBuffer().GetBottomMargin());
        wxPoint newPt = caretRect.GetPosition();
        wxSize newSz = caretRect.GetSize();
        wxPoint pt = GetPhysicalPoint(newPt);
        if (GetCaret()->GetPosition() != pt || GetCaret()->GetSize() != newSz)
        {
            GetCaret()->Hide();
            if (GetCaret()->GetSize() != newSz)
                GetCaret()->SetSize(newSz);

            // Adjust size so the caret size and position doesn't appear in the margins
            if (((pt.y + newSz.y) <= topMargin) || (pt.y >= (GetClientSize().y - bottomMargin)))
            {
                pt.x = -200;
                pt.y = -200;
            }
            else if (pt.y < topMargin && (pt.y + newSz.y) > topMargin)
            {
                newSz.y -= (topMargin - pt.y);
                if (newSz.y > 0)
                {
                    pt.y = topMargin;
                    GetCaret()->SetSize(newSz);
                }
            }
            else if (pt.y < (GetClientSize().y - bottomMargin) && (pt.y + newSz.y) > (GetClientSize().y - bottomMargin))
            {
                newSz.y = GetClientSize().y - bottomMargin - pt.y;
                GetCaret()->SetSize(newSz);
            }

            GetCaret()->Move(pt);
            GetCaret()->Show();
        }
    }
}

/// Get the caret height and position for the given character position
bool wxRichTextCtrl::GetCaretPositionForIndex(long position, wxRect& rect, wxRichTextParagraphLayoutBox* container)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetUserScale(GetScale(), GetScale());
    dc.SetFont(GetFont());

    wxPoint pt;
    int height = 0;

    if (!container)
        container = GetFocusObject();

    wxRichTextDrawingContext context(& GetBuffer());
    if (container->FindPosition(dc, context, position, pt, & height, m_caretAtLineStart))
    {
        // Caret height can't be zero
        if (height == 0)
            height = dc.GetCharHeight();

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
    wxRichTextLine* line = GetFocusObject()->GetLineAtPosition(caretPosition, true);
    wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(caretPosition, true);
    if (line)
    {
        wxRichTextRange lineRange = line->GetAbsoluteRange();
        if (caretPosition == lineRange.GetStart()-1 &&
            (para->GetRange().GetStart() != lineRange.GetStart()))
        {
            // Only test for caret start/end position if we're looking at the current caret position,
            // otherwise m_caretAtLineStart is meaningless
            if (!m_caretAtLineStart && (caretPosition == m_caretPosition))
                line = GetFocusObject()->GetLineAtPosition(caretPosition-1, true);
        }
    }
    return line;
}


/// Move the caret to the given character position
bool wxRichTextCtrl::MoveCaret(long pos, bool showAtLineStart, wxRichTextParagraphLayoutBox* container)
{
    if (GetBuffer().IsDirty())
        LayoutContent();

    if (!container)
        container = GetFocusObject();

    if (pos <= container->GetOwnRange().GetEnd())
    {
        SetCaretPosition(pos, showAtLineStart);

        PositionCaret(container);

        return true;
    }
    else
        return false;
}

/// Layout the buffer: which we must do before certain operations, such as
/// setting the caret position.
bool wxRichTextCtrl::LayoutContent(bool onlyVisibleRect)
{
    if (GetBuffer().IsDirty() || onlyVisibleRect)
    {
        wxRect availableSpace(GetUnscaledSize(GetClientSize()));
        if (availableSpace.width == 0)
            availableSpace.width = 10;
        if (availableSpace.height == 0)
            availableSpace.height = 10;

        int flags = wxRICHTEXT_FIXED_WIDTH|wxRICHTEXT_VARIABLE_HEIGHT;
        if (onlyVisibleRect)
        {
            flags |= wxRICHTEXT_LAYOUT_SPECIFIED_RECT;
            availableSpace.SetPosition(GetUnscaledPoint(GetLogicalPoint(wxPoint(0, 0))));
        }

        wxClientDC dc(this);

        PrepareDC(dc);
        dc.SetFont(GetFont());
        dc.SetUserScale(GetScale(), GetScale());

        wxRichTextDrawingContext context(& GetBuffer());
        GetBuffer().Defragment(context);
        GetBuffer().UpdateRanges();     // If items were deleted, ranges need recalculation
        DoLayoutBuffer(GetBuffer(), dc, context, availableSpace, availableSpace, flags);
        GetBuffer().Invalidate(wxRICHTEXT_NONE);

        dc.SetUserScale(1.0, 1.0);

        if (!IsFrozen() && !onlyVisibleRect)
            SetupScrollbars();

        if (GetDelayedImageLoading())
            RequestDelayedImageProcessing();
    }

    return true;
}

void wxRichTextCtrl::DoLayoutBuffer(wxRichTextBuffer& buffer, wxDC& dc, wxRichTextDrawingContext& context, const wxRect& rect, const wxRect& parentRect, int flags)
{
    buffer.Layout(dc, context, rect, parentRect, flags);
}

/// Is all of the selection, or the current caret position, bold?
bool wxRichTextCtrl::IsSelectionBold()
{
    if (HasSelection())
    {
        wxRichTextAttr attr;
        wxRichTextRange range = GetSelectionRange();
        attr.SetFlags(wxTEXT_ATTR_FONT_WEIGHT);
        attr.SetFontWeight(wxFONTWEIGHT_BOLD);

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
            return attr.GetFontWeight() == wxFONTWEIGHT_BOLD;
        }
    }
    return false;
}

/// Is all of the selection, or the current caret position, italics?
bool wxRichTextCtrl::IsSelectionItalics()
{
    if (HasSelection())
    {
        wxRichTextRange range = GetSelectionRange();
        wxRichTextAttr attr;
        attr.SetFlags(wxTEXT_ATTR_FONT_ITALIC);
        attr.SetFontStyle(wxFONTSTYLE_ITALIC);

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
            return attr.GetFontStyle() == wxFONTSTYLE_ITALIC;
        }
    }
    return false;
}

/// Is all of the selection, or the current caret position, underlined?
bool wxRichTextCtrl::IsSelectionUnderlined()
{
    if (HasSelection())
    {
        wxRichTextRange range = GetSelectionRange();
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

/// Does all of the selection, or the current caret position, have this wxTextAttrEffects flag(s)?
bool wxRichTextCtrl::DoesSelectionHaveTextEffectFlag(int flag)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_EFFECTS);
    attr.SetTextEffectFlags(flag);
    attr.SetTextEffects(flag);

    if (HasSelection())
    {
        return HasCharacterAttributes(GetSelectionRange(), attr);
    }
    else
    {
        // If no selection, then we need to combine current style with default style
        // to see what the effect would be if we started typing.
        long pos = GetAdjustedCaretPosition(GetCaretPosition());
        if (GetStyle(pos, attr))
        {
            if (IsDefaultStyleShowing())
                wxRichTextApplyStyle(attr, GetDefaultStyleEx());
            return (attr.GetTextEffectFlags() & flag) != 0;
        }
    }
    return false;
}

/// Apply bold to the selection
bool wxRichTextCtrl::ApplyBoldToSelection()
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT_WEIGHT);
    attr.SetFontWeight(IsSelectionBold() ? wxFONTWEIGHT_NORMAL : wxFONTWEIGHT_BOLD);

    if (HasSelection())
        return SetStyleEx(GetSelectionRange(), attr, wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_OPTIMIZE|wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY);
    else
    {
        wxRichTextAttr current = GetDefaultStyleEx();
        current.Apply(attr);
        SetAndShowDefaultStyle(current);
    }
    return true;
}

/// Apply italic to the selection
bool wxRichTextCtrl::ApplyItalicToSelection()
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT_ITALIC);
    attr.SetFontStyle(IsSelectionItalics() ? wxFONTSTYLE_NORMAL : wxFONTSTYLE_ITALIC);

    if (HasSelection())
        return SetStyleEx(GetSelectionRange(), attr, wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_OPTIMIZE|wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY);
    else
    {
        wxRichTextAttr current = GetDefaultStyleEx();
        current.Apply(attr);
        SetAndShowDefaultStyle(current);
    }
    return true;
}

/// Apply underline to the selection
bool wxRichTextCtrl::ApplyUnderlineToSelection()
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT_UNDERLINE);
    attr.SetFontUnderlined(!IsSelectionUnderlined());

    if (HasSelection())
        return SetStyleEx(GetSelectionRange(), attr, wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_OPTIMIZE|wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY);
    else
    {
        wxRichTextAttr current = GetDefaultStyleEx();
        current.Apply(attr);
        SetAndShowDefaultStyle(current);
    }
    return true;
}

/// Apply the wxTextAttrEffects flag(s) to the selection, or the current caret position if there's no selection
bool wxRichTextCtrl::ApplyTextEffectToSelection(int flags)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_EFFECTS);
    attr.SetTextEffectFlags(flags);
    if (!DoesSelectionHaveTextEffectFlag(flags))
        attr.SetTextEffects(flags);
     else
        attr.SetTextEffects(attr.GetTextEffectFlags() & ~flags);

    if (HasSelection())
        return SetStyleEx(GetSelectionRange(), attr, wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_OPTIMIZE|wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY);
    else
    {
        wxRichTextAttr current = GetDefaultStyleEx();
        current.Apply(attr);
        SetAndShowDefaultStyle(current);
    }
    return true;
}

/// Is all of the selection aligned according to the specified flag?
bool wxRichTextCtrl::IsSelectionAligned(wxTextAttrAlignment alignment)
{
    wxRichTextRange range;
    if (HasSelection())
        range = GetSelectionRange();
    else
        range = wxRichTextRange(GetCaretPosition()+1, GetCaretPosition()+2);

    wxRichTextAttr attr;
    attr.SetAlignment(alignment);

    return HasParagraphAttributes(range, attr);
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
        wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(GetCaretPosition()+1);
        if (para)
            return SetStyleEx(para->GetRange().FromInternal(), attr, wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_OPTIMIZE|wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY);
    }
    return true;
}

/// Apply a named style to the selection
bool wxRichTextCtrl::ApplyStyle(wxRichTextStyleDefinition* def)
{
    // Flags are defined within each definition, so only certain
    // attributes are applied.
    wxRichTextAttr attr(GetStyleSheet() ? def->GetStyleMergedWithBase(GetStyleSheet()) : def->GetStyle());

    int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_OPTIMIZE|wxRICHTEXT_SETSTYLE_RESET;

    if (wxDynamicCast(def, wxRichTextListStyleDefinition))
    {
        flags |= wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY;

        wxRichTextRange range;

        if (HasSelection())
            range = GetSelectionRange();
        else
        {
            long pos = GetAdjustedCaretPosition(GetCaretPosition());
            range = wxRichTextRange(pos, pos+1);
        }

        return SetListStyle(range, (wxRichTextListStyleDefinition*) def, flags);
    }

    bool isPara = false;

    // Make sure the attr has the style name
    if (wxDynamicCast(def, wxRichTextParagraphStyleDefinition))
    {
        isPara = true;
        attr.SetParagraphStyleName(def->GetName());

        // If applying a paragraph style, we only want the paragraph nodes to adopt these
        // attributes, and not the leaf nodes. This will allow the content (e.g. text)
        // to change its style independently.
        flags |= wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY;
    }
    else if (wxDynamicCast(def, wxRichTextCharacterStyleDefinition))
        attr.SetCharacterStyleName(def->GetName());
    else if (wxDynamicCast(def, wxRichTextBoxStyleDefinition))
        attr.GetTextBoxAttr().SetBoxStyleName(def->GetName());

    if (wxDynamicCast(def, wxRichTextBoxStyleDefinition))
    {
        if (GetFocusObject() && (GetFocusObject() != & GetBuffer()))
        {
            SetStyle(GetFocusObject(), attr);
            return true;
        }
        else
            return false;
    }
    else if (HasSelection())
        return SetStyleEx(GetSelectionRange(), attr, flags);
    else
    {
        wxRichTextAttr current = GetDefaultStyleEx();
        wxRichTextAttr defaultStyle(attr);
        if (isPara)
        {
            // Don't apply extra character styles since they are already implied
            // in the paragraph style
            defaultStyle.SetFlags(defaultStyle.GetFlags() & ~wxTEXT_ATTR_CHARACTER);
        }
        current.Apply(defaultStyle);
        SetAndShowDefaultStyle(current);

        // If it's a paragraph style, we want to apply the style to the
        // current paragraph even if we didn't select any text.
        if (isPara)
        {
            long pos = GetAdjustedCaretPosition(GetCaretPosition());
            wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(pos);
            if (para)
            {
                return SetStyleEx(para->GetRange().FromInternal(), attr, flags);
            }
        }
        return true;
    }
}

/// Apply the style sheet to the buffer, for example if the styles have changed.
bool wxRichTextCtrl::ApplyStyleSheet(wxRichTextStyleSheet* styleSheet)
{
    if (!styleSheet)
        styleSheet = GetBuffer().GetStyleSheet();
    if (!styleSheet)
        return false;

    if (GetBuffer().ApplyStyleSheet(styleSheet))
    {
        GetBuffer().Invalidate(wxRICHTEXT_ALL);
        Refresh(false);
        return true;
    }
    else
        return false;
}

/// Sets the default style to the style under the cursor
bool wxRichTextCtrl::SetDefaultStyleToCursorStyle()
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_CHARACTER);

    // If at the start of a paragraph, use the next position.
    long pos = GetAdjustedCaretPosition(GetCaretPosition());

    wxRichTextObject* obj = GetFocusObject()->GetLeafObjectAtPosition(pos);
    if (obj && obj->IsTopLevel())
    {
        // Don't use the attributes of a top-level object, since they might apply
        // to content of the object, e.g. background colour.
        SetDefaultStyle(wxRichTextAttr());
        return true;
    }
    else if (GetUncombinedStyle(pos, attr))
    {
        SetDefaultStyle(attr);
        return true;
    }

    return false;
}

/// Returns the first visible position in the current view
long wxRichTextCtrl::GetFirstVisiblePosition() const
{
    wxRichTextLine* line = GetFocusObject()->GetLineAtYPosition(GetUnscaledPoint(GetLogicalPoint(wxPoint(0, 0))).y);
    if (line)
        return line->GetAbsoluteRange().GetStart();
    else
        return 0;
}

/// Get the first visible point in the window
wxPoint wxRichTextCtrl::GetFirstVisiblePoint() const
{
    int ppuX, ppuY;
    int startXUnits, startYUnits;

    GetScrollPixelsPerUnit(& ppuX, & ppuY);
    GetViewStart(& startXUnits, & startYUnits);

    return wxPoint(startXUnits * ppuX, startYUnits * ppuY);
}

/// The adjusted caret position is the character position adjusted to take
/// into account whether we're at the start of a paragraph, in which case
/// style information should be taken from the next position, not current one.
long wxRichTextCtrl::GetAdjustedCaretPosition(long caretPos) const
{
    wxRichTextParagraph* para = GetFocusObject()->GetParagraphAtPosition(caretPos+1);

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
    SetSelection(range.GetStart(), range.GetEnd());
}

/// Set list style
bool wxRichTextCtrl::SetListStyle(const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags, int startFrom, int specifiedLevel)
{
    return GetFocusObject()->SetListStyle(range.ToInternal(), def, flags, startFrom, specifiedLevel);
}

bool wxRichTextCtrl::SetListStyle(const wxRichTextRange& range, const wxString& defName, int flags, int startFrom, int specifiedLevel)
{
    return GetFocusObject()->SetListStyle(range.ToInternal(), defName, flags, startFrom, specifiedLevel);
}

/// Clear list for given range
bool wxRichTextCtrl::ClearListStyle(const wxRichTextRange& range, int flags)
{
    return GetFocusObject()->ClearListStyle(range.ToInternal(), flags);
}

/// Number/renumber any list elements in the given range
bool wxRichTextCtrl::NumberList(const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags, int startFrom, int specifiedLevel)
{
    return GetFocusObject()->NumberList(range.ToInternal(), def, flags, startFrom, specifiedLevel);
}

bool wxRichTextCtrl::NumberList(const wxRichTextRange& range, const wxString& defName, int flags, int startFrom, int specifiedLevel)
{
    return GetFocusObject()->NumberList(range.ToInternal(), defName, flags, startFrom, specifiedLevel);
}

/// Promote the list items within the given range. promoteBy can be a positive or negative number, e.g. 1 or -1
bool wxRichTextCtrl::PromoteList(int promoteBy, const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags, int specifiedLevel)
{
    return GetFocusObject()->PromoteList(promoteBy, range.ToInternal(), def, flags, specifiedLevel);
}

bool wxRichTextCtrl::PromoteList(int promoteBy, const wxRichTextRange& range, const wxString& defName, int flags, int specifiedLevel)
{
    return GetFocusObject()->PromoteList(promoteBy, range.ToInternal(), defName, flags, specifiedLevel);
}

// Given a character position at which there is a list style, find the range
// encompassing the same list style by looking backwards and forwards.
wxRichTextRange wxRichTextCtrl::FindRangeForList(long pos, bool& isNumberedList)
{
    wxRichTextParagraphLayoutBox* focusObject = GetFocusObject();
    wxRichTextRange range = wxRichTextRange(-1, -1);
    wxRichTextParagraph* para = focusObject->GetParagraphAtPosition(pos);
    if (!para || !para->GetAttributes().HasListStyleName())
        return range;
    else
    {
        wxString listStyle = para->GetAttributes().GetListStyleName();
        range = para->GetRange();

        isNumberedList = para->GetAttributes().HasBulletNumber();

        // Search back
        wxRichTextObjectList::compatibility_iterator initialNode = focusObject->GetChildren().Find(para);
        if (initialNode)
        {
            wxRichTextObjectList::compatibility_iterator startNode = initialNode->GetPrevious();
            while (startNode)
            {
                wxRichTextParagraph* p = wxDynamicCast(startNode->GetData(), wxRichTextParagraph);
                if (p)
                {
                    if (!p->GetAttributes().HasListStyleName() || p->GetAttributes().GetListStyleName() != listStyle)
                        break;
                    else
                        range.SetStart(p->GetRange().GetStart());
                }

                startNode = startNode->GetPrevious();
            }

            // Search forward
            wxRichTextObjectList::compatibility_iterator endNode = initialNode->GetNext();
            while (endNode)
            {
                wxRichTextParagraph* p = wxDynamicCast(endNode->GetData(), wxRichTextParagraph);
                if (p)
                {
                    if (!p->GetAttributes().HasListStyleName() || p->GetAttributes().GetListStyleName() != listStyle)
                        break;
                    else
                        range.SetEnd(p->GetRange().GetEnd());
                }

                endNode = endNode->GetNext();
            }
        }
    }
    return range;
}

/// Deletes the content in the given range
bool wxRichTextCtrl::Delete(const wxRichTextRange& range)
{
    return GetFocusObject()->DeleteRangeWithUndo(range.ToInternal(), this, & GetBuffer());
}

const wxArrayString& wxRichTextCtrl::GetAvailableFontNames()
{
    if (sm_availableFontNames.GetCount() == 0)
    {
        sm_availableFontNames = wxFontEnumerator::GetFacenames();
        sm_availableFontNames.Sort();
    }
    return sm_availableFontNames;
}

void wxRichTextCtrl::ClearAvailableFontNames()
{
    sm_availableFontNames.Clear();
}

void wxRichTextCtrl::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
    //wxLogDebug(wxT("wxRichTextCtrl::OnSysColourChanged"));

    wxTextAttrEx basicStyle = GetBasicStyle();
    basicStyle.SetTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    SetBasicStyle(basicStyle);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    Refresh();
}

// Refresh the area affected by a selection change
bool wxRichTextCtrl::RefreshForSelectionChange(const wxRichTextSelection& oldSelection, const wxRichTextSelection& newSelection)
{
    // If the selection is not part of the focus object, or we have multiple ranges, then the chances are that
    // the selection contains whole containers rather than just text, so refresh everything
    // for now as it would be hard to compute the rectangle bounding all selections.
    // TODO: improve on this.
    if ((oldSelection.IsValid() && (oldSelection.GetContainer() != GetFocusObject() || oldSelection.GetCount() > 1)) ||
        (newSelection.IsValid() && (newSelection.GetContainer() != GetFocusObject() || newSelection.GetCount() > 1)))
    {
        Refresh(false);
        return true;
    }

    wxRichTextRange oldRange, newRange;
    if (oldSelection.IsValid())
        oldRange = oldSelection.GetRange();
    else
        oldRange = wxRICHTEXT_NO_SELECTION;
    if (newSelection.IsValid())
        newRange = newSelection.GetRange();
    else
        newRange = wxRICHTEXT_NO_SELECTION;

    // Calculate the refresh rectangle - just the affected lines
    long firstPos, lastPos;
    if (oldRange.GetStart() == -2 && newRange.GetStart() != -2)
    {
        firstPos = newRange.GetStart();
        lastPos = newRange.GetEnd();
    }
    else if (oldRange.GetStart() != -2 && newRange.GetStart() == -2)
    {
        firstPos = oldRange.GetStart();
        lastPos = oldRange.GetEnd();
    }
    else if (oldRange.GetStart() == -2 && newRange.GetStart() == -2)
    {
        return false;
    }
    else
    {
        firstPos = wxMin(oldRange.GetStart(), newRange.GetStart());
        lastPos = wxMax(oldRange.GetEnd(), newRange.GetEnd());
    }

    wxRichTextLine* firstLine = GetFocusObject()->GetLineAtPosition(firstPos);
    wxRichTextLine* lastLine = GetFocusObject()->GetLineAtPosition(lastPos);

    if (firstLine && lastLine)
    {
        wxSize clientSize = GetClientSize();
        wxPoint pt1 = GetPhysicalPoint(GetScaledPoint(firstLine->GetAbsolutePosition()));
        wxPoint pt2 = GetPhysicalPoint(GetScaledPoint(lastLine->GetAbsolutePosition())) + wxPoint(0, (int) (0.5 + lastLine->GetSize().y * GetScale()));

        pt1.x = 0;
        pt1.y = wxMax(0, pt1.y);
        pt2.x = 0;
        pt2.y = wxMin(clientSize.y, pt2.y);

        // Take into account any floating objects within the selection
        if (wxRichTextBuffer::GetFloatingLayoutMode() && GetFocusObject()->GetFloatingObjectCount() > 0)
        {
            wxRichTextObjectList floatingObjects;
            GetFocusObject()->GetFloatingObjects(floatingObjects);
            wxRichTextObjectList::compatibility_iterator node = floatingObjects.GetFirst();
            while (node)
            {
                wxRichTextObject* obj = node->GetData();
                if (obj->GetRange().GetStart() >= firstPos && obj->GetRange().GetStart() <= lastPos)
                {
                    wxPoint pt1Obj = GetPhysicalPoint(GetScaledPoint(obj->GetPosition()));
                    wxPoint pt2Obj = GetPhysicalPoint(GetScaledPoint(obj->GetPosition())) + wxPoint(0, (int) (0.5 + obj->GetCachedSize().y * GetScale()));
                    pt1.y = wxMin(pt1.y, pt1Obj.y);
                    pt2.y = wxMax(pt2.y, pt2Obj.y);
                }
                node = node->GetNext();
            }
        }

        wxRect rect(pt1, wxSize(clientSize.x, pt2.y - pt1.y));
        RefreshRect(rect, false);
    }
    else
        Refresh(false);

    return true;
}

// Overrides standard refresh in order to provoke delayed image loading.
void wxRichTextCtrl::Refresh( bool eraseBackground, const wxRect *rect)
{
    if (GetDelayedImageLoading())
        RequestDelayedImageProcessing();

    wxWindow::Refresh(eraseBackground, rect);
}

// margins functions
bool wxRichTextCtrl::DoSetMargins(const wxPoint& pt)
{
    GetBuffer().GetAttributes().GetTextBoxAttr().GetMargins().GetLeft().SetValue(pt.x, wxTEXT_ATTR_UNITS_PIXELS);
    GetBuffer().GetAttributes().GetTextBoxAttr().GetMargins().GetRight().SetValue(pt.x, wxTEXT_ATTR_UNITS_PIXELS);
    GetBuffer().GetAttributes().GetTextBoxAttr().GetMargins().GetTop().SetValue(pt.y, wxTEXT_ATTR_UNITS_PIXELS);
    GetBuffer().GetAttributes().GetTextBoxAttr().GetMargins().GetBottom().SetValue(pt.y, wxTEXT_ATTR_UNITS_PIXELS);

    return true;
}

wxPoint wxRichTextCtrl::DoGetMargins() const
{
    return wxPoint(GetBuffer().GetAttributes().GetTextBoxAttr().GetMargins().GetLeft().GetValue(),
                   GetBuffer().GetAttributes().GetTextBoxAttr().GetMargins().GetTop().GetValue());
}

bool wxRichTextCtrl::SetFocusObject(wxRichTextParagraphLayoutBox* obj, bool setCaretPosition)
{
    if (obj && !obj->AcceptsFocus())
        return false;

    wxRichTextParagraphLayoutBox* oldContainer = GetFocusObject();
    bool changingContainer = (m_focusObject != obj);

    if (changingContainer && HasSelection())
        SelectNone();

    m_focusObject = obj;

    if (!obj)
        m_focusObject = & m_buffer;

    if (setCaretPosition && changingContainer)
    {
        m_selection.Reset();
        m_selectionAnchor = -2;
        m_selectionAnchorObject = nullptr;
        m_selectionState = wxRichTextCtrlSelectionState_Normal;

        long pos = -1;

        m_caretAtLineStart = false;
        MoveCaret(pos, m_caretAtLineStart);
        SetDefaultStyleToCursorStyle();

        wxRichTextEvent cmdEvent(
            wxEVT_RICHTEXT_FOCUS_OBJECT_CHANGED,
            GetId());
        cmdEvent.SetEventObject(this);
        cmdEvent.SetPosition(m_caretPosition+1);
        cmdEvent.SetOldContainer(oldContainer);
        cmdEvent.SetContainer(m_focusObject);

        GetEventHandler()->ProcessEvent(cmdEvent);
    }
    return true;
}

#if wxUSE_DRAG_AND_DROP
// Helper function for OnDrop. Returns true if the target is contained within source,
// and if it is, also returns the position relative to the source so we can properly check if it's
// within the current selection.
static bool wxRichTextCtrlIsContainedIn(wxRichTextParagraphLayoutBox* source, wxRichTextParagraphLayoutBox* target,
                                        long& position)
{
    wxRichTextObject* t = target;
    while (t)
    {
        if (t->GetParent() == source)
        {
            position = t->GetRange().GetStart();
            return true;
        }

        t = t->GetParent();
    }
    return false;
}

void wxRichTextCtrl::OnDrop(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def, wxDataObject* DataObj)
{
    m_preDrag = false;

    if ((def != wxDragCopy) && (def != wxDragMove))
    {
        return;
    }

    if (!GetSelection().IsValid())
    {
        return;
    }

    wxRichTextParagraphLayoutBox* originContainer = GetSelection().GetContainer();
    wxRichTextParagraphLayoutBox* destContainer = GetFocusObject(); // This will be the drop container, not necessarily the same as the origin one

    wxRichTextBuffer* richTextBuffer = ((wxRichTextBufferDataObject*)DataObj)->GetRichTextBuffer();
    if (richTextBuffer)
    {
        long position = GetCaretPosition();
        long positionRelativeToSource = position;
        wxRichTextRange selectionrange = GetInternalSelectionRange();
        if (def == wxDragMove)
        {
            // Are the containers the same, or is one contained within the other?
            if (((originContainer == destContainer) ||
                 wxRichTextCtrlIsContainedIn(originContainer, destContainer, positionRelativeToSource)) &&
                selectionrange.Contains(positionRelativeToSource))
            {
                // It doesn't make sense to move onto itself
                return;
            }
        }

        // If we're moving, and the data is being moved forward, we need to drop first, then delete the selection
        // If moving backwards, we need to delete then drop. If we're copying (or doing nothing) we don't delete anyway
        bool DeleteAfter = (def == wxDragMove) && (positionRelativeToSource > selectionrange.GetEnd());
        if ((def == wxDragMove) && !DeleteAfter)
        {
            // We can't use e.g. DeleteSelectedContent() as it uses the focus container
            originContainer->DeleteRangeWithUndo(selectionrange, this, &GetBuffer());
        }

        destContainer->InsertParagraphsWithUndo(&GetBuffer(), position+1, *richTextBuffer, this, 0);
        ShowPosition(position + richTextBuffer->GetOwnRange().GetEnd());

        delete richTextBuffer;

        if (DeleteAfter)
        {
            // We can't use e.g. DeleteSelectedContent() as it uses the focus container
            originContainer->DeleteRangeWithUndo(selectionrange, this, &GetBuffer());
        }

        SelectNone();
        Refresh();
    }
}
#endif // wxUSE_DRAG_AND_DROP


#if wxUSE_DRAG_AND_DROP
bool wxRichTextDropSource::GiveFeedback(wxDragResult WXUNUSED(effect))
{
    wxCHECK_MSG(m_rtc, false, wxT("null m_rtc"));

    long position = 0;
    int hit = 0;
    wxRichTextObject* hitObj = nullptr;
    wxRichTextParagraphLayoutBox* container = m_rtc->FindContainerAtPoint(m_rtc->GetUnscaledPoint(m_rtc->ScreenToClient(wxGetMousePosition())), position, hit, hitObj);

    if (!(hit & wxRICHTEXT_HITTEST_NONE) && container && container->AcceptsFocus())
    {
        m_rtc->StoreFocusObject(container);
        m_rtc->SetCaretPositionAfterClick(container, position, hit);
    }

    return false;  // so that the base-class sets a cursor
}
#endif // wxUSE_DRAG_AND_DROP

bool wxRichTextCtrl::CanDeleteRange(wxRichTextParagraphLayoutBox& WXUNUSED(container), const wxRichTextRange& WXUNUSED(range)) const
{
    return true;
}

bool wxRichTextCtrl::CanInsertContent(wxRichTextParagraphLayoutBox& WXUNUSED(container), long WXUNUSED(pos)) const
{
    return true;
}

void wxRichTextCtrl::EnableVerticalScrollbar(bool enable)
{
    m_verticalScrollbarEnabled = enable;
    SetupScrollbars();
}

void wxRichTextCtrl::SetFontScale(double fontScale, bool refresh)
{
    GetBuffer().SetFontScale(fontScale);
    if (refresh)
    {
        GetBuffer().Invalidate(wxRICHTEXT_ALL);
        Refresh();
    }
}

void wxRichTextCtrl::SetDimensionScale(double dimScale, bool refresh)
{
    GetBuffer().SetDimensionScale(dimScale);
    if (refresh)
    {
        GetBuffer().Invalidate(wxRICHTEXT_ALL);
        Refresh();
    }
}

// Sets an overall scale factor for displaying and editing the content.
void wxRichTextCtrl::SetScale(double scale, bool refresh)
{
    m_scale = scale;
    if (refresh)
    {
        GetBuffer().Invalidate(wxRICHTEXT_ALL);
        Refresh();
    }
}

// Get an unscaled point
wxPoint wxRichTextCtrl::GetUnscaledPoint(const wxPoint& pt) const
{
    if (GetScale() == 1.0)
        return pt;
    else
        return wxPoint((int) (0.5 + double(pt.x) / GetScale()), (int) (0.5 + double(pt.y) / GetScale()));
}

// Get a scaled point
wxPoint wxRichTextCtrl::GetScaledPoint(const wxPoint& pt) const
{
    if (GetScale() == 1.0)
        return pt;
    else
        return wxPoint((int) (0.5 + double(pt.x) * GetScale()), (int) (0.5 + double(pt.y) * GetScale()));
}

// Get an unscaled size
wxSize wxRichTextCtrl::GetUnscaledSize(const wxSize& sz) const
{
    if (GetScale() == 1.0)
        return sz;
    else
        return wxSize((int) (0.5 + double(sz.x) / GetScale()), (int) (0.5 + double(sz.y) / GetScale()));
}

// Get a scaled size
wxSize wxRichTextCtrl::GetScaledSize(const wxSize& sz) const
{
    if (GetScale() == 1.0)
        return sz;
    else
        return wxSize((int) (0.5 + double(sz.x) * GetScale()), (int) (0.5 + double(sz.y) * GetScale()));
}

// Get an unscaled rect
wxRect wxRichTextCtrl::GetUnscaledRect(const wxRect& rect) const
{
    if (GetScale() == 1.0)
        return rect;
    else
        return wxRect((int) (0.5 + double(rect.x) / GetScale()), (int) (0.5 + double(rect.y) / GetScale()),
                      (int) (0.5 + double(rect.width) / GetScale()), (int) (0.5 + double(rect.height) / GetScale()));
}

// Get a scaled rect
wxRect wxRichTextCtrl::GetScaledRect(const wxRect& rect) const
{
    if (GetScale() == 1.0)
        return rect;
    else
        return wxRect((int) (0.5 + double(rect.x) * GetScale()), (int) (0.5 + double(rect.y) * GetScale()),
                      (int) (0.5 + double(rect.width) * GetScale()), (int) (0.5 + double(rect.height) * GetScale()));
}

// Do delayed image loading and garbage-collect other images
bool wxRichTextCtrl::ProcessDelayedImageLoading(bool refresh)
{
    int loadCount = 0;

    wxSize clientSize = GetUnscaledSize(GetClientSize());
    wxPoint firstVisiblePt = GetUnscaledPoint(GetFirstVisiblePoint());
    wxRect screenRect(firstVisiblePt, clientSize);

    // Expand screen rect so that we actually process images in the vicinity,
    // for smoother paging and scrolling.
    screenRect.y -= (clientSize.y*3);
    screenRect.height += (clientSize.y*6);
    ProcessDelayedImageLoading(screenRect, & GetBuffer(), loadCount);

    if (loadCount > 0 && refresh)
    {
        wxWindow::Refresh(false);
    }

    return loadCount > 0;
}

bool wxRichTextCtrl::ProcessDelayedImageLoading(const wxRect& screenRect, wxRichTextParagraphLayoutBox* box, int& loadCount)
{
    if (!box || !box->IsShown())
        return true;

    wxRichTextObjectList::compatibility_iterator node = box->GetChildren().GetFirst();
    while (node)
    {
        // Could be a cell or a paragraph
        wxRichTextCompositeObject* composite = wxDynamicCast(node->GetData(), wxRichTextCompositeObject);
        if (composite->IsTopLevel())
            ProcessDelayedImageLoading(screenRect, wxDynamicCast(composite, wxRichTextParagraphLayoutBox), loadCount);
        else // assume a paragraph
        {
            wxRichTextObjectList::compatibility_iterator node2 = composite->GetChildren().GetFirst();
            while (node2)
            {
                wxRichTextObject* obj = node2->GetData();
                if (obj->IsTopLevel())
                    ProcessDelayedImageLoading(screenRect, wxDynamicCast(obj, wxRichTextParagraphLayoutBox), loadCount);
                else
                {
                    wxRichTextImage* imageObj = wxDynamicCast(obj, wxRichTextImage);
                    if (imageObj && imageObj->IsShown())
                    {
                        const wxRect& rect(imageObj->GetRect());
                        if ((rect.GetBottom() < screenRect.GetTop()) || (rect.GetTop() > screenRect.GetBottom()))
                        {
                            // Off-screen
                            imageObj->ResetImageCache();
                        }
                        else
                        {
                            // On-screen
                            wxRichTextDrawingContext context(& GetBuffer());
                            context.SetLayingOut(true);
                            context.EnableDelayedImageLoading(false);

                            wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
                            marginRect = imageObj->GetRect(); // outer rectangle, will calculate contentRect
                            if (marginRect.GetSize() != wxDefaultSize)
                            {
                                wxClientDC dc(this);
                                wxRichTextAttr attr(imageObj->GetAttributes());
                                imageObj->AdjustAttributes(attr, context);
                                imageObj->GetBoxRects(dc, & GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);

                                wxImage image;
                                bool changed = false;
                                if (imageObj->LoadAndScaleImageCache(image, contentRect.GetSize(), context, changed) && changed)
                                {
                                    loadCount ++;
                                }
                            }
                        }
                    }
                }
                node2 = node2->GetNext();
            }
        }

        node = node->GetNext();
    }

    return true;
}

void wxRichTextCtrl::RequestDelayedImageProcessing()
{
    SetDelayedImageProcessingRequired(true);
    SetDelayedImageProcessingTime(wxGetLocalTimeMillis());
    m_delayedImageProcessingTimer.SetOwner(this, GetId());
    m_delayedImageProcessingTimer.Start(wxRICHTEXT_DEFAULT_DELAYED_IMAGE_PROCESSING_INTERVAL);
}

void wxRichTextCtrl::OnTimer(wxTimerEvent& event)
{
    if (event.GetId() == GetId())
        wxWakeUpIdle();
    else
        event.Skip();
}

#if wxRICHTEXT_USE_OWN_CARET

// ----------------------------------------------------------------------------
// initialization and destruction
// ----------------------------------------------------------------------------

void wxRichTextCaret::Init()
{
    m_hasFocus = true;
    m_refreshEnabled = true;

    m_xOld =
    m_yOld = -1;
    m_richTextCtrl = nullptr;
    m_needsUpdate = false;
    m_flashOn = true;
}

wxRichTextCaret::~wxRichTextCaret()
{
    if (m_timer.IsRunning())
        m_timer.Stop();
}

// ----------------------------------------------------------------------------
// showing/hiding/moving the caret (base class interface)
// ----------------------------------------------------------------------------

void wxRichTextCaret::DoShow()
{
    m_flashOn = true;

    if (!m_timer.IsRunning() && GetBlinkTime() > 0)
        m_timer.Start(GetBlinkTime());

    Refresh();
}

void wxRichTextCaret::DoHide()
{
    if (m_timer.IsRunning())
        m_timer.Stop();

    Refresh();
}

void wxRichTextCaret::DoMove()
{
    if (IsVisible())
    {
        Refresh();

        if (m_xOld != -1 && m_yOld != -1)
        {
            if (m_richTextCtrl && m_refreshEnabled)
            {
                wxRect rect(wxPoint(m_xOld, m_yOld), GetSize());
                wxRect scaledRect = m_richTextCtrl->GetScaledRect(rect);
                m_richTextCtrl->RefreshRect(scaledRect, false);
            }
        }
    }

    m_xOld = m_x;
    m_yOld = m_y;
}

void wxRichTextCaret::DoSize()
{
    int countVisible = m_countVisible;
    if (countVisible > 0)
    {
        m_countVisible = 0;
        DoHide();

        m_countVisible = countVisible;
        DoShow();
    }
}

// ----------------------------------------------------------------------------
// handling the focus
// ----------------------------------------------------------------------------

void wxRichTextCaret::OnSetFocus()
{
    m_hasFocus = true;

    if ( IsVisible() )
        Refresh();
}

void wxRichTextCaret::OnKillFocus()
{
    m_hasFocus = false;
}

// ----------------------------------------------------------------------------
// drawing the caret
// ----------------------------------------------------------------------------

void wxRichTextCaret::Refresh()
{
    if (m_richTextCtrl && m_refreshEnabled)
    {
        wxRect rect(GetPosition(), GetSize());
        wxRect rectScaled = m_richTextCtrl->GetScaledRect(rect);
        m_richTextCtrl->RefreshRect(rectScaled, false);
    }
}

void wxRichTextCaret::DoDraw(wxDC *dc)
{
    wxBrush brush(m_caretBrush);
    wxPen pen(m_caretPen);
    if (m_richTextCtrl && m_richTextCtrl->GetBasicStyle().HasTextColour())
    {
        brush = wxBrush(m_richTextCtrl->GetBasicStyle().GetTextColour());
        pen = wxPen(m_richTextCtrl->GetBasicStyle().GetTextColour());
    }
    dc->SetBrush((m_hasFocus ? brush : *wxTRANSPARENT_BRUSH));
    dc->SetPen(pen);

    wxPoint pt(m_x, m_y);

    if (m_richTextCtrl)
    {
        pt = m_richTextCtrl->GetLogicalPoint(pt);
    }
    if (IsVisible() && m_flashOn)
        dc->DrawRectangle(pt.x, pt.y, m_width, m_height);
}

void wxRichTextCaret::Notify()
{
#ifdef __WXMAC__
    // Workaround for lack of kill focus event in wxOSX
    if (m_richTextCtrl && !m_richTextCtrl->HasFocus())
    {
        if (IsVisible())
            Hide();
        return;
    }
#endif

    m_flashOn = !m_flashOn;
    Refresh();
}

void wxRichTextCaretTimer::Notify()
{
    m_caret->Notify();
}
#endif
    // wxRICHTEXT_USE_OWN_CARET

// Add an item
bool wxRichTextContextMenuPropertiesInfo::AddItem(const wxString& label, wxRichTextObject* obj)
{
    if (GetCount() < 3)
    {
        m_labels.Add(label);
        m_objects.Add(obj);
        return true;
    }
    else
        return false;
}

// Returns number of menu items were added.
int wxRichTextContextMenuPropertiesInfo::AddMenuItems(wxMenu* menu, int startCmd) const
{
    wxMenuItem* item = menu->FindItem(startCmd);
    // If none of the standard properties identifiers are in the menu, add them if necessary.
    // If no items to add, just set the text to something generic
    if (GetCount() == 0)
    {
        if (item)
        {
            menu->SetLabel(startCmd, _("&Properties"));

            // Delete the others if necessary
            int i;
            for (i = startCmd+1; i < startCmd+3; i++)
            {
                if (menu->FindItem(i))
                {
                    menu->Delete(i);
                }
            }
        }
    }
    else
    {
        int i;
        int pos = -1;
        // Find the position of the first properties item
        for (i = 0; i < (int) menu->GetMenuItemCount(); i++)
        {
            wxMenuItem* searchItem = menu->FindItemByPosition(i);
            if (searchItem && searchItem->GetId() == startCmd)
            {
                pos = i;
                break;
            }
        }

        if (pos != -1)
        {
            int insertBefore = pos+1;
            for (i = startCmd; i < startCmd+GetCount(); i++)
            {
                if (menu->FindItem(i))
                {
                    menu->SetLabel(i, m_labels[i - startCmd]);
                }
                else
                {
                    if (insertBefore >= (int) menu->GetMenuItemCount())
                        menu->Append(i, m_labels[i - startCmd]);
                    else
                        menu->Insert(insertBefore, i, m_labels[i - startCmd]);
                }
                insertBefore ++;
            }

            // Delete any old items still left on the menu
            for (i = startCmd + GetCount(); i < startCmd+3; i++)
            {
                if (menu->FindItem(i))
                {
                    menu->Delete(i);
                }
            }
        }
        else
        {
            // No existing property identifiers were found, so append to the end of the menu.
            menu->AppendSeparator();
            for (i = startCmd; i < startCmd+GetCount(); i++)
            {
                menu->Append(i, m_labels[i - startCmd]);
            }
        }
    }

    return GetCount();
}

// Add appropriate menu items for the current container and clicked on object
// (and container's parent, if appropriate).
int wxRichTextContextMenuPropertiesInfo::AddItems(wxRichTextCtrl* ctrl, wxRichTextObject* container, wxRichTextObject* obj)
{
    Clear();
    if (obj && ctrl->CanEditProperties(obj))
        AddItem(ctrl->GetPropertiesMenuLabel(obj), obj);

    if (container && container != obj && ctrl->CanEditProperties(container) && m_labels.Index(ctrl->GetPropertiesMenuLabel(container)) == wxNOT_FOUND)
        AddItem(ctrl->GetPropertiesMenuLabel(container), container);

    if (container && container->GetParent() && ctrl->CanEditProperties(container->GetParent()) && m_labels.Index(ctrl->GetPropertiesMenuLabel(container->GetParent())) == wxNOT_FOUND)
        AddItem(ctrl->GetPropertiesMenuLabel(container->GetParent()), container->GetParent());

    return GetCount();
}

#endif
    // wxUSE_RICHTEXT
