////////////////////////////////////////////////////////////////////////////
// Name:        ScintillaWX.cxx
// Purpose:     A wxWindows implementation of Scintilla.  A class derived
//              from ScintillaBase that uses the "wx platform" defined in
//              PlatformWX.cxx  This class is one end of a bridge between
//              the wx world and the Scintilla world.  It needs a peer
//              object of type wxStyledTextCtrl to function.
//
// Author:      Robin Dunn
//
// Created:     13-Jan-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


#include "ScintillaWX.h"
#include "wx/stc/stc.h"
#include "PlatWX.h"

//----------------------------------------------------------------------
// Helper classes

class wxSTCTimer : public wxTimer {
public:
    wxSTCTimer(ScintillaWX* swx) {
        this->swx = swx;
    }

    void Notify() {
        swx->DoTick();
    }

private:
    ScintillaWX* swx;
};


#if wxUSE_DRAG_AND_DROP
bool wxSTCDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {
    return swx->DoDropText(x, y, data);
}

wxDragResult  wxSTCDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult def) {
    return swx->DoDragEnter(x, y, def);
}

wxDragResult  wxSTCDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def) {
    return swx->DoDragOver(x, y, def);
}

void  wxSTCDropTarget::OnLeave() {
    swx->DoDragLeave();
}
#endif


#ifdef __WXGTK__
#undef wxSTC_USE_POPUP
#define wxSTC_USE_POPUP 0
#endif

#if wxUSE_POPUPWIN && wxSTC_USE_POPUP
#include <wx/popupwin.h>
#define wxSTCCallTipBase wxPopupWindow
#define param2  wxBORDER_NONE  // popup's 2nd param is flags
#else
#define wxSTCCallTipBase wxWindow
#define param2 -1 // wxWindow's 2nd param is ID
#endif

class wxSTCCallTip : public wxSTCCallTipBase {
public:
    wxSTCCallTip(wxWindow* parent, CallTip* ct, ScintillaWX* swx)
        : wxSTCCallTipBase(parent, param2),
          m_ct(ct), m_swx(swx)
        {
        }

    ~wxSTCCallTip() {
    }

    bool AcceptsFocus() const { return FALSE; }

    void OnPaint(wxPaintEvent& evt) {
        wxPaintDC dc(this);
        Surface* surfaceWindow = Surface::Allocate();
        surfaceWindow->Init(&dc, m_ct->wDraw.GetID());
        m_ct->PaintCT(surfaceWindow);
        surfaceWindow->Release();
        delete surfaceWindow;
    }

    void OnFocus(wxFocusEvent& event) {
        GetParent()->SetFocus();
        event.Skip();
    }

    void OnLeftDown(wxMouseEvent& event) {
        wxPoint pt = event.GetPosition();
        Point p(pt.x, pt.y);
        m_ct->MouseClick(p);
        m_swx->CallTipClick();
    }

#if wxUSE_POPUPWIN && wxSTC_USE_POPUP
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) {
        if (x != -1)
            GetParent()->ClientToScreen(&x, NULL);
        if (y != -1)
            GetParent()->ClientToScreen(NULL, &y);
        wxSTCCallTipBase::DoSetSize(x, y, width, height, sizeFlags);
    }
#endif

private:
    CallTip*      m_ct;
    ScintillaWX*  m_swx;
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSTCCallTip, wxSTCCallTipBase)
    EVT_PAINT(wxSTCCallTip::OnPaint)
    EVT_SET_FOCUS(wxSTCCallTip::OnFocus)
    EVT_LEFT_DOWN(wxSTCCallTip::OnLeftDown)
END_EVENT_TABLE()


//----------------------------------------------------------------------
// Constructor/Destructor


ScintillaWX::ScintillaWX(wxStyledTextCtrl* win) {
    capturedMouse = false;
    wMain = win;
    stc   = win;
    wheelRotation = 0;
    Initialise();
}


