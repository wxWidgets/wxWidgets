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

#include <ctype.h>

#include "ScintillaWX.h"
#include "wx/stc/stc.h"


//----------------------------------------------------------------------

const int H_SCROLL_MAX  = 4000;
const int H_SCROLL_STEP = 20;
const int H_SCROLL_PAGE = 200;

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


#if wxUSE_POPUPWIN
#include <wx/popupwin.h>
#define wxSTCCallTipBase wxPopupWindow
#define param2  wxBORDER_NONE  // popup's 2nd param is flags
#else
#define wxSTCCallTipBase wxWindow
#define param2 -1 // wxWindows 2nd param is ID
#endif

class wxSTCCallTip : public wxSTCCallTipBase {
public:
    wxSTCCallTip(wxWindow* parent, CallTip* ct)
        : wxSTCCallTipBase(parent, param2)
        {
            m_ct = ct;
        }

    void OnPaint(wxPaintEvent& evt) {
        wxPaintDC dc(this);
        Surface surfaceWindow;
        surfaceWindow.Init(&dc);
        m_ct->PaintCT(&surfaceWindow);
        surfaceWindow.Release();
    }

#if wxUSE_POPUPWIN
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
    CallTip*    m_ct;
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSTCCallTip, wxSTCCallTipBase)
    EVT_PAINT(wxSTCCallTip::OnPaint)
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
    wxString dragText(drag.s, drag.len);

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
            timer.tickerID = (int)steTimer;
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
    else if (!on && capturedMouse)
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


