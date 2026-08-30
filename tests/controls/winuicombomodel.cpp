///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuicombomodel.cpp
// Purpose:     deterministic tests for the WinUI ComboBox text/item bridge
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_COMBOBOX

#include "testableframe.h"

#include "wx/app.h"
#if wxUSE_BITMAPCOMBOBOX
    #include "wx/bmpcbox.h"
    #include "wx/bmpbndl.h"
#endif
#include "wx/combobox.h"
#include "combobox-test-access.h"
#include "wx/event.h"
#include "choice-test-access.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"
#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif
#include "wx/winui/private/textpeer.h"

#include <sstream>
#include <vector>

namespace
{

class wxWinUIComboCountedData final : public wxClientData
{
public:
    explicit wxWinUIComboCountedData(int *destructions)
        : m_destructions(destructions)
    {
    }

    ~wxWinUIComboCountedData() override
    {
        ++*m_destructions;
    }

private:
    int *m_destructions;
};

std::string wxWinUIComboDiagnostics(const wxComboBox& combo)
{
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot d;
    if ( !wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo, &d) )
        return "combo diagnostics unavailable";

    std::ostringstream s;
    s << "uia=" << d.automationStage <<
         " hr=0x" << std::hex <<
            static_cast<unsigned long>(d.automationHResult) << std::dec <<
         " transition=" << d.templateTransitions <<
         " replay=" << d.templateReplayRequests << "/" <<
            d.templateReplayAttempts << "/" << d.templateReplaySuccesses <<
         " layout=" << d.comboLayoutEdges << "/" << d.editLayoutEdges <<
         " layoutResolve=" << d.comboLayoutResolveRequests << "/" <<
            d.comboLayoutResolveRuns << "/" <<
            d.comboLayoutRealizations <<
         " layoutCoalesced=" << d.comboLayoutCoalescedEdges <<
         " layoutForbidden=" << d.comboLayoutSynchronousRealizations <<
            "/" << d.comboLayoutUnlatchedRealizations <<
         " rangeMismatch=" << d.rangeProjectionMismatches <<
         " finalize=" << d.rangeFinalizations <<
         " replayPending=" << d.templateReplayPending <<
         " pending=" << d.pendingText << "/" << d.pendingRange <<
         " range=" << d.pendingRangeStart << "," << d.pendingRangeEnd <<
         " epoch/gen=" << d.pendingRangeEpoch << "/" <<
            d.pendingRangeGeneration <<
         " revision=" << d.pendingRangeRevision;
    return s.str();
}

void RequireRealizedComboTemplate(wxComboBox& combo)
{
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot peer;
    bool captured = false;
    const bool realized = WaitFor("WinUI ComboBox template realization", [&]()
    {
        captured = wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo, &peer);
        return captured &&
               peer.state == wxWinUIComboBoxTestAccess::WinUITemplate_Complete;
    }, 1000);
    INFO("template snapshot captured=" << captured <<
         " state=" << peer.state <<
         " hostContent=" << peer.hostContentIdentity <<
         " visualRoot=" << peer.visualRootIdentity <<
         " visualRootXamlRoot=" << peer.visualRootXamlRootIdentity <<
         " visualRootLogicalParent=" <<
             peer.visualRootLogicalParentIdentity <<
         " visualRootVisualParent=" <<
             peer.visualRootVisualParentIdentity <<
         " visualRootChildren=" << peer.visualRootChildCount <<
         " combo=" << peer.comboIdentity <<
         " template=" << peer.templateIdentity <<
         " xamlRoot=" << peer.xamlRootIdentity <<
         " visualChildren=" << peer.visualChildCount <<
         " actual=" << peer.actualWidth << "x" << peer.actualHeight <<
         " desired=" << peer.desiredWidth << "x" << peer.desiredHeight <<
         " hwnd=" << peer.windowWidth << "x" << peer.windowHeight <<
         "/visible=" << peer.windowVisible <<
         "/shownOnScreen=" << peer.windowShownOnScreen <<
         " bridge=" << peer.bridgeWidth << "x" << peer.bridgeHeight <<
         "/visible=" << peer.bridgeVisible <<
         " edit=" << peer.editIdentity <<
         "/generation=" << peer.editGeneration <<
         " phase=" << peer.phase <<
         " selection=" << peer.peerSelectionStart << "+" <<
             peer.peerSelectionLength <<
         " focus=" << peer.logicalFocus << "/" <<
             peer.nativeFocusInHost << "/" << peer.editFocused);
    REQUIRE(captured);
    REQUIRE(realized);
    REQUIRE(peer.state == wxWinUIComboBoxTestAccess::WinUITemplate_Complete);
}

bool WaitForSettledComboTemplateGeneration(
    wxComboBox& combo,
    std::uint64_t previousGeneration,
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot *peer,
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot *diagnostic)
{
    constexpr unsigned inFlightPhases =
        wxWinUIComboBoxTestAccess::WinUITemplatePhase_Resolving |
        wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingResolve |
        wxWinUIComboBoxTestAccess::WinUITemplatePhase_Transition |
        wxWinUIComboBoxTestAccess::WinUITemplatePhase_PeerMutation |
        wxWinUIComboBoxTestAccess::WinUITemplatePhase_QueuedResolve;

    return WaitFor("WinUI ComboBox deferred template generation", [&]()
    {
        return wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo, peer) &&
               wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo, diagnostic) &&
               (peer->state & wxWinUIComboBoxTestAccess::WinUITemplate_HasEdit) != 0 &&
               peer->editGeneration > previousGeneration &&
               (peer->phase & inFlightPhases) == 0 &&
               !diagnostic->templateReplayPending;
    }, 1000);
}

