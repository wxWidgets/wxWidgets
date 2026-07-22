/////////////////////////////////////////////////////////////////////////////
// Name:        samples/winuispike/winuispike.cpp
// Purpose:     validation harness for the one-island-per-TLW architecture
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// THROWAWAY harness driving the wxWinUITopLevelHost library infrastructure
// (wx/winui/private/tlwhost.h).  Phase 0 validated the raw recipe in this
// same sample; it now exercises the real host:
//
//  - free elements (button, textbox, list, translucent overlays) placed on
//    the host's root canvas over plain wx-drawn panels: composition + input
//    routing come entirely from the library (bridge subclasses answering
//    WM_NCHITTEST -- nothing wx-side anymore);
//  - a wandering wx child window inside the LEFT panel carrying a slot
//    (RegisterSlot) with a XAML badge: the badge must follow every move,
//    get clipped exactly like the HWND at the panel's edges, and vanish
//    with the window (F4);
//  - S3 focus (F2/F3), S4 ContentDialog (F5), forced resync (F6).

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/evtloop.h"
#include "wx/settings.h"

#include "wx/winui/winui.h"

#if !wxUSE_WINUI3
    #error "This sample requires wxUSE_WINUI3"
#endif

#include "wx/winui/private/tlwhost.h"

#include "wx/msw/private.h"

#include <windowsx.h>

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

#include <algorithm>
#include <deque>
#include <unordered_map>

namespace MUX  = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

