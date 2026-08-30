///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiappearance.cpp
// Purpose:     deterministic WinUI appearance/static-control contract tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "button-test-access.h"

#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/bmpbuttn.h"
#include "wx/bmpbndl.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/commandlinkbutton.h"
#include "wx/log.h"
#include "wx/panel.h"
#include "wx/radiobox.h"
#include "wx/radiobut.h"
#include "wx/scopeguard.h"
#include "wx/sizer.h"
#include "wx/statbmp.h"
#include "wx/statbox.h"
#include "wx/stattext.h"
#include "wx/tglbtn.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"

#include <winrt/Windows.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

namespace
{

wxFont MakeTestFont()
{
    return wxFont(wxFontInfo(13)
                      .FaceName("Segoe UI")
                      .Bold()
                      .Italic());
}

void CheckFontLocals(const wxWinUIAppearanceSnapshot& snapshot,
                     bool expected)
{
    CHECK(snapshot.hasFontFamily == expected);
    CHECK(snapshot.hasFontSize == expected);
    CHECK(snapshot.hasFontWeight == expected);
    CHECK(snapshot.hasFontStyle == expected);
}

wxBitmapBundle MakeDPIBundle(int logicalSize = 16)
{
    wxBitmap one;
    wxBitmap two;
    REQUIRE(one.CreateWithDIPSize(
        wxSize(logicalSize, logicalSize), 1.0, 32));
    REQUIRE(two.CreateWithDIPSize(
        wxSize(logicalSize, logicalSize), 2.0, 32));
    return wxBitmapBundle::FromBitmaps(one, two);
}

MUXC::Primitives::ToggleButton GetTogglePeer(wxToggleButton *toggle)
{
    if ( !toggle )
        return nullptr;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(toggle);
    wxWinUISlot * const slot = host ? host->FindSlot(toggle) : nullptr;
    return slot
        ? slot->GetContent().try_as<MUXC::Primitives::ToggleButton>()
        : MUXC::Primitives::ToggleButton{ nullptr };
}

MUXC::TextBlock FindToggleText(
    const winrt::Windows::Foundation::IInspectable& content)
{
    if ( !content )
        return nullptr;
    if ( const auto text = content.try_as<MUXC::TextBlock>() )
        return text;
    if ( const auto panel = content.try_as<MUXC::Panel>() )
    {
        const auto children = panel.Children();
        for ( std::uint32_t i = 0; i < children.Size(); ++i )
        {
            if ( const auto text = FindToggleText(children.GetAt(i)) )
                return text;
        }
    }
    return nullptr;
}

MUXC::Image FindToggleImage(
    const winrt::Windows::Foundation::IInspectable& content)
{
    if ( !content )
        return nullptr;
    if ( const auto image = content.try_as<MUXC::Image>() )
        return image;
    if ( const auto panel = content.try_as<MUXC::Panel>() )
    {
        const auto children = panel.Children();
        for ( std::uint32_t i = 0; i < children.Size(); ++i )
        {
            if ( const auto image = FindToggleImage(children.GetAt(i)) )
                return image;
        }
    }
    return nullptr;
}

class TogglePropertyCallbackGuard final
{
public:
    TogglePropertyCallbackGuard(
        const MUX::DependencyObject& object,
        const MUX::DependencyProperty& property,
        int64_t token)
        : m_object(object),
          m_property(property),
          m_token(token)
    {
    }

    ~TogglePropertyCallbackGuard()
    {
        if ( m_object )
        {
            try
            {
                m_object.UnregisterPropertyChangedCallback(
                    m_property, m_token);
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
    }

private:
    MUX::DependencyObject m_object{ nullptr };
    MUX::DependencyProperty m_property{ nullptr };
    int64_t m_token = 0;
};

#if wxUSE_BMPBUTTON

enum class ToggleBundleThrowPoint
{
    None,
    DefaultSize,
    PreferredSize,
    Bitmap
};

struct ToggleBundleException
{
};

struct ToggleBundleCallbackState
{
    std::function<void()> onDestroy;
    std::function<void()> onDefaultSize;
    std::function<void()> onPreferredSize;
    std::function<void()> onBitmap;
    unsigned defaultSizeCalls = 0;
    unsigned preferredSizeCalls = 0;
    unsigned bitmapCalls = 0;
    bool returnInvalidBitmap = false;
    bool returnInvalidPreferredSize = false;
    ToggleBundleThrowPoint throwPoint = ToggleBundleThrowPoint::None;
};

class ReentrantToggleBitmapBundleImpl final : public wxBitmapBundleImpl
{
public:
    ReentrantToggleBitmapBundleImpl(
        const std::shared_ptr<ToggleBundleCallbackState>& state,
        int logicalSize)
        : m_state(state),
          m_logicalSize(logicalSize)
    {
    }

    ~ReentrantToggleBitmapBundleImpl() override
    {
        std::function<void()> callback =
            std::move(m_state->onDestroy);
        if ( callback )
            callback();
    }

    wxSize GetDefaultSize() const override
    {
        ++m_state->defaultSizeCalls;
        std::function<void()> callback =
            std::move(m_state->onDefaultSize);
        if ( callback )
            callback();
        if ( m_state->throwPoint ==
             ToggleBundleThrowPoint::DefaultSize )
        {
            m_state->throwPoint = ToggleBundleThrowPoint::None;
            throw ToggleBundleException();
        }
        return wxSize(m_logicalSize, m_logicalSize);
    }

    wxSize GetPreferredBitmapSizeAtScale(double scale) const override
    {
        ++m_state->preferredSizeCalls;
        std::function<void()> callback =
            std::move(m_state->onPreferredSize);
        if ( callback )
            callback();
        if ( m_state->throwPoint ==
             ToggleBundleThrowPoint::PreferredSize )
        {
            m_state->throwPoint = ToggleBundleThrowPoint::None;
            throw ToggleBundleException();
        }
        if ( m_state->returnInvalidPreferredSize )
            return wxSize();

        const int pixels = wxMax(
            1, static_cast<int>(std::lround(m_logicalSize * scale)));
        return wxSize(pixels, pixels);
    }

    wxBitmap GetBitmap(const wxSize& size) override
    {
        ++m_state->bitmapCalls;
        std::function<void()> callback = std::move(m_state->onBitmap);
        if ( callback )
            callback();
        if ( m_state->throwPoint ==
             ToggleBundleThrowPoint::Bitmap )
        {
            m_state->throwPoint = ToggleBundleThrowPoint::None;
            throw ToggleBundleException();
        }

        if ( m_state->returnInvalidBitmap )
            return wxBitmap();

        wxBitmap bitmap;
        const double scale =
            static_cast<double>(size.y) / m_logicalSize;
        bitmap.CreateWithDIPSize(
            wxSize(m_logicalSize, m_logicalSize), scale, 32);
        return bitmap;
    }

private:
    std::shared_ptr<ToggleBundleCallbackState> m_state;
    int m_logicalSize;
};

wxBitmapBundle MakeReentrantToggleBundle(
    const std::shared_ptr<ToggleBundleCallbackState>& state,
    int logicalSize = 17)
{
    return wxBitmapBundle::FromImpl(
        new ReentrantToggleBitmapBundleImpl(state, logicalSize));
}

#endif // wxUSE_BMPBUTTON

#if wxUSE_BMPBUTTON

enum class ButtonBundleCallbackPoint
{
    PreferredSize,
    DefaultSize,
    Bitmap
};

struct ButtonBundleCallbackState
{
    ButtonBundleCallbackPoint point =
        ButtonBundleCallbackPoint::PreferredSize;
    std::function<void()> callback;
    std::function<void()> onDestroy;
    unsigned preferredSizeCalls = 0;
    unsigned defaultSizeCalls = 0;
    unsigned bitmapCalls = 0;
    bool throwAtPoint = false;
};

struct ButtonBundleException
{
};

class ReentrantButtonBitmapBundleImpl final : public wxBitmapBundleImpl
{
public:
    ReentrantButtonBitmapBundleImpl(
        const std::shared_ptr<ButtonBundleCallbackState>& state,
        int logicalSize)
        : m_state(state),
          m_logicalSize(logicalSize)
    {
    }

    ~ReentrantButtonBitmapBundleImpl() override
    {
        std::function<void()> callback =
            std::move(m_state->onDestroy);
        if ( callback )
            callback();
    }

    wxSize GetDefaultSize() const override
    {
        ++m_state->defaultSizeCalls;
        Invoke(ButtonBundleCallbackPoint::DefaultSize);
        return wxSize(m_logicalSize, m_logicalSize);
    }

    wxSize GetPreferredBitmapSizeAtScale(double scale) const override
    {
        ++m_state->preferredSizeCalls;
        Invoke(ButtonBundleCallbackPoint::PreferredSize);
        const int pixels = wxMax(
            1, static_cast<int>(std::lround(m_logicalSize * scale)));
        return wxSize(pixels, pixels);
    }

    wxBitmap GetBitmap(const wxSize& size) override
    {
        ++m_state->bitmapCalls;
        Invoke(ButtonBundleCallbackPoint::Bitmap);

        wxBitmap bitmap;
        const double scale =
            static_cast<double>(size.y) / m_logicalSize;
        bitmap.CreateWithDIPSize(
            wxSize(m_logicalSize, m_logicalSize), scale, 32);
        return bitmap;
    }

private:
    void Invoke(ButtonBundleCallbackPoint point) const
    {
        if ( point != m_state->point )
            return;

        if ( m_state->callback )
        {
            std::function<void()> callback =
                std::move(m_state->callback);
            callback();
        }
        if ( m_state->throwAtPoint )
        {
            m_state->throwAtPoint = false;
            throw ButtonBundleException();
        }
    }

    std::shared_ptr<ButtonBundleCallbackState> m_state;
    int m_logicalSize;
};

wxBitmapBundle MakeReentrantButtonBundle(
    const std::shared_ptr<ButtonBundleCallbackState>& state,
    int logicalSize = 17)
{
    return wxBitmapBundle::FromImpl(
        new ReentrantButtonBitmapBundleImpl(state, logicalSize));
}

MUXC::Button GetButtonPeer(wxButton *button)
{
    if ( !button )
        return nullptr;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    wxWinUISlot * const slot = host ? host->FindSlot(button) : nullptr;
    return slot
        ? slot->GetContent().try_as<MUXC::Button>()
        : MUXC::Button{ nullptr };
}

MUXC::Image FindTaggedButtonImage(
    const winrt::Windows::Foundation::IInspectable& content,
    const wchar_t *tag)
{
    if ( !content || !tag )
        return nullptr;

    if ( const auto image = content.try_as<MUXC::Image>() )
    {
        const winrt::hstring actual =
            winrt::unbox_value_or<winrt::hstring>(image.Tag(), {});
        return actual == winrt::hstring(tag)
            ? image
            : MUXC::Image{ nullptr };
    }

    if ( const auto panel = content.try_as<MUXC::Panel>() )
    {
        const auto children = panel.Children();
        for ( std::uint32_t i = 0; i < children.Size(); ++i )
        {
            if ( const auto image =
                     FindTaggedButtonImage(children.GetAt(i), tag) )
            {
                return image;
            }
        }
    }

    return nullptr;
}

#endif // wxUSE_BMPBUTTON

#if wxUSE_RADIOBOX
class YieldingRadioBoxLogTarget final : public wxLog
{
public:
    YieldingRadioBoxLogTarget()
        : m_previous(wxLog::SetActiveTarget(this))
    {
    }

    ~YieldingRadioBoxLogTarget() override
    {
        wxLog::SetActiveTarget(m_previous);
    }

    bool DidYield() const { return m_didYield; }

protected:
    void DoLogText(const wxString&) override
    {
        if ( m_didYield )
            return;

        m_didYield = true;
        wxYield();
    }

private:
    wxLog *m_previous = nullptr;
    bool m_didYield = false;
};

struct RadioBoxStormContext
{
    wxRadioBox *box = nullptr;
    unsigned writes = 0;
};

void ContinueRadioBoxProjectionStorm(void *opaque)
{
    auto& context =
        *static_cast<RadioBoxStormContext *>(opaque);
    if ( !context.box )
        return;

    ++context.writes;
    context.box->SetLabel(
        wxString::Format("&Storm %u", context.writes));
    context.box->WinUISetNextPeerWriteHookForTesting(
        &ContinueRadioBoxProjectionStorm, &context);
}
#endif // wxUSE_RADIOBOX

#if wxUSE_STATBOX
class ReentrantLabelStaticBox final : public wxStaticBox
{
public:
    void ReplaceOwnedLabelForTesting(wxWindow *label)
    {
        m_labelWin = label;
    }
};
#endif // wxUSE_STATBOX

class LayoutCountingPanel final : public wxPanel
{
public:
    explicit LayoutCountingPanel(wxWindow *parent)
        : wxPanel(parent)
    {
    }

    bool Layout() override
    {
        ++layoutCalls;
        return wxPanel::Layout();
    }

    unsigned layoutCalls = 0;
};

void CheckDefaultCreateSize(wxWindow& window)
{
    const wxSize size = window.GetSize();
    const wxSize bestSize = window.GetBestSize();

    CHECK(size.x > 0);
    CHECK(size.y > 0);
    CHECK(size == bestSize);
}

} // namespace

#if wxUSE_BUTTON && wxUSE_CHECKBOX && wxUSE_RADIOBTN && wxUSE_TOGGLEBTN

TEST_CASE("wxWinUI action controls support non-physical default Create",
          "[winui-appearance][winui-008d][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxButton button;
    CHECK(button.GetBitmapMargins() == wxSize(0, 0));
    CHECK_FALSE(button.GetAuthNeeded());
    REQUIRE(button.Create(parent, wxID_ANY, "&Run"));
    CHECK(button.GetBitmapMargins() == wxSize(0, 0));
    CHECK_FALSE(button.GetAuthNeeded());
    CheckDefaultCreateSize(button);

    wxCheckBox checkBox;
    CHECK(checkBox.Get3StateValue() == wxCHK_UNCHECKED);
    REQUIRE(checkBox.Create(parent, wxID_ANY, "&Check"));
    CHECK(checkBox.Get3StateValue() == wxCHK_UNCHECKED);
    CheckDefaultCreateSize(checkBox);

    wxRadioButton radio;
    CHECK_FALSE(radio.GetValue());
    REQUIRE(radio.Create(parent, wxID_ANY, "&Choice"));
    CHECK_FALSE(radio.GetValue());
    CheckDefaultCreateSize(radio);

    wxToggleButton toggle;
    CHECK_FALSE(toggle.GetValue());
    REQUIRE(toggle.Create(parent, wxID_ANY, "&Toggle"));
    CHECK_FALSE(toggle.GetValue());
    CheckDefaultCreateSize(toggle);

    wxBitmapToggleButton bitmapToggle;
    CHECK_FALSE(bitmapToggle.GetValue());
    CHECK(bitmapToggle.GetBitmapMargins() == wxSize(0, 0));
    REQUIRE(bitmapToggle.Create(
        parent, wxID_ANY, MakeDPIBundle()));
    CHECK_FALSE(bitmapToggle.GetValue());
    CHECK(bitmapToggle.GetBitmapMargins() == wxSize(0, 0));
    CheckDefaultCreateSize(bitmapToggle);
}

#if wxUSE_COMMANDLINKBUTTON && wxUSE_BMPBUTTON

TEST_CASE("wxWinUI generic command-link button projects its default bitmap",
          "[winui-appearance][command-link][bitmap][parity]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxCommandLinkButton button(
        parent, wxID_ANY, "&Continue", "Open the next page");

