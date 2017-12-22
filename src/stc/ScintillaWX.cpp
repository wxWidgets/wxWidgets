////////////////////////////////////////////////////////////////////////////
// Name:        src/stc/ScintillaWX.cpp
// Purpose:     A wxWidgets implementation of Scintilla.  A class derived
//              from ScintillaBase that uses the "wx platform" defined in
//              PlatformWX.cxx  This class is one end of a bridge between
//              the wx world and the Scintilla world.  It needs a peer
//              object of type wxStyledTextCtrl to function.
//
// Author:      Robin Dunn
//
// Created:     13-Jan-2000
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STC

#ifndef WX_PRECOMP
    #include "wx/scrolbar.h"
    #include "wx/math.h"
    #include "wx/menu.h"
    #include "wx/timer.h"
#endif // WX_PRECOMP

#include "wx/textbuf.h"
#include "wx/dataobj.h"
#include "wx/clipbrd.h"
#include "wx/dnd.h"

#if !wxUSE_STD_CONTAINERS && !wxUSE_STD_IOSTREAM && !wxUSE_STD_STRING
    #include "wx/beforestd.h"
    #include <string>
    #include "wx/afterstd.h"
#endif

#include "ScintillaWX.h"
#include "ExternalLexer.h"
#include "wx/stc/stc.h"
#include "wx/stc/private.h"
#include "PlatWX.h"

#ifdef __WXMSW__
    // GetHwndOf()
    #include "wx/msw/private.h"
#endif

//----------------------------------------------------------------------
// Helper classes

class wxSTCTimer : public wxTimer {
public:
    wxSTCTimer(ScintillaWX* swx, ScintillaWX::TickReason reason) {
        m_swx = swx;
        m_reason = reason;
    }

    void Notify() wxOVERRIDE {
        m_swx->TickFor(m_reason);
    }

private:
    ScintillaWX* m_swx;
    ScintillaWX::TickReason m_reason;
};


#if wxUSE_DRAG_AND_DROP
bool wxSTCDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {
    return m_swx->DoDropText(x, y, data);
}

wxDragResult  wxSTCDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult def) {
    return m_swx->DoDragEnter(x, y, def);
}

wxDragResult  wxSTCDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def) {
    return m_swx->DoDragOver(x, y, def);
}

void  wxSTCDropTarget::OnLeave() {
    m_swx->DoDragLeave();
}
#endif // wxUSE_DRAG_AND_DROP


#if wxUSE_POPUPWIN
#include "wx/popupwin.h"
#define wxSTCCallTipBase wxPopupWindow
#else
#include "wx/frame.h"
#define wxSTCCallTipBase wxFrame
#endif

#include "wx/dcbuffer.h"

class wxSTCCallTip : public wxSTCCallTipBase {
public:
    wxSTCCallTip(wxWindow* parent, CallTip* ct, ScintillaWX* swx) :
#if wxUSE_POPUPWIN
        wxSTCCallTipBase(parent, wxBORDER_NONE),
#else
        wxSTCCallTipBase(parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                         wxFRAME_NO_TASKBAR
                         | wxFRAME_FLOAT_ON_PARENT
                         | wxBORDER_NONE
#ifdef __WXMAC__
                         | wxPOPUP_WINDOW
#endif
            ),
#endif
          m_ct(ct), m_swx(swx), m_cx(wxDefaultCoord), m_cy(wxDefaultCoord)
        {
            SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        }

    ~wxSTCCallTip() {
#if wxUSE_POPUPWIN && defined(__WXGTK__)
        wxRect rect = GetRect();
        rect.x = m_cx;
        rect.y = m_cy;
        GetParent()->Refresh(false, &rect);
#endif
    }

    bool AcceptsFocus() const wxOVERRIDE { return false; }

    void OnPaint(wxPaintEvent& WXUNUSED(evt))
    {
        wxAutoBufferedPaintDC dc(this);
        Surface* surfaceWindow = Surface::Allocate(0);
        surfaceWindow->Init(&dc, m_ct->wDraw.GetID());
        m_ct->PaintCT(surfaceWindow);
        surfaceWindow->Release();
        delete surfaceWindow;
    }

    void OnFocus(wxFocusEvent& event)
    {
        GetParent()->SetFocus();
        event.Skip();
    }

    void OnLeftDown(wxMouseEvent& event)
    {
        wxPoint pt = event.GetPosition();
        Point p(pt.x, pt.y);
        m_ct->MouseClick(p);
        m_swx->CallTipClick();
    }

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) wxOVERRIDE
    {
        // convert coords to screen coords since we're a top-level window
        if (x != wxDefaultCoord) {
            m_cx = x;
            GetParent()->ClientToScreen(&x, NULL);
        }
        if (y != wxDefaultCoord) {
            m_cy = y;
            GetParent()->ClientToScreen(NULL, &y);
        }
        wxSTCCallTipBase::DoSetSize(x, y, width, height, sizeFlags);
    }

