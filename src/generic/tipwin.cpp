///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/tipwin.cpp
// Purpose:     implementation of wxTipWindow
// Author:      Vadim Zeitlin
// Created:     10.09.00
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_TIPWINDOW

#include "wx/tipwin.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/timer.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/display.h"
#include "wx/except.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"
#include "wx/vector.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/msw/wrapwin.h"
    #include "wx/winui/private/auxwindows.h"
    #include "wx/winui/private/tlwhostmsw.h"
    #include "wx/winui/private/transient.h"
#endif

#include <new>
#include <unordered_map>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const wxCoord TEXT_MARGIN_X = 3;
static const wxCoord TEXT_MARGIN_Y = 3;

namespace
{

struct TipRuntimeState
{
    wxWeakRef<wxWindow> tipLifetime;
    wxTipWindow** expectedRefStorage = nullptr;
    wxWindow* viewAddress = nullptr;
    wxWeakRef<wxWindow> viewLifetime;
};

using TipRuntimeStates =
    std::unordered_map<const wxTipWindow*, TipRuntimeState>;

TipRuntimeStates& GetTipRuntimeStates()
{
    // Deliberately outlive the GUI module: late TLW teardown must never race
    // static destruction. Constructors reset entries, preventing address ABA.
    static TipRuntimeStates* const states = new TipRuntimeStates;
    return *states;
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

bool gs_failNextTipRuntimeAllocationForTesting = false;

void MaybeFailTipRuntimeAllocationForTesting()
{
    if ( gs_failNextTipRuntimeAllocationForTesting )
    {
        gs_failNextTipRuntimeAllocationForTesting = false;
#if wxUSE_EXCEPTIONS
        throw std::bad_alloc();
#endif
    }
}

#endif // __WXWINUI__ && wxUSE_WINUI3

TipRuntimeState& ResetTipRuntimeState(wxTipWindow* const tip)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    MaybeFailTipRuntimeAllocationForTesting();
#endif
    TipRuntimeState& state = GetTipRuntimeStates()[tip];
    state.tipLifetime = wxWeakRef<wxWindow>(tip);
    state.expectedRefStorage = nullptr;
    state.viewAddress = nullptr;
    state.viewLifetime.Release();
    return state;
}

TipRuntimeState& GetTipRuntimeState(wxTipWindow* const tip)
{
    TipRuntimeState& state = GetTipRuntimeStates()[tip];
    if ( state.tipLifetime.get() != tip )
        return ResetTipRuntimeState(tip);
    return state;
}

void SetExpectedTipRefStorage(wxTipWindow* const tip,
                              wxTipWindow** const storage)
{
    GetTipRuntimeState(tip).expectedRefStorage = storage;
}

bool IsExpectedTipRefStorage(wxTipWindow* const tip,
                             wxTipWindow** const storage)
{
    return storage &&
           GetTipRuntimeState(tip).expectedRefStorage == storage;
}

void ClearExpectedTipRefStorage(wxTipWindow* const tip,
                                wxTipWindow** const storage)
{
    TipRuntimeStates& states = GetTipRuntimeStates();
    const TipRuntimeStates::iterator it = states.find(tip);
    if ( it != states.end() && it->second.tipLifetime.get() == tip &&
         it->second.expectedRefStorage == storage )
    {
        it->second.expectedRefStorage = nullptr;
    }
}

void SetTipRuntimeView(wxTipWindow* const tip, wxWindow* const view)
{
    TipRuntimeState& state = GetTipRuntimeState(tip);
    state.viewAddress = view;
    state.viewLifetime = wxWeakRef<wxWindow>(view);
}

wxWindow* GetTipRuntimeView(wxTipWindow* const tip,
                            wxWindow* const expected)
{
    TipRuntimeState& state = GetTipRuntimeState(tip);
    return state.viewAddress == expected &&
           state.viewLifetime.get() == expected
                ? expected
                : nullptr;
}

void EraseTipRuntimeState(const wxTipWindow* const tip)
{
    GetTipRuntimeStates().erase(tip);
}

struct ActiveTipClose
{
    wxTipWindow* tip;
    wxWeakRef<wxWindow> lifetime;
    ActiveTipClose* previous;
};

// Close() used to publish its reentrancy record in a vector, making a routine
// user dismissal a new bad_alloc boundary on every port. A stack-local record
// linked through this TLS head has the same per-thread semantics and cannot
// allocate.
thread_local ActiveTipClose* gs_activeTipClose = nullptr;

#if defined(__WXWINUI__) && wxUSE_WINUI3
wxWinUITipCreateHookForTesting gs_tipCreateHookForTesting = nullptr;
wxWinUITipHookForTesting gs_tipHookForTesting = nullptr;

void InvokeTipHookForTesting(
    wxTipWindow* const tip,
    const wxWinUITipHookPointForTesting point)
{
    if ( gs_tipHookForTesting )
        gs_tipHookForTesting(tip, point);
}
#endif

bool IsTipCloseActive(wxTipWindow* const tip)
{
    for ( const ActiveTipClose* active = gs_activeTipClose;
          active;
          active = active->previous )
    {
        if ( active->tip == tip && active->lifetime.get() == tip )
            return true;
    }

    return false;
}

} // anonymous namespace

