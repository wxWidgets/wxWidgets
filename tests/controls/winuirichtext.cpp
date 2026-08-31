///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuirichtext.cpp
// Purpose:     deterministic WinUI wxRichTextCtrl qualification
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && \
        wxUSE_RICHTEXT && wxUSE_BUTTON

#ifndef WX_PRECOMP
    #include "wx/accel.h"
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/image.h"
    #include "wx/log.h"
    #include "wx/panel.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
    #include "wx/clipbrd.h"
    #include "wx/dataobj.h"
#endif
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx/msw/wrapwin.h"
#include "wx/private/windowlifetime.h"
#include "wx/richtext/richtextbuffer.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextformatdlg.h"
#include "wx/richtext/richtextimagedlg.h"
#include "wx/richtext/richtextstyledlg.h"
#include "wx/richtext/richtextstyles.h"
#include "wx/scopeguard.h"
#if wxUSE_HTML
    #include "wx/richtext/richtexthtml.h"
#endif
#if wxUSE_XML
    #include "wx/richtext/richtextxml.h"
#endif
#include "wx/sstream.h"
#include "wx/toplevel.h"
#include "wx/weakref.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/inputtest.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/transient.h"
#include "wx/winui/winui.h"

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    #include "wx/msw/private/dropsession.h"
    #include "wx/winui/private/dropbroker.h"
#endif

#if wxUSE_XRC && wxUSE_XML
    #include "wx/xml/xml.h"
    #include "wx/xrc/xh_richtext.h"
    #include "wx/xrc/xmlres.h"
#endif

#include <shellapi.h>
#include <shlobj_core.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>

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
bool DrainUntil(Predicate predicate, unsigned rounds = 150)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainDispatch(1);
    }

    return predicate();
}

// wxTopLevelWindow::Destroy() normally queues the object in wxPendingDelete.
// These tests themselves run from an idle callback, so a nested wxYield()
// cannot be relied on to enter another idle pass and prune that list. Use this
// only for pending-delete assertions and their RAII cleanup.
template <typename Predicate>
bool DrainPendingDeleteUntil(Predicate predicate, unsigned rounds = 40)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;

        wxTheApp->ProcessPendingEvents();
        wxTheApp->ProcessIdle();
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

    void CheckRestored(bool requireHandlerTraffic = true) const
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
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              pendingFocusMigrations);

        const unsigned slotAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotHandlerAdds;
        const unsigned slotRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                slotHandlerRevokes;
        if ( requireHandlerTraffic )
            CHECK(slotAdds > 0);
        CHECK(slotAdds == slotRevokes);

        const unsigned rootAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount() - rootHandlerAdds;
        const unsigned rootRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                rootHandlerRevokes;
        if ( requireHandlerTraffic )
            CHECK(rootAdds > 0);
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