bool ScintillaWX::ModifyScrollBars(int nMax, int nPage) {
    bool modified = false;

    if (stc->m_vScrollBar == NULL) {  // Use built-in scrollbar
        int  sbMax    = stc->GetScrollRange(wxVERTICAL);
        int  sbThumb  = stc->GetScrollThumb(wxVERTICAL);
        int  sbPos    = stc->GetScrollPos(wxVERTICAL);
        if (sbMax != nMax || sbThumb != nPage) {
            stc->SetScrollbar(wxVERTICAL, sbPos, nPage, nMax);
            modified = true;
        }
    }
    else { // otherwise use the one that's been given to us
        int  sbMax    = stc->m_vScrollBar->GetRange();
        int  sbPage   = stc->m_vScrollBar->GetPageSize();
        int  sbPos    = stc->m_vScrollBar->GetThumbPosition();
        if (sbMax != nMax || sbPage != nPage) {
            stc->m_vScrollBar->SetScrollbar(sbPos, nPage, nMax, nPage);
            modified = true;
        }
    }


    if (horizontalScrollBarVisible) {
        if (stc->m_hScrollBar == NULL) {  // Use built-in scrollbar
            int sbMax    = stc->GetScrollRange(wxHORIZONTAL);
            int sbThumb  = stc->GetScrollThumb(wxHORIZONTAL);
            if ((sbMax != H_SCROLL_MAX) || (sbThumb != H_SCROLL_STEP)) {
                stc->SetScrollbar(wxHORIZONTAL, 0, H_SCROLL_STEP, H_SCROLL_MAX);
                modified = true;
            }
        }
        else { // otherwise use the one that's been given to us
            int  sbMax    = stc->m_hScrollBar->GetRange();
            int  sbPage   = stc->m_hScrollBar->GetPageSize();
            if ((sbMax != H_SCROLL_MAX) || (sbPage != H_SCROLL_STEP)) {
                stc->m_hScrollBar->SetScrollbar(0, H_SCROLL_STEP, H_SCROLL_MAX, H_SCROLL_STEP);
                modified = true;
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
        wxTheClipboard->Open();
        wxTheClipboard->SetData(new wxTextDataObject(wxString(st.s, st.len)));
        wxTheClipboard->Close();
    }
}


void ScintillaWX::Paste() {
    pdoc->BeginUndoAction();
    ClearSelection();

    wxTextDataObject data;
    bool gotData;

    wxTheClipboard->Open();
    gotData = wxTheClipboard->GetData(data);
    wxTheClipboard->Close();
    if (gotData) {
        wxString str = data.GetText();
        int      len = str.Length();
        pdoc->InsertString(currentPos, str.c_str(), len);
        SetEmptySelection(currentPos + len);
    }

    pdoc->EndUndoAction();
    NotifyChange();
    Redraw();
}


bool ScintillaWX::CanPaste() {
    bool canPaste;

    wxTheClipboard->Open();
    canPaste = wxTheClipboard->IsSupported( wxDF_TEXT );
    wxTheClipboard->Close();

    return canPaste;
}

void ScintillaWX::CreateCallTipWindow(PRectangle) {
    ct.wCallTip = new wxSTCCallTip(stc, &ct);
    ct.wDraw = ct.wCallTip;
}


void ScintillaWX::AddToPopUp(const char *label, int cmd, bool enabled) {
    if (!label[0])
        popup.GetID()->AppendSeparator();
    else
        popup.GetID()->Append(cmd, label);

    if (!enabled)
        popup.GetID()->Enable(cmd, enabled);
}


void ScintillaWX::ClaimSelection() {

}


long ScintillaWX::DefWndProc(unsigned int /*iMessage*/, unsigned long /*wParam*/, long /*lParam*/) {
    return 0;
}

long ScintillaWX::WndProc(unsigned int iMessage, unsigned long wParam, long lParam) {
//      switch (iMessage) {
//      case EM_CANPASTE:
//          return CanPaste();
//      default:
        return ScintillaBase::WndProc(iMessage, wParam, lParam);
//      }
//      return 0;
}



//----------------------------------------------------------------------
// Event delegates

void ScintillaWX::DoPaint(wxDC* dc, wxRect rect) {

    paintState = painting;
    Surface surfaceWindow;
    surfaceWindow.Init(dc);
    PRectangle rcPaint = PRectangleFromwxRect(rect);
    dc->BeginDrawing();
    Paint(&surfaceWindow, rcPaint);
    dc->EndDrawing();
    surfaceWindow.Release();
    if (paintState == paintAbandoned) {
        // Painting area was insufficient to cover new styling or brace highlight positions
        FullPaint();
    }
    paintState = notPainting;
#ifdef __WXGTK__
    // On wxGTK the editor window paints can overwrite the listbox...
    if (ac.Active())
        ((wxWindow*)ac.lb.GetID())->Refresh(TRUE);
#endif
}


void ScintillaWX::DoHScroll(int type, int pos) {
    int xPos = xOffset;
    if (type == wxEVT_SCROLLWIN_LINEUP || type == wxEVT_SCROLL_LINEUP)
        xPos -= H_SCROLL_STEP;
    else if (type == wxEVT_SCROLLWIN_LINEDOWN || type == wxEVT_SCROLL_LINEDOWN)
        xPos += H_SCROLL_STEP;
    else if (type == wxEVT_SCROLLWIN_PAGEUP || type == wxEVT_SCROLL_PAGEUP)
        xPos -= H_SCROLL_PAGE;
    else if (type == wxEVT_SCROLLWIN_PAGEDOWN || type == wxEVT_SCROLL_PAGEDOWN)
        xPos += H_SCROLL_PAGE;
    else if (type == wxEVT_SCROLLWIN_TOP || type == wxEVT_SCROLL_TOP)
        xPos = 0;
    else if (type == wxEVT_SCROLLWIN_BOTTOM || type == wxEVT_SCROLL_BOTTOM)
        xPos = H_SCROLL_MAX;
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


void ScintillaWX::DoMouseWheel(int rotation, int delta, int linesPerAction, int ctrlDown) {
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
        wheelRotation += rotation;
        lines = wheelRotation / delta;
        wheelRotation -= lines * delta;
        if (lines != 0) {
            lines *= linesPerAction;
            topLineNew -= lines;
            ScrollTo(topLineNew);
        }
    }
}


void ScintillaWX::DoSize(int width, int height) {
    PRectangle rcClient(0,0,width,height);
    SetScrollBarsTo(rcClient);
    DropGraphics();
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

void ScintillaWX::DoButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt) {
    ButtonDown(pt, curTime, shift, ctrl, alt);
}

void ScintillaWX::DoButtonUp(Point pt, unsigned int curTime, bool ctrl) {
    ButtonUp(pt, curTime, ctrl);
}

void ScintillaWX::DoButtonMove(Point pt) {
    ButtonMove(pt);
}


void ScintillaWX::DoAddChar(char ch) {
    AddChar(ch);
}

int  ScintillaWX::DoKeyDown(int key, bool shift, bool ctrl, bool alt, bool* consumed) {
#ifdef __WXGTK__
    // Ctrl chars (A-Z) end up with the wrong keycode on wxGTK...
    if (ctrl && key >= 1 && key <= 26)
        key += 'A' - 1;
#endif

    switch (key) {
    case WXK_DOWN: key = SCK_DOWN; break;
    case WXK_UP: key = SCK_UP; break;
    case WXK_LEFT: key = SCK_LEFT; break;
    case WXK_RIGHT: key = SCK_RIGHT; break;
    case WXK_HOME: key = SCK_HOME; break;
    case WXK_END: key = SCK_END; break;
    case WXK_PRIOR: key = SCK_PRIOR; break;
    case WXK_NEXT: key = SCK_NEXT; break;
    case WXK_DELETE: key = SCK_DELETE; break;
    case WXK_INSERT: key = SCK_INSERT; break;
    case WXK_ESCAPE: key = SCK_ESCAPE; break;
    case WXK_BACK: key = SCK_BACK; break;
    case WXK_TAB: key = SCK_TAB; break;
    case WXK_RETURN: key = SCK_RETURN; break;
    case WXK_ADD:
    case WXK_NUMPAD_ADD:
        key = SCK_ADD; break;
    case WXK_SUBTRACT:
    case WXK_NUMPAD_SUBTRACT:
        key = SCK_SUBTRACT; break;
    case WXK_DIVIDE:
    case WXK_NUMPAD_DIVIDE:
        key = SCK_DIVIDE; break;
    case WXK_CONTROL: key = 0; break;
    case WXK_ALT: key = 0; break;
    case WXK_SHIFT: key = 0; break;
    case WXK_MENU: key = 0; break;
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
               evt.GetDragText(),
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
    rcPaint = GetTextRectangle();
    paintingAllText = true;
    wxClientDC dc(stc);
    Surface surfaceWindow;
    surfaceWindow.Init(&dc);
    Paint(&surfaceWindow, rcPaint);
    surfaceWindow.Release();

//     stc->Refresh(FALSE);

    paintState = notPainting;
}


void ScintillaWX::DoScrollToLine(int line) {
    ScrollTo(line);
}


void ScintillaWX::DoScrollToColumn(int column) {
    HorizontalScrollTo(column * vs.spaceWidth);
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------
