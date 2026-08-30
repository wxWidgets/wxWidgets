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

namespace
{
struct RemovedPublicPeerWriteHook {};
}

// Global names cannot be checked by dependent-member SFINAE. Redeclaring the
// historical hook aliases to this consumer-only sentinel must compile: an
// installed declaration of any of these aliases would conflict with it.
#if wxUSE_RADIOBOX
using wxWinUIRadioBoxPeerWriteHookForTesting = RemovedPublicPeerWriteHook;
#endif
#if wxUSE_ACTIVITYINDICATOR
using wxWinUIActivityPeerWriteHookForTesting = RemovedPublicPeerWriteHook;
#endif
#if wxUSE_HYPERLINKCTRL
using wxWinUIHyperlinkPeerWriteHookForTesting = RemovedPublicPeerWriteHook;
#endif
#if wxUSE_INFOBAR
using wxWinUIInfoBarContentWriteHookForTesting = RemovedPublicPeerWriteHook;
#endif
#if wxUSE_STATUSBAR
using wxWinUIStatusBarReentryPointForTesting = RemovedPublicPeerWriteHook;
using wxWinUIStatusBarReentryHookForTesting = RemovedPublicPeerWriteHook;
using wxWinUIStatusBarSizeGripSnapshot = RemovedPublicPeerWriteHook;
using wxWinUIStatusBarResizeActionHookForTesting = RemovedPublicPeerWriteHook;
#endif

#if wxUSE_CHOICE
using wxWinUIPopupRetirementCoreProbeForTesting = RemovedPublicPeerWriteHook;
#endif

#if wxUSE_BUTTON || wxUSE_TOGGLEBTN || wxUSE_STATTEXT || wxUSE_STATBMP || \
    wxUSE_STATBOX || wxUSE_GAUGE || wxUSE_SCROLLBAR || wxUSE_SPINBTN || \
    wxUSE_RADIOBOX || wxUSE_ACTIVITYINDICATOR || wxUSE_HYPERLINKCTRL || \
    wxUSE_COLOURPICKERCTRL || wxUSE_INFOBAR || wxUSE_STATUSBAR || \
    wxUSE_LISTBOX || wxUSE_TREECTRL || wxUSE_CHOICE || wxUSE_COMBOBOX || \
    wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL || wxUSE_CALENDARCTRL || \
    wxUSE_SLIDER || wxUSE_SPINCTRL || wxUSE_TEXTCTRL || wxUSE_SEARCHCTRL || \
    wxUSE_TEXTDLG
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

#if wxUSE_RADIOBOX || wxUSE_ACTIVITYINDICATOR || wxUSE_HYPERLINKCTRL
// Detect the members independently of their hook argument types: removing
// only a public alias must not conceal a still-public operation from the test.
template<typename T>
using NextPeerWriteHookForTesting =
    decltype(&T::WinUISetNextPeerWriteHookForTesting);

template<typename T>
using DeferredPeerWriteForTesting =
    decltype(&T::WinUIHasDeferredPeerWriteForTesting);

template<typename T>
using PeerProjectionQuarantineForTesting =
    decltype(&T::WinUIIsPeerProjectionQuarantinedForTesting);

template<typename T>
using ModelRevisionForTesting = decltype(&T::WinUIGetModelRevisionForTesting);
#endif

#if wxUSE_RADIOBOX
template<typename T>
using RadioBoxAppearanceForTesting = decltype(&T::WinUIGetAppearanceForTesting);

template<typename T>
using RadioBoxPeerStateForTesting = decltype(&T::WinUIGetPeerStateForTesting);

template<typename T>
using SelectItemForTesting = decltype(&T::WinUISelectItemForTesting);

template<typename T>
using CheckedHandlerCountsForTesting =
    decltype(&T::WinUIGetCheckedHandlerCountsForTesting);

static_assert(!HasPublicTestOperation<RadioBoxAppearanceForTesting,
                                     wxRadioBox>::value,
              "wxRadioBox must not expose the test appearance inspector");
static_assert(!HasPublicTestOperation<RadioBoxPeerStateForTesting,
                                     wxRadioBox>::value,
              "wxRadioBox must not expose the test peer-state inspector");
static_assert(!HasPublicTestOperation<SelectItemForTesting, wxRadioBox>::value,
              "wxRadioBox must not expose the test item-selection helper");
static_assert(!HasPublicTestOperation<NextPeerWriteHookForTesting,
                                     wxRadioBox>::value,
              "wxRadioBox must not expose the test peer-write hook setter");