#if wxUSE_POPUPWIN
#else
    virtual bool Show( bool show = true )
    {
        // Although we're a frame, we always want the parent to be active, so
        // raise it whenever we get shown.
        bool rv = wxSTCCallTipBase::Show(show);
        if (rv && show)
        {
            wxTopLevelWindow *frame = wxDynamicCast(
                wxGetTopLevelParent(GetParent()), wxTopLevelWindow);
            if (frame)
                frame->Raise();
        }
        return rv;
    }
#endif

    wxPoint GetMyPosition()
    {
        return wxPoint(m_cx, m_cy);
    }

private:
    CallTip*      m_ct;
    ScintillaWX*  m_swx;
    int           m_cx, m_cy;
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxSTCCallTip, wxSTCCallTipBase)
    EVT_PAINT(wxSTCCallTip::OnPaint)
    EVT_SET_FOCUS(wxSTCCallTip::OnFocus)
    EVT_LEFT_DOWN(wxSTCCallTip::OnLeftDown)
wxEND_EVENT_TABLE()


//----------------------------------------------------------------------

#if wxUSE_DATAOBJ
static wxTextFileType wxConvertEOLMode(int scintillaMode)
{
    wxTextFileType type;

    switch (scintillaMode) {
        case wxSTC_EOL_CRLF:
            type = wxTextFileType_Dos;
            break;

        case wxSTC_EOL_CR:
            type = wxTextFileType_Mac;
            break;

        case wxSTC_EOL_LF:
            type = wxTextFileType_Unix;
            break;

        default:
            type = wxTextBuffer::typeDefault;
            break;
    }
    return type;
}
#endif // wxUSE_DATAOBJ


//----------------------------------------------------------------------
// Constructor/Destructor


ScintillaWX::ScintillaWX(wxStyledTextCtrl* win) {
    capturedMouse = false;
    focusEvent = false;
    wMain = win;
    stc   = win;
    wheelVRotation = 0;
    wheelHRotation = 0;
    Initialise();
#ifdef __WXMSW__
    sysCaretBitmap = 0;
    sysCaretWidth = 0;
    sysCaretHeight = 0;
#endif

#ifdef wxHAVE_STC_RECT_FORMAT
#if defined(__WXMSW__)
    m_clipRectTextFormat = wxDataFormat(wxT("MSDEVColumnSelect"));
#elif defined(__WXGTK__)
    m_clipRectTextFormat = wxDataFormat(wxT("SECONDARY"));
#else
    #error "Must define rectangular text selection clipboard format."
#endif
#endif // wxHAVE_STC_RECT_FORMAT

    //A timer is needed for each member of TickReason enum except tickPlatform
    timers[tickCaret] = new wxSTCTimer(this,tickCaret);
    timers[tickScroll] = new wxSTCTimer(this,tickScroll);
    timers[tickWiden] = new wxSTCTimer(this,tickWiden);
    timers[tickDwell] = new wxSTCTimer(this,tickDwell);
}


ScintillaWX::~ScintillaWX() {
    for ( TimersHash::iterator i=timers.begin(); i!=timers.end(); ++i ) {
        delete i->second;
    }
    timers.clear();
    Finalise();
}

//----------------------------------------------------------------------
// base class virtuals


void ScintillaWX::Initialise() {
    //ScintillaBase::Initialise();
#if wxUSE_DRAG_AND_DROP
    dropTarget = new wxSTCDropTarget;
    dropTarget->SetScintilla(this);
    stc->SetDropTarget(dropTarget);
#endif // wxUSE_DRAG_AND_DROP
    vs.extraFontFlag = true;   // UseAntiAliasing

    // Set up default OS X key mappings. Remember that SCI_CTRL stands for
    // "Cmd" key here, as elsewhere in wx API, while SCI_ALT is the "Option"
    // key (and "Ctrl" key, if we ever need it, should be represented by
    // SCI_META).
#ifdef __WXMAC__
    kmap.AssignCmdKey(SCK_LEFT, SCI_CTRL, SCI_VCHOME);
    kmap.AssignCmdKey(SCK_RIGHT, SCI_CTRL, SCI_LINEEND);
    kmap.AssignCmdKey(SCK_LEFT, SCI_ALT, SCI_WORDLEFT);
    kmap.AssignCmdKey(SCK_RIGHT, SCI_ALT, SCI_WORDRIGHT);
    kmap.AssignCmdKey(SCK_LEFT, SCI_ALT | SCI_SHIFT, SCI_WORDLEFTEXTEND);
    kmap.AssignCmdKey(SCK_RIGHT, SCI_ALT | SCI_SHIFT, SCI_WORDRIGHTEXTEND);
    kmap.AssignCmdKey(SCK_LEFT, SCI_CTRL | SCI_SHIFT, SCI_VCHOMEEXTEND);
    kmap.AssignCmdKey(SCK_RIGHT, SCI_CTRL | SCI_SHIFT, SCI_LINEENDEXTEND);
    kmap.AssignCmdKey(SCK_UP, SCI_CTRL | SCI_SHIFT, SCI_DOCUMENTSTARTEXTEND);
    kmap.AssignCmdKey(SCK_DOWN, SCI_CTRL | SCI_SHIFT, SCI_DOCUMENTENDEXTEND);
    kmap.AssignCmdKey(SCK_UP, SCI_CTRL, SCI_DOCUMENTSTART);
    kmap.AssignCmdKey(SCK_DOWN, SCI_CTRL, SCI_DOCUMENTEND);
#endif // __WXMAC__
}