void RequireReadOnlyComboPopupPeer(wxComboBox& combo)
{
    constexpr unsigned requiredState =
        wxWinUIComboBoxTestAccess::WinUITemplate_HasCombo |
        wxWinUIComboBoxTestAccess::WinUITemplate_HasXamlRoot |
        wxWinUIComboBoxTestAccess::WinUITemplate_HasTemplate |
        wxWinUIComboBoxTestAccess::WinUITemplate_HasVisualChild |
        wxWinUIComboBoxTestAccess::WinUITemplate_IsArranged |
        wxWinUIComboBoxTestAccess::WinUITemplate_HasDesiredSize;

    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot peer;
    bool captured = false;
    const bool ready = WaitFor("WinUI read-only ComboBox popup peer readiness",
                               [&]()
    {
        captured = wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo, &peer);
        return captured &&
               (peer.state & requiredState) == requiredState;
    }, 1000);
    INFO("popup peer captured=" << captured <<
         " state=" << peer.state <<
         " combo=" << peer.comboIdentity <<
         " xamlRoot=" << peer.xamlRootIdentity <<
         " template=" << peer.templateIdentity <<
         " visualChildren=" << peer.visualChildCount <<
         " actual=" << peer.actualWidth << "x" << peer.actualHeight <<
         " desired=" << peer.desiredWidth << "x" << peer.desiredHeight <<
         " hwnd=" << peer.windowWidth << "x" << peer.windowHeight <<
         " bridge=" << peer.bridgeWidth << "x" << peer.bridgeHeight);
    REQUIRE(captured);
    REQUIRE(ready);
    REQUIRE((peer.state & requiredState) == requiredState);
}

} // anonymous namespace

TEST_CASE("wxWinUI ComboBox LayoutUpdated realization is deferred",
          "[winui-itemmodel][winui-combo]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxComboBox combo(parent, wxID_ANY, "deferred");
    RequireRealizedComboTemplate(combo);

    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot beforePeer;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot beforeDiagnostic;
    REQUIRE(wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo, &beforePeer));
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo, &beforeDiagnostic));
    REQUIRE(beforePeer.editIdentity != 0);

    // This invokes two genuine outer LayoutUpdated bodies and asks their one
    // coalesced Low continuation to retire and naturally rebind the editor
    // generation. The synchronous return is the anti-recursion oracle: the old
    // edit is untouched and no realization has entered from either callback.
    REQUIRE(wxWinUIComboBoxTestAccess::QueueTemplateLayoutResolution(&combo));
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot queuedPeer;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot queuedDiagnostic;
    REQUIRE(wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo, &queuedPeer));
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo, &queuedDiagnostic));
    CHECK(queuedPeer.editIdentity == beforePeer.editIdentity);
    CHECK(queuedPeer.editGeneration == beforePeer.editGeneration);
    CHECK((queuedPeer.phase &
           wxWinUIComboBoxTestAccess::WinUITemplatePhase_QueuedResolve) != 0);
    CHECK(queuedDiagnostic.comboLayoutEdges ==
          beforeDiagnostic.comboLayoutEdges + 2);
    CHECK(queuedDiagnostic.comboLayoutResolveRequests ==
          beforeDiagnostic.comboLayoutResolveRequests + 1);
    CHECK(queuedDiagnostic.comboLayoutResolveRuns ==
          beforeDiagnostic.comboLayoutResolveRuns);
    CHECK(queuedDiagnostic.comboLayoutCoalescedEdges ==
          beforeDiagnostic.comboLayoutCoalescedEdges + 1);
    CHECK(queuedDiagnostic.comboLayoutSynchronousRealizations ==
          beforeDiagnostic.comboLayoutSynchronousRealizations);

    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot settledPeer;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot settledDiagnostic;
    const bool settled = WaitFor(
        "WinUI ComboBox deferred template resolution", [&]()
        {
            return wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo,
                       &settledPeer) &&
                   wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
                   &settledDiagnostic) &&
                    settledPeer.state == wxWinUIComboBoxTestAccess::WinUITemplate_Complete &&
                    settledPeer.editGeneration > beforePeer.editGeneration &&
                    !(settledPeer.phase &
                      (wxWinUIComboBoxTestAccess::WinUITemplatePhase_Resolving |
                       wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingResolve |
                       wxWinUIComboBoxTestAccess::WinUITemplatePhase_Transition |
                       wxWinUIComboBoxTestAccess::WinUITemplatePhase_PeerMutation |
                       wxWinUIComboBoxTestAccess::WinUITemplatePhase_QueuedResolve)) &&
                    !settledDiagnostic.templateReplayPending &&
                    settledDiagnostic.comboLayoutResolveRuns ==
                        settledDiagnostic.comboLayoutResolveRequests &&
                    settledDiagnostic.comboLayoutResolveRuns >
                        beforeDiagnostic.comboLayoutResolveRuns;
         }, 1000);
    INFO(wxWinUIComboDiagnostics(combo));
    REQUIRE(settled);
    CHECK(settledDiagnostic.templateTransitions ==
          beforeDiagnostic.templateTransitions + 1);
    CHECK(settledDiagnostic.templateReplayAttempts ==
          beforeDiagnostic.templateReplayAttempts + 1);
    CHECK(settledDiagnostic.templateReplaySuccesses ==
          beforeDiagnostic.templateReplaySuccesses + 1);
    CHECK(settledDiagnostic.comboLayoutRealizations ==
          beforeDiagnostic.comboLayoutRealizations);
    CHECK(settledDiagnostic.comboLayoutSynchronousRealizations ==
          beforeDiagnostic.comboLayoutSynchronousRealizations);
    CHECK(settledDiagnostic.comboLayoutUnlatchedRealizations ==
          beforeDiagnostic.comboLayoutUnlatchedRealizations);

    // The request was scoped to the two earlier callback invocations. A later
    // genuine passive edge must not inherit a force bit from their test helper.
    REQUIRE(wxWinUIComboBoxTestAccess::RunTemplateLayoutEdge(&combo, false));
    wxYield();
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot passivePeer;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot passiveDiagnostic;
    REQUIRE(wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(
        &combo, &passivePeer));
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(
        &combo, &passiveDiagnostic));
    CHECK(passivePeer.editGeneration == settledPeer.editGeneration);
    CHECK(passiveDiagnostic.templateTransitions ==
          settledDiagnostic.templateTransitions);
    CHECK(passiveDiagnostic.comboLayoutSynchronousRealizations ==
          settledDiagnostic.comboLayoutSynchronousRealizations);
}

