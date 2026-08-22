///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/mswoledropsession.cpp
// Purpose:     deterministic tests for the shared MSW OLE drop session
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if (defined(__WXMSW__) || defined(__WXWINUI__)) && \
        wxUSE_OLE && wxUSE_DRAG_AND_DROP

#include "wx/app.h"
#include "wx/dataobj.h"
#include "wx/dnd.h"
#include "wx/frame.h"
#include "wx/msw/ole/oleutils.h"
#include "wx/msw/private/dropsession.h"

#include <atomic>
#include <functional>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{

class CountedTextDropTarget : public wxTextDropTarget
{
public:
    explicit CountedTextDropTarget(int* destructions)
        : m_destructions(destructions)
    {
    }

    ~CountedTextDropTarget() override
    {
        if ( m_destructions )
            ++*m_destructions;
    }

    bool OnDropText(wxCoord, wxCoord, const wxString&) override
    {
        return true;
    }

private:
    int* const m_destructions;
};

class ReplaceDropTargetFromDestructor final : public wxTextDropTarget
{
public:
    ReplaceDropTargetFromDestructor(wxWindow* owner,
                                    wxDropTarget* replacement,
                                    int* destructions)
        : m_owner(owner),
          m_replacement(replacement),
          m_destructions(destructions)
    {
    }

    ~ReplaceDropTargetFromDestructor() override
    {
        if ( m_destructions )
            ++*m_destructions;

        wxDropTarget* const replacement = m_replacement;
        m_replacement = nullptr;
        if ( m_owner )
            m_owner->SetDropTarget(replacement);
    }

    bool OnDropText(wxCoord, wxCoord, const wxString&) override
    {
        return true;
    }

private:
    wxWindow* const m_owner;
    wxDropTarget* m_replacement;
    int* const m_destructions;
};

struct TargetEvent
{
    TargetEvent(const std::string& kind_, int id_, int x_, int y_)
        : kind(kind_), id(id_), x(x_), y(y_)
    {
    }

    std::string kind;
    int id = 0;
    int x = 0;
    int y = 0;

    bool operator==(const TargetEvent& other) const
    {
        return kind == other.kind &&
               id == other.id &&
               x == other.x &&
               y == other.y;
    }
};

class RecordingDropTarget final : public wxDropTarget
{
public:
    RecordingDropTarget(int id,
                        wxDataObject* destination,
                        std::vector<TargetEvent>& events)
        : wxDropTarget(destination),
          m_id(id),
          m_events(events),
          m_destination(destination)
    {
        SetDefaultAction(wxDragMove);
    }

    wxDragResult OnEnter(wxCoord x,
                         wxCoord y,
                         wxDragResult def) override
    {
        m_matchingFormatValid =
            GetMatchingPair() != wxDataFormat(wxDF_INVALID);
        Record("Enter", x, y);
        RunOneShot(m_onEnter);
        return m_overFromEnter ? OnDragOver(x, y, def) : def;
    }

    wxDragResult OnDragOver(wxCoord x,
                            wxCoord y,
                            wxDragResult def) override
    {
        m_matchingFormatValid =
            m_matchingFormatValid &&
            GetMatchingPair() != wxDataFormat(wxDF_INVALID);
        Record("Over", x, y);
        RunOneShot(m_onOver);
        return m_overResult == wxDragError ? def : m_overResult;
    }

    void OnLeave() override
    {
        Record("Leave", 0, 0);
        RunOneShot(m_onLeave);
    }

    bool OnDrop(wxCoord x, wxCoord y) override
    {
        Record("Drop", x, y);
        RunOneShot(m_onDrop);
        return m_acceptDrop;
    }

    wxDragResult OnData(wxCoord x,
                        wxCoord y,
                        wxDragResult def) override
    {
        Record("Data", x, y);
        RunOneShot(m_onData);
        m_getDataSucceeded = GetData();
        if ( !m_getDataSucceeded )
            return wxDragNone;

        return m_dataResult == wxDragError ? def : m_dataResult;
    }

    void SetOverFromEnter(bool value) { m_overFromEnter = value; }
    void SetOverResult(wxDragResult value) { m_overResult = value; }
    void SetDataResult(wxDragResult value) { m_dataResult = value; }
    void SetAcceptDrop(bool value) { m_acceptDrop = value; }

    void OnNextEnter(std::function<void()> action)
        { m_onEnter = std::move(action); }
    void OnNextOver(std::function<void()> action)
        { m_onOver = std::move(action); }
    void OnNextLeave(std::function<void()> action)
        { m_onLeave = std::move(action); }
    void OnNextDrop(std::function<void()> action)
        { m_onDrop = std::move(action); }
    void OnNextData(std::function<void()> action)
        { m_onData = std::move(action); }

    bool GetDataSucceeded() const { return m_getDataSucceeded; }
    bool MatchingFormatWasValid() const { return m_matchingFormatValid; }
    wxDataObject* GetDestination() const { return m_destination; }

private:
    void Record(const char* kind, int x, int y)
    {
        m_events.push_back({ kind, m_id, x, y });
    }

    static void RunOneShot(std::function<void()>& action)
    {
        if ( !action )
            return;

        std::function<void()> current = std::move(action);
        action = nullptr;
        current();
    }