namespace
{

int g_tick = 0;

// ----------------------------------------------------------------------------
// Message-queue probe: tallies every message the GUI thread actually retrieves
// (WH_GETMESSAGE, PM_REMOVE) and dumps a per-second histogram into
// %TEMP%\spike-ticks.txt.  Purpose: name whatever floods the queue during
// modal loops (system resize/move, ShowModal, MessageBox) and starves
// WM_PAINT + mouse input down to ~1 Hz.
// ----------------------------------------------------------------------------

HHOOK g_msgHook = nullptr;
ULONG g_msgCount[0x10000];
HWND g_msgLastHwnd[0x10000];
ULONGLONG g_msgWindowStart = 0;

FILE *ProbeLog()
{
    static FILE *s_log = nullptr;
    if ( !s_log )
    {
        wchar_t path[MAX_PATH] = L"";
        ::GetTempPathW(MAX_PATH, path);
        wcscat_s(path, L"spike-ticks.txt");
        s_log = _wfopen(path, L"a");
        if ( s_log )
            fprintf(s_log, "==== session pid=%lu ====\n",
                    ::GetCurrentProcessId());
    }
    return s_log;
}

void DumpMessageHistogram(ULONGLONG now)
{
    FILE * const log = ProbeLog();
    if ( !log )
        return;

    struct Entry { UINT msg; ULONG n; };
    Entry top[8] = {};
    ULONG total = 0;
    for ( UINT m = 0; m < 0x10000; ++m )
    {
        const ULONG n = g_msgCount[m];
        if ( !n )
            continue;
        total += n;
        for ( int k = 0; k < 8; ++k )
        {
            if ( n > top[k].n )
            {
                for ( int j = 7; j > k; --j )
                    top[j] = top[j - 1];
                top[k] = { m, n };
                break;
            }
        }
    }

    fprintf(log, "%llu queue: total=%lu |", now, total);
    for ( int k = 0; k < 8 && top[k].n; ++k )
    {
        char cls[64] = "";
        const HWND h = g_msgLastHwnd[top[k].msg];
        if ( h )
            ::GetClassNameA(h, cls, sizeof(cls));
        fprintf(log, " 0x%04X=%lu(%s)", top[k].msg, top[k].n, cls);
    }
    fprintf(log, "\n");
    fflush(log);

    memset(g_msgCount, 0, sizeof(g_msgCount));
}

// Sent-message cost meter: WH_CALLWNDPROC / WH_CALLWNDPROCRET time every
// message delivered synchronously to a wndproc on this thread -- exactly the
// ones the modal move/size loop SENDS (WM_MOVING, WM_WINDOWPOSCHANGING...)
// which never show up in the WH_GETMESSAGE histogram.
HHOOK g_cwpHook = nullptr;
HHOOK g_cwpRetHook = nullptr;

struct SentCost
{
    double totalMs = 0.0;
    double maxMs = 0.0;
    ULONG n = 0;
    HWND lastHwnd = nullptr;
};
std::unordered_map<UINT, SentCost> g_sentCost;

struct CwpFrame { HWND hwnd; UINT msg; LONGLONG t0; };
CwpFrame g_cwpStack[128];
int g_cwpDepth = 0;

LONGLONG QpcNow()
{
    LARGE_INTEGER li;
    ::QueryPerformanceCounter(&li);
    return li.QuadPart;
}

double QpcMs(LONGLONG dt)
{
    static LONGLONG s_freq = 0;
    if ( !s_freq )
    {
        LARGE_INTEGER li;
        ::QueryPerformanceFrequency(&li);
        s_freq = li.QuadPart;
    }
    return 1000.0 * dt / s_freq;
}

LRESULT CALLBACK SpikeCallWndHook(int code, WPARAM wParam, LPARAM lParam)
{
    if ( code >= 0 && g_cwpDepth < int(WXSIZEOF(g_cwpStack)) )
    {
        const CWPSTRUCT * const cwp = reinterpret_cast<CWPSTRUCT *>(lParam);
        g_cwpStack[g_cwpDepth++] = { cwp->hwnd, cwp->message, QpcNow() };
    }
    return ::CallNextHookEx(g_cwpHook, code, wParam, lParam);
}

LRESULT CALLBACK SpikeCallWndRetHook(int code, WPARAM wParam, LPARAM lParam)
{
    if ( code >= 0 && g_cwpDepth > 0 )
    {
        const CWPRETSTRUCT * const ret =
            reinterpret_cast<CWPRETSTRUCT *>(lParam);
        const CwpFrame& top = g_cwpStack[g_cwpDepth - 1];
        if ( top.hwnd == ret->hwnd && top.msg == ret->message )
        {
            --g_cwpDepth;
            const double ms = QpcMs(QpcNow() - top.t0);
            SentCost& c = g_sentCost[top.msg];
            c.totalMs += ms;
            if ( ms > c.maxMs )
                c.maxMs = ms;
            ++c.n;
            c.lastHwnd = top.hwnd;
        }
        else
        {
            // Pairing lost (early unhook, filtered call): resynchronize.
            g_cwpDepth = 0;
        }
    }
    return ::CallNextHookEx(g_cwpRetHook, code, wParam, lParam);
}

void DumpSentCosts(FILE *log)
{
    struct Entry { UINT msg; SentCost c; };
    Entry top[8] = {};
    for ( const auto& kv : g_sentCost )
    {
        for ( int k = 0; k < 8; ++k )
        {
            if ( kv.second.totalMs > top[k].c.totalMs )
            {
                for ( int j = 7; j > k; --j )
                    top[j] = top[j - 1];
                top[k] = { kv.first, kv.second };
                break;
            }
        }
    }

    fprintf(log, "  sent:");
    for ( int k = 0; k < 8 && top[k].c.n; ++k )
    {
        char cls[64] = "";
        if ( top[k].c.lastHwnd )
            ::GetClassNameA(top[k].c.lastHwnd, cls, sizeof(cls));
        fprintf(log, " 0x%04X=%.0fms/%lu(max %.1f,%s)",
                top[k].msg, top[k].c.totalMs, top[k].c.n,
                top[k].c.maxMs, cls);
    }
    fprintf(log, "\n");

    g_sentCost.clear();
}

LRESULT CALLBACK SpikeGetMsgHook(int code, WPARAM wParam, LPARAM lParam)
{
    if ( code >= 0 && wParam == PM_REMOVE )
    {
        const MSG * const msg = reinterpret_cast<MSG *>(lParam);
        if ( msg->message < 0x10000 )
        {
            ++g_msgCount[msg->message];
            g_msgLastHwnd[msg->message] = msg->hwnd;
        }

        const ULONGLONG now = ::GetTickCount64();
        if ( g_msgWindowStart == 0 )
            g_msgWindowStart = now;
        if ( now - g_msgWindowStart >= 1000 )
        {
            g_msgWindowStart = now;
            DumpMessageHistogram(now);
            FILE * const log = ProbeLog();
            if ( log )
            {
                DumpSentCosts(log);
                fflush(log);
            }
        }
    }
    return ::CallNextHookEx(g_msgHook, code, wParam, lParam);
}

winrt::Windows::UI::Color SpikeColor(BYTE a, BYTE r, BYTE g, BYTE b)
{
    winrt::Windows::UI::Color c;
    c.A = a;
    c.R = r;
    c.G = g;
    c.B = b;
    return c;
}

MUXM::SolidColorBrush SpikeBrush(BYTE a, BYTE r, BYTE g, BYTE b)
{
    return MUXM::SolidColorBrush(SpikeColor(a, r, g, b));
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// Log panel: purely wx-drawn event log (no wxTextCtrl: that would create a
// per-control island and muddy the experiment).
// ----------------------------------------------------------------------------

class LogPanel : public wxWindow
{
public:
    explicit LogPanel(wxWindow *parent)
        : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   wxFULL_REPAINT_ON_RESIZE)
    {
        SetMinSize(wxSize(-1, FromDIP(200)));
        Bind(wxEVT_PAINT, &LogPanel::OnPaint, this);
        Bind(wxEVT_MOUSEWHEEL, &LogPanel::OnWheel, this);
    }

    void Add(const wxString& line)
    {
        m_lines.push_back(wxDateTime::UNow().Format("%H:%M:%S.%l  ") + line);
        while ( m_lines.size() > 400 )
            m_lines.pop_front();
        m_scroll = 0;
        Refresh(false);
    }

private:
    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);
        const wxSize size = GetClientSize();
        dc.SetBrush(wxBrush(wxColour(18, 18, 22)));
        dc.SetPen(wxPen(wxColour(70, 70, 78)));
        dc.DrawRectangle(0, 0, size.x, size.y);

        dc.SetFont(wxFontInfo(9).Family(wxFONTFAMILY_TELETYPE));
        const int lineHeight = dc.GetCharHeight() + FromDIP(1);
        int y = size.y - lineHeight - FromDIP(4);
        dc.SetTextForeground(wxColour(190, 220, 190));

        for ( size_t i = m_lines.size() - std::min<size_t>(m_lines.size(), m_scroll);
              i > 0 && y > -lineHeight;
              --i, y -= lineHeight )
        {
            dc.DrawText(m_lines[i - 1], FromDIP(6), y);
        }
    }

    void OnWheel(wxMouseEvent& event)
    {
        const int step = event.GetWheelRotation() > 0 ? 3 : -3;
        m_scroll = std::max(0, std::min<int>(int(m_lines.size()),
                                             int(m_scroll) + step));
        Add(wxString::Format("LogPanel: wxEVT_MOUSEWHEEL natif (delta=%d)",
                             event.GetWheelRotation()));
    }

    std::deque<wxString> m_lines;
    size_t m_scroll = 0;
};

