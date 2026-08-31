///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiglcanvas.cpp
// Purpose:     deterministic WinUI wxGLCanvas hybrid qualification
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#if wxUSE_BUTTON && wxUSE_TEXTCTRL

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/textctrl.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/cursor.h"
#include "wx/msw/private.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/winui.h"

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    #include "wx/dataobj.h"
    #include "wx/dnd.h"
    #include "wx/msw/private/dropsession.h"
    #include "wx/winui/private/dropbroker.h"
#endif

#include <UIAutomation.h>
#include <UIAutomationCoreApi.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

namespace
{

void DrainDispatch(unsigned rounds = 20)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainUntil(Predicate predicate, unsigned rounds = 160)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainDispatch(1);
    }

    return predicate();
}

bool DrainToQuiescence()
{
    for ( unsigned i = 0; i < 60; ++i )
    {
        const unsigned schedules =
            wxWinUITopLevelHost::GetFlushScheduleCount();
        const unsigned runs = wxWinUITopLevelHost::GetFlushRunCount();
        const unsigned callbacks =
            wxWinUITopLevelHost::GetFlushCallbackAttemptCount();
        DrainDispatch(2);
        if ( schedules == wxWinUITopLevelHost::GetFlushScheduleCount() &&
             runs == wxWinUITopLevelHost::GetFlushRunCount() &&
             callbacks ==
                 wxWinUITopLevelHost::GetFlushCallbackAttemptCount() )
        {
            return true;
        }
    }

    return false;
}

bool SameTransientRegistry(const wxWinUITransientRegistrySnapshot& lhs,
                           const wxWinUITransientRegistrySnapshot& rhs)
{
    return lhs.ownerCount == rhs.ownerCount &&
           lhs.transientCount == rhs.transientCount &&
           lhs.cancelCallbackCount == rhs.cancelCallbackCount &&
           lhs.modalCount == rhs.modalCount &&
           lhs.popupCount == rhs.popupCount &&
           lhs.teachingTipCount == rhs.teachingTipCount &&
           lhs.pendingOwnerRetireCount == rhs.pendingOwnerRetireCount &&
           lhs.activeOwnerRetirementCount ==
               rhs.activeOwnerRetirementCount;
}

struct RuntimeSnapshot
{
    static RuntimeSnapshot Capture()
    {
        RuntimeSnapshot snapshot;
        snapshot.hosts = wxWinUITopLevelHost::GetLiveHostCount();
        snapshot.hostLifetimes =
            wxWinUITopLevelHost::GetLiveHostLifetimeCount();
        snapshot.slots = wxWinUITopLevelHost::GetLiveSlotCount();
        snapshot.slotLifetimes =
            wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
        snapshot.subclasses =
            wxWinUITopLevelHost::GetLiveSubclassContextCount();
        snapshot.loadedHooks =
            wxWinUITopLevelHost::GetLiveLoadedHookCount();
        snapshot.automationObservers =
            wxWinUITopLevelHost::
                GetLiveAutomationNameStyleObserverCountForTest();
        snapshot.contentObservers =
            wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest();
        snapshot.invisibleShellProviders =
            wxWinUITestGetLiveInvisibleShellProviderCount();
        snapshot.pendingFocusMigrations =
            wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
        snapshot.slotHandlerAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount();
        snapshot.slotHandlerRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
        snapshot.rootHandlerAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount();
        snapshot.rootHandlerRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount();
        snapshot.transients =
            wxWinUIGetTransientRegistrySnapshotForTesting();
        snapshot.wxCapture = wxWindow::GetCapture();
        snapshot.nativeCapture = ::GetCapture();
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        snapshot.dropMutationGeneration =
            wxMSWOleGetDropTargetMutationGeneration();
#endif
        return snapshot;
    }

    bool IsRestored() const
    {
        return wxWinUITopLevelHost::GetLiveHostCount() == hosts &&
               wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                   hostLifetimes &&
               wxWinUITopLevelHost::GetLiveSlotCount() == slots &&
               wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                   slotLifetimes &&
               wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
                   subclasses &&
               wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks &&
               wxWinUITopLevelHost::
                       GetLiveAutomationNameStyleObserverCountForTest() ==
                   automationObservers &&
               wxWinUITopLevelHost::
                       GetLiveContentLoadedObserverCountForTest() ==
                   contentObservers &&
               wxWinUITestGetLiveInvisibleShellProviderCount() ==
                   invisibleShellProviders &&
               wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
                   pendingFocusMigrations &&
               wxWinUITopLevelHost::GetSlotHandlerAddCount() -
                       slotHandlerAdds ==
                   wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                       slotHandlerRevokes &&
               wxWinUITopLevelHost::GetRootHandlerAddCount() -
                       rootHandlerAdds ==
                   wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                       rootHandlerRevokes &&
               SameTransientRegistry(
                   wxWinUIGetTransientRegistrySnapshotForTesting(),
                   transients) &&
               wxWindow::GetCapture() == wxCapture &&
               ::GetCapture() == nativeCapture;
    }

    void CheckRestored(bool requireHostTraffic = true) const
    {
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts);
        CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
              hostLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
              slotLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              subclasses);
        CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks);
        CHECK(wxWinUITopLevelHost::
                  GetLiveAutomationNameStyleObserverCountForTest() ==
              automationObservers);
        CHECK(wxWinUITopLevelHost::
                  GetLiveContentLoadedObserverCountForTest() ==
              contentObservers);
        CHECK(wxWinUITestGetLiveInvisibleShellProviderCount() ==
              invisibleShellProviders);
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              pendingFocusMigrations);

        const unsigned slotAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotHandlerAdds;
        const unsigned slotRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                slotHandlerRevokes;
        const unsigned rootAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount() - rootHandlerAdds;
        const unsigned rootRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                rootHandlerRevokes;
        if ( requireHostTraffic )
        {
            CHECK(slotAdds > 0);
            CHECK(rootAdds > 0);
        }
        CHECK(slotAdds == slotRevokes);
        CHECK(rootAdds == rootRevokes);

        CHECK(SameTransientRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(), transients));
        CHECK(wxWindow::GetCapture() == wxCapture);
        CHECK(::GetCapture() == nativeCapture);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        CHECK(wxMSWOleGetDropTargetMutationGeneration() >=
              dropMutationGeneration);
#endif
    }

    unsigned hosts = 0;
    unsigned hostLifetimes = 0;
    unsigned slots = 0;
    unsigned slotLifetimes = 0;
    unsigned subclasses = 0;
    unsigned loadedHooks = 0;
    unsigned automationObservers = 0;
    unsigned contentObservers = 0;
    unsigned invisibleShellProviders = 0;
    unsigned pendingFocusMigrations = 0;
    unsigned slotHandlerAdds = 0;
    unsigned slotHandlerRevokes = 0;
    unsigned rootHandlerAdds = 0;
    unsigned rootHandlerRevokes = 0;
    wxWinUITransientRegistrySnapshot transients;
    wxWindow *wxCapture = nullptr;
    HWND nativeCapture = nullptr;
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    std::uint64_t dropMutationGeneration = 0;
#endif
};

class WGLCurrentRestoreGuard final
{
public:
    WGLCurrentRestoreGuard()
        : m_dc(::wglGetCurrentDC()), m_context(::wglGetCurrentContext())
    {
    }

    ~WGLCurrentRestoreGuard()
    {
        if ( m_context && m_dc )
            (void)::wglMakeCurrent(m_dc, m_context);
        else
            (void)::wglMakeCurrent(nullptr, nullptr);
    }

private:
    HDC m_dc;
    HGLRC m_context;