    int m_id;
    std::vector<TargetEvent>& m_events;
    wxDataObject* m_destination;
    bool m_overFromEnter = true;
    bool m_acceptDrop = true;
    bool m_getDataSucceeded = false;
    bool m_matchingFormatValid = true;
    wxDragResult m_overResult = wxDragError;
    wxDragResult m_dataResult = wxDragError;
    std::function<void()> m_onEnter;
    std::function<void()> m_onOver;
    std::function<void()> m_onLeave;
    std::function<void()> m_onDrop;
    std::function<void()> m_onData;
};

class RegionResolver final : public wxMSWOleDropResolver
{
public:
    void Add(int firstX,
             int lastX,
             wxDropTarget* target,
             std::uintptr_t owner,
             std::uint64_t generation = 1)
    {
        m_regions.push_back(
            { firstX, lastX, wxMSWOleAcquireDropTarget(target),
              owner, generation });
    }

    wxMSWOleDropResolveResult
    Resolve(const POINTL& point, wxMSWOleDropRoute* route) override
    {
        if ( m_unstable )
            return wxMSWOleDropResolveResult::Unstable;

        for ( const Region& region : m_regions )
        {
            if ( point.x < region.firstX || point.x > region.lastX )
                continue;

            route->target = region.target;
            route->ownerIdentity = region.owner;
            route->ownerGeneration = region.generation;
            route->clientPoint =
                { point.x - region.firstX, point.y };
            return wxMSWOleDropResolveResult::Hit;
        }

        return wxMSWOleDropResolveResult::Miss;
    }

    bool Refresh(const wxMSWOleDropRoute& expected,
                 const POINTL& point,
                 wxMSWOleDropRoute* refreshed) override
    {
        return Resolve(point, refreshed) ==
                    wxMSWOleDropResolveResult::Hit &&
               refreshed->IsSameLogicalTarget(expected);
    }

    void SetUnstable(bool value) { m_unstable = value; }

private:
    struct Region
    {
        int firstX;
        int lastX;
        wxMSWOleDropTargetLease target;
        std::uintptr_t owner;
        std::uint64_t generation;
    };

    std::vector<Region> m_regions;
    bool m_unstable = false;
};

struct FeedbackEvent
{
    FeedbackEvent(const std::string& kind_,
                  bool hasLogicalTarget_,
                  LONG x_,
                  LONG y_,
                  DWORD effect_)
        : kind(kind_),
          hasLogicalTarget(hasLogicalTarget_),
          x(x_),
          y(y_),
          effect(effect_)
    {
    }

    std::string kind;
    bool hasLogicalTarget = false;
    LONG x = 0;
    LONG y = 0;
    DWORD effect = DROPEFFECT_NONE;
};

class RecordingFeedback final : public wxMSWOleDropFeedback
{
public:
    void DragEnter(const wxMSWOleDropRoute* route,
                   IDataObject* WXUNUSED(data),
                   const POINTL& point,
                   DWORD effect) noexcept override
    {
        m_events.push_back(
            { "Enter", route != nullptr, point.x, point.y, effect });
    }

    void DragOver(const wxMSWOleDropRoute* route,
                  const POINTL& point,
                  DWORD effect) noexcept override
    {
        m_events.push_back(
            { "Over", route != nullptr, point.x, point.y, effect });
    }

    void DragLeave() noexcept override
    {
        m_events.push_back({ "Leave", false, 0, 0, DROPEFFECT_NONE });
    }

    void Drop(const wxMSWOleDropRoute* route,
              IDataObject* WXUNUSED(data),
              const POINTL& point,
              DWORD effect) noexcept override
    {
        m_events.push_back(
            { "Drop", route != nullptr, point.x, point.y, effect });
    }

    const std::vector<FeedbackEvent>& Events() const { return m_events; }
    void Clear() { m_events.clear(); }

private:
    std::vector<FeedbackEvent> m_events;
};

struct DataObjectCounters
{
    std::atomic<unsigned> addRefs{ 0 };
    std::atomic<unsigned> releases{ 0 };
    std::atomic<unsigned> queryGetData{ 0 };
    std::atomic<unsigned> getData{ 0 };
    std::atomic<long> outstanding{ 1 };
    std::atomic<bool> destroyed{ false };
};

class CountingDataObject final : public IDataObject
{
public:
    CountingDataObject(IDataObject* delegate,
                       const std::shared_ptr<DataObjectCounters>& counters)
        : m_delegate(delegate),
          m_counters(counters)
    {
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override
    {
        if ( !object )
            return E_POINTER;

        if ( iid == IID_IUnknown || iid == IID_IDataObject )
        {
            *object = static_cast<IDataObject*>(this);
            AddRef();
            return S_OK;
        }

        *object = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        ++m_counters->addRefs;
        ++m_counters->outstanding;
        return ++m_refs;
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        ++m_counters->releases;
        --m_counters->outstanding;
        const ULONG refs = --m_refs;
        if ( refs == 0 )
        {
            m_counters->destroyed = true;
            delete this;
        }
        return refs;
    }

    HRESULT STDMETHODCALLTYPE GetData(FORMATETC* format,
                                      STGMEDIUM* medium) override
    {
        ++m_counters->getData;
        return m_delegate->GetData(format, medium);
    }

    HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC* format,
                                          STGMEDIUM* medium) override
        { return m_delegate->GetDataHere(format, medium); }

    HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC* format) override
    {
        ++m_counters->queryGetData;
        return m_delegate->QueryGetData(format);
    }

    HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
        FORMATETC* input, FORMATETC* output) override
        { return m_delegate->GetCanonicalFormatEtc(input, output); }

    HRESULT STDMETHODCALLTYPE SetData(FORMATETC* format,
                                      STGMEDIUM* medium,
                                      BOOL release) override
        { return m_delegate->SetData(format, medium, release); }

    HRESULT STDMETHODCALLTYPE EnumFormatEtc(
        DWORD direction, IEnumFORMATETC** enumerator) override
        { return m_delegate->EnumFormatEtc(direction, enumerator); }

    HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC* format,
                                      DWORD flags,
                                      IAdviseSink* sink,
                                      DWORD* connection) override
        { return m_delegate->DAdvise(format, flags, sink, connection); }

    HRESULT STDMETHODCALLTYPE DUnadvise(DWORD connection) override
        { return m_delegate->DUnadvise(connection); }

    HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA** enumerator) override
        { return m_delegate->EnumDAdvise(enumerator); }

private:
    ~CountingDataObject() = default;

    std::atomic<ULONG> m_refs{ 1 };
    IDataObject* m_delegate;
    std::shared_ptr<DataObjectCounters> m_counters;
};

POINTL Point(LONG x, LONG y = 7)
{
    POINTL point = { x, y };
    return point;
}

} // anonymous namespace