// ----------------------------------------------------------------------------
// PaintPanel: wx-drawn surface standing in for the generic controls; logs the
// native input it receives, repaints on a timer.
// ----------------------------------------------------------------------------

class PaintPanel : public wxWindow
{
public:
    PaintPanel(wxWindow *parent, LogPanel *log, const wxString& title)
        : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   wxFULL_REPAINT_ON_RESIZE),
          m_log(log),
          m_title(title)
    {
        Bind(wxEVT_PAINT, &PaintPanel::OnPaint, this);
        Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& e) {
            ++m_enters;
            Log("wxEVT_ENTER_WINDOW natif");
            Refresh(false);
            e.Skip();
        });
        Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& e) {
            ++m_leaves;
            Log("wxEVT_LEAVE_WINDOW natif");
            Refresh(false);
            e.Skip();
        });
        Bind(wxEVT_MOTION, [this](wxMouseEvent& e) {
            if ( ++m_motions % 40 == 0 )
                Log(wxString::Format("wxEVT_MOTION natif (x40, total %d)", m_motions));
            e.Skip();
        });
        Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& e) {
            ++m_clicks;
            m_lastClick = e.GetPosition();
            Log(wxString::Format("wxEVT_LEFT_DOWN natif à (%d,%d)",
                                 e.GetPosition().x, e.GetPosition().y));
            SetFocus();
            Refresh(false);
            e.Skip();
        });
        Bind(wxEVT_LEFT_DCLICK, [this](wxMouseEvent& e) {
            Log("wxEVT_LEFT_DCLICK natif");
            e.Skip();
        });
        Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent& e) {
            Log("wxEVT_RIGHT_DOWN natif");
            e.Skip();
        });
        Bind(wxEVT_MOUSEWHEEL, [this](wxMouseEvent& e) {
            m_wheel += e.GetWheelRotation();
            Log(wxString::Format("wxEVT_MOUSEWHEEL natif (delta=%d)",
                                 e.GetWheelRotation()));
            Refresh(false);
        });
        Bind(wxEVT_SET_FOCUS, [this](wxFocusEvent& e) {
            m_hasFocus = true;
            Log("wxEVT_SET_FOCUS natif");
            Refresh(false);
            e.Skip();
        });
        Bind(wxEVT_KILL_FOCUS, [this](wxFocusEvent& e) {
            m_hasFocus = false;
            Log("wxEVT_KILL_FOCUS natif");
            Refresh(false);
            e.Skip();
        });
    }

private:
    void Log(const wxString& what)
    {
        if ( m_log )
            m_log->Add(m_title + ": " + what);
    }

    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);
        const wxSize size = GetClientSize();
        const int margin = FromDIP(10);

        // The outer margin stays unpainted: black fill + DWM = Mica.
        const wxRect inner(margin, margin,
                           size.x - 2*margin, size.y - 2*margin);
        dc.SetBrush(wxBrush(m_hasFocus ? wxColour(58, 44, 26)
                                       : wxColour(38, 38, 44)));
        dc.SetPen(m_hasFocus ? wxPen(wxColour(255, 170, 60), FromDIP(2))
                             : wxPen(wxColour(96, 96, 104)));
        dc.DrawRoundedRectangle(inner, FromDIP(10));

        dc.SetTextForeground(*wxWHITE);
        dc.SetFont(wxFontInfo(11).Bold());
        int y = inner.y + FromDIP(10);
        dc.DrawText(m_title, inner.x + FromDIP(14), y);

        dc.SetFont(wxFontInfo(9));
        dc.SetTextForeground(wxColour(200, 200, 210));
        y += FromDIP(28);
        dc.DrawText(wxString::Format(
            "tick=%d  (le compteur avance => le GDI vit sous la bande)", g_tick),
            inner.x + FromDIP(14), y);
        y += FromDIP(18);
        dc.DrawText(wxString::Format(
            "enter=%d leave=%d clics=%d molette=%d motions=%d",
            m_enters, m_leaves, m_clicks, m_wheel, m_motions),
            inner.x + FromDIP(14), y);
        y += FromDIP(18);
        dc.DrawText(m_hasFocus ? "FOCUS NATIF ICI" : "(pas le focus)",
                    inner.x + FromDIP(14), y);

        y += FromDIP(26);
        dc.SetTextForeground(wxColour(150, 170, 200));
        dc.DrawText("Clic = focus natif    F2 = focus TextBox XAML",
                    inner.x + FromDIP(14), y);
        y += FromDIP(16);
        dc.DrawText("F3 = où est ::GetFocus()    F5 = ContentDialog (S4)",
                    inner.x + FromDIP(14), y);
        y += FromDIP(16);
        dc.DrawText("F4 = montrer/cacher la fenêtre à SLOT    F6 = resynchro",
                    inner.x + FromDIP(14), y);
        y += FromDIP(16);
        dc.DrawText("F7 = wxDialog MODAL    F8 = wxMessageBox    F9 = MessageBox natif",
                    inner.x + FromDIP(14), y);

        if ( m_lastClick != wxDefaultPosition )
        {
            dc.SetPen(wxPen(wxColour(120, 220, 120), FromDIP(2)));
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawCircle(m_lastClick, FromDIP(9));
        }
    }

    LogPanel *m_log;
    wxString m_title;
    wxPoint m_lastClick = wxDefaultPosition;
    int m_enters = 0,
        m_leaves = 0,
        m_clicks = 0,
        m_wheel = 0,
        m_motions = 0;
    bool m_hasFocus = false;
};