    CHECK(button.GetMainLabel() == "&Continue");
    CHECK(button.GetNote() == "Open the next page");
    CHECK(button.GetBitmap().IsOk());

    const MUXC::Button peer = GetButtonPeer(&button);
    REQUIRE(peer);
    const MUXC::Image bitmap = FindTaggedButtonImage(
        peer.Content(), L"wxWinUIButtonBitmap");
    REQUIRE(bitmap);
    CHECK(bitmap.Source());
    CHECK(bitmap.Width() > 0.0);
    CHECK(bitmap.Height() > 0.0);
}

#endif // wxUSE_COMMANDLINKBUTTON && wxUSE_BMPBUTTON

TEST_CASE("wxWinUI action-control Create aborts on attachment destruction",
          "[winui-appearance][winui-008d][action][create][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("Button retained peer and stale Click")
    {
        const unsigned statesBefore =
            wxWinUIButtonTestAccess::LiveCallbackStateCount();
        const unsigned invokesBefore =
            wxWinUIButtonTestAccess::PeerInvokeAttemptCount();

        wxButton *button = new wxButton;
        wxButton * const invoking = button;
        int clickEvents = 0;
        bool attached = false;
        bool queued = false;
        button->Bind(
            wxEVT_BUTTON,
            [&](wxCommandEvent&) { ++clickEvents; });

        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                attached = true;
                queued = wxWinUIButtonTestAccess::QueueClick(*invoking);
                wxButton * const doomed = button;
                button = nullptr;
                delete doomed;
            });

        const bool created =
            invoking->Create(parent, wxID_ANY, "&Doomed");
        wxWinUITopLevelHost::TestOnNextSlotAttached({});

        CHECK(attached);
        CHECK(queued);
        CHECK_FALSE(created);
        CHECK(button == nullptr);
        REQUIRE(WaitFor(
            "retained WinUI Button peer invocation",
            [&]()
            {
                return wxWinUIButtonTestAccess::
                           PeerInvokeAttemptCount() >
                       invokesBefore;
            }));
        CHECK(clickEvents == 0);
        CHECK(wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
              statesBefore);
        delete button;
    }

    SECTION("CheckBox")
    {
        wxCheckBox *check = new wxCheckBox;
        wxCheckBox * const invoking = check;
        bool attached = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                attached = true;
                wxCheckBox * const doomed = check;
                check = nullptr;
                delete doomed;
            });

        const bool created =
            invoking->Create(parent, wxID_ANY, "&Doomed");
        wxWinUITopLevelHost::TestOnNextSlotAttached({});
        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(check == nullptr);
        delete check;
    }

    SECTION("RadioButton")
    {
        wxRadioButton *radio = new wxRadioButton;
        wxRadioButton * const invoking = radio;
        bool attached = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                attached = true;
                wxRadioButton * const doomed = radio;
                radio = nullptr;
                delete doomed;
            });

        const bool created =
            invoking->Create(parent, wxID_ANY, "&Doomed");
        wxWinUITopLevelHost::TestOnNextSlotAttached({});
        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(radio == nullptr);
        delete radio;
    }

    SECTION("ToggleButton")
    {
        wxToggleButton *toggle = new wxToggleButton;
        wxToggleButton * const invoking = toggle;
        bool attached = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                attached = true;
                wxToggleButton * const doomed = toggle;
                toggle = nullptr;
                delete doomed;
            });

        const bool created =
            invoking->Create(parent, wxID_ANY, "&Doomed");
        wxWinUITopLevelHost::TestOnNextSlotAttached({});
        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(toggle == nullptr);
        delete toggle;
    }
}

TEST_CASE("wxWinUI action APIs stop after a terminal host flush",
          "[winui-appearance][winui-008d][action][lifetime][command]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("Button SetLabel")
    {
        wxButton *button =
            new wxButton(parent, wxID_ANY, "&Original");
        wxButton * const invoking = button;
        wxYield();
        const wxSize size = invoking->GetSize();
        invoking->SetSize(size.x + 1, size.y);

        bool synced = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                synced = true;
                wxButton * const doomed = button;
                button = nullptr;
                delete doomed;
            });

        invoking->SetLabel("&Replacement");
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(synced);
        CHECK(button == nullptr);
        delete button;
    }

    SECTION("CheckBox Command")
    {
        wxCheckBox *check =
            new wxCheckBox(parent, wxID_ANY, "&Check");
        wxCheckBox * const invoking = check;
        int events = 0;
        check->Bind(
            wxEVT_CHECKBOX,
            [&](wxCommandEvent&) { ++events; });
        wxYield();
        const wxSize size = invoking->GetSize();
        invoking->SetSize(size.x + 1, size.y);

        bool synced = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                synced = true;
                wxCheckBox * const doomed = check;
                check = nullptr;
                delete doomed;
            });

        wxCommandEvent event(wxEVT_CHECKBOX, invoking->GetId());
        event.SetInt(wxCHK_CHECKED);
        invoking->Command(event);
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(synced);
        CHECK(check == nullptr);
        CHECK(events == 0);
        delete check;
    }

    SECTION("RadioButton Command")
    {
        wxRadioButton *radio =
            new wxRadioButton(
                parent, wxID_ANY, "&Radio",
                wxDefaultPosition, wxDefaultSize, wxRB_SINGLE);
        wxRadioButton * const invoking = radio;
        int events = 0;
        radio->Bind(
            wxEVT_RADIOBUTTON,
            [&](wxCommandEvent&) { ++events; });
        wxYield();
        const wxSize size = invoking->GetSize();
        invoking->SetSize(size.x + 1, size.y);

        bool synced = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                synced = true;
                wxRadioButton * const doomed = radio;
                radio = nullptr;
                delete doomed;
            });

        wxCommandEvent event(wxEVT_RADIOBUTTON, invoking->GetId());
        event.SetInt(1);
        invoking->Command(event);
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(synced);
        CHECK(radio == nullptr);
        CHECK(events == 0);
        delete radio;
    }

    SECTION("ToggleButton Command")
    {
        wxToggleButton *toggle =
            new wxToggleButton(parent, wxID_ANY, "&Toggle");
        wxToggleButton * const invoking = toggle;
        int events = 0;
        toggle->Bind(
            wxEVT_TOGGLEBUTTON,
            [&](wxCommandEvent&) { ++events; });
        wxYield();
        const wxSize size = invoking->GetSize();
        invoking->SetSize(size.x + 1, size.y);

        bool synced = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                synced = true;
                wxToggleButton * const doomed = toggle;
                toggle = nullptr;
                delete doomed;
            });

        wxCommandEvent event(wxEVT_TOGGLEBUTTON, invoking->GetId());
        event.SetInt(1);
        invoking->Command(event);
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(synced);
        CHECK(toggle == nullptr);
        CHECK(events == 0);
        delete toggle;
    }
}

TEST_CASE("wxWinUI ToggleButton content projection is reentrant latest-wins",
          "[winui-appearance][togglebutton][content][lifetime][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("Content callback supersedes text and access key")
    {
        wxToggleButton toggle(parent, wxID_ANY, "&Original");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        bool reentered = false;
        const auto property = MUXC::ContentControl::ContentProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( reentered )
                    return;
                reentered = true;
                toggle.SetLabel("&Newest");
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        toggle.SetLabel("&Outer");

        CHECK(reentered);
        CHECK(toggle.GetLabel() == "&Newest");
        const auto text = FindToggleText(peer.Content());
        REQUIRE(text);
        CHECK(text.Text() == L"Newest");
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"N");
        CHECK(peer.AccessKey() == L"N");
    }

    SECTION("Attached access-key callback cannot leave the second key stale")
    {
        wxToggleButton toggle(parent, wxID_ANY, "&Original");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        bool reentered = false;
        const auto property =
            MUXA::AutomationProperties::AccessKeyProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( reentered )
                    return;
                reentered = true;
                toggle.SetLabel("&Final");
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        // The initial label also uses access key O. Use a genuinely different
        // key here so WinUI is required to raise the attached-property
        // callback which exercises the reentrant second write.
        toggle.SetLabel("&Middle");

        CHECK(reentered);
        const auto text = FindToggleText(peer.Content());
        REQUIRE(text);
        CHECK(text.Text() == L"Final");
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"F");
        CHECK(peer.AccessKey() == L"F");
    }

    SECTION("Destruction from Content prevents every following peer write")
    {
        wxToggleButton *toggle =
            new wxToggleButton(parent, wxID_ANY, "&Original");
        wxToggleButton * const invoking = toggle;
        const auto peer = GetTogglePeer(toggle);
        REQUIRE(peer);
        REQUIRE(MUXA::AutomationProperties::GetAccessKey(peer) == L"O");
        REQUIRE(peer.AccessKey() == L"O");

        bool destroyed = false;
        const auto property = MUXC::ContentControl::ContentProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( destroyed )
                    return;
                destroyed = true;
                wxToggleButton * const doomed = toggle;
                toggle = nullptr;
                delete doomed;
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        invoking->SetLabel("&Doomed");

        CHECK(destroyed);
        CHECK(toggle == nullptr);
        // Content was the boundary which destroyed the owner. Neither of the
        // two access-key writes from the stale transaction may follow it.
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"O");
        CHECK(peer.AccessKey() == L"O");
        delete toggle;
    }

    SECTION("A perpetual Content callback gets one bounded replay")
    {
        wxToggleButton toggle(parent, wxID_ANY, "&Original");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        bool stormEnabled = true;
        unsigned contentWrites = 0;
        const auto property = MUXC::ContentControl::ContentProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                ++contentWrites;
                if ( stormEnabled )
                {
                    toggle.SetLabel(
                        wxString::Format(
                            "&Storm%u", contentWrites));
                }
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        toggle.SetLabel("&Kick");
        REQUIRE(WaitFor(
            "bounded ToggleButton content replay",
            [&]() { return contentWrites >= 16; }));
        CHECK(contentWrites == 16);
        YieldForAWhile(20);
        CHECK(contentWrites == 16);

        // Quarantine is rearmed by the next explicit mutation.
        stormEnabled = false;
        toggle.SetLabel("&Recovered");
        CHECK(contentWrites == 17);
        const auto text = FindToggleText(peer.Content());
        REQUIRE(text);
        CHECK(text.Text() == L"Recovered");
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"R");
        CHECK(peer.AccessKey() == L"R");
    }
}