ScintillaWX::~ScintillaWX() {
    SetTicking(false);
}

//----------------------------------------------------------------------
// base class virtuals


void ScintillaWX::Initialise() {
    //ScintillaBase::Initialise();
#if wxUSE_DRAG_AND_DROP
    dropTarget = new wxSTCDropTarget;
    dropTarget->SetScintilla(this);
    stc->SetDropTarget(dropTarget);
#endif
}


void ScintillaWX::Finalise() {
    ScintillaBase::Finalise();
}


void ScintillaWX::StartDrag() {
#if wxUSE_DRAG_AND_DROP
    wxString dragText = stc2wx(drag.s, drag.len);

    // Send an event to allow the drag text to be changed
    wxStyledTextEvent evt(wxEVT_STC_START_DRAG, stc->GetId());
    evt.SetEventObject(stc);
    evt.SetDragText(dragText);
    evt.SetDragAllowMove(TRUE);
    evt.SetPosition(wxMin(stc->GetSelectionStart(),
                          stc->GetSelectionEnd()));
    stc->GetEventHandler()->ProcessEvent(evt);
    dragText = evt.GetDragText();

    if (dragText.Length()) {
        wxDropSource        source(stc);
        wxTextDataObject    data(dragText);
        wxDragResult        result;

        source.SetData(data);
        dropWentOutside = TRUE;
        result = source.DoDragDrop(evt.GetDragAllowMove());
        if (result == wxDragMove && dropWentOutside)
            ClearSelection();
        inDragDrop = FALSE;
        SetDragPosition(invalidPosition);
    }
#endif
}


void ScintillaWX::SetTicking(bool on) {
    wxSTCTimer* steTimer;
    if (timer.ticking != on) {
        timer.ticking = on;
        if (timer.ticking) {
            steTimer = new wxSTCTimer(this);
            steTimer->Start(timer.tickSize);
            timer.tickerID = steTimer;
        } else {
            steTimer = (wxSTCTimer*)timer.tickerID;
            steTimer->Stop();
            delete steTimer;
            timer.tickerID = 0;
        }
    }
    timer.ticksToWait = caret.period;
}


void ScintillaWX::SetMouseCapture(bool on) {
    if (on && !capturedMouse)
        stc->CaptureMouse();
    else if (!on && capturedMouse && stc->HasCapture())
        stc->ReleaseMouse();
    capturedMouse = on;
}


bool ScintillaWX::HaveMouseCapture() {
    return capturedMouse;
}


void ScintillaWX::ScrollText(int linesToMove) {
    int dy = vs.lineHeight * (linesToMove);
    stc->ScrollWindow(0, dy);
    stc->Update();
}

void ScintillaWX::SetVerticalScrollPos() {
    if (stc->m_vScrollBar == NULL) {  // Use built-in scrollbar
        stc->SetScrollPos(wxVERTICAL, topLine);
    }
    else { // otherwise use the one that's been given to us
        stc->m_vScrollBar->SetThumbPosition(topLine);
    }
}

void ScintillaWX::SetHorizontalScrollPos() {
    if (stc->m_hScrollBar == NULL) {  // Use built-in scrollbar
        stc->SetScrollPos(wxHORIZONTAL, xOffset);
    }
    else { // otherwise use the one that's been given to us
        stc->m_hScrollBar->SetThumbPosition(xOffset);
    }
}

const int H_SCROLL_STEP = 20;