static_assert(!HasPublicTestOperation<DeferredPeerWriteForTesting,
                                     wxRadioBox>::value,
              "wxRadioBox must not expose the test deferred-write inspector");
static_assert(!HasPublicTestOperation<PeerProjectionQuarantineForTesting,
                                     wxRadioBox>::value,
              "wxRadioBox must not expose the test quarantine inspector");
static_assert(!HasPublicTestOperation<ModelRevisionForTesting,
                                     wxRadioBox>::value,
              "wxRadioBox must not expose the test model-revision inspector");
static_assert(!HasPublicTestOperation<CheckedHandlerCountsForTesting,
                                     wxRadioBox>::value,
              "wxRadioBox must not expose the test checked-handler counters");
#endif

#if wxUSE_ACTIVITYINDICATOR || wxUSE_HYPERLINKCTRL
template<typename T>
using ControlStateForTesting = decltype(&T::WinUIGetStateForTesting);
#endif

#if wxUSE_ACTIVITYINDICATOR
static_assert(!HasPublicTestOperation<ControlStateForTesting,
                                     wxActivityIndicator>::value,
              "wxActivityIndicator must not expose the test state inspector");
static_assert(!HasPublicTestOperation<NextPeerWriteHookForTesting,
                                     wxActivityIndicator>::value,
              "wxActivityIndicator must not expose the test peer-write hook setter");
static_assert(!HasPublicTestOperation<DeferredPeerWriteForTesting,
                                     wxActivityIndicator>::value,
              "wxActivityIndicator must not expose the test deferred-write inspector");
static_assert(!HasPublicTestOperation<PeerProjectionQuarantineForTesting,
                                     wxActivityIndicator>::value,
              "wxActivityIndicator must not expose the test quarantine inspector");
static_assert(!HasPublicTestOperation<ModelRevisionForTesting,
                                     wxActivityIndicator>::value,
              "wxActivityIndicator must not expose the test model-revision inspector");
#endif

#if wxUSE_HYPERLINKCTRL || (WX_EXPECT_WINUI && wxUSE_COLOURPICKERCTRL)
template<typename T>
using LivePeerCallbackCountForTesting =
    decltype(&T::WinUIGetLiveCallbackStateCountForTesting);
#endif

#if wxUSE_HYPERLINKCTRL
template<typename T>
using InvokeForTesting = decltype(&T::WinUIInvokeForTesting);

template<typename T>
using InteractiveRectForTesting = decltype(&T::WinUIGetInteractiveRectForTesting);

template<typename T>
using HitTestForTesting = decltype(&T::WinUIHitTestForTesting);

template<typename T>
using InvokeAtForTesting = decltype(&T::WinUIInvokeAtForTesting);

template<typename T>
using PointerOverForTesting = decltype(&T::WinUISetPointerOverForTesting);

template<typename T>
using CopyURLForTesting = decltype(&T::WinUICopyURLForTesting);