TEST_CASE("MSW OLE session routes one physical drag across logical targets",
          "[msw-drop-session]")
{
    std::vector<TargetEvent> events;
    auto* const textA = new wxTextDataObject;
    auto* const textB = new wxTextDataObject;
    RecordingDropTarget targetA(1, textA, events);
    RecordingDropTarget targetB(2, textB, events);

    RegionResolver resolver;
    resolver.Add(0, 99, &targetA, 1);
    resolver.Add(100, 199, &targetB, 2);
    RecordingFeedback feedback;
    wxMSWOleDropSession session(resolver, feedback);

    wxTextDataObject source(wxString::FromUTF8("hélice β\nline2"));
    IDataObject* const data = source.GetInterface();
    DWORD effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;

    REQUIRE(session.DragEnter(data, 0, Point(-20), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    CHECK(events.empty());

    effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(session.DragOver(0, Point(10), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_MOVE);

    effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(session.DragOver(0, Point(20, 8), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_MOVE);

    effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(session.DragOver(0, Point(110, 9), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_MOVE);

    effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(session.DragOver(0, Point(250, 10), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);

    effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(session.DragOver(0, Point(30, 11), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_MOVE);

    effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(session.Drop(data, 0, Point(30, 11), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_MOVE);
    CHECK(targetA.GetDataSucceeded());
    CHECK(textA->GetText() == wxString::FromUTF8("hélice β\nline2"));

    const std::vector<TargetEvent> expected =
    {
        { "Enter", 1, 10, 7 },
        { "Over", 1, 10, 7 },
        { "Over", 1, 20, 8 },
        { "Leave", 1, 0, 0 },
        { "Enter", 2, 10, 9 },
        { "Over", 2, 10, 9 },
        { "Leave", 2, 0, 0 },
        { "Enter", 1, 30, 11 },
        { "Over", 1, 30, 11 },
        { "Drop", 1, 30, 11 },
        { "Data", 1, 30, 11 }
    };
    CHECK(events == expected);

    const auto& physical = feedback.Events();
    REQUIRE(physical.size() == 8);
    CHECK(physical.front().kind == "Enter");
    CHECK_FALSE(physical.front().hasLogicalTarget);
    CHECK(physical[1].kind == "Over");
    CHECK_FALSE(physical[1].hasLogicalTarget);
    CHECK(physical.back().kind == "Drop");
    CHECK(physical.back().hasLogicalTarget);
}

TEST_CASE("MSW OLE session retains data through rejected and empty regions",
          "[msw-drop-session]")
{
    std::vector<TargetEvent> events;
    auto* const files = new wxFileDataObject;
    auto* const text = new wxTextDataObject;
    RecordingDropTarget fileTarget(1, files, events);
    RecordingDropTarget textTarget(2, text, events);

    RegionResolver resolver;
    resolver.Add(0, 99, &fileTarget, 1);
    resolver.Add(100, 199, &textTarget, 2);
    RecordingFeedback feedback;
    wxMSWOleDropSession session(resolver, feedback);

    wxTextDataObject source("accepted later");
    IDataObject* const data = source.GetInterface();
    DWORD effect = DROPEFFECT_COPY;

    REQUIRE(session.DragEnter(data, 0, Point(10), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    CHECK(events.empty());

    effect = DROPEFFECT_COPY;
    REQUIRE(session.DragOver(0, Point(110), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);

    REQUIRE(session.DragLeave() == S_OK);
    REQUIRE(session.DragLeave() == S_OK);

    const std::vector<TargetEvent> expected =
    {
        { "Enter", 2, 10, 7 },
        { "Over", 2, 10, 7 },
        { "Leave", 2, 0, 0 }
    };
    CHECK(events == expected);

    int feedbackLeaves = 0;
    for ( const FeedbackEvent& event : feedback.Events() )
    {
        if ( event.kind == "Leave" )
            ++feedbackLeaves;
    }
    CHECK(feedbackLeaves == 1);
}

TEST_CASE("MSW OLE session computes source-constrained keyboard effects",
          "[msw-drop-session]")
{
    std::vector<TargetEvent> events;
    auto* const text = new wxTextDataObject;
    RecordingDropTarget target(1, text, events);
    RegionResolver resolver;
    resolver.Add(0, 99, &target, 1);
    RecordingFeedback feedback;
    wxMSWOleDropSession session(resolver, feedback);
    wxTextDataObject source("effects");
    IDataObject* const data = source.GetInterface();

    const auto enterAndLeave =
        [&](wxDragResult defaultAction, DWORD keys, DWORD allowed)
        {
            target.SetDefaultAction(defaultAction);
            DWORD effect = allowed;
            REQUIRE(session.DragEnter(data, keys, Point(10), &effect) == S_OK);
            REQUIRE(session.DragLeave() == S_OK);
            return effect;
        };

    CHECK(enterAndLeave(wxDragMove, 0,
                        DROPEFFECT_COPY | DROPEFFECT_MOVE) ==
          DROPEFFECT_MOVE);
    CHECK(enterAndLeave(wxDragMove, MK_CONTROL,
                        DROPEFFECT_COPY | DROPEFFECT_MOVE) ==
          DROPEFFECT_COPY);
    CHECK(enterAndLeave(wxDragCopy, 0,
                        DROPEFFECT_COPY | DROPEFFECT_MOVE) ==
          DROPEFFECT_COPY);
    CHECK(enterAndLeave(wxDragCopy, MK_SHIFT,
                        DROPEFFECT_COPY | DROPEFFECT_MOVE) ==
          DROPEFFECT_MOVE);
    CHECK(enterAndLeave(wxDragMove, 0, DROPEFFECT_LINK) ==
          DROPEFFECT_LINK);
    CHECK(enterAndLeave(wxDragMove, 0, DROPEFFECT_NONE) ==
          DROPEFFECT_NONE);
}

TEST_CASE("MSW OLE session abandons stale and nested callbacks",
          "[msw-drop-session]")
{
    SECTION("nested DragLeave wins over outer DragEnter")
    {
        std::vector<TargetEvent> events;
        auto* const text = new wxTextDataObject;
        RecordingDropTarget target(1, text, events);
        target.SetOverFromEnter(false);
        RegionResolver resolver;
        resolver.Add(0, 99, &target, 1);
        RecordingFeedback feedback;
        wxMSWOleDropSession session(resolver, feedback);
        wxTextDataObject source("nested");
        DWORD effect = DROPEFFECT_COPY;

        target.OnNextEnter([&]() { REQUIRE(session.DragLeave() == S_OK); });
        REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                                  &effect) == S_OK);
        CHECK(effect == DROPEFFECT_NONE);

        const std::vector<TargetEvent> expected =
        {
            { "Enter", 1, 10, 7 },
            { "Leave", 1, 0, 0 }
        };
        CHECK(events == expected);
        REQUIRE(feedback.Events().size() == 2);
        CHECK(feedback.Events()[0].kind == "Enter");
        CHECK(feedback.Events()[1].kind == "Leave");
    }

    SECTION("target invalidated from OnDragOver is never reused")
    {
        std::vector<TargetEvent> events;
        auto* const text = new wxTextDataObject;
        RecordingDropTarget target(1, text, events);
        RegionResolver resolver;
        resolver.Add(0, 99, &target, 1);
        RecordingFeedback feedback;
        wxMSWOleDropSession session(resolver, feedback);
        wxTextDataObject source("invalidate");
        DWORD effect = DROPEFFECT_COPY;

        REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                                  &effect) == S_OK);
        target.OnNextOver(
            [&]() { wxMSWOleInvalidateDropTarget(&target); });
        effect = DROPEFFECT_COPY;
        REQUIRE(session.DragOver(0, Point(20), &effect) == S_OK);
        CHECK(effect == DROPEFFECT_NONE);
        REQUIRE(session.DragLeave() == S_OK);

        const std::vector<TargetEvent> expected =
        {
            { "Enter", 1, 10, 7 },
            { "Over", 1, 10, 7 },
            { "Over", 1, 20, 7 }
        };
        CHECK(events == expected);
    }

    SECTION("unstable topology retires the route and later recovers")
    {
        std::vector<TargetEvent> events;
        auto* const text = new wxTextDataObject;
        RecordingDropTarget target(1, text, events);
        RegionResolver resolver;
        resolver.Add(0, 99, &target, 1);
        RecordingFeedback feedback;
        wxMSWOleDropSession session(resolver, feedback);
        wxTextDataObject source("topology");
        DWORD effect = DROPEFFECT_COPY;

        REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                                  &effect) == S_OK);
        resolver.SetUnstable(true);
        effect = DROPEFFECT_COPY;
        REQUIRE(session.DragOver(0, Point(20), &effect) == S_OK);
        CHECK(effect == DROPEFFECT_NONE);

        resolver.SetUnstable(false);
        effect = DROPEFFECT_COPY;
        REQUIRE(session.DragOver(0, Point(30), &effect) == S_OK);
        CHECK(effect == DROPEFFECT_COPY);
        REQUIRE(session.DragLeave() == S_OK);

        const std::vector<TargetEvent> expected =
        {
            { "Enter", 1, 10, 7 },
            { "Over", 1, 10, 7 },
            { "Leave", 1, 0, 0 },
            { "Enter", 1, 30, 7 },
            { "Over", 1, 30, 7 },
            { "Leave", 1, 0, 0 }
        };
        CHECK(events == expected);
    }

    SECTION("nested DragLeave during OnData wins exactly once")
    {
        std::vector<TargetEvent> events;
        auto* const text = new wxTextDataObject;
        RecordingDropTarget target(1, text, events);
        RegionResolver resolver;
        resolver.Add(0, 99, &target, 1);
        RecordingFeedback feedback;
        wxMSWOleDropSession session(resolver, feedback);
        wxTextDataObject source("nested-data");
        DWORD effect = DROPEFFECT_COPY;

        REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                                  &effect) == S_OK);
        target.OnNextData(
            [&]() { REQUIRE(session.DragLeave() == S_OK); });
        effect = DROPEFFECT_COPY;
        REQUIRE(session.Drop(source.GetInterface(), 0, Point(20),
                             &effect) == S_OK);
        CHECK(effect == DROPEFFECT_NONE);

        int physicalTerminals = 0;
        for ( const FeedbackEvent& event : feedback.Events() )
        {
            if ( event.kind == "Leave" || event.kind == "Drop" )
                ++physicalTerminals;
        }
        CHECK(physicalTerminals == 1);
        REQUIRE(feedback.Events().back().kind == "Leave");
    }
}

TEST_CASE("MSW fixed OLE session preserves the historical wrapper contract",
          "[msw-drop-session]")
{
    SECTION("accepted target receives data and one physical terminal")
    {
        std::vector<TargetEvent> events;
        auto* const text = new wxTextDataObject;
        RecordingDropTarget target(1, text, events);
        RegionResolver resolver;
        resolver.Add(0, 99, &target, 1);
        RecordingFeedback feedback;
        wxMSWOleDropSession session(
            resolver, feedback,
            wxMSWOleDropSessionMode::FixedTargetCompatibility);
        wxTextDataObject source("fixed accepted");
        DWORD effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;

        REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                                  &effect) == S_OK);
        CHECK(effect == DROPEFFECT_MOVE);
        CHECK(target.MatchingFormatWasValid());

        effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
        REQUIRE(session.DragOver(MK_CONTROL, Point(20), &effect) == S_OK);
        CHECK(effect == DROPEFFECT_COPY);

        effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
        REQUIRE(session.Drop(source.GetInterface(), 0, Point(20),
                             &effect) == S_OK);
        CHECK(effect == DROPEFFECT_MOVE);
        CHECK(text->GetText() == "fixed accepted");

        REQUIRE(feedback.Events().size() == 3);
        CHECK(feedback.Events()[0].kind == "Enter");
        CHECK(feedback.Events()[1].kind == "Over");
        CHECK(feedback.Events()[2].kind == "Drop");
    }

    SECTION("rejected format is not retained but still gets legacy Leave")
    {
        std::vector<TargetEvent> events;
        auto* const files = new wxFileDataObject;
        RecordingDropTarget target(1, files, events);
        RegionResolver resolver;
        resolver.Add(0, 99, &target, 1);
        RecordingFeedback feedback;
        wxMSWOleDropSession session(
            resolver, feedback,
            wxMSWOleDropSessionMode::FixedTargetCompatibility);
        wxTextDataObject source("fixed rejected");
        const auto counters = std::make_shared<DataObjectCounters>();
        CountingDataObject* const counted =
            new CountingDataObject(source.GetInterface(), counters);
        DWORD effect = DROPEFFECT_COPY;

        REQUIRE(session.DragEnter(counted, 0, Point(10), &effect) == S_OK);
        CHECK(effect == DROPEFFECT_NONE);
        CHECK(counters->outstanding == 1);
        CHECK(feedback.Events().empty());

        effect = DROPEFFECT_COPY;
        REQUIRE(session.DragOver(0, Point(20), &effect) == S_OK);
        CHECK(effect == DROPEFFECT_NONE);
        REQUIRE(session.DragLeave() == S_OK);

        const std::vector<TargetEvent> expected =
        {
            { "Leave", 1, 0, 0 }
        };
        CHECK(events == expected);
        REQUIRE(feedback.Events().size() == 2);
        CHECK(feedback.Events()[0].kind == "Over");
        CHECK(feedback.Events()[1].kind == "Leave");
        CHECK(counters->addRefs == counters->releases);

        counted->Release();
        CHECK(counters->destroyed);
        CHECK(counters->outstanding == 0);
    }
}

TEST_CASE("MSW dynamic OLE session balances IDataObject ownership",
          "[msw-drop-session]")
{
    std::vector<TargetEvent> events;
    auto* const text = new wxTextDataObject;
    RecordingDropTarget target(1, text, events);
    RegionResolver resolver;
    resolver.Add(0, 99, &target, 1);
    RecordingFeedback feedback;
    wxMSWOleDropSession session(resolver, feedback);
    wxTextDataObject source("counted");
    const auto counters = std::make_shared<DataObjectCounters>();
    CountingDataObject* const counted =
        new CountingDataObject(source.GetInterface(), counters);
    DWORD effect = DROPEFFECT_COPY;

    REQUIRE(session.DragEnter(counted, 0, Point(-10), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    CHECK(counters->outstanding == 2);

    effect = DROPEFFECT_COPY;
    REQUIRE(session.DragOver(0, Point(10), &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(counters->outstanding == 2);
    CHECK(counters->queryGetData > 0);

    session.Reset();
    CHECK(counters->outstanding == 1);
    CHECK(counters->addRefs == counters->releases);
    REQUIRE(feedback.Events().back().kind == "Leave");

    session.Reset();
    int leaveCount = 0;
    for ( const FeedbackEvent& event : feedback.Events() )
    {
        if ( event.kind == "Leave" )
            ++leaveCount;
    }
    CHECK(leaveCount == 1);

    counted->Release();
    CHECK(counters->destroyed);
    CHECK(counters->outstanding == 0);
}

TEST_CASE("MSW OLE session performs exact terminal cleanup",
          "[msw-drop-session]")
{
    SECTION("OnDrop false suppresses OnData and GetData")
    {
        std::vector<TargetEvent> events;
        auto* const text = new wxTextDataObject;
        RecordingDropTarget target(1, text, events);
        target.SetAcceptDrop(false);
        RegionResolver resolver;
        resolver.Add(0, 99, &target, 1);
        RecordingFeedback feedback;
        wxMSWOleDropSession session(resolver, feedback);
        wxTextDataObject source("not dropped");
        DWORD effect = DROPEFFECT_COPY;

        REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                                  &effect) == S_OK);
        effect = DROPEFFECT_COPY;
        REQUIRE(session.Drop(source.GetInterface(), 0, Point(10),
                             &effect) == S_OK);
        CHECK(effect == DROPEFFECT_NONE);

        int dataEvents = 0;
        int leaveEvents = 0;
        for ( const TargetEvent& event : events )
        {
            if ( event.kind == "Data" )
                ++dataEvents;
            if ( event.kind == "Leave" )
                ++leaveEvents;
        }
        CHECK(dataEvents == 0);
        CHECK(leaveEvents == 0);
        REQUIRE(feedback.Events().back().kind == "Drop");
        CHECK(feedback.Events().back().effect == DROPEFFECT_NONE);
    }

#if wxUSE_EXCEPTIONS
    SECTION("OnDragOver exception ends feedback and session is reusable")
    {
        std::vector<TargetEvent> events;
        auto* const text = new wxTextDataObject;
        RecordingDropTarget target(1, text, events);
        RegionResolver resolver;
        resolver.Add(0, 99, &target, 1);
        RecordingFeedback feedback;
        wxMSWOleDropSession session(
            resolver, feedback,
            wxMSWOleDropSessionMode::DynamicBroker,
            wxMSWOleDropExceptionPolicy::SuppressForTest);
        wxTextDataObject source("exception over");
        DWORD effect = DROPEFFECT_COPY;

        REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                                  &effect) == S_OK);
        target.OnNextOver(
            []() { throw std::runtime_error("OnDragOver"); });
        effect = DROPEFFECT_COPY;
        CHECK(session.DragOver(0, Point(20), &effect) == E_UNEXPECTED);
        CHECK(effect == DROPEFFECT_NONE);
        REQUIRE(feedback.Events().back().kind == "Leave");

        const std::size_t terminalCount = feedback.Events().size();
        REQUIRE(session.DragLeave() == S_OK);
        CHECK(feedback.Events().size() == terminalCount);

        effect = DROPEFFECT_COPY;
        REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                                  &effect) == S_OK);
        REQUIRE(session.DragLeave() == S_OK);
    }

    SECTION("OnDrop exception reports physical Drop none")
    {
        std::vector<TargetEvent> events;
        auto* const text = new wxTextDataObject;
        RecordingDropTarget target(1, text, events);
        RegionResolver resolver;
        resolver.Add(0, 99, &target, 1);
        RecordingFeedback feedback;
        wxMSWOleDropSession session(
            resolver, feedback,
            wxMSWOleDropSessionMode::DynamicBroker,
            wxMSWOleDropExceptionPolicy::SuppressForTest);
        wxTextDataObject source("exception drop");
        DWORD effect = DROPEFFECT_COPY;

        REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                                  &effect) == S_OK);
        target.OnNextDrop(
            []() { throw std::runtime_error("OnDrop"); });
        effect = DROPEFFECT_COPY;
        CHECK(session.Drop(source.GetInterface(), 0, Point(20),
                           &effect) == E_UNEXPECTED);
        CHECK(effect == DROPEFFECT_NONE);
        REQUIRE(feedback.Events().back().kind == "Drop");
        CHECK(feedback.Events().back().x == 20);
        CHECK(feedback.Events().back().effect == DROPEFFECT_NONE);

        const std::size_t terminalCount = feedback.Events().size();
        REQUIRE(session.DragLeave() == S_OK);
        CHECK(feedback.Events().size() == terminalCount);
    }
