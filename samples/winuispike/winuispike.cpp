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
#include "wx/cmdline.h"
#include "wx/dcbuffer.h"
#include "wx/filefn.h"
#include "wx/filename.h"
#include "wx/log.h"
#include "wx/notebook.h"
#include "wx/scrolwin.h"
#include "wx/settings.h"
#include "wx/splitter.h"
#include "wx/utils.h"

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
#include <array>
#include <cmath>
#include <cstdio>
#include <deque>
#include <functional>
#include <iomanip>
#include <limits>
#include <locale>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace MUX  = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

namespace
{

int g_tick = 0;

// ----------------------------------------------------------------------------
// Message-queue probe: tallies every message the GUI thread actually retrieves
// (WH_GETMESSAGE, PM_REMOVE) and dumps a per-second histogram into the run
// directory. It is deliberately opt-in: set WX_WINUI_DIAGNOSTICS=1 (or its
// compatibility alias) and WX_WINUI_DIAGNOSTICS_DIR=<run-dir>. The normal
// sample must not perturb the message queue or keep an always-active render
// probe.
// ----------------------------------------------------------------------------

HHOOK g_msgHook = nullptr;
ULONG g_msgCount[0x10000];
HWND g_msgLastHwnd[0x10000];
ULONGLONG g_msgWindowStart = 0;
FILE *g_probeLog = nullptr;
bool g_probeLogOpenAttempted = false;

void ReportSpikeDiagnosticError(const wxString& message)
{
    const std::wstring text =
        ("wxWinUI spike diagnostics: " + message + "\n").ToStdWstring();
    fputws(text.c_str(), stderr);
    ::OutputDebugStringW(text.c_str());
}

bool IsSpikeDiagnosticsActivationValue(wxString value)
{
    value.Trim(true);
    value.Trim(false);
    value.MakeLower();
    return value == "1" ||
           value == "true" ||
           value == "on" ||
           value == "yes";
}

bool ValidateSpikeDiagnosticsActivationValues()
{
    struct ActivationCase
    {
        const char *value;
        bool expected;
    };

    static constexpr std::array<ActivationCase, 13> cases = {{
        { "1", true },
        { "true", true },
        { "ON", true },
        { "  YeS  ", true },
        { "", false },
        { "0", false },
        { "false", false },
        { "off", false },
        { "no", false },
        { "typo", false },
        { "TRUEE", false },
        { "2", false },
        { " on please ", false }
    }};

    for ( const ActivationCase& item : cases )
    {
        if ( IsSpikeDiagnosticsActivationValue(
                 wxString::FromAscii(item.value)) != item.expected )
        {
            return false;
        }
    }
    return true;
}

bool SpikeDiagnosticsEnabled()
{
    static const bool enabled = []()
    {
        const auto isEnabled = [](const wxString& name)
        {
            wxString value;
            if ( !wxGetEnv(name, &value) )
                return false;

            return IsSpikeDiagnosticsActivationValue(value);
        };

        return isEnabled("WX_WINUI_DIAGNOSTICS") ||
               isEnabled("WX_WINUI_SPIKE_DIAGNOSTICS");
    }();
    return enabled;
}

wxString SpikeDiagnosticsDirectory()
{
    static const wxString directory = []()
    {
        wxString value;
        if ( !wxGetEnv("WX_WINUI_DIAGNOSTICS_DIR", &value) ||
                value.empty() )
        {
            ReportSpikeDiagnosticError(
                "WX_WINUI_DIAGNOSTICS_DIR is required when diagnostics "
                "are enabled");
            return wxString();
        }

        if ( !wxDirExists(value) &&
                !wxFileName::Mkdir(
                    value, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL) )
        {
            ReportSpikeDiagnosticError(
                wxString::Format("cannot create directory '%s'", value));
            return wxString();
        }
        return value;
    }();
    return directory;
}

FILE *ProbeLog()
{
    if ( !g_probeLogOpenAttempted )
    {
        g_probeLogOpenAttempted = true;
        if ( !SpikeDiagnosticsEnabled() )
            return nullptr;

        const wxString directory = SpikeDiagnosticsDirectory();
        if ( directory.empty() )
            return nullptr;

        const wxString path =
            wxFileName(directory, "spike-ticks.txt").GetFullPath();
        g_probeLog = _wfopen(path.wc_str(), L"a");
        if ( g_probeLog )
            fprintf(g_probeLog, "==== session pid=%lu ====\n",
                    ::GetCurrentProcessId());
        else
            ReportSpikeDiagnosticError(
                wxString::Format("cannot open log '%s'", path));
    }
    return g_probeLog;
}

void CloseProbeLog()
{
    if ( !g_probeLog )
        return;

    fprintf(g_probeLog, "==== session end pid=%lu ====\n",
            ::GetCurrentProcessId());
    fclose(g_probeLog);
    g_probeLog = nullptr;
    g_probeLogOpenAttempted = false;
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
ULONGLONG g_sentWindowStart = 0;
bool g_publishingSentCosts = false;
ULONGLONG g_sentPublishSequence = 0;
ULONGLONG g_lastSentPublishTimestamp = 0;
ULONGLONG g_lastSentPublishBatchAge = 0;
UINT g_lastSentPublishedOnlyMessage = 0;
bool g_lastSentPublishForced = false;

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

void PublishSentCosts(ULONGLONG now, bool force);
LRESULT CALLBACK SpikeGetMsgHook(int code, WPARAM wParam, LPARAM lParam);

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
    double elapsedMs = 0.0;
    if ( code >= 0 && g_cwpDepth > 0 )
    {
        const CWPRETSTRUCT * const ret =
            reinterpret_cast<CWPRETSTRUCT *>(lParam);
        const CwpFrame& top = g_cwpStack[g_cwpDepth - 1];
        if ( top.hwnd == ret->hwnd && top.msg == ret->message )
        {
            --g_cwpDepth;
            const double ms = QpcMs(QpcNow() - top.t0);
            elapsedMs = ms;
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
    const LRESULT result =
        ::CallNextHookEx(g_cwpRetHook, code, wParam, lParam);
    // A modal sent-message storm may stop WH_GETMESSAGE entirely. Publish
    // from the return hook itself, after forwarding the hook chain and after
    // popping our timing frame. A single slow wndproc is immediately useful;
    // ordinary traffic is batched to keep probe overhead bounded.
    if ( code >= 0 )
        PublishSentCosts(::GetTickCount64(), elapsedMs >= 50.0);
    return result;
}

void DumpSentCosts(
    FILE *log,
    ULONGLONG now,
    const std::unordered_map<UINT, SentCost>& costs)
{
    struct Entry { UINT msg; SentCost c; };
    Entry top[8] = {};
    for ( const auto& kv : costs )
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

    fprintf(log, "%llu sent:", now);
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
}

void PublishSentCosts(ULONGLONG now, bool force)
{
    if ( g_publishingSentCosts || g_sentCost.empty() )
        return;

    if ( !g_sentWindowStart )
        g_sentWindowStart = now;
    const ULONGLONG batchAge = now - g_sentWindowStart;
    if ( !force && batchAge < 250 )
        return;

    g_publishingSentCosts = true;
    std::unordered_map<UINT, SentCost> snapshot;
    snapshot.swap(g_sentCost);
    g_sentWindowStart = now;

    FILE * const log = ProbeLog();
    if ( log )
    {
        DumpSentCosts(log, now, snapshot);
        if ( fflush(log) == 0 && !ferror(log) )
        {
            ++g_sentPublishSequence;
            g_lastSentPublishTimestamp = now;
            g_lastSentPublishBatchAge = batchAge;
            g_lastSentPublishForced = force;
            g_lastSentPublishedOnlyMessage =
                snapshot.size() == 1 ? snapshot.begin()->first : 0;
        }
    }

    // Formatting/class-name lookup can synchronously re-enter USER32. Those
    // new samples accumulated in the now-empty global table and belong to the
    // next window; never clear them with this snapshot.
    g_publishingSentCosts = false;
}

void UninstallSpikeDiagnosticHooks()
{
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
    g_cwpDepth = 0;
}

bool InstallSpikeDiagnosticHooks(bool includeGetMessage)
{
    // This is an all-or-nothing transaction. In particular, never install a
    // hook when there is nowhere to publish its samples.
    if ( !ProbeLog() )
        return false;

    DWORD error = ERROR_SUCCESS;
    if ( includeGetMessage )
    {
        g_msgHook = ::SetWindowsHookExW(
            WH_GETMESSAGE, SpikeGetMsgHook,
            nullptr, ::GetCurrentThreadId());
        if ( !g_msgHook )
            error = ::GetLastError();
    }
    g_cwpHook = ::SetWindowsHookExW(
        WH_CALLWNDPROC, SpikeCallWndHook,
        nullptr, ::GetCurrentThreadId());
    if ( !g_cwpHook && error == ERROR_SUCCESS )
        error = ::GetLastError();
    g_cwpRetHook = ::SetWindowsHookExW(
        WH_CALLWNDPROCRET, SpikeCallWndRetHook,
        nullptr, ::GetCurrentThreadId());
    if ( !g_cwpRetHook && error == ERROR_SUCCESS )
        error = ::GetLastError();

    if ( (!includeGetMessage || g_msgHook) && g_cwpHook && g_cwpRetHook )
        return true;

    UninstallSpikeDiagnosticHooks();
    ::SetLastError(error != ERROR_SUCCESS ? error : ERROR_INVALID_HOOK_HANDLE);
    return false;
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
        }
        PublishSentCosts(now, false);
    }
    return ::CallNextHookEx(g_msgHook, code, wParam, lParam);
}

constexpr UINT kSentHookSelfTestMessage = WM_APP + 0x42;
constexpr LRESULT kSentHookSelfTestResult = 0x5733;

LRESULT CALLBACK SpikeSentHookSelfTestWndProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    if ( message == kSentHookSelfTestMessage )
    {
        // The return hook considers a >=50 ms wndproc immediately useful.
        // Refresh the batching origin just before returning: publication can
        // then only happen because WH_CALLWNDPROCRET requested a force flush,
        // not because the ordinary 250 ms window elapsed.
        ::Sleep(60);
        g_sentWindowStart = ::GetTickCount64();
        return kSentHookSelfTestResult;
    }

    return ::DefWindowProcW(hwnd, message, wParam, lParam);
}

