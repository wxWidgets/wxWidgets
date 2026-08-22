///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuigenericfeedback.cpp
// Purpose:     WinUI qualification of generic banner/animation fallbacks
// Author:      wxWidgets development team
// Created:     2026-08-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && \
        (wxUSE_BANNERWINDOW || wxUSE_ANIMATIONCTRL)

#include "wx/app.h"
#include "wx/artprov.h"
#include "wx/frame.h"
#include "wx/image.h"
#include "wx/log.h"
#include "wx/mstream.h"
#include "wx/panel.h"
#include "wx/timer.h"
#include "wx/utils.h"
#include "wx/weakref.h"

#if wxUSE_BANNERWINDOW
    #include "wx/bannerwindow.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
#endif

#if wxUSE_ANIMATIONCTRL
    #include "wx/animate.h"
#endif

#if wxUSE_XRC
    #include "wx/sstream.h"
    #include "wx/xml/xml.h"
    #include "wx/xrc/xmlres.h"
#endif

#include <initializer_list>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace
{

template <typename Predicate>
bool DrainGenericFeedbackUntil(Predicate predicate, unsigned rounds = 160)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;

        wxTheApp->ProcessPendingEvents();
        wxTheApp->ProcessIdle();
        wxMilliSleep(2);
    }

    return predicate();
}

std::unique_ptr<wxFrame>
CreateGenericFeedbackFrame(const wxString& title, int ordinal)
{
    std::unique_ptr<wxFrame> frame(new wxFrame(
        nullptr, wxID_ANY, title,
        wxPoint(-32000 + ordinal * 320, -32000), wxSize(320, 240)));
    frame->ShowWithoutActivating();
    return frame;
}

#if wxUSE_XRC

class XrcUnloadGuard final
{
public:
    XrcUnloadGuard(wxXmlResource* resource, const wxString& name)
        : m_resource(resource), m_name(name)
    {
    }

    ~XrcUnloadGuard()
    {
        if ( m_resource )
            (void)m_resource->Unload(m_name);
    }

private:
    wxXmlResource* const m_resource;
    const wxString m_name;

    wxDECLARE_NO_COPY_CLASS(XrcUnloadGuard);
};

#endif // wxUSE_XRC

#if wxUSE_ANIMATIONCTRL

std::function<void()> gs_genericFeedbackGetDelayHook;
std::function<void()> gs_genericFeedbackConvertHook;
std::function<void()> gs_genericFeedbackLoadHook;

void InvokeGenericFeedbackDecoderHook(std::function<void()>& hook)
{
    std::function<void()> action = std::move(hook);
    hook = std::function<void()>();
    if ( action )
        action();
}

// A private, deterministic decoder lets this test exercise the decoder
// contracts that the checked-in ANI happens not to contain: zero-delay and
// forever (-1) frames, two resolutions, and a load failure after CanRead().
// It is registered only for wxANIMATION_TYPE_INVALID for this test's scope.
class GenericFeedbackDecoder final : public wxAnimationDecoder
{
public:
    ~GenericFeedbackDecoder() override = default;

    wxNODISCARD wxAnimationDecoder* Clone() const override
    {
        return new GenericFeedbackDecoder;
    }

    wxAnimationType GetType() const override
    {
        return wxANIMATION_TYPE_INVALID;
    }

    bool Load(wxInputStream& stream) override
    {
        unsigned char header[7];
        stream.Read(header, sizeof(header));
        if ( stream.LastRead() != sizeof(header) ||
                header[0] != 'W' || header[1] != 'X' ||
                header[2] != 'A' || header[3] != 'F' ||
                header[4] == 0 || header[5] == 0 || header[6] == 0 )
        {
            return false;
        }

        std::vector<long> delays;
        delays.reserve(header[6]);
        for ( unsigned int i = 0; i < header[6]; ++i )
        {
            unsigned char encodedDelay = 0;
            stream.Read(&encodedDelay, 1);
            if ( stream.LastRead() != 1 )
                return false;

            delays.push_back(encodedDelay == 0xff
                                 ? -1
                                 : static_cast<long>(encodedDelay));
        }

        std::vector<wxImage> frames;
        frames.reserve(header[6]);
        for ( unsigned int i = 0; i < header[6]; ++i )
        {
            wxImage frame(header[4], header[5]);
            if ( !frame.IsOk() )
                return false;

            frame.SetRGB(
                wxRect(0, 0, header[4], header[5]),
                static_cast<unsigned char>(32 + (i * 53) % 192),
                static_cast<unsigned char>(64 + (i * 71) % 160),
                static_cast<unsigned char>(96 + (i * 37) % 128));
            frames.push_back(frame);
        }

        InvokeGenericFeedbackDecoderHook(gs_genericFeedbackLoadHook);

        m_szAnimation = wxSize(header[4], header[5]);
        m_nFrames = header[6];
        m_background = *wxBLACK;
        m_delays.swap(delays);
        m_frames.swap(frames);
        return true;
    }

    bool ConvertToImage(unsigned int frame, wxImage* image) const override
    {
        if ( !image || frame >= m_frames.size() )
            return false;

        InvokeGenericFeedbackDecoderHook(gs_genericFeedbackConvertHook);
        *image = m_frames[frame];
        return image->IsOk();
    }

    wxSize GetFrameSize(unsigned int WXUNUSED(frame)) const override
    {
        return m_szAnimation;
    }