TEST_CASE("wxWinUI ComboBox text model and template generations",
          "[winui-itemmodel][winui-combo]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxComboBox combo(parent, wxID_ANY);
    combo.ChangeValue("012345");
    RequireRealizedComboTemplate(combo);
    std::uintptr_t firstEditBefore = 0;
    std::uint64_t firstGenerationBefore = 0;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot beforeFirstRetemplate;
    REQUIRE(wxWinUIComboBoxTestAccess::GetTextEntryPeerState(&combo,
        nullptr, nullptr, nullptr,
        &firstEditBefore, &firstGenerationBefore,
        nullptr, nullptr, nullptr, false));
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
        &beforeFirstRetemplate));
    const bool firstRetemplate = wxWinUIComboBoxTestAccess::Retemplate(&combo);
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot acceptedFirstRetemplate;
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
        &acceptedFirstRetemplate));
    CHECK(acceptedFirstRetemplate.comboLayoutRealizations ==
          beforeFirstRetemplate.comboLayoutRealizations);
    CHECK(acceptedFirstRetemplate.comboLayoutSynchronousRealizations ==
          beforeFirstRetemplate.comboLayoutSynchronousRealizations);

    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot firstPeerAfter;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot firstDiagnosticAfter;
    const bool firstEditCommitted = firstRetemplate &&
        WaitForSettledComboTemplateGeneration(
            combo, firstGenerationBefore,
            &firstPeerAfter, &firstDiagnosticAfter);
    const std::uintptr_t firstEditAfter = firstPeerAfter.editIdentity;
    const std::uint64_t firstGenerationAfter =
        firstPeerAfter.editGeneration;
    INFO("retemplate result=" << firstRetemplate <<
         " committed=" << firstEditCommitted <<
         " edit=" << firstEditBefore << "->" << firstEditAfter <<
         " generation=" << firstGenerationBefore << "->" <<
             firstGenerationAfter <<
         " state=" << wxWinUIComboBoxTestAccess::GetTemplateState(&combo));
    REQUIRE(firstRetemplate);
    REQUIRE(firstEditCommitted);
    REQUIRE(firstEditAfter != 0);
    CHECK(firstGenerationAfter > firstGenerationBefore);
    CHECK(firstDiagnosticAfter.templateReplaySuccesses ==
          beforeFirstRetemplate.templateReplaySuccesses + 1);
    CHECK(firstDiagnosticAfter.comboLayoutRealizations ==
          beforeFirstRetemplate.comboLayoutRealizations);
    CHECK(firstDiagnosticAfter.comboLayoutSynchronousRealizations ==
          beforeFirstRetemplate.comboLayoutSynchronousRealizations);
    CHECK((wxWinUIComboBoxTestAccess::GetTemplateState(&combo) & 0x5) == 0x5);

    combo.SetSelection(2, 4);
    long from = -1;
    long to = -1;
    combo.GetSelection(&from, &to);
    CHECK(from == 2);
    CHECK(to == 4);

    EventCounter textEvents(&combo, wxEVT_TEXT);
    REQUIRE(combo.SetHint("placeholder"));
    CHECK(combo.GetHint() == "placeholder");
    CHECK(combo.GetValue() == "012345");

    combo.WriteText("AB");
    CHECK(combo.GetValue() == "01AB45");
    CHECK(combo.GetInsertionPoint() == 4);
    CHECK_FALSE(combo.HasSelection());
    CHECK(textEvents.GetCount() == 1);

    combo.SetInsertionPoint(1);
    combo.SetValue("01AB45");
    CHECK(combo.GetInsertionPoint() == 1);
    CHECK(textEvents.GetCount() == 2);

    combo.SetValue("different");
    CHECK(combo.GetInsertionPoint() == 0);
    CHECK(textEvents.GetCount() == 3);

    textEvents.Clear();
    REQUIRE(wxWinUIComboBoxTestAccess::SetPeerText(&combo, "peer"));
    CHECK(combo.GetValue() == "peer");
    CHECK(textEvents.GetCount() == 1);

    // A wx writer can receive its only TextProperty callback synchronously
    // inside the peer-mutation guard. Its marker must nevertheless drain at
    // the exact editor's next real layout edge before a later accessibility
    // writer.
    combo.SetValue("writer");
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot textMarkerSnapshot;
    REQUIRE(WaitFor("ComboBox programmatic text marker drain", [&]()
    {
        return wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo,
                   &textMarkerSnapshot) &&
               !(textMarkerSnapshot.phase &
                 wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingText);
    }));

    // ValuePattern::SetValue first asks UIAutomationCore to focus an
    // unfocused provider. Establish the same real precondition an interactive
    // accessibility client has, and prove all three focus authorities
    // passively before the external MTA client process starts.
    combo.SetFocus();
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot automationFocus;
    REQUIRE(WaitFor("ComboBox exact UIA editor focus", [&]()
    {
        return wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo,
                   &automationFocus) &&
               wxWindow::FindFocus() == &combo &&
               automationFocus.logicalFocus &&
               automationFocus.nativeFocusInHost &&
               automationFocus.editFocused;
    }));

    // Enter through the native UIAutomationCore Value provider of the exact
    // TextBox child published by the associated ComboBoxAutomationPeer. The
    // outer editable ComboBox advertises Value but WinUI deliberately rejects
    // SetValue there; no second peer or direct Text mutation is permitted.
    textEvents.Clear();
    const bool automationSet =
        wxWinUIComboBoxTestAccess::SetPeerTextViaAutomation(&combo, "uia value");
    REQUIRE(automationSet);
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot automationResult;
    const bool automationCompleted =
        WaitFor("ComboBox external-process MTA UIA client", [&]()
    {
        return wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
                   &automationResult) &&
               (automationResult.automationStage ==
                    wxWinUIComboBoxTestAccess::WinUIAutomation_Succeeded ||
                automationResult.automationStage < 0);
    }, 2000);
    INFO(wxWinUIComboDiagnostics(combo));
    INFO("uia observed value=" << combo.GetValue() <<
         " textEvents=" << textEvents.GetCount() <<
         " peerCallbacks=" << automationResult.peerTextCallbacks <<
         " peerDeliveries=" << automationResult.peerTextDeliveries <<
         " lastPeerText=" << automationResult.lastPeerText);
    REQUIRE(automationCompleted);
    REQUIRE(automationResult.automationStage ==
            wxWinUIComboBoxTestAccess::WinUIAutomation_Succeeded);
    REQUIRE(automationResult.automationHResult == 0);
    REQUIRE(WaitFor("ComboBox UIA value writer", [&]()
    {
        return combo.GetValue() == "uia value" &&
               textEvents.GetCount() == 1;
    }, 1000));
    CHECK(textEvents.GetCount() == 1);

    REQUIRE(wxWinUIComboBoxTestAccess::SetEditSelection(&combo, 1, 3));
    combo.GetSelection(&from, &to);
    CHECK(from == 1);
    CHECK(to == 3);

    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot beforeProjection;
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot beforeProjectionPeer;
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
        &beforeProjection));
    REQUIRE(wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo,
        &beforeProjectionPeer));
    REQUIRE(wxWinUIComboBoxTestAccess::RunTemplateLayoutEdge(&combo, true));
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot acceptedProjection;
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
        &acceptedProjection));
    CHECK(acceptedProjection.comboLayoutRealizations ==
          beforeProjection.comboLayoutRealizations);
    CHECK(acceptedProjection.comboLayoutSynchronousRealizations ==
          beforeProjection.comboLayoutSynchronousRealizations);

    // Retirement is synchronous, but the post-theme editor generation and its
    // state replay are committed only from the natural layout continuation.
    // WinUI may reuse the same TextBox ABI identity, so generation advancement
    // and a drained replay/resolve queue are the authoritative boundary.
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot creatorEdge;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot creatorProjection;
    REQUIRE(WaitForSettledComboTemplateGeneration(
        combo, beforeProjectionPeer.editGeneration,
        &creatorEdge, &creatorProjection));
    CHECK((wxWinUIComboBoxTestAccess::GetTemplateState(&combo) & 0x5) == 0x5);
    CHECK(creatorProjection.comboLayoutRealizations ==
          beforeProjection.comboLayoutRealizations);
    CHECK(creatorProjection.comboLayoutSynchronousRealizations ==
          beforeProjection.comboLayoutSynchronousRealizations);
    const long peerLast = combo.GetLastPosition();
    const bool creatorIsExpected =
        creatorEdge.peerSelectionStart == 1 &&
        creatorEdge.peerSelectionLength == 2;
    const bool creatorIsSelectAll =
        creatorEdge.peerSelectionStart == 0 &&
        creatorEdge.peerSelectionLength == peerLast;
    const bool creatorHasRangeTicket =
        (creatorEdge.phase &
         wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingRange) != 0;
    std::uint64_t finalizedBeforeDrain =
        creatorProjection.rangeFinalizations;

    if ( creatorHasRangeTicket )
    {
        // A range ticket is valid only when the natural WinUI transition made
        // the peer range diverge from the wx range. Its creator is therefore
        // either the corrected range or the exact transient SelectAll.
        REQUIRE(creatorIsExpected != creatorIsSelectAll);
        if ( creatorProjection.pendingRangeRevision == 0 )
        {
            CHECK(creatorProjection.rangeProjectionMismatches ==
                  beforeProjection.rangeProjectionMismatches);
        }
        else
        {
            REQUIRE(creatorIsExpected);
            CHECK(creatorProjection.rangeProjectionMismatches ==
                  beforeProjection.rangeProjectionMismatches + 1);
        }
        CHECK(creatorProjection.rangeFinalizations ==
              beforeProjection.rangeFinalizations);

        // Even a public getter must not resolve/layout the peer and thereby
        // heal its own oracle while this exact transition ticket is pending.
        combo.GetSelection(&from, &to);
        CHECK(from == 1);
        CHECK(to == 3);
        wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot afterPendingGetter;
        REQUIRE(wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo,
            &afterPendingGetter));
        CHECK((afterPendingGetter.phase &
               wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingRange) != 0);

        // Chain the next writers only from genuine callbacks. This retains the
        // strong revision/finalization oracle for templates which really do
        // diverge, without manufacturing a ticket for a reused exact editor.
        const auto rangeSequence = std::make_shared<
            wxWinUIComboBoxTestAccess::WinUIRangeSequenceSnapshot>();
        REQUIRE(wxWinUIComboBoxTestAccess::ChainPendingRangeCallbacks(&combo,
            2, 4, rangeSequence));
        REQUIRE(WaitFor("ComboBox staged range callbacks", [&]()
        {
            return rangeSequence->independentRangeObserved;
        }, 1000));
        CHECK(rangeSequence->awaitedExistingProjection !=
              rangeSequence->firstSelectAllInjected);
        CHECK(rangeSequence->firstProjectionObserved);
        CHECK(rangeSequence->firstRangeProjectionMismatches ==
              creatorProjection.rangeProjectionMismatches + 1);
        CHECK(rangeSequence->firstRangeFinalizations ==
              beforeProjection.rangeFinalizations);
        CHECK(rangeSequence->firstPendingRangeRevision != 0);
        CHECK(rangeSequence->firstPendingRangeRevision !=
              creatorProjection.pendingRangeRevision);
        CHECK(rangeSequence->firstPeerSelectionStart == 1);
        CHECK(rangeSequence->firstPeerSelectionLength == 2);

        CHECK(rangeSequence->secondSelectAllInjected);
        CHECK(rangeSequence->secondProjectionObserved);
        CHECK(rangeSequence->secondRangeProjectionMismatches ==
              rangeSequence->firstRangeProjectionMismatches + 1);
        CHECK(rangeSequence->secondRangeFinalizations ==
              beforeProjection.rangeFinalizations);
        CHECK(rangeSequence->secondPendingRangeRevision != 0);
        CHECK(rangeSequence->secondPendingRangeRevision !=
              rangeSequence->firstPendingRangeRevision);
        CHECK(rangeSequence->secondPeerSelectionStart == 1);
        CHECK(rangeSequence->secondPeerSelectionLength == 2);

        CHECK(rangeSequence->independentIntentArmed);
        CHECK(rangeSequence->independentIntentConsumed);
        CHECK(rangeSequence->independentObservationArmed);
        CHECK(rangeSequence->independentRangeInjected);
        CHECK_FALSE(rangeSequence->independentPendingRange);
        CHECK(rangeSequence->independentModelSelectionStart == 2);
        CHECK(rangeSequence->independentModelSelectionEnd == 4);
        CHECK(rangeSequence->independentPeerSelectionStart == 2);
        CHECK(rangeSequence->independentPeerSelectionLength == 2);
        CHECK(rangeSequence->finalizedRangeProjectionMismatches >=
              rangeSequence->secondRangeProjectionMismatches);
        CHECK(rangeSequence->finalizedRangeFinalizations ==
              beforeProjection.rangeFinalizations + 1);
        CHECK(rangeSequence->independentRangeProjectionMismatches ==
              rangeSequence->finalizedRangeProjectionMismatches);
        CHECK(rangeSequence->independentRangeFinalizations ==
              rangeSequence->finalizedRangeFinalizations);
        finalizedBeforeDrain = rangeSequence->finalizedRangeFinalizations;
    }
    else
    {
        // A same-part natural transition normally preserves the exact range.
        // In that case replay must not arm an unretirable SelectionChanged
        // ticket. If a genuinely divergent ticket already drained while the
        // passive wait ran, its one mismatch/finalization remains observable.
        REQUIRE(creatorIsExpected);
        CHECK_FALSE(creatorProjection.pendingRange);
        CHECK(creatorProjection.pendingRangeRevision == 0);
        const bool divergentTicketAlreadyDrained =
            creatorProjection.rangeFinalizations >
                beforeProjection.rangeFinalizations;
        if ( divergentTicketAlreadyDrained )
        {
            CHECK(creatorProjection.rangeFinalizations ==
                  beforeProjection.rangeFinalizations + 1);
            CHECK(creatorProjection.rangeProjectionMismatches >
                  beforeProjection.rangeProjectionMismatches);
        }
        else
        {
            CHECK(creatorProjection.rangeProjectionMismatches ==
                  beforeProjection.rangeProjectionMismatches);
            CHECK(creatorProjection.rangeFinalizations ==
                  beforeProjection.rangeFinalizations);
        }
        REQUIRE(wxWinUIComboBoxTestAccess::SetEditSelection(&combo, 2, 4));
    }

    combo.GetSelection(&from, &to);
    CHECK(from == 2);
    CHECK(to == 4);
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot nativeSelection;
    REQUIRE(wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo,
        &nativeSelection));
    CHECK((nativeSelection.phase &
           wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingRange) == 0);
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot beforeStaleContinuations;
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
        &beforeStaleContinuations));
    CHECK(beforeStaleContinuations.rangeFinalizations ==
          finalizedBeforeDrain);

    // Finally exercise the successful causal path in isolation. Polling is
    // passive. If the creator already delivered its SelectAll, its revision is
    // authoritative; if the corrected range has no revision yet, inject one
    // raw SelectAll. An already-visible SelectAll is never written again: its
    // genuine deferred event is the only permitted producer.
    REQUIRE(wxWinUIComboBoxTestAccess::RunTemplateLayoutEdge(&combo, true));
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot acceptedDrainTransition;
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
        &acceptedDrainTransition));
    CHECK(acceptedDrainTransition.comboLayoutRealizations ==
          beforeStaleContinuations.comboLayoutRealizations);
    CHECK(acceptedDrainTransition.comboLayoutSynchronousRealizations ==
          beforeStaleContinuations.comboLayoutSynchronousRealizations);
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot drainCreatorEdge;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot drainCreatorProjection;
    REQUIRE(WaitForSettledComboTemplateGeneration(
        combo, nativeSelection.editGeneration,
        &drainCreatorEdge, &drainCreatorProjection));
    CHECK(drainCreatorProjection.comboLayoutRealizations ==
          beforeStaleContinuations.comboLayoutRealizations);
    CHECK(drainCreatorProjection.comboLayoutSynchronousRealizations ==
          beforeStaleContinuations.comboLayoutSynchronousRealizations);
    const bool drainCreatorIsExpected =
        drainCreatorEdge.peerSelectionStart == 2 &&
        drainCreatorEdge.peerSelectionLength == 2;
    const bool drainCreatorIsSelectAll =
        drainCreatorEdge.peerSelectionStart == 0 &&
        drainCreatorEdge.peerSelectionLength == peerLast;
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot settledTemplate;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot settledDiagnostic;
    const bool drainHasRangeTicket =
        (drainCreatorEdge.phase &
         wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingRange) != 0;
    if ( drainHasRangeTicket )
    {
        REQUIRE(drainCreatorIsExpected != drainCreatorIsSelectAll);
        CHECK(drainCreatorProjection.rangeFinalizations ==
              finalizedBeforeDrain);
        const bool drainNeedsCallback =
            drainCreatorProjection.pendingRangeRevision == 0;
        if ( drainNeedsCallback && drainCreatorIsExpected )
            REQUIRE(wxWinUIComboBoxTestAccess::SetRawEditSelection(&combo, 0, -1));
        else if ( !drainNeedsCallback )
            REQUIRE(drainCreatorIsExpected);

        const std::uint64_t minimumDrainMismatches =
            drainCreatorProjection.rangeProjectionMismatches +
            (drainNeedsCallback ? 1 : 0);
        const bool rangeDrained =
            WaitFor("ComboBox causal transition range drain", [&]()
        {
            return wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&combo,
                       &settledTemplate) &&
                   wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
                       &settledDiagnostic) &&
                   !(settledTemplate.phase &
                     wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingRange) &&
                   settledTemplate.peerSelectionStart == 2 &&
                   settledTemplate.peerSelectionLength == 2 &&
                   settledDiagnostic.rangeProjectionMismatches >=
                       minimumDrainMismatches &&
                   settledDiagnostic.rangeFinalizations ==
                       finalizedBeforeDrain + 1;
        });
        CAPTURE(drainCreatorIsExpected,
                drainCreatorIsSelectAll,
                drainNeedsCallback,
                drainCreatorProjection.pendingRangeRevision,
                drainCreatorProjection.rangeProjectionMismatches,
                minimumDrainMismatches,
                settledTemplate.phase,
                settledTemplate.peerSelectionStart,
                settledTemplate.peerSelectionLength,
                settledDiagnostic.pendingRange,
                settledDiagnostic.pendingRangeRevision,
                settledDiagnostic.rangeProjectionMismatches,
                finalizedBeforeDrain,
                settledDiagnostic.rangeFinalizations);
        REQUIRE(rangeDrained);
    }
    else
    {
        REQUIRE(drainCreatorIsExpected);
        CHECK_FALSE(drainCreatorProjection.pendingRange);
        CHECK(drainCreatorProjection.pendingRangeRevision == 0);
        const bool divergentTicketAlreadyDrained =
            drainCreatorProjection.rangeFinalizations >
                finalizedBeforeDrain;
        if ( divergentTicketAlreadyDrained )
        {
            CHECK(drainCreatorProjection.rangeFinalizations ==
                  finalizedBeforeDrain + 1);
            CHECK(drainCreatorProjection.rangeProjectionMismatches >
                  beforeStaleContinuations.rangeProjectionMismatches);
        }
        else
        {
            CHECK(drainCreatorProjection.rangeFinalizations ==
                  finalizedBeforeDrain);
            CHECK(drainCreatorProjection.rangeProjectionMismatches ==
                  beforeStaleContinuations.rangeProjectionMismatches);
        }
        settledTemplate = drainCreatorEdge;
        settledDiagnostic = drainCreatorProjection;
    }

    // The retired generation's delegates were revoked before the replacement
    // commit. If WinUI supplied a distinct ABI object, mutate that retained old
    // sender and prove it is inert. If WinUI reused the object, do not mutate
    // the current sender through the alias: generation advancement plus stable
    // tickets/callback counters is the stale-token oracle.
    REQUIRE(settledTemplate.editGeneration > nativeSelection.editGeneration);
    if ( settledTemplate.editIdentity != nativeSelection.editIdentity )
    {
        REQUIRE(wxWinUIComboBoxTestAccess::SetEditSelection(&combo, 0, 1, true));
    }
    else
    {
        wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot afterAliasedRetirement;
        REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&combo,
            &afterAliasedRetirement));
        CHECK((wxWinUIComboBoxTestAccess::GetTemplateState(&combo) & 0x7) == 0x7);
        CHECK(afterAliasedRetirement.peerTextCallbacks ==
              settledDiagnostic.peerTextCallbacks);
        CHECK(afterAliasedRetirement.rangeProjectionMismatches ==
              settledDiagnostic.rangeProjectionMismatches);
        CHECK(afterAliasedRetirement.rangeFinalizations ==
              settledDiagnostic.rangeFinalizations);
        CHECK(afterAliasedRetirement.pendingRange ==
              settledDiagnostic.pendingRange);
    }
    combo.GetSelection(&from, &to);
    CHECK(from == 2);
    CHECK(to == 4);
}

