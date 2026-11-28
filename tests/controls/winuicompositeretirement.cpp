///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuicompositeretirement.cpp
// Purpose:     Composite slot physical-disconnection receipt contracts
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "../../src/winui/compositeretirement.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <utility>

namespace
{

using RetirementGate = wxWinUICompositeSlotRetirementGate;

// This gate has no XAML projection, dispatcher, or process-global failure
// state. A test advances only its own causal boundary with Signal().
class FakeDisconnectGate final : public wxWinUIPhysicalDisconnectGate
{
public:
    bool IsSatisfied() const noexcept override
    {
        return ready && !sealed && !terminal;
    }

    bool IsDegraded() const noexcept override { return false; }
    bool IsSealedForHostShutdown() const noexcept override { return sealed; }

    winrt::Microsoft::UI::Dispatching::DispatcherQueue
        GetDispatcherQueue() const noexcept override
    {
        return nullptr;
    }

    bool SetCompletion(std::function<void()> complete) noexcept override
    {
        if ( terminal )
            return false;
        ++completionInstalls;
        completion = std::move(complete);
        NotifyIfReady();
        return true;
    }

    void SealForHostShutdown() noexcept override
    {
        ++sealCalls;
        sealed = true;
    }

    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
    {
        ++latchCalls;
    }

    bool CompleteSealedWithoutPopup() noexcept override
    {
        ++sealedCompletionAttempts;
        return sealed && ready && !terminal;
    }

    void CompleteAtXamlBoundary() noexcept override
    {
        ++boundaryCalls;
        terminal = true;
        completion = {};
    }

    void Signal()
    {
        ready = true;
        NotifyIfReady();
    }

    std::function<void()> CopyCompletion() const { return completion; }

    unsigned completionInstalls = 0;
    unsigned sealCalls = 0;
    unsigned latchCalls = 0;
    unsigned sealedCompletionAttempts = 0;
    unsigned boundaryCalls = 0;

private:
    void NotifyIfReady()
    {
        if ( IsSatisfied() && completion )
        {
            // Keep the currently dispatched callback alive if it replaces
            // itself while running.
            const auto callback = completion;
            callback();
        }
    }

    std::function<void()> completion;
    bool ready = false;
    bool sealed = false;
    bool terminal = false;
};

} // namespace

TEST_CASE("WinUICompositeRetirement::ParentWithoutPopupGate",
          "[winui-composite-retirement]")
{
    const auto gate = RetirementGate::Make(nullptr, nullptr);
    REQUIRE(gate);
    CHECK(gate->IsSatisfied());
    unsigned completions = 0;

    SECTION("child receipt is the only causal debt")
    {
        const auto child = gate->AddChild();
        REQUIRE(child != 0);
        CHECK_FALSE(gate->IsSatisfied());
        REQUIRE(gate->SetCompletion([&] { ++completions; }));
        CHECK(completions == 0);
        CHECK(gate->ChildDisconnected(child));
        CHECK(gate->IsSatisfied());
        CHECK(completions == 1);
    }
    SECTION("empty sealed parent waits for the host graph transfer")
    {
        gate->SealForHostShutdown();
        gate->LatchPublishedPopupDebtBeforeSourceClose();
        REQUIRE(gate->SetCompletion([&] { ++completions; }));
        CHECK_FALSE(gate->IsSatisfied());
        CHECK(completions == 0);
        CHECK(gate->CompleteSealedWithoutPopup());
        CHECK(completions == 1);
        CHECK(gate->CompleteSealedWithoutPopup());
        CHECK(completions == 1);
    }
}

TEST_CASE("WinUICompositeRetirement::ParentAndChildReceiptsFanIn",
          "[winui-composite-retirement]")
{
    const auto parent = std::make_shared<FakeDisconnectGate>();
    const auto gate = RetirementGate::Make(parent, nullptr);
    REQUIRE(gate);
    const std::uint64_t first = gate->AddChild();
    const std::uint64_t second = gate->AddChild();
    REQUIRE(first != 0);
    REQUIRE(second != 0);
    CHECK(first != second);
    CHECK_FALSE(gate->IsSatisfied());
    CHECK_FALSE(gate->IsDegraded());
    CHECK_FALSE(gate->GetDispatcherQueue());
    unsigned completions = 0;
    REQUIRE(gate->SetCompletion([&] { ++completions; }));
    CHECK(parent->completionInstalls == 1);
    CHECK(gate->AddChild() == 0);

    SECTION("parent disconnects first")
    {
        parent->Signal();
        CHECK(completions == 0);
        CHECK_FALSE(gate->IsSatisfied());
        CHECK(gate->ChildDisconnected(first));
        CHECK(completions == 0);
        CHECK_FALSE(gate->IsSatisfied());
        CHECK(gate->ChildDisconnected(second));
    }
    SECTION("parent disconnects between the two children")
    {
        CHECK(gate->ChildDisconnected(second));
        parent->Signal();
        CHECK(completions == 0);
        CHECK_FALSE(gate->IsSatisfied());
        CHECK(gate->ChildDisconnected(first));
    }
    SECTION("parent disconnects last")
    {
        CHECK(gate->ChildDisconnected(first));
        CHECK(gate->ChildDisconnected(second));
        CHECK(completions == 0);
        CHECK_FALSE(gate->IsSatisfied());
        parent->Signal();
    }

    CHECK(gate->IsSatisfied());
    CHECK(completions == 1);
    parent->Signal();
    CHECK(completions == 1);
}