void ScintillaWX::Finalise() {
    ScintillaBase::Finalise();
    SetIdle(false);
    DestroySystemCaret();
}


void ScintillaWX::StartDrag() {
#if wxUSE_DRAG_AND_DROP
    wxString dragText = stc2wx(drag.Data(), drag.Length());

    // Send an event to allow the drag text to be changed
    wxStyledTextEvent evt(wxEVT_STC_START_DRAG, stc->GetId());
    evt.SetEventObject(stc);
    evt.SetString(dragText);
    evt.SetDragFlags(wxDrag_DefaultMove);
    evt.SetPosition(wxMin(stc->GetSelectionStart(),
                          stc->GetSelectionEnd()));
    stc->GetEventHandler()->ProcessEvent(evt);
    dragText = evt.GetString();

    if ( !dragText.empty() ) {
        wxDropSource        source(stc);
        wxTextDataObject    data(dragText);
        wxDragResult        result;

        source.SetData(data);
        dropWentOutside = true;
        inDragDrop = ddDragging;
        result = source.DoDragDrop(evt.GetDragFlags());
        if (result == wxDragMove && dropWentOutside)
            ClearSelection();
        inDragDrop = ddNone;
        SetDragPosition(SelectionPosition(invalidPosition));
    }
#endif // wxUSE_DRAG_AND_DROP
}


bool ScintillaWX::SetIdle(bool on) {
    if (idler.state != on) {
        // connect or disconnect the EVT_IDLE handler
        if (on)
            stc->Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(wxStyledTextCtrl::OnIdle));
        else
            stc->Disconnect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(wxStyledTextCtrl::OnIdle));
        idler.state = on;
    }
    return idler.state;
}


void ScintillaWX::SetMouseCapture(bool on) {
    if (mouseDownCaptures) {
        if (on && !capturedMouse)
            stc->CaptureMouse();
        else if (!on && capturedMouse && stc->HasCapture())
            stc->ReleaseMouse();
        capturedMouse = on;
    }
}


bool ScintillaWX::HaveMouseCapture() {
    return capturedMouse;
}