TEST_CASE("wxWinUI ComboBox readonly and dropdown contracts",
          "[winui-itemmodel][winui-combo]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choices[] = { "item 1", "item 2" };
    wxComboBox combo(parent, wxID_ANY, wxString(),
                     wxDefaultPosition, wxDefaultSize,
                     WXSIZEOF(choices), choices, wxCB_READONLY);

    EventCounter selectionEvents(&combo, wxEVT_COMBOBOX);
    EventCounter textEvents(&combo, wxEVT_TEXT);
    EventCounter dropEvents(&combo, wxEVT_COMBOBOX_DROPDOWN);
    EventCounter closeEvents(&combo, wxEVT_COMBOBOX_CLOSEUP);

    combo.SetValue("ITEM 2");
    CHECK(combo.GetSelection() == 1);
    CHECK(combo.GetValue() == "item 2");
    CHECK(selectionEvents.GetCount() == 0);
    CHECK(textEvents.GetCount() == 0);

    combo.SetValue("not an item");
    CHECK(combo.GetSelection() == 1);
    CHECK(combo.GetValue() == "item 2");
    combo.SetEditable(true);
    CHECK_FALSE(combo.IsEditable());

    long from = 17;
    long to = 23;
    combo.GetSelection(&from, &to);
    CHECK(from == -1);
    CHECK(to == -1);

    REQUIRE(wxWinUIComboBoxTestAccess::SetPeerText(&combo, "external"));
    CHECK(combo.GetValue() == "item 2");
    CHECK(textEvents.GetCount() == 0);

#if wxUSE_TOOLTIPS
    // Creating/activating the shared native tooltip popup used to make
    // WinUI defer DropDownClosed and violate wxMSW's synchronous contract.
    wxToolTip::Enable(true);
#endif

    RequireReadOnlyComboPopupPeer(combo);
    combo.Popup();
    combo.Popup();
    CHECK(dropEvents.GetCount() == 1);
    combo.Dismiss();
    combo.Dismiss();
    CHECK(closeEvents.GetCount() == 1);
}