TEST_CASE("wxWinUI ToggleButton appearance projection is transactional",
          "[winui-appearance][togglebutton][appearance][lifetime][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("Font callback publishes the complete latest appearance")
    {
        wxToggleButton toggle(parent, wxID_ANY, "&Toggle");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        const wxFont latestFont(
            wxFontInfo(17).FaceName("Consolas"));
        const wxColour latestForeground(17, 83, 149);
        const wxColour latestBackground(131, 47, 19);
        bool reentered = false;
        const auto property = MUXC::Control::FontFamilyProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( reentered )
                    return;
                reentered = true;
                toggle.SetFont(latestFont);
                toggle.SetForegroundColour(latestForeground);
                toggle.SetBackgroundColour(latestBackground);
                toggle.SetLayoutDirection(wxLayout_RightToLeft);
                toggle.Disable();
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        toggle.SetFont(MakeTestFont());

        CHECK(reentered);
        REQUIRE(peer.FontFamily());
        CHECK(peer.FontFamily().Source() == L"Consolas");
        const double expectedFontSize =
            latestFont.GetFractionalPointSize() * 96.0 / 72.0;
        // XAML stores this dependency property through a float projection on
        // this runtime, so compare the resulting DIP value with float-sized
        // tolerance instead of requiring bit-identical double arithmetic.
        CHECK(std::abs(peer.FontSize() - expectedFontSize) < 0.00001);
        CHECK(peer.FontWeight().Weight ==
              latestFont.GetNumericWeight());
        CHECK(peer.FontStyle() ==
              winrt::Windows::UI::Text::FontStyle::Normal);
        const auto foreground =
            peer.Foreground().try_as<MUXM::SolidColorBrush>();
        REQUIRE(foreground);
        CHECK(foreground.Color().R == latestForeground.Red());
        CHECK(foreground.Color().G == latestForeground.Green());
        CHECK(foreground.Color().B == latestForeground.Blue());
        const auto background =
            peer.Background().try_as<MUXM::SolidColorBrush>();
        REQUIRE(background);
        CHECK(background.Color().R == latestBackground.Red());
        CHECK(background.Color().G == latestBackground.Green());
        CHECK(background.Color().B == latestBackground.Blue());
        CHECK(peer.FlowDirection() ==
              MUX::FlowDirection::RightToLeft);
        CHECK_FALSE(peer.IsEnabled());
    }

    SECTION("Destruction at FontFamily prevents every later peer write")
    {
        wxToggleButton *toggle =
            new wxToggleButton(parent, wxID_ANY, "&Toggle");
        wxToggleButton * const invoking = toggle;
        const auto peer = GetTogglePeer(toggle);
        REQUIRE(peer);

        bool destroyed = false;
        const auto property = MUXC::Control::FontFamilyProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( destroyed )
                    return;
                destroyed = true;
                wxToggleButton * const doomed = toggle;
                toggle = nullptr;
                delete doomed;
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        invoking->SetFont(MakeTestFont());

        CHECK(destroyed);
        CHECK(toggle == nullptr);
        const auto unset = MUX::DependencyProperty::UnsetValue();
        CHECK(peer.ReadLocalValue(
                  MUXC::Control::FontSizeProperty()) == unset);
        CHECK(peer.ReadLocalValue(
                  MUXC::Control::FontWeightProperty()) == unset);
        CHECK(peer.ReadLocalValue(
                  MUXC::Control::FontStyleProperty()) == unset);
        delete toggle;
    }

    SECTION("Appearance storm is bounded and externally rearmed")
    {
        wxToggleButton toggle(parent, wxID_ANY, "&Toggle");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        bool stormEnabled = true;
        unsigned backgroundWrites = 0;
        const auto property = MUXC::Control::BackgroundProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                ++backgroundWrites;
                if ( stormEnabled )
                {
                    toggle.SetBackgroundColour(
                        wxColour(
                            static_cast<unsigned char>(
                                20 + backgroundWrites),
                            static_cast<unsigned char>(
                                40 + backgroundWrites),
                            static_cast<unsigned char>(
                                60 + backgroundWrites)));
                }
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        toggle.SetBackgroundColour(wxColour(1, 2, 3));
        REQUIRE(WaitFor(
            "bounded ToggleButton appearance replay",
            [&]() { return backgroundWrites >= 16; }));
        CHECK(backgroundWrites == 16);
        YieldForAWhile(20);
        CHECK(backgroundWrites == 16);

        stormEnabled = false;
        const wxColour recovered(9, 71, 133);
        toggle.SetBackgroundColour(recovered);
        CHECK(backgroundWrites == 17);
        const auto brush =
            peer.Background().try_as<MUXM::SolidColorBrush>();
        REQUIRE(brush);
        CHECK(brush.Color().R == recovered.Red());
        CHECK(brush.Color().G == recovered.Green());
        CHECK(brush.Color().B == recovered.Blue());
    }

}

TEST_CASE("wxWinUI RadioButton group clearing revalidates every sibling",
          "[winui-appearance][winui-008d][radiobutton][group][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxRadioButton first(
        parent, wxID_ANY, "&First",
        wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    wxRadioButton second(parent, wxID_ANY, "&Second");
    wxRadioButton *owner =
        new wxRadioButton(parent, wxID_ANY, "&Owner");
    wxRadioButton * const invoking = owner;
    wxYield();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(owner);
    REQUIRE(host);

    // The first flush belongs to the owner. Its callback dirties the first
    // sibling for a NEW flush generation and arms destruction exactly inside
    // first.SetValue(false), i.e. inside ClearRadioGroup(). Mark the slots
    // explicitly: SetSize() is allowed to synchronize geometry immediately,
    // before this one-shot hook is installed, and is therefore not a
    // deterministic way to create pending host work.
    bool ownerSynced = false;
    bool siblingSynced = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            if ( window != invoking )
                return;

            ownerSynced = true;
            host->MarkDirty(&first);
            wxWinUITopLevelHost::TestOnNextSlotSynced(
                [&](wxWindow *sibling)
                {
                    if ( sibling != &first )
                        return;

                    siblingSynced = true;
                    wxRadioButton * const doomed = owner;
                    owner = nullptr;
                    delete doomed;
                });
        });

    host->MarkDirty(invoking);
    invoking->SetValue(true);
    wxWinUITopLevelHost::TestOnNextSlotSynced({});

    CHECK(ownerSynced);
    CHECK(siblingSynced);
    CHECK(owner == nullptr);
    CHECK_FALSE(first.GetValue());
    CHECK_FALSE(second.GetValue());
    delete owner;
}

#if wxUSE_BMPBUTTON

TEST_CASE("wxWinUI BitmapButton bundle callbacks are transactional",
          "[winui-appearance][button][bitmap][lifetime][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    for ( const ButtonBundleCallbackPoint point :
          { ButtonBundleCallbackPoint::PreferredSize,
            ButtonBundleCallbackPoint::DefaultSize,
            ButtonBundleCallbackPoint::Bitmap } )
    {
        const char* const sectionName =
            point == ButtonBundleCallbackPoint::PreferredSize
                ? "Preferred-size replacement is latest-writer-wins"
                : point == ButtonBundleCallbackPoint::DefaultSize
                    ? "Default-size replacement is latest-writer-wins"
                    : "GetBitmap replacement is latest-writer-wins";
        DYNAMIC_SECTION(sectionName)
        {
            wxBitmapButton button(
                parent, wxID_ANY, MakeDPIBundle(11));
            button.SetLabel("&Original");
            const MUXC::Button peer = GetButtonPeer(&button);
            REQUIRE(peer);

            std::uint64_t generationBefore = 0;
            REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(button,
                nullptr, nullptr, nullptr, &generationBefore));

            const wxBitmapBundle latestBundle = MakeDPIBundle(23);
            const auto callbackState =
                std::make_shared<ButtonBundleCallbackState>();
            callbackState->point = point;
            callbackState->callback =
                [&]()
                {
                    button.SetBitmapLabel(latestBundle);
                    button.SetLabel("&Latest");
                };

            button.SetBitmapLabel(
                MakeReentrantButtonBundle(callbackState));

            CHECK(callbackState->preferredSizeCalls == 1);
            CHECK(callbackState->defaultSizeCalls ==
                  (point == ButtonBundleCallbackPoint::PreferredSize
                       ? 0u
                       : 1u));
            CHECK(callbackState->bitmapCalls ==
                  (point == ButtonBundleCallbackPoint::Bitmap ? 1u : 0u));

            wxSize pixels;
            wxAnyButton::State projected = wxAnyButton::State_Max;
            std::uint64_t generation = 0;
            REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(button,
                &pixels, nullptr, &projected, &generation));
            CHECK(pixels ==
                  latestBundle.GetPreferredBitmapSizeFor(&button));
            CHECK(projected == wxAnyButton::State_Normal);
            CHECK(generation > generationBefore);

            const MUXC::Image image = FindTaggedButtonImage(
                peer.Content(), L"wxWinUIButtonBitmap");
            REQUIRE(image);
            CHECK(image.Width() == 23.0);
            CHECK(image.Height() == 23.0);
        }
    }

    for ( const ButtonBundleCallbackPoint point :
          { ButtonBundleCallbackPoint::PreferredSize,
            ButtonBundleCallbackPoint::DefaultSize,
            ButtonBundleCallbackPoint::Bitmap } )
    {
        const char* const sectionName =
            point == ButtonBundleCallbackPoint::PreferredSize
                ? "Destruction from preferred-size callback is terminal"
                : point == ButtonBundleCallbackPoint::DefaultSize
                    ? "Destruction from default-size callback is terminal"
                    : "Destruction from GetBitmap callback is terminal";
        DYNAMIC_SECTION(sectionName)
        {
            const unsigned statesBefore =
                wxWinUIButtonTestAccess::LiveCallbackStateCount();
            wxBitmapButton *button =
                new wxBitmapButton(
                    parent, wxID_ANY, MakeDPIBundle(11));
            wxBitmapButton * const invoking = button;
            REQUIRE(GetButtonPeer(button));
            CHECK(wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
                  statesBefore + 1);

            const auto callbackState =
                std::make_shared<ButtonBundleCallbackState>();
            callbackState->point = point;
            callbackState->callback =
                [&]()
                {
                    wxBitmapButton * const doomed = button;
                    button = nullptr;
                    delete doomed;
                };

            invoking->SetBitmapLabel(
                MakeReentrantButtonBundle(callbackState));

            CHECK(button == nullptr);
            CHECK(callbackState->preferredSizeCalls == 1);
            CHECK(callbackState->defaultSizeCalls ==
                  (point == ButtonBundleCallbackPoint::PreferredSize
                       ? 0u
                       : 1u));
            CHECK(callbackState->bitmapCalls ==
                  (point == ButtonBundleCallbackPoint::Bitmap ? 1u : 0u));
            CHECK(wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
                  statesBefore);
            delete button;
        }
    }

    SECTION("Best-size uses the validated cache without bundle callbacks")
    {
        const unsigned statesBefore =
            wxWinUIButtonTestAccess::LiveCallbackStateCount();
        wxBitmapButton *button =
            new wxBitmapButton(
                parent, wxID_ANY, MakeDPIBundle(11));
        wxBitmapButton * const invoking = button;
        const auto callbackState =
            std::make_shared<ButtonBundleCallbackState>();
        callbackState->point = ButtonBundleCallbackPoint::DefaultSize;
        invoking->SetBitmapLabel(
            MakeReentrantButtonBundle(callbackState));

        const unsigned preferredBefore =
            callbackState->preferredSizeCalls;
        const unsigned defaultBefore =
            callbackState->defaultSizeCalls;
        const unsigned bitmapBefore =
            callbackState->bitmapCalls;
        callbackState->callback =
            [&]()
            {
                wxBitmapButton * const doomed = button;
                button = nullptr;
                delete doomed;
            };

        const wxSize best = invoking->GetBestSize();

        CHECK(best.x > 0);
        CHECK(best.y > 0);
        CHECK(button == invoking);
        CHECK(callbackState->preferredSizeCalls == preferredBefore);
        CHECK(callbackState->defaultSizeCalls == defaultBefore);
        CHECK(callbackState->bitmapCalls == bitmapBefore);
        delete button;
        button = nullptr;
        CHECK(wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
              statesBefore);
        delete button;
    }

    SECTION("Bitmap getter stops after destructive hidden GetDefaultSize")
    {
        const unsigned statesBefore =
            wxWinUIButtonTestAccess::LiveCallbackStateCount();
        wxBitmapButton *button =
            new wxBitmapButton(
                parent, wxID_ANY, MakeDPIBundle(11));
        wxBitmapButton * const invoking = button;
        const auto callbackState =
            std::make_shared<ButtonBundleCallbackState>();
        callbackState->point =
            ButtonBundleCallbackPoint::DefaultSize;
        invoking->SetBitmapLabel(
            MakeReentrantButtonBundle(callbackState));

        const unsigned preferredBefore =
            callbackState->preferredSizeCalls;
        const unsigned defaultBefore =
            callbackState->defaultSizeCalls;
        const unsigned bitmapBefore =
            callbackState->bitmapCalls;
        callbackState->callback =
            [&]()
            {
                wxBitmapButton * const doomed = button;
                button = nullptr;
                delete doomed;
            };

        const wxBitmap bitmap = invoking->GetBitmapLabel();

        CHECK(button == nullptr);
        CHECK_FALSE(bitmap.IsOk());
        CHECK(callbackState->preferredSizeCalls ==
              preferredBefore + 1);
        CHECK(callbackState->defaultSizeCalls ==
              defaultBefore + 1);
        CHECK(callbackState->bitmapCalls == bitmapBefore);
        CHECK(wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
              statesBefore);
        delete button;
    }

    SECTION("Best-size cache is invalidated by the committed latest bitmap")
    {
        wxBitmapButton button(
            parent, wxID_ANY, MakeDPIBundle(11));
        const wxBitmapBundle latest = MakeDPIBundle(31);
        const auto callbackState =
            std::make_shared<ButtonBundleCallbackState>();
        callbackState->point =
            ButtonBundleCallbackPoint::PreferredSize;

        wxSize cachedDuringReentry;
        callbackState->callback =
            [&]()
            {
                button.SetBitmapLabel(latest);
                cachedDuringReentry = button.GetBestSize();
            };

        button.SetBitmapLabel(
            MakeReentrantButtonBundle(callbackState, 17));

        const wxSize committed = button.GetBestSize();
        CHECK(cachedDuringReentry.x > 0);
        CHECK(cachedDuringReentry.y > 0);
        CHECK(committed.x > cachedDuringReentry.x);
        CHECK(committed.y > cachedDuringReentry.y);
    }

    SECTION("Nested bitmap getter is non-recursive")
    {
        wxBitmapButton button(
            parent, wxID_ANY, MakeDPIBundle(11));
        const auto callbackState =
            std::make_shared<ButtonBundleCallbackState>();
        callbackState->point =
            ButtonBundleCallbackPoint::PreferredSize;
        button.SetBitmapLabel(
            MakeReentrantButtonBundle(callbackState, 19));

        bool nestedCalled = false;
        wxBitmap nested;
        callbackState->callback =
            [&]()
            {
                nestedCalled = true;
                nested = button.GetBitmapLabel();
            };

        const wxBitmap outer = button.GetBitmapLabel();
        CHECK(nestedCalled);
        CHECK_FALSE(nested.IsOk());
        CHECK(outer.IsOk());
        CHECK(outer.GetDIPSize() == wxSize(19, 19));
    }

    SECTION("Public bitmap getter is transactional before Create")
    {
        wxBitmapButton button;
        const auto callbackState =
            std::make_shared<ButtonBundleCallbackState>();
        callbackState->point =
            ButtonBundleCallbackPoint::PreferredSize;
        button.SetBitmapLabel(
            MakeReentrantButtonBundle(callbackState, 13));

        const wxBitmapBundle latest = MakeDPIBundle(29);
        callbackState->callback =
            [&]() { button.SetBitmapLabel(latest); };

        const wxBitmap bitmap = button.GetBitmapLabel();
        REQUIRE(bitmap.IsOk());
        CHECK(bitmap.GetDIPSize() == wxSize(29, 29));
    }

    for ( const ButtonBundleCallbackPoint point :
          { ButtonBundleCallbackPoint::PreferredSize,
            ButtonBundleCallbackPoint::DefaultSize,
            ButtonBundleCallbackPoint::Bitmap } )
    {
        const char* const sectionName =
            point == ButtonBundleCallbackPoint::PreferredSize
                ? "Pre-Create getter survives destruction at PreferredSize"
                : point == ButtonBundleCallbackPoint::DefaultSize
                    ? "Pre-Create getter survives destruction at DefaultSize"
                    : "Pre-Create getter survives destruction at GetBitmap";
        DYNAMIC_SECTION(sectionName)
        {
            wxBitmapButton *button = new wxBitmapButton;
            wxBitmapButton * const invoking = button;
            const auto callbackState =
                std::make_shared<ButtonBundleCallbackState>();
            callbackState->point = point;
            invoking->SetBitmapLabel(
                MakeReentrantButtonBundle(callbackState, 17));
            callbackState->callback =
                [&]()
                {
                    wxBitmapButton * const doomed = button;
                    button = nullptr;
                    delete doomed;
                };

            const wxBitmap bitmap = invoking->GetBitmapLabel();
            CHECK(button == nullptr);
            CHECK_FALSE(bitmap.IsOk());
            delete button;
        }
    }

    SECTION("Pre-Create bundle destructor reentry sees an inert owner")
    {
        wxBitmapButton *button = new wxBitmapButton;
        const auto callbackState =
            std::make_shared<ButtonBundleCallbackState>();
        button->SetBitmapLabel(
            MakeReentrantButtonBundle(callbackState, 17));

        bool reentered = false;
        callbackState->onDestroy =
            [&]()
            {
                reentered = true;
                CHECK_FALSE(button->GetBitmapLabel().IsOk());
                button->SetBitmapLabel(MakeDPIBundle(23));
            };

        delete button;
        button = nullptr;
        CHECK(reentered);
    }

    SECTION("DPI bundle exception is contained and projection recovers")
    {
        wxBitmapButton button(
            parent, wxID_ANY, MakeDPIBundle(11));
        const auto callbackState =
            std::make_shared<ButtonBundleCallbackState>();
        callbackState->point =
            ButtonBundleCallbackPoint::DefaultSize;
        button.SetBitmapLabel(
            MakeReentrantButtonBundle(callbackState, 21));

        std::uint64_t generationBefore = 0;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(button,
            nullptr, nullptr, nullptr, &generationBefore));
        callbackState->throwAtPoint = true;
        wxDPIChangedEvent event(
            wxSize(96, 96), wxSize(144, 144));
        wxLogNull noLog;
        CHECK_NOTHROW(button.ProcessWindowEvent(event));
        CHECK_FALSE(callbackState->throwAtPoint);

        button.SetLabel("&Recovered");
        std::uint64_t generationAfter = 0;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(button,
            nullptr, nullptr, nullptr, &generationAfter));
        CHECK(generationAfter > generationBefore);
    }

    SECTION("Failed visual-state projection cannot suppress a later click")
    {
        wxBitmapButton button(
            parent, wxID_ANY, MakeDPIBundle(11));

        const auto callbackState =
            std::make_shared<ButtonBundleCallbackState>();
        callbackState->point =
            ButtonBundleCallbackPoint::PreferredSize;
        button.SetBitmapFocus(
            MakeReentrantButtonBundle(callbackState, 17));
        callbackState->throwAtPoint = true;

        wxLogNull noLog;
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(button,
            wxAnyButton::State_Focused, button.GetDPIScaleFactor()));
        CHECK_FALSE(callbackState->throwAtPoint);

        int clicks = 0;
        button.Bind(
            wxEVT_BUTTON,
            [&](wxCommandEvent&) { ++clicks; });
        REQUIRE(wxWinUIButtonTestAccess::QueueClick(button));
        wxYield();
        wxYield();
        CHECK(clicks == 1);
    }

    SECTION("Non-finite scale seam is rejected without publication")
    {
        wxBitmapButton button(
            parent, wxID_ANY, MakeDPIBundle(13));
        wxSize beforePixels;
        wxAnyButton::State beforeState = wxAnyButton::State_Max;
        std::uint64_t beforeGeneration = 0;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(button,
            &beforePixels, nullptr, &beforeState, &beforeGeneration));

        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(button,
            wxAnyButton::State_Normal,
            std::numeric_limits<double>::quiet_NaN()));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(button,
            wxAnyButton::State_Normal,
            std::numeric_limits<double>::infinity()));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(button,
            wxAnyButton::State_Normal,
            -std::numeric_limits<double>::infinity()));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(button,
            wxAnyButton::State_Normal,
            std::numeric_limits<double>::denorm_min()));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(button,
            wxAnyButton::State_Normal, 1.0 / 128.0));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(button,
            wxAnyButton::State_Normal, 65.0));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(button,
            wxAnyButton::State_Normal,
            std::numeric_limits<double>::max()));

        wxSize afterPixels;
        wxAnyButton::State afterState = wxAnyButton::State_Max;
        std::uint64_t afterGeneration = 0;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(button,
            &afterPixels, nullptr, &afterState, &afterGeneration));
        CHECK(afterPixels == beforePixels);
        CHECK(afterState == beforeState);
        CHECK(afterGeneration == beforeGeneration);
    }
}