    wxPoint GetFramePosition(unsigned int WXUNUSED(frame)) const override
    {
        return wxPoint(0, 0);
    }

    wxAnimationDisposal
    GetDisposalMethod(unsigned int WXUNUSED(frame)) const override
    {
        return wxANIM_TOBACKGROUND;
    }

    long GetDelay(unsigned int frame) const override
    {
        InvokeGenericFeedbackDecoderHook(gs_genericFeedbackGetDelayHook);
        return m_delays[frame];
    }

    wxColour GetTransparentColour(unsigned int WXUNUSED(frame)) const override
    {
        return wxNullColour;
    }

protected:
    bool DoCanRead(wxInputStream& stream) const override
    {
        unsigned char magic[4];
        stream.Read(magic, sizeof(magic));
        return stream.LastRead() == sizeof(magic) &&
               magic[0] == 'W' && magic[1] == 'X' &&
               magic[2] == 'A' && magic[3] == 'F';
    }

private:
    std::vector<long> m_delays;
    std::vector<wxImage> m_frames;
};

class GenericFeedbackDecoderGuard final
{
public:
    GenericFeedbackDecoderGuard() = default;

    bool Install()
    {
        if ( wxAnimation::FindHandler(wxANIMATION_TYPE_INVALID) )
            return false;

        m_handler = new GenericFeedbackDecoder;
        wxAnimation::AddHandler(m_handler);
        return wxAnimation::FindHandler(wxANIMATION_TYPE_INVALID) ==
               m_handler;
    }

    bool HookNextGetDelay(std::function<void()> hook)
    {
        if ( gs_genericFeedbackGetDelayHook )
            return false;
        gs_genericFeedbackGetDelayHook = std::move(hook);
        return true;
    }

    bool HookNextConvertToImage(std::function<void()> hook)
    {
        if ( gs_genericFeedbackConvertHook )
            return false;
        gs_genericFeedbackConvertHook = std::move(hook);
        return true;
    }

    bool HookNextLoad(std::function<void()> hook)
    {
        if ( gs_genericFeedbackLoadHook )
            return false;
        gs_genericFeedbackLoadHook = std::move(hook);
        return true;
    }

    ~GenericFeedbackDecoderGuard()
    {
        gs_genericFeedbackGetDelayHook = std::function<void()>();
        gs_genericFeedbackConvertHook = std::function<void()>();
        gs_genericFeedbackLoadHook = std::function<void()>();
        if ( m_handler &&
                wxAnimation::GetHandlers().DeleteObject(m_handler) )
        {
            delete m_handler;
        }
    }

private:
    GenericFeedbackDecoder* m_handler = nullptr;

    wxDECLARE_NO_COPY_CLASS(GenericFeedbackDecoderGuard);
};

class GenericFeedbackAnimationCtrlForTesting final : public wxAnimationCtrl
{
public:
    GenericFeedbackAnimationCtrlForTesting(wxWindow* parent,
                                           const wxSize& size)
        : wxAnimationCtrl(parent, wxID_ANY, wxNullAnimation,
                          wxDefaultPosition, size, wxAC_NO_AUTORESIZE)
    {
    }

    void DispatchDPIChangedForTesting(wxDPIChangedEvent& event)
    {
        WXHandleDPIChanged(event);
    }
};

std::vector<unsigned char>
MakeGenericFeedbackAnimationData(const wxSize& size,
                                 std::initializer_list<int> delays)
{
    std::vector<unsigned char> data =
    {
        'W', 'X', 'A', 'F',
        static_cast<unsigned char>(size.x),
        static_cast<unsigned char>(size.y),
        static_cast<unsigned char>(delays.size())
    };
    for ( const int delay : delays )
        data.push_back(static_cast<unsigned char>(delay));
    return data;
}

wxAnimation
LoadGenericFeedbackAnimation(wxAnimationCtrlBase& control,
                             const wxSize& size,
                             std::initializer_list<int> delays)
{
    const std::vector<unsigned char> data =
        MakeGenericFeedbackAnimationData(size, delays);
    wxMemoryInputStream stream(data.data(), data.size());
    wxAnimation animation = control.CreateAnimation();
    (void)animation.Load(stream, wxANIMATION_TYPE_INVALID);
    return animation;
}

void DispatchAnimationTimer(wxGenericAnimationCtrl& control, int timerId)
{
    wxTimer timer(&control, timerId);
    wxTimerEvent event(timer);
    REQUIRE(control.ProcessWindowEvent(event));
}

void PaintAndRequireArmed(wxGenericAnimationCtrl& control)
{
    control.Update();
    REQUIRE(control.IsPlaying());
    REQUIRE(control.WinUIIsTimerRunningForTesting());
}

#endif // wxUSE_ANIMATIONCTRL

} // anonymous namespace

#if wxUSE_BANNERWINDOW