void ScintillaWX::ScrollText(int linesToMove) {
    int dy = vs.lineHeight * (linesToMove);
    stc->ScrollWindow(0, dy);
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

    int vertEnd = nMax+1;
    if (!verticalScrollBarVisible)
        vertEnd = 0;

    // Check the vertical scrollbar
    if (stc->m_vScrollBar == NULL) {  // Use built-in scrollbar
        int  sbMax    = stc->GetScrollRange(wxVERTICAL);
        int  sbThumb  = stc->GetScrollThumb(wxVERTICAL);
        int  sbPos    = stc->GetScrollPos(wxVERTICAL);
        if (sbMax != vertEnd || sbThumb != nPage) {
            stc->SetScrollbar(wxVERTICAL, sbPos, nPage, vertEnd);
            modified = true;
        }
    }
    else { // otherwise use the one that's been given to us
        int  sbMax    = stc->m_vScrollBar->GetRange();
        int  sbPage   = stc->m_vScrollBar->GetPageSize();
        int  sbPos    = stc->m_vScrollBar->GetThumbPosition();
        if (sbMax != vertEnd || sbPage != nPage) {
            stc->m_vScrollBar->SetScrollbar(sbPos, nPage, vertEnd, nPage);
            modified = true;
        }
    }


    // Check the horizontal scrollbar
    PRectangle rcText = GetTextRectangle();
    int horizEnd = scrollWidth;
    if (horizEnd < 0)
        horizEnd = 0;
    if (!horizontalScrollBarVisible || Wrapping())
        horizEnd = 0;
    int pageWidth = wxRound(rcText.Width());

    if (stc->m_hScrollBar == NULL) {  // Use built-in scrollbar
        int sbMax    = stc->GetScrollRange(wxHORIZONTAL);
        int sbThumb  = stc->GetScrollThumb(wxHORIZONTAL);
        int sbPos    = stc->GetScrollPos(wxHORIZONTAL);
        if ((sbMax != horizEnd) || (sbThumb != pageWidth) || (sbPos != 0)) {
            stc->SetScrollbar(wxHORIZONTAL, sbPos, pageWidth, horizEnd);
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
            stc->m_hScrollBar->SetScrollbar(sbPos, pageWidth, horizEnd, pageWidth);
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


// This method is overloaded from ScintillaBase in order to prevent the
// AutoComplete window from being destroyed when it gets the focus.  There is
// a side effect that the AutoComp will also not be destroyed when switching
// to another window, but I think that is okay.
void ScintillaWX::CancelModes() {
    if (! focusEvent)
        AutoCompleteCancel();
    ct.CallTipCancel();
    Editor::CancelModes();
}



void ScintillaWX::Copy() {
    if (!sel.Empty()) {
        SelectionText st;
        CopySelectionRange(&st);
        CopyToClipboard(st);
    }
}


void ScintillaWX::Paste() {
    pdoc->BeginUndoAction();
    ClearSelection(multiPasteMode == SC_MULTIPASTE_EACH);

#if wxUSE_DATAOBJ
    wxTextDataObject data;
    bool gotData = false;
    bool isRectangularClipboard = false;

    wxTheClipboard->UsePrimarySelection(false);
    if (wxTheClipboard->Open()) {
#ifdef wxHAVE_STC_RECT_FORMAT
        isRectangularClipboard = wxTheClipboard->IsSupported(m_clipRectTextFormat);
#endif
        gotData = wxTheClipboard->GetData(data);
        wxTheClipboard->Close();
    }
    if (gotData) {
        wxString   text = wxTextBuffer::Translate(data.GetText(),
                                                  wxConvertEOLMode(pdoc->eolMode));

        // Send an event to allow the pasted text to be changed
        wxStyledTextEvent evt(wxEVT_STC_CLIPBOARD_PASTE, stc->GetId());
        evt.SetEventObject(stc);
        evt.SetPosition(sel.MainCaret());
        evt.SetString(text);
        stc->GetEventHandler()->ProcessEvent(evt);

        const wxCharBuffer buf(wx2stc(evt.GetString()));

#if wxUSE_UNICODE
        // free up the old character buffer in case the text is real big
        data.SetText(wxEmptyString);
        text = wxEmptyString;
#endif
        const size_t len = buf.length();
        SelectionPosition selStart = sel.IsRectangular() ?
            sel.Rectangular().Start() :
            sel.Range(sel.Main()).Start();

        // call the appropriate scintilla paste method if the
        // depending on whether the text was copied FROM a rectangular selection
        // or not.
        if (isRectangularClipboard)
        {
            PasteRectangular(selStart, buf, len);
        }
        else
        {
            InsertPaste(buf, len);
        }
    }
#endif // wxUSE_DATAOBJ

    pdoc->EndUndoAction();
    NotifyChange();
    Redraw();
}


void ScintillaWX::CopyToClipboard(const SelectionText& st) {
#if wxUSE_CLIPBOARD
    if ( !st.LengthWithTerminator() )
        return;

    // Send an event to allow the copied text to be changed
    wxStyledTextEvent evt(wxEVT_STC_CLIPBOARD_COPY, stc->GetId());
    evt.SetEventObject(stc);
    evt.SetString(wxTextBuffer::Translate(stc2wx(st.Data(), st.Length())));
    stc->GetEventHandler()->ProcessEvent(evt);

    wxTheClipboard->UsePrimarySelection(false);
    if (wxTheClipboard->Open()) {
        wxString text = evt.GetString();

#ifdef wxHAVE_STC_RECT_FORMAT
        if (st.rectangular)
        {
            // when copying the text to the clipboard, add extra meta-data that
            // tells the Paste() method that the user copied a rectangular
            // block of text, as opposed to a stream of text.
            wxDataObjectComposite* composite = new wxDataObjectComposite();
            composite->Add(new wxTextDataObject(text), true);
            composite->Add(new wxCustomDataObject(m_clipRectTextFormat));
            wxTheClipboard->SetData(composite);
        }
        else
#endif // wxHAVE_STC_RECT_FORMAT
            wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
    }
#else
    wxUnusedVar(st);
#endif // wxUSE_CLIPBOARD
}


bool ScintillaWX::CanPaste() {
#if wxUSE_CLIPBOARD
    bool canPaste = false;
    bool didOpen;

    if (Editor::CanPaste()) {
        wxTheClipboard->UsePrimarySelection(false);
        didOpen = !wxTheClipboard->IsOpened();
        if ( didOpen )
            wxTheClipboard->Open();

        if (wxTheClipboard->IsOpened()) {
            canPaste = wxTheClipboard->IsSupported(wxUSE_UNICODE ? wxDF_UNICODETEXT : wxDF_TEXT);
            if (didOpen)
                wxTheClipboard->Close();
        }
    }
    return canPaste;
#else
    return false;
#endif // wxUSE_CLIPBOARD
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
        ((wxMenu*)popup.GetID())->Append(cmd, wxGetTranslation(stc2wx(label)));

    if (!enabled)
        ((wxMenu*)popup.GetID())->Enable(cmd, enabled);
}


// This is called by the Editor base class whenever something is selected.
// For wxGTK we can put this text in the primary selection and then other apps
// can paste with the middle button.
void ScintillaWX::ClaimSelection() {
#ifdef __WXGTK__
    // Put the selected text in the PRIMARY selection
    if (!sel.Empty()) {
        SelectionText st;
        CopySelectionRange(&st);
        wxTheClipboard->UsePrimarySelection(true);
        if (wxTheClipboard->Open()) {
            wxString text = stc2wx(st.Data(), st.Length());
            wxTheClipboard->SetData(new wxTextDataObject(text));
            wxTheClipboard->Close();
        }
        wxTheClipboard->UsePrimarySelection(false);
    }
#endif
}


void ScintillaWX::UpdateSystemCaret() {
#ifdef __WXMSW__
    if (hasFocus) {
        if (HasCaretSizeChanged()) {
            DestroySystemCaret();
            CreateSystemCaret();
        }
        Point pos = PointMainCaret();
        ::SetCaretPos(wxRound(pos.x), wxRound(pos.y));
    }
#endif
}


bool ScintillaWX::HasCaretSizeChanged() {
#ifdef __WXMSW__
    if ( (vs.caretWidth && (sysCaretWidth != vs.caretWidth))
        || (vs.lineHeight && (sysCaretHeight != vs.lineHeight)) ) {
        return true;
    }
#endif
    return false;
}

bool ScintillaWX::CreateSystemCaret() {
#ifdef __WXMSW__
    sysCaretWidth = vs.caretWidth;
    if (0 == sysCaretWidth) {
        sysCaretWidth = 1;
    }
    sysCaretHeight = vs.lineHeight;
    int bitmapSize = (((sysCaretWidth + 15) & ~15) >> 3) * sysCaretHeight;
    char *bits = new char[bitmapSize];
    memset(bits, 0, bitmapSize);
    sysCaretBitmap = ::CreateBitmap(sysCaretWidth, sysCaretHeight, 1,
                                    1, reinterpret_cast<BYTE *>(bits));
    delete [] bits;
    BOOL retval = ::CreateCaret(GetHwndOf(stc), sysCaretBitmap,
                                sysCaretWidth, sysCaretHeight);
    ::ShowCaret(GetHwndOf(stc));
    return retval != 0;
#else
    return false;
#endif
}

bool ScintillaWX::DestroySystemCaret() {
#ifdef __WXMSW__
    if (sysCaretBitmap)
    {
        ::HideCaret(GetHwndOf(stc));
        BOOL retval = ::DestroyCaret();
        if (sysCaretBitmap) {
            ::DeleteObject(sysCaretBitmap);
            sysCaretBitmap = 0;
        }
        return retval != 0;
    }
    else
        return false;
#else
    return false;
#endif
}

bool ScintillaWX::FineTickerAvailable() {
    return true;
}

bool ScintillaWX::FineTickerRunning(TickReason reason) {
    bool running = false;
    TimersHash::iterator i = timers.find(reason);
    wxASSERT_MSG( i != timers.end(), "At least one TickReason is missing a timer.");
    if ( i != timers.end() ) {
        running = i->second->IsRunning();
    }
    return running;
}

void ScintillaWX::FineTickerStart(TickReason reason, int millis,
                                  int WXUNUSED(tolerance)) {
    TimersHash::iterator i = timers.find(reason);
    wxASSERT_MSG( i != timers.end(), "At least one TickReason is missing a timer." );
    if ( i != timers.end() ) {
        i->second->Start(millis);
    }
}

void ScintillaWX::FineTickerCancel(TickReason reason) {
    TimersHash::iterator i = timers.find(reason);
    wxASSERT_MSG( i != timers.end(), "At least one TickReason is missing a timer." );
    if ( i != timers.end() ) {
        i->second->Stop();
    }
}

//----------------------------------------------------------------------


sptr_t ScintillaWX::DefWndProc(unsigned int /*iMessage*/, uptr_t /*wParam*/, sptr_t /*lParam*/) {
    return 0;
}

sptr_t ScintillaWX::WndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam) {
      switch (iMessage) {
#if 0  // TODO: check this
          
      case SCI_CALLTIPSHOW: {
          // NOTE: This is copied here from scintilla/src/ScintillaBase.cxx
          // because of the little tweak that needs done below for wxGTK.
          // When updating new versions double check that this is still
          // needed, and that any new code there is copied here too.
          Point pt = LocationFromPosition(wParam);
          char* defn = reinterpret_cast<char *>(lParam);
          AutoCompleteCancel();
          pt.y += vs.lineHeight;
          int ctStyle = ct.UseStyleCallTip() ? STYLE_CALLTIP : STYLE_DEFAULT;
          if (ct.UseStyleCallTip())
          {
              ct.SetForeBack(vs.styles[STYLE_CALLTIP].fore, vs.styles[STYLE_CALLTIP].back);
          }
          int caretMain = sel.MainCaret();
          PRectangle rc = ct.CallTipStart(caretMain, pt,
                                          defn,
                                          vs.styles[ctStyle].fontName,
                                          vs.styles[ctStyle].sizeZoomed,
                                          CodePage(),
                                          vs.styles[ctStyle].characterSet,
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
#endif

#ifdef SCI_LEXER
      case SCI_LOADLEXERLIBRARY:
            LexerManager::GetInstance()->Load((const char*)lParam);
            break;
#endif
      case SCI_GETDIRECTFUNCTION:
            return reinterpret_cast<sptr_t>(DirectFunction);

      case SCI_GETDIRECTPOINTER:
            return reinterpret_cast<sptr_t>(this);

      default:
          return ScintillaBase::WndProc(iMessage, wParam, lParam);
      }
      return 0;
}



//----------------------------------------------------------------------
// Event delegates

void ScintillaWX::DoPaint(wxDC* dc, wxRect rect) {

    paintState = painting;
    AutoSurface surfaceWindow(dc, this);
    if (surfaceWindow) {
        rcPaint = PRectangleFromwxRect(rect);
        PRectangle rcClient = GetClientRectangle();
        paintingAllText = rcPaint.Contains(rcClient);

        ClipChildren(*dc, rcPaint);
        Paint(surfaceWindow, rcPaint);
        surfaceWindow->Release();
    }

    if (paintState == paintAbandoned) {
        // Painting area was insufficient to cover new styling or brace
        // highlight positions.  So trigger a new paint event that will
        // repaint the whole window.
        stc->Refresh(false);
        
#if defined(__WXOSX__)
        // On Mac we also need to finish the current paint to make sure that
        // everything is on the screen that needs to be there between now and
        // when the next paint event arrives.
        FullPaintDC(dc);
#endif
    }
    paintState = notPainting;
}


// Force the whole window to be repainted
void ScintillaWX::FullPaint() {
    stc->Refresh(false);
    stc->Update();
}


void ScintillaWX::FullPaintDC(wxDC* dc) {
    paintState = painting;
    rcPaint = GetClientRectangle();
    paintingAllText = true;
    AutoSurface surfaceWindow(dc, this);
    if (surfaceWindow) {
        Paint(surfaceWindow, rcPaint);
        surfaceWindow->Release();
    }
    paintState = notPainting;
}



void ScintillaWX::DoHScroll(int type, int pos) {
    int xPos = xOffset;
    PRectangle rcText = GetTextRectangle();
    int pageWidth = wxRound(rcText.Width() * 2 / 3);
    if (type == wxEVT_SCROLLWIN_LINEUP || type == wxEVT_SCROLL_LINEUP)
        xPos -= H_SCROLL_STEP;
    else if (type == wxEVT_SCROLLWIN_LINEDOWN || type == wxEVT_SCROLL_LINEDOWN)
        xPos += H_SCROLL_STEP;
    else if (type == wxEVT_SCROLLWIN_PAGEUP || type == wxEVT_SCROLL_PAGEUP)
        xPos -= pageWidth;
    else if (type == wxEVT_SCROLLWIN_PAGEDOWN || type == wxEVT_SCROLL_PAGEDOWN) {
        xPos += pageWidth;
        if (xPos > scrollWidth - rcText.Width()) {
            xPos = wxRound(scrollWidth - rcText.Width());
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

void ScintillaWX::DoMouseWheel(wxMouseWheelAxis axis, int rotation, int delta,
                               int linesPerAction, int columnsPerAction,
                               bool ctrlDown, bool isPageScroll) {
    int topLineNew = topLine;
    int lines;
    int xPos = xOffset;
    int pixels;

    if (axis == wxMOUSE_WHEEL_HORIZONTAL) {
        wheelHRotation += wxRound(rotation * (columnsPerAction * vs.spaceWidth));
        pixels = wheelHRotation / delta;
        wheelHRotation -= pixels * delta;
        if (pixels != 0) {
            xPos += pixels;
            PRectangle rcText = GetTextRectangle();
            if (xPos > scrollWidth - rcText.Width()) {
                xPos = wxRound(scrollWidth - rcText.Width());
            }
            HorizontalScrollTo(xPos);
        }
    }
    else if (ctrlDown) {  // Zoom the fonts if Ctrl key down
        if (rotation > 0) {
            KeyCommand(SCI_ZOOMIN);
        }
        else {
            KeyCommand(SCI_ZOOMOUT);
        }
    }
    else { // otherwise just scroll the window
        if ( !delta )
            delta = 120;
        wheelVRotation += rotation;
        lines = wheelVRotation / delta;
        wheelVRotation -= lines * delta;
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


void ScintillaWX::DoSize(int WXUNUSED(width), int WXUNUSED(height)) {
    ChangeSize();
}

void ScintillaWX::DoLoseFocus(){
    focusEvent = true;
    SetFocusState(false);
    focusEvent = false;
    DestroySystemCaret();
}

void ScintillaWX::DoGainFocus(){
    focusEvent = true;
    SetFocusState(true);
    focusEvent = false;
    DestroySystemCaret();
    CreateSystemCaret();
}

void ScintillaWX::DoSysColourChange() {
    InvalidateStyleData();
}

void ScintillaWX::DoLeftButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt) {
    ButtonDown(pt, curTime, shift, ctrl, alt);
}

void ScintillaWX::DoRightButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt) {
    if (!PointInSelection(pt)) {
        CancelModes();
        SetEmptySelection(PositionFromLocation(pt));
    }

    RightButtonDownWithModifiers(pt, curTime, ModifierFlags(shift, ctrl, alt));
}

void ScintillaWX::DoLeftButtonUp(Point pt, unsigned int curTime, bool ctrl) {
    ButtonUp(pt, curTime, ctrl);
}

void ScintillaWX::DoLeftButtonMove(Point pt) {
    ButtonMove(pt);
}

#ifdef __WXGTK__
void ScintillaWX::DoMiddleButtonUp(Point pt) {
    // Set the current position to the mouse click point and
    // then paste in the PRIMARY selection, if any.  wxGTK only.
    int newPos = PositionFromLocation(pt);
    MovePositionTo(newPos, Selection::noSel, true);

    pdoc->BeginUndoAction();
    wxTextDataObject data;
    bool gotData = false;
    wxTheClipboard->UsePrimarySelection(true);
    if (wxTheClipboard->Open()) {
        gotData = wxTheClipboard->GetData(data);
        wxTheClipboard->Close();
    }
    wxTheClipboard->UsePrimarySelection(false);
    if (gotData) {
        wxString   text = wxTextBuffer::Translate(data.GetText(),
                                                  wxConvertEOLMode(pdoc->eolMode));
        const wxCharBuffer buf(wx2stc(text));
        const size_t len = buf.length();
        int caretMain = sel.MainCaret();
        pdoc->InsertString(caretMain, buf, len);
        SetEmptySelection(caretMain + len);
    }
    pdoc->EndUndoAction();
    NotifyChange();
    Redraw();

    ShowCaretAtCurrentPosition();
    EnsureCaretVisible();
}
#else
void ScintillaWX::DoMiddleButtonUp(Point WXUNUSED(pt)) {
}
#endif


void ScintillaWX::DoAddChar(int key) {
#if wxUSE_UNICODE
    wxChar wszChars[2];
    wszChars[0] = (wxChar)key;
    wszChars[1] = 0;
    const wxCharBuffer buf(wx2stc(wszChars));
    AddCharUTF(buf, buf.length());
#else
    AddChar((char)key);
#endif
}


int  ScintillaWX::DoKeyDown(const wxKeyEvent& evt, bool* consumed)
{
    int key = evt.GetKeyCode();
    if (key == WXK_NONE) {
        // This is a Unicode character not representable in Latin-1 or some key
        // without key code at all (e.g. dead key or VK_PROCESSKEY under MSW).
        if ( consumed )
            *consumed = false;
        return 0;
    }

    if (evt.RawControlDown() && key >= 1 && key <= 26 && key != WXK_BACK)
        key += 'A' - 1;

    switch (key) {
    case WXK_DOWN:              key = SCK_DOWN;     break;
    case WXK_UP:                key = SCK_UP;       break;
    case WXK_LEFT:              key = SCK_LEFT;     break;
    case WXK_RIGHT:             key = SCK_RIGHT;    break;
    case WXK_HOME:              key = SCK_HOME;     break;
    case WXK_END:               key = SCK_END;      break;
    case WXK_PAGEUP:            key = SCK_PRIOR;    break;
    case WXK_PAGEDOWN:          key = SCK_NEXT;     break;
    case WXK_NUMPAD_DOWN:       key = SCK_DOWN;     break;
    case WXK_NUMPAD_UP:         key = SCK_UP;       break;
    case WXK_NUMPAD_LEFT:       key = SCK_LEFT;     break;
    case WXK_NUMPAD_RIGHT:      key = SCK_RIGHT;    break;
    case WXK_NUMPAD_HOME:       key = SCK_HOME;     break;
    case WXK_NUMPAD_END:        key = SCK_END;      break;
    case WXK_NUMPAD_PAGEUP:     key = SCK_PRIOR;    break;
    case WXK_NUMPAD_PAGEDOWN:   key = SCK_NEXT;     break;
    case WXK_NUMPAD_DELETE:     key = SCK_DELETE;   break;
    case WXK_NUMPAD_INSERT:     key = SCK_INSERT;   break;
    case WXK_DELETE:            key = SCK_DELETE;   break;
    case WXK_INSERT:            key = SCK_INSERT;   break;
    case WXK_ESCAPE:            key = SCK_ESCAPE;   break;
    case WXK_BACK:              key = SCK_BACK;     break;
    case WXK_TAB:               key = SCK_TAB;      break;
    case WXK_NUMPAD_ENTER:      wxFALLTHROUGH;
    case WXK_RETURN:            key = SCK_RETURN;   break;
    case WXK_ADD:               wxFALLTHROUGH;
    case WXK_NUMPAD_ADD:        key = SCK_ADD;      break;
    case WXK_SUBTRACT:          wxFALLTHROUGH;
    case WXK_NUMPAD_SUBTRACT:   key = SCK_SUBTRACT; break;
    case WXK_DIVIDE:            wxFALLTHROUGH;
    case WXK_NUMPAD_DIVIDE:     key = SCK_DIVIDE;   break;
    case WXK_CONTROL:           key = 0; break;
    case WXK_ALT:               key = 0; break;
    case WXK_SHIFT:             key = 0; break;
    case WXK_MENU:              key = SCK_MENU; break;
    }

    int rv = KeyDownWithModifiers
             (
                key,
                ModifierFlags
                (
                    evt.ShiftDown(),
                    evt.ControlDown(),
                    evt.AltDown(),
                    // Under Mac, ControlDown() returns the status of Cmd key,
                    // so we sneak the status of the real Ctrl key via SCI_META
                    // but we shouldn't be doing this elsewhere.
#ifdef __WXMAC__
                    evt.RawControlDown()
#else
                    0
#endif
                ),
                consumed
             );

    if (key)
        return rv;
    else
        return 1;
}


void ScintillaWX::DoCommand(int ID) {
    Command(ID);
}


bool ScintillaWX::DoContextMenu(Point pt) {
    if (ShouldDisplayPopup(pt))
    {
        // To prevent generating EVT_MOUSE_CAPTURE_LOST.
        if ( HaveMouseCapture() ) {
            SetMouseCapture(false);
        }
        ContextMenu(pt);
        return true;
    }
    return false;
}

void ScintillaWX::DoOnListBox() {
    AutoCompleteCompleted(0, SC_AC_COMMAND);
}


void ScintillaWX::DoMouseCaptureLost()
{
    capturedMouse = false;
}


void ScintillaWX::DoOnIdle(wxIdleEvent& evt) {

    if ( Idle() )
        evt.RequestMore();
    else
        SetIdle(false);
}

//----------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP
bool ScintillaWX::DoDropText(long x, long y, const wxString& data) {
    SetDragPosition(SelectionPosition(invalidPosition));

    wxString text = wxTextBuffer::Translate(data,
                                            wxConvertEOLMode(pdoc->eolMode));

    // Send an event to allow the drag details to be changed
    wxStyledTextEvent evt(wxEVT_STC_DO_DROP, stc->GetId());
    evt.SetEventObject(stc);
    evt.SetDragResult(dragResult);
    evt.SetX(x);
    evt.SetY(y);
    evt.SetPosition(PositionFromLocation(Point(x,y)));
    evt.SetString(text);
    stc->GetEventHandler()->ProcessEvent(evt);

    dragResult = evt.GetDragResult();
    if (dragResult == wxDragMove || dragResult == wxDragCopy) {
        DropAt(SelectionPosition(evt.GetPosition()),
               wx2stc(evt.GetString()),
               dragResult == wxDragMove,
               false); // TODO: rectangular?
        return true;
    }
    return false;
}


wxDragResult ScintillaWX::DoDragEnter(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def) {
    dragResult = def;
    return dragResult;
}


wxDragResult ScintillaWX::DoDragOver(wxCoord x, wxCoord y, wxDragResult def) {
    SetDragPosition(SelectionPosition(PositionFromLocation(Point(x, y))));

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
    SetDragPosition(SelectionPosition(invalidPosition));
}
#endif // wxUSE_DRAG_AND_DROP
//----------------------------------------------------------------------

void ScintillaWX::DoScrollToLine(int line) {
    ScrollTo(line);
}


void ScintillaWX::DoScrollToColumn(int column) {
    HorizontalScrollTo(wxRound(column * vs.spaceWidth));
}

// wxGTK doesn't appear to need this explicit clipping code any longer, but I
// will leave it here commented out for a while just in case...
void ScintillaWX::ClipChildren(wxDC& WXUNUSED(dc), PRectangle WXUNUSED(rect))
{
//     wxRegion rgn(wxRectFromPRectangle(rect));
//     if (ac.Active()) {
//         wxRect childRect = ((wxWindow*)ac.lb->GetID())->GetRect();
//         rgn.Subtract(childRect);
//     }
//     if (ct.inCallTipMode) {
//         wxSTCCallTip* tip = (wxSTCCallTip*)ct.wCallTip.GetID();
//         wxRect childRect = tip->GetRect();
// #if wxUSE_POPUPWIN
//         childRect.SetPosition(tip->GetMyPosition());
// #endif
//         rgn.Subtract(childRect);
//     }
//     dc.SetClippingRegion(rgn);
}


void ScintillaWX::SetUseAntiAliasing(bool useAA) {
    vs.extraFontFlag = useAA;
    InvalidateStyleRedraw();
}

bool ScintillaWX::GetUseAntiAliasing() {
    return vs.extraFontFlag != 0;
}

sptr_t ScintillaWX::DirectFunction(
    ScintillaWX* swx, unsigned int iMessage, uptr_t wParam, sptr_t lParam) {
    return swx->WndProc(iMessage, wParam, lParam);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

#endif // wxUSE_STC