TEST_CASE("wxWinUI Button appearance projection is transactional",
          "[winui-appearance][button][appearance][lifetime][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("Background callback is latest-writer-wins")
    {
        wxButton button(parent, wxID_ANY, "&Original");
        const MUXC::Button peer = GetButtonPeer(&button);
        REQUIRE(peer);

        const wxColour latest(17, 43, 91);
        bool reentered = false;
        const auto property = MUXC::Control::BackgroundProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( reentered )
                    return;
                reentered = true;
                button.SetBackgroundColour(latest);
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        button.SetBackgroundColour(wxColour(111, 7, 29));

        CHECK(reentered);
        const auto brush =
            peer.Background().try_as<MUXM::SolidColorBrush>();
        REQUIRE(brush);
        const auto colour = brush.Color();
        CHECK(colour.R == latest.Red());
        CHECK(colour.G == latest.Green());
        CHECK(colour.B == latest.Blue());
        CHECK(colour.A == latest.Alpha());
    }

    SECTION("Appearance callback storm has one bounded replay")
    {
        wxButton button(parent, wxID_ANY, "&Original");
        const MUXC::Button peer = GetButtonPeer(&button);
        REQUIRE(peer);

        bool stormEnabled = true;
        unsigned backgroundWrites = 0;
        const auto property = MUXC::Control::BackgroundProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                ++backgroundWrites;
                if ( stormEnabled )
                {
                    button.SetBackgroundColour(
                        wxColour(
                            static_cast<unsigned char>(
                                20 + backgroundWrites),
                            static_cast<unsigned char>(
                                40 + backgroundWrites),
                            static_cast<unsigned char>(
                                60 + backgroundWrites)));
                }
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        button.SetBackgroundColour(wxColour(1, 2, 3));
        REQUIRE(WaitFor(
            "bounded WinUI Button appearance replay",
            [&]() { return backgroundWrites >= 16; }));
        CHECK(backgroundWrites == 16);
        YieldForAWhile(20);
        CHECK(backgroundWrites == 16);

        stormEnabled = false;
        const wxColour recovered(9, 71, 133);
        button.SetBackgroundColour(recovered);
        CHECK(backgroundWrites == 17);
        const auto brush =
            peer.Background().try_as<MUXM::SolidColorBrush>();
        REQUIRE(brush);
        const auto colour = brush.Color();
        CHECK(colour.R == recovered.Red());
        CHECK(colour.G == recovered.Green());
        CHECK(colour.B == recovered.Blue());
    }

    SECTION("Font-family destruction prevents every later font write")
    {
        const unsigned statesBefore =
            wxWinUIButtonTestAccess::LiveCallbackStateCount();
        wxButton *button =
            new wxButton(parent, wxID_ANY, "&Original");
        wxButton * const invoking = button;
        const MUXC::Button peer = GetButtonPeer(button);
        REQUIRE(peer);

        bool destroyed = false;
        const auto property = MUXC::Control::FontFamilyProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( destroyed )
                    return;
                destroyed = true;
                wxButton * const doomed = button;
                button = nullptr;
                delete doomed;
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        invoking->SetFont(MakeTestFont());

        CHECK(destroyed);
        CHECK(button == nullptr);
        const auto unset = MUX::DependencyProperty::UnsetValue();
        CHECK(peer.ReadLocalValue(
                  MUXC::Control::FontSizeProperty()) == unset);
        CHECK(peer.ReadLocalValue(
                  MUXC::Control::FontWeightProperty()) == unset);
        CHECK(peer.ReadLocalValue(
                  MUXC::Control::FontStyleProperty()) == unset);
        CHECK(wxWinUIButtonTestAccess::LiveCallbackStateCount() ==
              statesBefore);
        delete button;
    }

    SECTION("An incompatible application accent style is contained")
    {
        wxButton first(parent, wxID_ANY, "&First");
        wxButton second(parent, wxID_ANY, "&Second");
        const MUXC::Button firstPeer = GetButtonPeer(&first);
        const MUXC::Button secondPeer = GetButtonPeer(&second);
        REQUIRE(firstPeer);
        REQUIRE(secondPeer);

        first.SetDefault();

        const MUX::Application application =
            MUX::Application::Current();
        REQUIRE(application);
        const MUX::ResourceDictionary resources =
            application.Resources();
        const auto key =
            winrt::box_value(winrt::hstring(L"AccentButtonStyle"));
        const bool hadLocalResource = resources.HasKey(key);
        const winrt::Windows::Foundation::IInspectable previousResource =
            hadLocalResource
                ? resources.Lookup(key)
                : nullptr;

        MUX::Style incompatibleStyle;
        incompatibleStyle.TargetType(
            winrt::xaml_typename<MUXC::TextBlock>());
        resources.Insert(key, incompatibleStyle);
        {
            wxScopeGuard restoreResource = wxMakeGuard(
                [resources, key, hadLocalResource, previousResource]()
                {
                    try
                    {
                        if ( hadLocalResource )
                            resources.Insert(key, previousResource);
                        else
                            resources.Remove(key);
                    }
                    catch ( ... )
                    {
                    }
                });
            wxUnusedVar(restoreResource);

            wxLogNull suppressExpectedStyleWarning;
            CHECK_NOTHROW(second.SetDefault());
            CHECK(secondPeer.Style() == nullptr);
        }

        // Once the application resource is restored, both the logical
        // default transition and the peer remain usable.
        CHECK_NOTHROW(first.SetDefault());
        CHECK(firstPeer.Style() != nullptr);
    }
}