    wxDECLARE_NO_COPY_CLASS(WGLCurrentRestoreGuard);
};

class NativeFocusRestoreGuard final
{
public:
    NativeFocusRestoreGuard()
        : m_hwnd(::GetFocus()),
          m_generation(m_hwnd
              ? wxWinUIMSWGetNativeHwndGeneration(
                    reinterpret_cast<WXHWND>(m_hwnd))
              : 0)
    {
    }

    ~NativeFocusRestoreGuard()
    {
        HWND target = nullptr;
        if ( m_hwnd && m_generation &&
             wxWinUIMSWGetNativeHwndGeneration(
                 reinterpret_cast<WXHWND>(m_hwnd)) == m_generation )
        {
            target = m_hwnd;
        }
        ::SetFocus(target);
    }

private:
    HWND m_hwnd;
    unsigned long long m_generation;

    wxDECLARE_NO_COPY_CLASS(NativeFocusRestoreGuard);
};

class NativeCursorRestoreGuard final
{
public:
    NativeCursorRestoreGuard() : m_cursor(::GetCursor()) {}
    ~NativeCursorRestoreGuard() { ::SetCursor(m_cursor); }

private:
    HCURSOR m_cursor;

    wxDECLARE_NO_COPY_CLASS(NativeCursorRestoreGuard);
};

class ProbeGLCanvas final : public wxGLCanvas
{
public:
    bool CreateProbe(wxWindow *parent,
                     const wxGLAttributes& attributes,
                     wxWindowID id,
                     const wxPoint& position,
                     const wxSize& size,
                     const wxString& name)
    {
        return Create(parent, attributes, id, position, size,
                      wxBORDER_NONE, name);
    }

    unsigned GetMouseMoveCount() const { return m_mouseMoves; }
    unsigned GetSetCursorCount() const { return m_setCursors; }

protected:
    WXLRESULT MSWWindowProc(WXUINT message,
                            WXWPARAM wParam,
                            WXLPARAM lParam) override
    {
        if ( message == WM_MOUSEMOVE )
            ++m_mouseMoves;
        else if ( message == WM_SETCURSOR )
            ++m_setCursors;

        return wxGLCanvas::MSWWindowProc(message, wParam, lParam);
    }

private:
    unsigned m_mouseMoves = 0;
    unsigned m_setCursors = 0;
};

class ProbeGLContext final : public wxGLContext
{
public:
    explicit ProbeGLContext(wxGLCanvas *canvas)
        : wxGLContext(canvas)
    {
    }

    // wxGLContext's destructor deliberately ignores wglDeleteContext()'s
    // return value. The lifetime spike needs to observe that exact WGL result
    // while preventing the base destructor from deleting a successful handle
    // twice. On failure the handle remains owned so the test can clear the
    // current context and perform an explicit, checked retry before teardown.
    bool DeleteForTest()
    {
        if ( !m_glContext )
            return true;

        if ( !::wglDeleteContext(m_glContext) )
            return false;

        m_glContext = nullptr;
        return true;
    }
};

wxGLAttributes MakeDisplayAttributes()
{
    wxGLAttributes attributes;
    attributes.PlatformDefaults()
              .RGBA()
              .DoubleBuffer()
              .Depth(16);
    attributes.EndList();
    return attributes;
}

bool HasTestPixelFormat(const wxGLAttributes& attributes)
{
    const HDC enteringDC = ::wglGetCurrentDC();
    const HGLRC enteringContext = ::wglGetCurrentContext();
    const bool supported = wxGLCanvas::IsDisplaySupported(attributes);
    DrainDispatch();
    CHECK(::wglGetCurrentDC() == enteringDC);
    CHECK(::wglGetCurrentContext() == enteringContext);
    return supported;
}

bool ShowOffscreenWithoutActivation(wxFrame *frame, unsigned ordinal)
{
    const HWND hwnd = frame ? static_cast<HWND>(frame->GetHWND()) : nullptr;
    if ( !hwnd )
        return false;

    const int left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const RECT desktop = {
        left,
        top,
        left + ::GetSystemMetrics(SM_CXVIRTUALSCREEN),
        top + ::GetSystemMetrics(SM_CYVIRTUALSCREEN)
    };
    frame->Move(wxPoint(left - 7200 - static_cast<int>(ordinal) * 500,
                        top - 7200));

    const auto isOutsideDesktop = [hwnd, &desktop]()
    {
        RECT windowRect = {};
        RECT intersection = {};
        return ::GetWindowRect(hwnd, &windowRect) &&
               !::IntersectRect(&intersection, &windowRect, &desktop);
    };
    if ( !isOutsideDesktop() )
        return false;

    frame->ShowWithoutActivating();
    if ( !isOutsideDesktop() )
    {
        frame->Hide();
        return false;
    }

    return true;
}

class TwoTLWGLFixture final
{
public:
    bool Create(const wxGLAttributes& attributes)
    {
        frameA = new wxFrame(nullptr, wxID_ANY, "gl-canvas-a",
                             wxDefaultPosition, wxSize(460, 280),
                             wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                                 wxFRAME_TOOL_WINDOW);
        frameB = new wxFrame(nullptr, wxID_ANY, "gl-canvas-b",
                             wxDefaultPosition, wxSize(460, 280),
                             wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                                 wxFRAME_TOOL_WINDOW);
        panelA = new wxPanel(frameA, wxID_ANY,
                             wxPoint(0, 0), wxSize(430, 230));
        panelB = new wxPanel(frameB, wxID_ANY,
                             wxPoint(0, 0), wxSize(430, 230));

        if ( !CreateCanvas(canvasA, panelA, attributes, 15301,
                           wxPoint(24, 24), wxSize(180, 88),
                           "gl-primary-a") ||
             !CreateCanvas(canvasB, panelB, attributes, 15302,
                           wxPoint(24, 24), wxSize(180, 88),
                           "gl-primary-b") )
        {
            Destroy();
            return false;
        }

        buttonA = new wxButton(panelA, wxID_ANY, "xaml-a",
                               wxPoint(250, 24), wxSize(130, 48));
        buttonB = new wxButton(panelB, wxID_ANY, "xaml-b",
                               wxPoint(250, 24), wxSize(130, 48));
        textA = new wxTextCtrl(panelA, wxID_ANY, "text-a",
                               wxPoint(250, 100), wxSize(140, 36));
        textB = new wxTextCtrl(panelB, wxID_ANY, "text-b",
                               wxPoint(250, 100), wxSize(140, 36));

        if ( !ShowOffscreenWithoutActivation(frameA, 0) ||
             !ShowOffscreenWithoutActivation(frameB, 1) ||
             !DrainToQuiescence() )
        {
            Destroy();
            return false;
        }

        hostA = wxWinUITopLevelHost::FindSlotOwner(buttonA);
        hostB = wxWinUITopLevelHost::FindSlotOwner(buttonB);
        if ( !hostA || !hostB || hostA == hostB ||
             wxWinUITopLevelHost::FindSlotOwner(textA) != hostA ||
             wxWinUITopLevelHost::FindSlotOwner(textB) != hostB ||
             wxWinUITopLevelHost::FindSlotOwner(canvasA) ||
             wxWinUITopLevelHost::FindSlotOwner(canvasB) )
        {
            Destroy();
            return false;
        }

        hostA->FlushSync();
        hostB->FlushSync();
        return true;
    }

    ~TwoTLWGLFixture()
    {
        Destroy();
    }