static_assert(!HasPublicTestOperation<InvokeForTesting, wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test invocation helper");
static_assert(!HasPublicTestOperation<InteractiveRectForTesting,
                                     wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test interactive-rect inspector");
static_assert(!HasPublicTestOperation<HitTestForTesting, wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test hit-test helper");
static_assert(!HasPublicTestOperation<InvokeAtForTesting, wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test coordinate-invocation helper");
static_assert(!HasPublicTestOperation<PointerOverForTesting,
                                     wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test pointer-over helper");
static_assert(!HasPublicTestOperation<ControlStateForTesting,
                                     wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test state inspector");
static_assert(!HasPublicTestOperation<CopyURLForTesting, wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the unused test CopyURL helper");
static_assert(!HasPublicTestOperation<NextPeerWriteHookForTesting,
                                     wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test peer-write hook setter");
static_assert(!HasPublicTestOperation<DeferredPeerWriteForTesting,
                                     wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test deferred-write inspector");
static_assert(!HasPublicTestOperation<PeerProjectionQuarantineForTesting,
                                     wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test quarantine inspector");
static_assert(!HasPublicTestOperation<ModelRevisionForTesting,
                                     wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test model-revision inspector");
static_assert(!HasPublicTestOperation<LivePeerCallbackCountForTesting,
                                     wxHyperlinkCtrl>::value,
              "wxHyperlinkCtrl must not expose the test callback-state counter");
#endif

#if WX_EXPECT_WINUI && wxUSE_COLOURPICKERCTRL
template<typename T>
using PeerColourForTesting = decltype(&T::WinUISetPeerColourForTesting);

template<typename T>
using ColourButtonPeerStateForTesting = decltype(&T::WinUIGetPeerStateForTesting);

template<typename T>
using DeliverClosedForTesting = decltype(&T::WinUIDeliverClosedForTesting);

static_assert(!HasPublicTestOperation<PeerColourForTesting,
                                     wxWinUIColourButton>::value,
              "wxWinUIColourButton must not expose the test peer-colour helper");
static_assert(!HasPublicTestOperation<ColourButtonPeerStateForTesting,
                                     wxWinUIColourButton>::value,
              "wxWinUIColourButton must not expose the test peer-state inspector");
static_assert(!HasPublicTestOperation<DeliverClosedForTesting,
                                     wxWinUIColourButton>::value,
              "wxWinUIColourButton must not expose the test close-delivery helper");
static_assert(!HasPublicTestOperation<LivePeerCallbackCountForTesting,
                                     wxWinUIColourButton>::value,
              "wxWinUIColourButton must not expose the test callback-state counter");
#endif

#if wxUSE_INFOBAR
template<typename T>
using InfoBarPeerOpenTestOperation = decltype(&T::WinUIIsPeerOpen);

static_assert(!HasPublicTestOperation<InfoBarPeerOpenTestOperation,
                                     wxInfoBar>::value,
              "wxInfoBar must not expose WinUIIsPeerOpen");

template<typename T>
using InfoBarCloseButtonTestOperation = decltype(&T::WinUIClickCloseButton);

static_assert(!HasPublicTestOperation<InfoBarCloseButtonTestOperation,
                                     wxInfoBar>::value,
              "wxInfoBar must not expose WinUIClickCloseButton");

template<typename T>
using InfoBarButtonTestOperation = decltype(&T::WinUIClickButtonForTesting);

static_assert(!HasPublicTestOperation<InfoBarButtonTestOperation,
                                     wxInfoBar>::value,
              "wxInfoBar must not expose WinUIClickButtonForTesting");

template<typename T>
using InfoBarContentWriteHookTestOperation = decltype(&T::WinUISetNextContentWriteHookForTesting);

static_assert(!HasPublicTestOperation<InfoBarContentWriteHookTestOperation,
                                     wxInfoBar>::value,
              "wxInfoBar must not expose WinUISetNextContentWriteHookForTesting");

template<typename T>
using InfoBarDeferredProjectionTestOperation = decltype(&T::WinUIHasDeferredContentProjectionForTesting);

static_assert(!HasPublicTestOperation<InfoBarDeferredProjectionTestOperation,
                                     wxInfoBar>::value,
              "wxInfoBar must not expose WinUIHasDeferredContentProjectionForTesting");

template<typename T>
using InfoBarProjectionQuarantineTestOperation = decltype(&T::WinUIIsContentProjectionQuarantinedForTesting);

static_assert(!HasPublicTestOperation<InfoBarProjectionQuarantineTestOperation,
                                     wxInfoBar>::value,
              "wxInfoBar must not expose WinUIIsContentProjectionQuarantinedForTesting");

#endif

#if wxUSE_STATUSBAR
template<typename T>
using StatusBarHasSizeGripTestOperation = decltype(&T::WinUIHasSizeGripForTesting);

static_assert(!HasPublicTestOperation<StatusBarHasSizeGripTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIHasSizeGripForTesting");

template<typename T>
using StatusBarSizeGripStateTestOperation = decltype(&T::WinUIGetSizeGripStateForTesting);

static_assert(!HasPublicTestOperation<StatusBarSizeGripStateTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIGetSizeGripStateForTesting");

template<typename T>
using StatusBarResizeHookTestOperation = decltype(&T::WinUISetResizeActionHookForTesting);

static_assert(!HasPublicTestOperation<StatusBarResizeHookTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUISetResizeActionHookForTesting");

template<typename T>
using StatusBarInvokeSizeGripTestOperation = decltype(&T::WinUIInvokeSizeGripForTesting);

static_assert(!HasPublicTestOperation<StatusBarInvokeSizeGripTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIInvokeSizeGripForTesting");

template<typename T>
using StatusBarTopLevelMaximizedTestOperation = decltype(&T::WinUISetTopLevelMaximizedForTesting);

static_assert(!HasPublicTestOperation<StatusBarTopLevelMaximizedTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUISetTopLevelMaximizedForTesting");

template<typename T>
using StatusBarFieldStateTestOperation = decltype(&T::WinUIGetFieldStateForTesting);

static_assert(!HasPublicTestOperation<StatusBarFieldStateTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIGetFieldStateForTesting");

template<typename T>
using StatusBarAppearanceTestOperation = decltype(&T::WinUIGetAppearanceForTesting);

static_assert(!HasPublicTestOperation<StatusBarAppearanceTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIGetAppearanceForTesting");

template<typename T>
using StatusBarThemeBordersTestOperation = decltype(&T::WinUIUsesThemeBordersForTesting);

static_assert(!HasPublicTestOperation<StatusBarThemeBordersTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIUsesThemeBordersForTesting");

template<typename T>
using StatusBarReentryHookTestOperation = decltype(&T::WinUISetNextReentryHookForTesting);

static_assert(!HasPublicTestOperation<StatusBarReentryHookTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUISetNextReentryHookForTesting");

template<typename T>
using StatusBarModelRevisionTestOperation = decltype(&T::WinUIGetModelRevisionForTesting);

static_assert(!HasPublicTestOperation<StatusBarModelRevisionTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIGetModelRevisionForTesting");

template<typename T>
using StatusBarDeferredRebuildTestOperation = decltype(&T::WinUIHasDeferredRebuildForTesting);

static_assert(!HasPublicTestOperation<StatusBarDeferredRebuildTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIHasDeferredRebuildForTesting");

template<typename T>
using StatusBarRebuildQuarantineTestOperation = decltype(&T::WinUIIsRebuildQuarantinedForTesting);

static_assert(!HasPublicTestOperation<StatusBarRebuildQuarantineTestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIIsRebuildQuarantinedForTesting");

template<typename T>
using StatusBarDeliverDPITestOperation = decltype(&T::WinUIDeliverDPIChangedForTesting);

static_assert(!HasPublicTestOperation<StatusBarDeliverDPITestOperation,
                                     wxStatusBar>::value,
              "wxStatusBar must not expose WinUIDeliverDPIChangedForTesting");

#endif

// Each removed declaration is checked independently: a missing typedef must
// not accidentally make a still-public test method look absent.
#define WX_ASSERT_NO_PUBLIC_TEST_METHOD(Class, Method) \
    template<typename T> \
    using Class##_##Method = decltype(&T::Method); \
    static_assert(!HasPublicTestOperation<Class##_##Method, Class>::value, \
                  #Class " must not expose " #Method)

#define WX_ASSERT_NO_PUBLIC_TEST_TYPE(Class, Type) \
    template<typename T> \
    using Class##_##Type = typename T::Type; \
    static_assert(!HasPublicTestOperation<Class##_##Type, Class>::value, \
                  #Class " must not expose type " #Type)

#if wxUSE_LISTBOX
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIGetItemIdForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIGetItemPeerIdentityForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIGetControlWidthDIPsForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIGetPeerCountForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIPoisonPeerForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUISetPeerSelectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIFocusPeerItemForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIDoubleTapPeerForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUISetPeerCheckForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIActivatePeerCheckForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUITogglePeerViaAutomationForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUITogglePeerWithKeyboardForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIDispatchCheckKeyForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIGetPeerCheckForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIGetScrollPresentationForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIGetPeerLayoutDirectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIGetItemPresentationForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUIGetThemePresentationForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxListBox, WinUISetPeerThemeForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxListBox, WinUIScrollMetricsForTesting);
#endif

#if wxUSE_TREECTRL
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIFailNextPeerMutationForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIFailPeerMutationsForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUISelectPeerItemForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIFocusPeerItemForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUISetPeerExpandedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIIsPeerExpandedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIKeyDownForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIBeginInternalDragForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUICompleteInternalDragForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUICompletePeerDragForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIScheduleLabelEditForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIFireLabelEditDelayForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIClickStateImageForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIDoubleClickItemForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIRightClickItemForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIInvokeItemForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIPointerPressedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIPointerMovedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIPointerReleasedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIQueueSelectionCorrectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIIsPeerSelectionChangeForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIIsPeerStructureRepairPendingForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetItemPeerIdentityForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIIsItemAttachedToPeerForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetPeerChildCountForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetMeasuredItemPartsForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetPeerIndentForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIIsPeerDropHighlightedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetPeerAutomationNameForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetPeerItemTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetPeerItemImageIdentityForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIRefreshForScaleForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetPeerItemImageProjectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetModelStatsForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIResetModelStatsForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIClosePeerForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTreeCtrl, WinUIGetLiveCallbackStateCountForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxTreeCtrl, WinUIPeerMutationForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxTreeCtrl, WinUIDragCompletionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxTreeCtrl, WinUIModelStats);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxTreeCtrl, WinUIMeasuredItemParts);
#endif

#if wxUSE_DATEPICKCTRL
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUISetPeerDateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIClearPeerDateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIGetPeerStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIGetDefaultPeerRangeForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIGetPeerDateFormatForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIUsesDropdownForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIGetSpinFieldsForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUISetSpinPartForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIGetSpinPartForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIGetSpinYearTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIGetLocaleDatePatternForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUIGetPeerNullTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxDatePickerCtrl, WinUISetLanguageForTesting);
#endif

#if wxUSE_TIMEPICKCTRL
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTimePickerCtrl, WinUISetPeerTimeForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTimePickerCtrl, WinUIGetPeerTimeForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTimePickerCtrl, WinUIGetLocaleTimePatternForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTimePickerCtrl, WinUIGetTimeFieldOrderForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTimePickerCtrl, WinUIGetHourSpinBindingForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTimePickerCtrl, WinUISetHourLoadedHookForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTimePickerCtrl, WinUISetLanguageForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxTimePickerCtrl, WinUIHourLoadedHookForTesting);
#endif

#if wxUSE_CALENDARCTRL
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUISetPeerDateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetPeerStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetDefaultPeerRangeForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIDoubleTapDateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIDoubleTapNonDayForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIIsMarkedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIIsHolidayForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetAppliedDensityColourForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUISetTodayForegroundForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIUseSystemMarkColourFallbackForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIDeliverThemeChangedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIHasThemeChangedHandlerForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetDateClientPointForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetPeerRightToLeftForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetPeerCalendarStyleForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetWeekNumberForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetWeekRefreshRunCountForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIRequestWeekRefreshForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUISetWeekRefreshFailuresForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUISetWeekRefreshPendingPassesForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUISetWeekRefreshPeerMutationPassesForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIRequestDayLayoutValidationForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIRequestDayLayoutValidationFailureForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetWeekRefreshRecoveryForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIResolveActiveMonthTopologyForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUIGetStableLayoutTicketForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxCalendarCtrl, WinUISetLanguageForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxCalendarCtrl, WinUIWeekRefreshRecoveryForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxCalendarCtrl, WinUIDayLayoutValidationFailureForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxCalendarCtrl, WinUIMonthTopologyDayForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxCalendarCtrl, WinUIMonthTopologyResultForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxCalendarCtrl, WinUILayoutTicketForTesting);
#endif

#if wxUSE_SLIDER
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIApplyInputForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIApplyPointerInputForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUISetUnclassifiedPeerValueForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIGetPeerStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIGetDecorationsForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIHasTickForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIRefreshForScaleForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIGetHostScaleStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIRefreshVisualStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIGetLastConvergenceFailureForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIDeliverThemeChangedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIRetemplatePeerForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSlider, WinUIGetVisualStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxSlider, WinUIInput);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxSlider, WinUISelectionVisual);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxSlider, WinUIActualTheme);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxSlider, WinUIConvergenceFailure);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxSlider, WinUIRetemplateHookForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxSlider, WinUIVisualRect);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxSlider, WinUIVisualState);
#endif

#if wxUSE_SPINCTRL
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrl, WinUISetPeerValueForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrl, WinUISetPeerTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrl, WinUIGetPeerStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrl, WinUIGetPeerSelectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrl, WinUIRetemplateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrl, WinUIEnterForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrl, WinUIStepForTesting);
#endif

#if wxUSE_SPINCTRL
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrlDouble, WinUISetPeerValueForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrlDouble, WinUISetPeerTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrlDouble, WinUIGetPeerStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrlDouble, WinUIGetPeerSelectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrlDouble, WinUIRetemplateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrlDouble, WinUIEnterForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSpinCtrlDouble, WinUIStepForTesting);
#endif

#if wxUSE_CHOICE
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxChoice, WinUIGetItemIdForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxChoice, WinUIGetItemPeerIdentityForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxChoice, WinUIGetItemPeerBitmapStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxChoice, WinUISelectPeerItemForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxChoice, WinUISetDropDownForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxChoice, WinUIIsPeerDropDownOpenForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxChoice, WinUIGetPopupReopenSnapshotForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxChoice, WinUIProbePopupRetirementCoreForTesting);
#endif

#if wxUSE_COMBOBOX
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUISetPeerTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUISetPeerTextViaAutomationForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUISelectPeerItemForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIRetemplateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIRunTemplateLayoutEdgeForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIQueueTemplateLayoutResolutionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUISetEditSelectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUISetRawEditSelectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIChainPendingRangeCallbacksForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUISetRawEditSelectionAndDestroyForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetTemplateStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetTemplatePeerSnapshotForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetTextEntryPeerStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetAutoCompleteSuggestionCountForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetAutoCompleteSuggestionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetAutoCompleteActiveSuggestionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIInvokeAutoCompleteSuggestionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIInvokeClipboardCommandForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetDiagnosticSnapshotForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetSimplePeerSnapshotForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetSimplePeerStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUINavigateSimpleListForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, WinUIGetSimplePageSizeForTesting);
// These production continuations retain private forced-transition parameters.
// The installed consumer must not be able to invoke them either.
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, OnPeerLayoutUpdated);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxComboBox, QueueEditPartResolutionAtLayoutEdge);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUIRangeSequenceSnapshot);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUIRangeDestructionSnapshot);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUITemplatePeerSnapshot);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUITemplatePeerState);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUITemplatePeerPhase);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUIPeerActualTheme);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUIAutomationTestStage);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUIDiagnosticSnapshot);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUISimplePeerSnapshot);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxComboBox, WinUISimplePeerState);
#endif