int RunSpikeDiagnosticsSelfTest()
{
    constexpr wchar_t className[] =
        L"wxWinUISpikeSentHookSelfTestWindow";

    const auto fail = [](int code, const wxString& reason)
    {
        const ULONGLONG now = ::GetTickCount64();
        if ( FILE * const log = ProbeLog() )
        {
            fprintf(log, "%llu selftest: FAIL code=%d %s\n",
                    now, code, reason.utf8_str().data());
            fflush(log);
        }
        ReportSpikeDiagnosticError(
            wxString::Format("self-test failed (exit %d): %s",
                             code, reason));
        return code;
    };

    if ( !ValidateSpikeDiagnosticsActivationValues() )
        return fail(29, "diagnostics activation-token matrix failed");

    if ( !SpikeDiagnosticsEnabled() )
    {
        return fail(
            20,
            "diagnostics are disabled; set WX_WINUI_DIAGNOSTICS=1");
    }
    if ( !ProbeLog() )
        return fail(21, "the probe log could not be opened");

    const HINSTANCE instance = ::GetModuleHandleW(nullptr);
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = SpikeSentHookSelfTestWndProc;
    wc.hInstance = instance;
    wc.lpszClassName = className;
    const ATOM atom = ::RegisterClassExW(&wc);
    if ( !atom && ::GetLastError() != ERROR_CLASS_ALREADY_EXISTS )
        return fail(22, "RegisterClassExW failed");

    const HWND window = ::CreateWindowExW(
        0, className, L"", 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, instance, nullptr);
    if ( !window )
    {
        if ( atom )
            ::UnregisterClassW(className, instance);
        return fail(23, "the message-only test window could not be created");
    }

    // Install no WH_GETMESSAGE hook: the receipt below is therefore a direct
    // proof that WH_CALLWNDPROCRET can publish while GetMessage is absent.
    if ( !InstallSpikeDiagnosticHooks(false) )
    {
        const DWORD error = ::GetLastError();
        ::DestroyWindow(window);
        if ( atom )
            ::UnregisterClassW(className, instance);
        return fail(
            24,
            wxString::Format("sent-message hooks failed to install "
                             "(Win32 error %lu)", error));
    }

    g_sentCost.clear();
    g_sentWindowStart = ::GetTickCount64();
    g_lastSentPublishedOnlyMessage = 0;
    g_lastSentPublishForced = false;
    const ULONGLONG sequenceBefore = g_sentPublishSequence;

    const LRESULT result =
        ::SendMessageW(window, kSentHookSelfTestMessage, 0, 0);

    UninstallSpikeDiagnosticHooks();
    ::DestroyWindow(window);
    if ( atom )
        ::UnregisterClassW(className, instance);

    if ( result != kSentHookSelfTestResult )
        return fail(25, "the synchronous test message returned unexpectedly");
    if ( g_sentPublishSequence != sequenceBefore + 1 ||
            g_lastSentPublishedOnlyMessage != kSentHookSelfTestMessage )
    {
        return fail(
            26,
            "WH_CALLWNDPROCRET did not publish the isolated test message");
    }
    if ( !g_lastSentPublishForced || g_lastSentPublishBatchAge >= 250 )
    {
        return fail(
            27,
            wxString::Format("publication did not bypass the 250 ms batch "
                             "window (forced=%d age=%llu ms)",
                             g_lastSentPublishForced,
                             g_lastSentPublishBatchAge));
    }
    if ( !g_lastSentPublishTimestamp )
        return fail(28, "the sent-message line has no timestamp");

    fprintf(ProbeLog(),
            "%llu selftest: PASS callwndprocret-only force-age=%llu ms\n",
            ::GetTickCount64(), g_lastSentPublishBatchAge);
    fflush(ProbeLog());
    CloseProbeLog();
    return 0;
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
        m_diagnosticsReady =
            SpikeDiagnosticsEnabled() && ProbeLog() != nullptr;
        if ( SpikeDiagnosticsEnabled() && !m_diagnosticsReady )
        {
            m_log->Add("diagnostics indisponibles : journal non ouvert, "
                       "aucun hook installé");
        }

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

        if ( m_diagnosticsReady )
        {
            // Open the file before installing hooks: lazy path/file APIs from
            // inside WH_CALLWNDPROCRET would otherwise pollute the first
            // timing batch and risk recursive initialization.
            if ( !InstallSpikeDiagnosticHooks(true) )
            {
                const DWORD error = ::GetLastError();
                ReportSpikeDiagnosticError(wxString::Format(
                    "hooks were not installed (Win32 error %lu)", error));
                m_log->Add(wxString::Format(
                    "diagnostics partielles : hooks non installés "
                    "(erreur Win32 %lu)", error));
            }
            else
            {
                m_log->Add(wxString::Format(
                    "diagnostics spike dans %s (hooks complets)",
                    SpikeDiagnosticsDirectory()));
            }
        }

        m_timer.SetOwner(this);
        Bind(wxEVT_TIMER, [this](wxTimerEvent&) {
            ++g_tick;
            m_left->Refresh(false);
            m_right->Refresh(false);
            BounceWanderer();
            if ( m_diagnosticsReady )
                ProbePointerTarget();
        });
        m_timer.Start(400);

        m_log->Add("=== harnais phase 1 : la baladeuse teste le moteur de "
                   "synchro (suivi, clip, F4 show/hide) ===");
    }

    ~SpikeFrame() override
    {
        m_timer.Stop();
        if ( m_renderToken.value )
        {
            try
            {
                MUXM::CompositionTarget::Rendering(m_renderToken);
            }
            catch ( const winrt::hresult_error& )
            {
            }
            m_renderToken = {};
        }
        UninstallSpikeDiagnosticHooks();
        if ( m_diagnosticsReady )
        {
            PublishSentCosts(::GetTickCount64(), true);
            if ( g_msgWindowStart )
                DumpMessageHistogram(::GetTickCount64());
        }
        CloseProbeLog();
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

        if ( m_diagnosticsReady )
        {
            // Render-pipeline probe: an active ProgressRing plus a per-second
            // UI-thread render count. Keeping both behind the flag is part of
            // the measurement contract: the default harness is silent.
            m_ring = MUXC::ProgressRing();
            m_ring.IsActive(true);
            m_ring.Width(44);
            m_ring.Height(44);
            m_ring.IsHitTestVisible(false);
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
        }

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

        if ( m_ring )
        {
            MUXC::Canvas::SetLeft(m_ring, dip(l.x + 24));
            MUXC::Canvas::SetTop(m_ring, dip(l.y + 24));
        }
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
                m_log->Add("F8: wxMessageBox (presenter Window par défaut)");
                wxMessageBox("Message via le presenter Window (Overlay opt-in).",
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
        // Keep this guard even though the timer currently checks it too:
        // cursor sampling is instrumentation, not part of the animated
        // wanderer. Any future caller must preserve the default-off policy.
        if ( !m_diagnosticsReady )
            return;

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
    bool m_diagnosticsReady = false;

    wxTimer m_timer;
    wxPoint m_wandererVel = wxPoint(14, 9);
    HWND m_lastProbe = nullptr;
    int m_xamlClicks = 0;
    wxDialog *m_modeless = nullptr;
    wxFrame *m_secondFrame = nullptr;
};

namespace
{

// ----------------------------------------------------------------------------
// Deterministic root-pointer scenarios.
//
// This mode never reads or moves the real pointer and never activates its
// window. It feeds normalized samples through the same adapter seam used by
// OnRootPointer(), and observes the actual synchronous Win32 messages emitted
// by the host. The PowerShell campaign runner adds the process-level watchdog
// needed to bound a deliberately synchronous SendMessage() path.
// ----------------------------------------------------------------------------

constexpr int kPointerScenarioExitCli = 30;
constexpr int kPointerScenarioExitSetup = 31;
constexpr int kPointerScenarioExitHost = 32;
constexpr int kPointerScenarioExitFailed = 33;
constexpr int kPointerScenarioExitTimeout = 34;
constexpr int kPointerScenarioExitWrite = 35;
constexpr int kPointerScenarioExitException = 36;

constexpr int kPointerScenarioFrameWidth = 360;
constexpr int kPointerScenarioFrameHeight = 260;
constexpr int kPointerScenarioOffscreenMargin = 64;

bool GetPointerScenarioOffscreenPosition(wxPoint *position)
{
    if ( !position )
        return false;

    const long long virtualLeft =
        ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const long long virtualWidth =
        ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    if ( virtualWidth <= 0 )
        return false;

    const long long virtualRight = virtualLeft + virtualWidth;
    // Native mouse messages pack both coordinates into signed 16-bit words.
    // Keep the entire harness window in that representable range so moving it
    // off-screen cannot silently turn the coordinate assertions into a test
    // of LPARAM truncation instead.
    const long long minimum = (std::numeric_limits<short>::min)();
    const long long maximum = (std::numeric_limits<short>::max)();

    // Prefer the left edge, but retain a representable right-edge fallback
    // when the virtual desktop starts below the mouse-message coordinate
    // range.
    const long long left =
        virtualLeft -
        kPointerScenarioFrameWidth -
        kPointerScenarioOffscreenMargin;
    if ( left >= minimum &&
         left <= maximum - kPointerScenarioFrameWidth )
    {
        *position = wxPoint(static_cast<int>(left), 0);
        return true;
    }

    const long long right =
        virtualRight + kPointerScenarioOffscreenMargin;
    if ( right >= minimum &&
         right <= maximum - kPointerScenarioFrameWidth )
    {
        *position = wxPoint(static_cast<int>(right), 0);
        return true;
    }

    return false;
}

bool IsPointerScenarioWindowOutsideVirtualDesktop(HWND hwnd)
{
    RECT window = {};
    if ( !hwnd || !::GetWindowRect(hwnd, &window) )
        return false;

    const long long virtualLeft =
        ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const long long virtualTop =
        ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const long long virtualWidth =
        ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const long long virtualHeight =
        ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if ( virtualWidth <= 0 || virtualHeight <= 0 )
        return false;

    const long long virtualRight = virtualLeft + virtualWidth;
    const long long virtualBottom = virtualTop + virtualHeight;
    return window.right <= virtualLeft ||
           window.left >= virtualRight ||
           window.bottom <= virtualTop ||
           window.top >= virtualBottom;
}

struct PointerScenarioMessageCounts
{
    unsigned hitTests = 0;
    unsigned setCursors = 0;
    unsigned clientMoves = 0;
    unsigned nonClientMoves = 0;
    unsigned clientLeaves = 0;
    unsigned nonClientLeaves = 0;
    unsigned hoverEnters = 0;
    unsigned hoverLeaves = 0;
    unsigned leftDowns = 0;
    unsigned leftUps = 0;
    unsigned cancelModes = 0;
};

struct PointerScenarioResult
{
    std::string name;
    bool passed = true;
    std::string failure;
    unsigned samples = 0;
    unsigned preparedActions = 0;
    unsigned completed = 0;
    unsigned policyFiltered = 0;
    unsigned slotReleaseConsumed = 0;
    unsigned failedClosed = 0;
    unsigned superseded = 0;
    unsigned routed = 0;
    unsigned xaml = 0;
    unsigned outside = 0;
    unsigned indeterminate = 0;
    unsigned stormSuppressed = 0;
    unsigned breakCycles = 0;
    unsigned cancelSource = 0;
    unsigned boundaryCalls = 0;
    unsigned boundaryCompleted = 0;
    unsigned interruptions = 0;
    PointerScenarioMessageCounts messages;
    ULONGLONG elapsedMs = 0;
};

std::string JsonEscape(const std::string& value)
{
    static constexpr char hex[] = "0123456789abcdef";
    std::string escaped;
    escaped.reserve(value.size() + 8);
    for ( const unsigned char ch : value )
    {
        switch ( ch )
        {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if ( ch < 0x20 )
                {
                    escaped += "\\u00";
                    escaped += hex[(ch >> 4) & 0x0f];
                    escaped += hex[ch & 0x0f];
                }
                else
                {
                    escaped += static_cast<char>(ch);
                }
                break;
        }
    }
    return escaped;
}

class PointerScenarioTarget : public wxWindow
{
public:
    PointerScenarioTarget(wxWindow *parent,
                          const wxPoint& position,
                          const wxSize& size)
        : wxWindow(parent, wxID_ANY, position, size)
    {
    }

    void ResetObservation()
    {
        m_counts = PointerScenarioMessageCounts();
        m_hoverArea = HoverArea::None;
        m_setCursorCallback = nullptr;
        m_captureOnButtonDown = false;
    }

    void SetHitTest(LRESULT hitTest) { m_hitTest = hitTest; }
    void CaptureOnButtonDown(bool capture = true)
    {
        m_captureOnButtonDown = capture;
    }
    void OnNextSetCursor(std::function<void ()> callback)
    {
        m_setCursorCallback = std::move(callback);
    }
    void ClearSetCursorCallback()
    {
        m_setCursorCallback = nullptr;
    }
    bool HasSetCursorCallback() const
    {
        return static_cast<bool>(m_setCursorCallback);
    }

    const PointerScenarioMessageCounts& GetCounts() const
    {
        return m_counts;
    }

    WXLRESULT MSWWindowProc(WXUINT message,
                            WXWPARAM wParam,
                            WXLPARAM lParam) override
    {
        wxUnusedVar(wParam);
        wxUnusedVar(lParam);

        switch ( message )
        {
            case WM_NCHITTEST:
                ++m_counts.hitTests;
                return m_hitTest;

            case WM_SETCURSOR:
            {
                ++m_counts.setCursors;
                std::function<void ()> callback =
                    std::move(m_setCursorCallback);
                m_setCursorCallback = nullptr;
                if ( callback )
                    callback();
                return TRUE;
            }

            case WM_MOUSEMOVE:
                ++m_counts.clientMoves;
                ObserveMove(HoverArea::Client);
                return 0;

            case WM_NCMOUSEMOVE:
                ++m_counts.nonClientMoves;
                ObserveMove(HoverArea::NonClient);
                return 0;

            case WM_MOUSELEAVE:
                ++m_counts.clientLeaves;
                ObserveLeave();
                return 0;

            case WM_NCMOUSELEAVE:
                ++m_counts.nonClientLeaves;
                ObserveLeave();
                return 0;

            case WM_LBUTTONDOWN:
                ++m_counts.leftDowns;
                if ( m_captureOnButtonDown )
                    ::SetCapture(GetHwndOf(this));
                return 0;

            case WM_LBUTTONUP:
                ++m_counts.leftUps;
                if ( ::GetCapture() == GetHwndOf(this) )
                    (void)::ReleaseCapture();
                return 0;

            case WM_CANCELMODE:
                ++m_counts.cancelModes;
                return 0;
        }

        return wxWindow::MSWWindowProc(message, wParam, lParam);
    }

private:
    enum class HoverArea
    {
        None,
        Client,
        NonClient
    };

    void ObserveMove(HoverArea area)
    {
        if ( m_hoverArea != area )
        {
            ++m_counts.hoverEnters;
            m_hoverArea = area;
        }
    }

    void ObserveLeave()
    {
        if ( m_hoverArea != HoverArea::None )
            ++m_counts.hoverLeaves;
        m_hoverArea = HoverArea::None;
    }

    LRESULT m_hitTest = HTCLIENT;
    HoverArea m_hoverArea = HoverArea::None;
    bool m_captureOnButtonDown = false;
    PointerScenarioMessageCounts m_counts;
    std::function<void ()> m_setCursorCallback;
};

class PointerScenarioFrame : public wxFrame
{
public:
    PointerScenarioFrame()
        : wxFrame(nullptr, wxID_ANY, "wxWinUI pointer scenarios",
                  wxDefaultPosition,
                  wxSize(kPointerScenarioFrameWidth,
                         kPointerScenarioFrameHeight),
                  wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW | wxBORDER_NONE)
    {
        Bind(wxEVT_ACTIVATE,
             [this](wxActivateEvent& event)
             {
                 if ( event.GetActive() )
                     ObserveActivation();
                 event.Skip();
             });

        SetClientSize(320, 220);
        m_target = new PointerScenarioTarget(
            this, wxPoint(40, 40), wxSize(220, 140));
        m_target->Show();

        const HWND hwnd = GetHwndOf(this);
        if ( !hwnd )
            return;

        ::SetLastError(ERROR_SUCCESS);
        const LONG_PTR oldStyle = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if ( !oldStyle && ::GetLastError() != ERROR_SUCCESS )
            return;
        ::SetLastError(ERROR_SUCCESS);
        (void)::SetWindowLongPtr(
            hwnd, GWL_EXSTYLE,
            oldStyle | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
        if ( ::GetLastError() != ERROR_SUCCESS )
            return;

        wxPoint offscreen;
        if ( !GetPointerScenarioOffscreenPosition(&offscreen) )
            return;
        if ( !::SetWindowPos(
                 hwnd, HWND_BOTTOM,
                 offscreen.x, offscreen.y,
                 kPointerScenarioFrameWidth,
                 kPointerScenarioFrameHeight,
                 SWP_FRAMECHANGED | SWP_NOACTIVATE |
                 SWP_NOOWNERZORDER) )
        {
            return;
        }

        ShowWithoutActivating();
        if ( !::SetWindowPos(
                 hwnd, HWND_BOTTOM,
                 offscreen.x, offscreen.y,
                 kPointerScenarioFrameWidth,
                 kPointerScenarioFrameHeight,
                 SWP_FRAMECHANGED | SWP_NOACTIVATE |
                 SWP_NOOWNERZORDER | SWP_SHOWWINDOW) )
        {
            return;
        }

        m_host = wxWinUITopLevelHost::ForWindow(this, true);
        m_setupOk = m_host != nullptr &&
                    IsPointerScenarioWindowOutsideVirtualDesktop(hwnd) &&
                    (::GetWindowLongPtr(hwnd, GWL_EXSTYLE) &
                        WS_EX_NOACTIVATE) != 0;
        ObserveActivation();
    }

    bool IsSetupOk() const { return m_setupOk; }
    bool HasHost() const { return m_host != nullptr; }
    bool WasActivated() const { return m_wasActivated; }

    std::vector<PointerScenarioResult> RunScenarios(
        ULONGLONG absoluteDeadline)
    {
        m_deadline = absoluteDeadline;
        std::vector<PointerScenarioResult> results;
        results.reserve(9);
        results.push_back(RunFixedPixel());
        results.push_back(RunTwoPixelOscillation());
        results.push_back(RunClientNonClient());
        results.push_back(RunLogicalXamlCrossing());
        results.push_back(RunRootBoundary());
        results.push_back(RunRefreshFailure());
        results.push_back(RunSetCursorReentry());
        results.push_back(RunReleaseOutside());
        results.push_back(RunCaptureLost());
        return results;
    }

private:
    static void Fail(PointerScenarioResult& result,
                     const std::string& reason)
    {
        if ( result.passed )
        {
            result.passed = false;
            result.failure = reason;
        }
    }

    static void Require(PointerScenarioResult& result,
                        bool condition,
                        const std::string& reason)
    {
        if ( !condition )
            Fail(result, reason);
    }

    bool CheckDeadline(PointerScenarioResult& result) const
    {
        if ( ::GetTickCount64() <= m_deadline )
            return true;

        Fail(result, "suite deadline exceeded");
        return false;
    }

    void ObserveActivation()
    {
        const HWND frame = GetHwndOf(this);
        const HWND focus = ::GetFocus();
        if ( frame &&
             (::GetForegroundWindow() == frame ||
              ::GetActiveWindow() == frame ||
              focus == frame ||
              (focus && ::IsChild(frame, focus))) )
        {
            m_wasActivated = true;
        }
    }

    wxWinUIPointerSample MakeMove(int offsetX = 0,
                                  int offsetY = 0)
    {
        RECT rect = {};
        if ( !m_target ||
             !::GetWindowRect(GetHwndOf(m_target), &rect) )
        {
            return wxWinUIPointerSample();
        }

        wxWinUIPointerSample sample;
        sample.device = wxWinUIInputDevice::Mouse;
        sample.kind = wxWinUIInputKind::Move;
        sample.pointerId = 1;
        sample.isPrimary = true;
        sample.screenX = (rect.left + rect.right) / 2 + offsetX;
        sample.screenY = (rect.top + rect.bottom) / 2 + offsetY;
        sample.timestamp = ++m_timestamp;
        return sample;
    }

    wxWinUIRootPointerOutcome Route(
        PointerScenarioResult& result,
        const wxWinUIPointerSample& sample,
        bool sourceAlreadyHandled = false)
    {
        ++result.samples;
        const wxWinUIRootPointerOutcome outcome =
            m_host->TestRouteRootPointerSample(
                sample, sourceAlreadyHandled);
        RecordOutcome(result, outcome);
        ObserveActivation();
        (void)CheckDeadline(result);
        return outcome;
    }

    static void RecordOutcome(
        PointerScenarioResult& result,
        const wxWinUIRootPointerOutcome& outcome)
    {
        result.preparedActions += outcome.preparedActionCount;
        if ( outcome.cancelSource )
            ++result.cancelSource;

        switch ( outcome.status )
        {
            case wxWinUIRootPointerStatus::Completed:
                ++result.completed;
                break;
            case wxWinUIRootPointerStatus::PolicyFiltered:
                ++result.policyFiltered;
                break;
            case wxWinUIRootPointerStatus::SlotReleaseConsumed:
                ++result.slotReleaseConsumed;
                break;
            case wxWinUIRootPointerStatus::FailedClosed:
                ++result.failedClosed;
                break;
            case wxWinUIRootPointerStatus::Superseded:
                ++result.superseded;
                break;
        }

        switch ( outcome.disposition )
        {
            case wxWinUIInputDisposition::Routed:
                ++result.routed;
                break;
            case wxWinUIInputDisposition::Xaml:
                ++result.xaml;
                break;
            case wxWinUIInputDisposition::Outside:
                ++result.outside;
                break;
            case wxWinUIInputDisposition::StormSuppressed:
                ++result.stormSuppressed;
                break;
            case wxWinUIInputDisposition::Indeterminate:
            case wxWinUIInputDisposition::InvalidTarget:
            case wxWinUIInputDisposition::SecondaryPointer:
            case wxWinUIInputDisposition::CompatibilityDuplicate:
                ++result.indeterminate;
                break;
        }

        if ( outcome.stormDecision ==
                wxWinUIInputStormDecision::BreakCycle )
        {
            ++result.breakCycles;
        }
    }

    bool RouteBoundary(PointerScenarioResult& result,
                       bool entered,
                       const wxWinUIPointerSample& sample)
    {
        ++result.samples;
        ++result.boundaryCalls;
        const bool completed =
            m_host->TestRouteRootPointerBoundary(entered, sample);
        if ( completed )
            ++result.boundaryCompleted;
        ObserveActivation();
        (void)CheckDeadline(result);
        return completed;
    }

    void Interrupt(PointerScenarioResult& result,
                   bool captureLost,
                   const wxWinUIPointerSample& sample)
    {
        ++result.samples;
        ++result.interruptions;
        m_host->TestHandleRootPointerInterrupted(captureLost, sample);
        ObserveActivation();
        (void)CheckDeadline(result);
    }

    void BeginScenario(PointerScenarioResult& result)
    {
        result.passed = true;
        if ( m_target &&
             ::GetCapture() == GetHwndOf(m_target) )
        {
            (void)::ReleaseCapture();
        }
        m_target->SetHitTest(HTCLIENT);
        m_target->CaptureOnButtonDown(false);
        m_target->ClearSetCursorCallback();

        // Retire any native hover left by the preceding scenario through the
        // production XAML-owned classification. Do not count this separator.
        PointerScenarioResult separator;
        separator.name = "separator";
        (void)Route(separator, MakeMove(), true);
        if ( separator.completed != 1 ||
             separator.xaml != 1 ||
             !separator.passed )
        {
            Fail(result, "could not establish the XAML separator state");
        }

        m_target->ResetObservation();
        result.samples = 0;
        result.preparedActions = 0;
        result.completed = 0;
        result.policyFiltered = 0;
        result.slotReleaseConsumed = 0;
        result.failedClosed = 0;
        result.superseded = 0;
        result.routed = 0;
        result.xaml = 0;
        result.outside = 0;
        result.indeterminate = 0;
        result.stormSuppressed = 0;
        result.breakCycles = 0;
        result.cancelSource = 0;
        result.boundaryCalls = 0;
        result.boundaryCompleted = 0;
        result.interruptions = 0;
    }

    void FinishScenario(PointerScenarioResult& result,
                        ULONGLONG started)
    {
        Require(result, !m_target->HasSetCursorCallback(),
                "one-shot WM_SETCURSOR callback was not consumed");
        m_target->ClearSetCursorCallback();
        result.elapsedMs = ::GetTickCount64() - started;
        result.messages = m_target->GetCounts();
        Require(result, !m_wasActivated,
                "the off-screen no-activate frame became active");
        (void)CheckDeadline(result);
    }

    void RequireOrdinaryNativeMove(
        PointerScenarioResult& result,
        const wxWinUIRootPointerOutcome& outcome)
    {
        Require(result,
                outcome.status == wxWinUIRootPointerStatus::Completed,
                "native MOVE did not complete");
        Require(result,
                outcome.disposition == wxWinUIInputDisposition::Routed,
                "native MOVE was not routed");
        Require(result, outcome.cancelSource,
                "native MOVE did not consume its XAML source");
        Require(result,
                outcome.stormDecision ==
                    wxWinUIInputStormDecision::Deliver,
                "ordinary MOVE was changed by the storm breaker");
    }

    PointerScenarioResult RunFixedPixel()
    {
        PointerScenarioResult result;
        result.name = "move-fixed-pixel";
        const ULONGLONG started = ::GetTickCount64();
        BeginScenario(result);

        constexpr unsigned moveCount = 128;
        const wxWinUIPointerSample sample = MakeMove();
        for ( unsigned i = 0;
              i != moveCount && CheckDeadline(result);
              ++i )
        {
            wxWinUIPointerSample current = sample;
            current.timestamp = ++m_timestamp;
            RequireOrdinaryNativeMove(result, Route(result, current));
        }

        const PointerScenarioMessageCounts counts = m_target->GetCounts();
        Require(result, result.samples == moveCount,
                "fixed-pixel sample count mismatch");
        Require(result, result.completed == moveCount &&
                        result.routed == moveCount,
                "fixed-pixel outcomes were not all routed");
        Require(result, result.stormSuppressed == 0 &&
                        result.breakCycles == 0,
                "fixed-pixel MOVE was spuriously suppressed");
        Require(result, counts.clientMoves == moveCount &&
                        counts.nonClientMoves == 0,
                "fixed-pixel native MOVE count mismatch");
        Require(result, counts.hoverEnters == 1 &&
                        counts.hoverLeaves == 0,
                "fixed-pixel hover prefix was duplicated");
        FinishScenario(result, started);
        return result;
    }

    PointerScenarioResult RunTwoPixelOscillation()
    {
        PointerScenarioResult result;
        result.name = "move-two-pixel-oscillation";
        const ULONGLONG started = ::GetTickCount64();
        BeginScenario(result);

        constexpr unsigned moveCount = 128;
        for ( unsigned i = 0;
              i != moveCount && CheckDeadline(result);
              ++i )
        {
            const int offset = (i & 1) ? 1 : -1;
            RequireOrdinaryNativeMove(
                result, Route(result, MakeMove(offset, 0)));
        }

        const PointerScenarioMessageCounts counts = m_target->GetCounts();
        Require(result, result.samples == moveCount,
                "oscillation sample count mismatch");
        Require(result, result.completed == moveCount &&
                        result.routed == moveCount,
                "oscillation outcomes were not all routed");
        Require(result, result.stormSuppressed == 0 &&
                        result.breakCycles == 0,
                "real two-pixel movement was suppressed");
        Require(result, counts.clientMoves == moveCount,
                "oscillation native MOVE count mismatch");
        Require(result, counts.hoverEnters == 1 &&
                        counts.hoverLeaves == 0,
                "oscillation manufactured hover crossings");
        FinishScenario(result, started);
        return result;
    }

    PointerScenarioResult RunClientNonClient()
    {
        PointerScenarioResult result;
        result.name = "client-nonclient-crossing";
        const ULONGLONG started = ::GetTickCount64();
        BeginScenario(result);

        m_target->SetHitTest(HTCLIENT);
        RequireOrdinaryNativeMove(result, Route(result, MakeMove()));
        m_target->SetHitTest(HTVSCROLL);
        RequireOrdinaryNativeMove(result, Route(result, MakeMove()));
        m_target->SetHitTest(HTCLIENT);
        RequireOrdinaryNativeMove(result, Route(result, MakeMove()));

        const PointerScenarioMessageCounts counts = m_target->GetCounts();
        Require(result, result.samples == 3 &&
                        result.completed == 3 &&
                        result.routed == 3,
                "client/non-client outcomes mismatch");
        Require(result, counts.clientMoves == 2 &&
                        counts.nonClientMoves == 1,
                "client/non-client MOVE messages mismatch");
        Require(result, counts.clientLeaves == 1 &&
                        counts.nonClientLeaves == 1,
                "client/non-client Leave messages mismatch");
        Require(result, counts.hoverEnters == 3 &&
                        counts.hoverLeaves == 2,
                "client/non-client hover ordering mismatch");
        FinishScenario(result, started);
        return result;
    }

    PointerScenarioResult RunLogicalXamlCrossing()
    {
        PointerScenarioResult result;
        result.name = "native-xaml-logical-crossing";
        const ULONGLONG started = ::GetTickCount64();
        BeginScenario(result);

        const wxWinUIPointerSample sample = MakeMove();
        RequireOrdinaryNativeMove(result, Route(result, sample));

        wxWinUIPointerSample xamlSample = sample;
        xamlSample.timestamp = ++m_timestamp;
        const wxWinUIRootPointerOutcome xaml =
            Route(result, xamlSample, true);
        Require(result,
                xaml.status == wxWinUIRootPointerStatus::Completed &&
                xaml.disposition == wxWinUIInputDisposition::Xaml &&
                !xaml.cancelSource,
                "logical XAML entry did not retain XAML ownership");

        xamlSample.timestamp = ++m_timestamp;
        const wxWinUIRootPointerOutcome repeatedXaml =
            Route(result, xamlSample, true);
        Require(result,
                repeatedXaml.status ==
                    wxWinUIRootPointerStatus::Completed &&
                repeatedXaml.disposition ==
                    wxWinUIInputDisposition::Xaml,
                "repeated logical XAML sample did not complete");

        RequireOrdinaryNativeMove(result, Route(result, MakeMove()));

        const PointerScenarioMessageCounts counts = m_target->GetCounts();
        Require(result, result.samples == 4 &&
                        result.completed == 4 &&
                        result.routed == 2 &&
                        result.xaml == 2,
                "native/XAML outcome counts mismatch");
        Require(result, counts.clientMoves == 2 &&
                        counts.clientLeaves == 1 &&
                        counts.nonClientLeaves == 0,
                "native/XAML message counts mismatch");
        Require(result, counts.hoverEnters == 2 &&
                        counts.hoverLeaves == 1,
                "logical XAML crossing duplicated hover");
        FinishScenario(result, started);
        return result;
    }

    PointerScenarioResult RunRootBoundary()
    {
        PointerScenarioResult result;
        result.name = "root-boundary-enter-exit";
        const ULONGLONG started = ::GetTickCount64();
        BeginScenario(result);

        RequireOrdinaryNativeMove(result, Route(result, MakeMove()));

        wxWinUIPointerSample boundary = MakeMove();
        Require(result, RouteBoundary(result, false, boundary),
                "root EXIT boundary did not complete");
        boundary.timestamp = ++m_timestamp;
        Require(result, RouteBoundary(result, false, boundary),
                "repeated root EXIT boundary did not complete");
        boundary.timestamp = ++m_timestamp;
        Require(result, RouteBoundary(result, true, boundary),
                "root ENTER boundary did not complete");

        RequireOrdinaryNativeMove(result, Route(result, MakeMove()));

        const PointerScenarioMessageCounts counts = m_target->GetCounts();
        Require(result, result.samples == 5 &&
                        result.completed == 2 &&
                        result.routed == 2 &&
                        result.boundaryCalls == 3 &&
                        result.boundaryCompleted == 3,
                "root boundary outcome counts mismatch");
        Require(result, counts.clientMoves == 2 &&
                        counts.clientLeaves == 1 &&
                        counts.nonClientLeaves == 0,
                "root boundary native messages mismatch");
        Require(result, counts.hoverEnters == 2 &&
                        counts.hoverLeaves == 1,
                "root boundary duplicated hover cleanup");
        FinishScenario(result, started);
        return result;
    }

    PointerScenarioResult RunRefreshFailure()
    {
        PointerScenarioResult result;
        result.name = "post-callback-refresh-failure";
        const ULONGLONG started = ::GetTickCount64();
        BeginScenario(result);

        m_target->SetHitTest(HTCLIENT);
        m_target->OnNextSetCursor(
            [this]()
            {
                // Keep HWND and geometry stable. Only the hit zone changes,
                // so the post-WM_SETCURSOR refresh is the rejecting proof.
                m_target->SetHitTest(HTVSCROLL);
            });

        const wxWinUIRootPointerOutcome rejected =
            Route(result, MakeMove());
        Require(result,
                rejected.status ==
                    wxWinUIRootPointerStatus::FailedClosed &&
                rejected.disposition ==
                    wxWinUIInputDisposition::Routed,
                "changed post-callback hit was not rejected");
        Require(result, rejected.cancelSource,
                "rejected native source was not consumed fail-closed");
        Require(result, !m_target->HasSetCursorCallback(),
                "refresh mutation callback was not consumed");
        m_target->ClearSetCursorCallback();

        PointerScenarioMessageCounts counts = m_target->GetCounts();
        Require(result, counts.setCursors == 1 &&
                        counts.clientMoves == 0 &&
                        counts.nonClientMoves == 0 &&
                        counts.clientLeaves == 0 &&
                        counts.nonClientLeaves == 0,
                "failed refresh published an orphan native message");

        m_target->SetHitTest(HTCLIENT);
        RequireOrdinaryNativeMove(result, Route(result, MakeMove()));
        counts = m_target->GetCounts();
        Require(result, result.samples == 2 &&
                        result.failedClosed == 1 &&
                        result.completed == 1,
                "refresh recovery outcome counts mismatch");
        Require(result, counts.clientMoves == 1 &&
                        counts.hoverEnters == 1 &&
                        counts.hoverLeaves == 0,
                "refresh recovery did not establish hover exactly once");
        FinishScenario(result, started);
        return result;
    }

    PointerScenarioResult RunSetCursorReentry()
    {
        PointerScenarioResult result;
        result.name = "setcursor-reentry";
        const ULONGLONG started = ::GetTickCount64();
        BeginScenario(result);

        struct ReentryState
        {
            wxWinUIPointerSample sample;
            wxWinUIRootPointerOutcome outcome;
            bool called = false;
        };
        const std::shared_ptr<ReentryState> reentry =
            std::make_shared<ReentryState>();
        reentry->sample = MakeMove();
        reentry->sample.timestamp = ++m_timestamp;
        m_target->OnNextSetCursor(
            [this, reentry]()
            {
                reentry->called = true;
                reentry->outcome =
                    m_host->TestRouteRootPointerSample(
                        reentry->sample, true);
            });

        const wxWinUIRootPointerOutcome outer =
            Route(result, MakeMove());
        Require(result, reentry->called,
                "WM_SETCURSOR reentry callback was not consumed");
        Require(result, !m_target->HasSetCursorCallback(),
                "WM_SETCURSOR reentry callback remained armed");
        m_target->ClearSetCursorCallback();
        if ( reentry->called )
        {
            ++result.samples;
            RecordOutcome(result, reentry->outcome);
            ObserveActivation();
            (void)CheckDeadline(result);
        }
        Require(result,
                reentry->outcome.status ==
                    wxWinUIRootPointerStatus::Completed &&
                reentry->outcome.disposition ==
                    wxWinUIInputDisposition::Xaml,
                "nested XAML route did not win reentry");
        Require(result,
                outer.status ==
                    wxWinUIRootPointerStatus::Superseded &&
                outer.disposition ==
                    wxWinUIInputDisposition::Routed,
                "stale outer route was not superseded");
        Require(result, outer.cancelSource,
                "superseded native source lost its fail-closed ownership");

        PointerScenarioMessageCounts counts = m_target->GetCounts();
        Require(result, counts.clientMoves == 0 &&
                        counts.nonClientMoves == 0 &&
                        counts.clientLeaves == 0 &&
                        counts.nonClientLeaves == 0,
                "superseded outer route published a native message");

        RequireOrdinaryNativeMove(result, Route(result, MakeMove()));
        counts = m_target->GetCounts();
        Require(result, result.samples == 3 &&
                        result.completed == 2 &&
                        result.superseded == 1 &&
                        result.xaml == 1 &&
                        result.routed == 2,
                "reentry outcome counts mismatch");
        Require(result, counts.clientMoves == 1 &&
                        counts.hoverEnters == 1 &&
                        counts.hoverLeaves == 0,
                "reentry recovery did not publish one hover");
        FinishScenario(result, started);
        return result;
    }

    PointerScenarioResult RunReleaseOutside()
    {
        PointerScenarioResult result;
        result.name = "release-outside-under-native-capture";
        const ULONGLONG started = ::GetTickCount64();
        BeginScenario(result);

        m_target->CaptureOnButtonDown();
        wxWinUIPointerSample press = MakeMove();
        press.kind = wxWinUIInputKind::Press;
        press.button = wxWinUIInputButton::Left;
        press.buttonMask = MK_LBUTTON;
        const wxWinUIRootPointerOutcome down =
            Route(result, press);
        Require(result,
                down.status == wxWinUIRootPointerStatus::Completed &&
                down.pointerDispatch.gestureSerial != 0 &&
                down.pointerDispatch.button ==
                    wxWinUIInputButton::Left,
                "capturing DOWN was not committed");
        Require(result, ::GetCapture() == GetHwndOf(m_target),
                "target did not acquire native capture");

        RECT frameRect = {};
        Require(result,
                ::GetWindowRect(GetHwndOf(this), &frameRect) != 0,
                "cannot read frame bounds for outside release");
        wxWinUIPointerSample release = press;
        release.kind = wxWinUIInputKind::Release;
        release.buttonMask = 0;
        release.timestamp = ++m_timestamp;
        release.screenX = frameRect.right + 40;
        release.screenY = frameRect.bottom + 40;
        const wxWinUIRootPointerOutcome up =
            Route(result, release);
        Require(result,
                up.status == wxWinUIRootPointerStatus::Completed &&
                up.pointerDispatch.balancesPress &&
                up.pointerDispatch.gestureSerial ==
                    down.pointerDispatch.gestureSerial,
                "outside UP did not balance the captured DOWN");

        const PointerScenarioMessageCounts counts = m_target->GetCounts();
        Require(result, result.samples == 2 &&
                        result.completed == 2,
                "release-outside outcome counts mismatch");
        Require(result, counts.leftDowns == 1 &&
                        counts.leftUps == 1 &&
                        counts.cancelModes == 0,
                "release-outside message balance mismatch");
        Require(result, ::GetCapture() == nullptr,
                "native capture remained after balanced outside UP");
        FinishScenario(result, started);
        return result;
    }

    PointerScenarioResult RunCaptureLost()
    {
        PointerScenarioResult result;
        result.name = "native-capture-lost-late-release";
        const ULONGLONG started = ::GetTickCount64();
        BeginScenario(result);

        m_target->CaptureOnButtonDown();
        wxWinUIPointerSample press = MakeMove();
        press.kind = wxWinUIInputKind::Press;
        press.button = wxWinUIInputButton::Left;
        press.buttonMask = MK_LBUTTON;
        const wxWinUIRootPointerOutcome down =
            Route(result, press);
        Require(result,
                down.status == wxWinUIRootPointerStatus::Completed &&
                down.pointerDispatch.gestureSerial != 0,
                "capturing DOWN was not committed before capture loss");
        Require(result, ::GetCapture() == GetHwndOf(m_target),
                "target did not acquire capture before capture loss");

        Require(result, ::ReleaseCapture() != 0,
                "ReleaseCapture failed in capture-loss scenario");
        wxWinUIPointerSample lost = press;
        lost.kind = wxWinUIInputKind::CaptureLost;
        lost.button = wxWinUIInputButton::None;
        lost.buttonMask = 0;
        lost.timestamp = ++m_timestamp;
        Interrupt(result, true, lost);

        wxWinUIPointerSample lateRelease = lost;
        lateRelease.kind = wxWinUIInputKind::Release;
        lateRelease.button = wxWinUIInputButton::Left;
        lateRelease.timestamp = ++m_timestamp;
        const wxWinUIRootPointerOutcome late =
            Route(result, lateRelease);
        Require(result,
                late.status == wxWinUIRootPointerStatus::Completed &&
                !late.pointerDispatch.balancesPress,
                "late UP inherited the canceled gesture serial");

        const PointerScenarioMessageCounts counts = m_target->GetCounts();
        Require(result, result.samples == 3 &&
                        result.completed == 2 &&
                        result.interruptions == 1,
                "capture-loss outcome counts mismatch");
        Require(result, counts.leftDowns == 1 &&
                        counts.leftUps == 0 &&
                        counts.cancelModes == 1,
                "capture-loss cancellation was not exact-once");
        Require(result, ::GetCapture() == nullptr,
                "native capture remained after capture loss");
        FinishScenario(result, started);
        return result;
    }

    PointerScenarioTarget *m_target = nullptr;
    wxWinUITopLevelHost *m_host = nullptr;
    bool m_setupOk = false;
    bool m_wasActivated = false;
    ULONGLONG m_timestamp = ::GetTickCount64();
    ULONGLONG m_deadline = 0;
};

std::string SerializePointerScenarioResults(
    int exitCode,
    ULONGLONG started,
    ULONGLONG elapsed,
    bool activated,
    const std::vector<PointerScenarioResult>& results)
{
    std::ostringstream json;
    json << "{\n"
         << "  \"schema_version\": 1,\n"
         << "  \"suite\": \"winui-root-pointer-scenarios\",\n"
         << "  \"pid\": " << ::GetCurrentProcessId() << ",\n"
         << "  \"started_tick_ms\": " << started << ",\n"
         << "  \"elapsed_ms\": " << elapsed << ",\n"
         << "  \"exit_code\": " << exitCode << ",\n"
         << "  \"status\": \""
         << (exitCode == 0 ? "pass" :
             exitCode == kPointerScenarioExitTimeout ? "timeout" : "fail")
         << "\",\n"
         << "  \"window_activated\": "
         << (activated ? "true" : "false") << ",\n"
         << "  \"scenario_count\": " << results.size() << ",\n"
         << "  \"scenarios\": [\n";

    for ( std::size_t i = 0; i < results.size(); ++i )
    {
        const PointerScenarioResult& r = results[i];
        const PointerScenarioMessageCounts& m = r.messages;
        json << "    {\n"
             << "      \"name\": \"" << JsonEscape(r.name) << "\",\n"
             << "      \"status\": \""
             << (r.passed ? "pass" : "fail") << "\",\n"
             << "      \"failure\": \"" << JsonEscape(r.failure)
             << "\",\n"
             << "      \"elapsed_ms\": " << r.elapsedMs << ",\n"
             << "      \"samples\": " << r.samples << ",\n"
             << "      \"prepared_actions\": " << r.preparedActions
             << ",\n"
             << "      \"outcomes\": {"
             << "\"completed\":" << r.completed
             << ",\"policy_filtered\":" << r.policyFiltered
             << ",\"slot_release_consumed\":" << r.slotReleaseConsumed
             << ",\"failed_closed\":" << r.failedClosed
             << ",\"superseded\":" << r.superseded
             << ",\"routed\":" << r.routed
             << ",\"xaml\":" << r.xaml
             << ",\"outside\":" << r.outside
             << ",\"indeterminate\":" << r.indeterminate
             << ",\"storm_suppressed\":" << r.stormSuppressed
             << ",\"break_cycles\":" << r.breakCycles
             << ",\"cancel_source\":" << r.cancelSource
             << ",\"boundary_calls\":" << r.boundaryCalls
             << ",\"boundary_completed\":" << r.boundaryCompleted
             << ",\"interruptions\":" << r.interruptions << "},\n"
             << "      \"messages\": {"
             << "\"hit_tests\":" << m.hitTests
             << ",\"set_cursors\":" << m.setCursors
             << ",\"client_moves\":" << m.clientMoves
             << ",\"nonclient_moves\":" << m.nonClientMoves
             << ",\"client_leaves\":" << m.clientLeaves
             << ",\"nonclient_leaves\":" << m.nonClientLeaves
             << ",\"hover_enters\":" << m.hoverEnters
             << ",\"hover_leaves\":" << m.hoverLeaves
             << ",\"left_downs\":" << m.leftDowns
             << ",\"left_ups\":" << m.leftUps
             << ",\"cancel_modes\":" << m.cancelModes << "}\n"
             << "    }" << (i + 1 == results.size() ? "\n" : ",\n");
    }
    json << "  ]\n"
         << "}\n";
    return json.str();
}

bool WritePointerScenarioResult(const wxString& path,
                                const std::string& contents,
                                wxString *error)
{
    if ( path.empty() )
    {
        if ( error )
            *error = "empty result path";
        return false;
    }

    const wxFileName output(path);
    if ( output.FileExists() )
    {
        if ( error )
            *error = "result path already exists";
        return false;
    }
    if ( !output.DirExists() )
    {
        if ( error )
            *error = "result directory does not exist";
        return false;
    }

    const wxString temporary =
        path + wxString::Format(".tmp-%lu", ::GetCurrentProcessId());
    FILE * const file = _wfopen(temporary.wc_str(), L"wb");
    if ( !file )
    {
        if ( error )
            *error = "cannot create the temporary result";
        return false;
    }

    const bool wrote =
        fwrite(contents.data(), 1, contents.size(), file) ==
            contents.size() &&
        fflush(file) == 0 &&
        !ferror(file);
    const int closeResult = fclose(file);
    if ( !wrote || closeResult != 0 )
    {
        (void)::DeleteFileW(temporary.wc_str());
        if ( error )
            *error = "cannot flush the temporary result";
        return false;
    }

    if ( !::MoveFileExW(
             temporary.wc_str(), path.wc_str(), MOVEFILE_WRITE_THROUGH) )
    {
        const DWORD win32Error = ::GetLastError();
        (void)::DeleteFileW(temporary.wc_str());
        if ( error )
        {
            *error = wxString::Format(
                "cannot publish the result (Win32 error %lu)",
                win32Error);
        }
        return false;
    }
    return true;
}

// ----------------------------------------------------------------------------
// Deterministic phase-007c composition scenarios.
//
// This is a separate, explicitly selected mode. A normal winuispike run still
// creates no render subscription and records no timings. The latency probe
// deliberately observes the production slot state instead of duplicating
// SyncSlot(): GetRectInTLW() is committed only after all XAML setters succeed.
// The first CompositionTarget.Rendering callback after that observation is the
// presentation-side bound. Exact entry/exit timestamps inside SyncSlot would
// require a small core diagnostic seam and are not fabricated here.
// ----------------------------------------------------------------------------

constexpr int kCompositionScenarioExitCli = 40;
constexpr int kCompositionScenarioExitSetup = 41;
constexpr int kCompositionScenarioExitHost = 42;
constexpr int kCompositionScenarioExitFailed = 43;
constexpr int kCompositionScenarioExitTimeout = 44;
constexpr int kCompositionScenarioExitWrite = 45;
constexpr int kCompositionScenarioExitException = 46;
constexpr int kCompositionScenarioExitUnqualified = 2;

constexpr int kCompositionFrameWidth = 720;
constexpr int kCompositionFrameHeight = 520;
constexpr int kCompositionOffscreenMargin = 64;
constexpr unsigned kLatencyWarmupRenderCallbacks = 4;
constexpr unsigned kLatencyWarmupScrollBatches = 1;

long long ScenarioQpcNow()
{
    LARGE_INTEGER value = {};
    ::QueryPerformanceCounter(&value);
    return value.QuadPart;
}

long long QpcFrequency()
{
    static const long long frequency = []()
    {
        LARGE_INTEGER value = {};
        return ::QueryPerformanceFrequency(&value)
                 ? value.QuadPart
                 : 0LL;
    }();
    return frequency;
}

long long QpcDeltaMicroseconds(long long started, long long ended)
{
    const long long frequency = QpcFrequency();
    if ( frequency <= 0 || ended < started )
        return -1;

    const long long seconds = (ended - started) / frequency;
    const long long remainder = (ended - started) % frequency;
    return seconds * 1000000LL +
           (remainder * 1000000LL) / frequency;
}

bool GetCompositionOffscreenPosition(wxPoint *position)
{
    if ( !position )
        return false;

    const long long virtualLeft = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const long long virtualTop = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const long long virtualWidth = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const long long virtualHeight = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if ( virtualWidth <= 0 || virtualHeight <= 0 )
        return false;

    const long long left =
        virtualLeft - kCompositionFrameWidth - kCompositionOffscreenMargin;
    if ( left >= (std::numeric_limits<int>::min)() &&
         left <= (std::numeric_limits<int>::max)() )
    {
        *position = wxPoint(static_cast<int>(left),
                            static_cast<int>(virtualTop));
        return true;
    }

    const long long right =
        virtualLeft + virtualWidth + kCompositionOffscreenMargin;
    if ( right >= (std::numeric_limits<int>::min)() &&
         right <= (std::numeric_limits<int>::max)() )
    {
        *position = wxPoint(static_cast<int>(right),
                            static_cast<int>(virtualTop));
        return true;
    }

    return false;
}

struct CompositionCheck
{
    std::string name;
    bool passed = true;
    std::string signature;
    std::string detail;
    unsigned diagnosticCount = 0;
};

struct ScrollLatencySample
{
    unsigned index = 0;
    unsigned backlog = 0;
    unsigned scheduledFlushes = 0;
    long long mutationToSlotUs = -1;
    long long mutationToRenderUs = -1;
    long long slotToRenderUs = -1;
    bool stale = true;
    bool passed = false;
};

enum class LatencyOutcome
{
    Pass,
    Unqualified,
    Fail
};

// Keep the three-state policy independent from the measurement machinery so
// every caller uses the same decision and the important combinations remain
// compile-time checked. A cadence mismatch can only make a technically sound
// run unqualified; it can never hide a structural, slot or safety failure.
constexpr LatencyOutcome ClassifyLatency(
    bool technicalFailure,
    bool structuralGatesPassed,
    bool slotBudgetPassed,
    bool targetRenderBudgetPassed,
    bool cadenceSafetyPassed,
    bool cadenceQualified)
{
    if ( technicalFailure ||
         !structuralGatesPassed ||
         !slotBudgetPassed ||
         !cadenceSafetyPassed )
    {
        return LatencyOutcome::Fail;
    }

    if ( !cadenceQualified )
        return LatencyOutcome::Unqualified;

    return targetRenderBudgetPassed
             ? LatencyOutcome::Pass
             : LatencyOutcome::Fail;
}

static_assert(
    ClassifyLatency(false, true, true, true, true, true) ==
        LatencyOutcome::Pass);
static_assert(
    ClassifyLatency(false, true, true, false, true, false) ==
        LatencyOutcome::Unqualified);
static_assert(
    ClassifyLatency(false, true, true, true, true, false) ==
        LatencyOutcome::Unqualified);
static_assert(
    ClassifyLatency(false, true, true, false, true, true) ==
        LatencyOutcome::Fail);
static_assert(
    ClassifyLatency(false, true, true, true, false, false) ==
        LatencyOutcome::Fail);
static_assert(
    ClassifyLatency(false, true, false, true, true, false) ==
        LatencyOutcome::Fail);
static_assert(
    ClassifyLatency(false, false, true, true, true, false) ==
        LatencyOutcome::Fail);
static_assert(
    ClassifyLatency(true, true, true, true, true, true) ==
        LatencyOutcome::Fail);

const char *LatencyOutcomeName(LatencyOutcome outcome)
{
    switch ( outcome )
    {
        case LatencyOutcome::Pass:
            return "pass";
        case LatencyOutcome::Unqualified:
            return "unqualified";
        case LatencyOutcome::Fail:
            return "fail";
    }

    return "fail";
}

struct LatencyEvaluation
{
    LatencyOutcome outcome = LatencyOutcome::Fail;
    int exitCode = kCompositionScenarioExitFailed;
    long long targetPeriodUs = -1;
    long long observedMedianRenderUs = -1;
    long long observedRenderMilliHz = 0;
    long long cadenceToleranceMilliHz = 0;
    long long slotP95Us = -1;
    long long renderP95Us = -1;
    long long renderSafetyLimitUs = -1;
    bool cadenceQualified = false;
    bool allSamplesPassed = false;
    bool structuralGatesPassed = false;
    bool slotBudgetPassed = false;
    bool targetRenderBudgetPassed = false;
    bool cadenceSafetyPassed = false;
    bool qualificationPassed = false;
    bool windowOffscreen = false;
};

class CompositionScenarioWindow : public wxWindow
{
public:
    CompositionScenarioWindow(wxWindow *parent,
                              const wxPoint& position,
                              const wxSize& size,
                              const wxColour& colour)
        : wxWindow(parent, wxID_ANY, position, size),
          m_colour(colour)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT,
             [this](wxPaintEvent&)
             {
                 wxAutoBufferedPaintDC dc(this);
                 dc.SetBackground(wxBrush(m_colour));
                 dc.Clear();
             });
    }

private:
    wxColour m_colour;
};

class CompositionScenarioFrame : public wxFrame
{
public:
    using LatencyCompletion =
        std::function<void (int, const std::string&)>;

    CompositionScenarioFrame()
        : wxFrame(nullptr, wxID_ANY, "wxWinUI composition scenarios",
                  wxDefaultPosition,
                  wxSize(kCompositionFrameWidth, kCompositionFrameHeight),
                  wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW | wxBORDER_NONE)
    {
        Bind(wxEVT_ACTIVATE,
             [this](wxActivateEvent& event)
             {
                 if ( event.GetActive() )
                     ObserveActivation();
                 event.Skip();
             });

        SetClientSize(kCompositionFrameWidth, kCompositionFrameHeight);

        // Establish the non-activating, off-screen contract before creating
        // any child. wxNotebook selects its first page during AddPage() and
        // that path may call SetFocus(); applying WS_EX_NOACTIVATE afterwards
        // is observably too late and can disturb the interactive desktop.
        const HWND hwnd = GetHwndOf(this);
        if ( !hwnd )
            return;

        ::SetLastError(ERROR_SUCCESS);
        const LONG_PTR oldStyle = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if ( !oldStyle && ::GetLastError() != ERROR_SUCCESS )
            return;
        ::SetLastError(ERROR_SUCCESS);
        (void)::SetWindowLongPtr(
            hwnd, GWL_EXSTYLE,
            oldStyle | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
        if ( ::GetLastError() != ERROR_SUCCESS )
            return;

        wxPoint offscreen;
        if ( !GetCompositionOffscreenPosition(&offscreen) )
            return;
        if ( !::SetWindowPos(
                 hwnd, HWND_BOTTOM,
                 offscreen.x, offscreen.y,
                 kCompositionFrameWidth, kCompositionFrameHeight,
                 SWP_FRAMECHANGED | SWP_NOACTIVATE |
                 SWP_NOOWNERZORDER) )
        {
            return;
        }

        // Three overlapping native siblings, two of which own XAML slots.
        // Their real HWND order is the source of truth for the z-order gate.
        m_zA = new CompositionScenarioWindow(
            this, wxPoint(20, 20), wxSize(170, 105),
            wxColour(76, 122, 210));
        m_zNative = new CompositionScenarioWindow(
            this, wxPoint(55, 42), wxSize(170, 105),
            wxColour(210, 145, 66));
        m_zB = new CompositionScenarioWindow(
            this, wxPoint(90, 64), wxSize(170, 105),
            wxColour(92, 174, 111));

        // Ancestor clipping gate.
        m_clipParent = new wxPanel(
            this, wxID_ANY, wxPoint(20, 180), wxSize(120, 90));
        m_clipped = new CompositionScenarioWindow(
            m_clipParent, wxPoint(50, 30), wxSize(140, 100),
            wxColour(155, 91, 190));

        // The same scrolled child drives both the deterministic scroll
        // scenario and the opt-in QPC latency campaign.
        m_scroll = new wxScrolledWindow(
            this, wxID_ANY, wxPoint(150, 180), wxSize(280, 130),
            wxHSCROLL | wxVSCROLL | wxBORDER_SIMPLE);
        m_scroll->SetScrollRate(1, 1);
        m_scroll->SetVirtualSize(520, 260);
        m_scrolledSlot = new CompositionScenarioWindow(
            m_scroll, wxPoint(180, 42), wxSize(180, 64),
            wxColour(52, 157, 164));

#if wxUSE_NOTEBOOK
        m_notebook = new wxNotebook(
            this, wxID_ANY, wxPoint(20, 330), wxSize(300, 150));
        m_notebook->AddPage(new wxPanel(m_notebook), "alpha", true);
        m_notebook->AddPage(new wxPanel(m_notebook), "beta", false);
#endif

#if wxUSE_SPLITTER
        m_splitter = new wxSplitterWindow(
            this, wxID_ANY, wxPoint(450, 180), wxSize(240, 220),
            wxSP_LIVE_UPDATE | wxSP_3D);
        m_splitterFirst = new wxPanel(m_splitter);
        m_splitterSecond = new wxPanel(m_splitter);
        m_splitter->SetMinimumPaneSize(24);
        m_splitter->SetSashGravity(0.5);
        m_splitter->SplitVertically(
            m_splitterFirst, m_splitterSecond, 104);
#endif

        ShowWithoutActivating();
        if ( !::SetWindowPos(
                 hwnd, HWND_BOTTOM,
                 offscreen.x, offscreen.y,
                 kCompositionFrameWidth, kCompositionFrameHeight,
                 SWP_FRAMECHANGED | SWP_NOACTIVATE |
                 SWP_NOOWNERZORDER | SWP_SHOWWINDOW) )
        {
            return;
        }

        m_host = wxWinUITopLevelHost::ForWindow(this, true);
        if ( !m_host )
            return;

        m_zASlot = RegisterColourSlot(m_zA, L"XAML A", 255, 58, 91, 192);
        m_zBSlot = RegisterColourSlot(m_zB, L"XAML B", 255, 52, 143, 74);
        m_clippedSlot =
            RegisterColourSlot(m_clipped, L"CLIPPED", 255, 119, 62, 151);
        m_scrolledSlotState =
            RegisterColourSlot(m_scrolledSlot, L"SCROLL", 255, 31, 128, 138);

        m_host->FlushSync();
        m_setupOk =
            m_zASlot && m_zBSlot && m_clippedSlot &&
            m_scrolledSlotState &&
            IsPointerScenarioWindowOutsideVirtualDesktop(hwnd) &&
            (::GetWindowLongPtr(hwnd, GWL_EXSTYLE) &
                WS_EX_NOACTIVATE) != 0 &&
            QpcFrequency() > 0;
        ObserveActivation();

        m_staleTimer.SetOwner(this);
        Bind(wxEVT_TIMER, &CompositionScenarioFrame::OnLatencyStaleTimer,
             this, m_staleTimer.GetId());
    }

    ~CompositionScenarioFrame() override
    {
        m_staleTimer.Stop();
        RevokeRendering();
    }

    bool IsSetupOk() const { return m_setupOk; }
    bool HasHost() const { return m_host != nullptr; }
    bool WasActivated() const { return m_wasActivated; }
    bool IsZOrderLayoutReady() const
    {
        if ( !m_setupOk )
            return true;
#if wxUSE_NOTEBOOK
        wxWinUISlot * const slot = m_host->FindSlot(m_notebook);
        if ( !slot )
            return false;
        const auto clip = slot->GetContainer().Clip();
        if ( !clip ||
             !clip.try_as<MUXM::RectangleGeometry>() )
        {
            return false;
        }
#endif
        return !m_zASlot->GetRectInTLW().IsEmpty() &&
               !m_scrolledSlotState->GetRectInTLW().IsEmpty();
    }

    std::pair<int, std::string> RunZOrderScenarios(
        ULONGLONG startedTick)
    {
        std::vector<CompositionCheck> checks;
        if ( !m_setupOk )
        {
            return {
                m_host ? kCompositionScenarioExitSetup
                       : kCompositionScenarioExitHost,
                SerializeZOrderResults(
                    m_host ? kCompositionScenarioExitSetup
                           : kCompositionScenarioExitHost,
                    startedTick, checks)
            };
        }

        ObserveActivation();
        m_host->FlushSync();
        checks.push_back(CheckXamlOrder("initial-xaml-sibling-order"));

        m_zA->Raise();
        m_host->FlushSync();
        CompositionCheck raised =
            CheckXamlOrder("raise-follows-native-sibling-order");
        Require(raised, IsAbove(m_zA, m_zB),
                "Raise() did not place A above B in USER32");
        checks.push_back(std::move(raised));

        m_zA->Lower();
        m_host->FlushSync();
        CompositionCheck lowered =
            CheckXamlOrder("lower-follows-native-sibling-order");
        Require(lowered, IsAbove(m_zB, m_zA),
                "Lower() did not place A below B in USER32");
        checks.push_back(std::move(lowered));

        checks.push_back(CheckImpossibleInterleave());
        checks.push_back(CheckAncestorClip());
        checks.push_back(CheckHiddenSlot());
        checks.push_back(CheckScrollClip());
        checks.push_back(CheckNotebookBand());
        checks.push_back(CheckSplitterGeometry());

        ObserveActivation();
        int exitCode = m_wasActivated
                         ? kCompositionScenarioExitFailed
                         : 0;
        for ( const CompositionCheck& check : checks )
        {
            if ( !check.passed )
            {
                exitCode = kCompositionScenarioExitFailed;
                break;
            }
        }

        return {
            exitCode,
            SerializeZOrderResults(exitCode, startedTick, checks)
        };
    }

    void StartScrollLatency(unsigned targetHz,
                            unsigned sampleCount,
                            ULONGLONG startedTick,
                            LatencyCompletion completion)
    {
        m_targetHz = targetHz;
        m_requestedSamples = sampleCount;
        m_latencyStartedTick = startedTick;
        m_latencyCompletion = std::move(completion);
        m_latencySamples.clear();
        m_renderIntervalsUs.clear();
        m_lastRenderQpc = 0;
        m_latencyFinished = false;
        m_maxBacklog = 0;
        m_maxScheduledFlushes = 0;
        m_warmupRenderCallbacksObserved = 0;
        m_warmupMutationScheduled = false;
        m_warmupMutation = false;
        m_warmupCompleted = false;
        m_mutationQpc = 0;
        m_slotObservedQpc = 0;
        m_waitingForRender = false;

        if ( !m_setupOk || !m_host || !m_scrolledSlotState )
        {
            FinishLatency(m_host ? kCompositionScenarioExitSetup
                                 : kCompositionScenarioExitHost);
            return;
        }

        try
        {
            m_renderToken = MUXM::CompositionTarget::Rendering(
                [this](winrt::Windows::Foundation::IInspectable const&,
                       winrt::Windows::Foundation::IInspectable const&)
                {
                    OnRendering();
                });
        }
        catch ( const winrt::hresult_error& )
        {
            FinishLatency(kCompositionScenarioExitSetup);
            return;
        }

        m_staleTimer.Start(10);
    }

private:
    static void Require(CompositionCheck& check,
                        bool condition,
                        const std::string& detail)
    {
        if ( !condition && check.passed )
        {
            check.passed = false;
            check.detail = detail;
        }
    }

    wxWinUISlot *RegisterColourSlot(wxWindow *window,
                                    const wchar_t *label,
                                    BYTE a, BYTE r, BYTE g, BYTE b)
    {
        MUXC::Border border;
        border.Background(SpikeBrush(a, r, g, b));
        border.IsHitTestVisible(false);
        MUXC::TextBlock text;
        text.Text(label);
        text.HorizontalAlignment(MUX::HorizontalAlignment::Center);
        text.VerticalAlignment(MUX::VerticalAlignment::Center);
        border.Child(text);
        return m_host->RegisterSlot(window, border);
    }

    void ObserveActivation()
    {
        const HWND frame = GetHwndOf(this);
        const HWND focus = ::GetFocus();
        if ( frame &&
             (::GetForegroundWindow() == frame ||
              ::GetActiveWindow() == frame ||
              focus == frame ||
              (focus && ::IsChild(frame, focus))) )
        {
            m_wasActivated = true;
        }
    }

    static bool IsAbove(const wxWindow *first, const wxWindow *second)
    {
        if ( !first || !second || first->GetParent() != second->GetParent() )
            return false;

        const HWND firstHwnd = GetHwndOf(first);
        const HWND secondHwnd = GetHwndOf(second);
        const HWND parentHwnd = GetHwndOf(first->GetParent());
        if ( !firstHwnd || !secondHwnd || !parentHwnd )
            return false;

        for ( HWND child = ::GetTopWindow(parentHwnd);
              child;
              child = ::GetWindow(child, GW_HWNDNEXT) )
        {
            if ( child == firstHwnd )
                return true;
            if ( child == secondHwnd )
                return false;
        }
        return false;
    }

    std::string NativeOrderSignature() const
    {
        const HWND parent = GetHwndOf(this);
        std::string signature;
        for ( HWND child = parent ? ::GetTopWindow(parent) : nullptr;
              child;
              child = ::GetWindow(child, GW_HWNDNEXT) )
        {
            const char *label = nullptr;
            if ( child == GetHwndOf(m_zA) )
                label = "A";
            else if ( child == GetHwndOf(m_zNative) )
                label = "N";
            else if ( child == GetHwndOf(m_zB) )
                label = "B";

            if ( label )
            {
                if ( !signature.empty() )
                    signature += ">";
                signature += label;
            }
        }
        return signature;
    }

    CompositionCheck CheckXamlOrder(const char *name) const
    {
        CompositionCheck check;
        check.name = name;
        if ( !m_zASlot || !m_zBSlot )
        {
            Require(check, false, "missing z-order slot");
            return check;
        }

        const int zA = MUXC::Canvas::GetZIndex(m_zASlot->GetContainer());
        const int zB = MUXC::Canvas::GetZIndex(m_zBSlot->GetContainer());
        const bool aAbove = IsAbove(m_zA, m_zB);
        const bool bAbove = IsAbove(m_zB, m_zA);
        check.signature =
            NativeOrderSignature() + "|xaml:" +
            std::to_string(zA) + "," + std::to_string(zB);
        Require(check, aAbove != bAbove,
                "USER32 sibling order is not total");
        Require(check, aAbove ? zA > zB : zB > zA,
                "Canvas.ZIndex disagrees with USER32 sibling order");
        return check;
    }

    CompositionCheck CheckImpossibleInterleave()
    {
        CompositionCheck check;
        check.name = "impossible-native-xaml-interleave-signature";

        wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
        const UINT flags =
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
            SWP_NOOWNERZORDER;
        const HWND a = GetHwndOf(m_zA);
        const HWND native = GetHwndOf(m_zNative);
        const HWND b = GetHwndOf(m_zB);
        bool positioned =
            a && native && b &&
            ::SetWindowPos(a, HWND_TOP, 0, 0, 0, 0, flags) &&
            ::SetWindowPos(native, a, 0, 0, 0, 0, flags) &&
            ::SetWindowPos(b, native, 0, 0, 0, 0, flags);
        m_host->FlushSync();

        check.diagnosticCount =
            wxWinUITopLevelHost::GetImpossibleZOrderCount();
        check.signature = NativeOrderSignature();
        Require(check, positioned,
                "SetWindowPos could not establish the test topology");
        Require(check, check.signature == "A>N>B",
                "unexpected USER32 signature for interleave");
        Require(check, check.diagnosticCount == 1,
                "host did not report the impossible interleave exactly once");
        if ( check.passed )
        {
            check.detail =
                "host reported the impossible interleave exactly once";
        }
        return check;
    }

    wxRect WindowRectInHost(const wxWindow *window) const
    {
        RECT rect = {};
        const RECT client = m_host->GetClientScreenRect();
        if ( !window ||
             !::GetWindowRect(GetHwndOf(window), &rect) )
        {
            return wxRect();
        }

        return wxRect(rect.left - client.left,
                      rect.top - client.top,
                      rect.right - rect.left,
                      rect.bottom - rect.top);
    }

    wxRect ClientRectInHost(const wxWindow *window) const
    {
        RECT rect = {};
        const HWND hwnd = window ? GetHwndOf(window) : nullptr;
        if ( !hwnd || !::GetClientRect(hwnd, &rect) )
            return wxRect();

        POINT origin = { rect.left, rect.top };
        POINT extent = { rect.right, rect.bottom };
        if ( !::ClientToScreen(hwnd, &origin) ||
             !::ClientToScreen(hwnd, &extent) )
        {
            return wxRect();
        }

        const RECT hostClient = m_host->GetClientScreenRect();
        return wxRect(origin.x - hostClient.left,
                      origin.y - hostClient.top,
                      extent.x - origin.x,
                      extent.y - origin.y);
    }

    CompositionCheck CheckAncestorClip()
    {
        CompositionCheck check;
        check.name = "ancestor-clip";
        m_host->MarkDirty(m_clipped);
        m_host->FlushSync();

        const wxRect slotRect = WindowRectInHost(m_clipped);
        const wxRect parentRect = WindowRectInHost(m_clipParent);
        wxRect visible = slotRect;
        visible.Intersect(parentRect);
        const auto clip = m_clippedSlot->GetContainer().Clip();
        const auto geometry =
            clip ? clip.try_as<MUXM::RectangleGeometry>()
                 : MUXM::RectangleGeometry{ nullptr };
        const double scale = m_host->GetScale();
        check.signature =
            std::to_string(slotRect.x) + "," +
            std::to_string(slotRect.y) + "," +
            std::to_string(slotRect.width) + "," +
            std::to_string(slotRect.height) + "|visible:" +
            std::to_string(visible.x) + "," +
            std::to_string(visible.y) + "," +
            std::to_string(visible.width) + "," +
            std::to_string(visible.height);
        Require(check, geometry != nullptr,
                "partially clipped slot has no RectangleGeometry");
        if ( geometry )
        {
            const auto actual = geometry.Rect();
            const auto close = [](double left, double right)
            {
                return std::abs(left - right) <= 0.6;
            };
            Require(check,
                    close(actual.X, (visible.x - slotRect.x) / scale) &&
                    close(actual.Y, (visible.y - slotRect.y) / scale) &&
                    close(actual.Width, visible.width / scale) &&
                    close(actual.Height, visible.height / scale),
                    "ancestor clip geometry differs from the HWND viewport");
        }
        return check;
    }

    CompositionCheck CheckHiddenSlot()
    {
        CompositionCheck check;
        check.name = "hidden-slot-collapses";
        m_clipped->Show(false);
        m_host->FlushSync();
        const auto hidden =
            m_clippedSlot->GetContainer().Visibility();
        m_clipped->Show(true);
        m_host->FlushSync();
        const auto restored =
            m_clippedSlot->GetContainer().Visibility();
        check.signature =
            std::to_string(static_cast<int>(hidden)) + ">" +
            std::to_string(static_cast<int>(restored));
        Require(check, hidden == MUX::Visibility::Collapsed,
                "hidden HWND left its slot visible");
        Require(check, restored == MUX::Visibility::Visible,
                "shown HWND did not restore its slot");
        return check;
    }

    CompositionCheck CheckScrollClip()
    {
        CompositionCheck check;
        check.name = "scroll-viewport-clip";
        m_scroll->Scroll(0, 24);
        m_host->FlushSync();

        const wxRect before = m_scrolledSlotState->GetRectInTLW();
        const auto clipObject =
            m_scrolledSlotState->GetContainer().Clip();
        const auto clip =
            clipObject
              ? clipObject.try_as<MUXM::RectangleGeometry>()
              : MUXM::RectangleGeometry{ nullptr };
        const wxRect slotRect = WindowRectInHost(m_scrolledSlot);
        wxRect visible = slotRect;
        visible.Intersect(ClientRectInHost(m_scroll));
        const double scale = m_host->GetScale();
        Require(check, clip != nullptr,
                "scrolled child extending past its viewport has no clip");
        if ( clip )
        {
            const auto actual = clip.Rect();
            const auto close = [](double left, double right)
            {
                return std::abs(left - right) <= 0.6;
            };
            Require(check,
                    close(actual.X, (visible.x - slotRect.x) / scale) &&
                    close(actual.Y, (visible.y - slotRect.y) / scale) &&
                    close(actual.Width, visible.width / scale) &&
                    close(actual.Height, visible.height / scale),
                    "scroll clip geometry differs from the client viewport");
        }
        m_scroll->Scroll(0, 0);
        m_host->FlushSync();
        const wxRect after = m_scrolledSlotState->GetRectInTLW();
        check.signature =
            std::to_string(before.y) + ">" + std::to_string(after.y) +
            "|visible:" + std::to_string(visible.x) + "," +
            std::to_string(visible.y) + "," +
            std::to_string(visible.width) + "," +
            std::to_string(visible.height);
        Require(check, before.y != after.y,
                "scroll mutation did not move the published slot");
        return check;
    }

    CompositionCheck CheckNotebookBand()
    {
        CompositionCheck check;
        check.name = "tabview-band-clip";
#if wxUSE_NOTEBOOK
        wxWinUISlot * const slot = m_host->FindSlot(m_notebook);
        m_host->FlushSync();
        if ( !slot )
        {
            Require(check, false, "notebook did not register a slot");
            return check;
        }

        const auto clip = slot->GetContainer().Clip();
        const auto geometry =
            clip ? clip.try_as<MUXM::RectangleGeometry>()
                 : MUXM::RectangleGeometry{ nullptr };
        const double height = slot->GetContainer().Height();
        const wxWindow * const currentPage =
            m_notebook->GetCurrentPage();
        const double expectedHeight =
            currentPage
              ? static_cast<double>(
                    m_notebook->ToDIP(currentPage->GetPosition().y))
              : -1.0;
        check.signature =
            geometry
              ? std::to_string(geometry.Rect().Height) + "/" +
                    std::to_string(expectedHeight) + "/" +
                    std::to_string(height)
              : "none/" + std::to_string(height);
        Require(check, geometry != nullptr,
                "TabView slot has no tab-band clip");
        if ( geometry )
        {
            const double actualHeight = geometry.Rect().Height;
            Require(check,
                    expectedHeight > 0 &&
                    actualHeight > 0 &&
                    actualHeight < height &&
                    std::abs(actualHeight - expectedHeight) <= 0.6,
                    "TabView clip differs from the measured tab band");
        }
#else
        check.signature = "not-built";
        Require(check, false,
                "wxUSE_NOTEBOOK=0: TabView band was not exercised");
#endif
        return check;
    }

    CompositionCheck CheckSplitterGeometry()
    {
        CompositionCheck check;
        check.name = "splitter-horizontal-vertical-geometry";
#if wxUSE_SPLITTER
        const bool vertical =
            m_splitter->IsSplit() &&
            m_splitter->GetSplitMode() == wxSPLIT_VERTICAL &&
            m_splitter->GetSashPosition() >=
                m_splitter->GetMinimumPaneSize();

        const bool unsplit = m_splitter->Unsplit(m_splitterSecond);
        const bool horizontal =
            unsplit &&
            m_splitter->SplitHorizontally(
                m_splitterFirst, m_splitterSecond, 82) &&
            m_splitter->GetSplitMode() == wxSPLIT_HORIZONTAL &&
            m_splitter->GetSashPosition() >=
                m_splitter->GetMinimumPaneSize();

        m_host->ApplyTheme(MUX::ElementTheme::Light);
        m_splitter->Refresh(false);
        m_splitter->Update();
        m_host->ApplyTheme(MUX::ElementTheme::Dark);
        m_splitter->Refresh(false);
        m_splitter->Update();
        m_host->ApplyTheme(MUX::ElementTheme::Default);

        check.signature =
            std::string(vertical ? "V" : "!V") + ">" +
            (horizontal ? "H" : "!H") + "|sash:" +
            std::to_string(m_splitter->GetSashPosition());
        Require(check, vertical,
                "vertical splitter geometry is invalid");
        Require(check, horizontal,
                "horizontal splitter geometry is invalid");
#else
        check.signature = "not-built";
        Require(check, false,
                "wxUSE_SPLITTER=0: splitter geometry was not exercised");
#endif
        return check;
    }

    std::string SerializeZOrderResults(
        int exitCode,
        ULONGLONG startedTick,
        const std::vector<CompositionCheck>& checks) const
    {
        const ULONGLONG elapsed = ::GetTickCount64() - startedTick;
        std::ostringstream json;
        json.imbue(std::locale::classic());
        json << "{\n"
             << "  \"schema_version\": 1,\n"
             << "  \"suite\": \"winui-zorder-clip-splitter\",\n"
             << "  \"pid\": " << ::GetCurrentProcessId() << ",\n"
             << "  \"started_tick_ms\": " << startedTick << ",\n"
             << "  \"elapsed_ms\": " << elapsed << ",\n"
             << "  \"exit_code\": " << exitCode << ",\n"
             << "  \"status\": \""
             << (exitCode == 0 ? "pass" : "fail") << "\",\n"
             << "  \"window_activated\": "
             << (m_wasActivated ? "true" : "false") << ",\n"
             << "  \"window_offscreen\": "
             << (IsPointerScenarioWindowOutsideVirtualDesktop(
                    GetHwndOf(this)) ? "true" : "false") << ",\n"
             << "  \"scale\": " << std::fixed << std::setprecision(6)
             << (m_host ? m_host->GetScale() : 0.0) << ",\n"
             << "  \"checks\": [\n";
        for ( std::size_t i = 0; i < checks.size(); ++i )
        {
            const CompositionCheck& check = checks[i];
            json << "    {\"name\":\"" << JsonEscape(check.name)
                 << "\",\"status\":\""
                 << (check.passed ? "pass" : "fail")
                 << "\",\"signature\":\""
                 << JsonEscape(check.signature)
                 << "\",\"diagnostic_count\":"
                 << check.diagnosticCount
                 << ",\"detail\":\""
                 << JsonEscape(check.detail) << "\"}"
                 << (i + 1 == checks.size() ? "\n" : ",\n");
        }
        json << "  ],\n"
             << "  \"manual_gates\": [\n"
             << "    \"splitter Light/Dark/High Contrast visual surface\",\n"
             << "    \"splitter capture, cursor and double-click with "
                "physical input\",\n"
             << "    \"per-monitor DPI transition and minimized "
                "composition\"\n"
             << "  ]\n"
             << "}\n";
        return json.str();
    }

    void IssueLatencyMutation()
    {
        if ( m_latencyFinished || !m_warmupCompleted )
            return;

        if ( m_latencySamples.size() >= m_requestedSamples )
        {
            FinishLatency(0);
            return;
        }

        IssueLatencyMutationBatch();
    }

    void IssueLatencyWarmupMutation()
    {
        if ( m_latencyFinished || m_warmupCompleted )
            return;

        m_warmupMutationScheduled = false;
        m_warmupMutation = true;
        IssueLatencyMutationBatch();
    }

    void IssueLatencyMutationBatch()
    {
        m_latencyForward = !m_latencyForward;
        m_pendingBacklog = 4;
        m_maxBacklog = std::max(m_maxBacklog, m_pendingBacklog);
        const unsigned scheduledBefore =
            wxWinUITopLevelHost::GetFlushScheduleCount();
        m_mutationQpc = ScenarioQpcNow();
        if ( m_latencyForward )
        {
            for ( int offset : { 6, 12, 18, 24 } )
                m_scroll->Scroll(0, offset);
        }
        else
        {
            for ( int offset : { 18, 12, 6, 0 } )
                m_scroll->Scroll(0, offset);
        }
        const unsigned scheduledAfter =
            wxWinUITopLevelHost::GetFlushScheduleCount();
        m_pendingScheduledFlushes =
            scheduledAfter >= scheduledBefore
              ? scheduledAfter - scheduledBefore
              : 0;
        m_maxScheduledFlushes =
            std::max(m_maxScheduledFlushes,
                     m_pendingScheduledFlushes);

        m_expectedLatencyRect = WindowRectInHost(m_scrolledSlot);
        m_slotObservedQpc = 0;
        m_waitingForRender = false;
        m_pollAttempts = 0;
        CallAfter(&CompositionScenarioFrame::PollLatency);
    }

    void PollLatency()
    {
        if ( m_latencyFinished || !m_scrolledSlotState )
            return;

        if ( m_scrolledSlotState->GetRectInTLW() !=
                m_expectedLatencyRect )
        {
            if ( ++m_pollAttempts > 5000 )
            {
                FinishLatency(kCompositionScenarioExitTimeout);
                return;
            }
            CallAfter(&CompositionScenarioFrame::PollLatency);
            return;
        }

        m_slotObservedQpc = ScenarioQpcNow();
        m_waitingForRender = true;
    }

    void OnRendering()
    {
        const long long now = ScenarioQpcNow();

        if ( !m_warmupCompleted )
        {
            if ( m_warmupMutation && m_waitingForRender )
            {
                // This first complete scroll batch is deliberately excluded:
                // it realizes templates and primes the compositor before any
                // measured sample or cadence interval is retained.
                m_waitingForRender = false;
                m_mutationQpc = 0;
                m_warmupMutation = false;
                m_warmupCompleted = true;
                m_renderIntervalsUs.clear();
                m_lastRenderQpc = now;
                m_maxBacklog = 0;
                m_maxScheduledFlushes = 0;
                CallAfter(
                    &CompositionScenarioFrame::IssueLatencyMutation);
                return;
            }

            if ( !m_warmupMutationScheduled && !m_warmupMutation )
            {
                ++m_warmupRenderCallbacksObserved;
                if ( m_warmupRenderCallbacksObserved >=
                        kLatencyWarmupRenderCallbacks )
                {
                    m_warmupMutationScheduled = true;
                    CallAfter(
                        &CompositionScenarioFrame::
                            IssueLatencyWarmupMutation);
                }
            }
            return;
        }

        if ( m_lastRenderQpc )
        {
            const long long interval =
                QpcDeltaMicroseconds(m_lastRenderQpc, now);
            if ( interval >= 0 )
                m_renderIntervalsUs.push_back(interval);
        }
        m_lastRenderQpc = now;

        if ( m_latencyFinished || !m_waitingForRender )
            return;

        m_waitingForRender = false;
        ScrollLatencySample sample;
        sample.index =
            static_cast<unsigned>(m_latencySamples.size());
        sample.backlog = m_pendingBacklog;
        sample.scheduledFlushes = m_pendingScheduledFlushes;
        sample.mutationToSlotUs =
            QpcDeltaMicroseconds(m_mutationQpc, m_slotObservedQpc);
        sample.mutationToRenderUs =
            QpcDeltaMicroseconds(m_mutationQpc, now);
        sample.slotToRenderUs =
            QpcDeltaMicroseconds(m_slotObservedQpc, now);
        sample.stale =
            sample.mutationToRenderUs < 0 ||
            sample.mutationToRenderUs > 250000;
        sample.passed =
            sample.mutationToSlotUs >= 0 &&
            sample.mutationToRenderUs >= 0 &&
            sample.slotToRenderUs >= 0 &&
            !sample.stale &&
            sample.backlog <= 4 &&
            sample.scheduledFlushes <= 1;
        m_latencySamples.push_back(sample);
        m_mutationQpc = 0;
        CallAfter(&CompositionScenarioFrame::IssueLatencyMutation);
    }

    void OnLatencyStaleTimer(wxTimerEvent&)
    {
        if ( m_latencyFinished || !m_mutationQpc )
            return;

        const long long now = ScenarioQpcNow();
        const long long elapsed =
            QpcDeltaMicroseconds(m_mutationQpc, now);
        if ( elapsed < 0 || elapsed <= 250000 )
            return;

        if ( m_warmupMutation )
        {
            FinishLatency(kCompositionScenarioExitFailed);
            return;
        }

        ScrollLatencySample sample;
        sample.index =
            static_cast<unsigned>(m_latencySamples.size());
        sample.backlog = m_pendingBacklog;
        sample.scheduledFlushes = m_pendingScheduledFlushes;
        sample.mutationToSlotUs =
            m_slotObservedQpc
              ? QpcDeltaMicroseconds(m_mutationQpc,
                                     m_slotObservedQpc)
              : -1;
        sample.mutationToRenderUs = elapsed;
        sample.slotToRenderUs = -1;
        sample.stale = true;
        sample.passed = false;
        m_latencySamples.push_back(sample);
        FinishLatency(kCompositionScenarioExitFailed);
    }

    static long long Percentile95(std::vector<long long> values)
    {
        if ( values.empty() )
            return -1;
        std::sort(values.begin(), values.end());
        const std::size_t index =
            (95 * values.size() + 99) / 100 - 1;
        return values[std::min(index, values.size() - 1)];
    }

    static long long Median(std::vector<long long> values)
    {
        if ( values.empty() )
            return -1;
        std::sort(values.begin(), values.end());
        return values[values.size() / 2];
    }

    LatencyEvaluation EvaluateLatency(int preliminaryExitCode) const
    {
        std::vector<long long> mutationToSlot;
        std::vector<long long> mutationToRender;
        mutationToSlot.reserve(m_latencySamples.size());
        mutationToRender.reserve(m_latencySamples.size());
        bool allPassed = true;
        for ( const ScrollLatencySample& sample : m_latencySamples )
        {
            mutationToSlot.push_back(sample.mutationToSlotUs);
            mutationToRender.push_back(sample.mutationToRenderUs);
            allPassed = allPassed && sample.passed;
        }

        LatencyEvaluation evaluation;
        evaluation.allSamplesPassed = allPassed;
        evaluation.windowOffscreen =
            IsPointerScenarioWindowOutsideVirtualDesktop(
                GetHwndOf(this));
        evaluation.targetPeriodUs =
            (1000000LL + m_targetHz - 1) / m_targetHz;
        evaluation.observedMedianRenderUs =
            Median(m_renderIntervalsUs);
        evaluation.slotP95Us = Percentile95(mutationToSlot);
        evaluation.renderP95Us = Percentile95(mutationToRender);

        evaluation.observedRenderMilliHz =
            evaluation.observedMedianRenderUs > 0
              ? 1000000000LL /
                    evaluation.observedMedianRenderUs
              : 0;
        const long long targetMilliHz =
            static_cast<long long>(m_targetHz) * 1000;
        const long long computedCadenceTolerance =
            (targetMilliHz * 2 + 99) / 100;
        evaluation.cadenceToleranceMilliHz =
            computedCadenceTolerance > 1000
              ? computedCadenceTolerance
              : 1000;
        const long long cadenceDifference =
            evaluation.observedRenderMilliHz >= targetMilliHz
              ? evaluation.observedRenderMilliHz - targetMilliHz
              : targetMilliHz - evaluation.observedRenderMilliHz;
        evaluation.cadenceQualified =
            evaluation.observedRenderMilliHz > 0 &&
            cadenceDifference <= evaluation.cadenceToleranceMilliHz;
        evaluation.structuralGatesPassed =
            m_warmupCompleted &&
            m_latencySamples.size() == m_requestedSamples &&
            m_renderIntervalsUs.size() >= m_latencySamples.size() &&
            allPassed &&
            !m_wasActivated &&
            evaluation.windowOffscreen &&
            m_maxBacklog <= 4 &&
            m_maxScheduledFlushes <= 1 &&
            evaluation.observedMedianRenderUs > 0;
        evaluation.slotBudgetPassed =
            evaluation.slotP95Us >= 0 &&
            evaluation.slotP95Us <= evaluation.targetPeriodUs;
        evaluation.targetRenderBudgetPassed =
            evaluation.renderP95Us >= 0 &&
            evaluation.renderP95Us <=
                2 * evaluation.targetPeriodUs;
        evaluation.renderSafetyLimitUs =
            evaluation.observedMedianRenderUs > 0
              ? 2 * std::max(evaluation.targetPeriodUs,
                             evaluation.observedMedianRenderUs)
              : -1;
        evaluation.cadenceSafetyPassed =
            evaluation.renderP95Us >= 0 &&
            evaluation.renderSafetyLimitUs >= 0 &&
            evaluation.renderP95Us <=
                evaluation.renderSafetyLimitUs;
        evaluation.qualificationPassed =
            evaluation.structuralGatesPassed &&
            evaluation.slotBudgetPassed &&
            evaluation.targetRenderBudgetPassed &&
            evaluation.cadenceSafetyPassed &&
            evaluation.cadenceQualified;
        evaluation.outcome = ClassifyLatency(
            preliminaryExitCode != 0,
            evaluation.structuralGatesPassed,
            evaluation.slotBudgetPassed,
            evaluation.targetRenderBudgetPassed,
            evaluation.cadenceSafetyPassed,
            evaluation.cadenceQualified);
        switch ( evaluation.outcome )
        {
            case LatencyOutcome::Pass:
                evaluation.exitCode = 0;
                break;
            case LatencyOutcome::Unqualified:
                evaluation.exitCode =
                    kCompositionScenarioExitUnqualified;
                break;
            case LatencyOutcome::Fail:
                evaluation.exitCode =
                    preliminaryExitCode != 0
                      ? preliminaryExitCode
                      : kCompositionScenarioExitFailed;
                break;
        }

        return evaluation;
    }

    std::string SerializeLatencyResults(
        const LatencyEvaluation& evaluation) const
    {
        const ULONGLONG elapsed =
            ::GetTickCount64() - m_latencyStartedTick;

        std::ostringstream json;
        json.imbue(std::locale::classic());
        json << "{\n"
             << "  \"schema_version\": 2,\n"
             << "  \"suite\": \"winui-scroll-latency\",\n"
             << "  \"pid\": " << ::GetCurrentProcessId() << ",\n"
             << "  \"started_tick_ms\": " << m_latencyStartedTick
             << ",\n"
             << "  \"elapsed_ms\": " << elapsed << ",\n"
             << "  \"exit_code\": " << evaluation.exitCode << ",\n"
             << "  \"status\": \""
             << LatencyOutcomeName(evaluation.outcome) << "\",\n"
             << "  \"window_activated\": "
             << (m_wasActivated ? "true" : "false") << ",\n"
             << "  \"window_offscreen\": "
             << (evaluation.windowOffscreen ? "true" : "false")
             << ",\n"
             << "  \"target_hz\": " << m_targetHz << ",\n"
             << "  \"target_period_us\": "
             << evaluation.targetPeriodUs << ",\n"
             << "  \"observed_render_median_period_us\": "
             << evaluation.observedMedianRenderUs << ",\n"
             << "  \"observed_render_millihz\": "
             << evaluation.observedRenderMilliHz << ",\n"
             << "  \"cadence_tolerance_millihz\": "
             << evaluation.cadenceToleranceMilliHz << ",\n"
             << "  \"cadence_qualified\": "
             << (evaluation.cadenceQualified ? "true" : "false")
             << ",\n"
             << "  \"sample_count\": " << m_latencySamples.size()
             << ",\n"
             << "  \"requested_sample_count\": "
             << m_requestedSamples << ",\n"
             << "  \"measured_render_interval_count\": "
             << m_renderIntervalsUs.size() << ",\n"
             << "  \"max_backlog\": " << m_maxBacklog << ",\n"
             << "  \"max_scheduled_flushes_per_batch\": "
             << m_maxScheduledFlushes << ",\n"
             << "  \"warmup\": {"
             << "\"required_render_callbacks\":"
             << kLatencyWarmupRenderCallbacks
             << ",\"observed_render_callbacks\":"
             << m_warmupRenderCallbacksObserved
             << ",\"scroll_batches\":"
             << kLatencyWarmupScrollBatches
             << ",\"completed\":"
             << (m_warmupCompleted ? "true" : "false") << "},\n"
             << "  \"budgets\": {"
             << "\"mutation_to_slot_p95_us\":"
             << evaluation.targetPeriodUs
             << ",\"mutation_to_render_target_p95_us\":"
             << 2 * evaluation.targetPeriodUs
             << ",\"mutation_to_render_safety_p95_us\":"
             << evaluation.renderSafetyLimitUs
             << ",\"render_safety_rule\":"
                "\"2 * max(target period, observed median period)\""
             << ",\"stale_limit_us\":250000,\"backlog_limit\":4"
             << ",\"scheduled_flushes_per_batch_limit\":1},\n"
             << "  \"gates\": {"
             << "\"structural_gates_passed\":"
             << (evaluation.structuralGatesPassed ? "true" : "false")
             << ",\"slot_budget_passed\":"
             << (evaluation.slotBudgetPassed ? "true" : "false")
             << ",\"target_render_budget_passed\":"
             << (evaluation.targetRenderBudgetPassed
                   ? "true" : "false")
             << ",\"cadence_safety_passed\":"
             << (evaluation.cadenceSafetyPassed ? "true" : "false")
             << ",\"qualification_passed\":"
             << (evaluation.qualificationPassed ? "true" : "false")
             << "},\n"
             << "  \"summary\": {"
             << "\"mutation_to_slot_p95_us\":"
             << evaluation.slotP95Us
             << ",\"mutation_to_render_p95_us\":"
             << evaluation.renderP95Us
             << ",\"all_samples_passed\":"
             << (evaluation.allSamplesPassed ? "true" : "false")
             << "},\n"
             << "  \"samples\": [\n";
        for ( std::size_t i = 0; i < m_latencySamples.size(); ++i )
        {
            const ScrollLatencySample& sample = m_latencySamples[i];
            json << "    {\"index\":" << sample.index
                 << ",\"backlog\":" << sample.backlog
                 << ",\"scheduled_flushes\":"
                 << sample.scheduledFlushes
                 << ",\"mutation_to_slot_us\":"
                 << sample.mutationToSlotUs
                 << ",\"mutation_to_render_us\":"
                 << sample.mutationToRenderUs
                 << ",\"slot_to_render_us\":"
                 << sample.slotToRenderUs
                 << ",\"stale\":"
                 << (sample.stale ? "true" : "false")
                 << ",\"status\":\""
                 << (sample.passed ? "pass" : "fail") << "\"}"
                 << (i + 1 == m_latencySamples.size() ? "\n" : ",\n");
        }
        json << "  ],\n"
             << "  \"render_intervals_us\": [";
        for ( std::size_t i = 0; i < m_renderIntervalsUs.size(); ++i )
        {
            if ( i )
                json << ",";
            json << m_renderIntervalsUs[i];
        }
        json << "],\n"
             << "  \"measurement_contract\": "
                "\"after four Rendering callbacks and one unmeasured "
                "scroll batch, QPC mutation to committed slot observation "
                "to next CompositionTarget.Rendering\"\n"
             << "}\n";
        return json.str();
    }

    void RevokeRendering()
    {
        if ( !m_renderToken.value )
            return;
        try
        {
            MUXM::CompositionTarget::Rendering(m_renderToken);
        }
        catch ( const winrt::hresult_error& )
        {
        }
        m_renderToken = {};
    }

    void FinishLatency(int exitCode)
    {
        if ( m_latencyFinished )
            return;

        m_latencyFinished = true;
        m_staleTimer.Stop();
        ObserveActivation();

        const LatencyEvaluation evaluation =
            EvaluateLatency(exitCode);
        RevokeRendering();
        const std::string serialized =
            SerializeLatencyResults(evaluation);
        LatencyCompletion completion = std::move(m_latencyCompletion);
        if ( completion )
            completion(evaluation.exitCode, serialized);
    }

    wxWinUITopLevelHost *m_host = nullptr;
    CompositionScenarioWindow *m_zA = nullptr;
    CompositionScenarioWindow *m_zNative = nullptr;
    CompositionScenarioWindow *m_zB = nullptr;
    wxPanel *m_clipParent = nullptr;
    CompositionScenarioWindow *m_clipped = nullptr;
    wxScrolledWindow *m_scroll = nullptr;
    CompositionScenarioWindow *m_scrolledSlot = nullptr;
#if wxUSE_NOTEBOOK
    wxNotebook *m_notebook = nullptr;
#endif
#if wxUSE_SPLITTER
    wxSplitterWindow *m_splitter = nullptr;
    wxPanel *m_splitterFirst = nullptr;
    wxPanel *m_splitterSecond = nullptr;
#endif
    wxWinUISlot *m_zASlot = nullptr;
    wxWinUISlot *m_zBSlot = nullptr;
    wxWinUISlot *m_clippedSlot = nullptr;
    wxWinUISlot *m_scrolledSlotState = nullptr;
    bool m_setupOk = false;
    bool m_wasActivated = false;

    winrt::event_token m_renderToken{};
    wxTimer m_staleTimer;
    LatencyCompletion m_latencyCompletion;
    std::vector<ScrollLatencySample> m_latencySamples;
    std::vector<long long> m_renderIntervalsUs;
    unsigned m_targetHz = 60;
    unsigned m_requestedSamples = 0;
    unsigned m_pendingBacklog = 0;
    unsigned m_maxBacklog = 0;
    unsigned m_pendingScheduledFlushes = 0;
    unsigned m_maxScheduledFlushes = 0;
    ULONGLONG m_latencyStartedTick = 0;
    long long m_mutationQpc = 0;
    long long m_slotObservedQpc = 0;
    long long m_lastRenderQpc = 0;
    unsigned m_pollAttempts = 0;
    wxRect m_expectedLatencyRect;
    bool m_latencyForward = false;
    bool m_waitingForRender = false;
    bool m_latencyFinished = false;
    unsigned m_warmupRenderCallbacksObserved = 0;
    bool m_warmupMutationScheduled = false;
    bool m_warmupMutation = false;
    bool m_warmupCompleted = false;
};

} // anonymous namespace

class SpikeApp : public wxApp
{
public:
#if wxUSE_CMDLINE_PARSER
    void OnInitCmdLine(wxCmdLineParser& parser) override
    {
        wxApp::OnInitCmdLine(parser);
        parser.AddSwitch(
            "", "diagnostics-self-test",
            "run the non-interactive sent-message diagnostics self-test");
        parser.AddSwitch(
            "", "input-scenarios",
            "run the deterministic root-pointer scenario suite");
        parser.AddSwitch(
            "", "zorder-scenarios",
            "run deterministic z-order, clip and splitter scenarios");
        parser.AddSwitch(
            "", "scroll-latency",
            "run the opt-in scroll-to-composition latency probe");
        parser.AddOption(
            "", "scenario-result",
            "write the selected scenario result to this fresh JSON path",
            wxCMD_LINE_VAL_STRING);
        parser.AddOption(
            "", "scenario-timeout-ms",
            "bound the selected in-process scenario suite",
            wxCMD_LINE_VAL_NUMBER);
        parser.AddOption(
            "", "target-hz",
            "latency budget refresh rate (60, 120 or 165)",
            wxCMD_LINE_VAL_NUMBER);
        parser.AddOption(
            "", "latency-samples",
            "number of scroll latency samples (8 through 200)",
            wxCMD_LINE_VAL_NUMBER);
    }

    bool OnCmdLineParsed(wxCmdLineParser& parser) override
    {
        if ( !wxApp::OnCmdLineParsed(parser) )
            return false;

        m_diagnosticsSelfTest =
            parser.Found("diagnostics-self-test");
        m_pointerScenariosRequested =
            parser.Found("input-scenarios");
        m_zOrderScenariosRequested =
            parser.Found("zorder-scenarios");
        m_scrollLatencyRequested =
            parser.Found("scroll-latency");
        (void)parser.Found(
            "scenario-result", &m_scenarioResultPath);

        long timeout = 0;
        if ( parser.Found("scenario-timeout-ms", &timeout) )
        {
            if ( timeout < 1000 || timeout > 60000 )
            {
                fputws(
                    L"winuispike: --scenario-timeout-ms must be "
                    L"between 1000 and 60000\n",
                    stderr);
                m_selfTestExitCode = kPointerScenarioExitCli;
            }
            else
            {
                m_scenarioTimeoutMs =
                    static_cast<unsigned long>(timeout);
            }
        }

        long targetHz = 0;
        if ( parser.Found("target-hz", &targetHz) )
        {
            m_targetHzSpecified = true;
            if ( targetHz != 60 && targetHz != 120 && targetHz != 165 )
            {
                fputws(
                    L"winuispike: --target-hz must be 60, 120 or 165\n",
                    stderr);
                m_selfTestExitCode = kCompositionScenarioExitCli;
            }
            else
            {
                m_targetHz = static_cast<unsigned>(targetHz);
            }
        }

        long latencySamples = 0;
        if ( parser.Found("latency-samples", &latencySamples) )
        {
            m_latencySamplesSpecified = true;
            if ( latencySamples < 8 || latencySamples > 200 )
            {
                fputws(
                    L"winuispike: --latency-samples must be between "
                    L"8 and 200\n",
                    stderr);
                m_selfTestExitCode = kCompositionScenarioExitCli;
            }
            else
            {
                m_latencySampleCount =
                    static_cast<unsigned>(latencySamples);
            }
        }

        return true;
    }
#endif

    bool OnInit() override
    {
        if ( !wxApp::OnInit() )
            return false;

#if !wxUSE_CMDLINE_PARSER
        // Keep a minimal fallback for configurations which compile out the
        // standard parser. Normal builds use OnCmdLineParsed() above so every
        // syntax accepted by wxCmdLineParser has exactly the same meaning.
        for ( int i = 1; i < argc; ++i )
        {
            const wxString argument(argv[i]);
            if ( argument == "--diagnostics-self-test" )
            {
                m_diagnosticsSelfTest = true;
            }
            else if ( argument == "--input-scenarios" )
            {
                m_pointerScenariosRequested = true;
            }
            else if ( argument == "--zorder-scenarios" )
            {
                m_zOrderScenariosRequested = true;
            }
            else if ( argument == "--scroll-latency" )
            {
                m_scrollLatencyRequested = true;
            }
            else if ( argument.StartsWith("--scenario-result=") )
            {
                m_scenarioResultPath =
                    argument.AfterFirst('=');
            }
            else if ( argument.StartsWith("--scenario-timeout-ms=") )
            {
                unsigned long timeout = 0;
                if ( !argument.AfterFirst('=').ToULong(&timeout) ||
                     timeout < 1000 || timeout > 60000 )
                {
                    fputws(
                        L"winuispike: --scenario-timeout-ms must be "
                        L"between 1000 and 60000\n",
                        stderr);
                    m_selfTestExitCode = kPointerScenarioExitCli;
                    return true;
                }
                m_scenarioTimeoutMs = timeout;
            }
            else if ( argument.StartsWith("--target-hz=") )
            {
                m_targetHzSpecified = true;
                unsigned long targetHz = 0;
                if ( !argument.AfterFirst('=').ToULong(&targetHz) ||
                     (targetHz != 60 &&
                      targetHz != 120 &&
                      targetHz != 165) )
                {
                    fputws(
                        L"winuispike: --target-hz must be 60, 120 or 165\n",
                        stderr);
                    m_selfTestExitCode = kCompositionScenarioExitCli;
                    return true;
                }
                m_targetHz = static_cast<unsigned>(targetHz);
            }
            else if ( argument.StartsWith("--latency-samples=") )
            {
                m_latencySamplesSpecified = true;
                unsigned long samples = 0;
                if ( !argument.AfterFirst('=').ToULong(&samples) ||
                     samples < 8 || samples > 200 )
                {
                    fputws(
                        L"winuispike: --latency-samples must be between "
                        L"8 and 200\n",
                        stderr);
                    m_selfTestExitCode = kCompositionScenarioExitCli;
                    return true;
                }
                m_latencySampleCount =
                    static_cast<unsigned>(samples);
            }
        }
#endif // !wxUSE_CMDLINE_PARSER

        if ( m_selfTestExitCode >= 0 )
            return true;

        const unsigned selectedModes =
            static_cast<unsigned>(m_diagnosticsSelfTest) +
            static_cast<unsigned>(m_pointerScenariosRequested) +
            static_cast<unsigned>(m_zOrderScenariosRequested) +
            static_cast<unsigned>(m_scrollLatencyRequested);
        if ( selectedModes > 1 )
        {
            fputws(L"winuispike: select only one self-test mode\n", stderr);
            m_selfTestExitCode = kCompositionScenarioExitCli;
            return true;
        }
        if ( (m_targetHzSpecified || m_latencySamplesSpecified) &&
             !m_scrollLatencyRequested )
        {
            fputws(
                L"winuispike: --target-hz and --latency-samples require "
                L"--scroll-latency\n",
                stderr);
            m_selfTestExitCode = kCompositionScenarioExitCli;
            return true;
        }

        if ( m_diagnosticsSelfTest )
        {
            m_selfTestExitCode = RunSpikeDiagnosticsSelfTest();
            return true;
        }

        if ( m_pointerScenariosRequested )
        {
            if ( m_scenarioResultPath.empty() )
            {
                fputws(
                    L"winuispike: --input-scenarios requires "
                    L"--scenario-result=<fresh-file>\n",
                    stderr);
                m_selfTestExitCode = kPointerScenarioExitCli;
                return true;
            }

            m_pointerScenarioMode = true;
            m_scenarioStarted = ::GetTickCount64();
            m_scenarioLog = new wxLogStderr();
            m_previousLog = wxLog::SetActiveTarget(m_scenarioLog);
            m_scenarioFrame = new PointerScenarioFrame();
            SetTopWindow(m_scenarioFrame);

            m_scenarioTimer.SetOwner(this);
            Bind(wxEVT_TIMER, &SpikeApp::OnPointerScenarioTimeout, this,
                 m_scenarioTimer.GetId());
            m_scenarioTimer.StartOnce(
                static_cast<int>(m_scenarioTimeoutMs));

            // Let source attachment and the first coalesced host flush drain
            // before asking VisualTreeHelper to classify the root.
            CallAfter(
                [this]()
                {
                    CallAfter(&SpikeApp::RunPointerScenarios);
                });
            return true;
        }

        if ( m_zOrderScenariosRequested || m_scrollLatencyRequested )
        {
            if ( m_scenarioResultPath.empty() )
            {
                fputws(
                    L"winuispike: composition scenarios require "
                    L"--scenario-result=<fresh-file>\n",
                    stderr);
                m_selfTestExitCode = kCompositionScenarioExitCli;
                return true;
            }

            m_compositionScenarioMode = true;
            m_scenarioStarted = ::GetTickCount64();
            m_scenarioLog = new wxLogStderr();
            m_previousLog = wxLog::SetActiveTarget(m_scenarioLog);
            m_compositionFrame = new CompositionScenarioFrame();
            SetTopWindow(m_compositionFrame);

            m_compositionTimer.SetOwner(this);
            Bind(wxEVT_TIMER, &SpikeApp::OnCompositionScenarioTimeout, this,
                 m_compositionTimer.GetId());
            m_compositionTimer.StartOnce(
                static_cast<int>(m_scenarioTimeoutMs));

            m_compositionLayoutTimer.SetOwner(this);
            Bind(wxEVT_TIMER, &SpikeApp::OnCompositionLayoutTimer, this,
                 m_compositionLayoutTimer.GetId());

            // The notebook band and first coalesced host layout need a real
            // dispatcher/layout interval, not a recursively saturated
            // CallAfter queue. Latency mode keeps its existing prompt start.
            if ( m_zOrderScenariosRequested )
                m_compositionLayoutTimer.StartOnce(10);
            else
                CallAfter(&SpikeApp::RunCompositionScenarios);
            return true;
        }

        (new SpikeFrame())->Show(true);
        return true;
    }

    int OnRun() override
    {
        if ( m_selfTestExitCode >= 0 )
            return m_selfTestExitCode;

        const int result = wxApp::OnRun();
        if ( m_pointerScenarioMode )
            return m_pointerScenarioExitCode;
        return m_compositionScenarioMode
                 ? m_compositionScenarioExitCode
                 : result;
    }

    int OnExit() override
    {
        m_scenarioTimer.Stop();
        m_compositionTimer.Stop();
        m_compositionLayoutTimer.Stop();
        if ( m_scenarioFrame )
        {
            m_scenarioFrame->Destroy();
            m_scenarioFrame = nullptr;
        }
        if ( m_compositionFrame )
        {
            m_compositionFrame->Destroy();
            m_compositionFrame = nullptr;
        }
        if ( m_scenarioLog )
        {
            (void)wxLog::SetActiveTarget(m_previousLog);
            delete m_scenarioLog;
            m_scenarioLog = nullptr;
            m_previousLog = nullptr;
        }
        CloseProbeLog();
        return wxApp::OnExit();
    }

private:
    void RunCompositionScenarios()
    {
        if ( m_compositionScenarioFinished )
            return;

        if ( !m_compositionFrame )
        {
            FinishCompositionScenarios(
                kCompositionScenarioExitSetup, std::string());
            return;
        }

        try
        {
            if ( m_zOrderScenariosRequested )
            {
                if ( !m_compositionFrame->IsZOrderLayoutReady() &&
                     m_compositionLayoutRetries++ < 200 )
                {
                    m_compositionLayoutTimer.StartOnce(10);
                    return;
                }
                const auto result =
                    m_compositionFrame->RunZOrderScenarios(
                        m_scenarioStarted);
                FinishCompositionScenarios(result.first, result.second);
            }
            else
            {
                m_compositionFrame->StartScrollLatency(
                    m_targetHz, m_latencySampleCount, m_scenarioStarted,
                    [this](int exitCode, const std::string& serialized)
                    {
                        FinishCompositionScenarios(
                            exitCode, serialized);
                    });
            }
        }
        catch ( const winrt::hresult_error& error )
        {
            std::ostringstream json;
            json << "{\"schema_version\":1,"
                 << "\"suite\":\"winui-composition-scenarios\","
                 << "\"status\":\"fail\","
                 << "\"error\":\""
                 << JsonEscape(
                        wxString(error.message().c_str()).utf8_string())
                 << "\"}\n";
            FinishCompositionScenarios(
                kCompositionScenarioExitException, json.str());
        }
        catch ( const std::exception& error )
        {
            std::ostringstream json;
            json << "{\"schema_version\":1,"
                 << "\"suite\":\"winui-composition-scenarios\","
                 << "\"status\":\"fail\","
                 << "\"error\":\"" << JsonEscape(error.what())
                 << "\"}\n";
            FinishCompositionScenarios(
                kCompositionScenarioExitException, json.str());
        }
    }

    void OnCompositionScenarioTimeout(wxTimerEvent&)
    {
        if ( m_compositionScenarioFinished )
            return;

        std::ostringstream json;
        json << "{\"schema_version\":1,"
             << "\"suite\":\"winui-composition-scenarios\","
             << "\"status\":\"timeout\","
             << "\"exit_code\":" << kCompositionScenarioExitTimeout
             << "}\n";
        FinishCompositionScenarios(
            kCompositionScenarioExitTimeout, json.str());
    }

    void OnCompositionLayoutTimer(wxTimerEvent&)
    {
        RunCompositionScenarios();
    }

    void FinishCompositionScenarios(
        int exitCode,
        const std::string& serialized)
    {
        if ( m_compositionScenarioFinished )
            return;

        m_compositionScenarioFinished = true;
        m_compositionTimer.Stop();
        m_compositionLayoutTimer.Stop();
        wxString writeError;
        if ( serialized.empty() ||
             !WritePointerScenarioResult(
                 m_scenarioResultPath, serialized, &writeError) )
        {
            fwprintf(stderr,
                     L"winuispike: cannot write composition result: %ls\n",
                     writeError.wc_str());
            exitCode = kCompositionScenarioExitWrite;
        }
        else
        {
            const char * const outcome =
                m_scrollLatencyRequested &&
                    exitCode == kCompositionScenarioExitUnqualified
                  ? "UNQUALIFIED"
                  : exitCode != 0
                      ? "FAIL"
                      : "PASS";
            fprintf(stderr,
                    "winuispike: composition scenarios %s "
                    "(%llu ms, exit %d)\n",
                    outcome,
                    static_cast<unsigned long long>(
                        ::GetTickCount64() - m_scenarioStarted),
                    exitCode);
        }

        m_compositionScenarioExitCode = exitCode;
        if ( m_compositionFrame )
        {
            m_compositionFrame->Destroy();
            m_compositionFrame = nullptr;
            SetTopWindow(nullptr);
        }
        ExitMainLoop();
    }

    void RunPointerScenarios()
    {
        if ( m_pointerScenarioFinished )
            return;

        std::vector<PointerScenarioResult> results;
        int exitCode = 0;
        try
        {
            if ( !m_scenarioFrame )
            {
                exitCode = kPointerScenarioExitSetup;
            }
            else if ( !m_scenarioFrame->HasHost() )
            {
                exitCode = kPointerScenarioExitHost;
            }
            else if ( !m_scenarioFrame->IsSetupOk() )
            {
                exitCode = kPointerScenarioExitSetup;
            }
            else
            {
                results = m_scenarioFrame->RunScenarios(
                    m_scenarioStarted + m_scenarioTimeoutMs);
                for ( const PointerScenarioResult& result : results )
                {
                    if ( !result.passed )
                    {
                        exitCode = kPointerScenarioExitFailed;
                        break;
                    }
                }
                if ( ::GetTickCount64() >
                        m_scenarioStarted + m_scenarioTimeoutMs )
                {
                    exitCode = kPointerScenarioExitTimeout;
                }
            }
        }
        catch ( const winrt::hresult_error& error )
        {
            PointerScenarioResult failure;
            failure.name = "unhandled-winrt-exception";
            failure.passed = false;
            failure.failure =
                wxString(error.message().c_str()).utf8_string();
            results.push_back(std::move(failure));
            exitCode = kPointerScenarioExitException;
        }
        catch ( const std::exception& error )
        {
            PointerScenarioResult failure;
            failure.name = "unhandled-standard-exception";
            failure.passed = false;
            failure.failure = error.what();
            results.push_back(std::move(failure));
            exitCode = kPointerScenarioExitException;
        }
        catch ( ... )
        {
            PointerScenarioResult failure;
            failure.name = "unhandled-unknown-exception";
            failure.passed = false;
            failure.failure = "unknown exception";
            results.push_back(std::move(failure));
            exitCode = kPointerScenarioExitException;
        }

        FinishPointerScenarios(exitCode, results);
    }

    void OnPointerScenarioTimeout(wxTimerEvent&)
    {
        if ( m_pointerScenarioFinished )
            return;

        PointerScenarioResult failure;
        failure.name = "event-loop-watchdog";
        failure.passed = false;
        failure.failure = "scenario event-loop deadline expired";
        FinishPointerScenarios(
            kPointerScenarioExitTimeout,
            std::vector<PointerScenarioResult>{ std::move(failure) });
    }

    void FinishPointerScenarios(
        int exitCode,
        const std::vector<PointerScenarioResult>& results)
    {
        if ( m_pointerScenarioFinished )
            return;

        m_pointerScenarioFinished = true;
        m_scenarioTimer.Stop();
        const ULONGLONG elapsed =
            ::GetTickCount64() - m_scenarioStarted;
        const bool activated =
            m_scenarioFrame && m_scenarioFrame->WasActivated();
        const std::string serialized =
            SerializePointerScenarioResults(
                exitCode, m_scenarioStarted, elapsed, activated, results);
        wxString writeError;
        if ( !WritePointerScenarioResult(
                 m_scenarioResultPath, serialized, &writeError) )
        {
            fwprintf(stderr,
                     L"winuispike: cannot write scenario result: %ls\n",
                     writeError.wc_str());
            exitCode = kPointerScenarioExitWrite;
        }
        else
        {
            fprintf(stderr,
                    "winuispike: pointer scenarios %s (%llu ms, exit %d)\n",
                    exitCode == 0 ? "PASS" : "FAIL",
                    static_cast<unsigned long long>(elapsed),
                    exitCode);
        }

        m_pointerScenarioExitCode = exitCode;
        if ( m_scenarioFrame )
        {
            m_scenarioFrame->Destroy();
            m_scenarioFrame = nullptr;
            SetTopWindow(nullptr);
        }
        ExitMainLoop();
    }

    int m_selfTestExitCode = -1;
    bool m_diagnosticsSelfTest = false;
    bool m_pointerScenariosRequested = false;
    bool m_zOrderScenariosRequested = false;
    bool m_scrollLatencyRequested = false;
    bool m_pointerScenarioMode = false;
    bool m_pointerScenarioFinished = false;
    bool m_compositionScenarioMode = false;
    bool m_compositionScenarioFinished = false;
    int m_pointerScenarioExitCode = kPointerScenarioExitException;
    int m_compositionScenarioExitCode =
        kCompositionScenarioExitException;
    unsigned m_targetHz = 60;
    unsigned m_latencySampleCount = 24;
    bool m_targetHzSpecified = false;
    bool m_latencySamplesSpecified = false;
    unsigned m_compositionLayoutRetries = 0;
    unsigned long m_scenarioTimeoutMs = 10000;
    ULONGLONG m_scenarioStarted = 0;
    wxString m_scenarioResultPath;
    PointerScenarioFrame *m_scenarioFrame = nullptr;
    CompositionScenarioFrame *m_compositionFrame = nullptr;
    wxTimer m_scenarioTimer;
    wxTimer m_compositionTimer;
    wxTimer m_compositionLayoutTimer;
    wxLog *m_previousLog = nullptr;
    wxLogStderr *m_scenarioLog = nullptr;
};

wxIMPLEMENT_APP(SpikeApp);