    void Destroy()
    {
        if ( frameA )
        {
            delete frameA;
            frameA = nullptr;
        }
        if ( frameB )
        {
            delete frameB;
            frameB = nullptr;
        }

        panelA = nullptr;
        panelB = nullptr;
        canvasA = nullptr;
        canvasB = nullptr;
        buttonA = nullptr;
        buttonB = nullptr;
        textA = nullptr;
        textB = nullptr;
        hostA = nullptr;
        hostB = nullptr;
        DrainDispatch();
    }

    wxFrame *frameA = nullptr;
    wxFrame *frameB = nullptr;
    wxPanel *panelA = nullptr;
    wxPanel *panelB = nullptr;
    ProbeGLCanvas *canvasA = nullptr;
    ProbeGLCanvas *canvasB = nullptr;
    wxButton *buttonA = nullptr;
    wxButton *buttonB = nullptr;
    wxTextCtrl *textA = nullptr;
    wxTextCtrl *textB = nullptr;
    wxWinUITopLevelHost *hostA = nullptr;
    wxWinUITopLevelHost *hostB = nullptr;

private:
    static bool CreateCanvas(ProbeGLCanvas *&canvas,
                             wxWindow *parent,
                             const wxGLAttributes& attributes,
                             wxWindowID id,
                             const wxPoint& position,
                             const wxSize& size,
                             const wxString& name)
    {
        std::unique_ptr<ProbeGLCanvas> pending(new ProbeGLCanvas);
        const bool created = pending->CreateProbe(
            parent, attributes, id, position, size, name);

        // CreateWindow() adopts the object immediately after CreateBase().
        // Before that point the temporary owner must delete it; after that
        // point the parent's child list is the sole lifetime owner, including
        // every later MSWCreate/pixel-format failure path.
        if ( pending->GetParent() == parent )
            canvas = pending.release();

        return created && canvas != nullptr;
    }
};

struct RenderResult
{
    std::uint64_t checksum = 0;
    std::array<GLubyte, 8 * 8 * 4> pixels{};
};

void DrainGLErrors()
{
    while ( ::glGetError() != GL_NO_ERROR )
    {
    }
}

RenderResult RenderSolid(wxGLCanvas& canvas,
                         const wxGLContext& context,
                         const std::array<GLubyte, 4>& colour)
{
    REQUIRE(context.SetCurrent(canvas));
    REQUIRE(::wglGetCurrentContext() == context.GetGLRC());
    REQUIRE(::wglGetCurrentDC() == canvas.GetHDC());
    const wxSize size = canvas.GetClientSize();
    REQUIRE(size.x >= 8);
    REQUIRE(size.y >= 8);

    DrainGLErrors();
    ::glViewport(0, 0, size.x, size.y);
    ::glDisable(GL_DITHER);
    ::glReadBuffer(GL_BACK);
    ::glClearColor(colour[0] / 255.0f,
                   colour[1] / 255.0f,
                   colour[2] / 255.0f,
                   colour[3] / 255.0f);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ::glFinish();

    std::array<GLubyte, 8 * 8 * 4> pixels{};
    ::glReadPixels(size.x / 2 - 4, size.y / 2 - 4,
                   8, 8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    REQUIRE(::glGetError() == GL_NO_ERROR);

    RenderResult result;
    result.pixels = pixels;
    result.checksum = UINT64_C(1469598103934665603);
    for ( const GLubyte value : pixels )
    {
        result.checksum ^= value;
        result.checksum *= UINT64_C(1099511628211);
    }
    return result;
}

void CheckColour(const RenderResult& result,
                 const std::array<GLubyte, 4>& expected)
{
    INFO("readback checksum " << result.checksum);
    for ( std::size_t pixel = 0; pixel < 8 * 8; ++pixel )
    {
        INFO("readback pixel " << pixel);
        const std::size_t offset = pixel * 4;
        CHECK((std::abs(static_cast<int>(result.pixels[offset]) -
                        expected[0]) <= 16 &&
               std::abs(static_cast<int>(result.pixels[offset + 1]) -
                        expected[1]) <= 16 &&
               std::abs(static_cast<int>(result.pixels[offset + 2]) -
                        expected[2]) <= 16));
    }
    CHECK(result.checksum != 0);
}

void CheckCanvasMetadata(wxGLCanvas *canvas,
                         int expectedId,
                         const wxString& expectedName)
{
    REQUIRE(canvas != nullptr);
    const HWND hwnd = static_cast<HWND>(canvas->GetHWND());
    const HDC hdc = canvas->GetHDC();
    REQUIRE(hwnd != nullptr);
    REQUIRE(hdc != nullptr);

    CHECK(canvas->GetId() == expectedId);
    CHECK(canvas->GetName() == expectedName);
    CHECK(wxFindWinFromHandle(hwnd) == canvas);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(canvas) == nullptr);
    CHECK_FALSE(wxWinUITLWHostIsInvisibleAccessibilityShell(canvas));
    CHECK((::GetClassLongPtrW(hwnd, GCL_STYLE) & CS_OWNDC) != 0);
    CHECK((::GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_TABSTOP) != 0);
    CHECK(::WindowFromDC(hdc) == hwnd);

    HDC reacquired = ::GetDC(hwnd);
    REQUIRE(reacquired != nullptr);
    CHECK(reacquired == hdc);
    // CS_OWNDC returns the persistent private DC. ReleaseDC() has no useful
    // ownership signal for this class and must not be used as a lifetime
    // oracle; wxGLCanvas releases its single stored acquisition at teardown.

    const int pixelFormat = ::GetPixelFormat(hdc);
    REQUIRE(pixelFormat > 0);
    PIXELFORMATDESCRIPTOR descriptor = {};
    descriptor.nSize = sizeof(descriptor);
    descriptor.nVersion = 1;
    REQUIRE(::DescribePixelFormat(
                hdc, pixelFormat, sizeof(descriptor), &descriptor) != 0);
    CHECK((descriptor.dwFlags & PFD_DRAW_TO_WINDOW) != 0);
    CHECK((descriptor.dwFlags & PFD_SUPPORT_OPENGL) != 0);
    CHECK((descriptor.dwFlags & PFD_DOUBLEBUFFER) != 0);
    CHECK(descriptor.cDepthBits >= 16);

    winrt::com_ptr<IRawElementProviderSimple> nativeProvider;
    REQUIRE(::UiaHostProviderFromHwnd(hwnd, nativeProvider.put()) == S_OK);
    REQUIRE(nativeProvider != nullptr);
    ProviderOptions providerOptions{};
    REQUIRE(nativeProvider->get_ProviderOptions(&providerOptions) == S_OK);
    CHECK((providerOptions & ProviderOptions_ServerSideProvider) != 0);
}

void CheckCurrentGLVersionAtLeast(int requiredMajor, int requiredMinor)
{
    const GLubyte * const rawVersion = ::glGetString(GL_VERSION);
    REQUIRE(rawVersion != nullptr);
    const char * const version =
        reinterpret_cast<const char *>(rawVersion);
    char *minorStart = nullptr;
    const long major = std::strtol(version, &minorStart, 10);
    REQUIRE(minorStart != version);
    REQUIRE(minorStart != nullptr);
    REQUIRE(*minorStart == '.');
    char *versionEnd = nullptr;
    const long minor = std::strtol(minorStart + 1, &versionEnd, 10);
    REQUIRE(versionEnd != minorStart + 1);
    INFO("requested GL >= " << requiredMajor << '.' << requiredMinor
         << ", received " << major << '.' << minor);
    CHECK((major > requiredMajor ||
           (major == requiredMajor && minor >= requiredMinor)));
}