TEST_CASE("WinUICompositeRetirement::OnlyDistinctKnownReceiptsCount",
          "[winui-composite-retirement]")
{
    const auto parent = std::make_shared<FakeDisconnectGate>();
    parent->Signal();
    const auto gate = RetirementGate::Make(parent, nullptr);
    REQUIRE(gate);
    const std::uint64_t first = gate->AddChild();
    const std::uint64_t second = gate->AddChild();
    REQUIRE(first != 0);
    REQUIRE(second != 0);
    unsigned completions = 0;
    REQUIRE(gate->SetCompletion([&] { ++completions; }));
    CHECK_FALSE(gate->ChildDisconnected(0));
    CHECK_FALSE(gate->ChildDisconnected(first + second + 1));
    CHECK(completions == 0);
    CHECK(gate->ChildDisconnected(first));
    CHECK_FALSE(gate->ChildDisconnected(first));
    CHECK_FALSE(gate->IsSatisfied());
    CHECK(completions == 0);
    CHECK(gate->ChildDisconnected(second));
    CHECK(gate->IsSatisfied());
    CHECK(completions == 1);
    CHECK_FALSE(gate->ChildDisconnected(second));
    CHECK(completions == 1);
}

TEST_CASE("WinUICompositeRetirement::CompletionRetargetsBeforeAndAfterReady",
          "[winui-composite-retirement]")
{
    const auto parent = std::make_shared<FakeDisconnectGate>();
    const auto gate = RetirementGate::Make(parent, nullptr);
    REQUIRE(gate);
    const std::uint64_t child = gate->AddChild();
    REQUIRE(child != 0);
    unsigned original = 0;
    unsigned replacement = 0;
    unsigned alreadyReady = 0;
    auto payload = std::make_shared<int>(42);
    const std::weak_ptr<int> weakPayload = payload;
    REQUIRE(gate->SetCompletion([payload, &original]
    {
        wxUnusedVar(payload);
        ++original;
    }));
    payload.reset();
    CHECK_FALSE(weakPayload.expired());
    REQUIRE(gate->SetCompletion([&] { ++replacement; }));
    CHECK(weakPayload.expired());
    CHECK(parent->completionInstalls == 1);
    parent->Signal();
    CHECK(gate->ChildDisconnected(child));
    CHECK(original == 0);
    CHECK(replacement == 1);
    REQUIRE(gate->SetCompletion([&] { ++alreadyReady; }));
    CHECK(alreadyReady == 1);
    CHECK(replacement == 1);
    CHECK(parent->completionInstalls == 1);
    parent->Signal();
    CHECK(alreadyReady == 1);
}

TEST_CASE("WinUICompositeRetirement::ChildPopupCallbackIsNotRetargeted",
          "[winui-composite-retirement]")
{
    const auto parent = std::make_shared<FakeDisconnectGate>();
    parent->Signal();
    const auto popup = std::make_shared<FakeDisconnectGate>();
    unsigned popupCompletions = 0;
    REQUIRE(popup->SetCompletion([&] { ++popupCompletions; }));
    const auto gate = RetirementGate::Make(parent, nullptr);
    REQUIRE(gate);
    const std::uint64_t receipt = gate->AddChild();
    REQUIRE(receipt != 0);
    unsigned oldDestination = 0;
    unsigned newDestination = 0;
    REQUIRE(gate->SetCompletion([&] { ++oldDestination; }));
    REQUIRE(gate->SetCompletion([&] { ++newDestination; }));
    CHECK(popup->completionInstalls == 1);
    popup->Signal();
    CHECK(popupCompletions == 1);
    CHECK(oldDestination == 0);
    CHECK(newDestination == 0);
    CHECK_FALSE(gate->IsSatisfied());
    // A popup's own callback is not a receipt for physical slot removal.
    // Only the explicit receipt from that removal releases the parent slot.
    CHECK(gate->ChildDisconnected(receipt));
    CHECK(newDestination == 1);
    CHECK(oldDestination == 0);
    CHECK(popup->completionInstalls == 1);
    CHECK(popup->boundaryCalls == 0);
}

