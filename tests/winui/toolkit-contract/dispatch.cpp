// Include every directly dispatched public control header, not private WinUI
// headers. This also detects duplicate public definitions and include-order
// coupling when both __WXWINUI__ and __WXMSW__ are defined.
#include "wx/activityindicator.h"
#include "wx/bmpcbox.h"
#include "wx/button.h"
#include "wx/calctrl.h"
#include "wx/checkbox.h"
#include "wx/checklst.h"
#include "wx/choice.h"
#include "wx/clrpicker.h"
#include "wx/colordlg.h"
#include "wx/combobox.h"
#include "wx/datectrl.h"
#include "wx/gauge.h"
#include "wx/hyperlink.h"
#include "wx/infobar.h"
#include "wx/listbox.h"
#include "wx/msgdlg.h"
#include "wx/notebook.h"
#include "wx/radiobox.h"
#include "wx/radiobut.h"
#include "wx/scrolbar.h"
#include "wx/slider.h"
#include "wx/spinbutt.h"
#include "wx/spinctrl.h"
#include "wx/srchctrl.h"
#include "wx/statbmp.h"
#include "wx/statbox.h"
#include "wx/statline.h"
#include "wx/stattext.h"
#include "wx/statusbr.h"
#include "wx/textctrl.h"
#include "wx/textdlg.h"
#include "wx/tglbtn.h"
#include "wx/timectrl.h"
#include "wx/toolbar.h"
#include "wx/treectrl.h"
#include "contract.h"

#if WX_EXPECT_WINUI
    #if !defined(_WX_WINUI_RADIOBOX_H_) || \
        !defined(_WX_WINUI_SPINCTRL_H_) || !defined(_WX_WINUI_SPINBUTT_H_)
        #error WinUI dispatch was shadowed by MSW
    #endif
#else
    #if defined(_WX_WINUI_RADIOBOX_H_) || \
        defined(_WX_WINUI_SPINCTRL_H_) || defined(_WX_WINUI_SPINBUTT_H_)
        #error WinUI declarations leaked into the ordinary MSW toolkit
    #endif
#endif

#if wxUSE_BUTTON || wxUSE_TOGGLEBTN || wxUSE_STATTEXT || wxUSE_STATBMP || \
    wxUSE_STATBOX || wxUSE_GAUGE || wxUSE_SCROLLBAR || wxUSE_SPINBTN