bool InstallWindowRegion(HWND hwnd, HRGN region)
{
    if ( ::SetWindowRgn(hwnd, region, TRUE) )
        return true;

    if ( region )
        ::DeleteObject(region);
    return false;
}

void CheckFocusCoherenceAfterNoActivateCrossTLWReparent(
    wxWindow *window,
    unsigned expectedPendingFocusMigrations)
{
    REQUIRE(window != nullptr);
    const HWND hwnd = static_cast<HWND>(window->GetHWND());
    REQUIRE(hwnd != nullptr);

    const auto check = [window, hwnd, expectedPendingFocusMigrations]()
    {
        CHECK((::GetFocus() == hwnd) ==
              (wxWindow::FindFocus() == window));
        CHECK(wxWinUITopLevelHost::
                  GetPendingFocusMigrationCountForTest() ==
              expectedPendingFocusMigrations);
    };
    check();
    DrainDispatch(4);
    REQUIRE(DrainToQuiescence());
    check();
}

void CheckOutboundTabToXaml(wxWindow *nativeWindow,
                            wxWinUITopLevelHost *host)
{
    REQUIRE(nativeWindow != nullptr);
    REQUIRE(host != nullptr);
    const HWND native = static_cast<HWND>(nativeWindow->GetHWND());
    const HWND tlw = static_cast<HWND>(host->GetTLW()->GetHWND());
    REQUIRE(native != nullptr);
    REQUIRE(tlw != nullptr);

    bool toSlotIsBackward = false;
    wxWindow *slotWindow = nullptr;
    for ( unsigned direction = 0; direction < 2; ++direction )
    {
        const bool backward = direction != 0;
        HWND first = nullptr;
        HWND candidate = native;
        for ( ;; )
        {
            candidate = ::GetNextDlgTabItem(tlw, candidate, backward);
            if ( !candidate || candidate == native || candidate == first )
                break;
            if ( !first )
                first = candidate;

            // Match wxWinUIFindNextKeyboardFocusable(): the raw dialog walk
            // can encounter WinAppSDK infrastructure before either logical
            // neighbour. wxGetWindowFromHWND() would incorrectly promote such
            // a foreign child to its wx TLW parent, so resolve exact wx HWNDs
            // first and admit implementation children only through the wx
            // tree's ContainsHWND() contract.
            wxWindow *window = wxFindWinFromHandle(candidate);
            if ( !window )
            {
                window = host->GetTLW()->FindItemByHWND(
                    reinterpret_cast<WXHWND>(candidate));
            }
            if ( !window )
                continue;
            if ( !window->CanAcceptFocusFromKeyboard() )
                continue;

            // This is the first logical wx focus target in this direction,
            // exactly the one production will choose. If it is native, try
            // the inverse direction instead of skipping a valid tab stop.
            if ( host->FindSlot(window) )
            {
                toSlotIsBackward = backward;
                slotWindow = window;
            }
            break;
        }
        if ( slotWindow )
            break;
    }
    REQUIRE(slotWindow != nullptr);

    ::SetFocus(native);
    REQUIRE(::GetFocus() == native);
    REQUIRE(wxWindow::FindFocus() == nativeWindow);

    MSG toSlot = {};
    toSlot.hwnd = native;
    toSlot.message = WM_KEYDOWN;
    toSlot.wParam = VK_TAB;
    CHECK(wxWinUI3ProcessTabNavigationWithModifiers(
        &toSlot, toSlotIsBackward, false, false));
    REQUIRE(DrainUntil(
        [host, slotWindow]()
        {
            return host->GetFocusOwner() == slotWindow &&
                   wxWindow::FindFocus() == slotWindow;
        }));

    REQUIRE((::GetWindowLongPtrW(native, GWL_STYLE) & WS_TABSTOP) != 0);

    MSG toNative = {};
    toNative.hwnd = ::GetFocus();
    toNative.message = WM_KEYDOWN;
    toNative.wParam = VK_TAB;
    REQUIRE(toNative.hwnd != nullptr);
    CHECK(wxWinUI3ProcessTabNavigationWithModifiers(
        &toNative, !toSlotIsBackward, false, false));
    REQUIRE(DrainUntil(
        [nativeWindow, native]()
        {
            return ::GetFocus() == native &&
                   wxWindow::FindFocus() == nativeWindow;
        }));
}

wxWinUIPointerSample CanvasCentreSample(wxGLCanvas *canvas)
{
    REQUIRE(canvas != nullptr);
    RECT rect = {};
    REQUIRE(::GetWindowRect(static_cast<HWND>(canvas->GetHWND()), &rect));

    wxWinUIPointerSample sample;
    sample.device = wxWinUIInputDevice::Mouse;
    sample.kind = wxWinUIInputKind::Move;
    sample.pointerId = 153;
    sample.isPrimary = true;
    sample.screenX = (rect.left + rect.right) / 2;
    sample.screenY = (rect.top + rect.bottom) / 2;
    sample.timestamp = ::GetTickCount64();
    return sample;
}

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
class GLTextDropTarget final : public wxTextDropTarget
{
public:
    wxDragResult OnEnter(wxCoord x,
                         wxCoord y,
                         wxDragResult def) override
    {
        ++enters;
        lastX = x;
        lastY = y;
        return def;
    }

    wxDragResult OnDragOver(wxCoord x,
                            wxCoord y,
                            wxDragResult def) override
    {
        ++overs;
        lastX = x;
        lastY = y;
        return def;
    }

    bool OnDropText(wxCoord x,
                    wxCoord y,
                    const wxString& value) override
    {
        ++drops;
        lastX = x;
        lastY = y;
        text = value;
        return true;
    }

    unsigned enters = 0;
    unsigned overs = 0;
    unsigned drops = 0;
    int lastX = 0;
    int lastY = 0;
    wxString text;
};

POINTL CanvasCentreInScreen(HWND hwnd)
{
    RECT rect = {};
    REQUIRE(::GetWindowRect(hwnd, &rect));
    return {
        rect.left + (rect.right - rect.left) / 2,
        rect.top + (rect.bottom - rect.top) / 2
    };
}