bool ShowOffscreenWithoutActivation(wxTopLevelWindow *window, int ordinal)
{
    const HWND hwnd = static_cast<HWND>(window->GetHWND());
    if ( !hwnd )
        return false;

    ::SetWindowLongPtr(
        hwnd,
        GWL_EXSTYLE,
        ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    window->Move(wxPoint(-30000 + ordinal * 96, -30000));
    window->ShowWithoutActivating();
    return true;
}

class TopLevelDestroyGuard final
{
public:
    explicit TopLevelDestroyGuard(wxTopLevelWindow *window)
        : m_window(window)
    {
    }

    ~TopLevelDestroyGuard()
    {
        if ( wxTopLevelWindow * const window = m_window.get() )
        {
            if ( !window->IsBeingDeleted() )
                window->Destroy();
        }
        DrainPendingDeleteUntil([this]() { return !m_window; }, 80);
    }

private:
    wxWeakRef<wxTopLevelWindow> m_window;
    wxDECLARE_NO_COPY_CLASS(TopLevelDestroyGuard);
};

struct ProbeState
{
    bool destroyed = false;
    unsigned timerCallbacks = 0;
    unsigned lateCallbacks = 0;
};

class ProbeRichTextCtrl final : public wxRichTextCtrl
{
public:
    ProbeRichTextCtrl()
    {
        ++ms_liveCount;
    }

    explicit ProbeRichTextCtrl(wxWindow *parent)
        : wxRichTextCtrl(parent, wxID_ANY, wxEmptyString,
                         wxPoint(20, 70), wxSize(280, 190),
                         wxRE_MULTILINE | wxBORDER_SIMPLE)
    {
        ++ms_liveCount;
    }

    ~ProbeRichTextCtrl() override
    {
        if ( m_probe )
            m_probe->destroyed = true;
        --ms_liveCount;
    }

    void ObserveTimers(const std::shared_ptr<ProbeState>& probe)
    {
        m_probe = probe;
        Bind(
            wxEVT_TIMER,
            [probe](wxTimerEvent& event)
            {
                ++probe->timerCallbacks;
                if ( probe->destroyed )
                    ++probe->lateCallbacks;
                event.Skip();
            });
    }

    static unsigned GetLiveCount() { return ms_liveCount; }

private:
    std::shared_ptr<ProbeState> m_probe;
    static unsigned ms_liveCount;
};

unsigned ProbeRichTextCtrl::ms_liveCount = 0;

class RichTextFixture final
{
public:
    ~RichTextFixture()
    {
        Cleanup();
    }

    bool Create(bool twoStepRich = false, int ordinal = 0)
    {
        wxFrame * const frameA = new wxFrame(
            nullptr, wxID_ANY, "WinUI RichText fixture A",
            wxDefaultPosition, wxSize(520, 340),
            wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                wxFRAME_TOOL_WINDOW);
        wxFrame * const frameB = new wxFrame(
            nullptr, wxID_ANY, "WinUI RichText fixture B",
            wxDefaultPosition, wxSize(520, 340),
            wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                wxFRAME_TOOL_WINDOW);
        m_frameA = wxWeakRef<wxFrame>(frameA);
        m_frameB = wxWeakRef<wxFrame>(frameB);

        wxPanel * const panelA = new wxPanel(
            frameA, wxID_ANY, wxPoint(0, 0), wxSize(500, 310));
        wxPanel * const panelB = new wxPanel(
            frameB, wxID_ANY, wxPoint(0, 0), wxSize(500, 310));
        m_panelA = wxWeakRef<wxPanel>(panelA);
        m_panelB = wxWeakRef<wxPanel>(panelB);

        // Create RichText before the first XAML slot. This exercises migration
        // of its logical drop target to the shared host registration.
        ProbeRichTextCtrl *rich = nullptr;
        if ( twoStepRich )
        {
            rich = new ProbeRichTextCtrl;
            if ( !rich->Create(panelA, wxID_ANY, wxEmptyString,
                               wxPoint(20, 70), wxSize(280, 190),
                               wxRE_MULTILINE | wxBORDER_SIMPLE) )
            {
                delete rich;
                return false;
            }
        }
        else
        {
            rich = new ProbeRichTextCtrl(panelA);
        }
        if ( !rich->GetHWND() )
        {
            delete rich;
            return false;
        }
        m_rich = wxWeakRef<ProbeRichTextCtrl>(rich);

        wxButton * const sentinelA = new wxButton(
            panelA, wxID_ANY, "XAML neighbour A",
            wxPoint(330, 24), wxSize(145, 38));
        wxButton * const sentinelB = new wxButton(
            panelB, wxID_ANY, "XAML neighbour B",
            wxPoint(330, 24), wxSize(145, 38));
        m_sentinelA = wxWeakRef<wxButton>(sentinelA);
        m_sentinelB = wxWeakRef<wxButton>(sentinelB);

        if ( !ShowOffscreenWithoutActivation(frameA, ordinal) ||
             !ShowOffscreenWithoutActivation(frameB, ordinal + 1) )
        {
            return false;
        }
        if ( !DrainToQuiescence() )
            return false;

        m_hostA = wxWinUITopLevelHost::FindSlotOwner(sentinelA);
        m_hostB = wxWinUITopLevelHost::FindSlotOwner(sentinelB);
        return m_hostA && m_hostB && m_hostA != m_hostB &&
               m_hostA->FindSlot(rich) == nullptr &&
               m_hostB->FindSlot(rich) == nullptr &&
               wxWinUITopLevelHost::FindSlotOwner(rich) == nullptr;
    }

    void ForgetRich()
    {
        m_rich = wxWeakRef<ProbeRichTextCtrl>();
    }

    void Cleanup()
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;

        if ( wxFrame * const frame = GetFrameA() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
        if ( wxFrame * const frame = GetFrameB() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
        DrainUntil(
            [this]()
            {
                return !m_frameA && !m_frameB && !m_rich;
            },
            240);
        DrainDispatch(8);
    }

    wxFrame *GetFrameA() const { return m_frameA.get(); }
    wxFrame *GetFrameB() const { return m_frameB.get(); }
    wxPanel *GetPanelA() const { return m_panelA.get(); }
    wxPanel *GetPanelB() const { return m_panelB.get(); }
    wxButton *GetSentinelA() const { return m_sentinelA.get(); }
    wxButton *GetSentinelB() const { return m_sentinelB.get(); }
    ProbeRichTextCtrl *GetRich() const { return m_rich.get(); }
    wxWinUITopLevelHost *GetHostA() const { return m_hostA; }
    wxWinUITopLevelHost *GetHostB() const { return m_hostB; }

private:
    bool m_cleaned = false;
    wxWeakRef<wxFrame> m_frameA;
    wxWeakRef<wxFrame> m_frameB;
    wxWeakRef<wxPanel> m_panelA;
    wxWeakRef<wxPanel> m_panelB;
    wxWeakRef<wxButton> m_sentinelA;
    wxWeakRef<wxButton> m_sentinelB;
    wxWeakRef<ProbeRichTextCtrl> m_rich;
    wxWinUITopLevelHost *m_hostA = nullptr;
    wxWinUITopLevelHost *m_hostB = nullptr;
};

struct XamlStageTrace
{
    unsigned calls = 0;
    bool handles = false;
};

bool TraceXamlStage(WXMSG *, void *context)
{
    XamlStageTrace& trace = *static_cast<XamlStageTrace *>(context);
    ++trace.calls;
    return trace.handles;
}

wxWinUIKeyboardModifiers Modifiers(bool shift = false,
                                   bool control = false,
                                   bool leftAlt = false,
                                   bool rightAlt = false)
{
    wxWinUIKeyboardModifiers modifiers = {};
    modifiers.shiftDown = shift;
    modifiers.controlDown = control;
    modifiers.leftAltDown = leftAlt;
    modifiers.rightAltDown = rightAlt;
    return modifiers;
}

MSG KeyMessage(HWND hwnd, UINT message, WPARAM key)
{
    MSG msg = {};
    msg.hwnd = hwnd;
    msg.message = message;
    msg.wParam = key;
    return msg;
}

bool DeliverCharacter(wxRichTextCtrl *rich,
                      int key,
                      bool shift = false,
                      bool control = false,
                      bool alt = false)
{
    wxKeyEvent event(wxEVT_CHAR);
    event.SetEventObject(rich);
    event.SetId(rich->GetId());
    event.m_keyCode = key;
    event.m_uniChar = key;
    event.m_shiftDown = shift;
    event.m_controlDown = control;
    event.m_altDown = alt;
    return rich->GetEventHandler()->ProcessEvent(event);
}

#if wxUSE_XML
class XMLHandlerScope final
{
public:
    XMLHandlerScope()
    {
        if ( !wxRichTextBuffer::FindHandler(wxRICHTEXT_TYPE_XML) )
        {
            wxRichTextBuffer::AddHandler(
                new wxRichTextXMLHandler(Name(), "xml",
                                         wxRICHTEXT_TYPE_XML));
            m_owned = true;
        }
    }

    ~XMLHandlerScope()
    {
        if ( m_owned )
            wxRichTextBuffer::RemoveHandler(Name());
    }

private:
    static wxString Name() { return "WinUI RichText 014 XML"; }
    bool m_owned = false;
};
#endif

#if wxUSE_HTML
class HTMLHandlerScope final
{
public:
    HTMLHandlerScope()
    {
        if ( !wxRichTextBuffer::FindHandler(wxRICHTEXT_TYPE_HTML) )
        {
            wxRichTextBuffer::AddHandler(
                new wxRichTextHTMLHandler(Name(), "html",
                                          wxRICHTEXT_TYPE_HTML));
            m_owned = true;
        }
    }

    ~HTMLHandlerScope()
    {
        if ( m_owned )
            wxRichTextBuffer::RemoveHandler(Name());
    }

private:
    static wxString Name() { return "WinUI RichText 014 HTML"; }
    bool m_owned = false;
};
#endif

#if wxUSE_FFILE && wxUSE_STREAMS
class TemporaryTextFile final
{
public:
    explicit TemporaryTextFile(const wxString& contents)
    {
        const wxString temporary =
            wxFileName::CreateTempFileName("wx-winui-rich-014-");
        if ( temporary.empty() )
            return;

        m_path = temporary + ".txt";
        if ( !wxRenameFile(temporary, m_path) )
        {
            m_path.clear();
            wxRemoveFile(temporary);
            return;
        }

        wxFFile file(m_path, "wb");
        if ( !file.IsOpened() || !file.Write(contents, wxConvUTF8) )
        {
            file.Close();
            wxRemoveFile(m_path);
            m_path.clear();
        }
    }

    ~TemporaryTextFile()
    {
        if ( !m_path.empty() && wxFileExists(m_path) )
        {
            wxLogNull noLog;
            wxRemoveFile(m_path);
        }
    }

    bool IsOk() const { return !m_path.empty(); }
    const wxString& GetPath() const { return m_path; }

private:
    wxString m_path;
};

HDROP MakeDropFiles(const POINT& bridgeClient, const wxString& path)
{
    const size_t chars = path.length() + 2;
    const size_t bytes = sizeof(DROPFILES) + chars * sizeof(wchar_t);
    HGLOBAL global = ::GlobalAlloc(GHND, bytes);
    if ( !global )
        return nullptr;

    DROPFILES * const drop =
        static_cast<DROPFILES *>(::GlobalLock(global));
    if ( !drop )
    {
        ::GlobalFree(global);
        return nullptr;
    }

    drop->pFiles = sizeof(DROPFILES);
    drop->pt = bridgeClient;
    drop->fNC = FALSE;
    drop->fWide = TRUE;
    wchar_t * const names =
        reinterpret_cast<wchar_t *>(
            reinterpret_cast<unsigned char *>(drop) + sizeof(DROPFILES));
    std::memcpy(names, path.wc_str(), path.length() * sizeof(wchar_t));
    names[path.length()] = L'\0';
    names[path.length() + 1] = L'\0';
    ::GlobalUnlock(global);
    return reinterpret_cast<HDROP>(global);
}

class DropFilesHandle final
{
public:
    explicit DropFilesHandle(HDROP handle) : m_handle(handle) {}

    ~DropFilesHandle()
    {
        if ( m_handle )
            ::DragFinish(m_handle);
    }

    HDROP Get() const { return m_handle; }

    HDROP Release()
    {
        HDROP const handle = m_handle;
        m_handle = nullptr;
        return handle;
    }

private:
    HDROP m_handle = nullptr;
    wxDECLARE_NO_COPY_CLASS(DropFilesHandle);
};
#endif // wxUSE_FFILE && wxUSE_STREAMS

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
class DropTargetRef final
{
public:
    explicit DropTargetRef(IDropTarget *target) : m_target(target)
    {
        if ( m_target )
            m_target->AddRef();
    }

    ~DropTargetRef()
    {
        if ( m_target )
            m_target->Release();
    }

    explicit operator bool() const { return m_target != nullptr; }
    IDropTarget *operator->() const { return m_target; }

private:
    IDropTarget *m_target = nullptr;
    wxDECLARE_NO_COPY_CLASS(DropTargetRef);
};
#endif

// TranslateAccelerator() consults the thread keyboard state itself. The
// WinUI pipeline seam injects its own modifier snapshot for the stages owned
// by wx, but that snapshot cannot alter USER32's accelerator matcher. Keep the
// real thread state deterministic, scoped and restored without SendInput.
class KeyboardStateScope final
{
public:
    explicit KeyboardStateScope(int virtualKey = 0)
    {
        m_savedValid = ::GetKeyboardState(m_saved.data()) != FALSE;
        if ( !m_savedValid )
            return;

        auto current = m_saved;
        for ( const int modifier :
              { VK_SHIFT, VK_LSHIFT, VK_RSHIFT,
                VK_CONTROL, VK_LCONTROL, VK_RCONTROL,
                VK_MENU, VK_LMENU, VK_RMENU } )
        {
            current[modifier] &= 0x7f;
        }

        if ( virtualKey )
        {
            current[virtualKey] |= 0x80;
            if ( virtualKey == VK_CONTROL )
                current[VK_LCONTROL] |= 0x80;
            else if ( virtualKey == VK_MENU )
                current[VK_LMENU] |= 0x80;
            else if ( virtualKey == VK_SHIFT )
                current[VK_LSHIFT] |= 0x80;
        }

        m_applied = ::SetKeyboardState(current.data()) != FALSE;
    }

    ~KeyboardStateScope()
    {
        if ( m_savedValid )
            ::SetKeyboardState(m_saved.data());
    }

    bool IsApplied() const { return m_applied; }

private:
    std::array<BYTE, 256> m_saved = {};
    bool m_savedValid = false;
    bool m_applied = false;
    wxDECLARE_NO_COPY_CLASS(KeyboardStateScope);
};

class TransactionFormattingDialog final
    : public wxRichTextFormattingDialog
{
public:
    bool ClickButton(wxWindowID id)
    {
        return EmulateButtonClickIfPresent(id);
    }

    bool Validate() override
    {
        ++validateCalls;
        return validationAllowsClose;
    }

    bool TransferDataFromWindow() override
    {
        ++transferCalls;
        if ( !transferAllowsClose )
            return false;
        return wxRichTextFormattingDialog::TransferDataFromWindow();
    }

    bool validationAllowsClose = true;
    bool transferAllowsClose = true;
    unsigned validateCalls = 0;
    unsigned transferCalls = 0;
};

} // anonymous namespace

TEST_CASE("WinUIRichText::EditSelectionCaretUndoAndSearch",
          "[winui-rich][rich][model][search]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    RichTextFixture fixture;
    REQUIRE(fixture.Create(true));
    wxRichTextCtrl * const rich = fixture.GetRich();
    REQUIRE(rich);
    CHECK(rich->HasFlag(wxWANTS_CHARS));

    rich->SetValue("alpha beta gamma");
    rich->SetInsertionPoint(5);
    CHECK(rich->GetInsertionPoint() == 5);
    rich->SetSelection(6, 10);
    CHECK(rich->GetStringSelection() == "beta");
    rich->Replace(6, 10, "BETA");
    CHECK(rich->GetValue() == "alpha BETA gamma");
    REQUIRE(rich->CanUndo());
    rich->Undo();
    CHECK(rich->GetValue() == "alpha beta gamma");
    REQUIRE(rich->CanRedo());
    rich->Redo();
    CHECK(rich->GetValue() == "alpha BETA gamma");

    rich->SetValue("zero very veryish VERY very end");
    wxTextSearchResult found = rich->SearchText(
        wxTextSearch("very").MatchCase().MatchWholeWord());
    REQUIRE(found);
    CHECK(found.m_start == 5);
    CHECK(found.m_end == 9);

    found = rich->SearchText(
        wxTextSearch("very").MatchCase().MatchWholeWord().Start(6));
    REQUIRE(found);
    CHECK(found.m_start == 23);
    CHECK(found.m_end == 27);
    found = rich->SearchText(
        wxTextSearch("very").MatchWholeWord().Start(6));
    REQUIRE(found);
    CHECK(found.m_start == 18);
    CHECK(found.m_end == 22);

    found = rich->SearchText(
        wxTextSearch("very")
            .SearchDirection(wxTextSearch::Direction::Up)
            .MatchWholeWord());
    REQUIRE(found);
    CHECK(found.m_start == 23);
    CHECK(found.m_end == 27);
    found = rich->SearchText(
        wxTextSearch("very")
            .SearchDirection(wxTextSearch::Direction::Up)
            .MatchWholeWord()
            .Start(found.m_start));
    REQUIRE(found);
    CHECK(found.m_start == 18);
    CHECK(found.m_end == 22);

    CHECK_FALSE(rich->SearchText(wxTextSearch("very").Start(999)));
    CHECK_FALSE(rich->SearchText(wxTextSearch("very").Start(-2)));
    CHECK_FALSE(rich->SearchText(wxTextSearch("missing")));
    CHECK_FALSE(rich->SearchText(wxTextSearch(wxEmptyString)));
    found = rich->SearchText(wxTextSearch("zero").MatchWholeWord());
    REQUIRE(found);
    CHECK(found.m_start == 0);
    found = rich->SearchText(
        wxTextSearch("end")
            .SearchDirection(wxTextSearch::Direction::Up)
            .MatchWholeWord());
    REQUIRE(found);
    CHECK(found.m_end == rich->GetLastPosition());

    // GetValue() omits an image even though it consumes one public RichText
    // position. Search results must retain that position and a match must not
    // cross the embedded object as if it were absent.
    rich->SetValue("left");
    rich->SetInsertionPointEnd();
    wxImage image(1, 1);
    image.SetRGB(wxRect(0, 0, 1, 1), 20, 80, 140);
    REQUIRE(rich->WriteImage(image, wxBITMAP_TYPE_BMP));
    rich->WriteText("target");
    const size_t flattenedTargetIndex = rich->GetValue().find("target");
    REQUIRE(flattenedTargetIndex != wxString::npos);
    const long flattenedTarget = static_cast<long>(flattenedTargetIndex);
    found = rich->SearchText(wxTextSearch("target").MatchCase());
    REQUIRE(found);
    CHECK(found.m_start == flattenedTarget + 1);
    CHECK(rich->GetRange(found.m_start, found.m_end) == "target");
    CHECK_FALSE(rich->SearchText(wxTextSearch("ttarget").MatchCase()));

    // A text box has its own zero-based public range. Searching while it has
    // the editing focus must neither see the outer buffer nor return flattened
    // outer-buffer offsets.
    rich->SetFocusObject(nullptr);
    rich->SetValue("outer-only");
    rich->SetInsertionPointEnd();
    wxRichTextBox * const box = rich->WriteTextBox();
    REQUIRE(box);
    REQUIRE(rich->SetFocusObject(box));
    rich->SetInsertionPointEnd();
    rich->WriteText("inner target");
    found = rich->SearchText(wxTextSearch("target").MatchCase());
    REQUIRE(found);
    CHECK(found.m_start == 6);
    CHECK(found.m_end == 12);
    CHECK(rich->GetRange(found.m_start, found.m_end) == "target");
    CHECK_FALSE(rich->SearchText(wxTextSearch("outer-only").MatchCase()));

    // Table cells are independent editing objects too.
    rich->SetFocusObject(nullptr);
    rich->SetInsertionPointEnd();
    wxRichTextTable * const nestedTable = rich->WriteTable(1, 1);
    REQUIRE(nestedTable);
    wxRichTextCell * const cell = nestedTable->GetCell(0, 0);
    REQUIRE(cell);
    REQUIRE(rich->SetFocusObject(cell));
    rich->SetInsertionPointEnd();
    rich->WriteText("cell target");
    found = rich->SearchText(wxTextSearch("target").MatchCase());
    REQUIRE(found);
    CHECK(found.m_start == 5);
    CHECK(found.m_end == 11);
    CHECK(rich->GetRange(found.m_start, found.m_end) == "target");
    CHECK_FALSE(rich->SearchText(wxTextSearch("outer-only").MatchCase()));
    rich->SetFocusObject(nullptr);

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}

TEST_CASE("WinUIRichText::StylesListsImagesObjectsAndMemoryFormats",
          "[winui-rich][rich][styles][formats]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

#if wxUSE_XML
    XMLHandlerScope xmlHandler;
#endif
#if wxUSE_HTML
    HTMLHandlerScope htmlHandler;
#endif
    wxRichTextBuffer::InitStandardHandlers();

    wxRichTextStyleSheet sheet;
    wxRichTextCharacterStyleDefinition * const strong =
        new wxRichTextCharacterStyleDefinition("Strong014");
    strong->GetStyle().SetFontWeight(wxFONTWEIGHT_BOLD);
    REQUIRE(sheet.AddCharacterStyle(strong));

    wxRichTextParagraphStyleDefinition * const centred =
        new wxRichTextParagraphStyleDefinition("Centred014");
    centred->GetStyle().SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
    centred->GetStyle().SetParagraphSpacingAfter(24);
    REQUIRE(sheet.AddParagraphStyle(centred));

    constexpr int numberedLevel = 1;
    wxRichTextListStyleDefinition * const numbered =
        new wxRichTextListStyleDefinition("Numbered014");
    numbered->SetAttributes(
        numberedLevel, 120, 40,
        wxTEXT_ATTR_BULLET_STYLE_ARABIC |
            wxTEXT_ATTR_BULLET_STYLE_PERIOD);
    const wxRichTextAttr * const numberedLevelAttributes =
        numbered->GetLevelAttributes(numberedLevel);
    REQUIRE(numberedLevelAttributes);
    REQUIRE(numberedLevelAttributes->HasBulletStyle());
    REQUIRE(sheet.AddListStyle(numbered));

    RichTextFixture fixture;
    REQUIRE(fixture.Create(false, 2));
    wxRichTextCtrl * const rich = fixture.GetRich();
    REQUIRE(rich);
    rich->SetStyleSheet(&sheet);

    REQUIRE(rich->BeginCharacterStyle("Strong014"));
    rich->AddParagraph("strong");
    REQUIRE(rich->EndCharacterStyle());
    REQUIRE(rich->BeginParagraphStyle("Centred014"));
    rich->AddParagraph("centred");
    REQUIRE(rich->EndParagraphStyle());
    // BeginListStyle() selects the same zero-based level slot consumed by
    // GetCombinedStyleForLevel(); configure and exercise that exact slot.
    REQUIRE(rich->BeginListStyle("Numbered014", numberedLevel, 1));
    rich->AddParagraph("listed");
    REQUIRE(rich->EndListStyle());
    rich->SetInsertionPointEnd();

    wxTextAttr attr;
    const size_t strongAt = rich->GetValue().find("strong");
    REQUIRE(strongAt != wxString::npos);
    REQUIRE(rich->GetStyle(static_cast<long>(strongAt), attr));
    CHECK(attr.GetFontWeight() == wxFONTWEIGHT_BOLD);
    const size_t centredAt = rich->GetValue().find("centred");
    REQUIRE(centredAt != wxString::npos);
    REQUIRE(rich->GetStyle(static_cast<long>(centredAt), attr));
    CHECK(attr.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE);
    CHECK(attr.GetParagraphSpacingAfter() == 24);
    const size_t listedAt = rich->GetValue().find("listed");
    REQUIRE(listedAt != wxString::npos);
    REQUIRE(rich->GetStyle(static_cast<long>(listedAt), attr));
    CHECK(attr.HasBulletStyle());
    CHECK(attr.HasListStyleName());
    CHECK(attr.GetListStyleName() == "Numbered014");

    wxImage image(2, 2);
    image.SetRGB(wxRect(0, 0, 2, 2), 20, 80, 140);
    REQUIRE(rich->WriteImage(image, wxBITMAP_TYPE_BMP));
    wxRichTextBox * const textBox = rich->WriteTextBox();
    REQUIRE(textBox);
    rich->SetFocusObject(nullptr);
    wxRichTextTable * const table = rich->WriteTable(2, 2);
    REQUIRE(table);
    CHECK(table->GetRowCount() == 2);
    CHECK(table->GetColumnCount() == 2);

#if wxUSE_STREAMS
    wxStringOutputStream plainOut;
    REQUIRE(rich->GetBuffer().SaveFile(
        plainOut, wxRICHTEXT_TYPE_TEXT));
    wxStringInputStream plainIn(plainOut.GetString());
    wxRichTextBuffer plainRoundTrip;
    REQUIRE(plainRoundTrip.LoadFile(plainIn, wxRICHTEXT_TYPE_TEXT));
    CHECK(plainRoundTrip.GetText().Contains("strong"));
    CHECK(plainRoundTrip.GetText().Contains("listed"));

#if wxUSE_XML
    wxStringOutputStream xmlOut;
    REQUIRE(rich->GetBuffer().SaveFile(xmlOut, wxRICHTEXT_TYPE_XML));
    CHECK(xmlOut.GetString().Contains("<image"));
    CHECK(xmlOut.GetString().Contains("<textbox"));
    CHECK(xmlOut.GetString().Contains("<table"));
    wxStringInputStream xmlIn(xmlOut.GetString());
    wxRichTextBuffer xmlRoundTrip;
    REQUIRE(xmlRoundTrip.LoadFile(xmlIn, wxRICHTEXT_TYPE_XML));
    CHECK(xmlRoundTrip.GetText().Contains("strong"));
    CHECK(xmlRoundTrip.GetText().Contains("listed"));
#endif

#if wxUSE_HTML
    wxStringOutputStream htmlOut;
    REQUIRE(rich->GetBuffer().SaveFile(htmlOut, wxRICHTEXT_TYPE_HTML));
    CHECK(htmlOut.GetString().Lower().Contains("<html"));
    CHECK(htmlOut.GetString().Contains("strong"));
    wxRichTextFileHandler * const handler =
        wxRichTextBuffer::FindHandler(wxRICHTEXT_TYPE_HTML);
    REQUIRE(handler);
    CHECK(handler->CanSave());
    CHECK_FALSE(handler->CanLoad());
#endif
#endif // wxUSE_STREAMS

    // The style sheet is caller-owned and must outlive the buffer reference.
    rich->SetStyleSheet(nullptr);
    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}

TEST_CASE("WinUIRichText::LargeDocumentScrollTwoTLWReparentAndHundredCycles",
          "[winui-rich][rich][large-buffer][reparent][stress]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    const unsigned liveBefore = ProbeRichTextCtrl::GetLiveCount();

    RichTextFixture fixture;
    REQUIRE(fixture.Create(false, 4));
    ProbeRichTextCtrl *rich = fixture.GetRich();
    REQUIRE(rich);

    wxString large;
    large.reserve(125000);
    for ( int line = 0; line < 2500; ++line )
    {
        large += wxString::Format(
            "line-%04d deterministic rich payload 0123456789", line);
        if ( line != 2499 )
            large += '\n';
    }
    rich->SetDelayedLayoutThreshold(512);
    rich->SetValue(large);
    CHECK(rich->GetValue().length() == large.length());
    CHECK(rich->GetNumberOfLines() == 2500);
    CHECK(rich->GetLineText(2147).StartsWith("line-2147"));
    rich->ShowPosition(rich->GetLastPosition());
    REQUIRE(DrainUntil(
        [rich]()
        {
            int x = 0;
            int y = 0;
            rich->GetViewStart(&x, &y);
            return y > 0;
        }));

    const HWND originalHwnd = static_cast<HWND>(rich->GetHWND());
    REQUIRE(rich->Reparent(fixture.GetPanelB()));
    CHECK(rich->GetParent() == fixture.GetPanelB());
    CHECK(static_cast<HWND>(rich->GetHWND()) == originalHwnd);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(rich) == nullptr);
    CHECK(fixture.GetHostA()->FindSlot(rich) == nullptr);
    CHECK(fixture.GetHostB()->FindSlot(rich) == nullptr);
    REQUIRE(rich->Reparent(fixture.GetPanelA()));
    CHECK(static_cast<HWND>(rich->GetHWND()) == originalHwnd);

    const unsigned steadyHosts = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned steadySlots = wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned steadySubclasses =
        wxWinUITopLevelHost::GetLiveSubclassContextCount();
    wxWeakRef<ProbeRichTextCtrl> initialLifetime(rich);
    rich->Destroy();
    fixture.ForgetRich();
    REQUIRE(DrainUntil([&initialLifetime]() { return !initialLifetime; }));

    std::shared_ptr<ProbeState> probes[100];
    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        CAPTURE(cycle);
        ProbeRichTextCtrl *control = nullptr;
        if ( cycle % 2 == 0 )
        {
            control = new ProbeRichTextCtrl(fixture.GetPanelA());
        }
        else
        {
            control = new ProbeRichTextCtrl;
            REQUIRE(control->Create(
                fixture.GetPanelA(), wxID_ANY, wxEmptyString,
                wxPoint(20, 70), wxSize(280, 190),
                wxRE_MULTILINE | wxBORDER_SIMPLE));
        }
        REQUIRE(control->GetHWND());
        probes[cycle] = std::make_shared<ProbeState>();
        control->ObserveTimers(probes[cycle]);
        control->SetValue(wxString::Format("cycle-%03u", cycle));
        control->EnableDelayedImageLoading(true);
        control->RequestDelayedImageProcessing();

        // Every call must be a genuine cross-TLW move: Reparent() to the
        // already-current parent is a documented no-op returning false.
        REQUIRE(control->Reparent(fixture.GetPanelB()));
        if ( cycle % 2 != 0 )
            REQUIRE(control->Reparent(fixture.GetPanelA()));
        CHECK(wxWinUITopLevelHost::FindSlotOwner(control) == nullptr);

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        wxMSWOleDropTargetBinding binding;
        REQUIRE(wxMSWOleLookupDropTarget(control, &binding) ==
                wxMSWOleDropTargetLookup::Found);
        CHECK(binding.GetOwnerIfCurrent() == control);
        CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
            control->GetDropTarget(), control->GetHWND()));