TEST_CASE("WinUIGenericFeedback::BannerFallbackContracts",
          "[winui-beta-aux][winui-beta-feedback][bannerwindow]")
{
    std::unique_ptr<wxFrame> frame =
        CreateGenericFeedbackFrame("banner fallback", 0);
    wxPanel* const firstParent = new wxPanel(frame.get(), wxID_ANY);
    wxPanel* const secondParent = new wxPanel(frame.get(), wxID_ANY);
    firstParent->SetSize(frame->GetClientSize());
    secondParent->SetSize(frame->GetClientSize());

    wxBitmap bitmap1x;
    wxBitmap bitmap2x;
    REQUIRE(bitmap1x.CreateWithDIPSize(wxSize(24, 12), 1.0, 32));
    REQUIRE(bitmap2x.CreateWithDIPSize(wxSize(24, 12), 2.0, 32));
    {
        wxMemoryDC dc(bitmap1x);
        dc.SetBackground(*wxBLUE_BRUSH);
        dc.Clear();
    }
    {
        wxMemoryDC dc(bitmap2x);
        dc.SetBackground(*wxRED_BRUSH);
        dc.Clear();
    }
    const wxBitmapBundle bitmapBundle =
        wxBitmapBundle::FromBitmaps(bitmap1x, bitmap2x);
    REQUIRE(bitmapBundle.IsOk());

    // A client larger than the source bitmap in both axes used to leave one
    // perpendicular strip untouched. Verify actual painted pixels at all four
    // corners for every orientation, including the direction-specific gap.
    const wxDirection paintDirections[] =
        { wxTOP, wxBOTTOM, wxLEFT, wxRIGHT };
    for ( const wxDirection direction : paintDirections )
    {
        wxBannerWindow painted;
        REQUIRE(painted.Create(firstParent, wxID_ANY, direction));
        painted.SetBackgroundColour(*wxGREEN);
        painted.SetSize(0, 0, 41, 29);
        painted.SetBitmap(bitmapBundle);
        painted.SetText(wxString(), wxString());
        wxBitmap rendered;
        REQUIRE(rendered.Create(41, 29, 32));
        {
            wxMemoryDC dc;
            dc.SelectObject(rendered);
            dc.SetBackground(*wxWHITE_BRUSH);
            dc.Clear();
            painted.WinUIDrawBitmapBackgroundForTesting(dc);
            dc.SelectObject(wxNullBitmap);
        }

        const wxImage selected =
            bitmapBundle.GetBitmapFor(&painted).ConvertToImage();
        REQUIRE(selected.IsOk());
        const wxImage actualImage = rendered.ConvertToImage();
        REQUIRE(actualImage.IsOk());
        const wxColour expected(selected.GetRed(0, 0),
                                selected.GetGreen(0, 0),
                                selected.GetBlue(0, 0));
        const wxPoint probes[] =
        {
            wxPoint(0, 0), wxPoint(40, 0),
            wxPoint(0, 28), wxPoint(40, 28)
        };
        for ( const wxPoint& point : probes )
        {
            INFO("banner direction=" << static_cast<int>(direction) <<
                 ", pixel=" << point.x << "," << point.y);
            const wxColour actual(actualImage.GetRed(point.x, point.y),
                                  actualImage.GetGreen(point.x, point.y),
                                  actualImage.GetBlue(point.x, point.y));
            CHECK(actual.Red() == expected.Red());
            CHECK(actual.Green() == expected.Green());
            CHECK(actual.Blue() == expected.Blue());
        }
    }

    wxBannerWindow* const banner = new wxBannerWindow();
    REQUIRE(banner->Create(firstParent, wxID_ANY, wxTOP));
    banner->SetText("Accessible title", "A localized two-line\nmessage");
    CHECK(banner->GetLabel() ==
          "Accessible title\nA localized two-line\nmessage");
    const wxSize textBest = banner->GetBestSize();
    CHECK(textBest.x > 0);
    CHECK(textBest.y > 0);

    // Exercise the canonical text-only and bitmap-only modes separately. The
    // empty SetText() mirrors the unconditional final call made by the XRC
    // handler and must not erase a bitmap it just installed.
    banner->SetText(wxString(), wxString());
    banner->SetBitmap(bitmapBundle);
    const wxSize bitmapBest = banner->GetBestSize();
    banner->SetText(wxString(), wxString());
    CHECK(banner->GetLabel().empty());
    CHECK(banner->GetBestSize() == bitmapBest);

    banner->SetBitmap(wxBitmapBundle());
    banner->SetText("Accessible title", "A localized two-line\nmessage");
    CHECK(banner->GetBestSize() == textBest);

    // Invalid/empty bitmap, gradient repaint, explicit resize and the ordinary
    // window state contract must all remain safe for the custom fallback.
    banner->SetBitmap(wxBitmapBundle());
    banner->SetText("Gradient title", "Long deterministic gradient message");
    banner->SetGradient(wxColour(12, 34, 56), wxColour(210, 220, 230));
    banner->SetMinSize(wxSize(80, 40));
    CHECK(banner->GetMinSize() == wxSize(80, 40));
    banner->SetSize(wxSize(210, 72));
    CHECK(banner->GetClientSize().x > 0);
    CHECK(banner->GetClientSize().y > 0);
    banner->Disable();
    CHECK_FALSE(banner->IsEnabled());
    banner->Enable();
    CHECK(banner->IsEnabled());
    banner->Hide();
    CHECK_FALSE(banner->IsShown());
    banner->Show();
    CHECK(banner->IsShown());
    banner->Update();

    // Exercise the DPI invalidation path with a real 1x/2x bitmap bundle. Its
    // logical best size remains stable even though the selected raster can
    // change when the native DPI transition updates the window.
    banner->SetBitmap(bitmapBundle);
    banner->SetText(wxString(), wxString());
    const wxSize logicalBundleSize = banner->GetBestSize();
    wxDPIChangedEvent dpiEvent(wxSize(96, 96), wxSize(192, 192));
    dpiEvent.SetEventObject(banner);
    dpiEvent.SetId(banner->GetId());
    banner->ProcessWindowEvent(dpiEvent);
    CHECK(banner->GetBestSize() == logicalBundleSize);

    REQUIRE(banner->Reparent(secondParent));
    CHECK(banner->GetParent() == secondParent);

    for ( int i = 0; i < 100; ++i )
    {
        wxBannerWindow cycle;
        INFO("banner cycle=" << i);
        const wxDirection directions[] =
            { wxLEFT, wxRIGHT, wxTOP, wxBOTTOM };
        REQUIRE(cycle.Create(
            (i % 2) ? firstParent : secondParent,
            wxID_ANY, directions[i % 4]));
        if ( i % 2 )
        {
            cycle.SetText(wxString::Format("Title %d", i),
                          "Long deterministic banner text");
        }
        else
        {
            cycle.SetBitmap(bitmapBundle);
            cycle.SetText(wxString(), wxString());
        }
        cycle.SetGradient(*wxWHITE, *wxBLACK);
        cycle.SetSize(wxSize(90 + i % 17, 44 + i % 11));
        cycle.Show(i % 3 != 0);
        cycle.Enable(i % 5 != 0);
        REQUIRE(cycle.GetBestSize().x > 0);
        REQUIRE(cycle.GetBestSize().y > 0);
    }

    wxBannerWindow invalidDirection;
#if wxDEBUG_LEVEL
    WX_ASSERT_FAILS_WITH_ASSERT(
        invalidDirection.Create(firstParent, wxID_ANY, wxALL));
#else
    CHECK_FALSE(invalidDirection.Create(firstParent, wxID_ANY, wxALL));
#endif

    wxBannerWindow* const doomed =
        new wxBannerWindow(secondParent, wxBOTTOM);
    doomed->SetBitmap(bitmapBundle);
    doomed->SetText(wxString(), wxString());
    const wxWeakRef<wxWindow> weakDoomed(doomed);
    doomed->Destroy();
    REQUIRE(DrainGenericFeedbackUntil(
        [weakDoomed]() { return !weakDoomed; }));

#if wxUSE_XRC
    wxBitmapBundle stockBitmap;
    REQUIRE(DrainGenericFeedbackUntil(
        [&stockBitmap]()
        {
            stockBitmap = wxArtProvider::GetBitmapBundle(
                wxART_INFORMATION, wxART_OTHER);
            return stockBitmap.IsOk();
        }));

    static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxBannerWindow" name="phase013FeedbackBitmapBanner">
    <direction>wxTOP</direction>
    <bitmap stock_id="wxART_INFORMATION" stock_client="wxART_OTHER"/>
  </object>
  <object class="wxBannerWindow" name="phase013FeedbackTextBanner">
    <direction>wxRIGHT</direction>
    <title>XRC title</title>
    <message>XRC text-only message</message>
  </object>
</resource>
)XRC";
    static const wxString xrcName = "phase013-feedback-banner.xrc";
    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream xrcStream(wxString::FromUTF8(xrcText));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(xrcStream));
    REQUIRE(document->IsOk());
    REQUIRE(xrc->LoadDocument(document.release(), xrcName));
    XrcUnloadGuard unload(xrc, xrcName);

    std::unique_ptr<wxBannerWindow> xrcBitmapBanner(wxDynamicCast(
        xrc->LoadObject(frame.get(),
                        "phase013FeedbackBitmapBanner",
                        "wxBannerWindow"),
        wxBannerWindow));
    REQUIRE(xrcBitmapBanner);
    CHECK(xrcBitmapBanner->GetLabel().empty());
    const wxSize expectedBitmapSize =
        stockBitmap.GetPreferredLogicalSizeFor(xrcBitmapBanner.get());
    CHECK(xrcBitmapBanner->GetBestSize() == expectedBitmapSize);

    std::unique_ptr<wxBannerWindow> xrcTextBanner(wxDynamicCast(
        xrc->LoadObject(frame.get(),
                        "phase013FeedbackTextBanner",
                        "wxBannerWindow"),
        wxBannerWindow));
    REQUIRE(xrcTextBanner);
    CHECK(xrcTextBanner->GetLabel() ==
          "XRC title\nXRC text-only message");
    CHECK(xrcTextBanner->GetBestSize().x > 0);
    CHECK(xrcTextBanner->GetBestSize().y > 0);