#if defined(__WXWINUI__) && wxUSE_WINUI3

void wxWinUISetTipCreateHookForTesting(
    wxWinUITipCreateHookForTesting hook)
{
    gs_tipCreateHookForTesting = hook;
}

void wxWinUIResetTipCreateHookForTesting()
{
    gs_tipCreateHookForTesting = nullptr;
}

void wxWinUISetTipHookForTesting(wxWinUITipHookForTesting hook)
{
    gs_tipHookForTesting = hook;
}

void wxWinUIResetTipHookForTesting()
{
    gs_tipHookForTesting = nullptr;
}

void wxWinUIFailNextTipRuntimeAllocationForTesting()
{
    gs_failNextTipRuntimeAllocationForTesting = true;
}

std::size_t wxWinUIGetTipRuntimeStateCountForTesting()
{
    return GetTipRuntimeStates().size();
}

#endif // __WXWINUI__ && wxUSE_WINUI3

// ----------------------------------------------------------------------------
// wxTipWindowView
// ----------------------------------------------------------------------------

// Viewer window to put in the frame
class WXDLLEXPORT wxTipWindowView : public wxWindow
{
public:
    wxTipWindowView(wxWindow *parent);

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouseClick(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);

    // calculate the client rect we need to display the text
    bool Adjust(const wxString& text, wxCoord maxLength);

private:
    wxWeakRef<wxTipWindow> m_parent;

    wxVector<wxString> m_textLines;
    wxCoord m_heightLine;


    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxTipWindowView);
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxTipWindow, wxPopupTransientWindow)
    EVT_LEFT_DOWN(wxTipWindow::OnMouseClick)
    EVT_RIGHT_DOWN(wxTipWindow::OnMouseClick)
    EVT_MIDDLE_DOWN(wxTipWindow::OnMouseClick)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(wxTipWindowView, wxWindow)
    EVT_PAINT(wxTipWindowView::OnPaint)

    EVT_LEFT_DOWN(wxTipWindowView::OnMouseClick)
    EVT_RIGHT_DOWN(wxTipWindowView::OnMouseClick)
    EVT_MIDDLE_DOWN(wxTipWindowView::OnMouseClick)

    EVT_MOTION(wxTipWindowView::OnMouseMove)
wxEND_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxTipWindow
// ----------------------------------------------------------------------------

wxTipWindow::Ref::~Ref()
{
    if (m_ptr)
    {
        m_ptr->SetTipWindowPtr(nullptr);
    }
}