#if wxUSE_TEXTCTRL
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIProcessEnterForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIProcessTabForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIPasteTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIInjectPasswordContentChangeForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUISetTextBoxPeerTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetPeerTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIReplacePeerSelectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIHasLocalFontOverridesForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIRichClipboardUsesAllFormatsForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetNativeCaretShownForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIHasNoHideSelectionProjectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetPasswordPeerSecurityForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIForceNextPasswordScrubFailureForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIForceNextPasswordScrubPartialWriteForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetPasswordFailClosedForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetPasswordClipboardExportAttemptCountForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetAutoUrlRangeCountForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetAutoUrlRangeForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetAutoCompleteSuggestionCountForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetAutoCompleteSuggestionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIInvokeAutoCompleteSuggestionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetPeerSelectionForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetScrollStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetVerticalScrollBarVisibilityForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIGetPositionVisibilityStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIUseTextBoxPeerForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUIForceNextPositionVisibilityRetryForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUISetNextTemporarySelectionHookForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUISetNextPasswordTextChangingHookForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextCtrl, WinUISetNextCreateLoadedHookForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxTextCtrl, WinUICreateLoadedHookForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxTextCtrl, WinUITemporarySelectionHookForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxTextCtrl, WinUIPasswordTextChangingHookForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxTextCtrl, WinUIScrollBarVisibilityForTesting);
#endif

#if wxUSE_SEARCHCTRL
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSearchCtrl, WinUIInvokeSearchButtonForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSearchCtrl, WinUIInvokeCancelButtonForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSearchCtrl, WinUISetPeerTextForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSearchCtrl, WinUIRetemplateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSearchCtrl, WinUIInvokeRetiredSearchButtonForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSearchCtrl, WinUIGetSuggestionCountForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSearchCtrl, WinUIGetTemplateStateForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxSearchCtrl, WinUISetNextCreateLoadedHookForTesting);
WX_ASSERT_NO_PUBLIC_TEST_TYPE(wxSearchCtrl, WinUICreateLoadedHookForTesting);
#endif

#if wxUSE_TEXTDLG
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextEntryDialog, WinUISetPeerValueForTesting);
WX_ASSERT_NO_PUBLIC_TEST_METHOD(wxTextEntryDialog, WinUIGetPeerValueForTesting);
#endif

#undef WX_ASSERT_NO_PUBLIC_TEST_TYPE
#undef WX_ASSERT_NO_PUBLIC_TEST_METHOD

} // anonymous namespace
#endif // any of the controls checked above