// ----------------------------------------------------------------------------
// Wanderer: a small wx child window bounced around inside the left panel by
// the frame timer.  It carries a slot in the TLW host: the XAML badge must
// follow it, clip with it at the panel edges and hide with it.
// ----------------------------------------------------------------------------

class WandererWindow : public wxWindow
{
public:
    WandererWindow(wxWindow *parent, LogPanel *log)
        : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
          m_log(log)
    {
        SetInitialSize(parent->FromDIP(wxSize(150, 90)));
        Bind(wxEVT_PAINT, [this](wxPaintEvent&) {
            wxPaintDC dc(this);
            const wxSize size = GetClientSize();
            dc.SetBrush(wxBrush(wxColour(24, 60, 46)));
            dc.SetPen(wxPen(wxColour(90, 190, 140), FromDIP(2)));
            dc.DrawRectangle(0, 0, size.x, size.y);
            dc.SetTextForeground(wxColour(210, 240, 220));
            dc.SetFont(wxFontInfo(8));
            dc.DrawText("fenêtre wx baladeuse", FromDIP(8), FromDIP(6));
            dc.DrawText("(le badge XAML doit suivre)", FromDIP(8), FromDIP(20));
        });
        Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& e) {
            if ( m_log )
                m_log->Add("BALADEUSE: wxEVT_LEFT_DOWN natif (l'entrée passe "
                           "à travers le badge)");
            e.Skip();
        });
    }

private:
    LogPanel *m_log;
};

// ----------------------------------------------------------------------------
// SlotButtonWindow: a wx child window whose slot hosts a real (interactive)
// XAML Button.  The XAML content covers it and consumes the native input, so
// every wx event it logs can only come from the library's slot synthesis --
// this is the phase-2 test.
// ----------------------------------------------------------------------------

class SlotButtonWindow : public wxWindow
{
public:
    SlotButtonWindow(wxWindow *parent, LogPanel *log)
        : wxWindow(parent, wxID_ANY),
          m_log(log)
    {
        SetInitialSize(parent->FromDIP(wxSize(170, 60)));

        Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& e) {
            Log("wxEVT_ENTER_WINDOW (synthèse slot)");
            e.Skip();
        });
        Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& e) {
            Log("wxEVT_LEAVE_WINDOW (synthèse slot)");
            e.Skip();
        });
        Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& e) {
            Log(wxString::Format("wxEVT_LEFT_DOWN (synthèse slot) à (%d,%d)",
                                 e.GetPosition().x, e.GetPosition().y));
            e.Skip();
        });
        Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& e) {
            Log("wxEVT_LEFT_UP (synthèse slot)");
            e.Skip();
        });
        Bind(wxEVT_SET_FOCUS, [this](wxFocusEvent& e) {
            Log("wxEVT_SET_FOCUS (arbitre de focus)");
            e.Skip();
        });
        Bind(wxEVT_KILL_FOCUS, [this](wxFocusEvent& e) {
            Log("wxEVT_KILL_FOCUS (arbitre de focus)");
            e.Skip();
        });
    }

private:
    void Log(const wxString& what)
    {
        if ( m_log )
            m_log->Add("FENETRE-SLOT: " + what);
    }

    LogPanel *m_log;
};

// ----------------------------------------------------------------------------
// The frame
// ----------------------------------------------------------------------------

class SpikeFrame : public wxFrame
{
public:
    SpikeFrame()
        : wxFrame(nullptr, wxID_ANY,
                  wxString::FromUTF8("wxWinUI spike — île unique par TLW "
                                     "(wxWinUITopLevelHost)"),
                  wxDefaultPosition, wxSize(1150, 780))
    {
        m_log = new LogPanel(this);
        m_left = new PaintPanel(this, m_log, "Panel wx GAUCHE");
        m_right = new PaintPanel(this, m_log, "Panel wx DROIT");

        m_wanderer = new WandererWindow(m_left, m_log);
        m_wanderer->Move(FromDIP(wxPoint(40, 220)));

        m_slotButton = new SlotButtonWindow(m_right, m_log);
        m_slotButton->Move(FromDIP(wxPoint(24, 420)));

        wxBoxSizer *top = new wxBoxSizer(wxHORIZONTAL);
        top->Add(m_left, 1, wxEXPAND);
        top->Add(m_right, 1, wxEXPAND);

        wxBoxSizer *main = new wxBoxSizer(wxVERTICAL);
        main->Add(top, 1, wxEXPAND);
        main->Add(m_log, 0, wxEXPAND);
        SetSizer(main);
        Layout();

        m_host = wxWinUITopLevelHost::ForWindow(this, true);
        if ( m_host )
        {
            m_log->Add("host TLW créé par la librairie (tlwhost.cpp)");
            BuildFreeElements();
            RegisterWandererSlot();
            RegisterSlotButton();
            PositionFreeElements();
        }
        else
        {
            m_log->Add("ECHEC: wxWinUITopLevelHost::ForWindow a rendu null");
        }

        for ( wxWindow *w : { static_cast<wxWindow *>(m_left),
                              static_cast<wxWindow *>(m_right),
                              static_cast<wxWindow *>(m_log) } )
        {
            w->Bind(wxEVT_KEY_DOWN, &SpikeFrame::OnKey, this);
        }

        Bind(wxEVT_SIZE, [this](wxSizeEvent& e) {
            e.Skip();
            CallAfter([this] { PositionFreeElements(); });
        });

        g_msgHook = ::SetWindowsHookExW(WH_GETMESSAGE, SpikeGetMsgHook,
                                        nullptr, ::GetCurrentThreadId());
        g_cwpHook = ::SetWindowsHookExW(WH_CALLWNDPROC, SpikeCallWndHook,
                                        nullptr, ::GetCurrentThreadId());
        g_cwpRetHook = ::SetWindowsHookExW(WH_CALLWNDPROCRET,
                                           SpikeCallWndRetHook,
                                           nullptr, ::GetCurrentThreadId());

        m_timer.SetOwner(this);
        Bind(wxEVT_TIMER, [this](wxTimerEvent&) {
            ++g_tick;
            m_left->Refresh(false);
            m_right->Refresh(false);
            BounceWanderer();
            ProbePointerTarget();
        });
        m_timer.Start(400);

        m_log->Add("=== harnais phase 1 : la baladeuse teste le moteur de "
                   "synchro (suivi, clip, F4 show/hide) ===");
    }