TEST_CASE("wxWinUI BitmapToggleButton bundle callbacks are transactional",
          "[winui-appearance][togglebutton][bitmap][lifetime][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("Public bitmap getter works before Create and is reentrant-safe")
    {
        wxBitmapToggleButton toggle;
        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        toggle.SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState, 17));

        const wxBitmap first = toggle.GetBitmapLabel();
        REQUIRE(first.IsOk());
        CHECK(first.GetDIPSize() == wxSize(17, 17));
        CHECK_FALSE(toggle.GetBitmapDisabled().IsOk());

        wxBitmap nested;
        callbackState->onDefaultSize =
            [&]() { nested = toggle.GetBitmapLabel(); };
        const wxBitmap recursive = toggle.GetBitmapLabel();
        CHECK_FALSE(nested.IsOk());
        REQUIRE(recursive.IsOk());
        CHECK(recursive.GetDIPSize() == wxSize(17, 17));

        const wxBitmapBundle latestBundle = MakeDPIBundle(31);
        callbackState->onBitmap =
            [&]() { toggle.SetBitmapLabel(latestBundle); };
        const wxBitmap latest = toggle.GetBitmapLabel();
        REQUIRE(latest.IsOk());
        CHECK(latest.GetDIPSize() == latestBundle.GetDefaultSize());
    }

    SECTION("Pre-Create bitmap getter survives destruction at every boundary")
    {
        constexpr std::array<ToggleBundleThrowPoint, 3> callbackPoints{
            ToggleBundleThrowPoint::DefaultSize,
            ToggleBundleThrowPoint::PreferredSize,
            ToggleBundleThrowPoint::Bitmap
        };

        for ( const ToggleBundleThrowPoint callbackPoint :
              callbackPoints )
        {
            CAPTURE(static_cast<int>(callbackPoint));
            wxBitmapToggleButton *toggle = new wxBitmapToggleButton;
            wxBitmapToggleButton * const invoking = toggle;
            const auto callbackState =
                std::make_shared<ToggleBundleCallbackState>();
            invoking->SetBitmapLabel(
                MakeReentrantToggleBundle(callbackState, 19));

            const auto destroy =
                [&]()
                {
                    wxBitmapToggleButton * const doomed = toggle;
                    toggle = nullptr;
                    delete doomed;
                };
            switch ( callbackPoint )
            {
                case ToggleBundleThrowPoint::DefaultSize:
                    callbackState->onDefaultSize = destroy;
                    break;
                case ToggleBundleThrowPoint::PreferredSize:
                    callbackState->onPreferredSize = destroy;
                    break;
                case ToggleBundleThrowPoint::Bitmap:
                    callbackState->onBitmap = destroy;
                    break;
                case ToggleBundleThrowPoint::None:
                    break;
            }

            const wxBitmap bitmap = invoking->GetBitmapLabel();

            CHECK(toggle == nullptr);
            CHECK_FALSE(bitmap.IsOk());
            delete toggle;
        }
    }

    SECTION("Pre-Create bundle destructor reentry sees an inert bitmap owner")
    {
        wxBitmapToggleButton *toggle = new wxBitmapToggleButton;
        wxBitmapToggleButton * const invoking = toggle;
        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        invoking->SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState, 19));

        bool reentered = false;
        wxBitmap nestedBitmap;
        wxSize nestedMargins(-1, -1);
        callbackState->onDestroy =
            [&]()
            {
                reentered = true;
                nestedBitmap = invoking->GetBitmapLabel();
                nestedMargins = invoking->GetBitmapMargins();
                invoking->SetBitmapMargins(7, 9);
                invoking->SetBitmapLabel(MakeDPIBundle(5));
            };

        delete toggle;
        toggle = nullptr;

        CHECK(reentered);
        CHECK_FALSE(nestedBitmap.IsOk());
        CHECK(nestedMargins == wxSize(0, 0));
    }

    SECTION("An empty bundle projects the text fallback in-place")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, wxBitmapBundle());
        toggle.SetLabel("&Text only");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        const auto text = FindToggleText(peer.Content());
        REQUIRE(text);
        CHECK(text.Text() == L"Text only");
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"T");
        CHECK(peer.AccessKey() == L"T");
    }

    SECTION("An invalid produced bitmap projects the text fallback in-place")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(11));
        toggle.SetLabel("&Fallback");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        callbackState->returnInvalidBitmap = true;
        toggle.SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState));

        CHECK(callbackState->preferredSizeCalls == 1);
        CHECK(callbackState->bitmapCalls == 1);
        const auto text = FindToggleText(peer.Content());
        REQUIRE(text);
        CHECK(text.Text() == L"Fallback");
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"F");
        CHECK(peer.AccessKey() == L"F");
    }

    SECTION("Preferred-size reentry publishes only the latest bundle and label")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(11));
        toggle.SetLabel("&Original");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        std::uint64_t generationBefore = 0;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
            nullptr, nullptr, &generationBefore));

        const wxBitmapBundle latestBundle = MakeDPIBundle(23);
        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        callbackState->onPreferredSize =
            [&]()
            {
                // Both calls start newer transactions while the custom
                // bundle's virtual callback is still on the stack.
                toggle.SetBitmapLabel(latestBundle);
                toggle.SetLabel("&Latest");
            };

        toggle.SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState));

        CHECK(callbackState->preferredSizeCalls == 1);
        CHECK(callbackState->bitmapCalls == 0);
        wxSize pixels;
        wxAnyButton::State projected = wxAnyButton::State_Max;
        std::uint64_t generation = 0;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
            &pixels, &projected, &generation));
        CHECK(pixels == latestBundle.GetPreferredBitmapSizeFor(&toggle));
        CHECK(projected == wxAnyButton::State_Normal);
        CHECK(generation > generationBefore);
        const auto text = FindToggleText(peer.Content());
        REQUIRE(text);
        CHECK(text.Text() == L"Latest");
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"L");
        CHECK(peer.AccessKey() == L"L");
    }

    SECTION("GetBitmap reentry publishes only the latest bundle and label")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(11));
        toggle.SetLabel("&Original");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        const wxBitmapBundle latestBundle = MakeDPIBundle(29);
        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        callbackState->onBitmap =
            [&]()
            {
                toggle.SetBitmapLabel(latestBundle);
                toggle.SetLabel("&Newest");
            };

        toggle.SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState));

        CHECK(callbackState->preferredSizeCalls == 1);
        CHECK(callbackState->bitmapCalls == 1);
        wxSize pixels;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
            &pixels, nullptr, nullptr));
        CHECK(pixels == latestBundle.GetPreferredBitmapSizeFor(&toggle));
        const auto text = FindToggleText(peer.Content());
        REQUIRE(text);
        CHECK(text.Text() == L"Newest");
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"N");
        CHECK(peer.AccessKey() == L"N");
    }

    SECTION("Every bundle exception leaves the projection recoverable")
    {
        constexpr std::array<ToggleBundleThrowPoint, 3> throwPoints{
            ToggleBundleThrowPoint::DefaultSize,
            ToggleBundleThrowPoint::PreferredSize,
            ToggleBundleThrowPoint::Bitmap
        };

        for ( const ToggleBundleThrowPoint throwPoint : throwPoints )
        {
            CAPTURE(static_cast<int>(throwPoint));
            wxBitmapToggleButton toggle(
                parent, wxID_ANY, MakeDPIBundle(11));
            toggle.SetLabel("&Before");
            std::uint64_t generationBefore = 0;
            REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
                nullptr, nullptr, &generationBefore));

            const auto callbackState =
                std::make_shared<ToggleBundleCallbackState>();
            callbackState->throwPoint = throwPoint;
            CHECK_THROWS_AS(
                toggle.SetBitmapLabel(
                    MakeReentrantToggleBundle(callbackState, 19)),
                ToggleBundleException);

            std::uint64_t generationAfterFailure = 0;
            REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
                nullptr, nullptr, &generationAfterFailure));
            CHECK(generationAfterFailure == generationBefore);

            toggle.SetLabel("&Recovered");
            wxSize pixels;
            std::uint64_t generation = 0;
            REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
                &pixels, nullptr, &generation));
            const int expectedPixels = wxMax(
                1,
                static_cast<int>(std::lround(
                    19.0 * toggle.GetDPIScaleFactor())));
            CHECK(pixels ==
                  wxSize(expectedPixels, expectedPixels));
            CHECK(generation > generationAfterFailure);
            const auto peer = GetTogglePeer(&toggle);
            REQUIRE(peer);
            const auto text = FindToggleText(peer.Content());
            REQUIRE(text);
            CHECK(text.Text() == L"Recovered");
        }
    }

    SECTION("DPI notification contains a bundle exception and recovers")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(11));
        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        toggle.SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState, 21));
        callbackState->throwPoint =
            ToggleBundleThrowPoint::DefaultSize;

        wxDPIChangedEvent event(
            wxSize(96, 96), wxSize(144, 144));
        wxLogNull noLog;
        CHECK_NOTHROW(toggle.ProcessWindowEvent(event));

        toggle.SetLabel("&Recovered");
        wxSize pixels;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
            &pixels, nullptr, nullptr));
        CHECK(pixels.x > 0);
        CHECK(pixels.y > 0);
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);
        const auto text = FindToggleText(peer.Content());
        REQUIRE(text);
        CHECK(text.Text() == L"Recovered");
    }

    SECTION("Projection failure cannot suppress native toggle events")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(11));
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        toggle.SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState));

        unsigned events = 0;
        int lastValue = -1;
        toggle.Bind(
            wxEVT_TOGGLEBUTTON,
            [&](wxCommandEvent& event)
            {
                ++events;
                lastValue = event.GetInt();
            });

        wxLogNull noLog;
        callbackState->throwPoint =
            ToggleBundleThrowPoint::DefaultSize;
        CHECK_NOTHROW(peer.IsChecked(true));
        CHECK(toggle.GetValue());
        CHECK(events == 1);
        CHECK(lastValue == 1);

        callbackState->throwPoint =
            ToggleBundleThrowPoint::DefaultSize;
        CHECK_NOTHROW(peer.IsChecked(false));
        CHECK_FALSE(toggle.GetValue());
        CHECK(events == 2);
        CHECK(lastValue == 0);
    }

    SECTION("Command remains exactly-once when bundle projection throws")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(11));
        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        toggle.SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState));
        callbackState->throwPoint =
            ToggleBundleThrowPoint::DefaultSize;

        unsigned events = 0;
        toggle.Bind(
            wxEVT_TOGGLEBUTTON,
            [&](wxCommandEvent&) { ++events; });

        wxCommandEvent event(wxEVT_TOGGLEBUTTON, toggle.GetId());
        event.SetInt(1);
        wxLogNull noLog;
        toggle.Command(event);

        CHECK(toggle.GetValue());
        CHECK(events == 1);
    }

    SECTION("Bitmap getter stops safely at every bundle boundary")
    {
        constexpr std::array<ToggleBundleThrowPoint, 3> callbackPoints{
            ToggleBundleThrowPoint::DefaultSize,
            ToggleBundleThrowPoint::PreferredSize,
            ToggleBundleThrowPoint::Bitmap
        };

        for ( const ToggleBundleThrowPoint callbackPoint :
              callbackPoints )
        {
            CAPTURE(static_cast<int>(callbackPoint));
            wxBitmapToggleButton *toggle =
                new wxBitmapToggleButton(
                    parent, wxID_ANY, MakeDPIBundle(11));
            wxBitmapToggleButton * const invoking = toggle;
            const auto callbackState =
                std::make_shared<ToggleBundleCallbackState>();
            invoking->SetBitmapLabel(
                MakeReentrantToggleBundle(callbackState));

            const auto destroy =
                [&]()
                {
                    wxBitmapToggleButton * const doomed = toggle;
                    toggle = nullptr;
                    delete doomed;
                };
            switch ( callbackPoint )
            {
                case ToggleBundleThrowPoint::DefaultSize:
                    callbackState->onDefaultSize = destroy;
                    break;
                case ToggleBundleThrowPoint::PreferredSize:
                    callbackState->onPreferredSize = destroy;
                    break;
                case ToggleBundleThrowPoint::Bitmap:
                    callbackState->onBitmap = destroy;
                    break;
                case ToggleBundleThrowPoint::None:
                    break;
            }

            const wxBitmap bitmap = invoking->GetBitmapLabel();

            CHECK(toggle == nullptr);
            CHECK_FALSE(bitmap.IsOk());
            delete toggle;
        }
    }

    SECTION("Bitmap getter retries a reentrant bundle replacement")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(11));
        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        toggle.SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState, 17));
        const wxBitmapBundle latestBundle = MakeDPIBundle(31);
        callbackState->onBitmap =
            [&]() { toggle.SetBitmapLabel(latestBundle); };

        const wxBitmap bitmap = toggle.GetBitmapLabel();

        REQUIRE(bitmap.IsOk());
        CHECK(bitmap.GetSize() ==
              latestBundle.GetPreferredBitmapSizeFor(&toggle));
        CHECK(bitmap.GetDIPSize() == latestBundle.GetDefaultSize());
    }

    SECTION("Nested bitmap getter is non-recursive")
    {
        wxBitmapToggleButton bitmapToggle(
            parent, wxID_ANY, MakeDPIBundle(11));
        const auto bitmapState =
            std::make_shared<ToggleBundleCallbackState>();
        bitmapToggle.SetBitmapLabel(
            MakeReentrantToggleBundle(bitmapState, 17));
        bool nestedBitmapCalled = false;
        wxBitmap nestedBitmap;
        bitmapState->onDefaultSize =
            [&]()
            {
                nestedBitmapCalled = true;
                nestedBitmap = bitmapToggle.GetBitmapLabel();
            };

        const wxBitmap outerBitmap =
            bitmapToggle.GetBitmapLabel();

        CHECK(nestedBitmapCalled);
        CHECK_FALSE(nestedBitmap.IsOk());
        CHECK(outerBitmap.IsOk());
    }

    SECTION("Best-size consumes its cache without bundle callbacks")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(11));
        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        toggle.SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState, 23));

        const unsigned defaultCalls =
            callbackState->defaultSizeCalls;
        const unsigned preferredCalls =
            callbackState->preferredSizeCalls;
        const unsigned bitmapCalls =
            callbackState->bitmapCalls;
        bool callbackCalled = false;
        callbackState->onDefaultSize =
            [&]() { callbackCalled = true; };

        const wxSize best = toggle.GetBestSize();

        CHECK_FALSE(callbackCalled);
        CHECK(callbackState->defaultSizeCalls == defaultCalls);
        CHECK(callbackState->preferredSizeCalls == preferredCalls);
        CHECK(callbackState->bitmapCalls == bitmapCalls);
        CHECK(best.x > 0);
        CHECK(best.y > 0);
        callbackState->onDefaultSize = {};

        toggle.SetBitmapLabel(MakeDPIBundle(47));
        const wxSize resized = toggle.GetBestSize();
        CHECK(resized.x > best.x);
        CHECK(resized.y > best.y);

        toggle.SetBitmapPressed(MakeDPIBundle(13));
        toggle.SetValue(true);
        toggle.SetBitmapLabel(MakeDPIBundle(61));
        const wxSize resizedWhilePressed = toggle.GetBestSize();
        CHECK(resizedWhilePressed.x > resized.x);
        CHECK(resizedWhilePressed.y > resized.y);
    }

    SECTION("Bundle destructor cannot reenter revoked XAML delegates")
    {
        wxBitmapToggleButton *toggle =
            new wxBitmapToggleButton(
                parent, wxID_ANY, MakeDPIBundle(11));
        const auto peer = GetTogglePeer(toggle);
        REQUIRE(peer);

        unsigned events = 0;
        toggle->Bind(
            wxEVT_TOGGLEBUTTON,
            [&](wxCommandEvent&) { ++events; });
        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        bool bundleDestroyed = false;
        callbackState->onDestroy =
            [&]()
            {
                bundleDestroyed = true;
                peer.IsChecked(true);
            };
        toggle->SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState));

        delete toggle;
        toggle = nullptr;

        CHECK(bundleDestroyed);
        CHECK(events == 0);
    }

    SECTION("Destruction from preferred-size callback stops before GetBitmap")
    {
        wxBitmapToggleButton *toggle =
            new wxBitmapToggleButton(
                parent, wxID_ANY, MakeDPIBundle(11));
        toggle->SetLabel("&Original");
        wxBitmapToggleButton * const invoking = toggle;
        const auto peer = GetTogglePeer(toggle);
        REQUIRE(peer);

        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        callbackState->onPreferredSize =
            [&]()
            {
                wxBitmapToggleButton * const doomed = toggle;
                toggle = nullptr;
                delete doomed;
            };

        invoking->SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState));

        CHECK(toggle == nullptr);
        CHECK(callbackState->preferredSizeCalls == 1);
        CHECK(callbackState->bitmapCalls == 0);
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"O");
        CHECK(peer.AccessKey() == L"O");
        delete toggle;
    }

    SECTION("Destruction from GetBitmap callback stops before peer Content")
    {
        wxBitmapToggleButton *toggle =
            new wxBitmapToggleButton(
                parent, wxID_ANY, MakeDPIBundle(11));
        toggle->SetLabel("&Original");
        wxBitmapToggleButton * const invoking = toggle;
        const auto peer = GetTogglePeer(toggle);
        REQUIRE(peer);

        const auto callbackState =
            std::make_shared<ToggleBundleCallbackState>();
        callbackState->onBitmap =
            [&]()
            {
                wxBitmapToggleButton * const doomed = toggle;
                toggle = nullptr;
                delete doomed;
            };

        invoking->SetBitmapLabel(
            MakeReentrantToggleBundle(callbackState));

        CHECK(toggle == nullptr);
        CHECK(callbackState->preferredSizeCalls == 1);
        CHECK(callbackState->bitmapCalls == 1);
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"O");
        CHECK(peer.AccessKey() == L"O");
        delete toggle;
    }

    SECTION("Bitmap content storm is bounded and externally rearmed")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(13));
        toggle.SetLabel("&Original");
        const auto peer = GetTogglePeer(&toggle);
        REQUIRE(peer);

        bool stormEnabled = true;
        unsigned contentWrites = 0;
        const auto property = MUXC::ContentControl::ContentProperty();
        const int64_t token = peer.RegisterPropertyChangedCallback(
            property,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                ++contentWrites;
                if ( stormEnabled )
                {
                    toggle.SetLabel(
                        wxString::Format(
                            "&BitmapStorm%u", contentWrites));
                }
            });
        TogglePropertyCallbackGuard guard(peer, property, token);

        toggle.SetLabel("&Kick");
        REQUIRE(WaitFor(
            "bounded BitmapToggleButton content replay",
            [&]() { return contentWrites >= 16; }));
        CHECK(contentWrites == 16);
        YieldForAWhile(20);
        CHECK(contentWrites == 16);

        stormEnabled = false;
        toggle.SetLabel("&Recovered");
        CHECK(contentWrites == 17);
        const auto text = FindToggleText(peer.Content());
        REQUIRE(text);
        CHECK(text.Text() == L"Recovered");
        CHECK(MUXA::AutomationProperties::GetAccessKey(peer) == L"R");
        CHECK(peer.AccessKey() == L"R");
    }

    SECTION("Non-finite scale seam is rejected without publication")
    {
        wxBitmapToggleButton toggle(
            parent, wxID_ANY, MakeDPIBundle(13));
        wxSize beforePixels;
        wxAnyButton::State beforeState = wxAnyButton::State_Max;
        std::uint64_t beforeGeneration = 0;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
            &beforePixels, &beforeState, &beforeGeneration));

        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(toggle,
            wxAnyButton::State_Normal,
            std::numeric_limits<double>::quiet_NaN()));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(toggle,
            wxAnyButton::State_Normal,
            std::numeric_limits<double>::infinity()));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(toggle,
            wxAnyButton::State_Normal,
            -std::numeric_limits<double>::infinity()));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(toggle,
            wxAnyButton::State_Normal,
            std::numeric_limits<double>::denorm_min()));
        CHECK_FALSE(wxWinUIButtonTestAccess::ProjectBitmap(toggle,
            wxAnyButton::State_Normal,
            std::numeric_limits<double>::max()));

        wxSize afterPixels;
        wxAnyButton::State afterState = wxAnyButton::State_Max;
        std::uint64_t afterGeneration = 0;
        REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
            &afterPixels, &afterState, &afterGeneration));
        CHECK(afterPixels == beforePixels);
        CHECK(afterState == beforeState);
        CHECK(afterGeneration == beforeGeneration);
    }
}