void CheckTextDropRoute(IDropTarget *comTarget,
                        HWND hwnd,
                        GLTextDropTarget *target,
                        const wxString& payload,
                        unsigned expectedCount)
{
    REQUIRE(comTarget != nullptr);
    REQUIRE(target != nullptr);
    const POINTL point = CanvasCentreInScreen(hwnd);
    wxTextDataObject source(payload);

    DWORD effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(comTarget->DragEnter(
                source.GetInterface(), MK_CONTROL, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(target->enters == expectedCount);

    effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(comTarget->DragOver(MK_SHIFT, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_MOVE);
    CHECK(target->overs == expectedCount);

    effect = DROPEFFECT_COPY;
    REQUIRE(comTarget->Drop(
                source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(target->drops == expectedCount);
    CHECK(target->text == payload);

    POINT expected = { point.x, point.y };
    ::SetLastError(ERROR_SUCCESS);
    const int mapped =
        ::MapWindowPoints(HWND_DESKTOP, hwnd, &expected, 1);
    REQUIRE((mapped != 0 || ::GetLastError() == ERROR_SUCCESS));
    CHECK(target->lastX == expected.x);
    CHECK(target->lastY == expected.y);
}
#endif

void FinishFixture(TwoTLWGLFixture& fixture,
                   const RuntimeSnapshot& before)
{
    fixture.Destroy();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}

} // anonymous namespace

TEST_CASE("WinUIGLCanvas::CreationRenderingMetadataSharingAndBoundedSoak",
          "[winui-glcanvas][winui-015][HostLifecycle][render][sharing]"
          "[uia][soak]")
{
    WGLCurrentRestoreGuard currentRestore;
    const wxGLAttributes displayAttributes = MakeDisplayAttributes();
    if ( !HasTestPixelFormat(displayAttributes) )
    {
        WARN("No double-buffered RGBA WGL pixel format with a 16-bit depth "
             "buffer is available; wxGLCanvas capability gate skipped");
        return;
    }

    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    TwoTLWGLFixture fixture;
    REQUIRE(fixture.Create(displayAttributes));

    // The fixture canvases exercise default construction followed by Create().
    // Keep a direct-construction instance in the same real host as the second
    // public creation contract.
    std::unique_ptr<wxGLCanvas> pendingDirect(new wxGLCanvas(
        fixture.panelA, displayAttributes, 15303,
        wxPoint(24, 150), wxSize(120, 56), wxBORDER_NONE,
        "gl-direct"));
    wxGLCanvas * const direct = pendingDirect.get();
    if ( direct->GetParent() == fixture.panelA )
        (void)pendingDirect.release();
    REQUIRE(direct->GetHWND() != nullptr);

    CheckCanvasMetadata(fixture.canvasA, 15301, "gl-primary-a");
    CheckCanvasMetadata(fixture.canvasB, 15302, "gl-primary-b");
    CheckCanvasMetadata(direct, 15303, "gl-direct");

    std::unique_ptr<wxGLContext> contextA(
        new wxGLContext(fixture.canvasA));
    REQUIRE(contextA->IsOK());
    REQUIRE(contextA->SetCurrent(*fixture.canvasA));
    const HDC contextAEnteringDC = ::wglGetCurrentDC();
    const HGLRC contextAEnteringContext = ::wglGetCurrentContext();
    REQUIRE(contextAEnteringDC == fixture.canvasA->GetHDC());
    REQUIRE(contextAEnteringContext == contextA->GetGLRC());

    std::unique_ptr<wxGLContext> contextB(
        new wxGLContext(fixture.canvasB, contextA.get()));
    REQUIRE(contextB->IsOK());
    CHECK(::wglGetCurrentDC() == contextAEnteringDC);
    CHECK(::wglGetCurrentContext() == contextAEnteringContext);

    CHECK(wxGLCanvas::IsDisplaySupported(displayAttributes));
    CHECK(::wglGetCurrentDC() == contextAEnteringDC);
    CHECK(::wglGetCurrentContext() == contextAEnteringContext);
    REQUIRE(contextA->GetGLRC() != nullptr);
    REQUIRE(contextB->GetGLRC() != nullptr);
    CHECK(contextA->GetGLRC() != contextB->GetGLRC());

    const std::array<GLubyte, 4> blue{{29, 83, 197, 255}};
    const std::array<GLubyte, 4> amber{{211, 137, 31, 255}};
    const RenderResult blueA =
        RenderSolid(*fixture.canvasA, *contextA, blue);
    const RenderResult amberB =
        RenderSolid(*fixture.canvasB, *contextB, amber);
    const RenderResult amberA =
        RenderSolid(*fixture.canvasA, *contextA, amber);
    CheckColour(blueA, blue);
    CheckColour(amberB, amber);
    CheckColour(amberA, amber);
    CHECK(blueA.checksum != amberB.checksum);
    CHECK(amberA.checksum == amberB.checksum);
    REQUIRE(contextA->SetCurrent(*fixture.canvasA));
    REQUIRE(fixture.canvasA->SwapBuffers());
    REQUIRE(contextB->SetCurrent(*fixture.canvasB));
    REQUIRE(fixture.canvasB->SwapBuffers());
    const RenderResult blueAgain =
        RenderSolid(*fixture.canvasA, *contextA, blue);
    CHECK(blueAgain.checksum == blueA.checksum);

    // Prove that the second context was created in the first context's share
    // group using an actual GL object, not just constructor metadata.
    REQUIRE(contextA->SetCurrent(*fixture.canvasA));
    DrainGLErrors();
    GLuint texture = 0;
    ::glGenTextures(1, &texture);
    REQUIRE(texture != 0);
    ::glBindTexture(GL_TEXTURE_2D, texture);
    const std::array<GLubyte, 4> texel{{7, 19, 43, 255}};
    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, texel.data());
    REQUIRE(::glGetError() == GL_NO_ERROR);
    REQUIRE(contextB->SetCurrent(*fixture.canvasB));
    CHECK(::glIsTexture(texture) == GL_TRUE);
    ::glDeleteTextures(1, &texture);
    REQUIRE(::glGetError() == GL_NO_ERROR);

    // Context-attribute construction is capability-gated independently from
    // the default context path. WGL exposes no portable emulation when the
    // ARB context factory is absent.
    REQUIRE(contextA->SetCurrent(*fixture.canvasA));
    if ( wxGLCanvas::IsExtensionSupported(
             "WGL_ARB_create_context_profile") )
    {
        wxGLContextAttrs contextAttributes;
        contextAttributes.PlatformDefaults()
                         .CoreProfile()
                         .OGLVersion(3, 2);
        contextAttributes.EndList();
        std::unique_ptr<wxGLContext> attributed(
            new wxGLContext(direct, nullptr, &contextAttributes));
        REQUIRE(attributed->IsOK());
        REQUIRE(attributed->SetCurrent(*direct));
        REQUIRE(::wglGetCurrentContext() == attributed->GetGLRC());
        REQUIRE(::wglGetCurrentDC() == direct->GetHDC());
        CheckCurrentGLVersionAtLeast(3, 2);
    }
    else if ( wxGLCanvas::IsExtensionSupported("WGL_ARB_create_context") )
    {
        wxGLContextAttrs contextAttributes;
        contextAttributes.PlatformDefaults().OGLVersion(2, 1);
        contextAttributes.EndList();
        std::unique_ptr<wxGLContext> attributed(
            new wxGLContext(direct, nullptr, &contextAttributes));
        REQUIRE(attributed->IsOK());
        REQUIRE(attributed->SetCurrent(*direct));
        REQUIRE(::wglGetCurrentContext() == attributed->GetGLRC());
        REQUIRE(::wglGetCurrentDC() == direct->GetHDC());
        CheckCurrentGLVersionAtLeast(2, 1);
    }
    else
    {
        WARN("WGL_ARB_create_context is unavailable; explicit profile/version "
             "context attributes are recorded as an unavailable capability");
    }

    REQUIRE(contextA->SetCurrent(*fixture.canvasA));
    const GLubyte * const vendor = ::glGetString(GL_VENDOR);
    const GLubyte * const renderer = ::glGetString(GL_RENDERER);
    const GLubyte * const version = ::glGetString(GL_VERSION);
    REQUIRE(vendor != nullptr);
    REQUIRE(renderer != nullptr);
    REQUIRE(version != nullptr);
    INFO("GL vendor: " << reinterpret_cast<const char *>(vendor));
    INFO("GL renderer: " << reinterpret_cast<const char *>(renderer));
    INFO("GL version: " << reinterpret_cast<const char *>(version));

    // A small fixed-frame campaign catches swap/readback instability without
    // depending on DWM presentation or running an unbounded animation loop.
    (void)fixture.canvasA->SetSwapInterval(0);
    FILETIME created = {}, exited = {}, kernelBefore = {}, userBefore = {};
    REQUIRE(::GetProcessTimes(::GetCurrentProcess(), &created, &exited,
                              &kernelBefore, &userBefore));
    const ULONGLONG wallBefore = ::GetTickCount64();
    for ( unsigned frame = 0; frame < 24; ++frame )
    {
        const bool useBlue = (frame % 2) == 0;
        const RenderResult rendered = RenderSolid(
            *fixture.canvasA, *contextA, useBlue ? blue : amber);
        CHECK(rendered.checksum ==
              (useBlue ? blueA.checksum : amberA.checksum));
        REQUIRE(fixture.canvasA->SwapBuffers());
    }
    const ULONGLONG wallElapsed = ::GetTickCount64() - wallBefore;
    FILETIME kernelAfter = {}, userAfter = {};
    REQUIRE(::GetProcessTimes(::GetCurrentProcess(), &created, &exited,
                              &kernelAfter, &userAfter));
    const auto as64 = [](const FILETIME& value)
    {
        ULARGE_INTEGER converted;
        converted.LowPart = value.dwLowDateTime;
        converted.HighPart = value.dwHighDateTime;
        return converted.QuadPart;
    };
    const ULONGLONG cpu100ns =
        as64(kernelAfter) - as64(kernelBefore) +
        as64(userAfter) - as64(userBefore);
    INFO("24-frame WGL soak wall ms: " << wallElapsed);
    INFO("24-frame WGL soak process CPU ms: " << cpu100ns / 10000);
    CHECK(wallElapsed < 30000);
    CHECK(cpu100ns < UINT64_C(30000) * 10000);

    wxGLContext::ClearCurrent();
    contextB.reset();
    contextA.reset();
    FinishFixture(fixture, before);
}

TEST_CASE("WinUIGLCanvas::CompositionInputDropAndStableCrossTLWReparent",
          "[winui-glcanvas][winui-015][HostLifecycle][z-order][focus]"
          "[capture][cursor][reparent][winui-drop-broker]")
{
    WGLCurrentRestoreGuard currentRestore;
    const wxGLAttributes displayAttributes = MakeDisplayAttributes();
    if ( !HasTestPixelFormat(displayAttributes) )
    {
        WARN("Required WGL pixel format unavailable; composition gate skipped");
        return;
    }

    NativeFocusRestoreGuard focusRestore;
    NativeCursorRestoreGuard cursorRestore;
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    TwoTLWGLFixture fixture;
    REQUIRE(fixture.Create(displayAttributes));

    ProbeGLCanvas * const canvas = fixture.canvasA;
    const HWND hwnd = static_cast<HWND>(canvas->GetHWND());
    const HDC hdc = canvas->GetHDC();
    const int pixelFormat = ::GetPixelFormat(hdc);
    const unsigned long long hwndGeneration =
        wxWinUIMSWGetHwndGeneration(
            canvas, reinterpret_cast<WXHWND>(hwnd));
    REQUIRE(hwnd != nullptr);
    REQUIRE(hdc != nullptr);
    REQUIRE(pixelFormat > 0);
    REQUIRE(hwndGeneration != 0);

    std::unique_ptr<wxGLContext> context(new wxGLContext(canvas));
    REQUIRE(context->IsOK());
    const HGLRC glrc = context->GetGLRC();
    REQUIRE(glrc != nullptr);
    const std::array<GLubyte, 4> green{{37, 181, 73, 255}};
    const RenderResult baseline = RenderSolid(*canvas, *context, green);
    CheckColour(baseline, green);

    canvas->SetSize(wxRect(24, 24, 224, 112));
    REQUIRE(context->SetCurrent(*canvas));
    ::glViewport(0, 0, 224, 112);
    GLint viewport[4] = {};
    ::glGetIntegerv(GL_VIEWPORT, viewport);
    CHECK(viewport[0] == 0);
    CHECK(viewport[1] == 0);
    CHECK(viewport[2] == 224);
    CHECK(viewport[3] == 112);
    CHECK(RenderSolid(*canvas, *context, green).checksum ==
          baseline.checksum);

    canvas->Freeze();
    CHECK(canvas->IsFrozen());
    canvas->SetSize(wxRect(28, 26, 196, 96));
    canvas->Thaw();
    CHECK_FALSE(canvas->IsFrozen());
    CHECK(RenderSolid(*canvas, *context, green).checksum ==
          baseline.checksum);
    REQUIRE(canvas->Hide());
    CHECK_FALSE(::IsWindowVisible(hwnd));
    REQUIRE(canvas->Show());
    CHECK(::IsWindowVisible(hwnd));
    CHECK(RenderSolid(*canvas, *context, green).checksum ==
          baseline.checksum);

    // Drive the exact production root router with an already-normalized
    // sample. This is logical input only: no SendInput and no physical cursor
    // movement. The route must resolve the native WGL HWND behind the island.
    const wxCursor crossCursor(wxCURSOR_CROSS);
    REQUIRE(canvas->SetCursor(crossCursor));
    const unsigned movesBefore = canvas->GetMouseMoveCount();
    const unsigned cursorsBefore = canvas->GetSetCursorCount();
    wxWinUIPointerSample sample = CanvasCentreSample(canvas);
    const wxWinUIRootPointerOutcome pointer =
        fixture.hostA->TestRouteRootPointerSample(sample);
    CHECK(pointer.status == wxWinUIRootPointerStatus::Completed);
    CHECK(pointer.disposition == wxWinUIInputDisposition::Routed);
    CHECK(pointer.cancelSource);
    CHECK(canvas->GetMouseMoveCount() == movesBefore + 1);
    CHECK(canvas->GetSetCursorCount() >= cursorsBefore + 1);
    CHECK(::GetCursor() ==
          reinterpret_cast<HCURSOR>(crossCursor.GetHCURSOR()));

    ::SetFocus(hwnd);
    REQUIRE(::GetFocus() == hwnd);
    REQUIRE(wxWindow::FindFocus() == canvas);
    REQUIRE(wxWindow::GetCapture() == before.wxCapture);
    REQUIRE(::GetCapture() == before.nativeCapture);
    canvas->CaptureMouse();
    CHECK(wxWindow::GetCapture() == canvas);
    CHECK(::GetCapture() == hwnd);
    canvas->ReleaseMouse();
    CHECK(wxWindow::GetCapture() == before.wxCapture);
    CHECK(::GetCapture() == before.nativeCapture);
    CheckOutboundTabToXaml(canvas, fixture.hostA);

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    GLTextDropTarget * const target = new GLTextDropTarget;
    canvas->SetDropTarget(target);
    wxMSWOleDropTargetBinding bindingA;
    REQUIRE(wxMSWOleLookupDropTarget(canvas, &bindingA) ==
            wxMSWOleDropTargetLookup::Found);
    REQUIRE(bindingA.IsCurrent());
    CHECK(bindingA.GetOwnerHwndIfCurrent() == hwnd);
    CHECK(wxWinUITLWHostOwnsOleDropRegistration(canvas));
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(target, hwnd));
    wxWinUIDropBroker * const brokerA =
        fixture.hostA->GetDropBrokerForTest();
    REQUIRE(brokerA != nullptr);
    winrt::com_ptr<IDropTarget> comA;
    comA.copy_from(brokerA->GetCOMTargetForTest());
    REQUIRE(comA != nullptr);
    CheckTextDropRoute(comA.get(), hwnd, target, "gl-host-a", 1);
#endif

    // Adjacent and XAML-above-native overlap are representable.
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    REQUIRE(::SetWindowPos(
        hwnd, HWND_TOP, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(canvas, true);
    fixture.hostA->FlushSync();
    REQUIRE(DrainToQuiescence());
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    fixture.buttonA->SetSize(wxRect(48, 34, 132, 48));
    REQUIRE(::SetWindowPos(
        hwnd, static_cast<HWND>(fixture.buttonA->GetHWND()),
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(canvas, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    REQUIRE(DrainToQuiescence());
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    // A native GL surface requested above overlapping XAML cannot overtake
    // the one shared topmost bridge. The production classifier must publish
    // this limitation instead of silently treating it as S0 support.
    REQUIRE(::SetWindowPos(
        hwnd, HWND_TOP, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(canvas, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    REQUIRE(DrainToQuiescence());
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() >= 1);

    // Ancestor viewport clipping and an explicit native region both retire
    // the overlap. Neither assertion attempts to infer DWM presentation.
    canvas->SetSize(wxRect(-240, 26, 196, 96));
    wxWinUITLWHostNotifyNativeLayout(canvas, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    REQUIRE(DrainToQuiescence());
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    canvas->SetSize(wxRect(28, 26, 196, 96));
    REQUIRE(::SetWindowPos(
        hwnd, HWND_TOP, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(canvas, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    REQUIRE(DrainToQuiescence());
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() >= 1);
    HRGN emptyClip = ::CreateRectRgn(0, 0, 0, 0);
    REQUIRE(emptyClip != nullptr);
    REQUIRE(InstallWindowRegion(hwnd, emptyClip));
    wxWinUITLWHostNotifyNativeLayout(canvas, false);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    REQUIRE(DrainToQuiescence());
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);
    REQUIRE(InstallWindowRegion(hwnd, nullptr));

    // Restore a normal adjacent geometry and an explicitly representable
    // sibling order before migration. No later input/reparent oracle inherits
    // the deliberately impossible Z setup above.
    fixture.buttonA->SetSize(wxRect(250, 24, 130, 48));
    REQUIRE(::SetWindowPos(
        hwnd, static_cast<HWND>(fixture.buttonA->GetHWND()),
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(canvas, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    REQUIRE(DrainToQuiescence());
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    // Cross-TLW reparent must preserve all WGL identities even if USER32
    // legitimately releases focus when entering the inactive tool window.
    ::SetFocus(hwnd);
    REQUIRE(::GetFocus() == hwnd);
    REQUIRE(wxWindow::FindFocus() == canvas);
    REQUIRE(canvas->Reparent(fixture.panelB));
    CHECK(canvas->GetParent() == fixture.panelB);
    CHECK(::GetParent(hwnd) == GetHwndOf(fixture.panelB));
    CHECK(canvas->GetHWND() == hwnd);
    CHECK(canvas->GetHDC() == hdc);
    CHECK(::GetPixelFormat(hdc) == pixelFormat);
    CHECK(context->GetGLRC() == glrc);
    CHECK(wxWinUIMSWGetHwndGeneration(
              canvas, reinterpret_cast<WXHWND>(hwnd)) == hwndGeneration);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(canvas) == nullptr);
    CheckFocusCoherenceAfterNoActivateCrossTLWReparent(
        canvas, before.pendingFocusMigrations);
    const unsigned movesBeforeHostMigration = canvas->GetMouseMoveCount();
    sample = CanvasCentreSample(canvas);
    const wxWinUIRootPointerOutcome staleHostRoute =
        fixture.hostA->TestRouteRootPointerSample(sample);
    CHECK(staleHostRoute.disposition == wxWinUIInputDisposition::Outside);
    CHECK(canvas->GetMouseMoveCount() == movesBeforeHostMigration);
    ++sample.timestamp;
    const wxWinUIRootPointerOutcome destinationHostRoute =
        fixture.hostB->TestRouteRootPointerSample(sample);
    CHECK(destinationHostRoute.status == wxWinUIRootPointerStatus::Completed);
    CHECK(destinationHostRoute.disposition ==
          wxWinUIInputDisposition::Routed);
    CHECK(destinationHostRoute.cancelSource);
    CHECK(canvas->GetMouseMoveCount() == movesBeforeHostMigration + 1);
    CHECK(RenderSolid(*canvas, *context, green).checksum ==
          baseline.checksum);
    REQUIRE(canvas->SwapBuffers());
    CheckOutboundTabToXaml(canvas, fixture.hostB);

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    wxMSWOleDropTargetBinding bindingB;
    REQUIRE(wxMSWOleLookupDropTarget(canvas, &bindingB) ==
            wxMSWOleDropTargetLookup::Found);
    REQUIRE(bindingB.IsCurrent());
    CHECK(bindingB.GetOwnerHwndIfCurrent() == hwnd);
    CHECK(bindingB.IsSameBinding(bindingA));
    CHECK(wxWinUITLWHostOwnsOleDropRegistration(canvas));

    fixture.hostB->FlushSync();
    const POINTL pointB = CanvasCentreInScreen(hwnd);
    wxTextDataObject movedSource("gl-host-b");
    DWORD oldEffect = DROPEFFECT_COPY;
    REQUIRE(comA->DragEnter(
                movedSource.GetInterface(), 0, pointB, &oldEffect) == S_OK);
    CHECK(oldEffect == DROPEFFECT_NONE);
    REQUIRE(comA->DragLeave() == S_OK);
    CHECK(target->enters == 1);
    CHECK(target->drops == 1);

    wxWinUIDropBroker * const brokerB =
        fixture.hostB->GetDropBrokerForTest();
    REQUIRE(brokerB != nullptr);
    winrt::com_ptr<IDropTarget> comB;
    comB.copy_from(brokerB->GetCOMTargetForTest());
    REQUIRE(comB != nullptr);
    CheckTextDropRoute(comB.get(), hwnd, target, "gl-host-b", 2);
#endif

    ::SetFocus(hwnd);
    REQUIRE(::GetFocus() == hwnd);
    REQUIRE(wxWindow::FindFocus() == canvas);
    REQUIRE(canvas->Reparent(fixture.panelA));
    CheckFocusCoherenceAfterNoActivateCrossTLWReparent(
        canvas, before.pendingFocusMigrations);
    CHECK(canvas->GetHDC() == hdc);
    CHECK(::GetPixelFormat(hdc) == pixelFormat);
    CHECK(context->GetGLRC() == glrc);
    const unsigned movesBeforeReturnMigration = canvas->GetMouseMoveCount();
    sample = CanvasCentreSample(canvas);
    const wxWinUIRootPointerOutcome oldDestinationRoute =
        fixture.hostB->TestRouteRootPointerSample(sample);
    CHECK(oldDestinationRoute.disposition ==
          wxWinUIInputDisposition::Outside);
    CHECK(canvas->GetMouseMoveCount() == movesBeforeReturnMigration);
    ++sample.timestamp;
    const wxWinUIRootPointerOutcome sourceHostRoute =
        fixture.hostA->TestRouteRootPointerSample(sample);
    CHECK(sourceHostRoute.status == wxWinUIRootPointerStatus::Completed);
    CHECK(sourceHostRoute.disposition == wxWinUIInputDisposition::Routed);
    CHECK(sourceHostRoute.cancelSource);
    CHECK(canvas->GetMouseMoveCount() == movesBeforeReturnMigration + 1);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    wxMSWOleDropTargetBinding bindingReturnA;
    REQUIRE(wxMSWOleLookupDropTarget(canvas, &bindingReturnA) ==
            wxMSWOleDropTargetLookup::Found);
    REQUIRE(bindingReturnA.IsCurrent());
    CHECK(bindingReturnA.GetOwnerHwndIfCurrent() == hwnd);
    CHECK(bindingReturnA.IsSameBinding(bindingB));
    CHECK(wxWinUITLWHostOwnsOleDropRegistration(canvas));

    const POINTL pointA = CanvasCentreInScreen(hwnd);
    wxTextDataObject returnedSource("gl-host-a-return");
    DWORD retiredEffect = DROPEFFECT_COPY;
    REQUIRE(comB->DragEnter(
                returnedSource.GetInterface(), 0,
                pointA, &retiredEffect) == S_OK);
    CHECK(retiredEffect == DROPEFFECT_NONE);
    REQUIRE(comB->DragLeave() == S_OK);
    CHECK(target->enters == 2);
    CHECK(target->drops == 2);
    CheckTextDropRoute(
        comA.get(), hwnd, target, "gl-host-a-return", 3);
#endif
    CHECK(RenderSolid(*canvas, *context, green).checksum ==
          baseline.checksum);
    REQUIRE(canvas->SwapBuffers());
    wxWinUITLWHostNotifyNativeLayout(canvas, true);
    fixture.hostA->FlushSync();
    REQUIRE(DrainToQuiescence());
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    comB = nullptr;
    comA = nullptr;
#endif
    wxGLContext::ClearCurrent();
    context.reset();
    FinishFixture(fixture, before);
}

TEST_CASE("WinUIGLCanvas::HundredCurrentAndNonCurrentLifetimeCycles",
          "[winui-glcanvas][winui-015][HostLifecycle][stress][destroy]"
          "[reparent]")
{
    WGLCurrentRestoreGuard currentRestore;
    const wxGLAttributes displayAttributes = MakeDisplayAttributes();
    if ( !HasTestPixelFormat(displayAttributes) )
    {
        WARN("Required WGL pixel format unavailable; lifetime gate skipped");
        return;
    }

    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    TwoTLWGLFixture fixture;
    REQUIRE(fixture.Create(displayAttributes));
    REQUIRE(DrainToQuiescence());

    const unsigned steadyHosts = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned steadyHostLifetimes =
        wxWinUITopLevelHost::GetLiveHostLifetimeCount();
    const unsigned steadySlots = wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned steadySlotLifetimes =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned steadySubclasses =
        wxWinUITopLevelHost::GetLiveSubclassContextCount();
    const unsigned steadyFocusMigrations =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    const wxWinUITransientRegistrySnapshot steadyTransients =
        wxWinUIGetTransientRegistrySnapshotForTesting();

    unsigned destroyedWhileCurrent = 0;
    unsigned destroyedWhileNonCurrent = 0;
    const std::array<GLubyte, 4> colour{{113, 47, 179, 255}};

    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        INFO("wxGLCanvas lifetime cycle " << cycle);
        wxPanel * const source = cycle % 2 ? fixture.panelB : fixture.panelA;
        wxPanel * const destination =
            cycle % 2 ? fixture.panelA : fixture.panelB;

        std::unique_ptr<ProbeGLCanvas> canvas(new ProbeGLCanvas);
        REQUIRE(canvas->CreateProbe(
            source, displayAttributes, 15400 + static_cast<int>(cycle),
            wxPoint(12 + static_cast<int>(cycle % 5), 146),
            wxSize(72, 48), wxString::Format("gl-cycle-%u", cycle)));
        const HWND hwnd = static_cast<HWND>(canvas->GetHWND());
        const HDC hdc = canvas->GetHDC();
        const int pixelFormat = ::GetPixelFormat(hdc);
        REQUIRE(hwnd != nullptr);
        REQUIRE(hdc != nullptr);
        REQUIRE(pixelFormat > 0);

        std::unique_ptr<ProbeGLContext> context(
            new ProbeGLContext(canvas.get()));
        REQUIRE(context->IsOK());
        const HGLRC glrc = context->GetGLRC();
        REQUIRE(glrc != nullptr);
        CheckColour(RenderSolid(*canvas, *context, colour), colour);
        REQUIRE(canvas->SwapBuffers());

        REQUIRE(canvas->Reparent(destination));
        CHECK(canvas->GetHWND() == hwnd);
        CHECK(canvas->GetHDC() == hdc);
        CHECK(::GetPixelFormat(hdc) == pixelFormat);
        CHECK(context->GetGLRC() == glrc);
        CHECK(RenderSolid(*canvas, *context, colour).checksum != 0);
        REQUIRE(canvas->SwapBuffers());

        if ( cycle % 2 == 0 )
        {
            REQUIRE(::wglGetCurrentContext() == glrc);
            REQUIRE(::wglGetCurrentDC() == hdc);
            const bool deleteResult = context->DeleteForTest();
            const HGLRC currentAfterDelete = ::wglGetCurrentContext();
            const HDC dcAfterDelete = ::wglGetCurrentDC();
            bool retryDeleteResult = true;

            // Even if an implementation unexpectedly refuses to delete its
            // current RC, make the failure path terminal before releasing the
            // canvas/HDC. The captured values remain the actual API verdict.
            if ( !deleteResult || currentAfterDelete || dcAfterDelete )
            {
                wxGLContext::ClearCurrent();
                if ( !deleteResult )
                    retryDeleteResult = context->DeleteForTest();
            }

            CHECK(deleteResult);
            REQUIRE(retryDeleteResult);
            CHECK(currentAfterDelete == nullptr);
            CHECK(dcAfterDelete == nullptr);
            CHECK(::wglGetCurrentContext() == nullptr);
            CHECK(::wglGetCurrentDC() == nullptr);
            context.reset();
            ++destroyedWhileCurrent;
        }
        else
        {
            wxGLContext::ClearCurrent();
            REQUIRE(::wglGetCurrentContext() == nullptr);
            REQUIRE(::wglGetCurrentDC() == nullptr);
            const bool deleteResult = context->DeleteForTest();
            bool retryDeleteResult = true;
            if ( !deleteResult )
            {
                wxGLContext::ClearCurrent();
                retryDeleteResult = context->DeleteForTest();
            }
            CHECK(deleteResult);
            REQUIRE(retryDeleteResult);
            context.reset();
            CHECK(::wglGetCurrentContext() == nullptr);
            CHECK(::wglGetCurrentDC() == nullptr);
            ++destroyedWhileNonCurrent;
        }

        canvas.reset();
        CHECK_FALSE(::IsWindow(hwnd));
        // WindowFromDC() is the sole post-destruction observation of the
        // former private-DC value. Do not pass this invalidated HDC to any GL,
        // pixel-format, ReleaseDC or object-introspection API afterwards.
        CHECK(::WindowFromDC(hdc) == nullptr);
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == steadyHosts);
        CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
              steadyHostLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
              steadySlotLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              steadySubclasses);
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              steadyFocusMigrations);
        CHECK(SameTransientRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            steadyTransients));
        CHECK(wxWindow::GetCapture() == before.wxCapture);
        CHECK(::GetCapture() == before.nativeCapture);

        if ( cycle % 10 == 9 )
            REQUIRE(DrainToQuiescence());
    }

    CHECK(destroyedWhileCurrent == 50);
    CHECK(destroyedWhileNonCurrent == 50);
    FinishFixture(fixture, before);
}

#endif // wxUSE_BUTTON && wxUSE_TEXTCTRL
#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_GLCANVAS