    ~SpikeFrame() override
    {
        m_timer.Stop();
        if ( g_msgHook )
        {
            ::UnhookWindowsHookEx(g_msgHook);
            g_msgHook = nullptr;
        }
        if ( g_cwpHook )
        {
            ::UnhookWindowsHookEx(g_cwpHook);
            g_cwpHook = nullptr;
        }
        if ( g_cwpRetHook )
        {
            ::UnhookWindowsHookEx(g_cwpRetHook);
            g_cwpRetHook = nullptr;
        }
    }

private:
    void BuildFreeElements()
    {
        auto root = m_host->Root();

        // Diagnostic: where do presses land island-side?
        root.AddHandler(
            MUX::UIElement::PointerPressedEvent(),
            winrt::box_value(
                MUX::Input::PointerEventHandler(
                    [this](winrt::Windows::Foundation::IInspectable const&,
                           MUX::Input::PointerRoutedEventArgs const& args)
                    {
                        const auto pos =
                            args.GetCurrentPoint(m_host->Root()).Position();
                        m_log->Add(wxString::Format(
                            "XAML: PointerPressed île à (%.0f,%.0f) DIP",
                            pos.X, pos.Y));
                    })),
            true /* handledEventsToo */);

        m_alphaBorder = MUXC::Border();
        m_alphaBorder.Background(SpikeBrush(96, 226, 46, 66));
        m_alphaBorder.CornerRadius(MUX::CornerRadiusHelper::FromUniformRadius(10));
        m_alphaBorder.IsHitTestVisible(false);
        {
            MUXC::TextBlock t;
            t.Text(L"bande alpha XAML — l'entrée passe à travers");
            t.Margin(MUX::ThicknessHelper::FromLengths(10, 8, 10, 8));
            t.TextWrapping(MUX::TextWrapping::Wrap);
            m_alphaBorder.Child(t);
        }
        root.Children().Append(m_alphaBorder);

        m_caption = MUXC::TextBlock();
        m_caption.Text(L"Texte XAML anticrénelé composé au-dessus du GDI wx");
        m_caption.FontSize(16);
        m_caption.IsHitTestVisible(false);
        root.Children().Append(m_caption);

        m_button = MUXC::Button();
        m_button.Content(winrt::box_value(L"XAML Button"));
        m_button.Click(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::RoutedEventArgs const&)
            {
                ++m_xamlClicks;
                m_clickInfo.Text(winrt::hstring(wxString::Format(
                    "clics XAML: %d", m_xamlClicks).ToStdWstring()));
                m_log->Add(wxString::Format(
                    "XAML: Button.Click #%d (entrée native arrivée à l'île)",
                    m_xamlClicks));
            });
        root.Children().Append(m_button);

        m_clickInfo = MUXC::TextBlock();
        m_clickInfo.Text(L"clics XAML: 0");
        m_clickInfo.IsHitTestVisible(false);
        root.Children().Append(m_clickInfo);