TEST_CASE("wxWinUI ComboBox peer selection preserves event contract",
          "[winui-itemmodel][winui-combo]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choices[] = { "zero", "one", "two" };
    int destructions = 0;
    {
        wxComboBox combo(parent, wxID_ANY, wxString(),
                         wxDefaultPosition, wxDefaultSize,
                         WXSIZEOF(choices), choices);
        RequireRealizedComboTemplate(combo);
        wxWinUIComboCountedData * const data =
            new wxWinUIComboCountedData(&destructions);
        combo.SetClientObject(1, data);

        std::vector<wxEventType> order;
        void *selectionData = nullptr;
        void *textData = nullptr;
        wxString selectionText;
        wxString text;
        combo.Bind(wxEVT_COMBOBOX, [&](wxCommandEvent& event)
        {
            order.push_back(event.GetEventType());
            selectionData = event.GetClientObject();
            selectionText = event.GetString();
        });
        combo.Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
        {
            order.push_back(event.GetEventType());
            textData = event.GetClientObject();
            text = event.GetString();
        });

        REQUIRE(wxWinUIComboBoxTestAccess::SelectPeerItem(&combo, 1));
        REQUIRE(order.size() == 2);
        CHECK(order[0] == wxEVT_COMBOBOX);
        CHECK(order[1] == wxEVT_TEXT);
        CHECK(selectionData == data);
        CHECK(textData == data);
        CHECK(selectionText == "one");
        CHECK(text == "one");
        CHECK(combo.GetSelection() == 1);
        CHECK(combo.GetValue() == "one");

        order.clear();
        combo.SetSelection(2);
        CHECK(order.empty());
        CHECK(combo.GetSelection() == 2);
        CHECK(combo.GetValue() == "two");
    }
    CHECK(destructions == 1);
}