TEST_CASE("wxWinUI bitmap buttons project every state at monitor scale",
          "[winui-appearance][button][togglebutton][dpi][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    constexpr std::array<wxAnyButton::State, 5> States{
        wxAnyButton::State_Normal,
        wxAnyButton::State_Current,
        wxAnyButton::State_Pressed,
        wxAnyButton::State_Disabled,
        wxAnyButton::State_Focused
    };
    constexpr std::array<int, 5> LogicalSizes{
        11, 12, 13, 14, 15
    };
    constexpr std::array<double, 4> Scales{
        1.0, 1.5, 2.0, 1.0
    };

    std::array<wxBitmapBundle, 5> bundles;
    for ( size_t i = 0; i < bundles.size(); ++i )
        bundles[i] = MakeDPIBundle(LogicalSizes[i]);

    wxBitmapButton button(parent, wxID_ANY, bundles[0]);
    button.SetBitmapCurrent(bundles[1]);
    button.SetBitmapPressed(bundles[2]);
    button.SetBitmapDisabled(bundles[3]);
    button.SetBitmapFocus(bundles[4]);
    button.SetAuthNeeded(true);
    const MUXC::Button buttonPeer = GetButtonPeer(&button);
    REQUIRE(buttonPeer);

    std::uint64_t lastButtonGeneration = 0;
    for ( const double scale : Scales )
    {
        for ( size_t i = 0; i < States.size(); ++i )
        {
            CAPTURE(scale, i);
            REQUIRE(wxWinUIButtonTestAccess::ProjectBitmap(button,
                States[i], scale));

            wxSize bitmapPixels;
            wxSize authPixels;
            wxAnyButton::State projected =
                wxAnyButton::State_Max;
            std::uint64_t generation = 0;
            REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(button,
                &bitmapPixels, &authPixels,
                &projected, &generation));
            CHECK(bitmapPixels ==
                  bundles[i].GetPreferredBitmapSizeAtScale(scale));
            const int authSize =
                static_cast<int>(std::lround(16.0 * scale));
            CHECK(authPixels == wxSize(authSize, authSize));
            CHECK(projected == States[i]);
            CHECK(generation > lastButtonGeneration);
            lastButtonGeneration = generation;

            const MUXC::Image bitmapImage = FindTaggedButtonImage(
                buttonPeer.Content(), L"wxWinUIButtonBitmap");
            REQUIRE(bitmapImage);
            CHECK(bitmapImage.Width() ==
                  static_cast<double>(LogicalSizes[i]));
            CHECK(bitmapImage.Height() ==
                  static_cast<double>(LogicalSizes[i]));
            const MUXC::Image authImage = FindTaggedButtonImage(
                buttonPeer.Content(), L"wxWinUIButtonAuthBitmap");
            REQUIRE(authImage);
            CHECK(authImage.Width() == 16.0);
            CHECK(authImage.Height() == 16.0);
        }
    }

    wxBitmapToggleButton toggle(
        parent, wxID_ANY, bundles[0]);
    toggle.SetLabel("&Toggle");
    toggle.SetBitmapCurrent(bundles[1]);
    toggle.SetBitmapPressed(bundles[2]);
    toggle.SetBitmapDisabled(bundles[3]);
    toggle.SetBitmapFocus(bundles[4]);
    const auto togglePeer = GetTogglePeer(&toggle);
    REQUIRE(togglePeer);

    std::uint64_t lastToggleGeneration = 0;
    for ( const double scale : Scales )
    {
        for ( size_t i = 0; i < States.size(); ++i )
        {
            CAPTURE(scale, i);
            REQUIRE(wxWinUIButtonTestAccess::ProjectBitmap(toggle,
                States[i], scale));

            wxSize bitmapPixels;
            wxAnyButton::State projected =
                wxAnyButton::State_Max;
            std::uint64_t generation = 0;
            REQUIRE(wxWinUIButtonTestAccess::GetBitmapProjection(toggle,
                &bitmapPixels, &projected, &generation));
            CHECK(bitmapPixels ==
                  bundles[i].GetPreferredBitmapSizeAtScale(scale));
            CHECK(projected == States[i]);
            CHECK(generation > lastToggleGeneration);
            lastToggleGeneration = generation;

            const MUXC::Image image =
                FindToggleImage(togglePeer.Content());
            REQUIRE(image);
            CHECK(image.Width() ==
                  static_cast<double>(LogicalSizes[i]));
            CHECK(image.Height() ==
                  static_cast<double>(LogicalSizes[i]));
        }
    }
}

#endif // wxUSE_BMPBUTTON

#endif // action controls

TEST_CASE("wxWinUI appearance adapter owns and resets local properties",
          "[winui-appearance][adapter]")
{
    MUXC::TextBlock text;
    MUXC::Border background;
    MUXC::Border accessible;

    wxWinUIApplyFont(text, MakeTestFont());
    wxWinUIApplyForeground(text, wxColour(17, 41, 89, 203));
    wxWinUIApplyBackground(background, wxColour(91, 37, 13, 177));
    wxWinUIApplyAccessKey(accessible, "Save && E&xit");
    wxWinUIApplyAccessibleLabel(accessible, "Save && E&xit", "probe");

    wxWinUIAppearanceSnapshot snapshot =
        wxWinUICaptureAppearance(text, background, accessible);
    CheckFontLocals(snapshot, true);
    CHECK(snapshot.hasForeground);
    CHECK(snapshot.hasBackground);
    CHECK(snapshot.automationName == "Save & Exit");
    CHECK(snapshot.accessKey.CmpNoCase("x") == 0);
    CHECK(snapshot.localizedControlType == "probe");

    wxWinUIApplyFont(text, wxNullFont);
    wxWinUIApplyForeground(text, wxNullColour);
    wxWinUIApplyBackground(background, wxNullColour);
    wxWinUIApplyAccessKey(accessible, "Reset");
    wxWinUIApplyAccessibleLabel(accessible, "Reset");

    snapshot = wxWinUICaptureAppearance(text, background, accessible);
    CheckFontLocals(snapshot, false);
    CHECK_FALSE(snapshot.hasForeground);
    CHECK_FALSE(snapshot.hasBackground);
    CHECK(snapshot.automationName == "Reset");
    CHECK(snapshot.accessKey.empty());
    CHECK(snapshot.localizedControlType.empty());
}

TEST_CASE("wxWinUI theme brush expressions update without peer rebuild",
          "[winui-appearance][winui-009][theme]")
{
    const MUXC::Border border = wxWinUICreateThemeBrushBorder(
        "ControlStrokeColorDefaultBrush",
        wxWinUIThemeBrushProperty::BorderBrush);
    REQUIRE(border != nullptr);
    REQUIRE(border.ReadLocalValue(MUXC::Border::BorderBrushProperty()) !=
            MUX::DependencyProperty::UnsetValue());

    border.RequestedTheme(MUX::ElementTheme::Light);
    wxYield();
    const auto light =
        border.BorderBrush().try_as<MUXM::SolidColorBrush>();
    REQUIRE(light != nullptr);
    const auto lightColour = light.Color();

    border.RequestedTheme(MUX::ElementTheme::Dark);
    wxYield();
    const auto dark =
        border.BorderBrush().try_as<MUXM::SolidColorBrush>();
    REQUIRE(dark != nullptr);
    const auto darkColour = dark.Color();

    CHECK((lightColour.A != darkColour.A ||
           lightColour.R != darkColour.R ||
           lightColour.G != darkColour.G ||
           lightColour.B != darkColour.B));
    CHECK(border.ReadLocalValue(MUXC::Border::BorderBrushProperty()) !=
          MUX::DependencyProperty::UnsetValue());
}

#if wxUSE_STATTEXT

TEST_CASE("wxWinUI StaticText appearance, mnemonic and reset are exact",
          "[winui-appearance][statictext][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStaticText text;
    REQUIRE(text.Create(
        parent, wxID_ANY, "Save && E&xit",
        wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE));
    CheckDefaultCreateSize(text);

    wxWinUIAppearanceSnapshot snapshot;
    REQUIRE(text.WinUIGetAppearanceForTesting(&snapshot));
    CHECK(snapshot.accessKey.CmpNoCase("x") == 0);

    text.SetFont(MakeTestFont());
    text.SetForegroundColour(wxColour(12, 34, 56));
    text.SetBackgroundColour(wxColour(87, 65, 43));
    REQUIRE(text.WinUIGetAppearanceForTesting(&snapshot));
    CheckFontLocals(snapshot, true);
    CHECK(snapshot.hasForeground);
    CHECK(snapshot.hasBackground);

    text.SetFont(wxNullFont);
    text.SetForegroundColour(wxNullColour);
    text.SetBackgroundColour(wxNullColour);
    REQUIRE(text.WinUIGetAppearanceForTesting(&snapshot));
    CheckFontLocals(snapshot, false);
    CHECK_FALSE(snapshot.hasForeground);
    CHECK_FALSE(snapshot.hasBackground);

    const wxSize fixed = text.GetSize();
    text.SetLabel("A much longer &replacement label");
    CHECK(text.GetSize() == fixed);
    REQUIRE(text.WinUIGetAppearanceForTesting(&snapshot));
    CHECK(snapshot.accessKey.CmpNoCase("r") == 0);
}

#if wxUSE_MARKUP