bool ScintillaWX::ModifyScrollBars(int nMax, int nPage) {
    bool modified = false;

    // Check the vertical scrollbar
    if (stc->m_vScrollBar == NULL) {  // Use built-in scrollbar
        int  sbMax    = stc->GetScrollRange(wxVERTICAL);
        int  sbThumb  = stc->GetScrollThumb(wxVERTICAL);
        int  sbPos    = stc->GetScrollPos(wxVERTICAL);
        if (sbMax != nMax || sbThumb != nPage) {
            stc->SetScrollbar(wxVERTICAL, sbPos, nPage, nMax+1);
            modified = true;
        }
    }
    else { // otherwise use the one that's been given to us
        int  sbMax    = stc->m_vScrollBar->GetRange();
        int  sbPage   = stc->m_vScrollBar->GetPageSize();
        int  sbPos    = stc->m_vScrollBar->GetThumbPosition();
        if (sbMax != nMax || sbPage != nPage) {
            stc->m_vScrollBar->SetScrollbar(sbPos, nPage, nMax+1, nPage);
            modified = true;
        }
    }


    // Check the horizontal scrollbar
    PRectangle rcText = GetTextRectangle();
    int horizEnd = scrollWidth;
    if (horizEnd < 0)
        horizEnd = 0;
    if (!horizontalScrollBarVisible || (wrapState != eWrapNone))
        horizEnd = 0;
    int pageWidth = rcText.Width();

    if (stc->m_hScrollBar == NULL) {  // Use built-in scrollbar
        int sbMax    = stc->GetScrollRange(wxHORIZONTAL);
        int sbThumb  = stc->GetScrollThumb(wxHORIZONTAL);
        int sbPos    = stc->GetScrollPos(wxHORIZONTAL);
        if ((sbMax != horizEnd) || (sbThumb != pageWidth) || (sbPos != 0)) {
            stc->SetScrollbar(wxHORIZONTAL, 0, pageWidth, horizEnd);
            modified = true;
            if (scrollWidth < pageWidth) {
                HorizontalScrollTo(0);
            }
        }
    }
    else { // otherwise use the one that's been given to us
        int sbMax    = stc->m_hScrollBar->GetRange();
        int sbThumb  = stc->m_hScrollBar->GetPageSize();
        int sbPos    = stc->m_hScrollBar->GetThumbPosition();
        if ((sbMax != horizEnd) || (sbThumb != pageWidth) || (sbPos != 0)) {
            stc->m_hScrollBar->SetScrollbar(0, pageWidth, horizEnd, pageWidth);
            modified = true;
            if (scrollWidth < pageWidth) {
                HorizontalScrollTo(0);
            }
        }
    }

    return modified;
}


void ScintillaWX::NotifyChange() {
    stc->NotifyChange();
}


void ScintillaWX::NotifyParent(SCNotification scn) {
    stc->NotifyParent(&scn);
}



void ScintillaWX::Copy() {
    if (currentPos != anchor) {
        SelectionText st;
        CopySelectionRange(&st);
        if (wxTheClipboard->Open()) {
            wxTheClipboard->UsePrimarySelection(FALSE);
            wxString text = stc2wx(st.s, st.len);
            wxTheClipboard->SetData(new wxTextDataObject(text));
            wxTheClipboard->Close();
        }
    }
}


void ScintillaWX::Paste() {
    pdoc->BeginUndoAction();
    ClearSelection();

    wxTextDataObject data;
    bool gotData = FALSE;

    if (wxTheClipboard->Open()) {
        wxTheClipboard->UsePrimarySelection(FALSE);
        gotData = wxTheClipboard->GetData(data);
        wxTheClipboard->Close();
    }
    if (gotData) {
        wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(data.GetText());
        int        len = strlen(buf);
        pdoc->InsertString(currentPos, buf, len);
        SetEmptySelection(currentPos + len);
    }

    pdoc->EndUndoAction();
    NotifyChange();
    Redraw();
}


bool ScintillaWX::CanPaste() {
    bool canPaste = FALSE;
    bool didOpen;

    if ( (didOpen = !wxTheClipboard->IsOpened()) )
        wxTheClipboard->Open();

    if (wxTheClipboard->IsOpened()) {
        wxTheClipboard->UsePrimarySelection(FALSE);
        canPaste = wxTheClipboard->IsSupported(wxUSE_UNICODE ? wxDF_UNICODETEXT : wxDF_TEXT);
        if (didOpen)
            wxTheClipboard->Close();
    }
    return canPaste;
}