TEST_CASE("wxWinUI ComboBox callbacks are destruction-safe",
          "[winui-itemmodel][winui-combo][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const std::size_t baseline =
        wxWinUITextCallbackState::GetLiveCountForTesting();
    const wxString choices[] = { "zero", "one" };

    wxComboBox *combo =
        new wxComboBox(parent, wxID_ANY, wxString(),
                       wxDefaultPosition, wxDefaultSize,
                       WXSIZEOF(choices), choices);
    RequireRealizedComboTemplate(*combo);
    std::uintptr_t lifetimeEditBefore = 0;
    std::uint64_t lifetimeGenerationBefore = 0;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot beforeLifetimeRetemplate;
    REQUIRE(wxWinUIComboBoxTestAccess::GetTextEntryPeerState(combo,
        nullptr, nullptr, nullptr,
        &lifetimeEditBefore, &lifetimeGenerationBefore,
        nullptr, nullptr, nullptr, false));
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(combo,
        &beforeLifetimeRetemplate));
    const bool lifetimeRetemplate = wxWinUIComboBoxTestAccess::Retemplate(combo);
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot acceptedLifetimeRetemplate;
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(combo,
        &acceptedLifetimeRetemplate));
    CHECK(acceptedLifetimeRetemplate.comboLayoutRealizations ==
          beforeLifetimeRetemplate.comboLayoutRealizations);
    CHECK(acceptedLifetimeRetemplate.comboLayoutSynchronousRealizations ==
          beforeLifetimeRetemplate.comboLayoutSynchronousRealizations);
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot lifetimePeerAfter;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot lifetimeDiagnosticAfter;
    const bool lifetimeEditCommitted = lifetimeRetemplate &&
        WaitForSettledComboTemplateGeneration(
            *combo, lifetimeGenerationBefore,
            &lifetimePeerAfter, &lifetimeDiagnosticAfter);
    const std::uintptr_t lifetimeEditAfter =
        lifetimePeerAfter.editIdentity;
    const std::uint64_t lifetimeGenerationAfter =
        lifetimePeerAfter.editGeneration;
    INFO("lifetime retemplate result=" << lifetimeRetemplate <<
         " committed=" << lifetimeEditCommitted <<
         " edit=" << lifetimeEditBefore << "->" << lifetimeEditAfter <<
         " generation=" << lifetimeGenerationBefore << "->" <<
             lifetimeGenerationAfter <<
         " state=" << wxWinUIComboBoxTestAccess::GetTemplateState(combo));
    REQUIRE(lifetimeRetemplate);
    REQUIRE(lifetimeEditCommitted);
    REQUIRE(lifetimeEditAfter != 0);
    CHECK(lifetimeGenerationAfter > lifetimeGenerationBefore);
    CHECK(lifetimeDiagnosticAfter.comboLayoutRealizations ==
          beforeLifetimeRetemplate.comboLayoutRealizations);
    CHECK(lifetimeDiagnosticAfter.comboLayoutSynchronousRealizations ==
          beforeLifetimeRetemplate.comboLayoutSynchronousRealizations);

    unsigned selectionEvents = 0;
    combo->Bind(wxEVT_COMBOBOX, [&](wxCommandEvent&)
    {
        ++selectionEvents;
        wxComboBox * const doomed = combo;
        combo = nullptr;
        delete doomed;
    });

    wxComboBox * const invoking = combo;
    REQUIRE(wxWinUIComboBoxTestAccess::SelectPeerItem(invoking, 1));
    CHECK(combo == nullptr);
    CHECK(selectionEvents == 1);

    // Accept a deferred production transition and destroy its owner before the
    // natural replay can be required to commit. The continuation must hold
    // only weak callback-state references. If this WinUI version happens to
    // complete inline with a genuinely divergent range, retain the stronger
    // causal range-destruction oracle as well.
    wxComboBox *queued =
        new wxComboBox(parent, wxID_ANY, "abcd");
    wxWeakRef<wxWindow> queuedRef(queued);
    wxScopeGuard queuedCleanup = wxMakeGuard([&]()
    {
        if ( queuedRef )
            delete queued;
        queued = nullptr;
    });
    wxUnusedVar(queuedCleanup);
    RequireRealizedComboTemplate(*queued);
    queued->SetSelection(1, 3);
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot beforeQueuedTransition;
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot beforeQueuedTransitionDiagnostic;
    REQUIRE(wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(queued,
        &beforeQueuedTransition));
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(queued,
        &beforeQueuedTransitionDiagnostic));
    REQUIRE(wxWinUIComboBoxTestAccess::RunTemplateLayoutEdge(queued, true));
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot queuedTicket;
    REQUIRE(wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(queued,
        &queuedTicket));
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot beforeQueuedProjection;
    REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(queued,
        &beforeQueuedProjection));
    REQUIRE(queuedTicket.editGeneration >
            beforeQueuedTransition.editGeneration);
    CHECK(beforeQueuedProjection.comboLayoutRealizations ==
          beforeQueuedTransitionDiagnostic.comboLayoutRealizations);
    CHECK(beforeQueuedProjection.comboLayoutSynchronousRealizations ==
          beforeQueuedTransitionDiagnostic.
              comboLayoutSynchronousRealizations);

    const bool queuedHasRangeTicket =
        (queuedTicket.phase &
         wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingRange) != 0;
    if ( queuedHasRangeTicket )
    {
        REQUIRE((queuedTicket.state &
                 wxWinUIComboBoxTestAccess::WinUITemplate_HasEdit) != 0);
        REQUIRE(queuedTicket.peerSelectionStart == 1);
        REQUIRE(queuedTicket.peerSelectionLength == 2);
        const auto destructionSnapshot = std::make_shared<
            wxWinUIComboBoxTestAccess::WinUIRangeDestructionSnapshot>();
        REQUIRE(wxWinUIComboBoxTestAccess::SetRawEditSelectionAndDestroy(queued,
            0, -1, destructionSnapshot));
        REQUIRE(WaitFor("ComboBox causal range destruction hook", [&]()
        {
            return destructionSnapshot->causalHookEntered && !queuedRef &&
                   destructionSnapshot->liveCountAfterDestroy !=
                       static_cast<std::size_t>(-1) &&
                   destructionSnapshot->lowDrainObserved;
        }, 1000));
        queued = nullptr;
        CHECK(destructionSnapshot->rangeProjectionMismatches ==
              beforeQueuedProjection.rangeProjectionMismatches + 1);
        CHECK(destructionSnapshot->pendingRangeRevision != 0);
        CHECK(destructionSnapshot->pendingRangeRevision !=
              beforeQueuedProjection.pendingRangeRevision);
        CHECK(destructionSnapshot->rangeFinalizations ==
              beforeQueuedProjection.rangeFinalizations);
        CHECK(destructionSnapshot->liveCountAfterDestroy == baseline);
    }
    else
    {
        CHECK((beforeQueuedProjection.templateReplayPending ||
               (queuedTicket.state &
                wxWinUIComboBoxTestAccess::WinUITemplate_HasEdit) != 0));
        wxComboBox * const deleting = queued;
        queued = nullptr;
        delete deleting;
        CHECK_FALSE(queuedRef);
        wxYield();
        CHECK(wxWinUITextCallbackState::GetLiveCountForTesting() ==
              baseline);
    }

    for ( int i = 0; i < 100; ++i )
    {
        wxComboBox * const transient =
            new wxComboBox(parent, wxID_ANY);
        RequireRealizedComboTemplate(*transient);
        wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot beforeTransientTransition;
        REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(transient,
            &beforeTransientTransition));
        REQUIRE(wxWinUIComboBoxTestAccess::Retemplate(transient));
        wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot acceptedTransientTransition;
        REQUIRE(wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(transient,
            &acceptedTransientTransition));
        CHECK(acceptedTransientTransition.comboLayoutRealizations ==
              beforeTransientTransition.comboLayoutRealizations);
        CHECK(acceptedTransientTransition.
                  comboLayoutSynchronousRealizations ==
              beforeTransientTransition.
                  comboLayoutSynchronousRealizations);
        delete transient;
    }

    wxYield();
    CHECK(wxWinUITextCallbackState::GetLiveCountForTesting() ==
          baseline);
}