TEST_CASE("wxWinUI StaticText drops stale markup for equal plain text",
          "[winui-appearance][statictext][markup]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStaticText text(
        parent, wxID_ANY, "same",
        wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    REQUIRE(text.SetLabelMarkup("<b>same</b>"));
    CHECK(text.WinUIHasLocalBoldInlineForTesting());

    // m_labelOrig already contains "same": this still has to rebuild the peer
    // because its inline representation changes from styled to plain.
    text.SetLabel("same");
    CHECK(text.GetLabel() == "same");
    CHECK_FALSE(text.WinUIHasLocalBoldInlineForTesting());
}

TEST_CASE("wxWinUI StaticText markup preserves wx mnemonics",
          "[winui-appearance][statictext][markup][mnemonic]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStaticText text(
        parent, wxID_ANY, wxString(),
        wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    REQUIRE(text.SetLabelMarkup("<b>&markup &amp;&amp; value</b>"));

    CHECK(text.GetLabel() == "&markup && value");
    CHECK(text.WinUIGetRenderedTextForTesting() == "markup & value");
    CHECK(text.WinUIHasLocalBoldInlineForTesting());
    CHECK(text.WinUIHasLocalUnderlineInlineForTesting());

    wxWinUIAppearanceSnapshot snapshot;
    REQUIRE(text.WinUIGetAppearanceForTesting(&snapshot));
    CHECK(snapshot.accessKey.CmpNoCase("m") == 0);
}

TEST_CASE("wxWinUI StaticText manually ellipsizes markup at both edges",
          "[winui-appearance][statictext][ellipsis]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    for ( const long ellipsisStyle :
          { wxST_ELLIPSIZE_START, wxST_ELLIPSIZE_MIDDLE } )
    {
        wxStaticText text(
            parent, wxID_ANY, wxString(),
            wxDefaultPosition, parent->FromDIP(wxSize(62, 28)),
            ellipsisStyle | wxST_NO_AUTORESIZE);

        REQUIRE(text.SetLabelMarkup(
            "<b>Alpha beta gamma delta epsilon omega</b>"));
        const wxString visible = text.WinUIGetVisibleLabelForTesting();
        CHECK(visible != text.GetLabel());
        CHECK(visible.length() < text.GetLabel().length());
        CHECK_FALSE(text.WinUIHasLocalBoldInlineForTesting());
        CHECK(text.WinUIGetTextTrimmingForTesting() ==
              static_cast<int>(MUX::TextTrimming::None));
    }

    wxStaticText trailing(
        parent, wxID_ANY, "Alpha beta gamma delta epsilon omega",
        wxDefaultPosition, parent->FromDIP(wxSize(62, 28)),
        wxST_ELLIPSIZE_END | wxST_NO_AUTORESIZE);
    CHECK(trailing.WinUIGetVisibleLabelForTesting() == trailing.GetLabel());
    CHECK(trailing.WinUIGetTextTrimmingForTesting() ==
          static_cast<int>(MUX::TextTrimming::CharacterEllipsis));
}

#endif // wxUSE_MARKUP
#endif // wxUSE_STATTEXT

#if wxUSE_STATBOX

TEST_CASE("wxWinUI StaticBox Create is reentrancy-safe",
          "[winui-appearance][staticbox][create][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("model mutation during shared-host attachment converges")
    {
        wxStaticBox box;
        bool reentered = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window != &box )
                    return;

                reentered = true;
                box.SetLabel("&Newest");
            });

        const bool created =
            box.Create(parent, wxID_ANY, "&Original");
        wxWinUITopLevelHost::TestOnNextSlotAttached({});

        REQUIRE(created);
        CHECK(reentered);
        CHECK(box.GetLabel() == "&Newest");

        wxWinUIAppearanceSnapshot snapshot;
        wxYield();
        REQUIRE(box.WinUIGetAppearanceForTesting(&snapshot));
        CHECK(snapshot.automationName == "Newest");
        CHECK(snapshot.accessKey.CmpNoCase("N") == 0);
        CHECK(snapshot.peerName == "Newest");
    }

    SECTION("destruction during shared-host attachment aborts Create")
    {
        wxStaticBox *box = new wxStaticBox;
        wxStaticBox * const invoking = box;
        bool attached = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                attached = true;
                wxStaticBox * const doomed = box;
                box = nullptr;
                delete doomed;
            });

        const bool created =
            invoking->Create(parent, wxID_ANY, "&Doomed");
        wxWinUITopLevelHost::TestOnNextSlotAttached({});

        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(box == nullptr);
        delete box;
    }
}

TEST_CASE("wxWinUI StaticBox uses theme DIPs and resets appearance",
          "[winui-appearance][staticbox][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStaticBox box;
    REQUIRE(box.Create(parent, wxID_ANY, "&Options"));
    CHECK(box.GetSize().x > 20);
    CHECK(box.GetSize().y > 20);

    double frameTop = -1.0;
    double titleGap = -1.0;
    bool themeBrush = false;
    REQUIRE(box.WinUIGetLayoutForTesting(
        &frameTop, &titleGap, &themeBrush));
    CHECK(frameTop == 8.0);
    CHECK(titleGap == 0.0);
    CHECK(themeBrush);

    wxWinUIAppearanceSnapshot snapshot;
    wxYield();
    REQUIRE(box.WinUIGetAppearanceForTesting(&snapshot));
    CHECK(snapshot.automationName == "Options");
    CHECK(snapshot.accessKey.CmpNoCase("O") == 0);
    CHECK(snapshot.localizedControlType.empty());
    CHECK_FALSE(snapshot.peerLocalizedControlType.empty());
    CHECK(snapshot.hasAutomationPeer);
    CHECK(snapshot.automationControlType ==
          static_cast<int>(MUXAP::AutomationControlType::Group));
    CHECK(snapshot.automationClassName == "wxStaticBox");
    CHECK(snapshot.peerName == "Options");

    box.SetFont(MakeTestFont());
    box.SetForegroundColour(wxColour(31, 73, 127));
    box.SetBackgroundColour(wxColour(119, 83, 47));
    REQUIRE(box.WinUIGetAppearanceForTesting(&snapshot));
    CheckFontLocals(snapshot, true);
    CHECK(snapshot.hasForeground);
    CHECK(snapshot.hasBackground);

    box.SetFont(wxNullFont);
    box.SetForegroundColour(wxNullColour);
    box.SetBackgroundColour(wxNullColour);
    REQUIRE(box.WinUIGetAppearanceForTesting(&snapshot));
    CheckFontLocals(snapshot, false);
    CHECK_FALSE(snapshot.hasForeground);
    CHECK_FALSE(snapshot.hasBackground);
}

#if wxUSE_STATTEXT

TEST_CASE("wxWinUI StaticBox creates its label-window gap immediately",
          "[winui-appearance][staticbox][label-window]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    auto label =
        std::make_unique<wxStaticText>(
            parent, wxID_ANY, "&Controlled options");
    wxStaticBox box;
    REQUIRE(box.Create(parent, wxID_ANY, label.get()));
    // wxStaticBox owns its label window after successful Create().
    label.release();

    double frameTop = -1.0;
    double titleGap = -1.0;
    REQUIRE(box.WinUIGetLayoutForTesting(
        &frameTop, &titleGap, nullptr));
    CHECK(frameTop == 8.0);
    CHECK(titleGap > 8.0);

    wxWinUIAppearanceSnapshot snapshot;
    wxYield();
    REQUIRE(box.WinUIGetAppearanceForTesting(&snapshot));
    CHECK(snapshot.automationName == "Controlled options");
    CHECK(snapshot.accessKey.CmpNoCase("C") == 0);
    CHECK(snapshot.localizedControlType.empty());
    CHECK_FALSE(snapshot.peerLocalizedControlType.empty());
    CHECK(snapshot.hasAutomationPeer);
    CHECK(snapshot.automationControlType ==
          static_cast<int>(MUXAP::AutomationControlType::Group));
    CHECK(snapshot.automationClassName == "wxStaticBox");
    CHECK(snapshot.peerName == "Controlled options");
}

TEST_CASE("wxWinUI StaticBox revalidates a label changed by SetSize",
          "[winui-appearance][staticbox][winui-008d][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("destroyed label")
    {
        auto *label = new wxStaticText(
            parent, wxID_ANY, "&Old label",
            wxDefaultPosition, wxDefaultSize,
            wxST_NO_AUTORESIZE);
        ReentrantLabelStaticBox box;
        REQUIRE(box.Create(parent, wxID_ANY, label));

        unsigned sizeEvents = 0;
        label->Bind(
            wxEVT_SIZE,
            [&](wxSizeEvent&)
            {
                ++sizeEvents;
                box.ReplaceOwnedLabelForTesting(nullptr);
                wxStaticText * const doomed = label;
                label = nullptr;
                delete doomed;
            });

        box.SetFont(MakeTestFont());
        CHECK(sizeEvents == 1);
        CHECK(label == nullptr);
        delete label;
    }

    SECTION("replacement label is not moved as the stale label")
    {
        auto *label = new wxStaticText(
            parent, wxID_ANY, "&Old label",
            wxDefaultPosition, wxDefaultSize,
            wxST_NO_AUTORESIZE);
        ReentrantLabelStaticBox box;
        REQUIRE(box.Create(parent, wxID_ANY, label));

        wxStaticText *replacement = nullptr;
        const wxPoint replacementPosition(73, 31);
        unsigned sizeEvents = 0;
        label->Bind(
            wxEVT_SIZE,
            [&](wxSizeEvent&)
            {
                ++sizeEvents;
                replacement = new wxStaticText(
                    &box, wxID_ANY, "&Replacement",
                    replacementPosition, wxDefaultSize,
                    wxST_NO_AUTORESIZE);
                box.ReplaceOwnedLabelForTesting(replacement);
                wxStaticText * const stale = label;
                label = nullptr;
                delete stale;
            });

        box.SetFont(MakeTestFont());
        REQUIRE(replacement);
        CHECK(sizeEvents == 1);
        CHECK(label == nullptr);
        CHECK(replacement->GetPosition() == replacementPosition);
        delete label;
    }
}

#endif // wxUSE_STATTEXT
#endif // wxUSE_STATBOX

#if wxUSE_RADIOBOX

TEST_CASE("wxWinUI RadioBox appearance and title UIA are exact",
          "[winui-appearance][radiobox][winui-008d][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choices[] = { "&One", "T&wo" };
    wxRadioBox box;
    REQUIRE(box.Create(
        parent, wxID_ANY, "&Numbers",
        wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(choices), choices));
    CHECK(box.GetSize().x > 20);
    CHECK(box.GetSize().y > 20);

    wxWinUIAppearanceSnapshot snapshot;
    bool titleIsRaw = false;
    wxYield();
    REQUIRE(box.WinUIGetAppearanceForTesting(
        &snapshot, &titleIsRaw));
    CHECK(titleIsRaw);
    CHECK(snapshot.hasAutomationPeer);
    CHECK(snapshot.automationControlType ==
          static_cast<int>(MUXAP::AutomationControlType::Group));
    CHECK(snapshot.automationClassName == "wxRadioBox");
    CHECK(snapshot.peerName == "Numbers");
    CHECK(snapshot.localizedControlType.empty());
    CHECK_FALSE(snapshot.peerLocalizedControlType.empty());

    box.SetFont(MakeTestFont());
    box.SetForegroundColour(wxColour(21, 61, 101));
    box.SetBackgroundColour(wxColour(91, 51, 31));
    REQUIRE(box.WinUIGetAppearanceForTesting(
        &snapshot, &titleIsRaw));
    CheckFontLocals(snapshot, true);
    CHECK(snapshot.hasForeground);
    CHECK(snapshot.hasBackground);
    CHECK(titleIsRaw);

    box.SetFont(wxNullFont);
    box.SetForegroundColour(wxNullColour);
    box.SetBackgroundColour(wxNullColour);
    REQUIRE(box.WinUIGetAppearanceForTesting(
        &snapshot, &titleIsRaw));
    CheckFontLocals(snapshot, false);
    CHECK_FALSE(snapshot.hasForeground);
    CHECK_FALSE(snapshot.hasBackground);
    CHECK(titleIsRaw);
}

TEST_CASE("wxWinUI RadioBox SetContent reentry is latest-writer-wins",
          "[winui-appearance][radiobox][winui-008d][reentry]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choices[] = { "&Initial", "&Second" };
    wxRadioBox box(
        parent, wxID_ANY, "&Group",
        wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(choices), choices);

    unsigned long long addedBefore = 0;
    unsigned long long revokedBefore = 0;
    box.WinUIGetCheckedHandlerCountsForTesting(
        &addedBefore, &revokedBefore);

    bool reentered = false;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            if ( window != &box )
                return;

            reentered = true;
            box.SetString(0, "&Newest");
        });
    box.SetString(0, "&Outer");
    wxWinUITopLevelHost::TestOnNextContentCarrierSet({});

    CHECK(reentered);
    CHECK(box.GetString(0) == "&Newest");
    CHECK_FALSE(box.WinUIHasDeferredPeerWriteForTesting());
    CHECK_FALSE(box.WinUIIsPeerProjectionQuarantinedForTesting());

    wxArrayString peerStrings;
    int peerSelection = wxNOT_FOUND;
    unsigned long long peerGeneration = 0;
    REQUIRE(box.WinUIGetPeerStateForTesting(
        &peerStrings, &peerSelection, &peerGeneration));
    REQUIRE(peerStrings.GetCount() == 2);
    CHECK(peerStrings[0] == "Newest");
    CHECK(peerStrings[1] == "Second");
    CHECK(peerSelection == 0);
    CHECK(peerGeneration >= 3);

    unsigned long long addedAfter = 0;
    unsigned long long revokedAfter = 0;
    box.WinUIGetCheckedHandlerCountsForTesting(
        &addedAfter, &revokedAfter);
    CHECK(addedAfter - addedBefore ==
          revokedAfter - revokedBefore);
}