void ScintillaWX::CreateCallTipWindow(PRectangle) {
    if (! ct.wCallTip.Created() ) {
        ct.wCallTip = new wxSTCCallTip(stc, &ct, this);
        ct.wDraw = ct.wCallTip;
    }
}


void ScintillaWX::AddToPopUp(const char *label, int cmd, bool enabled) {
    if (!label[0])
        ((wxMenu*)popup.GetID())->AppendSeparator();
    else
        ((wxMenu*)popup.GetID())->Append(cmd, stc2wx(label));

    if (!enabled)
        ((wxMenu*)popup.GetID())->Enable(cmd, enabled);
}


// This is called by the Editor base class whenever something is selected
void ScintillaWX::ClaimSelection() {
#if 0
    // Until wxGTK is able to support using both the primary selection and the
    // clipboard at the same time I think it causes more problems than it is
    // worth to implement this method.  Selecting text should not clear the
    // clipboard.  --Robin
#ifdef __WXGTK__
    // Put the selected text in the PRIMARY selection
    if (currentPos != anchor) {
        SelectionText st;
        CopySelectionRange(&st);
        if (wxTheClipboard->Open()) {
            wxTheClipboard->UsePrimarySelection(TRUE);
            wxString text = stc2wx(st.s, st.len);
            wxTheClipboard->SetData(new wxTextDataObject(text));
            wxTheClipboard->UsePrimarySelection(FALSE);
            wxTheClipboard->Close();
        }
    }
#endif
#endif
}


long ScintillaWX::DefWndProc(unsigned int /*iMessage*/, unsigned long /*wParam*/, long /*lParam*/) {
    return 0;
}

long ScintillaWX::WndProc(unsigned int iMessage, unsigned long wParam, long lParam) {
      switch (iMessage) {
      case SCI_CALLTIPSHOW: {
          // NOTE: This is copied here from scintilla/src/ScintillaBase.cxx
          // because of the little tweak that needs done below for wxGTK.
          // When updating new versions double check that this is still
          // needed, and that any new code there is copied here too.
          Point pt = LocationFromPosition(wParam);
          char* defn = reinterpret_cast<char *>(lParam);
          AutoCompleteCancel();
          pt.y += vs.lineHeight;
          PRectangle rc = ct.CallTipStart(currentPos, pt,
                                          defn,
                                          vs.styles[STYLE_DEFAULT].fontName,
                                          vs.styles[STYLE_DEFAULT].sizeZoomed,
                                          IsUnicodeMode(),
                                          wMain);
          // If the call-tip window would be out of the client
          // space, adjust so it displays above the text.
          PRectangle rcClient = GetClientRectangle();
          if (rc.bottom > rcClient.bottom) {
#ifdef __WXGTK__
              int offset = int(vs.lineHeight * 1.25)  + rc.Height();
#else
              int offset = vs.lineHeight + rc.Height();
#endif
              rc.top -= offset;
              rc.bottom -= offset;
          }
          // Now display the window.
          CreateCallTipWindow(rc);
          ct.wCallTip.SetPositionRelative(rc, wMain);
          ct.wCallTip.Show();
          break;
      }

      default:
          return ScintillaBase::WndProc(iMessage, wParam, lParam);
      }
      return 0;
}



//----------------------------------------------------------------------
// Event delegates

void ScintillaWX::DoPaint(wxDC* dc, wxRect rect) {

    paintState = painting;
    Surface* surfaceWindow = Surface::Allocate();
    surfaceWindow->Init(dc, wMain.GetID());
    rcPaint = PRectangleFromwxRect(rect);
    PRectangle rcClient = GetClientRectangle();
    paintingAllText = rcPaint.Contains(rcClient);

    dc->BeginDrawing();
    ClipChildren(*dc, rcPaint);
    Paint(surfaceWindow, rcPaint);
    dc->EndDrawing();

    delete surfaceWindow;
    if (paintState == paintAbandoned) {
        // Painting area was insufficient to cover new styling or brace highlight positions
        FullPaint();
    }
    paintState = notPainting;
}