        m_textBox = MUXC::TextBox();
        m_textBox.PlaceholderText(L"TextBox XAML — tape ici (S3)");
        m_textBox.Width(240);
        m_textBox.GotFocus(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::RoutedEventArgs const&)
            {
                m_log->Add("XAML: TextBox.GotFocus");
            });
        m_textBox.LostFocus(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::RoutedEventArgs const&)
            {
                m_log->Add("XAML: TextBox.LostFocus");
            });
        root.Children().Append(m_textBox);

        // Render-pipeline probe: an always-active ProgressRing (spun by the
        // animation machinery) + a per-second count of UI-thread render
        // ticks, dumped to %TEMP%\spike-ticks.txt.  If the ring spins
        // smoothly while ticks/s stays near zero the composition thread is
        // healthy and the UI-thread render tick is starved; if the ring is
        // janky too the whole present path is broken.
        m_ring = MUXC::ProgressRing();
        m_ring.IsActive(true);
        m_ring.Width(44);
        m_ring.Height(44);
        root.Children().Append(m_ring);

        m_renderToken = MUXM::CompositionTarget::Rendering(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Windows::Foundation::IInspectable const&)
            {
                ++m_renderTicks;
                const ULONGLONG now = ::GetTickCount64();
                if ( m_renderWindowStart == 0 )
                    m_renderWindowStart = now;
                if ( now - m_renderWindowStart >= 1000 )
                {
                    FILE * const ticksLog = ProbeLog();
                    if ( ticksLog )
                    {
                        fprintf(ticksLog, "%llu ticks/s=%d\n",
                                now, m_renderTicks);
                        fflush(ticksLog);
                    }
                    m_log->Add(wxString::Format(
                        "XAML CompositionTarget.Rendering: %d ticks/s",
                        m_renderTicks));
                    m_renderTicks = 0;
                    m_renderWindowStart = now;
                }
            });

        m_scroller = MUXC::ScrollViewer();
        m_scroller.Width(240);
        m_scroller.Height(150);
        {
            MUXC::StackPanel stack;
            stack.Background(SpikeBrush(150, 30, 30, 36));
            for ( int i = 1; i <= 30; ++i )
            {
                MUXC::TextBlock t;
                t.Text(winrt::hstring(wxString::Format(
                    wxString::FromUTF8("élément XAML %d — molette ici (S5)"),
                    i).ToStdWstring()));
                t.Margin(MUX::ThicknessHelper::FromLengths(8, 3, 8, 3));
                stack.Children().Append(t);
            }
            m_scroller.Content(stack);
        }
        root.Children().Append(m_scroller);
    }

    void RegisterWandererSlot()
    {
        MUXC::Border badge;
        badge.Background(SpikeBrush(120, 40, 110, 220));
        badge.CornerRadius(MUX::CornerRadiusHelper::FromUniformRadius(8));
        badge.IsHitTestVisible(false);
        {
            MUXC::TextBlock t;
            t.Text(L"SLOT XAML");
            t.FontSize(18);
            t.HorizontalAlignment(MUX::HorizontalAlignment::Center);
            t.VerticalAlignment(MUX::VerticalAlignment::Center);
            badge.Child(t);
        }
        // Fill the slot container.
        badge.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
        badge.VerticalAlignment(MUX::VerticalAlignment::Stretch);

        if ( m_host->RegisterSlot(m_wanderer, badge) )
            m_log->Add("slot enregistré pour la baladeuse");
        else
            m_log->Add("ECHEC RegisterSlot(baladeuse)");
    }

    void RegisterSlotButton()
    {
        MUXC::Button button;
        button.Content(winrt::box_value(L"Slot Button (phase 2)"));
        button.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
        button.VerticalAlignment(MUX::VerticalAlignment::Stretch);
        button.Click(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::RoutedEventArgs const&)
            {
                m_log->Add("FENETRE-SLOT: Button.Click XAML (le contrôle "
                           "réagit toujours)");
            });

        if ( m_host->RegisterSlot(m_slotButton, button) )
            m_log->Add("slot interactif enregistré (Slot Button)");
        else
            m_log->Add("ECHEC RegisterSlot(Slot Button)");
    }

    void PositionFreeElements()
    {
        if ( !m_host )
            return;

        const double scale = m_host->GetScale();
        auto dip = [scale](int px) { return px / scale; };

        const wxRect l = m_left->GetRect();
        const wxRect r = m_right->GetRect();

        MUXC::Canvas::SetLeft(m_alphaBorder, dip(l.GetRight() - 140));
        MUXC::Canvas::SetTop(m_alphaBorder, dip(l.y + 40));
        m_alphaBorder.Width(dip(280));
        m_alphaBorder.Height(dip(110));

        MUXC::Canvas::SetLeft(m_caption, dip(l.x + 24));
        MUXC::Canvas::SetTop(m_caption, dip(l.GetBottom() - 64));

        const int x = r.x + 24;
        int y = r.y + 170;
        MUXC::Canvas::SetLeft(m_button, dip(x));
        MUXC::Canvas::SetTop(m_button, dip(y));
        MUXC::Canvas::SetLeft(m_clickInfo, dip(x + 150));
        MUXC::Canvas::SetTop(m_clickInfo, dip(y + 8));

        y += 52;
        MUXC::Canvas::SetLeft(m_textBox, dip(x));
        MUXC::Canvas::SetTop(m_textBox, dip(y));

        y += 52;
        MUXC::Canvas::SetLeft(m_scroller, dip(x));
        MUXC::Canvas::SetTop(m_scroller, dip(y));

        MUXC::Canvas::SetLeft(m_ring, dip(l.x + 24));
        MUXC::Canvas::SetTop(m_ring, dip(l.y + 24));
    }

    void BounceWanderer()
    {
        if ( !m_wanderer || !m_wanderer->IsShown() )
            return;

        const wxSize parentSize = m_left->GetClientSize();
        const wxSize size = m_wanderer->GetSize();
        wxPoint pos = m_wanderer->GetPosition();

        pos += m_wandererVel;

        // Deliberately allow a 40px overshoot beyond the parent edges so the
        // slot's Clip against the ancestor chain is exercised.
        const int over = FromDIP(40);
        if ( pos.x < -over || pos.x + size.x > parentSize.x + over )
        {
            m_wandererVel.x = -m_wandererVel.x;
            pos.x += 2*m_wandererVel.x;
        }
        if ( pos.y < -over || pos.y + size.y > parentSize.y + over )
        {
            m_wandererVel.y = -m_wandererVel.y;
            pos.y += 2*m_wandererVel.y;
        }

        m_wanderer->Move(pos);
    }

    void OnKey(wxKeyEvent& event)
    {
        switch ( event.GetKeyCode() )
        {
            case WXK_F2:
                if ( m_host )
                {
                    ::SetFocus(m_host->GetBridgeHwnd());
                    m_textBox.Focus(MUX::FocusState::Programmatic);
                    m_log->Add("F2: focus poussé vers la TextBox XAML");
                }
                break;

            case WXK_F3:
                LogNativeFocus();
                break;

            case WXK_F4:
                if ( m_wanderer )
                {
                    m_wanderer->Show(!m_wanderer->IsShown());
                    m_log->Add(m_wanderer->IsShown()
                                   ? "F4: baladeuse montrée (le badge doit revenir)"
                                   : "F4: baladeuse cachée (le badge doit disparaître)");
                }
                break;

            case WXK_F5:
                ShowTestDialog();
                break;

            case WXK_F6:
                if ( m_host )
                {
                    m_host->MarkAllDirty();
                    m_log->Add("F6: MarkAllDirty (resynchro forcée)");
                }
                break;

            // Modal-surface scenarios: the input-freeze family (frozen wx
            // modal dialogs, delayed crash, border-resize freeze) all mix
            // modal loops with the island pipeline.  Each flavor gets its
            // own key so the failing combination can be isolated on this
            // minimal system.
            case WXK_F7:
            {
                m_log->Add("F7: wxDialog MODAL (ShowModal) — île propre");
                wxDialog dlg(this, wxID_ANY, "Spike — dialogue wx modal",
                             wxDefaultPosition, wxSize(420, 260),
                             wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
                wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
                sizer->Add(new wxStaticText(&dlg, wxID_ANY,
                    "Dialogue wx modal avec sa propre île.\n"
                    "Cliquer, taper, redimensionner, rouvrir (F7), fermer."),
                    wxSizerFlags().Border(wxALL, 12));
                sizer->Add(new wxTextCtrl(&dlg, wxID_ANY, "texte"),
                    wxSizerFlags().Expand().Border(wxALL, 12));
                sizer->AddStretchSpacer();
                sizer->Add(dlg.CreateStdDialogButtonSizer(wxOK | wxCANCEL),
                    wxSizerFlags().Expand().Border(wxALL, 12));
                dlg.SetSizer(sizer);
                const int rc = dlg.ShowModal();
                m_log->Add(wxString::Format("F7: ShowModal => %d", rc));
                break;
            }

            case WXK_F8:
                m_log->Add("F8: wxMessageBox (presenter ContentDialog)");
                wxMessageBox("Message via le presenter (overlay par défaut).",
                             "Spike F8", wxOK | wxCANCEL, this);
                m_log->Add("F8: wxMessageBox terminé");
                break;

            case WXK_F9:
                m_log->Add("F9: MessageBoxW NATIF (boucle modale système)");
                ::MessageBoxW(GetHwnd(), L"Boucle modale système pure.",
                              L"Spike F9", MB_OKCANCEL);
                m_log->Add("F9: MessageBoxW terminé");
                break;

            // Same dialog content WITHOUT the modal machinery: no
            // wxModalEventLoop, parent stays enabled.  Isolates the modal
            // context from "second TLW with an island".
            case WXK_F10:
                ToggleModeless();
                break;

            // Second full wxFrame with island content: neither the dialog
            // window class nor any modal loop.
            case WXK_F11:
                ToggleSecondFrame();
                break;

            default:
                event.Skip();
        }
    }

    void ToggleModeless()
    {
        if ( m_modeless && m_modeless->IsShown() )
        {
            m_modeless->Destroy();
            m_modeless = nullptr;
            m_log->Add("F10: dialogue MODELESS détruit");
            return;
        }
        m_log->Add("F10: wxDialog MODELESS (Show) — île propre");
        m_modeless = new wxDialog(this, wxID_ANY,
                                  "Spike — dialogue wx MODELESS",
                                  wxDefaultPosition, wxSize(420, 260),
                                  wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxStaticText(m_modeless, wxID_ANY,
            "Dialogue wx MODELESS avec sa propre île."),
            wxSizerFlags().Border(wxALL, 12));
        sizer->Add(new wxTextCtrl(m_modeless, wxID_ANY, "texte"),
            wxSizerFlags().Expand().Border(wxALL, 12));
        sizer->AddStretchSpacer();
        sizer->Add(m_modeless->CreateStdDialogButtonSizer(wxOK | wxCANCEL),
            wxSizerFlags().Expand().Border(wxALL, 12));
        m_modeless->SetSizer(sizer);
        m_modeless->Show();
    }

    void ToggleSecondFrame()
    {
        if ( m_secondFrame && m_secondFrame->IsShown() )
        {
            m_secondFrame->Destroy();
            m_secondFrame = nullptr;
            m_log->Add("F11: 2e frame détruite");
            return;
        }
        m_log->Add("F11: 2e wxFrame avec île (modeless, classe frame)");
        m_secondFrame = new wxFrame(this, wxID_ANY,
                                    "Spike — 2e frame à île",
                                    wxDefaultPosition, wxSize(420, 260));
        wxPanel *panel = new wxPanel(m_secondFrame);
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxStaticText(panel, wxID_ANY,
            "2e frame, mêmes contrôles, pas de boucle modale."),
            wxSizerFlags().Border(wxALL, 12));
        sizer->Add(new wxTextCtrl(panel, wxID_ANY, "texte"),
            wxSizerFlags().Expand().Border(wxALL, 12));
        sizer->Add(new wxButton(panel, wxID_ANY, "Un bouton"),
            wxSizerFlags().Border(wxALL, 12));
        panel->SetSizer(sizer);
        m_secondFrame->Show();
    }