TEST_CASE("wxWinUI RadioBox setters preserve explicit geometry",
          "[winui-appearance][radiobox][winui-008d][geometry]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    LayoutCountingPanel panel(parent);
    const wxString choices[] = { "&One", "&Two" };
    wxRadioBox box(
        &panel, wxID_ANY, "&Initial",
        wxPoint(13, 17), wxSize(257, 113),
        WXSIZEOF(choices), choices);

    const wxPoint position = box.GetPosition();
    const wxSize size = box.GetSize();
    const unsigned layoutCalls = panel.layoutCalls;

    box.SetString(0, "&A much longer item");
    CHECK(box.GetPosition() == position);
    CHECK(box.GetSize() == size);
    CHECK(panel.layoutCalls == layoutCalls);

    box.SetLabel("&A much longer title");
    CHECK(box.GetPosition() == position);
    CHECK(box.GetSize() == size);
    CHECK(panel.layoutCalls == layoutCalls);
}

TEST_CASE("wxWinUI RadioBox Show reentry keeps the latest item writer",
          "[winui-appearance][radiobox][winui-008d][show-reentry]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choices[] = { "&One", "&Two", "&Three" };
    wxRadioBox box(
        parent, wxID_ANY, "&Group",
        wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(choices), choices);

    bool reentered = false;
    box.Bind(
        wxEVT_SHOW,
        [&](wxShowEvent& event)
        {
            if ( !reentered && !event.IsShown() )
            {
                reentered = true;
                CHECK(box.Show(1, false));
            }
            event.Skip();
        });

    box.Hide();
    REQUIRE(reentered);
    CHECK_FALSE(box.IsShown());
    CHECK(box.IsItemShown(0));
    CHECK_FALSE(box.IsItemShown(1));
    CHECK(box.IsItemShown(2));

    CHECK(box.Show());
    CHECK(box.IsShown());
    CHECK(box.IsItemShown(0));
    CHECK(box.IsItemShown(1));
    CHECK(box.IsItemShown(2));
}

TEST_CASE("wxWinUI RadioBox projection is bounded and externally rearmed",
          "[winui-appearance][radiobox][winui-008d][quarantine]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choices[] = { "&One", "&Two" };
    wxRadioBox box(
        parent, wxID_ANY, "&Initial",
        wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(choices), choices);

    unsigned long long addedBefore = 0;
    unsigned long long revokedBefore = 0;
    box.WinUIGetCheckedHandlerCountsForTesting(
        &addedBefore, &revokedBefore);

    RadioBoxStormContext storm{ &box, 0 };
    box.WinUISetNextPeerWriteHookForTesting(
        &ContinueRadioBoxProjectionStorm, &storm);
    YieldingRadioBoxLogTarget yieldingLog;
    box.SetLabel("&Kick");

    const bool quarantined = WaitFor(
        "bounded WinUI RadioBox projection quarantine",
        [&]()
        {
            return box.WinUIIsPeerProjectionQuarantinedForTesting();
        });
    box.WinUISetNextPeerWriteHookForTesting(nullptr, nullptr);

    REQUIRE(quarantined);
    CHECK(yieldingLog.DidYield());
    CHECK(storm.writes == 16);
    CHECK_FALSE(box.WinUIHasDeferredPeerWriteForTesting());

    // A peer-originated selection is already projected by WinUI. It advances
    // the model revision but must not silently re-arm a quarantined, stale
    // structural projection.
    REQUIRE(box.WinUISelectItemForTesting(1));
    CHECK(box.GetSelection() == 1);
    CHECK(box.WinUIIsPeerProjectionQuarantinedForTesting());
    CHECK_FALSE(box.WinUIHasDeferredPeerWriteForTesting());

    unsigned long long addedAfterStorm = 0;
    unsigned long long revokedAfterStorm = 0;
    box.WinUIGetCheckedHandlerCountsForTesting(
        &addedAfterStorm, &revokedAfterStorm);
    CHECK(addedAfterStorm - addedBefore ==
          revokedAfterStorm - revokedBefore);

    const unsigned long long quarantinedRevision =
        box.WinUIGetModelRevisionForTesting();
    box.SetLabel("&Recovered");
    CHECK(box.GetLabel() == "&Recovered");
    CHECK(box.WinUIGetModelRevisionForTesting() >
          quarantinedRevision);
    CHECK_FALSE(box.WinUIIsPeerProjectionQuarantinedForTesting());
    CHECK_FALSE(box.WinUIHasDeferredPeerWriteForTesting());

    wxWinUIAppearanceSnapshot snapshot;
    REQUIRE(box.WinUIGetAppearanceForTesting(&snapshot));
    CHECK(snapshot.peerName == "Recovered");
}

TEST_CASE("wxWinUI RadioBox keeps superseding model on failed replay",
          "[winui-appearance][radiobox][winui-008d][rollback]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choices[] = { "&Initial", "&Second" };
    wxRadioBox box(
        parent, wxID_ANY, "&Group",
        wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(choices), choices);

    unsigned long long addedBefore = 0;
    unsigned long long revokedBefore = 0;
    box.WinUIGetCheckedHandlerCountsForTesting(
        &addedBefore, &revokedBefore);

    bool superseded = false;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            if ( window != &box )
                return;

            superseded = true;
            box.SetString(0, "&Newest");
            wxWinUITopLevelHost::TestFailContentSwap(
                wxWinUITopLevelHost::TestContentFault_Install);
        });
    box.SetString(0, "&Outer");
    wxWinUITopLevelHost::TestOnNextContentCarrierSet({});
    wxWinUITopLevelHost::TestFailContentSwap(0);

    REQUIRE(superseded);
    // The failed replay may leave the last committed peer on "Outer", but
    // the outer setter must not roll the newer wx model back to "Initial".
    CHECK(box.GetString(0) == "&Newest");
    wxArrayString peerStrings;
    REQUIRE(box.WinUIGetPeerStateForTesting(
        &peerStrings, nullptr));
    REQUIRE(peerStrings.GetCount() == 2);
    CHECK(peerStrings[0] == "Outer");

    unsigned long long addedAfter = 0;
    unsigned long long revokedAfter = 0;
    box.WinUIGetCheckedHandlerCountsForTesting(
        &addedAfter, &revokedAfter);
    CHECK(addedAfter - addedBefore ==
          revokedAfter - revokedBefore);

    // A fresh external mutation owns a new budget and converges the peer.
    box.SetString(0, "&Recovered");
    CHECK(box.GetString(0) == "&Recovered");
    peerStrings.Clear();
    REQUIRE(box.WinUIGetPeerStateForTesting(
        &peerStrings, nullptr));
    REQUIRE(peerStrings.GetCount() == 2);
    CHECK(peerStrings[0] == "Recovered");
}

TEST_CASE("wxWinUI RadioBox revokes candidates on SetContent destruction",
          "[winui-appearance][radiobox][winui-008d][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choice[] = { "&Counter" };
    wxRadioBox counter(
        parent, wxID_ANY, "&Counter",
        wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(choice), choice);
    unsigned long long addedBefore = 0;
    unsigned long long revokedBefore = 0;
    counter.WinUIGetCheckedHandlerCountsForTesting(
        &addedBefore, &revokedBefore);

    wxRadioBox *box = new wxRadioBox(
        parent, wxID_ANY, "&Doomed",
        wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(choice), choice);
    wxRadioBox * const invoking = box;
    bool destroyed = false;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            if ( window != invoking )
                return;

            destroyed = true;
            wxRadioBox * const doomed = box;
            box = nullptr;
            delete doomed;
        });
    invoking->SetString(0, "&Candidate");
    wxWinUITopLevelHost::TestOnNextContentCarrierSet({});

    CHECK(destroyed);
    CHECK(box == nullptr);
    unsigned long long addedAfter = 0;
    unsigned long long revokedAfter = 0;
    counter.WinUIGetCheckedHandlerCountsForTesting(
        &addedAfter, &revokedAfter);
    CHECK(addedAfter - addedBefore ==
          revokedAfter - revokedBefore);
    delete box;
}

TEST_CASE("wxWinUI RadioBox stops after terminal flush destruction",
          "[winui-appearance][radiobox][winui-008d][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choices[] = { "&One", "&Two" };
    wxRadioBox *box = new wxRadioBox(
        parent, wxID_ANY, "&Before",
        wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(choices), choices);
    wxRadioBox * const invoking = box;
    unsigned syncCallbacks = 0;

    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            if ( window != invoking )
                return;

            ++syncCallbacks;
            wxRadioBox * const doomed = box;
            box = nullptr;
            delete doomed;
        });
    invoking->SetLabel("&After");
    wxWinUITopLevelHost::TestOnNextSlotSynced({});

    CHECK(syncCallbacks == 1);
    CHECK(box == nullptr);
    delete box;
}

#endif // wxUSE_RADIOBOX

#if wxUSE_STATBMP

TEST_CASE("wxWinUI StaticBitmap reselects DPI bundles without double scaling",
          "[winui-appearance][staticbitmap][dpi]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStaticBitmap bitmap(parent, wxID_ANY, MakeDPIBundle());

    wxSize pixels;
    wxSize dips;
    int stretch = -1;
    unsigned generation = 0;
    bool hasSource = false;

    REQUIRE(bitmap.WinUIRefreshForScaleForTesting(1.0));
    REQUIRE(bitmap.WinUIGetPeerImageStateForTesting(
        &pixels, &dips, &stretch, &generation, &hasSource));
    CHECK(hasSource);
    CHECK(pixels == wxSize(16, 16));
    CHECK(dips == wxSize(16, 16));
    CHECK(stretch == static_cast<int>(MUXM::Stretch::None));
    const unsigned firstGeneration = generation;

    REQUIRE(bitmap.WinUIRefreshForScaleForTesting(2.0));
    REQUIRE(bitmap.WinUIGetPeerImageStateForTesting(
        &pixels, &dips, &stretch, &generation, &hasSource));
    CHECK(hasSource);
    CHECK(pixels == wxSize(32, 32));
    CHECK(dips == wxSize(16, 16));
    CHECK(generation > firstGeneration);

    bitmap.SetScaleMode(wxStaticBitmap::Scale_AspectFit);
    REQUIRE(bitmap.WinUIGetPeerImageStateForTesting(
        nullptr, nullptr, &stretch, nullptr, nullptr));
    CHECK(stretch == static_cast<int>(MUXM::Stretch::Uniform));

    bitmap.SetBitmap(wxBitmapBundle());
    REQUIRE(bitmap.WinUIGetPeerImageStateForTesting(
        nullptr, nullptr, nullptr, nullptr, &hasSource));
    CHECK_FALSE(hasSource);
}

#if wxUSE_STATTEXT

TEST_CASE("wxWinUI StaticText has no post-flush effect after destruction",
          "[winui-appearance][statictext][winui-008d][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    LayoutCountingPanel panel(parent);
    auto *sizer = new wxBoxSizer(wxVERTICAL);
    panel.SetSizer(sizer);
    wxStaticText *text = new wxStaticText(
        &panel, wxID_ANY, "&Before",
        wxDefaultPosition, wxDefaultSize,
        wxST_NO_AUTORESIZE);
    sizer->Add(text);
    panel.Layout();
    const unsigned layoutsBefore = panel.layoutCalls;

    wxStaticText * const invoking = text;
    unsigned syncCallbacks = 0;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            if ( window != invoking )
                return;

            ++syncCallbacks;
            wxStaticText * const doomed = text;
            text = nullptr;
            delete doomed;
        });
    invoking->SetLabel("&After UpdateLayout");
    wxWinUITopLevelHost::TestOnNextSlotSynced({});

    CHECK(syncCallbacks == 1);
    CHECK(text == nullptr);
    CHECK(panel.layoutCalls == layoutsBefore);
    delete text;
}

#endif // wxUSE_STATTEXT

TEST_CASE("wxWinUI StaticBitmap stops after terminal flush destruction",
          "[winui-appearance][staticbitmap][winui-008d][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    LayoutCountingPanel panel(parent);
    auto *sizer = new wxBoxSizer(wxVERTICAL);
    panel.SetSizer(sizer);
    wxStaticBitmap *bitmap = new wxStaticBitmap(
        &panel, wxID_ANY, MakeDPIBundle());
    sizer->Add(bitmap);
    panel.Layout();
    const unsigned layoutsBefore = panel.layoutCalls;

    wxStaticBitmap * const invoking = bitmap;
    unsigned syncCallbacks = 0;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            if ( window != invoking )
                return;

            ++syncCallbacks;
            wxStaticBitmap * const doomed = bitmap;
            bitmap = nullptr;
            delete doomed;
        });
    invoking->SetBitmap(MakeDPIBundle());
    wxWinUITopLevelHost::TestOnNextSlotSynced({});

    CHECK(syncCallbacks == 1);
    CHECK(bitmap == nullptr);
    CHECK(panel.layoutCalls == layoutsBefore);
    delete bitmap;
}

#if wxUSE_STATTEXT && wxUSE_STATBOX

TEST_CASE("wxWinUI static appearance peers survive repeated teardown",
          "[winui-appearance][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bundle = MakeDPIBundle();

    for ( int i = 0; i < 50; ++i )
    {
        wxStaticText text(parent, wxID_ANY, "&Text");
        text.SetFont(MakeTestFont());

        wxStaticBox box(parent, wxID_ANY, "&Group");
        box.SetBackgroundColour(wxColour(20 + i, 30, 40));

        wxStaticBitmap bitmap(parent, wxID_ANY, bundle);
        REQUIRE(bitmap.WinUIRefreshForScaleForTesting(
            i % 2 ? 2.0 : 1.0));
    }
}

#endif // wxUSE_STATTEXT && wxUSE_STATBOX
#endif // wxUSE_STATBMP

#endif // __WXWINUI__ && wxUSE_WINUI3