#endif // wxUSE_EXCEPTIONS
}

TEST_CASE("MSW OLE session keeps physical state under nested DragOver",
          "[msw-drop-session]")
{
    std::vector<TargetEvent> events;
    auto* const text = new wxTextDataObject;
    RecordingDropTarget target(1, text, events);
    target.SetOverFromEnter(false);
    RegionResolver resolver;
    resolver.Add(0, 99, &target, 1);
    RecordingFeedback feedback;
    wxMSWOleDropSession session(resolver, feedback);
    wxTextDataObject source("nested over");
    DWORD effect = DROPEFFECT_COPY;

    target.OnNextEnter(
        [&]()
        {
            DWORD nestedEffect = DROPEFFECT_COPY;
            REQUIRE(session.DragOver(0, Point(20), &nestedEffect) == S_OK);
            CHECK(nestedEffect == DROPEFFECT_COPY);
        });

    REQUIRE(session.DragEnter(source.GetInterface(), 0, Point(10),
                              &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    REQUIRE(session.DragLeave() == S_OK);

    const std::vector<TargetEvent> expected =
    {
        { "Enter", 1, 10, 7 },
        { "Over", 1, 20, 7 },
        { "Leave", 1, 0, 0 }
    };
    CHECK(events == expected);
    REQUIRE(feedback.Events().size() == 3);
    CHECK(feedback.Events()[0].kind == "Enter");
    CHECK(feedback.Events()[1].kind == "Over");
    CHECK(feedback.Events()[2].kind == "Leave");
}

TEST_CASE("MSW drop-target slot has deterministic reentrant ownership",
          "[msw-drop-session][shell-registration][ownership]")
{
    SECTION("nested destructor replacement is the latest writer")
    {
        int oldDestructions = 0;
        int outerCandidateDestructions = 0;
        int replacementDestructions = 0;
        auto* const frame = new wxFrame(
            nullptr, wxID_ANY, "drop-target-slot-latest-writer",
            wxPoint(-24000, -24000), wxSize(320, 180));
        auto* const replacement =
            new CountedTextDropTarget(&replacementDestructions);
        frame->SetDropTarget(new ReplaceDropTargetFromDestructor(
            frame, replacement, &oldDestructions));

        frame->SetDropTarget(
            new CountedTextDropTarget(&outerCandidateDestructions));

        CHECK(oldDestructions == 1);
        CHECK(outerCandidateDestructions == 1);
        CHECK(replacementDestructions == 0);
        CHECK(frame->GetDropTarget() == replacement);

        frame->SetDropTarget(nullptr);
        CHECK(replacementDestructions == 1);
        delete frame;
    }

    SECTION("nested null is also the latest writer")
    {
        int oldDestructions = 0;
        int outerCandidateDestructions = 0;
        auto* const frame = new wxFrame(
            nullptr, wxID_ANY, "drop-target-slot-null-writer",
            wxPoint(-24000, -24000), wxSize(320, 180));
        frame->SetDropTarget(new ReplaceDropTargetFromDestructor(
            frame, nullptr, &oldDestructions));

        frame->SetDropTarget(
            new CountedTextDropTarget(&outerCandidateDestructions));

        CHECK(oldDestructions == 1);
        CHECK(outerCandidateDestructions == 1);
        CHECK(frame->GetDropTarget() == nullptr);
        delete frame;
    }

    SECTION("window destruction is the terminal writer")
    {
        int oldDestructions = 0;
        int rejectedReplacementDestructions = 0;
        auto* const frame = new wxFrame(
            nullptr, wxID_ANY, "drop-target-slot-terminal-writer",
            wxPoint(-24000, -24000), wxSize(320, 180));
        frame->SetDropTarget(new ReplaceDropTargetFromDestructor(
            frame,
            new CountedTextDropTarget(&rejectedReplacementDestructions),
            &oldDestructions));

        delete frame;

        CHECK(oldDestructions == 1);
        CHECK(rejectedReplacementDestructions == 1);
    }
}

TEST_CASE("MSW shell registration ledger serializes exact operations",
          "[msw-drop-session][shell-registration]")
{
    std::vector<TargetEvent> events;
    auto* const text = new wxTextDataObject;
    RecordingDropTarget target(1, text, events);
    wxWindow* const top = wxTheApp->GetTopWindow();
    REQUIRE(top);
    const WXHWND hwnd = top->GetHandle();
    REQUIRE(hwnd);

    SECTION("competing register is rejected while revoke is in flight")
    {
        wxMSWOleShellDropTargetOperation seed =
            wxMSWOleBeginShellDropTargetOperation(&target, hwnd);
        REQUIRE(seed.IsActive());
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&seed));
        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &seed, true));

        wxMSWOleShellDropTargetOperation outer =
            wxMSWOleBeginShellDropTargetOperation(
                &target, hwnd,
                wxMSWOleShellDropTargetOperationKind::Revoke);
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&outer));
        wxMSWOleShellDropTargetOperation inner =
            wxMSWOleBeginShellDropTargetOperation(&target, hwnd);
        CHECK_FALSE(inner.IsActive());
        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &outer, false));
        CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(&target, hwnd));
        CHECK_FALSE(wxMSWOleIsShellDropTargetOperationInFlight(
            &target, hwnd));
    }

    SECTION("cancelled revoke preserves owner and a later retry commits")
    {
        wxMSWOleShellDropTargetOperation seed =
            wxMSWOleBeginShellDropTargetOperation(&target, hwnd);
        REQUIRE(seed.IsActive());
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&seed));
        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &seed, true));

        wxMSWOleShellDropTargetOperation outer =
            wxMSWOleBeginShellDropTargetOperation(
                &target, hwnd,
                wxMSWOleShellDropTargetOperationKind::Revoke);
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&outer));
        wxMSWOleShellDropTargetOperation blocked =
            wxMSWOleBeginShellDropTargetOperation(&target, hwnd);
        CHECK_FALSE(blocked.IsActive());
        CHECK(wxMSWOleCancelShellDropTargetOperation(&outer));
        CHECK(wxMSWOleIsShellDropTargetRegistered(&target, hwnd));

        wxMSWOleShellDropTargetOperation retry =
            wxMSWOleBeginShellDropTargetOperation(
                &target, hwnd,
                wxMSWOleShellDropTargetOperationKind::Revoke);
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&retry));
        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &retry, false));
        CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(&target, hwnd));
        CHECK_FALSE(wxMSWOleIsShellDropTargetOperationInFlight(
            &target, hwnd));
    }

    SECTION("competing revoke is rejected while register is in flight")
    {
        wxMSWOleShellDropTargetOperation outer =
            wxMSWOleBeginShellDropTargetOperation(&target, hwnd);
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&outer));
        wxMSWOleShellDropTargetOperation inner =
            wxMSWOleBeginShellDropTargetOperation(
                &target, hwnd,
                wxMSWOleShellDropTargetOperationKind::Revoke);
        CHECK_FALSE(inner.IsActive());
        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &outer, true));
        CHECK(wxMSWOleIsShellDropTargetRegistered(&target, hwnd));

        wxMSWOleShellDropTargetOperation cleanup =
            wxMSWOleBeginShellDropTargetOperation(
                &target, hwnd,
                wxMSWOleShellDropTargetOperationKind::Revoke);
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&cleanup));
        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &cleanup, false));
    }
}