#endif // wxUSE_XRC
}

#endif // wxUSE_BANNERWINDOW

#if wxUSE_ANIMATIONCTRL

TEST_CASE("WinUIGenericFeedback::AnimationFallbackTransactions",
          "[winui-beta-aux][winui-beta-feedback][animationctrl]")
{
    GenericFeedbackDecoderGuard decoderGuard;
    REQUIRE(decoderGuard.Install());

    std::unique_ptr<wxFrame> frame =
        CreateGenericFeedbackFrame("animation fallback", 1);
    GenericFeedbackAnimationCtrlForTesting* const control =
        new GenericFeedbackAnimationCtrlForTesting(
            frame.get(), wxSize(120, 100));

    // Both direct wxAnimation loading and the control setter are
    // transactional: malformed input with a valid signature doesn't destroy
    // the previously decoded animation.
    REQUIRE(control->LoadFile("horse.ani", wxANIMATION_TYPE_ANI));
    REQUIRE(control->GetAnimation().IsOk());
    REQUIRE(control->GetAnimation().GetFrameCount() > 1);

    wxAnimation transactional = control->CreateAnimation();
    REQUIRE(transactional.LoadFile("horse.ani", wxANIMATION_TYPE_ANI));
    const unsigned int transactionalFrames = transactional.GetFrameCount();
    const wxSize transactionalSize = transactional.GetSize();
    const std::vector<unsigned char> truncated =
        { 'W', 'X', 'A', 'F', 8, 8, 2, 25 };
    wxMemoryInputStream directInvalidStream(
        truncated.data(), truncated.size());
    {
        wxLogNull suppressExpectedDecoderError;
        CHECK_FALSE(transactional.Load(
            directInvalidStream, wxANIMATION_TYPE_INVALID));
    }
    CHECK(transactional.IsOk());
    CHECK(transactional.GetFrameCount() == transactionalFrames);
    CHECK(transactional.GetSize() == transactionalSize);

    wxMemoryInputStream invalidControlStream(
        truncated.data(), truncated.size());
    {
        wxLogNull suppressExpectedDecoderError;
        CHECK_FALSE(control->Load(
            invalidControlStream, wxANIMATION_TYPE_INVALID));
    }
    CHECK(control->GetAnimation().IsOk());
    CHECK(control->GetAnimation().GetFrameCount() > 1);

    const wxAnimation timed = LoadGenericFeedbackAnimation(
        *control, wxSize(12, 10), { 100, 100 });
    const wxAnimation zeroThenForever = LoadGenericFeedbackAnimation(
        *control, wxSize(12, 10), { 0, -1 });
    const wxAnimation forever = LoadGenericFeedbackAnimation(
        *control, wxSize(9, 7), { -1, 100 });
    REQUIRE(timed.IsOk());
    REQUIRE(zeroThenForever.IsOk());
    REQUIRE(forever.IsOk());

    // A nested publication from the decoder's Load() hook is the last writer:
    // the outer load reports that its candidate was not installed and leaves
    // the nested animation intact.
    control->SetAnimation(forever);
    int nestedLoadCount = 0;
    REQUIRE(decoderGuard.HookNextLoad(
        [&]()
        {
            ++nestedLoadCount;
            control->SetAnimation(timed);
        }));
    const std::vector<unsigned char> nestedLoadData =
        MakeGenericFeedbackAnimationData(wxSize(18, 13), { 20, 20 });
    wxMemoryInputStream nestedLoadStream(
        nestedLoadData.data(), nestedLoadData.size());
    CHECK_FALSE(control->Load(
        nestedLoadStream, wxANIMATION_TYPE_INVALID));
    CHECK(nestedLoadCount == 1);
    CHECK(control->GetAnimation().IsSameAs(timed));

    // Decoder methods are extension boundaries. A newer operation issued
    // from ConvertToImage() must win without the outer render publishing its
    // stale bitmap or state.
    control->SetAnimation(timed);
    bool convertStopped = false;
    REQUIRE(decoderGuard.HookNextConvertToImage(
        [&]()
        {
            convertStopped = true;
            control->Stop();
        }));
    CHECK_FALSE(control->Play(false));
    CHECK(convertStopped);
    CHECK_FALSE(control->IsPlaying());
    CHECK(control->GetAnimation().GetSize() == timed.GetSize());
    CHECK_FALSE(control->WinUIIsTimerRunningForTesting());

    control->SetAnimation(timed);
    bool nestedPlayResult = false;
    REQUIRE(decoderGuard.HookNextConvertToImage(
        [&]() { nestedPlayResult = control->Play(false); }));
    CHECK(control->Play(false));
    CHECK(nestedPlayResult);
    CHECK(control->IsPlaying());
    control->Refresh();
    PaintAndRequireArmed(*control);
    control->Stop();

    control->SetAnimation(timed);
    bool animationReplaced = false;
    REQUIRE(decoderGuard.HookNextConvertToImage(
        [&]()
        {
            animationReplaced = true;
            control->SetAnimation(forever);
        }));
    CHECK_FALSE(control->Play(false));
    CHECK(animationReplaced);
    CHECK_FALSE(control->IsPlaying());
    CHECK(control->GetAnimation().GetSize() == forever.GetSize());

    // GetDelay() has the same rule: Stop() from the callback must not be
    // followed by an outer StartOnce() using the retired generation.
    control->SetAnimation(timed);
    REQUIRE(control->Play(false));
    bool delayStopped = false;
    REQUIRE(decoderGuard.HookNextGetDelay(
        [&]()
        {
            delayStopped = true;
            control->Stop();
        }));
    control->Update();
    CHECK(delayStopped);
    CHECK_FALSE(control->IsPlaying());
    CHECK_FALSE(control->WinUIIsTimerRunningForTesting());

    // Non-looping consumes both frame delays, then stops and resets.
    control->SetAnimation(timed);
    REQUIRE(control->Play(false));
    PaintAndRequireArmed(*control);
    int timerId = control->WinUIGetTimerIdForTesting();
    DispatchAnimationTimer(*control, timerId);
    CHECK(control->WinUIGetCurrentFrameForTesting() == 1);
    PaintAndRequireArmed(*control);
    timerId = control->WinUIGetTimerIdForTesting();
    DispatchAnimationTimer(*control, timerId);
    CHECK_FALSE(control->IsPlaying());
    CHECK(control->WinUIGetCurrentFrameForTesting() == 0);
    CHECK_FALSE(control->WinUIIsTimerRunningForTesting());

    // Looping advances through the same terminal boundary and restarts at 0.
    REQUIRE(control->Play(true));
    PaintAndRequireArmed(*control);
    DispatchAnimationTimer(
        *control, control->WinUIGetTimerIdForTesting());
    PaintAndRequireArmed(*control);
    DispatchAnimationTimer(
        *control, control->WinUIGetTimerIdForTesting());
    CHECK(control->IsPlaying());
    CHECK(control->WinUIGetCurrentFrameForTesting() == 0);
    control->Update();
    CHECK(control->WinUIIsTimerRunningForTesting());
    control->Stop();

    // Delay 0 is clamped to a real timer; delay -1 holds the selected frame
    // indefinitely without polling while preserving the stoppable state.
    control->SetAnimation(zeroThenForever);
    REQUIRE(control->Play(false));
    PaintAndRequireArmed(*control);
    CHECK(control->WinUIGetTimerIntervalForTesting() == 10);
    DispatchAnimationTimer(
        *control, control->WinUIGetTimerIdForTesting());
    CHECK(control->WinUIGetCurrentFrameForTesting() == 1);
    control->Update();
    CHECK(control->IsPlaying());
    CHECK_FALSE(control->WinUIIsTimerRunningForTesting());
    control->Stop();
    CHECK_FALSE(control->IsPlaying());

    control->SetAnimation(forever);
    REQUIRE(control->Play());
    control->Update();
    CHECK(control->IsPlaying());
    CHECK(control->WinUIGetCurrentFrameForTesting() == 0);
    CHECK_FALSE(control->WinUIIsTimerRunningForTesting());
    control->Stop();

    // A notification from an earlier generation cannot advance a restarted
    // run and cannot consume its newly armed timer.
    control->SetAnimation(timed);
    REQUIRE(control->Play());
    PaintAndRequireArmed(*control);
    const int staleTimerId = control->WinUIGetTimerIdForTesting();
    control->Stop();
    REQUIRE(control->Play());
    PaintAndRequireArmed(*control);
    REQUIRE(control->WinUIGetTimerIdForTesting() != staleTimerId);
    DispatchAnimationTimer(*control, staleTimerId);
    CHECK(control->WinUIGetCurrentFrameForTesting() == 0);
    CHECK(control->IsPlaying());
    CHECK(control->WinUIIsTimerRunningForTesting());
    control->Stop();

    // Resize rebuilds the exact clipped backing dimensions while stopped and
    // while active; it must not retain a grow-only surface from an old size.
    control->SetClientSize(wxSize(0, 0));
    REQUIRE(control->GetClientSize() == wxSize(0, 0));
    control->SetAnimation(timed);
    CHECK(control->GetAnimation().IsOk());
    CHECK(control->GetAnimation().GetSize() == timed.GetSize());
    CHECK_FALSE(control->GetBackingStore().IsOk());
    control->SetClientSize(wxSize(7, 6));
    REQUIRE(control->GetClientSize() == wxSize(7, 6));
    CHECK(control->GetBackingStore().GetSize() == wxSize(7, 6));
    REQUIRE(control->Play());
    PaintAndRequireArmed(*control);
    control->SetClientSize(wxSize(5, 4));
    REQUIRE(control->GetClientSize() == wxSize(5, 4));
    CHECK(control->GetBackingStore().GetSize() == wxSize(5, 4));

    // A transient zero-sized layout pauses rather than invalidating the
    // decoded animation. Restoring a positive client resumes one schedule.
    control->SetClientSize(wxSize(0, 0));
    REQUIRE(control->GetClientSize() == wxSize(0, 0));
    CHECK(control->GetAnimation().IsOk());
    CHECK(control->IsPlaying());
    CHECK(control->WinUIIsPausedForHiddenForTesting());
    CHECK_FALSE(control->WinUIIsTimerRunningForTesting());
    control->SetClientSize(wxSize(5, 4));
    REQUIRE(control->GetClientSize() == wxSize(5, 4));
    CHECK(control->GetAnimation().IsOk());
    CHECK(control->GetBackingStore().GetSize() == wxSize(5, 4));
    control->Update();
    CHECK(control->IsPlaying());
    CHECK_FALSE(control->WinUIIsPausedForHiddenForTesting());
    CHECK(control->WinUIIsTimerRunningForTesting());

    control->Hide();
    CHECK(control->IsPlaying());
    CHECK(control->WinUIIsPausedForHiddenForTesting());
    CHECK_FALSE(control->WinUIIsTimerRunningForTesting());
    REQUIRE(control->Show());
    REQUIRE(DrainGenericFeedbackUntil(
        [control]()
        {
            control->Update();
            return control->IsPlaying() &&
                   !control->WinUIIsPausedForHiddenForTesting() &&
                   control->WinUIIsTimerRunningForTesting();
        }));
    control->Stop();

    // Exercise bundle selection and reselection through the DPI event. The
    // backing store must use the selected raster's native pixels exactly,
    // without applying the window scale for a second time.
    const wxAnimation oneX = LoadGenericFeedbackAnimation(
        *control, wxSize(8, 8), { 100, 100 });
    const wxAnimation twoX = LoadGenericFeedbackAnimation(
        *control, wxSize(16, 16), { 100, 100 });
    REQUIRE(oneX.IsOk());
    REQUIRE(twoX.IsOk());
    wxAnimationBundle animationBundle;
    animationBundle.Add(oneX);
    animationBundle.Add(twoX);
    control->SetClientSize(wxSize(40, 40));
    control->SetAnimation(animationBundle);
    const wxSize wanted = oneX.GetSize() * control->GetDPIScaleFactor();
    const wxSize expectedSelected =
        oneX.GetSize().IsAtLeast(wanted) ? oneX.GetSize() : twoX.GetSize();
    CHECK(control->GetAnimation().GetSize() == expectedSelected);
    CHECK(control->GetBackingStore().GetSize() == expectedSelected);
    CHECK(control->WinUIGetPreferredAnimationSizeForScaleForTesting(2.0) ==
          twoX.GetSize());

    wxDPIChangedEvent dpiEvent(wxSize(96, 96), wxSize(192, 192));
    dpiEvent.SetEventObject(control);
    dpiEvent.SetId(control->GetId());
    control->DispatchDPIChangedForTesting(dpiEvent);
    REQUIRE(control->GetAnimation().IsOk());
    CHECK(control->GetAnimation().GetSize() == expectedSelected);
    REQUIRE(control->Play());
    PaintAndRequireArmed(*control);
    control->Stop();

    // DPI replay is also a transaction. If frame materialization publishes a
    // newer animation, the outer handler must neither overwrite it nor resume
    // the pre-DPI playing state.
    control->SetAnimation(timed);
    REQUIRE(control->Play(true));
    int nestedDpiCount = 0;
    REQUIRE(decoderGuard.HookNextConvertToImage(
        [&]()
        {
            ++nestedDpiCount;
            control->SetAnimation(forever);
        }));
    wxDPIChangedEvent nestedDpiEvent(wxSize(96, 96), wxSize(144, 144));
    nestedDpiEvent.SetEventObject(control);
    nestedDpiEvent.SetId(control->GetId());
    control->DispatchDPIChangedForTesting(nestedDpiEvent);
    CHECK(nestedDpiCount == 1);
    CHECK(control->GetAnimation().IsSameAs(forever));
    CHECK_FALSE(control->IsPlaying());
    CHECK_FALSE(control->WinUIIsTimerRunningForTesting());

    // Every stress iteration now reaches a genuinely armed native timer
    // before revoking it, then delivers the retired generation explicitly.
    control->SetClientSize(wxSize(20, 20));
    control->SetAnimation(timed);
    for ( int i = 0; i < 1000; ++i )
    {
        INFO("animation cycle=" << i);
        REQUIRE(control->Play());
        PaintAndRequireArmed(*control);
        const int retiredId = control->WinUIGetTimerIdForTesting();
        control->Stop();
        REQUIRE_FALSE(control->IsPlaying());
        REQUIRE_FALSE(control->WinUIIsTimerRunningForTesting());
        DispatchAnimationTimer(*control, retiredId);
        REQUIRE(control->WinUIGetCurrentFrameForTesting() == 0);
    }

#if wxUSE_XRC
    wxBitmapBundle stockInactiveBitmap;
    REQUIRE(DrainGenericFeedbackUntil(
        [&stockInactiveBitmap]()
        {
            stockInactiveBitmap = wxArtProvider::GetBitmapBundle(
                wxART_INFORMATION, wxART_OTHER);
            return stockInactiveBitmap.IsOk();
        }));

    static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxAnimationCtrl" name="phase013FeedbackAnimation">
    <size>64,64</size>
    <style>wxAC_NO_AUTORESIZE</style>
    <animation>horse.ani</animation>
    <inactive-bitmap stock_id="wxART_INFORMATION" stock_client="wxART_OTHER"/>
  </object>
</resource>
)XRC";
    static const wxString xrcName = "phase013-feedback-animation.xrc";
    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream xrcStream(wxString::FromUTF8(xrcText));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(xrcStream));
    REQUIRE(document->IsOk());
    REQUIRE(xrc->LoadDocument(document.release(), xrcName));
    XrcUnloadGuard unload(xrc, xrcName);

    std::unique_ptr<wxAnimationCtrl> xrcControl(wxDynamicCast(
        xrc->LoadObject(frame.get(),
                        "phase013FeedbackAnimation",
                        "wxAnimationCtrl"),
        wxAnimationCtrl));
    REQUIRE(xrcControl);
    REQUIRE(xrcControl->GetAnimation().IsOk());
    REQUIRE(xrcControl->GetInactiveBitmap().IsOk());
    REQUIRE(xrcControl->Play());
    PaintAndRequireArmed(*xrcControl);
    xrcControl->Stop();