void ScintillaWX::DoHScroll(int type, int pos) {
    int xPos = xOffset;
    PRectangle rcText = GetTextRectangle();
    int pageWidth = rcText.Width() * 2 / 3;
    if (type == wxEVT_SCROLLWIN_LINEUP || type == wxEVT_SCROLL_LINEUP)
        xPos -= H_SCROLL_STEP;
    else if (type == wxEVT_SCROLLWIN_LINEDOWN || type == wxEVT_SCROLL_LINEDOWN)
        xPos += H_SCROLL_STEP;
    else if (type == wxEVT_SCROLLWIN_PAGEUP || type == wxEVT_SCROLL_PAGEUP)
        xPos -= pageWidth;
    else if (type == wxEVT_SCROLLWIN_PAGEDOWN || type == wxEVT_SCROLL_PAGEDOWN) {
        xPos += pageWidth;
        if (xPos > scrollWidth - rcText.Width()) {
            xPos = scrollWidth - rcText.Width();
        }
    }
    else if (type == wxEVT_SCROLLWIN_TOP || type == wxEVT_SCROLL_TOP)
        xPos = 0;
    else if (type == wxEVT_SCROLLWIN_BOTTOM || type == wxEVT_SCROLL_BOTTOM)
        xPos = scrollWidth;
    else if (type == wxEVT_SCROLLWIN_THUMBTRACK || type == wxEVT_SCROLL_THUMBTRACK)
        xPos = pos;

    HorizontalScrollTo(xPos);
}

void ScintillaWX::DoVScroll(int type, int pos) {
    int topLineNew = topLine;
    if (type == wxEVT_SCROLLWIN_LINEUP || type == wxEVT_SCROLL_LINEUP)
        topLineNew -= 1;
    else if (type == wxEVT_SCROLLWIN_LINEDOWN || type == wxEVT_SCROLL_LINEDOWN)
        topLineNew += 1;
    else if (type ==  wxEVT_SCROLLWIN_PAGEUP || type == wxEVT_SCROLL_PAGEUP)
        topLineNew -= LinesToScroll();
    else if (type ==  wxEVT_SCROLLWIN_PAGEDOWN || type == wxEVT_SCROLL_PAGEDOWN)
        topLineNew += LinesToScroll();
    else if (type ==  wxEVT_SCROLLWIN_TOP || type == wxEVT_SCROLL_TOP)
        topLineNew = 0;
    else if (type ==  wxEVT_SCROLLWIN_BOTTOM || type == wxEVT_SCROLL_BOTTOM)
        topLineNew = MaxScrollPos();
    else if (type ==   wxEVT_SCROLLWIN_THUMBTRACK || type == wxEVT_SCROLL_THUMBTRACK)
        topLineNew = pos;

    ScrollTo(topLineNew);
}

void ScintillaWX::DoMouseWheel(int rotation, int delta,
                               int linesPerAction, int ctrlDown,
                               bool isPageScroll ) {
    int topLineNew = topLine;
    int lines;

    if (ctrlDown) {  // Zoom the fonts if Ctrl key down
        if (rotation < 0) {
            KeyCommand(SCI_ZOOMIN);
        }
        else {
            KeyCommand(SCI_ZOOMOUT);
        }
    }
    else { // otherwise just scroll the window
        if ( !delta )
            delta = 120;
        wheelRotation += rotation;
        lines = wheelRotation / delta;
        wheelRotation -= lines * delta;
        if (lines != 0) {
            if (isPageScroll)
                lines = lines * LinesOnScreen();  // lines is either +1 or -1
            else
                lines *= linesPerAction;
            topLineNew -= lines;
            ScrollTo(topLineNew);
        }
    }
}