wxTipWindow::Ref::Ref(Ref&& other)
{
    m_ptr = other.m_ptr;
    if (m_ptr)
    {
        other.m_ptr = nullptr;
        m_ptr->SetTipWindowPtr(&m_ptr);
    }
}

wxTipWindow::Ref& wxTipWindow::Ref::operator=(Ref&& other)
{
    if (m_ptr != other.m_ptr)
    {
        if (m_ptr)
        {
            m_ptr->SetTipWindowPtr(nullptr);
        }

        m_ptr = other.m_ptr;
        if (m_ptr)
        {
            other.m_ptr = nullptr;
            m_ptr->SetTipWindowPtr(&m_ptr);
        }
    }

    return *this;
}

wxTipWindow::Ref& wxTipWindow::Ref::operator=(std::nullptr_t)
{
    if (m_ptr)
    {
        m_ptr->SetTipWindowPtr(nullptr);
        m_ptr = nullptr;
    }

    return *this;
}

wxTipWindow::Ref wxTipWindow::New(wxWindow *parent,
            const wxString& text,
            wxCoord maxLength /*= 100*/,
            wxRect *rectBound /*= nullptr*/)
{
    std::unique_ptr<wxTipWindow> temp(new wxTipWindow);
    wxTipWindow* const rawTip = temp.get();
    const wxWeakRef<wxWindow> weakTip(rawTip);
    wxTipWindow::Ref retval;
    retval.m_ptr = rawTip;
    SetExpectedTipRefStorage(rawTip, &retval.m_ptr);

    bool created = false;
    wxTRY
    {
        created =
            rawTip->Create(parent, text, maxLength, &retval.m_ptr, rectBound);
    }
    wxCATCH_ALL(
    {
        // Ref is destroyed before temp during stack unwinding. Clear its plain
        // storage first so it can never dereference a window deleted by the
        // callback which threw. A normal live failure remains owned by temp;
        // direct deletion or a successful Destroy() transferred that ownership
        // away from the provisional unique_ptr.
        retval.m_ptr = nullptr;
        if ( weakTip.get() != rawTip ||
             wxWindowItselfIsUnavailableForCallbacks(rawTip) )
        {
            temp.release();
        }
        else
        {
            // Create() must already have withdrawn its backlink while
            // unwinding, but enforce the ownership boundary here too. A
            // callback is allowed to replace m_windowPtr with arbitrary
            // caller storage and that storage may have ceased to exist before
            // control reaches this catch block. Assigning the pointer itself
            // is safe; dereferencing the replacement is not.
            rawTip->SetTipWindowPtr(nullptr);

            // Delete while retval storage is still alive. Ordinary reverse-
            // order unwinding would otherwise destroy retval before temp.
            temp.reset();
        }
        throw;
    })

    // Create() is a callback-rich native boundary. If it synchronously deleted
    // the object, the unique_ptr is now dangling; if it queued wx destruction,
    // ownership belongs to that queue. In both cases release without touching
    // the raw address again. A normal live failure remains owned by temp.
    if ( weakTip.get() != rawTip ||
         wxWindowItselfIsUnavailableForCallbacks(rawTip) )
    {
        temp.release();
        // The hook may have destroyed the object before Create() published
        // m_windowPtr, so its destructor could not clear this provisional Ref.
        // Clear it as plain storage without touching the stale address.
        retval.m_ptr = nullptr;
        return Ref();
    }

    if ( !created || retval.m_ptr != rawTip ||
         rawTip->m_windowPtr != &retval.m_ptr )
    {
        // Keep both directions inert while temp destroys the still-live
        // object. In particular, never let ~wxTipWindow() dereference a
        // replacement backlink whose storage belonged to a callback frame.
        rawTip->SetTipWindowPtr(nullptr);
        retval.m_ptr = nullptr;
        return Ref();
    }

    temp.release();
    return retval;
}

