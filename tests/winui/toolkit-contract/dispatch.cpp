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

#if wxUSE_BUTTON || wxUSE_TOGGLEBTN
namespace
{

// Detect the old public call expressions, not private support declarations.
// These checks also run for ordinary MSW, where the WinUI-only operations
// must never appear. They require neither test support nor a control instance.
template<template<typename> class Operation, typename T, typename = void>
struct HasPublicTestOperation : std::false_type {};

template<template<typename> class Operation, typename T>
struct HasPublicTestOperation<Operation, T, std::void_t<Operation<T>>>
    : std::true_type {};

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

} // anonymous namespace
#endif // wxUSE_BUTTON || wxUSE_TOGGLEBTN