void ScintillaWX::DoSize(int width, int height) {
//      PRectangle rcClient(0,0,width,height);
//      SetScrollBarsTo(rcClient);
//      DropGraphics();
    ChangeSize();
}

void ScintillaWX::DoLoseFocus(){
    SetFocusState(false);
}

void ScintillaWX::DoGainFocus(){
    SetFocusState(true);
}

void ScintillaWX::DoSysColourChange() {
    InvalidateStyleData();
}

void ScintillaWX::DoLeftButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt) {
    ButtonDown(pt, curTime, shift, ctrl, alt);
}

void ScintillaWX::DoLeftButtonUp(Point pt, unsigned int curTime, bool ctrl) {
    ButtonUp(pt, curTime, ctrl);
}

void ScintillaWX::DoLeftButtonMove(Point pt) {
    ButtonMove(pt);
}

void ScintillaWX::DoMiddleButtonUp(Point pt) {
#ifdef __WXGTK__
    // Set the current position to the mouse click point and
    // then paste in the PRIMARY selection, if any.  wxGTK only.
    int newPos = PositionFromLocation(pt);
    MovePositionTo(newPos, 0, 1);

    pdoc->BeginUndoAction();
    wxTextDataObject data;
    bool gotData = FALSE;
    if (wxTheClipboard->Open()) {
        wxTheClipboard->UsePrimarySelection(TRUE);
        gotData = wxTheClipboard->GetData(data);
        wxTheClipboard->UsePrimarySelection(FALSE);
        wxTheClipboard->Close();
    }
    if (gotData) {
        wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(data.GetText());
        int        len = strlen(buf);
        pdoc->InsertString(currentPos, buf, len);
        SetEmptySelection(currentPos + len);
    }
    pdoc->EndUndoAction();
    NotifyChange();
    Redraw();

    ShowCaretAtCurrentPosition();
    EnsureCaretVisible();
#endif
}


void ScintillaWX::DoAddChar(int key) {
#if wxUSE_UNICODE
    wxChar wszChars[2];
    wszChars[0] = key;
    wszChars[1] = 0;
    wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(wszChars);
    AddCharUTF((char*)buf.data(), strlen(buf));
#else
    AddChar(key);
#endif
}


int  ScintillaWX::DoKeyDown(int key, bool shift, bool ctrl, bool alt, bool* consumed) {
#if defined(__WXGTK__) || defined(__WXMAC__)
    // Ctrl chars (A-Z) end up with the wrong keycode on wxGTK...
    if (ctrl && key >= 1 && key <= 26)
        key += 'A' - 1;
#endif

    switch (key) {
    case WXK_DOWN:              key = SCK_DOWN;     break;
    case WXK_UP:                key = SCK_UP;       break;
    case WXK_LEFT:              key = SCK_LEFT;     break;
    case WXK_RIGHT:             key = SCK_RIGHT;    break;
    case WXK_HOME:              key = SCK_HOME;     break;
    case WXK_END:               key = SCK_END;      break;
    case WXK_PAGEUP:            // fall through
    case WXK_PRIOR:             key = SCK_PRIOR;    break;
    case WXK_PAGEDOWN:          // fall through
    case WXK_NEXT:              key = SCK_NEXT;     break;
    case WXK_DELETE:            key = SCK_DELETE;   break;
    case WXK_INSERT:            key = SCK_INSERT;   break;
    case WXK_ESCAPE:            key = SCK_ESCAPE;   break;
    case WXK_BACK:              key = SCK_BACK;     break;
    case WXK_TAB:               key = SCK_TAB;      break;
    case WXK_RETURN:            key = SCK_RETURN;   break;
    case WXK_ADD:               // fall through
    case WXK_NUMPAD_ADD:        key = SCK_ADD;      break;
    case WXK_SUBTRACT:          // fall through
    case WXK_NUMPAD_SUBTRACT:   key = SCK_SUBTRACT; break;
    case WXK_DIVIDE:            // fall through
    case WXK_NUMPAD_DIVIDE:     key = SCK_DIVIDE;   break;
    case WXK_CONTROL:           key = 0; break;
    case WXK_ALT:               key = 0; break;
    case WXK_SHIFT:             key = 0; break;
    case WXK_MENU:              key = 0; break;
    }

    int rv = KeyDown(key, shift, ctrl, alt, consumed);

    if (key)
        return rv;
    else
        return 1;
}