TEST_CASE("wxWinUI ComboBox best size follows model text",
          "[winui-itemmodel][winui-combo]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxComboBox combo(parent, wxID_ANY);
    const int minimumWidth = combo.FromDIP(180);
    CHECK(combo.GetBestSize().x >= minimumWidth);

    const wxString longValue(
        "A deliberately long editable ComboBox value which must not be clipped");
    combo.ChangeValue(longValue);

    wxCoord textWidth = 0;
    combo.GetTextExtent(longValue, &textWidth, nullptr);
    const int chromeWidth = combo.FromDIP(48);
    CHECK(combo.GetBestSize().x >= textWidth + chromeWidth);
    CHECK(combo.GetBestSize().x > minimumWidth);
}

#if wxUSE_BITMAPCOMBOBOX

TEST_CASE("wxWinUI BitmapComboBox reprojects bundles at destination DPI",
          "[winui-itemmodel][winui-combo][winui-009][dpi]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxBitmap one;
    wxBitmap two;
    REQUIRE(one.CreateWithDIPSize(wxSize(16, 16), 1.0, 32));
    REQUIRE(two.CreateWithDIPSize(wxSize(16, 16), 2.0, 32));
    const wxBitmapBundle bundle =
        wxBitmapBundle::FromBitmaps(one, two);

    wxBitmapComboBox combo(parent, wxID_ANY);
    REQUIRE(combo.Append("same item", bundle) != wxNOT_FOUND);
    const std::uintptr_t peer =
        wxWinUIChoiceTestAccess::GetItemPeerIdentity(&combo, 0);
    REQUIRE(peer != 0);

    wxDPIChangedEvent dpi200(wxSize(96, 96), wxSize(192, 192));
    dpi200.SetEventObject(&combo);
    dpi200.SetId(combo.GetId());
    combo.ProcessWindowEvent(dpi200);

    wxSize pixels;
    wxSize dips;
    REQUIRE(wxWinUIChoiceTestAccess::GetItemPeerBitmapState(&combo,
        0, &pixels, &dips));
    CHECK(pixels == wxSize(32, 32));
    CHECK(dips == wxSize(16, 16));
    CHECK(wxWinUIChoiceTestAccess::GetItemPeerIdentity(&combo, 0) == peer);

    wxDPIChangedEvent dpi100(wxSize(192, 192), wxSize(96, 96));
    dpi100.SetEventObject(&combo);
    dpi100.SetId(combo.GetId());
    combo.ProcessWindowEvent(dpi100);

    REQUIRE(wxWinUIChoiceTestAccess::GetItemPeerBitmapState(&combo,
        0, &pixels, &dips));
    CHECK(pixels == wxSize(16, 16));
    CHECK(dips == wxSize(16, 16));
    CHECK(wxWinUIChoiceTestAccess::GetItemPeerIdentity(&combo, 0) == peer);

    wxBitmapComboBox plain(parent, wxID_ANY);
    REQUIRE(plain.Append("same item") != wxNOT_FOUND);
    const int bitmapContribution =
        combo.GetBestSize().x - plain.GetBestSize().x;
    CHECK(bitmapContribution ==
          combo.GetBitmapSize().x + combo.FromDIP(8));
}

#endif // wxUSE_BITMAPCOMBOBOX

TEST_CASE("wxWinUI ComboBox selection survives reentrant item deletion",
          "[winui-itemmodel][winui-combo][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxString choices[] = { "zero", "one", "two" };
    wxComboBox combo(parent, wxID_ANY, wxString(),
                     wxDefaultPosition, wxDefaultSize,
                     WXSIZEOF(choices), choices);
    RequireRealizedComboTemplate(combo);

    int destructions = 0;
    combo.SetClientObject(
        1, new wxWinUIComboCountedData(&destructions));

    unsigned textEvents = 0;
    void *textClientData = reinterpret_cast<void *>(1);
    combo.Bind(wxEVT_COMBOBOX, [&](wxCommandEvent& event)
    {
        combo.Delete(static_cast<unsigned int>(event.GetInt()));
    });
    combo.Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
    {
        ++textEvents;
        textClientData = event.GetClientObject();
    });

    REQUIRE(wxWinUIComboBoxTestAccess::SelectPeerItem(&combo, 1));
    CHECK(destructions == 1);
    CHECK(combo.GetCount() == 2);
    CHECK(textEvents == 1);
    CHECK(textClientData == nullptr);
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_COMBOBOX