#endif // wxUSE_XRC

    // Destruction is tested only after paint has armed a real timer. The frame
    // is RAII-owned, so any earlier REQUIRE also tears down the control tree.
    control->SetAnimation(timed);
    REQUIRE(control->Play());
    PaintAndRequireArmed(*control);
    const wxWeakRef<wxWindow> weakControl(control);
    control->Destroy();
    REQUIRE(DrainGenericFeedbackUntil(
        [weakControl]() { return !weakControl; }));
}

TEST_CASE("WinUIGenericFeedback::AnimationDecoderSelfDestruction",
          "[winui-beta-aux][winui-beta-feedback][animationctrl]")
{
    GenericFeedbackDecoderGuard decoderGuard;
    REQUIRE(decoderGuard.Install());

    std::unique_ptr<wxFrame> frame =
        CreateGenericFeedbackFrame("animation self destruction", 2);
    GenericFeedbackAnimationCtrlForTesting* const loader =
        new GenericFeedbackAnimationCtrlForTesting(
            frame.get(), wxSize(40, 40));
    const wxAnimation timed = LoadGenericFeedbackAnimation(
        *loader, wxSize(12, 10), { 100, 100 });
    REQUIRE(timed.IsOk());

    // GetFrame()/ConvertToImage() runs while Play() owns only candidate state.
    // Synchronous wx destruction must make the outer Play() return without
    // touching the retired control or publishing the candidate bitmap.
    wxAnimationCtrl* frameVictim = new wxAnimationCtrl(
        frame.get(), wxID_ANY, timed, wxDefaultPosition,
        wxSize(30, 30), wxAC_NO_AUTORESIZE);
    const wxWeakRef<wxWindow> weakFrameVictim(frameVictim);
    int frameDeleteCount = 0;
    REQUIRE(decoderGuard.HookNextConvertToImage(
        [&]()
        {
            ++frameDeleteCount;
            wxAnimationCtrl* const doomed = frameVictim;
            frameVictim = nullptr;
            doomed->Destroy();
        }));
    wxAnimationCtrl* const frameCaller = frameVictim;
    CHECK_FALSE(frameCaller->Play(false));
    CHECK(frameDeleteCount == 1);
    CHECK_FALSE(weakFrameVictim);

    // Load() retains its candidate decoder but not its owner. A decoder may
    // synchronously delete the control and the outer load must simply fail.
    wxAnimationCtrl* loadVictim = new wxAnimationCtrl(
        frame.get(), wxID_ANY, wxNullAnimation, wxDefaultPosition,
        wxSize(30, 30), wxAC_NO_AUTORESIZE);
    const wxWeakRef<wxWindow> weakLoadVictim(loadVictim);
    int loadDeleteCount = 0;
    REQUIRE(decoderGuard.HookNextLoad(
        [&]()
        {
            ++loadDeleteCount;
            wxAnimationCtrl* const doomed = loadVictim;
            loadVictim = nullptr;
            doomed->Destroy();
        }));
    const std::vector<unsigned char> loadData =
        MakeGenericFeedbackAnimationData(wxSize(14, 11), { 25, 25 });
    wxMemoryInputStream loadStream(loadData.data(), loadData.size());
    wxAnimationCtrl* const loadCaller = loadVictim;
    CHECK_FALSE(loadCaller->Load(loadStream, wxANIMATION_TYPE_INVALID));
    CHECK(loadDeleteCount == 1);
    CHECK_FALSE(weakLoadVictim);

    // Exercise GetDelay() through the implementation seam, outside native
    // paint dispatch. Destroy() still deletes this child synchronously, while
    // avoiding a test-induced deletion of the HWND in the middle of WM_PAINT.
    wxAnimationCtrl* delayVictim = new wxAnimationCtrl(
        frame.get(), wxID_ANY, timed, wxDefaultPosition,
        wxSize(30, 30), wxAC_NO_AUTORESIZE);
    REQUIRE(delayVictim->Play(false));
    const wxWeakRef<wxWindow> weakDelayVictim(delayVictim);
    int delayDeleteCount = 0;
    REQUIRE(decoderGuard.HookNextGetDelay(
        [&]()
        {
            ++delayDeleteCount;
            wxAnimationCtrl* const doomed = delayVictim;
            delayVictim = nullptr;
            doomed->Destroy();
        }));
    wxAnimationCtrl* const delayCaller = delayVictim;
    CHECK_FALSE(delayCaller->WinUIScheduleNextFrameForTesting());
    CHECK(delayDeleteCount == 1);
    REQUIRE(DrainGenericFeedbackUntil(
        [weakDelayVictim]() { return !weakDelayVictim; }));

    // DPI replay traverses bundle selection and frame materialization.
    // Synchronous destruction from the frame callback must abort both
    // SetAnimation() and the outer replay before either can inspect the dead
    // object or restart it.
    GenericFeedbackAnimationCtrlForTesting* dpiVictim =
        new GenericFeedbackAnimationCtrlForTesting(
            frame.get(), wxSize(30, 30));
    dpiVictim->SetAnimation(timed);
    const wxWeakRef<wxWindow> weakDpiVictim(dpiVictim);
    int dpiDeleteCount = 0;
    REQUIRE(decoderGuard.HookNextConvertToImage(
        [&]()
        {
            ++dpiDeleteCount;
            GenericFeedbackAnimationCtrlForTesting* const doomed = dpiVictim;
            dpiVictim = nullptr;
            doomed->Destroy();
        }));
    wxDPIChangedEvent dpiEvent(wxSize(96, 96), wxSize(192, 192));
    dpiEvent.SetEventObject(dpiVictim);
    dpiEvent.SetId(dpiVictim->GetId());
    GenericFeedbackAnimationCtrlForTesting* const dpiCaller = dpiVictim;
    dpiCaller->DispatchDPIChangedForTesting(dpiEvent);
    CHECK(dpiDeleteCount == 1);
    CHECK_FALSE(weakDpiVictim);

    // All one-shot extension seams were consumed; no callback leaks into a
    // later test and the surviving control can still own and revoke a timer.
    CHECK_FALSE(gs_genericFeedbackGetDelayHook);
    CHECK_FALSE(gs_genericFeedbackConvertHook);
    CHECK_FALSE(gs_genericFeedbackLoadHook);
    loader->SetAnimation(timed);
    REQUIRE(loader->Play());
    PaintAndRequireArmed(*loader);
    loader->Stop();
    CHECK_FALSE(loader->WinUIIsTimerRunningForTesting());
}

#endif // wxUSE_ANIMATIONCTRL

#endif // __WXWINUI__ && wxUSE_WINUI3 && feature set