wxTipWindow::wxTipWindow()
{
    ResetTipRuntimeState(this);
    SetTipWindowPtr(nullptr);
    m_view = nullptr;
}

bool wxTipWindow::Create(wxWindow *parent,
                         const wxString& text,
                         wxCoord maxLength,
                         wxTipWindow** windowPtr,
                         wxRect *rectBounds)
{
    if ( !parent || wxWindowIsUnavailableForCallbacks(parent) )
        return false;

    const wxWeakRef<wxTipWindow> weakThis(this);
    const wxWeakRef<wxWindow> weakParent(parent);
    const auto transactionIsValid =
        [this, parent, weakThis, weakParent]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   wxWeakWindowIsAvailableForCallbacks(weakParent, parent) &&
                   GetParent() == parent;
        };

    const bool ownsPublishedExternalValue =
        IsExpectedTipRefStorage(this, windowPtr);
    bool creationCommitted = false;
    const wxScopeGuard rollbackPublishedPointer = wxMakeGuard(
        [this, windowPtr, weakThis, ownsPublishedExternalValue,
         &creationCommitted]()
        {
            if ( creationCommitted )
                return;

            // The external storage and the object's backlink are two separate
            // publications. Only New() owns the original external storage and
            // may inspect it. The backlink itself is always withdrawn by
            // assignment, never dereferenced: a callback may have replaced it
            // with storage whose lifetime ended before this guard runs.
            if ( ownsPublishedExternalValue && *windowPtr == this )
                *windowPtr = nullptr;

            if ( weakThis.get() == this )
            {
                m_windowPtr = nullptr;
                ClearExpectedTipRefStorage(this, windowPtr);
            }
        });
    wxUnusedVar(rollbackPublishedPointer);

    if ( !wxPopupTransientWindow::Create(parent) ||
         !transactionIsValid() )
    {
        return false;
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( gs_tipCreateHookForTesting )
        gs_tipCreateHookForTesting(this);
    if ( !transactionIsValid() )
        return false;
#endif

    SetTipWindowPtr(windowPtr);
#if defined(__WXWINUI__) && wxUSE_WINUI3
    InvokeTipHookForTesting(
        this, wxWinUITipHookPointForTesting::AfterPointerPublication);
#endif
    const auto publishedPointerIsValid = [&]()
    {
        return transactionIsValid() && m_windowPtr == windowPtr &&
               (!ownsPublishedExternalValue || *windowPtr == this);
    };
    if ( !publishedPointerIsValid() )
        return false;

    if ( rectBounds )
    {
        SetBoundingRect(*rectBounds);
    }

    // set colours
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    if ( !publishedPointerIsValid() )
        return false;
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    if ( !publishedPointerIsValid() )
        return false;

    int x, y;
    wxGetMousePosition(&x, &y);
    if ( !publishedPointerIsValid() )
        return false;

    // move to the center of the target display so wxTipWindowView will use the
    // correct DPI
    wxPoint posScreen;
    wxSize sizeScreen;

    const int displayNum = wxDisplay::GetFromPoint(wxPoint(x, y));
    if ( displayNum != wxNOT_FOUND )
    {
        const wxRect rectScreen = wxDisplay(displayNum).GetGeometry();
        posScreen = rectScreen.GetPosition();
        sizeScreen = rectScreen.GetSize();
    }
    else // outside of any display?
    {
        // just use the primary one then
        posScreen = wxPoint(0, 0);
        sizeScreen = wxGetDisplaySize();
    }
    if ( !publishedPointerIsValid() )
        return false;

    wxPoint center(posScreen.x + sizeScreen.GetWidth() / 2,
                   posScreen.y + sizeScreen.GetHeight() / 2);
    Move(center, wxSIZE_NO_ADJUSTMENTS);
    if ( !publishedPointerIsValid() )
        return false;

    // set size, position and show it
    wxTipWindowView* const view = new wxTipWindowView(this);
    const wxWeakRef<wxWindow> weakView(view);
    if ( !transactionIsValid() ||
         !wxWeakWindowIsAvailableForCallbacks(weakView, view) ||
         view->GetParent() != this )
    {
        return false;
    }
    m_view = view;
    SetTipRuntimeView(this, view);
    const auto viewContextIsValid = [&]()
    {
        return publishedPointerIsValid() &&
               wxWeakWindowIsAvailableForCallbacks(weakView, view) &&
               view->GetParent() == this && m_view == view &&
               GetTipRuntimeView(this, m_view) == view;
    };
    if ( !viewContextIsValid() )
        return false;

    const wxCoord logicalMaxLength = parent->ToDIP(maxLength);
    if ( !viewContextIsValid() )
    {
        return false;
    }
    const wxCoord physicalMaxLength = FromDIP(logicalMaxLength);
    if ( !viewContextIsValid() )
    {
        return false;
    }
    if ( !view->Adjust(text, physicalMaxLength) )
        return false;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    InvokeTipHookForTesting(
        this, wxWinUITipHookPointForTesting::AfterViewAdjust);
#endif
    if ( !viewContextIsValid() )
    {
        return false;
    }

    // we want to show the tip below the mouse, not over it, make sure to not
    // overflow into the next display
    //
    // NB: the reason we use "/ 2" here is that we don't know where the current
    //     cursors hot spot is... it would be nice if we could find this out
    //     though
    int cursorOffset = wxSystemSettings::GetMetric(wxSYS_CURSOR_Y, this) / 2;
    if ( !viewContextIsValid() )
    {
        return false;
    }
    if (y + cursorOffset >= posScreen.y + sizeScreen.GetHeight())
        cursorOffset = posScreen.y + sizeScreen.GetHeight() - y - 1;
    y += cursorOffset;

    Position(wxPoint(x, y), wxSize(0,0));
    if ( !viewContextIsValid() )
    {
        return false;
    }

    Popup(view);
#if defined(__WXWINUI__) && wxUSE_WINUI3
    InvokeTipHookForTesting(
        this, wxWinUITipHookPointForTesting::AfterPopup);
#endif
    if ( !viewContextIsValid() || !IsShown() )
    {
        return false;
    }
    #ifdef __WXGTK__
        view->CaptureMouse();
        if ( !viewContextIsValid() )
        {
            return false;
        }
    #endif

    ClearExpectedTipRefStorage(this, windowPtr);
    creationCommitted = true;
    return true;
}