void ScintillaWX::DoCommand(int ID) {
    Command(ID);
}


void ScintillaWX::DoContextMenu(Point pt) {
    if (displayPopupMenu)
        ContextMenu(pt);
}

void ScintillaWX::DoOnListBox() {
    AutoCompleteCompleted();
}

//----------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP
bool ScintillaWX::DoDropText(long x, long y, const wxString& data) {
    SetDragPosition(invalidPosition);

    // Send an event to allow the drag details to be changed
    wxStyledTextEvent evt(wxEVT_STC_DO_DROP, stc->GetId());
    evt.SetEventObject(stc);
    evt.SetDragResult(dragResult);
    evt.SetX(x);
    evt.SetY(y);
    evt.SetPosition(PositionFromLocation(Point(x,y)));
    evt.SetDragText(data);
    stc->GetEventHandler()->ProcessEvent(evt);

    dragResult = evt.GetDragResult();
    if (dragResult == wxDragMove || dragResult == wxDragCopy) {
        DropAt(evt.GetPosition(),
               wx2stc(evt.GetDragText()),
               dragResult == wxDragMove,
               FALSE); // TODO: rectangular?
        return TRUE;
    }
    return FALSE;
}


wxDragResult ScintillaWX::DoDragEnter(wxCoord x, wxCoord y, wxDragResult def) {
    dragResult = def;
    return dragResult;
}


wxDragResult ScintillaWX::DoDragOver(wxCoord x, wxCoord y, wxDragResult def) {
    SetDragPosition(PositionFromLocation(Point(x, y)));

    // Send an event to allow the drag result to be changed
    wxStyledTextEvent evt(wxEVT_STC_DRAG_OVER, stc->GetId());
    evt.SetEventObject(stc);
    evt.SetDragResult(def);
    evt.SetX(x);
    evt.SetY(y);
    evt.SetPosition(PositionFromLocation(Point(x,y)));
    stc->GetEventHandler()->ProcessEvent(evt);

    dragResult = evt.GetDragResult();
    return dragResult;
}


void ScintillaWX::DoDragLeave() {
    SetDragPosition(invalidPosition);
}
#endif
//----------------------------------------------------------------------

// Redraw all of text area. This paint will not be abandoned.
void ScintillaWX::FullPaint() {
    paintState = painting;
    rcPaint = GetClientRectangle();
    paintingAllText = true;
    wxClientDC dc(stc);
    Surface* surfaceWindow = Surface::Allocate();
    surfaceWindow->Init(&dc, wMain.GetID());

    dc.BeginDrawing();
    ClipChildren(dc, rcPaint);
    Paint(surfaceWindow, rcPaint);
    dc.EndDrawing();

    delete surfaceWindow;
    paintState = notPainting;
}


void ScintillaWX::DoScrollToLine(int line) {
    ScrollTo(line);
}


void ScintillaWX::DoScrollToColumn(int column) {
    HorizontalScrollTo(column * vs.spaceWidth);
}

void ScintillaWX::ClipChildren(wxDC& dc, PRectangle rect) {
#ifdef __WXGTK__
    wxRegion rgn(wxRectFromPRectangle(rect));
    if (ac.Active()) {
        wxRect childRect = ((wxWindow*)ac.lb->GetID())->GetRect();
        rgn.Subtract(childRect);
    }
    if (ct.inCallTipMode) {
        wxRect childRect = ((wxWindow*)ct.wCallTip.GetID())->GetRect();
        rgn.Subtract(childRect);
    }

    dc.SetClippingRegion(rgn);
#endif
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