TEST_CASE("WinUICompositeRetirement::SealedCompletionRequiresEveryReceipt",
          "[winui-composite-retirement]")
{
    const auto parent = std::make_shared<FakeDisconnectGate>();
    const auto gate = RetirementGate::Make(parent, nullptr);
    REQUIRE(gate);
    const std::uint64_t child = gate->AddChild();
    REQUIRE(child != 0);
    unsigned completions = 0;
    REQUIRE(gate->SetCompletion([&] { ++completions; }));
    gate->SealForHostShutdown();
    CHECK(gate->IsSealedForHostShutdown());
    CHECK(parent->sealCalls == 1);
    CHECK(gate->AddChild() == 0);
    gate->LatchPublishedPopupDebtBeforeSourceClose();
    CHECK(parent->latchCalls == 1);
    CHECK_FALSE(gate->CompleteSealedWithoutPopup());
    CHECK_FALSE(gate->IsSatisfied());
    CHECK(completions == 0);

    SECTION("parent becomes safe while child is still connected")
    {
        parent->Signal();
        CHECK_FALSE(gate->CompleteSealedWithoutPopup());
        CHECK(completions == 0);
        CHECK(gate->ChildDisconnected(child));
    }
    SECTION("child disconnects while parent is not yet safe")
    {
        CHECK(gate->ChildDisconnected(child));
        CHECK_FALSE(gate->CompleteSealedWithoutPopup());
        CHECK(completions == 0);
        parent->Signal();
    }

    CHECK(gate->CompleteSealedWithoutPopup());
    CHECK(completions == 1);
    CHECK_FALSE(gate->IsSatisfied());
    CHECK_FALSE(gate->ChildDisconnected(child));
}

TEST_CASE("WinUICompositeRetirement::XamlBoundaryCancelsUnresolvedCompletion",
          "[winui-composite-retirement]")
{
    const auto parent = std::make_shared<FakeDisconnectGate>();
    const auto gate = RetirementGate::Make(parent, nullptr);
    REQUIRE(gate);
    const std::uint64_t child = gate->AddChild();
    REQUIRE(child != 0);
    unsigned completions = 0;
    auto payload = std::make_shared<int>(42);
    const std::weak_ptr<int> weakPayload = payload;
    REQUIRE(gate->SetCompletion([payload, &completions]
    {
        wxUnusedVar(payload);
        ++completions;
    }));
    payload.reset();
    const auto lateParentCallback = parent->CopyCompletion();
    REQUIRE(lateParentCallback);
    gate->SealForHostShutdown();
    gate->LatchPublishedPopupDebtBeforeSourceClose();
    CHECK_FALSE(gate->CompleteSealedWithoutPopup());
    CHECK_FALSE(weakPayload.expired());
    CHECK(completions == 0);

    gate->CompleteAtXamlBoundary();
    CHECK(parent->boundaryCalls == 1);
    CHECK(weakPayload.expired());
    CHECK(completions == 0);
    CHECK_FALSE(gate->IsSatisfied());
    CHECK_FALSE(gate->GetDispatcherQueue());
    CHECK_FALSE(gate->ChildDisconnected(child));
    CHECK(gate->AddChild() == 0);
    CHECK_FALSE(gate->SetCompletion([&] { ++completions; }));
    lateParentCallback();
    parent->Signal();
    CHECK(completions == 0);
    gate->CompleteAtXamlBoundary();
    CHECK(parent->boundaryCalls == 1);
}