wxTipWindow::~wxTipWindow()
{
    if ( m_windowPtr )
    {
        *m_windowPtr = nullptr;
    }
    #ifdef __WXGTK__
        wxWindow* const liveView = GetTipRuntimeView(this, m_view);
        if ( liveView && liveView->HasCapture() )
            liveView->ReleaseMouse();
    #endif
    EraseTipRuntimeState(this);
}

void wxTipWindow::OnMouseClick(wxMouseEvent& WXUNUSED(event))
{
    Close();
}

void wxTipWindow::OnDismiss()
{
    Close();
}

void wxTipWindow::SetBoundingRect(const wxRect& rectBound)
{
    m_rectBound = rectBound;
}

void wxTipWindow::Close()
{
    if ( wxWindowItselfIsUnavailableForCallbacks(this) ||
         IsTipCloseActive(this) )
    {
        return;
    }

    const wxWeakRef<wxWindow> weakThis(this);
    wxWindow* const liveView = GetTipRuntimeView(this, m_view);
    wxTipWindowView* const view = liveView
                                      ? static_cast<wxTipWindowView*>(liveView)
                                      : nullptr;
    if ( m_view && !view )
        m_view = nullptr;
    const wxWeakRef<wxWindow> weakView(view);
    wxUnusedVar(weakView);
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const WXHWND closeHwnd = GetHWND();
    const unsigned long long closeHwndGeneration =
        closeHwnd ? wxWinUIMSWGetHwndGeneration(this, closeHwnd) : 0;
    const unsigned long long closeNativeHwndGeneration =
        closeHwnd ? wxWinUIMSWGetNativeHwndGeneration(closeHwnd) : 0;
#endif
    ActiveTipClose activeClose = { this, weakThis, gs_activeTipClose };
    gs_activeTipClose = &activeClose;
    const wxScopeGuard leaveClose = wxMakeGuard(
        [this, &activeClose]()
        {
            wxASSERT( gs_activeTipClose == &activeClose &&
                      activeClose.tip == this );
            gs_activeTipClose = activeClose.previous;
        });
    wxUnusedVar(leaveClose);

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( IsShown() )
        wxPopupTransientWindow::Dismiss();
#else
    if ( IsShown() )
        Show(false);
#endif
    if ( weakThis.get() != this ||
         wxWindowItselfIsUnavailableForCallbacks(this) )
    {
        return;
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Dismiss() deliberately preserves a visible popup when callbacks keep
    // transferring capture to descendants. Logical visibility is also
    // insufficient on Win32: a WM_SHOWWINDOW handler may call SW_SHOWNA after
    // wxWindowBase already records the object hidden. Close() is terminal by
    // contract, so both cases enter the popup manager's reusable bounded
    // capture/exact-HWND retirement transaction before Ref can be cleared.
    const bool terminalRetirementRequired =
        IsShown() ||
        (closeHwnd &&
         (GetHWND() != closeHwnd ||
          wxWinUIMSWGetHwndGeneration(this, closeHwnd) !=
              closeHwndGeneration ||
          wxWinUIMSWGetNativeHwndGeneration(closeHwnd) !=
              closeNativeHwndGeneration ||
          ::IsWindowVisible(static_cast<HWND>(closeHwnd))));
    if ( terminalRetirementRequired )
    {
        const bool retired = wxWinUIRetirePopupTerminal(this);
        if ( weakThis.get() != this ||
             wxWindowItselfIsUnavailableForCallbacks(this) )
        {
            return;
        }

        // A false result means an outer terminal transaction remains
        // authoritative or the destroy pipeline rejected ownership. Preserve
        // Ref in either case. A successful exceptional retirement also keeps
        // the backlink: terminal retirement has crossed into delayed-destroy
        // semantics, so only the destructor may publish the final null after
        // the ordinary event which initiated Close() has unwound.
        const WXHWND liveHwnd = GetHWND();
        if ( !retired || IsShown() ||
             (liveHwnd &&
              ::IsWindowVisible(static_cast<HWND>(liveHwnd))) )
        {
            return;
        }

        if ( !wxWinUITLWHostIsDestroyScheduled(this) )
            (void)Destroy();
        return;
    }
#endif

    if ( m_windowPtr )
    {
        *m_windowPtr = nullptr;
        m_windowPtr = nullptr;
    }

    #ifdef __WXGTK__
        if ( view && weakView.get() == view && view->HasCapture() )
            view->ReleaseMouse();
        if ( weakThis.get() != this ||
             wxWindowItselfIsUnavailableForCallbacks(this) )
        {
            return;
        }
    #endif
    // Under OS X and Qt we get destroyed because of wxEVT_KILL_FOCUS generated by
    // Show(false).
    #if !defined(__WXOSX__) && !defined(__WXQT__)
        Destroy();
    #endif
}

// ----------------------------------------------------------------------------
// wxTipWindowView
// ----------------------------------------------------------------------------

wxTipWindowView::wxTipWindowView(wxWindow *parent)
               : wxWindow(parent, wxID_ANY,
                          wxDefaultPosition, wxDefaultSize,
                          wxNO_BORDER),
                 m_parent(static_cast<wxTipWindow*>(parent))
{
    // set colours
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

    m_heightLine = 0;
}

bool wxTipWindowView::Adjust(const wxString& text, wxCoord maxLength)
{
    const wxWeakRef<wxWindow> weakThis(this);
    wxTipWindow* const parent = m_parent.get();
    const wxWeakRef<wxWindow> weakParent(parent);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakParent, parent) )
        return false;

    wxInfoDC dc(this);
    m_textLines.clear();
    m_heightLine = 0;

    // calculate the length: we want each line be no longer than maxLength
    // pixels and we only break lines at words boundary
    wxString current;
    wxCoord height, width,
            widthMax = 0;

    bool breakLine = false;
    for ( const wxChar *p = text.c_str(); ; p++ )
    {
        if ( *p == wxT('\n') || *p == wxT('\0') )
        {
            dc.GetTextExtent(current, &width, &height);
            if ( width > widthMax )
                widthMax = width;

            if ( height > m_heightLine )
                m_heightLine = height;

            m_textLines.push_back(current);

            if ( !*p )
            {
                // end of text
                break;
            }

            current.clear();
            breakLine = false;
        }
        else if ( breakLine && (*p == wxT(' ') || *p == wxT('\t')) )
        {
            // word boundary - break the line here
            m_textLines.push_back(current);
            current.clear();
            breakLine = false;
        }
        else // line goes on
        {
            current += *p;
            dc.GetTextExtent(current, &width, &height);
            if ( width > maxLength )
                breakLine = true;

            if ( width > widthMax )
                widthMax = width;

            if ( height > m_heightLine )
                m_heightLine = height;
        }
    }

    // take into account the border size and the margins
    width  = 2*(TEXT_MARGIN_X + 1) + widthMax;
    height = 2*(TEXT_MARGIN_Y + 1) + wx_truncate_cast(wxCoord, m_textLines.size())*m_heightLine;
    parent->SetClientSize(width, height);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakParent, parent) ||
         !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
    {
        return false;
    }

    SetSize(0, 0, width, height);
    return wxWeakWindowIsAvailableForCallbacks(weakParent, parent) &&
           wxWeakWindowIsAvailableForCallbacks(weakThis, this);
}