TEST_CASE("MSW shell operation tokens retain exact lifetime and HWND",
          "[msw-drop-session][shell-registration]")
{
    std::vector<TargetEvent> events;
    wxWindow* const top = wxTheApp->GetTopWindow();
    REQUIRE(top);
    const WXHWND firstHwnd = top->GetHandle();
    REQUIRE(firstHwnd);
    std::unique_ptr<wxFrame> secondFrame(new wxFrame(
        nullptr, wxID_ANY, "shell-ledger-secondary",
        wxPoint(-24000, -24000), wxSize(160, 100)));
    const WXHWND secondHwnd = secondFrame->GetHandle();
    REQUIRE(secondHwnd);

    SECTION("in-flight destruction coverage is HWND exact")
    {
        auto* const text = new wxTextDataObject;
        RecordingDropTarget target(1, text, events);
        wxMSWOleShellDropTargetOperation operation =
            wxMSWOleBeginShellDropTargetOperation(&target, firstHwnd);
        REQUIRE(operation.IsActive());
        CHECK(wxMSWOleIsShellDropTargetOperationInFlight(
            &target, firstHwnd));
        CHECK_FALSE(wxMSWOleIsShellDropTargetOperationInFlight(
            &target, secondHwnd));
        CHECK(wxMSWOleCancelShellDropTargetOperation(&operation));
        CHECK_FALSE(wxMSWOleIsShellDropTargetOperationInFlight(
            &target, firstHwnd));
    }

    SECTION("old completion cannot cross placement-new ABA")
    {
        using Storage = typename std::aligned_storage<
            sizeof(RecordingDropTarget),
            alignof(RecordingDropTarget)>::type;
        Storage storage;

        RecordingDropTarget* first = new (&storage) RecordingDropTarget(
            1, new wxTextDataObject, events);
        wxMSWOleShellDropTargetOperation oldOperation =
            wxMSWOleBeginShellDropTargetOperation(first, firstHwnd);
        REQUIRE(oldOperation.IsActive());
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&oldOperation));
        first->~RecordingDropTarget();

        RecordingDropTarget* second = new (&storage) RecordingDropTarget(
            2, new wxTextDataObject, events);
        CHECK_FALSE(wxMSWOleCompleteShellDropTargetOperation(
            &oldOperation, true));
        CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
            second, firstHwnd));
        second->~RecordingDropTarget();
    }

    SECTION("successful operations on distinct HWNDs commit independently")
    {
        RecordingDropTarget firstTarget(
            1, new wxTextDataObject, events);
        RecordingDropTarget secondTarget(
            2, new wxTextDataObject, events);

        wxMSWOleShellDropTargetOperation seedFirst =
            wxMSWOleBeginShellDropTargetOperation(
                &firstTarget, firstHwnd);
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&seedFirst));
        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &seedFirst, true));

        wxMSWOleShellDropTargetOperation outerSecond =
            wxMSWOleBeginShellDropTargetOperation(
                &secondTarget, secondHwnd);
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&outerSecond));
        wxMSWOleShellDropTargetOperation innerFirst =
            wxMSWOleBeginShellDropTargetOperation(
                &firstTarget, firstHwnd,
                wxMSWOleShellDropTargetOperationKind::Revoke);
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&innerFirst));

        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &innerFirst, false));
        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &outerSecond, true));
        CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
            &firstTarget, firstHwnd));
        CHECK(wxMSWOleIsShellDropTargetRegistered(
            &secondTarget, secondHwnd));

        wxMSWOleShellDropTargetOperation cleanupSecond =
            wxMSWOleBeginShellDropTargetOperation(
                &secondTarget, secondHwnd,
                wxMSWOleShellDropTargetOperationKind::Revoke);
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&cleanupSecond));
        REQUIRE(wxMSWOleCompleteShellDropTargetOperation(
            &cleanupSecond, false));
    }
}

#endif // MSW/WinUI && wxUSE_OLE && wxUSE_DRAG_AND_DROP