namespace
{

// Detect the old public operations and types, not private support declarations.
// These checks also run for ordinary MSW, where the WinUI-only operations
// must never appear. They require neither test support nor a control instance.
template<template<typename> class Operation, typename T, typename = void>
struct HasPublicTestOperation : std::false_type {};

template<template<typename> class Operation, typename T>
struct HasPublicTestOperation<Operation, T, std::void_t<Operation<T>>>
    : std::true_type {};

#if wxUSE_BUTTON || wxUSE_TOGGLEBTN
template<typename T>
using QueueClickForTesting =
    decltype(std::declval<T&>().WinUIQueueClickForTesting());

template<typename T>
using LiveCallbackStateCountForTesting =
    decltype(T::WinUIGetLiveCallbackStateCountForTesting());

template<typename T>
using PeerInvokeAttemptCountForTesting =
    decltype(T::WinUIGetPeerInvokeAttemptCountForTesting());

template<typename T>
using ProjectBitmapForTesting =
    decltype(std::declval<T&>().WinUIProjectBitmapStateForTesting(
        wxAnyButton::State_Normal, 1.0));

template<typename T>
using ButtonBitmapProjectionForTesting =
    decltype(std::declval<const T&>().WinUIGetPeerBitmapProjectionForTesting(
        nullptr, nullptr, nullptr, nullptr));

template<typename T>
using ToggleBitmapProjectionForTesting =
    decltype(std::declval<const T&>().WinUIGetPeerBitmapProjectionForTesting(
        nullptr, nullptr, nullptr));
#endif

#if wxUSE_BUTTON
static_assert(!HasPublicTestOperation<QueueClickForTesting, wxButton>::value,
              "wxButton must not expose the test Click helper");
static_assert(!HasPublicTestOperation<LiveCallbackStateCountForTesting,
                                     wxButton>::value,
              "wxButton must not expose the test callback-state counter");
static_assert(!HasPublicTestOperation<PeerInvokeAttemptCountForTesting,
                                     wxButton>::value,
              "wxButton must not expose the test peer-invocation counter");
static_assert(!HasPublicTestOperation<ProjectBitmapForTesting, wxButton>::value,
              "wxButton must not expose the test bitmap projection helper");
static_assert(!HasPublicTestOperation<ButtonBitmapProjectionForTesting,
                                     wxButton>::value,
              "wxButton must not expose the test bitmap projection inspector");
#endif

#if wxUSE_TOGGLEBTN
static_assert(!HasPublicTestOperation<ProjectBitmapForTesting,
                                     wxBitmapToggleButton>::value,
              "wxBitmapToggleButton must not expose the test projection helper");
static_assert(!HasPublicTestOperation<ToggleBitmapProjectionForTesting,
                                     wxBitmapToggleButton>::value,
              "wxBitmapToggleButton must not expose the test projection inspector");
#endif

#if wxUSE_STATTEXT || wxUSE_STATBOX
template<typename T>
using AppearanceForTesting =
    decltype(std::declval<const T&>().WinUIGetAppearanceForTesting(nullptr));
#endif

#if wxUSE_STATTEXT
template<typename T>
using VisibleLabelForTesting =
    decltype(std::declval<const T&>().WinUIGetVisibleLabelForTesting());

template<typename T>
using RenderedTextForTesting =
    decltype(std::declval<const T&>().WinUIGetRenderedTextForTesting());

template<typename T>
using TextTrimmingForTesting =
    decltype(std::declval<const T&>().WinUIGetTextTrimmingForTesting());

template<typename T>
using LocalBoldInlineForTesting =
    decltype(std::declval<const T&>().WinUIHasLocalBoldInlineForTesting());

template<typename T>
using LocalUnderlineInlineForTesting =
    decltype(std::declval<const T&>().WinUIHasLocalUnderlineInlineForTesting());

static_assert(!HasPublicTestOperation<AppearanceForTesting, wxStaticText>::value,
              "wxStaticText must not expose the test appearance inspector");
static_assert(!HasPublicTestOperation<VisibleLabelForTesting, wxStaticText>::value,
              "wxStaticText must not expose the test visible-label inspector");
static_assert(!HasPublicTestOperation<RenderedTextForTesting, wxStaticText>::value,
              "wxStaticText must not expose the test rendered-text inspector");
static_assert(!HasPublicTestOperation<TextTrimmingForTesting, wxStaticText>::value,
              "wxStaticText must not expose the test text-trimming inspector");
static_assert(!HasPublicTestOperation<LocalBoldInlineForTesting,
                                     wxStaticText>::value,
              "wxStaticText must not expose the test bold-inline inspector");
static_assert(!HasPublicTestOperation<LocalUnderlineInlineForTesting,
                                     wxStaticText>::value,
              "wxStaticText must not expose the test underline-inline inspector");
#endif

#if wxUSE_STATBMP
template<typename T>
using RefreshForScaleForTesting =
    decltype(std::declval<T&>().WinUIRefreshForScaleForTesting(1.0));

template<typename T>
using PeerImageStateForTesting =
    decltype(std::declval<const T&>().WinUIGetPeerImageStateForTesting(
        nullptr, nullptr, nullptr, nullptr, nullptr));

static_assert(!HasPublicTestOperation<RefreshForScaleForTesting,
                                     wxStaticBitmap>::value,
              "wxStaticBitmap must not expose the test scale-refresh helper");
static_assert(!HasPublicTestOperation<PeerImageStateForTesting,
                                     wxStaticBitmap>::value,
              "wxStaticBitmap must not expose the test image-state inspector");
#endif

#if wxUSE_STATBOX
template<typename T>
using StaticBoxLayoutForTesting =
    decltype(std::declval<const T&>().WinUIGetLayoutForTesting(
        nullptr, nullptr, nullptr));

static_assert(!HasPublicTestOperation<AppearanceForTesting, wxStaticBox>::value,
              "wxStaticBox must not expose the test appearance inspector");
static_assert(!HasPublicTestOperation<StaticBoxLayoutForTesting,
                                     wxStaticBox>::value,
              "wxStaticBox must not expose the test layout inspector");
#endif

#if wxUSE_GAUGE
template<typename T>
using GaugePeerStateForTesting =
    decltype(std::declval<const T&>().WinUIGetPeerStateForTesting(
        nullptr, nullptr, nullptr, nullptr));

template<typename T>
using AppProgressForTesting =
    decltype(std::declval<const T&>().WinUIHasAppProgressForTesting());

static_assert(!HasPublicTestOperation<GaugePeerStateForTesting, wxGauge>::value,
              "wxGauge must not expose the test peer-state inspector");
static_assert(!HasPublicTestOperation<AppProgressForTesting, wxGauge>::value,
              "wxGauge must not expose the test app-progress inspector");
#endif

#if wxUSE_SCROLLBAR
// Check the member itself independently of the action type: making just the
// enum private must not hide a still-public test operation from this probe.
template<typename T>
using ApplyPeerActionForTesting = decltype(&T::WinUIApplyPeerActionForTesting);

template<typename T>
using ScrollBarPeerStateForTesting =
    decltype(std::declval<const T&>().WinUIGetPeerStateForTesting(
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));

template<typename T>
using ScrollBarPeerAction = typename T::WinUIPeerAction;

static_assert(!HasPublicTestOperation<ApplyPeerActionForTesting,
                                     wxScrollBar>::value,
              "wxScrollBar must not expose the test peer-action helper");
static_assert(!HasPublicTestOperation<ScrollBarPeerStateForTesting,
                                     wxScrollBar>::value,
              "wxScrollBar must not expose the test peer-state inspector");
static_assert(!HasPublicTestOperation<ScrollBarPeerAction, wxScrollBar>::value,
              "wxScrollBar must not expose the implementation-only action type");
#endif

#if wxUSE_SPINBTN
template<typename T>
using StepForTesting = decltype(std::declval<T&>().WinUIStepForTesting(1));

template<typename T>
using PeerLayoutForTesting =
    decltype(std::declval<const T&>().WinUIGetPeerLayoutForTesting(
        nullptr, nullptr, nullptr));

template<typename T>
using AutomationForTesting =
    decltype(std::declval<const T&>().WinUIGetAutomationForTesting(
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));

template<typename T>
using InvokeArrowForTesting =
    decltype(std::declval<T&>().WinUIInvokeArrowForTesting(1));

static_assert(!HasPublicTestOperation<StepForTesting, wxSpinButton>::value,
              "wxSpinButton must not expose the test step helper");
static_assert(!HasPublicTestOperation<PeerLayoutForTesting, wxSpinButton>::value,
              "wxSpinButton must not expose the test peer-layout inspector");
static_assert(!HasPublicTestOperation<AutomationForTesting, wxSpinButton>::value,
              "wxSpinButton must not expose the test automation inspector");
static_assert(!HasPublicTestOperation<InvokeArrowForTesting, wxSpinButton>::value,
              "wxSpinButton must not expose the test arrow-invocation helper");
#endif

} // anonymous namespace
#endif // any of the controls checked above