TEST_CASE("WinUICompositeRetirement::ReentrantCompletionCanRetargetOrRelease",
          "[winui-composite-retirement]")
{
    const auto parent = std::make_shared<FakeDisconnectGate>();
    auto gate = RetirementGate::Make(parent, nullptr);
    REQUIRE(gate);
    const std::weak_ptr<RetirementGate> weakGate = gate;
    unsigned original = 0;
    unsigned nested = 0;

    SECTION("callback retargets an already satisfied gate")
    {
        bool retargeted = false;
        REQUIRE(gate->SetCompletion([&]
        {
            ++original;
            retargeted = gate->SetCompletion([&] { ++nested; });
        }));
        parent->Signal();
        CHECK(retargeted);
        CHECK(original == 1);
        CHECK(nested == 1);
        CHECK(parent->completionInstalls == 1);
        parent->Signal();
        CHECK(original == 1);
        CHECK(nested == 1);
    }
    SECTION("parent readiness callback releases the last owner")
    {
        REQUIRE(gate->SetCompletion([&]
        {
            ++original;
            gate.reset();
        }));
        parent->Signal();
        CHECK(original == 1);
        CHECK_FALSE(gate);
        CHECK(weakGate.expired());
        parent->Signal();
        CHECK(original == 1);
    }
    SECTION("last child receipt callback releases the last owner")
    {
        const std::uint64_t child = gate->AddChild();
        REQUIRE(child != 0);
        parent->Signal();
        REQUIRE(gate->SetCompletion([&]
        {
            ++original;
            gate.reset();
        }));
        RetirementGate * const raw = gate.get();
        CHECK(raw->ChildDisconnected(child));
        CHECK(original == 1);
        CHECK_FALSE(gate);
        CHECK(weakGate.expired());
    }
}

TEST_CASE("WinUICompositeRetirement::WholeGraphHandoffPreservesParentGate",
          "[winui-composite-retirement]")
{
    const auto parent = std::make_shared<FakeDisconnectGate>();
    const auto gate = RetirementGate::Make(parent, nullptr);
    REQUIRE(gate);
    const auto child = gate->AddChild();
    REQUIRE(child != 0);
    unsigned oldSlotCompletions = 0;
    unsigned graphCompletions = 0;
    auto payload = std::make_shared<int>(42);
    const std::weak_ptr<int> weakPayload = payload;
    REQUIRE(gate->SetCompletion([payload, &oldSlotCompletions]
    {
        wxUnusedVar(payload);
        ++oldSlotCompletions;
    }));
    payload.reset();
    const auto lateParentCallback = parent->CopyCompletion();
    REQUIRE(lateParentCallback);
    CHECK_FALSE(weakPayload.expired());

    // The host now owns the complete graph and independently accounts for
    // the child's existing gate. No physical disconnection receipt is forged.
    const auto transferred = gate->TakeParentGateForGraphRetirement();
    REQUIRE(transferred == parent);
    CHECK(weakPayload.expired());
    CHECK(oldSlotCompletions == 0);
    CHECK(parent->sealCalls == 0);
    CHECK(parent->boundaryCalls == 0);
    CHECK_FALSE(parent->IsSatisfied());
    CHECK_FALSE(gate->IsSatisfied());
    CHECK_FALSE(gate->ChildDisconnected(child));
    CHECK(gate->AddChild() == 0);
    CHECK_FALSE(gate->SetCompletion([&] { ++oldSlotCompletions; }));
    CHECK_FALSE(gate->TakeParentGateForGraphRetirement());

    // A callback already on the old stack is inert, while the transferred
    // gate still accepts the ledger's new causal completion destination.
    lateParentCallback();
    CHECK(oldSlotCompletions == 0);
    REQUIRE(transferred->SetCompletion([&] { ++graphCompletions; }));
    CHECK(graphCompletions == 0);
    parent->Signal();
    CHECK(graphCompletions == 1);
    CHECK(oldSlotCompletions == 0);
    gate->CompleteAtXamlBoundary();
    CHECK(parent->boundaryCalls == 0);
    transferred->SealForHostShutdown();
    transferred->LatchPublishedPopupDebtBeforeSourceClose();
    CHECK(parent->sealCalls == 1);
    CHECK(parent->latchCalls == 1);
    transferred->CompleteAtXamlBoundary();
    CHECK(parent->boundaryCalls == 1);
}

TEST_CASE("WinUICompositeRetirement::WholeGraphHandoffWithoutParentPopup",
          "[winui-composite-retirement]")
{
    const auto gate = RetirementGate::Make(nullptr, nullptr);
    REQUIRE(gate);
    const auto child = gate->AddChild();
    REQUIRE(child != 0);
    unsigned completions = 0;
    REQUIRE(gate->SetCompletion([&] { ++completions; }));
    CHECK_FALSE(gate->TakeParentGateForGraphRetirement());
    CHECK_FALSE(gate->ChildDisconnected(child));
    CHECK_FALSE(gate->IsSatisfied());
    gate->CompleteAtXamlBoundary();
    CHECK(completions == 0);
}

#endif // __WXWINUI__ && wxUSE_WINUI3