protected:
    // Remote control for the test harness: scenarios must be reachable with
    // a plain PostMessage, no focus or foreground required (input injection
    // is impossible while another app covers the screen).
    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override
    {
        switch ( nMsg )
        {
            case WM_APP + 1:
                ToggleModeless();
                return 1;

            case WM_APP + 2:
                ToggleSecondFrame();
                return 1;

            case WM_APP + 3:
                // Modal: never from inside the wndproc.
                CallAfter([this] {
                    wxKeyEvent ev(wxEVT_KEY_DOWN);
                    ev.m_keyCode = WXK_F7;
                    OnKey(ev);
                });
                return 1;
        }
        return wxFrame::MSWWindowProc(nMsg, wParam, lParam);
    }

private:
    void LogNativeFocus()
    {
        const HWND focus = ::GetFocus();
        wchar_t cls[128] = L"";
        if ( focus )
            ::GetClassNameW(focus, cls, WXSIZEOF(cls));

        const wxWindow *w = wxGetWindowFromHWND((WXHWND)focus);
        wxString where;
        if ( m_host && focus == m_host->GetBridgeHwnd() )
            where = "le PONT de l'île";
        else if ( w )
            where = wxString::Format("wx '%s'", w->GetClassInfo()->GetClassName());
        else
            where = "une fenêtre interne de l'île";

        // FindFocus goes through the new DoFindFocus hook: with the focus in
        // the island it must resolve to the slotted wx window, not the TLW.
        const wxWindow *ff = wxWindow::FindFocus();
        wxString ffDesc = "(aucune)";
        if ( ff )
        {
            ffDesc = ff->GetClassInfo()->GetClassName();
            if ( ff == m_slotButton )
                ffDesc += " = LA FENETRE-SLOT (hook DoFindFocus OK)";
            else if ( ff == this )
                ffDesc += " = la frame";
        }

        m_log->Add(wxString::Format(
            "F3: ::GetFocus()=%p classe='%s' => %s | FindFocus => %s",
            (void*)focus, wxString(cls), where, ffDesc));
    }

    void ShowTestDialog()
    {
        if ( !m_host || !m_host->GetXamlRoot() )
            return;

        using namespace winrt::Windows::Foundation;
        using MUXC::ContentDialog;
        using MUXC::ContentDialogResult;

        m_log->Add("F5: ouverture du ContentDialog sur le XamlRoot du host");

        try
        {
            ContentDialog dialog;
            dialog.XamlRoot(m_host->GetXamlRoot());
            dialog.Title(winrt::box_value(
                winrt::hstring(wxString::FromUTF8(
                    "Spike S4 — dialogue intégré").ToStdWstring())));
            dialog.Content(winrt::box_value(
                winrt::hstring(wxString::FromUTF8(
                    "Le voile doit couvrir TOUT le client, panels wx "
                    "compris.").ToStdWstring())));
            dialog.PrimaryButtonText(L"OK");
            dialog.CloseButtonText(L"Fermer");
            dialog.DefaultButton(MUXC::ContentDialogButton::Primary);

            ContentDialogResult result = ContentDialogResult::None;
            bool done = false;
            wxEventLoop *running = nullptr;

            auto operation = dialog.ShowAsync();
            operation.Completed(
                [&](IAsyncOperation<ContentDialogResult> const& async,
                    AsyncStatus status)
                {
                    if ( status == AsyncStatus::Completed )
                        result = async.GetResults();
                    done = true;
                    if ( running )
                        running->Exit();
                });

            wxEventLoop loop;
            if ( !done )
            {
                running = &loop;
                loop.Run();
                running = nullptr;
            }

            m_log->Add(wxString::Format("F5: dialogue fermé (résultat=%d)",
                                        int(result)));
        }
        catch ( const winrt::hresult_error& e )
        {
            m_log->Add(wxString::Format("F5: exception WinRT: %s",
                                        wxString(e.message().c_str())));
        }
    }

    void ProbePointerTarget()
    {
        POINT pt;
        if ( !::GetCursorPos(&pt) )
            return;

        RECT rc;
        ::GetWindowRect(GetHwndOf(this), &rc);
        if ( !::PtInRect(&rc, pt) )
            return;

        const HWND target = ::WindowFromPoint(pt);
        if ( target == m_lastProbe )
            return;
        m_lastProbe = target;

        wchar_t cls[128] = L"";
        if ( target )
            ::GetClassNameW(target, cls, WXSIZEOF(cls));

        wxString who;
        if ( target == GetHwndOf(m_left) )
            who = "panel GAUCHE";
        else if ( target == GetHwndOf(m_right) )
            who = "panel DROIT";
        else if ( target == GetHwndOf(m_log) )
            who = "panel LOG";
        else if ( m_wanderer && target == GetHwndOf(m_wanderer) )
            who = "la BALADEUSE";
        else if ( target == GetHwndOf(this) )
            who = "la FRAME";
        else
            who = wxString::Format("'%s'", wxString(cls));

        m_log->Add("sonde WindowFromPoint: le curseur cible " + who);
    }

    LogPanel *m_log = nullptr;
    PaintPanel *m_left = nullptr;
    PaintPanel *m_right = nullptr;
    WandererWindow *m_wanderer = nullptr;
    SlotButtonWindow *m_slotButton = nullptr;

    wxWinUITopLevelHost *m_host = nullptr;

    MUXC::Border m_alphaBorder{nullptr};
    MUXC::TextBlock m_caption{nullptr};
    MUXC::Button m_button{nullptr};
    MUXC::TextBlock m_clickInfo{nullptr};
    MUXC::TextBox m_textBox{nullptr};
    MUXC::ScrollViewer m_scroller{nullptr};
    MUXC::ProgressRing m_ring{nullptr};
    winrt::event_token m_renderToken{};
    int m_renderTicks = 0;
    ULONGLONG m_renderWindowStart = 0;

    wxTimer m_timer;
    wxPoint m_wandererVel = wxPoint(14, 9);
    HWND m_lastProbe = nullptr;
    int m_xamlClicks = 0;
    wxDialog *m_modeless = nullptr;
    wxFrame *m_secondFrame = nullptr;
};

class SpikeApp : public wxApp
{
public:
    bool OnInit() override
    {
        if ( !wxApp::OnInit() )
            return false;

        (new SpikeFrame())->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(SpikeApp);