#endif

        wxWeakRef<ProbeRichTextCtrl> lifetime(control);
        control->Destroy();
        REQUIRE(DrainUntil([&lifetime]() { return !lifetime; }, 40));
        CHECK(probes[cycle]->destroyed);
        CHECK(probes[cycle]->lateCallbacks == 0);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        CHECK_FALSE(binding.IsCurrent());
#endif
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == steadyHosts);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              steadySubclasses);
    }

    DrainDispatch(20);
    for ( const std::shared_ptr<ProbeState>& probe : probes )
    {
        REQUIRE(probe);
        CHECK(probe->lateCallbacks == 0);
    }
    CHECK(ProbeRichTextCtrl::GetLiveCount() == liveBefore);

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 260));
    before.CheckRestored();
}

TEST_CASE("WinUIRichText::LogicalKeyboardTabAndAccelerators",
          "[winui-rich][rich][keyboard]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    RichTextFixture fixture;
    REQUIRE(fixture.Create(false, 6));
    wxRichTextCtrl * const rich = fixture.GetRich();
    REQUIRE(rich);

    enum { ID_F6 = wxID_HIGHEST + 6420 };
    wxAcceleratorEntry entry;
    entry.Set(wxACCEL_NORMAL, WXK_F6, ID_F6);
    fixture.GetFrameA()->SetAcceleratorTable(
        wxAcceleratorTable(1, &entry));
    unsigned frameCommands = 0;
    fixture.GetFrameA()->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { ++frameCommands; },
        ID_F6);

    rich->SetValue("accelerator selection");
    rich->SelectNone();
    XamlStageTrace acceleratorTrace;
    MSG ctrlA = KeyMessage(
        static_cast<HWND>(rich->GetHWND()), WM_KEYDOWN, 'A');
    {
        KeyboardStateScope controlDown(VK_CONTROL);
        REQUIRE(controlDown.IsApplied());
        CHECK(wxWinUI3RunKeyboardPipelineForTesting(
                  &ctrlA, rich, Modifiers(false, true), TraceXamlStage,
                  &acceleratorTrace) ==
              wxWinUIKeyboardPipelineResult::WxHandled);
    }
    CHECK(rich->GetStringSelection() == rich->GetValue());
    CHECK(frameCommands == 0);
    CHECK(acceleratorTrace.calls == 0);

    MSG f6 = KeyMessage(
        static_cast<HWND>(rich->GetHWND()), WM_KEYDOWN, VK_F6);
    {
        KeyboardStateScope noModifiers;
        REQUIRE(noModifiers.IsApplied());
        CHECK(wxWinUI3RunKeyboardPipelineForTesting(
                  &f6, rich, Modifiers(), TraceXamlStage,
                  &acceleratorTrace) ==
              wxWinUIKeyboardPipelineResult::WxHandled);
    }
    CHECK(frameCommands == 1);
    CHECK(acceleratorTrace.calls == 0);

    // RichText is a wxWANTS_CHARS native sibling immediately before a XAML
    // button. The mixed arbiter must not steal Tab from its character path.
    rich->SetValue("tab:");
    rich->SetInsertionPointEnd();
    HWND focusBefore = ::GetFocus();
    XamlStageTrace tabTrace;
    MSG tab = KeyMessage(
        static_cast<HWND>(rich->GetHWND()), WM_KEYDOWN, VK_TAB);
    CHECK_FALSE(wxWinUI3ProcessTabNavigationWithModifiers(
        &tab, false, false, false));
    CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
              &tab, Modifiers(), TraceXamlStage, &tabTrace) ==
          wxWinUIKeyboardPipelineResult::NotHandled);
    CHECK(::GetFocus() == focusBefore);
    CHECK(DeliverCharacter(rich, WXK_TAB));
    CHECK(rich->GetValue() == "tab:\t");
    CHECK(::GetFocus() == focusBefore);

    MSG shiftTab = KeyMessage(
        static_cast<HWND>(rich->GetHWND()), WM_KEYDOWN, VK_TAB);
    CHECK_FALSE(wxWinUI3ProcessTabNavigationWithModifiers(
        &shiftTab, true, false, false));
    CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
              &shiftTab, Modifiers(true), TraceXamlStage, &tabTrace) ==
          wxWinUIKeyboardPipelineResult::NotHandled);
    CHECK(DeliverCharacter(rich, WXK_TAB, true));
    CHECK(rich->GetValue() == "tab:\t\t");
    CHECK(::GetFocus() == focusBefore);

    MSG controlTab = KeyMessage(
        static_cast<HWND>(rich->GetHWND()), WM_KEYDOWN, VK_TAB);
    CHECK_FALSE(wxWinUI3ProcessTabNavigationWithModifiers(
        &controlTab, false, true, false));
    CHECK(wxWinUI3ProcessKeyboardMessageForTesting(
              &controlTab, Modifiers(false, true),
              TraceXamlStage, &tabTrace) ==
          wxWinUIKeyboardPipelineResult::NotHandled);
    const wxString beforeControlTab = rich->GetValue();
    DeliverCharacter(rich, WXK_TAB, false, true);
    CHECK(rich->GetValue() == beforeControlTab);
    CHECK(::GetFocus() == focusBefore);

    // These are committed character events, not a claim that physical IME
    // composition was automated.
    CHECK(DeliverCharacter(rich, 0x754C));
    CHECK(rich->GetValue().EndsWith(wxString::FromUTF8("界")));
    CHECK(DeliverCharacter(rich, '@', false, true, true));
    CHECK(rich->GetValue().EndsWith(wxString::FromUTF8("界@")));

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
TEST_CASE("WinUIRichText::ClipboardPublicAPIManualIsolatedDesktopOnly",
          "[.][winui-rich-clipboard-manual][clipboard]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    RichTextFixture fixture;
    REQUIRE(fixture.Create(false, 7));
    wxRichTextCtrl * const rich = fixture.GetRich();
    REQUIRE(rich);

    // Hidden/manual by design: this mutates the process-global Windows
    // clipboard and must only be selected on a genuinely isolated desktop.
    {
        wxClipboardLocker clipboard;
        REQUIRE_FALSE(!clipboard);
        REQUIRE(wxTheClipboard->SetData(
            new wxTextDataObject("clipboard-014")));
    }
    rich->SetValue("tail-");
    rich->SetInsertionPointEnd();
    REQUIRE(rich->CanPaste());
    rich->Paste();
    CHECK(rich->GetValue() == "tail-clipboard-014");
    rich->SetSelection(0, 4);
    rich->Copy();
    {
        wxClipboardLocker clipboard;
        REQUIRE_FALSE(!clipboard);
        wxTextDataObject copied;
        REQUIRE(wxTheClipboard->GetData(copied));
        CHECK(copied.GetText() == "tail");
    }
    rich->Cut();
    CHECK(rich->GetValue() == "-clipboard-014");

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}
#endif // wxUSE_CLIPBOARD && wxUSE_DATAOBJ

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP && wxUSE_DATAOBJ && \
        wxUSE_XML && wxUSE_STREAMS && wxUSE_FFILE