void wxTipWindowView::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    wxRect rect;
    wxSize size = GetClientSize();
    rect.width = size.x;
    rect.height = size.y;

    // first filll the background
    dc.SetBrush(wxBrush(GetBackgroundColour(), wxBRUSHSTYLE_SOLID));
    dc.SetPen(wxPen(GetForegroundColour(), 1, wxPENSTYLE_SOLID));
    dc.DrawRectangle(rect);

    // and then draw the text line by line
    wxPoint pt;
    pt.x = TEXT_MARGIN_X;
    pt.y = TEXT_MARGIN_Y;
    const size_t count = m_textLines.size();
    for ( size_t n = 0; n < count; n++ )
    {
        dc.DrawText(m_textLines[n], pt);

        pt.y += m_heightLine;
    }
}

void wxTipWindowView::OnMouseClick(wxMouseEvent& WXUNUSED(event))
{
    wxTipWindow* const parent = m_parent.get();
    if ( parent && !wxWindowIsUnavailableForCallbacks(parent) )
        parent->Close();
}

void wxTipWindowView::OnMouseMove(wxMouseEvent& event)
{
    wxTipWindow* const parent = m_parent.get();
    if ( !parent || wxWindowIsUnavailableForCallbacks(parent) )
        return;

    const wxRect rectBound = parent->m_rectBound;

    if ( rectBound.width &&
            !rectBound.Contains(ClientToScreen(event.GetPosition())) )
    {
        if ( m_parent.get() == parent &&
             !wxWindowIsUnavailableForCallbacks(parent) )
        {
            // Mouse left the bounding rect, disappear.
            parent->Close();
        }
    }
    else
    {
        event.Skip();
    }
}

#endif // wxUSE_TIPWINDOW