TEST_CASE("WinUIRichText::CommonOleBrokerAndRealDropFilesRoute",
          "[winui-rich][rich][dnd][ole][drop-files]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    XMLHandlerScope xmlHandler;

    RichTextFixture fixture;
    REQUIRE(fixture.Create(false, 8));
    ProbeRichTextCtrl * const rich = fixture.GetRich();
    REQUIRE(rich);
    wxWinUITopLevelHost * const hostA = fixture.GetHostA();
    wxWinUITopLevelHost * const hostB = fixture.GetHostB();
    REQUIRE(hostA);
    REQUIRE(hostB);
    REQUIRE(hostA->GetDropBrokerForTest());
    REQUIRE(hostB->GetDropBrokerForTest());

    wxDropTarget * const logicalTarget = rich->GetDropTarget();
    REQUIRE(logicalTarget);
    wxMSWOleDropTargetBinding bindingA;
    REQUIRE(wxMSWOleLookupDropTarget(rich, &bindingA) ==
            wxMSWOleDropTargetLookup::Found);
    CHECK(bindingA.GetTargetIfCurrent() == logicalTarget);
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
        logicalTarget, rich->GetHWND()));

    const wxWinUIDropBrokerSnapshot snapshotA =
        hostA->GetDropBrokerForTest()->GetSnapshotForTest();
    const wxWinUIDropBrokerSnapshot snapshotB =
        hostB->GetDropBrokerForTest()->GetSnapshotForTest();
    CHECK(snapshotA.active);
    CHECK(snapshotA.ownsRegistration);
    CHECK(snapshotA.registerCalls == 1);
    CHECK(snapshotA.registrationHwnd == GetHwndOf(fixture.GetFrameA()));
    CHECK(snapshotA.registrationHwnd != hostA->GetBridgeHwnd());
    CHECK(snapshotB.active);
    CHECK(snapshotB.ownsRegistration);
    CHECK(snapshotB.registerCalls == 1);
    CHECK(snapshotB.registrationHwnd == GetHwndOf(fixture.GetFrameB()));
    CHECK(snapshotB.registrationHwnd != hostB->GetBridgeHwnd());

    DropTargetRef comA(
        hostA->GetDropBrokerForTest()->GetCOMTargetForTest());
    DropTargetRef comB(
        hostB->GetDropBrokerForTest()->GetCOMTargetForTest());
    REQUIRE(comA);
    REQUIRE(comB);

    REQUIRE(rich->Reparent(fixture.GetPanelB()));
    REQUIRE(DrainToQuiescence());
    // Reparenting does not replace the logical target or its native HWND, so
    // its binding remains stable. The per-TLW broker route, tested below, is
    // the state that migrates from A to B.
    CHECK(bindingA.IsCurrent());
    wxMSWOleDropTargetBinding bindingB;
    REQUIRE(wxMSWOleLookupDropTarget(rich, &bindingB) ==
            wxMSWOleDropTargetLookup::Found);
    CHECK(bindingB.IsCurrent());
    CHECK(bindingB.GetOwnerIfCurrent() == rich);
    CHECK(bindingB.IsSameBinding(bindingA));
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
        logicalTarget, rich->GetHWND()));

    RECT richRect = {};
    REQUIRE(::GetWindowRect(
        static_cast<HWND>(rich->GetHWND()), &richRect));
    POINTL point =
    {
        richRect.left + (richRect.right - richRect.left) / 2,
        richRect.top + (richRect.bottom - richRect.top) / 2
    };

    wxRichTextBuffer * const sourceBuffer = new wxRichTextBuffer;
    sourceBuffer->AddParagraph("broker-object-014");
    wxRichTextBufferDataObject source(sourceBuffer);
    rich->SetValue("seed");
    rich->SetSelection(0, 4);

    DWORD effect = DROPEFFECT_COPY;
    REQUIRE(comA->DragEnter(
        source.GetInterface(), MK_CONTROL, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    REQUIRE(comA->DragLeave() == S_OK);

    effect = DROPEFFECT_COPY;
    REQUIRE(comB->DragEnter(
        source.GetInterface(), MK_CONTROL, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    REQUIRE(comB->Drop(
        source.GetInterface(), MK_CONTROL, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(rich->GetValue().Contains("broker-object-014"));

    TemporaryTextFile validFile("loaded-through-WM_DROPFILES");
    REQUIRE(validFile.IsOk());
    unsigned fileEvents = 0;
    rich->Bind(
        wxEVT_DROP_FILES,
        [&](wxDropFilesEvent& event)
        {
            ++fileEvents;
            event.Skip();
        });
    rich->DragAcceptFiles(true);
    CHECK((::GetWindowLongPtr(
               hostB->GetBridgeHwnd(), GWL_EXSTYLE) &
           WS_EX_ACCEPTFILES) != 0);

    POINT bridgePoint = { point.x, point.y };
    ::MapWindowPoints(
        HWND_DESKTOP, hostB->GetBridgeHwnd(), &bridgePoint, 1);
    DropFilesHandle drop(
        MakeDropFiles(bridgePoint, validFile.GetPath()));
    REQUIRE(drop.Get());
    ::SendMessage(
        hostB->GetBridgeHwnd(), WM_DROPFILES,
        reinterpret_cast<WPARAM>(drop.Release()), 0);
    CHECK(fileEvents == 1);
    CHECK(rich->GetValue() == "loaded-through-WM_DROPFILES");

    const wxString valueBeforeInvalid = rich->GetValue();
    {
        DropFilesHandle invalidDrop(MakeDropFiles(
            bridgePoint, validFile.GetPath() + ".missing"));
        REQUIRE(invalidDrop.Get());
        wxLogNull noExpectedLoadError;
        ::SendMessage(
            hostB->GetBridgeHwnd(), WM_DROPFILES,
            reinterpret_cast<WPARAM>(invalidDrop.Release()), 0);
    }
    CHECK(fileEvents == 2);
    CHECK(rich->GetValue() == valueBeforeInvalid);

    // The last-bound handler runs first and destroys the owning TLW without
    // skipping. The bridge must finish consuming HDROP and retire its COM
    // route before a retained callback can observe the dead target.
    wxWeakRef<wxFrame> ownerLifetime(fixture.GetFrameB());
    wxWeakRef<ProbeRichTextCtrl> richLifetime(rich);
    bool destroyedFromDropCallback = false;
    rich->Bind(
        wxEVT_DROP_FILES,
        [&](wxDropFilesEvent&)
        {
            destroyedFromDropCallback = true;
            fixture.GetFrameB()->Destroy();
        });
    {
        DropFilesHandle destroyingDrop(
            MakeDropFiles(bridgePoint, validFile.GetPath()));
        REQUIRE(destroyingDrop.Get());
        ::SendMessage(
            hostB->GetBridgeHwnd(), WM_DROPFILES,
            reinterpret_cast<WPARAM>(destroyingDrop.Release()), 0);
    }
    CHECK(destroyedFromDropCallback);
    REQUIRE(DrainUntil(
        [&ownerLifetime, &richLifetime]()
        {
            return !ownerLifetime && !richLifetime;
        },
        220));
    fixture.ForgetRich();
    CHECK_FALSE(bindingB.IsCurrent());
    effect = DROPEFFECT_MOVE;
    REQUIRE(comB->DragOver(0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 240));
    before.CheckRestored();
}
#endif

TEST_CASE("WinUIRichText::DialogsHandlersOwnerDefaultCancelAndCallbackDestroy",
          "[winui-rich][rich][dialog][lifetime]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    const bool savedRestoreLastPage =
        wxRichTextFormattingDialog::GetRestoreLastPage();
    const int savedLastPage = wxRichTextFormattingDialog::GetLastPage();
    wxScopeGuard restoreFormattingDialogStatics = wxMakeGuard(
        [savedRestoreLastPage, savedLastPage]()
        {
            wxRichTextFormattingDialog::SetRestoreLastPage(
                savedRestoreLastPage);
            wxRichTextFormattingDialog::SetLastPage(savedLastPage);
        });
    wxUnusedVar(restoreFormattingDialogStatics);

    RichTextFixture fixture;
    REQUIRE(fixture.Create(false, 10));
    ProbeRichTextCtrl * const rich = fixture.GetRich();
    REQUIRE(rich);

    const long allPages = wxRICHTEXT_FORMAT_STYLE_EDITOR |
                          wxRICHTEXT_FORMAT_FONT |
                          wxRICHTEXT_FORMAT_TABS |
                          wxRICHTEXT_FORMAT_BULLETS |
                          wxRICHTEXT_FORMAT_INDENTS_SPACING |
                          wxRICHTEXT_FORMAT_LIST_STYLE |
                          wxRICHTEXT_FORMAT_MARGINS |
                          wxRICHTEXT_FORMAT_SIZE |
                          wxRICHTEXT_FORMAT_BORDERS |
                          wxRICHTEXT_FORMAT_BACKGROUND;
    wxRichTextStyleSheet dialogSheet;
    wxRichTextListStyleDefinition dialogList("DialogList014");
    dialogList.GetStyle().SetFontWeight(wxFONTWEIGHT_BOLD);
    dialogList.GetStyle().SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
    dialogList.SetAttributes(
        0, 120, 40,
        wxTEXT_ATTR_BULLET_STYLE_ARABIC |
            wxTEXT_ATTR_BULLET_STYLE_PERIOD);
    {
        TransactionFormattingDialog dialog;
        REQUIRE(dialog.Create(
            allPages, fixture.GetFrameA(),
            "RichText transaction 014"));
        REQUIRE(dialog.GetBookCtrl());
        CHECK(dialog.GetBookCtrl()->GetPageCount() == 10);
        CHECK(dialog.FindWindow(wxID_OK));
        CHECK(dialog.FindWindow(wxID_CANCEL));
        CHECK(dialog.GetAffirmativeId() == wxID_OK);

        // The list-style page edits a style definition, not the dialog's
        // free-form attribute value. Supplying the required definition keeps
        // all ten pages meaningful and avoids exercising the upstream page
        // with an invalid null model.
        REQUIRE(dialog.SetStyleDefinition(
            dialogList, &dialogSheet, false));
        REQUIRE(dialog.TransferDataToWindow());
        REQUIRE(ShowOffscreenWithoutActivation(&dialog, 12));
        REQUIRE(dialog.IsShown());

        dialog.validationAllowsClose = false;
        REQUIRE(dialog.ClickButton(wxID_OK));
        CHECK(dialog.IsShown());
        CHECK(dialog.validateCalls == 1);
        CHECK(dialog.transferCalls == 0);

        dialog.validationAllowsClose = true;
        dialog.transferAllowsClose = false;
        REQUIRE(dialog.ClickButton(wxID_OK));
        CHECK(dialog.IsShown());
        CHECK(dialog.validateCalls == 2);
        CHECK(dialog.transferCalls == 1);

        dialog.transferAllowsClose = true;
        REQUIRE(dialog.ClickButton(wxID_OK));
        CHECK_FALSE(dialog.IsShown());
        CHECK(dialog.validateCalls == 3);
        CHECK(dialog.transferCalls == 2);
        REQUIRE(ShowOffscreenWithoutActivation(&dialog, 12));
        REQUIRE(dialog.ClickButton(wxID_CANCEL));
        CHECK_FALSE(dialog.IsShown());
        CHECK(dialog.validateCalls == 3);
        CHECK(dialog.transferCalls == 2);
    }

    wxRichTextStyleSheet sheet;
    wxRichTextCharacterStyleDefinition * const definition =
        new wxRichTextCharacterStyleDefinition("DialogStyle014");
    definition->GetStyle().SetFontStyle(wxFONTSTYLE_ITALIC);
    REQUIRE(sheet.AddCharacterStyle(definition));
    {
        wxRichTextStyleOrganiserDialog organiser(
            wxRICHTEXT_ORGANISER_BROWSE,
            &sheet, rich, fixture.GetFrameA(), wxID_ANY,
            "RichText organiser 014");
        CHECK(organiser.GetStyleSheet() == &sheet);
        CHECK(organiser.GetRichTextCtrl() == rich);
        CHECK(organiser.FindWindow(wxID_OK));
        CHECK(organiser.FindWindow(wxID_CANCEL));
        REQUIRE(organiser.TransferDataToWindow());
    }
    {
        wxRichTextStyleOrganiserDialog organiser;
        REQUIRE(organiser.Create(
            wxRICHTEXT_ORGANISER_BROWSE,
            &sheet, rich, fixture.GetFrameA(), wxID_ANY,
            "RichText organiser Create 014"));
        CHECK(organiser.GetStyleSheet() == &sheet);
    }

    rich->SetInsertionPointEnd();
    wxRichTextBox * const box = rich->WriteTextBox();
    REQUIRE(box);
    rich->SetFocusObject(nullptr);
    {
        wxRichTextObjectPropertiesDialog properties(
            box, fixture.GetFrameA(), wxID_ANY,
            "RichText object properties 014");
        CHECK(properties.GetObject() == box);
        REQUIRE(properties.TransferDataToWindow());
    }

    // Start the first two-page dialog on its first page, independently of any
    // process-global selection left by an earlier test.
    wxRichTextFormattingDialog::SetRestoreLastPage(false);
    wxRichTextFormattingDialog::SetLastPage(wxNOT_FOUND);
    const long ownerDialogPages =
        wxRICHTEXT_FORMAT_FONT | wxRICHTEXT_FORMAT_TABS;
    const std::size_t ownerTransactionBaseline =
        wxWinUI3GetOwnerSnapshotForTesting(nullptr).transactionCount;
    wxFrame * const owner = new wxFrame(
        nullptr, wxID_ANY, "RichText dialog owner 014",
        wxDefaultPosition, wxSize(360, 220),
        wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
            wxFRAME_TOOL_WINDOW);
    TopLevelDestroyGuard ownerGuard(owner);
    REQUIRE(ShowOffscreenWithoutActivation(owner, 14));
    std::unique_ptr<wxRichTextFormattingDialog> ownedDialogOwner(
        new wxRichTextFormattingDialog);
    REQUIRE(ownedDialogOwner->Create(
        ownerDialogPages, owner,
        "RichText owned dialog 014"));
    wxRichTextFormattingDialog * const ownedDialog =
        ownedDialogOwner.release();
    TopLevelDestroyGuard ownedDialogGuard(ownedDialog);
    wxWeakRef<wxFrame> ownerLifetime(owner);
    wxWeakRef<wxRichTextFormattingDialog> dialogLifetime(ownedDialog);
    wxBookCtrlBase* const ownedBook = ownedDialog->GetBookCtrl();
    REQUIRE(ownedBook);
    REQUIRE(ownedBook->GetPageCount() == 2);
    REQUIRE(ownedBook->GetSelection() == 0);
    REQUIRE(ownedBook->ChangeSelection(1) == 0);
    REQUIRE(ownedBook->GetSelection() == 1);
    const WXHWND ownedDialogHwnd = ownedDialog->GetHWND();
    const HWND nativeDialog = reinterpret_cast<HWND>(ownedDialogHwnd);
    const HWND nativeOwner = static_cast<HWND>(owner->GetHWND());
    constexpr wchar_t stashedOwnerProperty[] =
        L"wxWidgets.WinUI.StashedOwner."
        L"{32117897-FD59-4E46-88AD-C7B707EC7D2B}";
    const auto getNativeOwner = [nativeDialog]()
    {
        return ::GetWindow(nativeDialog, GW_OWNER);
    };
    const auto getStashedOwner =
        [nativeDialog, &stashedOwnerProperty]()
    {
        return reinterpret_cast<HWND>(
            ::GetPropW(nativeDialog, stashedOwnerProperty));
    };
    // wxPropertySheetDialog resolves its parent using the modal-dialog
    // policy. The requested owner must already be shown or the resolver is
    // allowed to replace it with another active TLW (or null), in which case
    // destroying `owner` would not be an owner/child lifetime test at all.
    REQUIRE(ownedDialog->GetParent() == owner);
    REQUIRE(owner->GetChildren().Find(ownedDialog));

    // Creating a WinUI dialog can immediately enter the deliberate active
    // owner-detach transaction. A null GW_OWNER is valid only when the exact
    // transaction and its USER32 marker both identify this logical owner.
    const wxWinUITransientSnapshot ownerStateAfterCreate =
        wxWinUI3GetOwnerSnapshotForTesting(ownedDialogHwnd);
    if ( !getNativeOwner() )
    {
        REQUIRE(ownerStateAfterCreate.transactionCount ==
                ownerTransactionBaseline + 1);
        REQUIRE_FALSE(ownerStateAfterCreate.inFlight);
        REQUIRE(ownerStateAfterCreate.generation != 0);
        REQUIRE(ownerStateAfterCreate.epoch != 0);
        REQUIRE(getStashedOwner() == nativeOwner);
        REQUIRE(wxWinUITransientOwnerActivation(ownedDialogHwnd, false));
    }
    else
    {
        REQUIRE(getNativeOwner() == nativeOwner);
        REQUIRE(ownerStateAfterCreate.transactionCount ==
                ownerTransactionBaseline);
        REQUIRE(ownerStateAfterCreate.generation == 0);
        REQUIRE(ownerStateAfterCreate.epoch == 0);
        REQUIRE(getStashedOwner() == nullptr);
    }
    REQUIRE(getNativeOwner() == nativeOwner);
    REQUIRE(getStashedOwner() == nullptr);
    REQUIRE(wxWinUI3GetOwnerSnapshotForTesting(ownedDialogHwnd).
                transactionCount == ownerTransactionBaseline);

    REQUIRE(ShowOffscreenWithoutActivation(ownedDialog, 15));
    REQUIRE(ownedDialog->GetParent() == owner);
    REQUIRE(owner->GetChildren().Find(ownedDialog));

    // Normalize any activation generated by ShowWithoutActivating(), then
    // enter a fresh, observable detached epoch ourselves. The lifetime test
    // below therefore proves that wx ownership destroys the dialog even while
    // its native owner is deliberately absent.
    if ( !getNativeOwner() )
    {
        const wxWinUITransientSnapshot ownerStateAfterShow =
            wxWinUI3GetOwnerSnapshotForTesting(ownedDialogHwnd);
        REQUIRE(ownerStateAfterShow.transactionCount ==
                ownerTransactionBaseline + 1);
        REQUIRE_FALSE(ownerStateAfterShow.inFlight);
        REQUIRE(ownerStateAfterShow.generation != 0);
        REQUIRE(ownerStateAfterShow.epoch != 0);
        REQUIRE(getStashedOwner() == nativeOwner);
        REQUIRE(wxWinUITransientOwnerActivation(ownedDialogHwnd, false));
    }
    REQUIRE(getNativeOwner() == nativeOwner);
    REQUIRE(getStashedOwner() == nullptr);
    REQUIRE(wxWinUI3GetOwnerSnapshotForTesting(ownedDialogHwnd).
                transactionCount == ownerTransactionBaseline);

    REQUIRE(wxWinUITransientOwnerActivation(ownedDialogHwnd, true));
    const wxWinUITransientSnapshot detachedOwnerState =
        wxWinUI3GetOwnerSnapshotForTesting(ownedDialogHwnd);
    REQUIRE(getNativeOwner() == nullptr);
    REQUIRE(getStashedOwner() == nativeOwner);
    REQUIRE(detachedOwnerState.transactionCount ==
            ownerTransactionBaseline + 1);
    REQUIRE_FALSE(detachedOwnerState.inFlight);
    REQUIRE(detachedOwnerState.generation != 0);
    REQUIRE(detachedOwnerState.epoch != 0);

    bool ownerDestroyObserved = false;
    bool ownerDestroyRetainedDialogChild = false;
    bool dialogDestroyObserved = false;
    bool dialogDestroyBookWasIndependentlyAvailable = false;
    bool dialogDestroyBookWasHierarchyUnavailable = false;
    ownedDialog->Bind(
        wxEVT_DESTROY,
        [&](wxWindowDestroyEvent& event)
        {
            if ( event.GetWindow() == ownedDialog )
            {
                dialogDestroyObserved = true;
                dialogDestroyBookWasIndependentlyAvailable =
                    !wxWindowItselfIsUnavailableForCallbacks(ownedBook);
                dialogDestroyBookWasHierarchyUnavailable =
                    wxWindowIsUnavailableForCallbacks(ownedBook);
            }
            event.Skip();
        });
    owner->Bind(
        wxEVT_DESTROY,
        [&](wxWindowDestroyEvent& event)
        {
            if ( event.GetWindow() == owner )
            {
                ownerDestroyObserved = true;
                ownerDestroyRetainedDialogChild =
                    ownedDialog->GetParent() == owner &&
                    owner->GetChildren().Find(ownedDialog);
            }
            event.Skip();
        });
    owner->Destroy();
    const bool ownerAndDialogDestroyed = DrainPendingDeleteUntil(
        [&ownerLifetime, &dialogLifetime]()
        {
            return !ownerLifetime && !dialogLifetime;
        },
        40);
    INFO("owner alive=" << (ownerLifetime.get() != nullptr)
         << ", owner pending=" << wxPendingDelete.Member(owner)
         << ", dialog alive=" << (dialogLifetime.get() != nullptr)
         << ", dialog pending=" << wxPendingDelete.Member(ownedDialog));
    CHECK(ownerDestroyObserved);
    CHECK(ownerDestroyRetainedDialogChild);
    CHECK(dialogDestroyObserved);
    CHECK(dialogDestroyBookWasIndependentlyAvailable);
    CHECK(dialogDestroyBookWasHierarchyUnavailable);
    CHECK_FALSE(ownerLifetime);
    CHECK_FALSE(dialogLifetime);
    REQUIRE(ownerAndDialogDestroyed);
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(nullptr).transactionCount ==
          ownerTransactionBaseline);
    REQUIRE(wxRichTextFormattingDialog::GetLastPage() ==
            wxRICHTEXT_FORMAT_TABS);

    // ChangeSelection() deliberately emits no page event. Recreating the same
    // dialog proves the destructor sampled the exact live book selection from
    // the property-sheet weak sidecar instead of relying on event tracking.
    wxRichTextFormattingDialog::SetRestoreLastPage(true);
    {
        wxRichTextFormattingDialog restoredDialog;
        REQUIRE(restoredDialog.Create(
            ownerDialogPages, fixture.GetFrameA(),
            "RichText restored page 014"));
        wxBookCtrlBase* const restoredBook = restoredDialog.GetBookCtrl();
        REQUIRE(restoredBook);
        REQUIRE(restoredBook->GetPageCount() == 2);
        CHECK(restoredBook->GetSelection() == 1);
    }

    // Destruction requested synchronously from a RichText callback must leave
    // no timer, callback, host or capture residue after the event unwinds.
    RichTextFixture callbackFixture;
    REQUIRE(callbackFixture.Create(false, 16));
    ProbeRichTextCtrl * const callbackRich = callbackFixture.GetRich();
    REQUIRE(callbackRich);
    std::shared_ptr<ProbeState> callbackProbe =
        std::make_shared<ProbeState>();
    callbackRich->ObserveTimers(callbackProbe);
    callbackRich->EnableDelayedImageLoading(true);
    callbackRich->RequestDelayedImageProcessing();
    wxWeakRef<wxFrame> callbackOwner(callbackFixture.GetFrameA());
    wxWeakRef<ProbeRichTextCtrl> callbackLifetime(callbackRich);
    bool callbackRan = false;
    callbackRich->Bind(
        wxEVT_RICHTEXT_CONTENT_INSERTED,
        [&](wxRichTextEvent&)
        {
            callbackRan = true;
            callbackFixture.GetFrameA()->Destroy();
        });
    callbackRich->WriteText("destroy-from-rich-callback");
    CHECK(callbackRan);
    REQUIRE(DrainUntil(
        [&callbackOwner, &callbackLifetime]()
        {
            return !callbackOwner && !callbackLifetime;
        },
        220));
    callbackFixture.ForgetRich();
    CHECK(callbackProbe->destroyed);
    DrainDispatch(20);
    CHECK(callbackProbe->lateCallbacks == 0);
    callbackFixture.Cleanup();

    rich->SetStyleSheet(nullptr);
    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 260));
    before.CheckRestored();
}

#if wxUSE_XRC && wxUSE_XML
TEST_CASE("WinUIRichText::XrcControlHandler",
          "[winui-rich][rich][xrc]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxRichTextCtrl" name="winuiRichTextXrc014">
    <value>XRC rich value 014</value>
    <pos>20,30</pos>
    <size>280,150</size>
    <style>wxTE_MULTILINE|wxTE_PROCESS_TAB</style>
  </object>
</resource>
)XRC";

    wxXmlResource resource;
    resource.AddHandler(new wxRichTextCtrlXmlHandler);
    wxStringInputStream stream(wxString::FromUTF8(xrcText));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE(document->IsOk());
    REQUIRE(resource.LoadDocument(
        document.release(), "winui-richtext-014.xrc"));

    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "WinUI RichText XRC 014",
        wxDefaultPosition, wxSize(380, 240),
        wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
            wxFRAME_TOOL_WINDOW);
    TopLevelDestroyGuard frameGuard(frame);
    wxWeakRef<wxFrame> frameLifetime(frame);
    wxRichTextCtrl * const rich = wxDynamicCast(
        resource.LoadObject(
            frame, "winuiRichTextXrc014", "wxRichTextCtrl"),
        wxRichTextCtrl);
    REQUIRE(rich);
    wxWeakRef<wxRichTextCtrl> richLifetime(rich);
    CHECK(rich->GetName() == "winuiRichTextXrc014");
    CHECK(rich->GetValue() == "XRC rich value 014");
    CHECK(rich->HasFlag(wxTE_PROCESS_TAB));
    CHECK(rich->HasFlag(wxWANTS_CHARS));
    CHECK(wxWinUITopLevelHost::FindSlotOwner(rich) == nullptr);
    REQUIRE(ShowOffscreenWithoutActivation(frame, 18));

    frame->Destroy();
    REQUIRE(DrainUntil(
        [&frameLifetime, &richLifetime]()
        {
            return !frameLifetime && !richLifetime;
        },
        220));
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 220));
    before.CheckRestored(false);
}
#endif // wxUSE_XRC && wxUSE_XML

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_RICHTEXT && wxUSE_BUTTON
