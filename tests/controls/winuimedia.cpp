///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuimedia.cpp
// Purpose:     test-only WinUI MediaPlayerElement/shared-host qualification
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_MEDIACTRL

#include "wx/winui/xamlhost.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/file.h"
#include "wx/filefn.h"
#include "wx/filename.h"
#include "wx/mediactrl.h"
#include "wx/msw/private.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/winui.h"

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.Playback.h>

#include <atomic>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace
{

namespace MUID = winrt::Microsoft::UI::Dispatching;
namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WF = winrt::Windows::Foundation;
namespace WMC = winrt::Windows::Media::Core;
namespace WMP = winrt::Windows::Media::Playback;

constexpr unsigned MediaWaitRounds = 1000; // bounded UI-pump turns
constexpr unsigned StateWaitRounds = 500;
constexpr long MediaSoakBudgetMilliseconds = 300000;

template <typename T>
winrt::com_ptr<IUnknown> GetCanonicalComIdentity(const T& value)
{
    winrt::com_ptr<IUnknown> identity;
    if ( value )
    {
        winrt::check_hresult(
            winrt::get_unknown(value)->QueryInterface(
                IID_PPV_ARGS(identity.put())));
    }
    return identity;
}

void DrainDispatch(unsigned rounds = 1)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainUntil(Predicate predicate, unsigned rounds = MediaWaitRounds)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainDispatch();
    }

    return predicate();
}

bool DrainToQuiescence()
{
    for ( unsigned i = 0; i < 100; ++i )
    {
        const unsigned schedules =
            wxWinUITopLevelHost::GetFlushScheduleCount();
        const unsigned runs = wxWinUITopLevelHost::GetFlushRunCount();
        const unsigned attempts =
            wxWinUITopLevelHost::GetFlushCallbackAttemptCount();
        DrainDispatch(2);
        if ( schedules == wxWinUITopLevelHost::GetFlushScheduleCount() &&
             runs == wxWinUITopLevelHost::GetFlushRunCount() &&
             attempts ==
                 wxWinUITopLevelHost::GetFlushCallbackAttemptCount() )
        {
            return true;
        }
    }

    return false;
}

struct HostSnapshot
{
    static HostSnapshot Capture()
    {
        HostSnapshot result;
        result.hosts = wxWinUITopLevelHost::GetLiveHostCount();
        result.hostLifetimes =
            wxWinUITopLevelHost::GetLiveHostLifetimeCount();
        result.slots = wxWinUITopLevelHost::GetLiveSlotCount();
        result.slotLifetimes =
            wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
        result.subclasses =
            wxWinUITopLevelHost::GetLiveSubclassContextCount();
        result.loadedHooks =
            wxWinUITopLevelHost::GetLiveLoadedHookCount();
        result.contentObservers =
            wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest();
        result.automationObservers =
            wxWinUITopLevelHost::
                GetLiveAutomationNameStyleObserverCountForTest();
        result.invisibleProviders =
            wxWinUITestGetLiveInvisibleShellProviderCount();
        result.pendingFocusMigrations =
            wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
        result.slotAdds = wxWinUITopLevelHost::GetSlotHandlerAddCount();
        result.slotRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
        result.rootAdds = wxWinUITopLevelHost::GetRootHandlerAddCount();
        result.rootRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount();
        return result;
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
                       GetLiveContentLoadedObserverCountForTest() ==
                   contentObservers &&
               wxWinUITopLevelHost::
                       GetLiveAutomationNameStyleObserverCountForTest() ==
                   automationObservers &&
               wxWinUITestGetLiveInvisibleShellProviderCount() ==
                   invisibleProviders &&
               wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
                   pendingFocusMigrations &&
               wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotAdds ==
                   wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                       slotRevokes &&
               wxWinUITopLevelHost::GetRootHandlerAddCount() - rootAdds ==
                   wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                       rootRevokes;
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
                  GetLiveContentLoadedObserverCountForTest() ==
              contentObservers);
        CHECK(wxWinUITopLevelHost::
                  GetLiveAutomationNameStyleObserverCountForTest() ==
              automationObservers);
        CHECK(wxWinUITestGetLiveInvisibleShellProviderCount() ==
              invisibleProviders);
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              pendingFocusMigrations);

        const unsigned addedSlots =
            wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotAdds;
        const unsigned revokedSlots =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - slotRevokes;
        const unsigned addedRoots =
            wxWinUITopLevelHost::GetRootHandlerAddCount() - rootAdds;
        const unsigned revokedRoots =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount() - rootRevokes;
        if ( requireHostTraffic )
        {
            CHECK(addedSlots > 0);
            CHECK(addedRoots > 0);
        }
        CHECK(addedSlots == revokedSlots);
        CHECK(addedRoots == revokedRoots);
    }

    unsigned hosts = 0;
    unsigned hostLifetimes = 0;
    unsigned slots = 0;
    unsigned slotLifetimes = 0;
    unsigned subclasses = 0;
    unsigned loadedHooks = 0;
    unsigned contentObservers = 0;
    unsigned automationObservers = 0;
    unsigned invisibleProviders = 0;
    unsigned pendingFocusMigrations = 0;
    unsigned slotAdds = 0;
    unsigned slotRevokes = 0;
    unsigned rootAdds = 0;
    unsigned rootRevokes = 0;
};

// The media is generated by the test itself: no binary fixture, codec pack,
// network endpoint or audio input is required. The AVI contains uncompressed
// BI_RGB frames plus silent PCM, and the WAV is silent PCM too.
using Bytes = std::vector<std::uint8_t>;

void PushU16(Bytes& bytes, std::uint16_t value)
{
    bytes.push_back(static_cast<std::uint8_t>(value));
    bytes.push_back(static_cast<std::uint8_t>(value >> 8));
}

void PushI16(Bytes& bytes, std::int16_t value)
{
    PushU16(bytes, static_cast<std::uint16_t>(value));
}

void PushU32(Bytes& bytes, std::uint32_t value)
{
    bytes.push_back(static_cast<std::uint8_t>(value));
    bytes.push_back(static_cast<std::uint8_t>(value >> 8));
    bytes.push_back(static_cast<std::uint8_t>(value >> 16));
    bytes.push_back(static_cast<std::uint8_t>(value >> 24));
}

void PatchU32(Bytes& bytes, std::size_t offset, std::uint32_t value)
{
    REQUIRE(offset + 4 <= bytes.size());
    bytes[offset] = static_cast<std::uint8_t>(value);
    bytes[offset + 1] = static_cast<std::uint8_t>(value >> 8);
    bytes[offset + 2] = static_cast<std::uint8_t>(value >> 16);
    bytes[offset + 3] = static_cast<std::uint8_t>(value >> 24);
}

void PushFourCC(Bytes& bytes, const char (&value)[5])
{
    bytes.insert(bytes.end(), value, value + 4);
}

std::size_t BeginChunk(Bytes& bytes, const char (&fourCC)[5])
{
    PushFourCC(bytes, fourCC);
    const std::size_t sizeOffset = bytes.size();
    PushU32(bytes, 0);
    return sizeOffset;
}

void EndChunk(Bytes& bytes, std::size_t sizeOffset)
{
    REQUIRE(sizeOffset + 4 <= bytes.size());
    const std::size_t size = bytes.size() - (sizeOffset + 4);
    REQUIRE(size <= 0xffffffffu);
    PatchU32(bytes, sizeOffset, static_cast<std::uint32_t>(size));
    if ( size & 1 )
        bytes.push_back(0);
}

Bytes MakeSilentWave()
{
    constexpr std::uint32_t SampleRate = 16000;
    constexpr std::uint32_t Seconds = 3;
    constexpr std::uint16_t BlockAlign = 2;

    Bytes bytes;
    const std::size_t riff = BeginChunk(bytes, "RIFF");
    PushFourCC(bytes, "WAVE");

    const std::size_t format = BeginChunk(bytes, "fmt ");
    PushU16(bytes, 1); // WAVE_FORMAT_PCM
    PushU16(bytes, 1); // mono
    PushU32(bytes, SampleRate);
    PushU32(bytes, SampleRate * BlockAlign);
    PushU16(bytes, BlockAlign);
    PushU16(bytes, 16);
    EndChunk(bytes, format);

    const std::size_t data = BeginChunk(bytes, "data");
    bytes.resize(bytes.size() + SampleRate * Seconds * BlockAlign, 0);
    EndChunk(bytes, data);
    EndChunk(bytes, riff);
    REQUIRE(bytes.size() == 96044);
    return bytes;
}

struct AviIndexEntry
{
    const char *chunk;
    std::uint32_t flags;
    std::uint32_t offset;
    std::uint32_t size;
};

Bytes MakeSilentAvi()
{
    constexpr std::uint32_t Width = 96;
    constexpr std::uint32_t Height = 64;
    constexpr std::uint32_t FramesPerSecond = 16;
    constexpr std::uint32_t FrameCount = 48;
    constexpr std::uint32_t AudioRate = 16000;
    constexpr std::uint32_t AudioBlockAlign = 2;
    constexpr std::uint32_t FrameBytes = Width * Height * 3;
    constexpr std::uint32_t AudioBytesPerFrame =
        AudioRate * AudioBlockAlign / FramesPerSecond;

    Bytes bytes;
    const std::size_t riff = BeginChunk(bytes, "RIFF");
    PushFourCC(bytes, "AVI ");

    const std::size_t hdrl = BeginChunk(bytes, "LIST");
    PushFourCC(bytes, "hdrl");

    const std::size_t avih = BeginChunk(bytes, "avih");
    PushU32(bytes, 1000000 / FramesPerSecond);
    PushU32(bytes, (FrameBytes + AudioBytesPerFrame) * FramesPerSecond);
    PushU32(bytes, 0);
    PushU32(bytes, 0x110); // AVIF_HASINDEX | AVIF_ISINTERLEAVED
    PushU32(bytes, FrameCount);
    PushU32(bytes, 0);
    PushU32(bytes, 2);
    PushU32(bytes, FrameBytes);
    PushU32(bytes, Width);
    PushU32(bytes, Height);
    PushU32(bytes, 0);
    PushU32(bytes, 0);
    PushU32(bytes, 0);
    PushU32(bytes, 0);
    EndChunk(bytes, avih);

    const std::size_t videoList = BeginChunk(bytes, "LIST");
    PushFourCC(bytes, "strl");
    const std::size_t videoHeader = BeginChunk(bytes, "strh");
    PushFourCC(bytes, "vids");
    PushFourCC(bytes, "DIB ");
    PushU32(bytes, 0);
    PushU16(bytes, 0);
    PushU16(bytes, 0);
    PushU32(bytes, 0);
    PushU32(bytes, 1);
    PushU32(bytes, FramesPerSecond);
    PushU32(bytes, 0);
    PushU32(bytes, FrameCount);
    PushU32(bytes, FrameBytes);
    PushU32(bytes, 0xffffffffu);
    PushU32(bytes, 0);
    PushI16(bytes, 0);
    PushI16(bytes, 0);
    PushI16(bytes, static_cast<std::int16_t>(Width));
    PushI16(bytes, static_cast<std::int16_t>(Height));
    EndChunk(bytes, videoHeader);
    const std::size_t videoFormat = BeginChunk(bytes, "strf");
    PushU32(bytes, 40); // BITMAPINFOHEADER
    PushU32(bytes, Width);
    PushU32(bytes, Height);
    PushU16(bytes, 1);
    PushU16(bytes, 24);
    PushU32(bytes, 0); // BI_RGB
    PushU32(bytes, FrameBytes);
    PushU32(bytes, 0);
    PushU32(bytes, 0);
    PushU32(bytes, 0);
    PushU32(bytes, 0);
    EndChunk(bytes, videoFormat);
    EndChunk(bytes, videoList);

    const std::size_t audioList = BeginChunk(bytes, "LIST");
    PushFourCC(bytes, "strl");
    const std::size_t audioHeader = BeginChunk(bytes, "strh");
    PushFourCC(bytes, "auds");
    PushU32(bytes, 0);
    PushU32(bytes, 0);
    PushU16(bytes, 0);
    PushU16(bytes, 0);
    PushU32(bytes, 0);
    PushU32(bytes, AudioBlockAlign);
    PushU32(bytes, AudioRate * AudioBlockAlign);
    PushU32(bytes, 0);
    PushU32(bytes, AudioRate * 3);
    PushU32(bytes, AudioBytesPerFrame);
    PushU32(bytes, 0xffffffffu);
    PushU32(bytes, AudioBlockAlign);
    PushI16(bytes, 0);
    PushI16(bytes, 0);
    PushI16(bytes, 0);
    PushI16(bytes, 0);
    EndChunk(bytes, audioHeader);
    const std::size_t audioFormat = BeginChunk(bytes, "strf");
    PushU16(bytes, 1); // WAVE_FORMAT_PCM
    PushU16(bytes, 1);
    PushU32(bytes, AudioRate);
    PushU32(bytes, AudioRate * AudioBlockAlign);
    PushU16(bytes, AudioBlockAlign);
    PushU16(bytes, 16);
    EndChunk(bytes, audioFormat);
    EndChunk(bytes, audioList);
    EndChunk(bytes, hdrl);

    std::vector<AviIndexEntry> index;
    index.reserve(FrameCount * 2);
    const std::size_t movi = BeginChunk(bytes, "LIST");
    PushFourCC(bytes, "movi");
    const std::size_t moviBase = movi + 4;

    for ( std::uint32_t frame = 0; frame < FrameCount; ++frame )
    {
        const std::size_t frameOffset = bytes.size() - moviBase;
        const std::size_t video = BeginChunk(bytes, "00db");
        for ( std::uint32_t y = 0; y < Height; ++y )
        {
            for ( std::uint32_t x = 0; x < Width; ++x )
            {
                bytes.push_back(static_cast<std::uint8_t>(x + frame * 3));
                bytes.push_back(static_cast<std::uint8_t>(y * 4));
                bytes.push_back(static_cast<std::uint8_t>(frame * 5));
            }
        }
        EndChunk(bytes, video);
        index.push_back({ "00db", 0x10,
                          static_cast<std::uint32_t>(frameOffset),
                          FrameBytes });

        const std::size_t audioOffset = bytes.size() - moviBase;
        const std::size_t audio = BeginChunk(bytes, "01wb");
        bytes.resize(bytes.size() + AudioBytesPerFrame, 0);
        EndChunk(bytes, audio);
        index.push_back({ "01wb", 0,
                          static_cast<std::uint32_t>(audioOffset),
                          AudioBytesPerFrame });
    }
    EndChunk(bytes, movi);

    const std::size_t idx1 = BeginChunk(bytes, "idx1");
    for ( const AviIndexEntry& entry : index )
    {
        bytes.insert(bytes.end(), entry.chunk, entry.chunk + 4);
        PushU32(bytes, entry.flags);
        PushU32(bytes, entry.offset);
        PushU32(bytes, entry.size);
    }
    EndChunk(bytes, idx1);
    EndChunk(bytes, riff);
    REQUIRE(bytes.size() == 983372);
    return bytes;
}

class LocalMediaFiles final
{
public:
    ~LocalMediaFiles()
    {
        Remove(wavePath);
        Remove(aviPath);
        Remove(invalidPath);
    }

    bool Create()
    {
        const Bytes invalid = {
            'n', 'o', 't', '-', 'm', 'e', 'd', 'i', 'a', '\n'
        };
        return Write("wav", MakeSilentWave(), &wavePath) &&
               Write("avi", MakeSilentAvi(), &aviPath) &&
               Write("bin", invalid, &invalidPath);
    }

    wxString WaveUrl() const
    {
        return wxFileName::FileNameToURL(wxFileName(wavePath));
    }

    wxString AviUrl() const
    {
        return wxFileName::FileNameToURL(wxFileName(aviPath));
    }

    wxString InvalidUrl() const
    {
        return wxFileName::FileNameToURL(wxFileName(invalidPath));
    }

    wxString wavePath;
    wxString aviPath;
    wxString invalidPath;

private:
    static bool Write(const wxString& extension,
                      const Bytes& bytes,
                      wxString *path)
    {
        wxCHECK_MSG(path, false, "null media path output");
        const wxString reservation =
            wxFileName::CreateTempFileName("wx-winui-media-015-");
        if ( reservation.empty() )
            return false;

        wxFileName destination(reservation);
        destination.SetExt(extension);
        if ( !wxRemoveFile(reservation) )
            return false;

        wxFile file(destination.GetFullPath(), wxFile::write);
        if ( !file.IsOpened() ||
             file.Write(bytes.data(), bytes.size()) != bytes.size() ||
             !file.Close() )
        {
            wxRemoveFile(destination.GetFullPath());
            return false;
        }

        *path = destination.GetFullPath();
        return true;
    }

    static void Remove(const wxString& path)
    {
        if ( !path.empty() && wxFileExists(path) )
            wxRemoveFile(path);
    }
};

struct CallbackLedger
{
    std::atomic<unsigned> sourceCallbacks{0};
    std::atomic<unsigned> callbacksOnDispatcher{0};
    std::atomic<unsigned> callbacksOffDispatcher{0};
    std::atomic<unsigned> enqueued{0};
    std::atomic<unsigned> enqueueFailures{0};
    std::atomic<unsigned> callbackExceptions{0};
    std::atomic<unsigned> accepted{0};
    std::atomic<unsigned> staleLifetime{0};
    std::atomic<unsigned> staleLoad{0};
    std::atomic<unsigned> expiredState{0};
    std::atomic<unsigned> handlerAdds{0};
    std::atomic<unsigned> handlerRevokes{0};
    std::atomic<unsigned> sessionsInitialized{0};
    std::atomic<unsigned> sessionsClosed{0};
    std::atomic<unsigned> teardownStepAttempts{0};
    std::atomic<unsigned> teardownStepSuccesses{0};
    std::atomic<unsigned> teardownStepFailures{0};
    std::atomic<unsigned> sourceCloseAttempts{0};
    std::atomic<unsigned> sourceCloseSuccesses{0};
    std::atomic<unsigned> playerCloseAttempts{0};
    std::atomic<unsigned> playerCloseSuccesses{0};
    std::atomic<unsigned> binderHandlerAdds{0};
    std::atomic<unsigned> binderHandlerRevokes{0};
};

struct NativeEventCorrelation
{
    // Delegates stamp arrival at their source before posting to the UI queue.
    // This preserves ordering only; it is deliberately not treated as proof
    // of which native operation caused an event.
    mutable std::mutex entryMutex;
    std::atomic<std::uint64_t> sourceSequence{0};
};

constexpr unsigned NativeSnapshotRetryLimit = 16;

std::uint64_t CaptureEntrySequence(
    NativeEventCorrelation& correlation) noexcept
{
    std::lock_guard<std::mutex> entryLock(correlation.entryMutex);
    return correlation.sourceSequence.load(std::memory_order_acquire);
}

bool EntrySequenceMatches(NativeEventCorrelation& correlation,
                          std::uint64_t expectedSequence) noexcept
{
    std::lock_guard<std::mutex> entryLock(correlation.entryMutex);
    return correlation.sourceSequence.load(std::memory_order_acquire) ==
           expectedSequence;
}

enum class NativeOperationKind
{
    None,
    PublicPlay,
    PublicPause,
    PublicSeek,
    StopPause,
    StopSeek
};

// Windows.Media.Playback event arguments expose no operation-correlation
// token. These tickets are harness-owned intent IDs used with strict
// serialization; they must never be described as native/WinRT tokens.
struct NativeOperationTicket
{
    NativeOperationKind kind = NativeOperationKind::None;
    std::uint64_t operationId = 0;
    std::uint64_t commandEpoch = 0;
    std::uint64_t reservationBarrier = 0;

    bool IsValid() const
    {
        return kind != NativeOperationKind::None && operationId != 0;
    }
};

enum class NativeOperationStartResult
{
    Started,
    Busy,
    StaleSnapshot
};

class NativeOperationTracker final
{
public:
    bool HasPending() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_play.IsValid() || m_pause.IsValid() || m_seek.IsValid();
    }

    NativeOperationStartResult TryBeginPublicPlay(
        std::uint64_t commandEpoch,
        NativeEventCorrelation& correlation,
        std::uint64_t expectedSequence,
        NativeOperationTicket *ticket)
    {
        return TryBeginExclusive(
            NativeOperationKind::PublicPlay, commandEpoch, correlation,
            expectedSequence, ticket);
    }

    NativeOperationStartResult TryBeginPublicPause(
        std::uint64_t commandEpoch,
        NativeEventCorrelation& correlation,
        std::uint64_t expectedSequence,
        NativeOperationTicket *ticket)
    {
        return TryBeginExclusive(
            NativeOperationKind::PublicPause, commandEpoch, correlation,
            expectedSequence, ticket);
    }

    NativeOperationStartResult TryBeginPublicSeek(
        std::uint64_t commandEpoch,
        NativeEventCorrelation& correlation,
        std::uint64_t expectedSequence,
        NativeOperationTicket *ticket)
    {
        return TryBeginExclusive(
            NativeOperationKind::PublicSeek, commandEpoch, correlation,
            expectedSequence, ticket);
    }

    NativeOperationStartResult TryBeginStop(
        std::uint64_t commandEpoch,
        NativeEventCorrelation& correlation,
        std::uint64_t expectedSequence,
        bool needsPause,
        NativeOperationTicket *pauseTicket,
        NativeOperationTicket *seekTicket)
    {
        if ( !pauseTicket || !seekTicket )
            return NativeOperationStartResult::Busy;

        *pauseTicket = NativeOperationTicket{};
        *seekTicket = NativeOperationTicket{};
        // Reservation is the only two-lock operation. The fixed order is
        // source entry, then tracker; Complete*/Cancel/HasPending never
        // acquire entryMutex.
        std::lock_guard<std::mutex> entryLock(correlation.entryMutex);
        std::lock_guard<std::mutex> trackerLock(m_mutex);
        const std::uint64_t sourceBarrier =
            correlation.sourceSequence.load(std::memory_order_acquire);
        if ( sourceBarrier != expectedSequence )
            return NativeOperationStartResult::StaleSnapshot;
        if ( m_play.IsValid() || m_pause.IsValid() || m_seek.IsValid() )
            return NativeOperationStartResult::Busy;

        // Sampling the barrier and publishing the ticket(s) are one tracker
        // critical section. A delegate stamped before this reservation is
        // therefore included in the immutable barrier returned in each ticket.
        if ( needsPause )
        {
            m_pause = MakeTicket(
                NativeOperationKind::StopPause, commandEpoch,
                sourceBarrier);
            *pauseTicket = m_pause;
        }
        m_seek = MakeTicket(
            NativeOperationKind::StopSeek, commandEpoch, sourceBarrier);
        *seekTicket = m_seek;
        return NativeOperationStartResult::Started;
    }

    NativeOperationTicket CompletePlayAtSource(
        std::uint64_t sourceSequence)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return ConsumeAfterBarrier(m_play, sourceSequence);
    }

    NativeOperationTicket CompletePauseAtSource(
        std::uint64_t sourceSequence)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return ConsumeAfterBarrier(m_pause, sourceSequence);
    }

    NativeOperationTicket CompleteSeekAtSource(
        std::uint64_t sourceSequence)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return ConsumeAfterBarrier(m_seek, sourceSequence);
    }

    void Cancel(const NativeOperationTicket& ticket)
    {
        if ( !ticket.IsValid() )
            return;

        std::lock_guard<std::mutex> lock(m_mutex);
        if ( SameOperation(m_play, ticket) )
            m_play = NativeOperationTicket{};
        if ( SameOperation(m_pause, ticket) )
            m_pause = NativeOperationTicket{};
        if ( SameOperation(m_seek, ticket) )
            m_seek = NativeOperationTicket{};
    }

private:
    NativeOperationStartResult TryBeginExclusive(
        NativeOperationKind kind,
        std::uint64_t commandEpoch,
        NativeEventCorrelation& correlation,
        std::uint64_t expectedSequence,
        NativeOperationTicket *ticket)
    {
        if ( !ticket )
            return NativeOperationStartResult::Busy;

        *ticket = NativeOperationTicket{};
        std::lock_guard<std::mutex> entryLock(correlation.entryMutex);
        std::lock_guard<std::mutex> trackerLock(m_mutex);
        const std::uint64_t sourceBarrier =
            correlation.sourceSequence.load(std::memory_order_acquire);
        if ( sourceBarrier != expectedSequence )
            return NativeOperationStartResult::StaleSnapshot;
        if ( m_play.IsValid() || m_pause.IsValid() || m_seek.IsValid() )
            return NativeOperationStartResult::Busy;

        const NativeOperationTicket operation =
            MakeTicket(kind, commandEpoch, sourceBarrier);
        if ( kind == NativeOperationKind::PublicPlay )
            m_play = operation;
        else if ( kind == NativeOperationKind::PublicPause )
            m_pause = operation;
        else
            m_seek = operation;
        *ticket = operation;
        return NativeOperationStartResult::Started;
    }

    NativeOperationTicket MakeTicket(NativeOperationKind kind,
                                       std::uint64_t commandEpoch,
                                       std::uint64_t sourceBarrier)
    {
        if ( ++m_nextOperationId == 0 )
            ++m_nextOperationId;
        NativeOperationTicket ticket;
        ticket.kind = kind;
        ticket.operationId = m_nextOperationId;
        ticket.commandEpoch = commandEpoch;
        ticket.reservationBarrier = sourceBarrier;
        return ticket;
    }

    static NativeOperationTicket ConsumeAfterBarrier(
        NativeOperationTicket& pending,
        std::uint64_t sourceSequence)
    {
        // A delegate stamps its source sequence before taking this mutex. If
        // it entered before a command reserved the slot, it must not steal
        // that command's ticket when it resumes after the reservation.
        if ( !pending.IsValid() ||
             sourceSequence <= pending.reservationBarrier )
        {
            return NativeOperationTicket{};
        }

        const NativeOperationTicket ticket = pending;
        pending = NativeOperationTicket{};
        return ticket;
    }

    static bool SameOperation(const NativeOperationTicket& lhs,
                              const NativeOperationTicket& rhs)
    {
        return lhs.IsValid() && rhs.IsValid() &&
               lhs.operationId == rhs.operationId &&
               lhs.kind == rhs.kind &&
               lhs.commandEpoch == rhs.commandEpoch &&
               lhs.reservationBarrier == rhs.reservationBarrier;
    }

    mutable std::mutex m_mutex;
    std::uint64_t m_nextOperationId = 0;
    NativeOperationTicket m_play;
    NativeOperationTicket m_pause;
    NativeOperationTicket m_seek;
};

enum class LogicalMediaState
{
    Stopped,
    Paused,
    Playing
};

struct MediaState
{
    bool alive = true;
    std::uint64_t lifetimeGeneration = 1;
    std::uint64_t loadGeneration = 0;
    HRESULT callbackHr = S_OK;

    std::uint64_t openedLoad = 0;
    std::uint64_t failedLoad = 0;
    std::uint64_t endedLoad = 0;
    unsigned opened = 0;
    unsigned failed = 0;
    unsigned ended = 0;
    unsigned playbackStateChanges = 0;
    unsigned seekCompleted = 0;
    unsigned playbackRateChanges = 0;
    unsigned durationChanges = 0;
    unsigned videoSizeChanges = 0;
    unsigned volumeChanges = 0;
    unsigned logicalPlayEvents = 0;
    unsigned logicalPauseEvents = 0;
    unsigned logicalStopEvents = 0;
    unsigned logicalFinishEvents = 0;
    unsigned eofStopRequests = 0;
    unsigned eofStopVetoes = 0;
    unsigned probesAccepted = 0;
    unsigned stalePlayIntentEvents = 0;
    unsigned staleStopPauseEvents = 0;
    unsigned staleStopSeekEvents = 0;
    unsigned stalePauseIntentEvents = 0;
    unsigned uncommandedPlayingStates = 0;
    unsigned uncommandedPausedStates = 0;
    std::uint64_t lastIssuedPublicSeekOperationId = 0;
    std::uint64_t lastCompletedPublicSeekOperationId = 0;

    WMP::MediaPlayerError lastError = WMP::MediaPlayerError::Unknown;
    HRESULT lastExtendedError = S_OK;
    std::wstring lastErrorMessage;
    WMP::MediaPlaybackState nativeState = WMP::MediaPlaybackState::None;
    LogicalMediaState logicalState = LogicalMediaState::Stopped;
    std::uint64_t commandEpoch = 0;
    std::uint64_t playEpoch = 0;
    std::uint64_t playOperationId = 0;
    std::uint64_t playAfterSequence = 0;
    std::uint64_t lastCompletedPlayEpoch = 0;
    std::uint64_t lastCompletedPlayOperationId = 0;
    std::uint64_t lastCompletedPlaySequence = 0;
    std::uint64_t stopEpoch = 0;
    std::uint64_t stopPauseAfterSequence = 0;
    std::uint64_t stopSeekAfterSequence = 0;
    std::uint64_t stopPauseObservedEpoch = 0;
    std::uint64_t stopSeekObservedEpoch = 0;
    std::uint64_t stopPauseOperationId = 0;
    std::uint64_t stopSeekOperationId = 0;
    std::uint64_t stopPauseSequence = 0;
    std::uint64_t stopSeekSequence = 0;
    std::uint64_t lastCompletedStopEpoch = 0;
    std::uint64_t lastCompletedStopPauseOperationId = 0;
    std::uint64_t lastCompletedStopSeekOperationId = 0;
    std::uint64_t lastCompletedStopPauseSequence = 0;
    std::uint64_t lastCompletedStopSeekSequence = 0;
    std::uint64_t pauseEpoch = 0;
    std::uint64_t pauseOperationId = 0;
    std::uint64_t pauseAfterSequence = 0;
    std::uint64_t lastCompletedPauseEpoch = 0;
    std::uint64_t lastCompletedPauseOperationId = 0;
    std::uint64_t lastEofSequence = 0;
    bool allowEofStop = true;
    std::int64_t durationTicks = 0;
    std::uint32_t videoWidth = 0;
    std::uint32_t videoHeight = 0;
};

bool HasActiveReducerIntent(const MediaState& state)
{
    const bool hasSeekIntent =
        state.lastIssuedPublicSeekOperationId != 0 &&
        state.lastCompletedPublicSeekOperationId !=
            state.lastIssuedPublicSeekOperationId;
    return state.playEpoch != 0 || state.pauseEpoch != 0 ||
           state.stopEpoch != 0 || hasSeekIntent;
}

bool PlaybackCommandIsBlocked(const MediaState& state,
                              const NativeOperationTracker& operations)
{
    // Source delegates consume tracker tickets before their reducers run on
    // the UI thread. Keep every command, including an idempotent one, closed
    // during that source-consumed/UI-queued interval as well as while a
    // native-operation ticket is still pending.
    return HasActiveReducerIntent(state) || operations.HasPending();
}

std::uint64_t StampNativeEvent(
    const std::shared_ptr<NativeEventCorrelation>& correlation) noexcept
{
    // Source entry and ticket reservation are totally ordered. Reservations
    // take this mutex before the tracker mutex; no path takes them in reverse.
    std::lock_guard<std::mutex> entryLock(correlation->entryMutex);
    // No test can approach 2^64 events. Keep zero reserved as "no event".
    std::uint64_t sequence =
        correlation->sourceSequence.fetch_add(
            1, std::memory_order_acq_rel) + 1;
    if ( sequence == 0 )
    {
        sequence = correlation->sourceSequence.fetch_add(
            1, std::memory_order_acq_rel) + 1;
    }
    return sequence;
}

void ClearStopIntent(MediaState& state)
{
    state.stopEpoch = 0;
    state.stopPauseAfterSequence = 0;
    state.stopSeekAfterSequence = 0;
    state.stopPauseObservedEpoch = 0;
    state.stopSeekObservedEpoch = 0;
    state.stopPauseOperationId = 0;
    state.stopSeekOperationId = 0;
    state.stopPauseSequence = 0;
    state.stopSeekSequence = 0;
}

void ClearPlayIntent(MediaState& state)
{
    state.playEpoch = 0;
    state.playOperationId = 0;
    state.playAfterSequence = 0;
}

void ClearPauseIntent(MediaState& state)
{
    state.pauseEpoch = 0;
    state.pauseOperationId = 0;
    state.pauseAfterSequence = 0;
}

std::uint64_t NextCommandEpoch(const MediaState& state)
{
    std::uint64_t epoch = state.commandEpoch + 1;
    if ( epoch == 0 )
        ++epoch;
    return epoch;
}

void CommitCommandEpoch(MediaState& state, std::uint64_t epoch)
{
    state.commandEpoch = epoch;
    ClearPlayIntent(state);
    ClearStopIntent(state);
    ClearPauseIntent(state);
}

std::uint64_t BeginCommandEpoch(MediaState& state)
{
    const std::uint64_t epoch = NextCommandEpoch(state);
    CommitCommandEpoch(state, epoch);
    return state.commandEpoch;
}

void BeginPlayIntent(MediaState& state,
                     std::uint64_t commandEpoch,
                     const NativeOperationTicket& playTicket)
{
    CommitCommandEpoch(state, commandEpoch);
    state.playEpoch = commandEpoch;
    state.playOperationId = playTicket.operationId;
    state.playAfterSequence = playTicket.reservationBarrier;
}

void BeginPauseIntent(MediaState& state,
                      std::uint64_t commandEpoch,
                      const NativeOperationTicket& pauseTicket)
{
    CommitCommandEpoch(state, commandEpoch);
    state.pauseEpoch = commandEpoch;
    state.pauseOperationId = pauseTicket.operationId;
    state.pauseAfterSequence = pauseTicket.reservationBarrier;
}

void BeginStopIntent(MediaState& state,
                     std::uint64_t commandEpoch,
                     bool pauseAlreadyObserved,
                     const NativeOperationTicket& pauseTicket,
                     const NativeOperationTicket& seekTicket)
{
    CommitCommandEpoch(state, commandEpoch);
    state.stopEpoch = commandEpoch;
    state.stopPauseAfterSequence =
        pauseTicket.IsValid() ? pauseTicket.reservationBarrier
                              : seekTicket.reservationBarrier;
    state.stopSeekAfterSequence = seekTicket.reservationBarrier;
    state.stopPauseOperationId = pauseTicket.operationId;
    state.stopSeekOperationId = seekTicket.operationId;
    if ( pauseAlreadyObserved )
    {
        // A Stop issued from an already-Paused native state needs no second
        // Paused notification. This synchronous sample belongs to this
        // command epoch; queued pre-command callbacks still cannot satisfy it.
        state.stopPauseObservedEpoch = commandEpoch;
        state.stopPauseSequence = state.stopPauseAfterSequence;
    }
}

void CompleteLogicalStopIfReady(MediaState& state)
{
    const std::uint64_t epoch = state.stopEpoch;
    if ( epoch == 0 || state.stopPauseObservedEpoch != epoch ||
         state.stopSeekObservedEpoch != epoch )
    {
        return;
    }

    state.lastCompletedStopEpoch = epoch;
    state.lastCompletedStopPauseOperationId =
        state.stopPauseOperationId;
    state.lastCompletedStopSeekOperationId =
        state.stopSeekOperationId;
    state.lastCompletedStopPauseSequence = state.stopPauseSequence;
    state.lastCompletedStopSeekSequence = state.stopSeekSequence;
    ClearStopIntent(state);
    state.logicalState = LogicalMediaState::Stopped;
    ++state.logicalStopEvents;
}

void ObservePlaybackStateChanged(MediaState& state,
                                 WMP::MediaPlaybackState native,
                                 std::uint64_t sourceSequence,
                                 const NativeOperationTicket& operation)
{
    ++state.playbackStateChanges;
    if ( native == WMP::MediaPlaybackState::Playing )
    {
        if ( state.playEpoch != 0 )
        {
            if ( operation.kind != NativeOperationKind::PublicPlay ||
                 operation.operationId != state.playOperationId ||
                 operation.commandEpoch != state.playEpoch ||
                 operation.reservationBarrier != state.playAfterSequence ||
                 sourceSequence <= state.playAfterSequence )
            {
                ++state.stalePlayIntentEvents;
                return;
            }

            state.nativeState = native;
            const std::uint64_t epoch = state.playEpoch;
            const std::uint64_t playBarrier = state.playAfterSequence;
            state.lastCompletedPlayEpoch = epoch;
            state.lastCompletedPlayOperationId = operation.operationId;
            state.lastCompletedPlaySequence = sourceSequence;
            ClearPlayIntent(state);
            ++state.logicalPlayEvents;

            // As for Pause, preserve the exact user event across dispatcher
            // reordering without allowing it to undo a causally later EOF.
            if ( state.lastEofSequence == 0 ||
                 state.lastEofSequence <= playBarrier )
            {
                state.logicalState = LogicalMediaState::Playing;
            }
            return;
        }

        // A valid ticket without its matching reducer intent (for example
        // after Load or failure cleanup) remains stale. An uncommanded Playing
        // state is not a wx Play event either.
        if ( operation.IsValid() )
            ++state.stalePlayIntentEvents;
        else
            ++state.uncommandedPlayingStates;
        return;
    }

    if ( native != WMP::MediaPlaybackState::Paused )
    {
        state.nativeState = native;
        return;
    }

    if ( state.stopEpoch != 0 )
    {
        if ( operation.kind != NativeOperationKind::StopPause ||
             operation.operationId != state.stopPauseOperationId ||
             operation.commandEpoch != state.stopEpoch ||
             operation.reservationBarrier !=
                 state.stopPauseAfterSequence ||
             sourceSequence <= state.stopPauseAfterSequence )
        {
            ++state.staleStopPauseEvents;
            return;
        }

        state.nativeState = native;
        state.stopPauseObservedEpoch = state.stopEpoch;
        state.stopPauseSequence = sourceSequence;
        CompleteLogicalStopIfReady(state);
        return;
    }

    if ( state.pauseEpoch != 0 )
    {
        if ( operation.kind != NativeOperationKind::PublicPause ||
             operation.operationId != state.pauseOperationId ||
             operation.commandEpoch != state.pauseEpoch ||
             operation.reservationBarrier != state.pauseAfterSequence ||
             sourceSequence <= state.pauseAfterSequence )
        {
            ++state.stalePauseIntentEvents;
            return;
        }

        state.nativeState = native;
        const std::uint64_t epoch = state.pauseEpoch;
        const std::uint64_t pauseBarrier = state.pauseAfterSequence;
        state.lastCompletedPauseEpoch = epoch;
        state.lastCompletedPauseOperationId = operation.operationId;
        ClearPauseIntent(state);
        ++state.logicalPauseEvents;

        // Dispatcher callbacks can be delivered in a different order from
        // their source delegates. Preserve the Pause event, but never undo a
        // causally later EOF, even if its UI callback ran first or its native
        // terminal Paused notification happened after MediaEnded.
        if ( state.lastEofSequence == 0 ||
             state.lastEofSequence <= pauseBarrier )
        {
            state.logicalState = LogicalMediaState::Paused;
        }
        return;
    }

    // A valid ticket without its matching reducer intent (for example after
    // Load or failure cleanup) remains stale and must not overwrite the newer
    // native/logical state snapshot.
    if ( operation.IsValid() )
    {
        ++state.stalePauseIntentEvents;
        return;
    }

    // A native terminal Paused without a public Pause intent is not a wx
    // Pause event. MediaEnded owns the EOF stop/finished transition. This is
    // causal and deliberately independent of position or a time threshold.
    state.nativeState = native;
    ++state.uncommandedPausedStates;
}

void ObserveSeekCompleted(MediaState& state,
                          std::uint64_t sourceSequence,
                          const NativeOperationTicket& operation)
{
    ++state.seekCompleted;
    if ( operation.kind == NativeOperationKind::PublicSeek &&
         operation.operationId == state.lastIssuedPublicSeekOperationId &&
         operation.commandEpoch == state.commandEpoch &&
         sourceSequence > operation.reservationBarrier )
    {
        state.lastCompletedPublicSeekOperationId = operation.operationId;
    }
    if ( state.stopEpoch == 0 )
        return;

    if ( operation.kind != NativeOperationKind::StopSeek ||
         operation.operationId != state.stopSeekOperationId ||
         operation.commandEpoch != state.stopEpoch ||
         operation.reservationBarrier != state.stopSeekAfterSequence ||
         sourceSequence <= state.stopSeekAfterSequence )
    {
        ++state.staleStopSeekEvents;
        return;
    }

    state.stopSeekObservedEpoch = state.stopEpoch;
    state.stopSeekSequence = sourceSequence;
    CompleteLogicalStopIfReady(state);
}

void ObserveMediaEnded(MediaState& state,
                       std::uint64_t loadGeneration,
                       std::uint64_t sourceSequence)
{
    ++state.ended;
    state.endedLoad = loadGeneration;
    state.lastEofSequence = sourceSequence;
    ClearStopIntent(state);
    state.logicalState = LogicalMediaState::Stopped;
    // wxMediaBackendCommonBase::SendStopEvent() is the single, vetoable stop
    // notification at EOF. It is not followed by QueueStopEvent(): an allowed
    // EOF queues only state-changed + finished.
    ++state.eofStopRequests;
    ++state.logicalStopEvents;
    if ( state.allowEofStop )
        ++state.logicalFinishEvents;
    else
        ++state.eofStopVetoes;
}

class NativeOpenGate final
{
public:
    explicit NativeOpenGate(const wxString& url)
        : m_uri(winrt::hstring(url.wc_str()))
    {
    }

    void Enter(const WMC::MediaBindingEventArgs& args,
               const std::shared_ptr<CallbackLedger>& ledger) noexcept
    {
        try
        {
            WF::Deferral deferral = args.GetDeferral();
            args.SetUri(m_uri);
            bool releaseNow = false;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                releaseNow = m_releaseRequested;
                if ( !releaseNow )
                    m_deferrals.push_back(deferral);
            }
            m_entries.fetch_add(1, std::memory_order_release);
            if ( releaseNow )
                Complete(deferral, ledger);
        }
        catch ( ... )
        {
            ledger->callbackExceptions.fetch_add(
                1, std::memory_order_relaxed);
        }
    }

    void Release(const std::shared_ptr<CallbackLedger>& ledger) noexcept
    {
        std::vector<WF::Deferral> deferrals;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_releaseRequested = true;
            deferrals.swap(m_deferrals);
        }
        for ( const WF::Deferral& deferral : deferrals )
            Complete(deferral, ledger);
    }

    unsigned Entries() const
    {
        return m_entries.load(std::memory_order_acquire);
    }

    unsigned CompletionAttempts() const
    {
        return m_completionAttempts.load(std::memory_order_acquire);
    }

    unsigned CompletionSuccesses() const
    {
        return m_completionSuccesses.load(std::memory_order_acquire);
    }

private:
    void Complete(const WF::Deferral& deferral,
                  const std::shared_ptr<CallbackLedger>& ledger) noexcept
    {
        m_completionAttempts.fetch_add(1, std::memory_order_relaxed);
        try
        {
            deferral.Complete();
            m_completionSuccesses.fetch_add(1, std::memory_order_release);
        }
        catch ( ... )
        {
            ledger->callbackExceptions.fetch_add(
                1, std::memory_order_relaxed);
        }
    }

    mutable std::mutex m_mutex;
    WF::Uri m_uri{ nullptr };
    std::vector<WF::Deferral> m_deferrals;
    bool m_releaseRequested = false;
    std::atomic<unsigned> m_entries{0};
    std::atomic<unsigned> m_completionAttempts{0};
    std::atomic<unsigned> m_completionSuccesses{0};
};

bool IsXamlFocusInside(const MUX::UIElement& content,
                       const MUX::XamlRoot& root)
{
    try
    {
        if ( !content || !root )
            return false;

        const MUX::DependencyObject contentObject =
            content.as<MUX::DependencyObject>();
        MUX::DependencyObject current =
            MUX::Input::FocusManager::GetFocusedElement(root)
                .try_as<MUX::DependencyObject>();
        for ( unsigned depth = 0; current && depth != 128; ++depth )
        {
            if ( current == contentObject )
                return true;
            current = MUXM::VisualTreeHelper::GetParent(current);
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }
    return false;
}

unsigned RemainingSoakRounds(const wxStopWatch& watch,
                             unsigned maximumRounds)
{
    const long remaining = MediaSoakBudgetMilliseconds - watch.Time();
    if ( remaining <= 0 )
        return 0;

    return std::min<unsigned>(
        maximumRounds,
        static_cast<unsigned>((remaining + 1) / 2));
}

template <typename Callback>
void QueueOnDispatcher(
    const MUID::DispatcherQueue& dispatcher,
    const std::weak_ptr<MediaState>& weakState,
    const std::shared_ptr<CallbackLedger>& ledger,
    std::uint64_t lifetimeGeneration,
    std::uint64_t loadGeneration,
    Callback callback) noexcept
{
    ledger->sourceCallbacks.fetch_add(1, std::memory_order_relaxed);
    if ( dispatcher && dispatcher.HasThreadAccess() )
    {
        ledger->callbacksOnDispatcher.fetch_add(
            1, std::memory_order_relaxed);
    }
    else
    {
        ledger->callbacksOffDispatcher.fetch_add(
            1, std::memory_order_relaxed);
    }

    try
    {
        const bool queued = dispatcher && dispatcher.TryEnqueue(
            [weakState, ledger, lifetimeGeneration, loadGeneration,
             callback = std::move(callback)]() mutable noexcept
            {
                const std::shared_ptr<MediaState> state = weakState.lock();
                if ( !state )
                {
                    ledger->expiredState.fetch_add(
                        1, std::memory_order_relaxed);
                    return;
                }
                if ( !state->alive ||
                     state->lifetimeGeneration != lifetimeGeneration )
                {
                    ledger->staleLifetime.fetch_add(
                        1, std::memory_order_relaxed);
                    return;
                }
                if ( state->loadGeneration != loadGeneration )
                {
                    ledger->staleLoad.fetch_add(
                        1, std::memory_order_relaxed);
                    return;
                }

                ledger->accepted.fetch_add(1, std::memory_order_relaxed);
                try
                {
                    callback(*state);
                }
                catch ( const winrt::hresult_error& e )
                {
                    state->callbackHr = e.code().value;
                    ledger->callbackExceptions.fetch_add(
                        1, std::memory_order_relaxed);
                }
                catch ( ... )
                {
                    state->callbackHr = E_UNEXPECTED;
                    ledger->callbackExceptions.fetch_add(
                        1, std::memory_order_relaxed);
                }
            });
        if ( queued )
            ledger->enqueued.fetch_add(1, std::memory_order_relaxed);
        else
            ledger->enqueueFailures.fetch_add(1, std::memory_order_relaxed);
    }
    catch ( ... )
    {
        ledger->enqueueFailures.fetch_add(1, std::memory_order_relaxed);
    }
}

class MediaPeerSession final
{
public:
    MediaPeerSession() = default;

    ~MediaPeerSession()
    {
        Close();
    }

    bool Initialize(wxWinUIXamlHost *owner,
                    const std::shared_ptr<CallbackLedger>& ledger)
    {
        if ( !owner || !ledger || m_state )
            return false;

        m_owner = owner;
        m_ledger = ledger;
        m_state = std::make_shared<MediaState>();
        m_correlation = std::make_shared<NativeEventCorrelation>();
        try
        {
            m_peer = MUXC::MediaPlayerElement();
            m_dispatcher = m_peer.DispatcherQueue();
            m_player = WMP::MediaPlayer();
            m_playback = m_player.PlaybackSession();
            if ( !m_peer || !m_dispatcher || !m_player || !m_playback )
                return false;

            // The test must never produce sound. Volume changes are still
            // exercised below while the independent mute bit remains true.
            m_player.IsMuted(true);
            m_player.Volume(0.0);
            m_player.AutoPlay(false);
            m_player.CommandManager().IsEnabled(false);
            m_peer.AutoPlay(false);
            m_peer.AreTransportControlsEnabled(false);
            // MediaPlayerElement is not a tab stop by default. A wx control
            // must nevertheless participate in the common host focus chain.
            m_peer.IsTabStop(true);
            m_peer.SetMediaPlayer(m_player);

            wxWinUITopLevelHost * const host =
                wxWinUITopLevelHost::ReconcileSlotOwner(owner, true);
            if ( !host || !host->RegisterSlot(owner, m_peer) )
                return false;
            host->SetSlotPreferredFocus(owner, m_peer);
            m_initialized = true;
            m_ledger->sessionsInitialized.fetch_add(
                1, std::memory_order_relaxed);
        }
        catch ( const winrt::hresult_error& e )
        {
            m_state->callbackHr = e.code().value;
            Close();
            return false;
        }
        return true;
    }

    bool LoadUrl(
        const wxString& url,
        const std::shared_ptr<NativeOpenGate>& nativeOpenGate = nullptr)
    {
        if ( !m_state || !m_state->alive || !m_peer || !m_player ||
             url.empty() )
        {
            return false;
        }

        const bool previousBinderRevoked =
            RevokeNativeOpenGateHandler();
        ReleaseNativeOpenGate();
        m_binder = nullptr;
        const bool previousHandlersRevoked = RevokeLoadHandlers();
        m_operations.reset();
        AdvanceLoadGeneration();
        const std::uint64_t loadGeneration = m_state->loadGeneration;
        m_state->openedLoad = 0;
        m_state->failedLoad = 0;
        m_state->endedLoad = 0;
        m_state->lastError = WMP::MediaPlayerError::Unknown;
        m_state->lastExtendedError = S_OK;
        m_state->lastErrorMessage.clear();
        m_state->nativeState = WMP::MediaPlaybackState::None;
        m_state->logicalState = LogicalMediaState::Stopped;
        BeginCommandEpoch(*m_state);
        m_state->lastIssuedPublicSeekOperationId = 0;
        m_state->lastCompletedPublicSeekOperationId = 0;
        m_state->lastEofSequence = 0;
        m_state->durationTicks = 0;
        m_state->videoWidth = 0;
        m_state->videoHeight = 0;

        bool oldSourceReleased = true;
        try
        {
            m_peer.Source(nullptr);
        }
        catch ( const winrt::hresult_error& e )
        {
            m_state->callbackHr = e.code().value;
            oldSourceReleased = false;
        }
        catch ( ... )
        {
            m_state->callbackHr = E_UNEXPECTED;
            oldSourceReleased = false;
        }

        if ( m_source )
        {
            m_ledger->sourceCloseAttempts.fetch_add(
                1, std::memory_order_relaxed);
            try
            {
                m_source.Close();
                m_ledger->sourceCloseSuccesses.fetch_add(
                    1, std::memory_order_relaxed);
            }
            catch ( const winrt::hresult_error& e )
            {
                m_state->callbackHr = e.code().value;
                oldSourceReleased = false;
            }
            catch ( ... )
            {
                m_state->callbackHr = E_UNEXPECTED;
                oldSourceReleased = false;
            }
            m_source = nullptr;
        }
        if ( !oldSourceReleased || !previousHandlersRevoked ||
             !previousBinderRevoked )
        {
            return false;
        }

        try
        {
            // A tracker is scoped to exactly one load generation. Delegates
            // already in flight retain the old tracker and therefore cannot
            // consume an operation issued for this new source.
            m_operations = std::make_shared<NativeOperationTracker>();
            BindLoadHandlers(loadGeneration);
            if ( nativeOpenGate )
            {
                m_nativeOpenGate = nativeOpenGate;
                m_binder = WMC::MediaBinder();
                m_binder.Token(winrt::hstring(url.wc_str()));
                const std::shared_ptr<CallbackLedger> ledger = m_ledger;
                m_bindingToken = m_binder.Binding(
                    [nativeOpenGate, ledger](
                        const WMC::MediaBinder&,
                        const WMC::MediaBindingEventArgs& args)
                    {
                        nativeOpenGate->Enter(args, ledger);
                    });
                m_hasBindingToken = true;
                m_ledger->binderHandlerAdds.fetch_add(
                    1, std::memory_order_relaxed);
                m_source =
                    WMC::MediaSource::CreateFromMediaBinder(m_binder);
            }
            else
            {
                const WF::Uri uri(winrt::hstring(url.wc_str()));
                m_source = WMC::MediaSource::CreateFromUri(uri);
            }
            m_peer.Source(m_source);
            if ( nativeOpenGate )
            {
                // Late binding is demand-driven. This muted, content-less
                // Play request deterministically asks the native pipeline to
                // resolve the binder; the gate prevents media from opening.
                m_player.Play();
            }
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            m_state->callbackHr = e.code().value;
            RevokeLoadHandlers();
            RevokeNativeOpenGateHandler();
            ReleaseNativeOpenGate();
            m_operations.reset();
            m_binder = nullptr;
            if ( m_source )
            {
                m_ledger->sourceCloseAttempts.fetch_add(
                    1, std::memory_order_relaxed);
                try
                {
                    m_source.Close();
                    m_ledger->sourceCloseSuccesses.fetch_add(
                        1, std::memory_order_relaxed);
                }
                catch ( ... )
                {
                }
                m_source = nullptr;
            }
            return false;
        }
    }

    bool WaitOpenedOrFailed(std::uint64_t loadGeneration = 0,
                            unsigned rounds = MediaWaitRounds) const
    {
        const std::shared_ptr<MediaState> state = m_state;
        if ( !state )
            return false;
        if ( !loadGeneration )
            loadGeneration = state->loadGeneration;
        return DrainUntil(
            [state, loadGeneration]()
            {
                return state->openedLoad == loadGeneration ||
                       state->failedLoad == loadGeneration;
            }, rounds);
    }

    bool WaitOpened(std::uint64_t loadGeneration = 0,
                    unsigned rounds = MediaWaitRounds) const
    {
        if ( !WaitOpenedOrFailed(loadGeneration, rounds) || !m_state )
            return false;
        if ( !loadGeneration )
            loadGeneration = m_state->loadGeneration;
        return m_state->openedLoad == loadGeneration &&
               m_state->failedLoad != loadGeneration;
    }

    bool Play()
    {
        if ( !IsOpened() || !m_operations )
            return false;
        if ( PlaybackCommandIsBlocked(*m_state, *m_operations) )
            return false;
        NativeOperationTicket playTicket;
        try
        {
            for ( unsigned attempt = 0;
                  attempt < NativeSnapshotRetryLimit; ++attempt )
            {
                const std::uint64_t expectedSequence =
                    CaptureEntrySequence(*m_correlation);
                const WMP::MediaPlaybackState native =
                    m_playback.PlaybackState();

                // MediaPlayer need not emit PlaybackStateChanged for an
                // idempotent Play. Accept it only if no native delegate entered
                // between the sequence sample and the state read.
                if ( native == WMP::MediaPlaybackState::Playing )
                {
                    if ( EntrySequenceMatches(
                             *m_correlation, expectedSequence) )
                    {
                        return true;
                    }
                    continue;
                }

                const std::uint64_t playEpoch =
                    NextCommandEpoch(*m_state);
                const NativeOperationStartResult start =
                    m_operations->TryBeginPublicPlay(
                        playEpoch, *m_correlation, expectedSequence,
                        &playTicket);
                if ( start == NativeOperationStartResult::StaleSnapshot )
                    continue;
                if ( start == NativeOperationStartResult::Busy )
                    return false;

                BeginPlayIntent(*m_state, playEpoch, playTicket);
                m_state->lastEofSequence = 0;
                m_player.Play();
                return true;
            }
            return false;
        }
        catch ( const winrt::hresult_error& e )
        {
            if ( playTicket.IsValid() )
            {
                m_operations->Cancel(playTicket);
                BeginCommandEpoch(*m_state);
            }
            m_state->callbackHr = e.code().value;
            return false;
        }
    }

    bool Pause()
    {
        if ( !IsOpened() || !m_operations )
            return false;
        if ( PlaybackCommandIsBlocked(*m_state, *m_operations) )
            return false;
        NativeOperationTicket pauseTicket;
        try
        {
            for ( unsigned attempt = 0;
                  attempt < NativeSnapshotRetryLimit; ++attempt )
            {
                const std::uint64_t expectedSequence =
                    CaptureEntrySequence(*m_correlation);
                const WMP::MediaPlaybackState native =
                    m_playback.PlaybackState();

                if ( native == WMP::MediaPlaybackState::Paused )
                {
                    if ( EntrySequenceMatches(
                             *m_correlation, expectedSequence) )
                    {
                        return true;
                    }
                    continue;
                }

                const bool canPause = m_playback.CanPause();
                if ( !canPause )
                {
                    if ( EntrySequenceMatches(
                             *m_correlation, expectedSequence) )
                    {
                        return false;
                    }
                    continue;
                }

                const std::uint64_t pauseEpoch =
                    NextCommandEpoch(*m_state);
                const NativeOperationStartResult start =
                    m_operations->TryBeginPublicPause(
                        pauseEpoch, *m_correlation, expectedSequence,
                        &pauseTicket);
                if ( start == NativeOperationStartResult::StaleSnapshot )
                    continue;
                if ( start == NativeOperationStartResult::Busy )
                    return false;

                BeginPauseIntent(*m_state, pauseEpoch, pauseTicket);
                m_player.Pause();
                return true;
            }
            return false;
        }
        catch ( const winrt::hresult_error& e )
        {
            if ( pauseTicket.IsValid() )
            {
                m_operations->Cancel(pauseTicket);
                BeginCommandEpoch(*m_state);
            }
            m_state->callbackHr = e.code().value;
            return false;
        }
    }

    // Windows.Media.Playback has Play/Pause but no Stop operation. This is
    // the exact adapter a wx backend needs: pause plus a seek to zero, with a
    // distinct logical stopped state/event. Public Stop() is not the wx veto
    // boundary; only the stop notification delivered for EOF is vetoable.
    bool Stop()
    {
        if ( !IsOpened() || !m_operations )
            return false;
        if ( PlaybackCommandIsBlocked(*m_state, *m_operations) )
            return false;
        NativeOperationTicket pauseTicket;
        NativeOperationTicket seekTicket;
        try
        {
            for ( unsigned attempt = 0;
                  attempt < NativeSnapshotRetryLimit; ++attempt )
            {
                const std::uint64_t expectedSequence =
                    CaptureEntrySequence(*m_correlation);
                const WMP::MediaPlaybackState native =
                    m_playback.PlaybackState();
                const bool alreadyPaused =
                    native == WMP::MediaPlaybackState::Paused;
                const bool canPause = alreadyPaused || m_playback.CanPause();
                const bool canSeek = m_playback.CanSeek();
                if ( !canPause || !canSeek )
                {
                    if ( EntrySequenceMatches(
                             *m_correlation, expectedSequence) )
                    {
                        return false;
                    }
                    continue;
                }

                const std::uint64_t stopEpoch =
                    NextCommandEpoch(*m_state);
                const NativeOperationStartResult start =
                    m_operations->TryBeginStop(
                        stopEpoch, *m_correlation, expectedSequence,
                        !alreadyPaused, &pauseTicket, &seekTicket);
                if ( start == NativeOperationStartResult::StaleSnapshot )
                    continue;
                if ( start == NativeOperationStartResult::Busy )
                    return false;

                BeginStopIntent(*m_state, stopEpoch, alreadyPaused,
                                pauseTicket, seekTicket);
                if ( !alreadyPaused )
                    m_player.Pause();
                m_playback.Position(std::chrono::milliseconds(0));
                return true;
            }
            return false;
        }
        catch ( const winrt::hresult_error& e )
        {
            if ( pauseTicket.IsValid() || seekTicket.IsValid() )
            {
                m_operations->Cancel(pauseTicket);
                m_operations->Cancel(seekTicket);
                BeginCommandEpoch(*m_state);
            }
            m_state->callbackHr = e.code().value;
            return false;
        }
    }

    bool WaitNativeState(WMP::MediaPlaybackState expected,
                         unsigned rounds = StateWaitRounds) const
    {
        const std::shared_ptr<MediaState> state = m_state;
        return state && DrainUntil(
            [state, expected]() { return state->nativeState == expected; },
            rounds);
    }

    bool WaitSeekAfter(unsigned previous,
                       unsigned rounds = StateWaitRounds) const
    {
        const std::shared_ptr<MediaState> state = m_state;
        const std::uint64_t targetOperation =
            state ? state->lastIssuedPublicSeekOperationId : 0;
        return state && DrainUntil(
            [state, previous, targetOperation]()
            {
                return targetOperation != 0 &&
                       state->lastCompletedPublicSeekOperationId ==
                           targetOperation &&
                       state->seekCompleted >= previous + 1;
            }, rounds);
    }

    bool WaitStoppedAfter(unsigned previousStopEvents,
                          unsigned previousSeeks,
                          unsigned rounds = StateWaitRounds) const
    {
        const std::shared_ptr<MediaState> state = m_state;
        return state && DrainUntil(
            [this, state, previousStopEvents, previousSeeks]()
            {
                return state->logicalStopEvents == previousStopEvents + 1 &&
                       state->seekCompleted == previousSeeks + 1 &&
                       state->nativeState ==
                           WMP::MediaPlaybackState::Paused &&
                       state->logicalState == LogicalMediaState::Stopped &&
                       PositionMilliseconds() < 100;
            }, rounds);
    }

    bool SeekMilliseconds(std::int64_t milliseconds)
    {
        if ( !IsOpened() || !m_operations || milliseconds < 0 )
            return false;
        if ( PlaybackCommandIsBlocked(*m_state, *m_operations) )
            return false;
        NativeOperationTicket seekTicket;
        try
        {
            for ( unsigned attempt = 0;
                  attempt < NativeSnapshotRetryLimit; ++attempt )
            {
                const std::uint64_t expectedSequence =
                    CaptureEntrySequence(*m_correlation);
                const bool canSeek = m_playback.CanSeek();
                if ( !canSeek )
                {
                    if ( EntrySequenceMatches(
                             *m_correlation, expectedSequence) )
                    {
                        return false;
                    }
                    continue;
                }

                const NativeOperationStartResult start =
                    m_operations->TryBeginPublicSeek(
                        m_state->commandEpoch, *m_correlation,
                        expectedSequence, &seekTicket);
                if ( start == NativeOperationStartResult::StaleSnapshot )
                    continue;
                if ( start == NativeOperationStartResult::Busy )
                    return false;

                m_state->lastIssuedPublicSeekOperationId =
                    seekTicket.operationId;
                m_playback.Position(
                    std::chrono::milliseconds(milliseconds));
                return true;
            }
            return false;
        }
        catch ( const winrt::hresult_error& e )
        {
            m_operations->Cancel(seekTicket);
            if ( m_state->lastIssuedPublicSeekOperationId ==
                 seekTicket.operationId )
            {
                m_state->lastIssuedPublicSeekOperationId = 0;
            }
            m_state->callbackHr = e.code().value;
            return false;
        }
    }

    std::int64_t PositionMilliseconds() const
    {
        if ( !m_playback )
            return -1;
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   m_playback.Position()).count();
    }

    std::int64_t DurationMilliseconds() const
    {
        if ( !m_playback )
            return -1;
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   m_playback.NaturalDuration()).count();
    }

    bool SetPlaybackRate(double rate)
    {
        if ( !IsOpened() || rate <= 0.0 )
            return false;
        try
        {
            if ( !m_playback.IsSupportedPlaybackRateRange(rate, rate) )
                return false;
            m_playback.PlaybackRate(rate);
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            m_state->callbackHr = e.code().value;
            return false;
        }
    }

    double PlaybackRate() const
    {
        return m_playback ? m_playback.PlaybackRate() : 0.0;
    }

    bool SetVolume(double volume)
    {
        if ( !m_player || volume < 0.0 || volume > 1.0 )
            return false;
        try
        {
            m_player.Volume(volume);
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            if ( m_state )
                m_state->callbackHr = e.code().value;
            return false;
        }
    }

    double Volume() const { return m_player ? m_player.Volume() : 0.0; }
    bool IsMuted() const { return m_player && m_player.IsMuted(); }

    bool ShowControls(wxMediaCtrlPlayerControls flags)
    {
        if ( !m_peer ||
             (flags != wxMEDIACTRLPLAYERCONTROLS_NONE &&
              flags != wxMEDIACTRLPLAYERCONTROLS_DEFAULT) )
        {
            return false;
        }

        try
        {
            const bool show = flags == wxMEDIACTRLPLAYERCONTROLS_DEFAULT;
            m_peer.AreTransportControlsEnabled(show);
            return m_peer.AreTransportControlsEnabled() == show;
        }
        catch ( const winrt::hresult_error& e )
        {
            if ( m_state )
                m_state->callbackHr = e.code().value;
            return false;
        }
    }

    bool AreControlsShown() const
    {
        return m_peer && m_peer.AreTransportControlsEnabled();
    }

    void PostGenerationProbeForTest(std::uint64_t loadGeneration)
    {
        if ( !m_state )
            return;
        QueueOnDispatcher(
            m_dispatcher, std::weak_ptr<MediaState>(m_state), m_ledger,
            m_state->lifetimeGeneration, loadGeneration,
            [](MediaState& state) { ++state.probesAccepted; });
    }

    void Close() noexcept
    {
        if ( m_closed )
            return;
        m_closed = true;

        if ( m_state )
        {
            m_state->alive = false;
            AdvanceNonZero(m_state->lifetimeGeneration);
            AdvanceNonZero(m_state->loadGeneration);
        }

        RevokeLoadHandlers();
        RevokeNativeOpenGateHandler();
        // The test-only binder gate represents native resolution genuinely
        // in flight. Release its deferral only after state invalidation and
        // token revocation, before closing the source/player graph.
        ReleaseNativeOpenGate();
        const auto teardownStep =
            [this](const std::function<void()>& operation) noexcept -> bool
        {
            if ( m_ledger )
            {
                m_ledger->teardownStepAttempts.fetch_add(
                    1, std::memory_order_relaxed);
            }
            try
            {
                operation();
                if ( m_ledger )
                {
                    m_ledger->teardownStepSuccesses.fetch_add(
                        1, std::memory_order_relaxed);
                }
                return true;
            }
            catch ( const winrt::hresult_error& e )
            {
                if ( m_state )
                    m_state->callbackHr = e.code().value;
                if ( m_ledger )
                {
                    m_ledger->teardownStepFailures.fetch_add(
                        1, std::memory_order_relaxed);
                }
                return false;
            }
            catch ( ... )
            {
                if ( m_state )
                    m_state->callbackHr = E_UNEXPECTED;
                if ( m_ledger )
                {
                    m_ledger->teardownStepFailures.fetch_add(
                        1, std::memory_order_relaxed);
                }
                return false;
            }
        };

        // Microsoft documents this order for an explicitly supplied player.
        // Each step is independent so one failure never suppresses the rest.
        if ( m_player && m_playback )
        {
            teardownStep(
                [this]()
                {
                    if ( m_playback.CanPause() )
                        m_player.Pause();
                });
        }
        if ( m_peer )
            teardownStep([this]() { m_peer.Source(nullptr); });
        if ( m_peer )
        {
            teardownStep(
                [this]()
                {
                    m_peer.SetMediaPlayer(WMP::MediaPlayer{ nullptr });
                });
        }
        if ( m_source )
        {
            if ( m_ledger )
            {
                m_ledger->sourceCloseAttempts.fetch_add(
                    1, std::memory_order_relaxed);
            }
            if ( teardownStep([this]() { m_source.Close(); }) && m_ledger )
            {
                m_ledger->sourceCloseSuccesses.fetch_add(
                    1, std::memory_order_relaxed);
            }
        }
        if ( m_player )
        {
            if ( m_ledger )
            {
                m_ledger->playerCloseAttempts.fetch_add(
                    1, std::memory_order_relaxed);
            }
            if ( teardownStep([this]() { m_player.Close(); }) && m_ledger )
            {
                m_ledger->playerCloseSuccesses.fetch_add(
                    1, std::memory_order_relaxed);
            }
        }

        if ( wxWinUIXamlHost * const owner = m_owner.get() )
        {
            if ( wxWinUITopLevelHost * const host =
                     wxWinUITopLevelHost::FindSlotOwner(owner) )
            {
                teardownStep(
                    [host, owner]() { host->UnregisterSlot(owner); });
            }
        }

        m_source = nullptr;
        m_binder = nullptr;
        m_playback = nullptr;
        m_player = nullptr;
        m_peer = nullptr;
        m_dispatcher = nullptr;
        m_operations.reset();
        m_correlation.reset();
        m_state.reset();
        if ( m_initialized && m_ledger )
        {
            m_ledger->sessionsClosed.fetch_add(
                1, std::memory_order_relaxed);
        }
        m_initialized = false;
    }

    bool IsOpened() const
    {
        return m_state &&
               m_state->openedLoad == m_state->loadGeneration &&
               m_state->failedLoad != m_state->loadGeneration;
    }

    std::uint64_t LoadGeneration() const
    {
        return m_state ? m_state->loadGeneration : 0;
    }

    MUXC::MediaPlayerElement Peer() const { return m_peer; }
    WMP::MediaPlayer Player() const { return m_player; }
    WMP::MediaPlaybackSession Playback() const { return m_playback; }
    WMC::MediaSource Source() const { return m_source; }
    MUID::DispatcherQueue Dispatcher() const { return m_dispatcher; }
    WMP::MediaPlayer AttachedPlayer() const
    {
        return m_peer ? m_peer.MediaPlayer() : nullptr;
    }
    WMP::MediaPlaybackSession AttachedPlayback() const
    {
        const WMP::MediaPlayer attached = AttachedPlayer();
        return attached ? attached.PlaybackSession() : nullptr;
    }
    WMP::IMediaPlaybackSource AttachedPeerSource() const
    {
        return m_peer ? m_peer.Source() : nullptr;
    }
    WMP::IMediaPlaybackSource AttachedPlayerSource() const
    {
        return m_player ? m_player.Source() : nullptr;
    }
    MUID::DispatcherQueue PeerDispatcher() const
    {
        return m_peer ? m_peer.DispatcherQueue() : nullptr;
    }
    unsigned ActiveHandlerCount() const
    {
        return static_cast<unsigned>(m_hasOpenedToken) +
               static_cast<unsigned>(m_hasEndedToken) +
               static_cast<unsigned>(m_hasFailedToken) +
               static_cast<unsigned>(m_hasStateToken) +
               static_cast<unsigned>(m_hasSeekToken) +
               static_cast<unsigned>(m_hasRateToken) +
               static_cast<unsigned>(m_hasDurationToken) +
               static_cast<unsigned>(m_hasVideoSizeToken) +
               static_cast<unsigned>(m_hasVolumeToken);
    }
    std::shared_ptr<MediaState> State() const { return m_state; }
    std::weak_ptr<MediaState> WeakState() const { return m_state; }
    std::shared_ptr<NativeOperationTracker> OperationTrackerForTest() const
    {
        return m_operations;
    }
    std::weak_ptr<NativeOperationTracker> WeakOperationTrackerForTest() const
    {
        return m_operations;
    }

private:
    static void AdvanceNonZero(std::uint64_t& generation)
    {
        if ( ++generation == 0 )
            ++generation;
    }

    void AdvanceLoadGeneration()
    {
        AdvanceNonZero(m_state->loadGeneration);
    }

    void RecordHandlerAdd()
    {
        m_ledger->handlerAdds.fetch_add(1, std::memory_order_relaxed);
    }

    void RecordHandlerRevoke()
    {
        m_ledger->handlerRevokes.fetch_add(1, std::memory_order_relaxed);
    }

    bool RevokeNativeOpenGateHandler() noexcept
    {
        if ( !m_hasBindingToken )
            return true;

        if ( m_ledger )
        {
            m_ledger->teardownStepAttempts.fetch_add(
                1, std::memory_order_relaxed);
        }

        HRESULT hr = E_POINTER;
        try
        {
            if ( m_binder )
            {
                const WMC::IMediaBinder binder =
                    m_binder.as<WMC::IMediaBinder>();
                auto * const abi = reinterpret_cast<
                    winrt::impl::abi_t<WMC::IMediaBinder>*>(
                        winrt::get_abi(binder));
                hr = abi->remove_Binding(m_bindingToken);
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            hr = e.code().value;
        }
        catch ( ... )
        {
            hr = E_UNEXPECTED;
        }

        m_hasBindingToken = false;
        if ( SUCCEEDED(hr) )
        {
            if ( m_ledger )
            {
                m_ledger->binderHandlerRevokes.fetch_add(
                    1, std::memory_order_relaxed);
                m_ledger->teardownStepSuccesses.fetch_add(
                    1, std::memory_order_relaxed);
            }
            return true;
        }

        if ( m_state )
            m_state->callbackHr = hr;
        if ( m_ledger )
        {
            m_ledger->teardownStepFailures.fetch_add(
                1, std::memory_order_relaxed);
        }
        return false;
    }

    void ReleaseNativeOpenGate() noexcept
    {
        if ( m_nativeOpenGate )
        {
            m_nativeOpenGate->Release(m_ledger);
            m_nativeOpenGate.reset();
        }
    }

    void BindLoadHandlers(std::uint64_t loadGeneration)
    {
        const MUID::DispatcherQueue dispatcher = m_dispatcher;
        const std::weak_ptr<MediaState> weakState(m_state);
        const std::shared_ptr<CallbackLedger> ledger = m_ledger;
        const std::shared_ptr<NativeEventCorrelation> correlation =
            m_correlation;
        const std::shared_ptr<NativeOperationTracker> operations =
            m_operations;
        const std::uint64_t lifetimeGeneration =
            m_state->lifetimeGeneration;
        const auto queue =
            [dispatcher, weakState, ledger, lifetimeGeneration,
             loadGeneration](auto callback) noexcept
            {
                QueueOnDispatcher(
                    dispatcher, weakState, ledger, lifetimeGeneration,
                    loadGeneration, std::move(callback));
            };

        const auto onOpened = [queue, ledger, loadGeneration](
            const WMP::MediaPlayer& sender, const WF::IInspectable&)
        {
            try
            {
                const WMP::MediaPlaybackSession playback =
                    sender.PlaybackSession();
                const std::int64_t duration =
                    playback.NaturalDuration().count();
                const std::uint32_t width = playback.NaturalVideoWidth();
                const std::uint32_t height = playback.NaturalVideoHeight();
                queue(
                    [loadGeneration, duration, width, height](
                        MediaState& state)
                    {
                        ++state.opened;
                        state.openedLoad = loadGeneration;
                        state.durationTicks = duration;
                        state.videoWidth = width;
                        state.videoHeight = height;
                    });
            }
            catch ( ... )
            {
                ledger->callbackExceptions.fetch_add(
                    1, std::memory_order_relaxed);
            }
        };
        m_openedToken = m_player.MediaOpened(onOpened);
        m_hasOpenedToken = true;
        RecordHandlerAdd();

        m_endedToken = m_player.MediaEnded(
            [queue, correlation, loadGeneration](const WMP::MediaPlayer&,
                                                 const WF::IInspectable&)
            {
                const std::uint64_t sourceSequence =
                    StampNativeEvent(correlation);
                queue(
                    [loadGeneration, sourceSequence](MediaState& state)
                    {
                        ObserveMediaEnded(state, loadGeneration,
                                          sourceSequence);
                    });
            });
        m_hasEndedToken = true;
        RecordHandlerAdd();

        m_failedToken = m_player.MediaFailed(
            [queue, ledger, loadGeneration](
                const WMP::MediaPlayer&,
                const WMP::MediaPlayerFailedEventArgs& args)
            {
                try
                {
                    const WMP::MediaPlayerError error = args.Error();
                    const HRESULT extended = args.ExtendedErrorCode().value;
                    const std::wstring message = args.ErrorMessage().c_str();
                    queue(
                        [loadGeneration, error, extended, message](
                            MediaState& state)
                        {
                            ++state.failed;
                            state.failedLoad = loadGeneration;
                            state.lastError = error;
                            state.lastExtendedError = extended;
                            state.lastErrorMessage = message;
                            ClearPlayIntent(state);
                            ClearStopIntent(state);
                            ClearPauseIntent(state);
                            state.logicalState = LogicalMediaState::Stopped;
                        });
                }
                catch ( ... )
                {
                    ledger->callbackExceptions.fetch_add(
                        1, std::memory_order_relaxed);
                }
            });
        m_hasFailedToken = true;
        RecordHandlerAdd();

        m_stateToken = m_playback.PlaybackStateChanged(
            [queue, ledger, correlation, operations](
                const WMP::MediaPlaybackSession& sender,
                const WF::IInspectable&)
            {
                const std::uint64_t sourceSequence =
                    StampNativeEvent(correlation);
                try
                {
                    const WMP::MediaPlaybackState native =
                        sender.PlaybackState();
                    NativeOperationTicket operation;
                    if ( native == WMP::MediaPlaybackState::Playing )
                    {
                        operation = operations->CompletePlayAtSource(
                            sourceSequence);
                    }
                    else if ( native == WMP::MediaPlaybackState::Paused )
                    {
                        operation = operations->CompletePauseAtSource(
                            sourceSequence);
                    }
                    queue(
                        [native, sourceSequence, operation](
                            MediaState& state)
                        {
                            ObservePlaybackStateChanged(
                                state, native, sourceSequence, operation);
                        });
                }
                catch ( ... )
                {
                    ledger->callbackExceptions.fetch_add(
                        1, std::memory_order_relaxed);
                }
            });
        m_hasStateToken = true;
        RecordHandlerAdd();

        m_seekToken = m_playback.SeekCompleted(
            [queue, correlation, operations](
                const WMP::MediaPlaybackSession&,
                const WF::IInspectable&)
            {
                const std::uint64_t sourceSequence =
                    StampNativeEvent(correlation);
                const NativeOperationTicket operation =
                    operations->CompleteSeekAtSource(sourceSequence);
                queue(
                    [sourceSequence, operation](MediaState& state)
                    {
                        ObserveSeekCompleted(
                            state, sourceSequence, operation);
                    });
            });
        m_hasSeekToken = true;
        RecordHandlerAdd();

        m_rateToken = m_playback.PlaybackRateChanged(
            [queue](const WMP::MediaPlaybackSession&,
                    const WF::IInspectable&)
            {
                queue(
                    [](MediaState& state) { ++state.playbackRateChanges; });
            });
        m_hasRateToken = true;
        RecordHandlerAdd();

        m_durationToken = m_playback.NaturalDurationChanged(
            [queue, ledger](
                const WMP::MediaPlaybackSession& sender,
                const WF::IInspectable&)
            {
                try
                {
                    const std::int64_t duration =
                        sender.NaturalDuration().count();
                    queue(
                        [duration](MediaState& state)
                        {
                            ++state.durationChanges;
                            state.durationTicks = duration;
                        });
                }
                catch ( ... )
                {
                    ledger->callbackExceptions.fetch_add(
                        1, std::memory_order_relaxed);
                }
            });
        m_hasDurationToken = true;
        RecordHandlerAdd();

        m_videoSizeToken = m_playback.NaturalVideoSizeChanged(
            [queue, ledger](
                const WMP::MediaPlaybackSession& sender,
                const WF::IInspectable&)
            {
                try
                {
                    const std::uint32_t width = sender.NaturalVideoWidth();
                    const std::uint32_t height = sender.NaturalVideoHeight();
                    queue(
                        [width, height](MediaState& state)
                        {
                            ++state.videoSizeChanges;
                            state.videoWidth = width;
                            state.videoHeight = height;
                        });
                }
                catch ( ... )
                {
                    ledger->callbackExceptions.fetch_add(
                        1, std::memory_order_relaxed);
                }
            });
        m_hasVideoSizeToken = true;
        RecordHandlerAdd();

        m_volumeToken = m_player.VolumeChanged(
            [queue](const WMP::MediaPlayer&,
                    const WF::IInspectable&)
            {
                queue(
                    [](MediaState& state) { ++state.volumeChanges; });
            });
        m_hasVolumeToken = true;
        RecordHandlerAdd();
    }

    bool RevokeLoadHandlers() noexcept
    {
        bool allRevoked = true;
        const auto revoke =
            [this, &allRevoked](
                auto&& operation) noexcept
        {
            if ( m_ledger )
            {
                m_ledger->teardownStepAttempts.fetch_add(
                    1, std::memory_order_relaxed);
            }

            HRESULT hr = E_UNEXPECTED;
            try
            {
                // The projected remove-event overloads are noexcept and
                // intentionally discard their ABI HRESULT. Call the ABI
                // directly here so the teardown gate cannot report a false
                // success when a native revocation fails.
                hr = operation();
            }
            catch ( const winrt::hresult_error& e )
            {
                hr = e.code().value;
            }
            catch ( ... )
            {
                hr = E_UNEXPECTED;
            }

            if ( SUCCEEDED(hr) )
            {
                RecordHandlerRevoke();
                if ( m_ledger )
                {
                    m_ledger->teardownStepSuccesses.fetch_add(
                        1, std::memory_order_relaxed);
                }
            }
            else
            {
                allRevoked = false;
                if ( m_state )
                    m_state->callbackHr = hr;
                if ( m_ledger )
                {
                    m_ledger->teardownStepFailures.fetch_add(
                        1, std::memory_order_relaxed);
                }
            }
        };

        if ( m_player )
        {
            if ( m_hasOpenedToken )
            {
                revoke(
                    [this]()
                    {
                        const WMP::IMediaPlayer player =
                            m_player.as<WMP::IMediaPlayer>();
                        auto * const abi = reinterpret_cast<
                            winrt::impl::abi_t<WMP::IMediaPlayer>*>(
                                winrt::get_abi(player));
                        return abi->remove_MediaOpened(m_openedToken);
                    });
            }
            if ( m_hasEndedToken )
            {
                revoke(
                    [this]()
                    {
                        const WMP::IMediaPlayer player =
                            m_player.as<WMP::IMediaPlayer>();
                        auto * const abi = reinterpret_cast<
                            winrt::impl::abi_t<WMP::IMediaPlayer>*>(
                                winrt::get_abi(player));
                        return abi->remove_MediaEnded(m_endedToken);
                    });
            }
            if ( m_hasFailedToken )
            {
                revoke(
                    [this]()
                    {
                        const WMP::IMediaPlayer player =
                            m_player.as<WMP::IMediaPlayer>();
                        auto * const abi = reinterpret_cast<
                            winrt::impl::abi_t<WMP::IMediaPlayer>*>(
                                winrt::get_abi(player));
                        return abi->remove_MediaFailed(m_failedToken);
                    });
            }
            if ( m_hasVolumeToken )
            {
                revoke(
                    [this]()
                    {
                        const WMP::IMediaPlayer player =
                            m_player.as<WMP::IMediaPlayer>();
                        auto * const abi = reinterpret_cast<
                            winrt::impl::abi_t<WMP::IMediaPlayer>*>(
                                winrt::get_abi(player));
                        return abi->remove_VolumeChanged(m_volumeToken);
                    });
            }
        }
        if ( m_playback )
        {
            if ( m_hasStateToken )
            {
                revoke(
                    [this]()
                    {
                        const WMP::IMediaPlaybackSession playback =
                            m_playback.as<WMP::IMediaPlaybackSession>();
                        auto * const abi = reinterpret_cast<
                            winrt::impl::abi_t<WMP::IMediaPlaybackSession>*>(
                                winrt::get_abi(playback));
                        return abi->remove_PlaybackStateChanged(m_stateToken);
                    });
            }
            if ( m_hasSeekToken )
            {
                revoke(
                    [this]()
                    {
                        const WMP::IMediaPlaybackSession playback =
                            m_playback.as<WMP::IMediaPlaybackSession>();
                        auto * const abi = reinterpret_cast<
                            winrt::impl::abi_t<WMP::IMediaPlaybackSession>*>(
                                winrt::get_abi(playback));
                        return abi->remove_SeekCompleted(m_seekToken);
                    });
            }
            if ( m_hasRateToken )
            {
                revoke(
                    [this]()
                    {
                        const WMP::IMediaPlaybackSession playback =
                            m_playback.as<WMP::IMediaPlaybackSession>();
                        auto * const abi = reinterpret_cast<
                            winrt::impl::abi_t<WMP::IMediaPlaybackSession>*>(
                                winrt::get_abi(playback));
                        return abi->remove_PlaybackRateChanged(m_rateToken);
                    });
            }
            if ( m_hasDurationToken )
            {
                revoke(
                    [this]()
                    {
                        const WMP::IMediaPlaybackSession playback =
                            m_playback.as<WMP::IMediaPlaybackSession>();
                        auto * const abi = reinterpret_cast<
                            winrt::impl::abi_t<WMP::IMediaPlaybackSession>*>(
                                winrt::get_abi(playback));
                        return abi->remove_NaturalDurationChanged(
                            m_durationToken);
                    });
            }
            if ( m_hasVideoSizeToken )
            {
                revoke(
                    [this]()
                    {
                        const WMP::IMediaPlaybackSession playback =
                            m_playback.as<WMP::IMediaPlaybackSession>();
                        auto * const abi = reinterpret_cast<
                            winrt::impl::abi_t<WMP::IMediaPlaybackSession>*>(
                                winrt::get_abi(playback));
                        return abi->remove_NaturalVideoSizeChanged(
                            m_videoSizeToken);
                    });
            }
        }
        m_hasOpenedToken = false;
        m_hasEndedToken = false;
        m_hasFailedToken = false;
        m_hasStateToken = false;
        m_hasSeekToken = false;
        m_hasRateToken = false;
        m_hasDurationToken = false;
        m_hasVideoSizeToken = false;
        m_hasVolumeToken = false;
        return allRevoked;
    }

    wxWeakRef<wxWinUIXamlHost> m_owner;
    std::shared_ptr<CallbackLedger> m_ledger;
    std::shared_ptr<MediaState> m_state;
    std::shared_ptr<NativeEventCorrelation> m_correlation;
    std::shared_ptr<NativeOperationTracker> m_operations;
    MUID::DispatcherQueue m_dispatcher{ nullptr };
    MUXC::MediaPlayerElement m_peer{ nullptr };
    WMP::MediaPlayer m_player{ nullptr };
    WMP::MediaPlaybackSession m_playback{ nullptr };
    WMC::MediaSource m_source{ nullptr };
    WMC::MediaBinder m_binder{ nullptr };
    std::shared_ptr<NativeOpenGate> m_nativeOpenGate;

    winrt::event_token m_openedToken{};
    winrt::event_token m_endedToken{};
    winrt::event_token m_failedToken{};
    winrt::event_token m_stateToken{};
    winrt::event_token m_seekToken{};
    winrt::event_token m_rateToken{};
    winrt::event_token m_durationToken{};
    winrt::event_token m_videoSizeToken{};
    winrt::event_token m_volumeToken{};
    winrt::event_token m_bindingToken{};
    bool m_hasOpenedToken = false;
    bool m_hasEndedToken = false;
    bool m_hasFailedToken = false;
    bool m_hasStateToken = false;
    bool m_hasSeekToken = false;
    bool m_hasRateToken = false;
    bool m_hasDurationToken = false;
    bool m_hasVideoSizeToken = false;
    bool m_hasVolumeToken = false;
    bool m_hasBindingToken = false;
    bool m_closed = false;
    bool m_initialized = false;

    wxDECLARE_NO_COPY_CLASS(MediaPeerSession);
};

bool ShowOffscreenWithoutActivation(wxFrame *frame, unsigned ordinal)
{
    if ( !frame )
        return false;

    const HWND hwnd = GetHwndOf(frame);
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
    frame->Move(wxPoint(left - 8200 - static_cast<int>(ordinal) * 500,
                        top - 8200));

    const auto outside = [hwnd, desktop]()
    {
        RECT window = {};
        RECT intersection = {};
        return ::GetWindowRect(hwnd, &window) &&
               !::IntersectRect(&intersection, &window, &desktop);
    };
    if ( !outside() )
        return false;

    frame->ShowWithoutActivating();
    if ( !outside() )
    {
        frame->Hide();
        return false;
    }
    return true;
}

class TwoTLWMediaFixture final
{
public:
    ~TwoTLWMediaFixture()
    {
        Destroy();
    }

    bool Create()
    {
        frameA = new wxFrame(nullptr, wxID_ANY, "xaml-media-a",
                             wxDefaultPosition, wxSize(500, 320),
                             wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                                 wxFRAME_TOOL_WINDOW);
        frameB = new wxFrame(nullptr, wxID_ANY, "xaml-media-b",
                             wxDefaultPosition, wxSize(500, 320),
                             wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                                 wxFRAME_TOOL_WINDOW);
        panelA = new wxPanel(frameA, wxID_ANY,
                             wxPoint(0, 0), wxSize(470, 270));
        panelB = new wxPanel(frameB, wxID_ANY,
                             wxPoint(0, 0), wxSize(470, 270));
        owner = new wxWinUIXamlHost(panelA, wxID_ANY,
                                    wxPoint(20, 20), wxSize(260, 150),
                                    wxBORDER_NONE);
        // wxMediaCtrl is a wxControl and therefore contributes a native
        // WS_TABSTOP. wxWinUIXamlHost is only the test carrier here and is a
        // container-style wxWindow, so mirror the public control contract
        // explicitly before exercising the shared host's Tab walk.
        const HWND ownerHwnd = GetHwndOf(owner);
        if ( !ownerHwnd )
            return false;
        ::SetWindowLongPtrW(
            ownerHwnd, GWL_STYLE,
            ::GetWindowLongPtrW(ownerHwnd, GWL_STYLE) | WS_TABSTOP);
        owner->SetLabel("offline silent media player");
        buttonA = new wxButton(panelA, wxID_ANY, "after-media-a",
                               wxPoint(315, 40), wxSize(130, 48));
        buttonB = new wxButton(panelB, wxID_ANY, "after-media-b",
                               wxPoint(315, 40), wxSize(130, 48));

        ledger = std::make_shared<CallbackLedger>();
        session.reset(new MediaPeerSession);
        if ( !session->Initialize(owner, ledger) ||
             !ShowOffscreenWithoutActivation(frameA, 0) ||
             !ShowOffscreenWithoutActivation(frameB, 1) ||
             !DrainToQuiescence() )
        {
            Destroy();
            return false;
        }

        hostA = wxWinUITopLevelHost::FindForTLW(frameA);
        hostB = wxWinUITopLevelHost::FindForTLW(frameB);
        if ( !hostA || !hostB || hostA == hostB ||
             wxWinUITopLevelHost::FindSlotOwner(owner) != hostA ||
             wxWinUITopLevelHost::FindSlotOwner(buttonA) != hostA ||
             wxWinUITopLevelHost::FindSlotOwner(buttonB) != hostB )
        {
            Destroy();
            return false;
        }
        hostA->FlushSync();
        hostB->FlushSync();
        return true;
    }

    void Destroy()
    {
        session.reset();
        if ( frameA )
            delete frameA;
        if ( frameB )
            delete frameB;
        frameA = nullptr;
        frameB = nullptr;
        panelA = nullptr;
        panelB = nullptr;
        owner = nullptr;
        buttonA = nullptr;
        buttonB = nullptr;
        hostA = nullptr;
        hostB = nullptr;
        DrainDispatch(5);
    }

    wxFrame *frameA = nullptr;
    wxFrame *frameB = nullptr;
    wxPanel *panelA = nullptr;
    wxPanel *panelB = nullptr;
    wxWinUIXamlHost *owner = nullptr;
    wxButton *buttonA = nullptr;
    wxButton *buttonB = nullptr;
    wxWinUITopLevelHost *hostA = nullptr;
    wxWinUITopLevelHost *hostB = nullptr;
    std::shared_ptr<CallbackLedger> ledger;
    std::unique_ptr<MediaPeerSession> session;
};

void CheckResourceLedger(const CallbackLedger& ledger);

void FinishFixture(TwoTLWMediaFixture& fixture,
                   const HostSnapshot& before)
{
    const std::shared_ptr<CallbackLedger> ledger = fixture.ledger;
    fixture.Destroy();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 750));
    before.CheckRestored();
    REQUIRE(ledger != nullptr);
    CheckResourceLedger(*ledger);
}

bool Nearly(std::int64_t actual,
            std::int64_t expected,
            std::int64_t tolerance)
{
    return std::llabs(actual - expected) <= tolerance;
}

void CheckDispatcherLedger(const CallbackLedger& ledger)
{
    CHECK(ledger.sourceCallbacks.load() > 0);
    CHECK(ledger.enqueued.load() == ledger.sourceCallbacks.load());
    CHECK(ledger.enqueueFailures.load() == 0);
    CHECK(ledger.callbackExceptions.load() == 0);
    CHECK(ledger.accepted.load() > 0);
}

void CheckResourceLedger(const CallbackLedger& ledger)
{
    CHECK(ledger.handlerAdds.load() > 0);
    CHECK(ledger.handlerAdds.load() % 9 == 0);
    CHECK(ledger.handlerAdds.load() == ledger.handlerRevokes.load());
    CHECK(ledger.sessionsInitialized.load() > 0);
    CHECK(ledger.sessionsInitialized.load() == ledger.sessionsClosed.load());
    CHECK(ledger.teardownStepAttempts.load() ==
          ledger.teardownStepSuccesses.load() +
              ledger.teardownStepFailures.load());
    CHECK(ledger.teardownStepFailures.load() == 0);
    CHECK(ledger.sourceCloseAttempts.load() ==
          ledger.sourceCloseSuccesses.load());
    CHECK(ledger.playerCloseAttempts.load() ==
          ledger.playerCloseSuccesses.load());
    CHECK(ledger.playerCloseAttempts.load() ==
          ledger.sessionsInitialized.load());
    CHECK(ledger.binderHandlerAdds.load() ==
          ledger.binderHandlerRevokes.load());
}

void DestroyAtPhase(const LocalMediaFiles& files, bool whilePlaying)
{
    const HostSnapshot before = HostSnapshot::Capture();
    const auto ledger = std::make_shared<CallbackLedger>();
    std::unique_ptr<wxFrame> frame(
        new wxFrame(nullptr, wxID_ANY, "media-retire",
                    wxDefaultPosition, wxSize(380, 240),
                    wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                        wxFRAME_TOOL_WINDOW));
    wxPanel * const panel = new wxPanel(frame.get());
    wxWinUIXamlHost * const owner =
        new wxWinUIXamlHost(panel, wxID_ANY,
                            wxPoint(10, 10), wxSize(300, 170));
    wxButton * const button = new wxButton(
        panel, wxID_ANY, "retire-neighbour",
        wxPoint(315, 30), wxSize(50, 40));
    wxUnusedVar(button);
    std::unique_ptr<MediaPeerSession> session(new MediaPeerSession);
    REQUIRE(session->Initialize(owner, ledger));
    REQUIRE(ShowOffscreenWithoutActivation(frame.get(), 2));
    REQUIRE(DrainToQuiescence());
    std::shared_ptr<NativeOpenGate> nativeOpenGate;
    if ( whilePlaying )
    {
        REQUIRE(session->LoadUrl(files.AviUrl()));
    }
    else
    {
        // MediaBinder::Binding plus its deferral is a real native resolution
        // boundary. Unlike an unchanged openedLoad counter, observing this
        // entry proves that the media pipeline has begun opening and is still
        // held there when Close() starts.
        nativeOpenGate =
            std::make_shared<NativeOpenGate>(files.WaveUrl());
        REQUIRE(session->LoadUrl(files.WaveUrl(), nativeOpenGate));
        REQUIRE(DrainUntil(
            [nativeOpenGate]()
            {
                return nativeOpenGate->Entries() != 0;
            }, MediaWaitRounds));
        CHECK(nativeOpenGate->CompletionAttempts() == 0);
        CHECK(nativeOpenGate->CompletionSuccesses() == 0);
    }

    if ( whilePlaying )
    {
        REQUIRE(session->WaitOpened());
        REQUIRE(session->Play());
        const std::shared_ptr<MediaState> state = session->State();
        REQUIRE(DrainUntil(
            [state]()
            {
                return state->logicalState == LogicalMediaState::Playing;
            }, StateWaitRounds));
    }
    else
    {
        const std::shared_ptr<MediaState> openingState = session->State();
        REQUIRE(openingState != nullptr);
        REQUIRE(nativeOpenGate != nullptr);
        CHECK(nativeOpenGate->Entries() > 0);
        CHECK(openingState->openedLoad != session->LoadGeneration());
        CHECK(openingState->failedLoad != session->LoadGeneration());
        CHECK(!session->IsOpened());
    }

    const std::weak_ptr<MediaState> weakState = session->WeakState();
    const std::weak_ptr<NativeOperationTracker> weakOperations =
        session->WeakOperationTrackerForTest();
    std::shared_ptr<MediaState> retiredState = session->State();
    REQUIRE(retiredState != nullptr);
    CHECK(session->ActiveHandlerCount() == 9);
    const unsigned acceptedAtClose = ledger->accepted.load();
    const unsigned staleLifetimeBefore = ledger->staleLifetime.load();
    session->PostGenerationProbeForTest(session->LoadGeneration());
    session->Close();
    session.reset();
    frame.reset();

    if ( nativeOpenGate )
    {
        CHECK(nativeOpenGate->CompletionAttempts() ==
              nativeOpenGate->Entries());
        CHECK(nativeOpenGate->CompletionSuccesses() ==
              nativeOpenGate->Entries());
        CHECK(ledger->binderHandlerAdds.load() == 1);
        CHECK(ledger->binderHandlerRevokes.load() == 1);
    }

    REQUIRE(DrainUntil(
        [ledger, staleLifetimeBefore]()
        {
            return ledger->staleLifetime.load() > staleLifetimeBefore;
        }, 250));
    retiredState.reset();
    REQUIRE(DrainUntil([&weakState]() { return weakState.expired(); }, 250));
    REQUIRE(DrainUntil(
        [&weakOperations]() { return weakOperations.expired(); }, 250));
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 750));
    CHECK(ledger->accepted.load() == acceptedAtClose);
    CHECK(ledger->expiredState.load() + ledger->staleLifetime.load() > 0);
    CHECK(ledger->enqueueFailures.load() == 0);
    CHECK(ledger->callbackExceptions.load() == 0);
    CheckResourceLedger(*ledger);
    before.CheckRestored();
}

} // namespace

TEST_CASE("WinUIMedia::OfflineWaveAviPlaybackEventsAndRecovery",
          "[winui-media][winui-015][media-player-element][playback]"
          "[events][lifetime]")
{
    LocalMediaFiles files;
    REQUIRE(files.Create());
    REQUIRE(files.WaveUrl().StartsWith("file:///"));
    REQUIRE(files.AviUrl().StartsWith("file:///"));
    REQUIRE(files.InvalidUrl().StartsWith("file:///"));
    const HostSnapshot before = HostSnapshot::Capture();
    TwoTLWMediaFixture fixture;
    REQUIRE(fixture.Create());
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts + 2);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == before.slots + 3);

    MediaPeerSession& media = *fixture.session;
    REQUIRE(media.LoadUrl(files.WaveUrl()));
    REQUIRE(media.WaitOpened());
    CHECK(media.ActiveHandlerCount() == 9);
    std::shared_ptr<MediaState> state = media.State();
    REQUIRE(state != nullptr);
    CHECK(state->failedLoad != media.LoadGeneration());
    REQUIRE(DrainUntil(
        [&media, state]()
        {
            return state->durationChanges > 0 &&
                   state->videoWidth == 0 && state->videoHeight == 0 &&
                   Nearly(media.DurationMilliseconds(), 3000, 250);
        }, StateWaitRounds));
    CHECK(media.IsMuted());

    const unsigned volumeChanges = state->volumeChanges;
    REQUIRE(media.SetVolume(0.375));
    REQUIRE(DrainUntil(
        [state, volumeChanges]()
        {
            return state->volumeChanges > volumeChanges;
        }, StateWaitRounds));
    CHECK(std::abs(media.Volume() - 0.375) < 0.001);
    CHECK(media.IsMuted());

    const unsigned rateChanges = state->playbackRateChanges;
    REQUIRE(media.SetPlaybackRate(1.25));
    REQUIRE(DrainUntil(
        [state, rateChanges]()
        {
            return state->playbackRateChanges > rateChanges;
        }, StateWaitRounds));
    CHECK(std::abs(media.PlaybackRate() - 1.25) < 0.001);

    const unsigned seeks = state->seekCompleted;
    REQUIRE(media.SeekMilliseconds(700));
    REQUIRE(DrainUntil(
        [state, seeks]() { return state->seekCompleted == seeks + 1; },
        StateWaitRounds));
    DrainDispatch(2);
    CHECK(state->seekCompleted == seeks + 1);
    CHECK(Nearly(media.PositionMilliseconds(), 700, 250));

    const unsigned playEvents = state->logicalPlayEvents;
    REQUIRE(media.Play());
    const std::uint64_t issuedPlayEpoch = state->playEpoch;
    const std::uint64_t issuedPlayOperation = state->playOperationId;
    const std::uint64_t issuedPlayBarrier = state->playAfterSequence;
    REQUIRE(issuedPlayEpoch != 0);
    REQUIRE(issuedPlayOperation != 0);
    REQUIRE(DrainUntil(
        [state, playEvents]()
        {
            return state->logicalState == LogicalMediaState::Playing &&
                   state->logicalPlayEvents == playEvents + 1;
        }, StateWaitRounds));
    DrainDispatch(2);
    CHECK(state->logicalPlayEvents == playEvents + 1);
    CHECK(state->lastCompletedPlayEpoch == issuedPlayEpoch);
    CHECK(state->lastCompletedPlayOperationId == issuedPlayOperation);
    CHECK(state->lastCompletedPlaySequence > issuedPlayBarrier);
    const std::uint64_t epochBeforeIdempotentPlay = state->commandEpoch;
    const unsigned eventsBeforeIdempotentPlay = state->logicalPlayEvents;
    const unsigned statesBeforeIdempotentPlay = state->playbackStateChanges;
    REQUIRE(media.Play());
    DrainDispatch(2);
    CHECK(state->commandEpoch == epochBeforeIdempotentPlay);
    CHECK(state->logicalPlayEvents == eventsBeforeIdempotentPlay);
    CHECK(state->playbackStateChanges == statesBeforeIdempotentPlay);
    REQUIRE(media.OperationTrackerForTest() != nullptr);
    CHECK(!media.OperationTrackerForTest()->HasPending());
    const unsigned pauseEvents = state->logicalPauseEvents;
    REQUIRE(media.Pause());
    const std::uint64_t issuedPauseOperation = state->pauseOperationId;
    REQUIRE(issuedPauseOperation != 0);
    REQUIRE(DrainUntil(
        [state, pauseEvents]()
        {
            return state->nativeState ==
                       WMP::MediaPlaybackState::Paused &&
                   state->logicalState == LogicalMediaState::Paused &&
                   state->logicalPauseEvents == pauseEvents + 1;
        }, StateWaitRounds));
    DrainDispatch(2);
    CHECK(state->logicalPauseEvents == pauseEvents + 1);
    CHECK(state->lastCompletedPauseOperationId == issuedPauseOperation);
    const std::uint64_t epochBeforeIdempotentPause = state->commandEpoch;
    const unsigned eventsBeforeIdempotentPause = state->logicalPauseEvents;
    const unsigned statesBeforeIdempotentPause = state->playbackStateChanges;
    REQUIRE(media.Pause());
    DrainDispatch(2);
    CHECK(state->commandEpoch == epochBeforeIdempotentPause);
    CHECK(state->logicalPauseEvents == eventsBeforeIdempotentPause);
    CHECK(state->playbackStateChanges == statesBeforeIdempotentPause);
    REQUIRE(media.OperationTrackerForTest() != nullptr);
    CHECK(!media.OperationTrackerForTest()->HasPending());

    // P2: Stop issued from an already-Paused native state reserves only its
    // StopSeek leg. The synchronous Paused sample is credited to this epoch,
    // while logical Stop still waits for the exact seek completion.
    const unsigned pausedStopEvents = state->logicalStopEvents;
    const unsigned pausedStopSeeks = state->seekCompleted;
    REQUIRE(media.Stop());
    const std::uint64_t pausedStopEpoch = state->stopEpoch;
    const std::uint64_t pausedStopBarrier =
        state->stopPauseAfterSequence;
    const std::uint64_t pausedStopSeekOperation =
        state->stopSeekOperationId;
    REQUIRE(pausedStopEpoch != 0);
    CHECK(state->stopPauseOperationId == 0);
    REQUIRE(pausedStopSeekOperation != 0);
    CHECK(state->stopPauseObservedEpoch == pausedStopEpoch);
    CHECK(state->stopPauseSequence == pausedStopBarrier);
    CHECK(state->logicalStopEvents == pausedStopEvents);
    REQUIRE(media.WaitStoppedAfter(pausedStopEvents, pausedStopSeeks));
    DrainDispatch(2);
    CHECK(state->logicalStopEvents == pausedStopEvents + 1);
    CHECK(state->lastCompletedStopEpoch == pausedStopEpoch);
    CHECK(state->lastCompletedStopPauseOperationId == 0);
    CHECK(state->lastCompletedStopSeekOperationId ==
          pausedStopSeekOperation);
    CHECK(state->lastCompletedStopPauseSequence == pausedStopBarrier);

    // Interleaving (b): if an older native operation has not emitted its
    // completion yet, Stop cannot reserve its pair of internal intents. The
    // failed reservation changes neither MediaState nor the output tickets.
    MediaState pendingStopOracle;
    pendingStopOracle.commandEpoch = 7;
    pendingStopOracle.logicalState = LogicalMediaState::Playing;
    NativeOperationTracker pendingSeekTracker;
    NativeEventCorrelation pendingSeekCorrelation;
    pendingSeekCorrelation.sourceSequence.store(
        10, std::memory_order_release);
    NativeOperationTicket pendingPublicSeek;
    REQUIRE(pendingSeekTracker.TryBeginPublicSeek(
        pendingStopOracle.commandEpoch,
        pendingSeekCorrelation,
        CaptureEntrySequence(pendingSeekCorrelation),
        &pendingPublicSeek) == NativeOperationStartResult::Started);
    NativeOperationTicket rejectedPauseTicket = pendingPublicSeek;
    NativeOperationTicket rejectedSeekTicket = pendingPublicSeek;
    CHECK(pendingSeekTracker.TryBeginStop(
        NextCommandEpoch(pendingStopOracle),
        pendingSeekCorrelation,
        CaptureEntrySequence(pendingSeekCorrelation), true,
        &rejectedPauseTicket, &rejectedSeekTicket) ==
        NativeOperationStartResult::Busy);
    CHECK(pendingStopOracle.commandEpoch == 7);
    CHECK(pendingStopOracle.stopEpoch == 0);
    CHECK(pendingStopOracle.logicalState == LogicalMediaState::Playing);
    CHECK(!rejectedPauseTicket.IsValid());
    CHECK(!rejectedSeekTicket.IsValid());
    CHECK(pendingSeekTracker.HasPending());
    const NativeOperationTicket retiredPublicSeek =
        pendingSeekTracker.CompleteSeekAtSource(11);
    CHECK(retiredPublicSeek.kind == NativeOperationKind::PublicSeek);
    CHECK(retiredPublicSeek.operationId == pendingPublicSeek.operationId);
    CHECK(!pendingSeekTracker.HasPending());

    NativeOperationTracker pendingPauseTracker;
    NativeEventCorrelation pendingPauseCorrelation;
    pendingPauseCorrelation.sourceSequence.store(
        20, std::memory_order_release);
    NativeOperationTicket pendingPublicPause;
    REQUIRE(pendingPauseTracker.TryBeginPublicPause(
        pendingStopOracle.commandEpoch,
        pendingPauseCorrelation,
        CaptureEntrySequence(pendingPauseCorrelation),
        &pendingPublicPause) == NativeOperationStartResult::Started);
    rejectedPauseTicket = pendingPublicPause;
    rejectedSeekTicket = pendingPublicPause;
    CHECK(pendingPauseTracker.TryBeginStop(
        NextCommandEpoch(pendingStopOracle),
        pendingPauseCorrelation,
        CaptureEntrySequence(pendingPauseCorrelation), true,
        &rejectedPauseTicket, &rejectedSeekTicket) ==
        NativeOperationStartResult::Busy);
    CHECK(pendingStopOracle.commandEpoch == 7);
    CHECK(pendingStopOracle.stopEpoch == 0);
    CHECK(pendingStopOracle.logicalState == LogicalMediaState::Playing);
    CHECK(!rejectedPauseTicket.IsValid());
    CHECK(!rejectedSeekTicket.IsValid());
    CHECK(pendingPauseTracker.HasPending());
    const NativeOperationTicket retiredPublicPause =
        pendingPauseTracker.CompletePauseAtSource(21);
    CHECK(retiredPublicPause.kind == NativeOperationKind::PublicPause);
    CHECK(retiredPublicPause.operationId == pendingPublicPause.operationId);
    CHECK(!pendingPauseTracker.HasPending());

    // A Play operation is pending until its Playing source event consumes the
    // exact ticket. Both Pause and Stop must reject during that interval and
    // leave the active Play intent and MediaState completely unchanged.
    MediaState pendingPlayOracle;
    pendingPlayOracle.commandEpoch = 30;
    pendingPlayOracle.nativeState = WMP::MediaPlaybackState::Paused;
    pendingPlayOracle.logicalState = LogicalMediaState::Paused;
    NativeOperationTracker pendingPlayTracker;
    NativeEventCorrelation pendingPlayCorrelation;
    pendingPlayCorrelation.sourceSequence.store(
        400, std::memory_order_release);
    NativeOperationTicket pendingPublicPlay;
    const std::uint64_t pendingPlayEpoch =
        NextCommandEpoch(pendingPlayOracle);
    REQUIRE(pendingPlayTracker.TryBeginPublicPlay(
        pendingPlayEpoch, pendingPlayCorrelation,
        CaptureEntrySequence(pendingPlayCorrelation),
        &pendingPublicPlay) == NativeOperationStartResult::Started);
    BeginPlayIntent(pendingPlayOracle, pendingPlayEpoch,
                    pendingPublicPlay);
    NativeOperationTicket rejectedByPlay = pendingPublicPlay;
    CHECK(pendingPlayTracker.TryBeginPublicPause(
        NextCommandEpoch(pendingPlayOracle),
        pendingPlayCorrelation,
        CaptureEntrySequence(pendingPlayCorrelation),
        &rejectedByPlay) == NativeOperationStartResult::Busy);
    CHECK(!rejectedByPlay.IsValid());
    NativeOperationTicket playBlockedStopPause = pendingPublicPlay;
    NativeOperationTicket playBlockedStopSeek = pendingPublicPlay;
    CHECK(pendingPlayTracker.TryBeginStop(
        NextCommandEpoch(pendingPlayOracle),
        pendingPlayCorrelation,
        CaptureEntrySequence(pendingPlayCorrelation), false,
        &playBlockedStopPause, &playBlockedStopSeek) ==
        NativeOperationStartResult::Busy);
    CHECK(!playBlockedStopPause.IsValid());
    CHECK(!playBlockedStopSeek.IsValid());
    CHECK(pendingPlayOracle.commandEpoch == pendingPlayEpoch);
    CHECK(pendingPlayOracle.playEpoch == pendingPlayEpoch);
    CHECK(pendingPlayOracle.playOperationId ==
          pendingPublicPlay.operationId);
    CHECK(pendingPlayOracle.pauseEpoch == 0);
    CHECK(pendingPlayOracle.stopEpoch == 0);
    CHECK(pendingPlayOracle.nativeState ==
          WMP::MediaPlaybackState::Paused);
    CHECK(pendingPlayOracle.logicalState == LogicalMediaState::Paused);
    CHECK(pendingPlayTracker.HasPending());
    const NativeOperationTicket completedPendingPlay =
        pendingPlayTracker.CompletePlayAtSource(401);
    CHECK(completedPendingPlay.operationId ==
          pendingPublicPlay.operationId);
    ObservePlaybackStateChanged(
        pendingPlayOracle, WMP::MediaPlaybackState::Playing, 401,
        completedPendingPlay);
    CHECK(pendingPlayOracle.logicalPlayEvents == 1);
    CHECK(pendingPlayOracle.lastCompletedPlayOperationId ==
          pendingPublicPlay.operationId);
    CHECK(!pendingPlayTracker.HasPending());

    // Once the Playing event consumes its ticket at source, the Play reducer
    // intent still blocks every command, including a repeated idempotent Play,
    // until that exact UI reducer has run.
    MediaState latePlayOracle;
    latePlayOracle.commandEpoch = 50;
    latePlayOracle.nativeState = WMP::MediaPlaybackState::Paused;
    latePlayOracle.logicalState = LogicalMediaState::Paused;
    NativeOperationTracker latePlayTracker;
    NativeEventCorrelation latePlayCorrelation;
    latePlayCorrelation.sourceSequence.store(
        500, std::memory_order_release);
    NativeOperationTicket latePublicPlay;
    const std::uint64_t latePlayEpoch = NextCommandEpoch(latePlayOracle);
    REQUIRE(latePlayTracker.TryBeginPublicPlay(
        latePlayEpoch, latePlayCorrelation,
        CaptureEntrySequence(latePlayCorrelation),
        &latePublicPlay) == NativeOperationStartResult::Started);
    BeginPlayIntent(latePlayOracle, latePlayEpoch, latePublicPlay);
    const NativeOperationTicket latePlayAtSource =
        latePlayTracker.CompletePlayAtSource(501);
    CHECK(!latePlayTracker.HasPending());
    CHECK(HasActiveReducerIntent(latePlayOracle));
    CHECK(PlaybackCommandIsBlocked(latePlayOracle, latePlayTracker));
    CHECK(latePlayOracle.commandEpoch == latePlayEpoch);
    CHECK(latePlayOracle.logicalState == LogicalMediaState::Paused);
    CHECK(latePlayOracle.logicalPauseEvents == 0);
    ObservePlaybackStateChanged(
        latePlayOracle, WMP::MediaPlaybackState::Playing, 501,
        latePlayAtSource);
    CHECK(!HasActiveReducerIntent(latePlayOracle));
    CHECK(!PlaybackCommandIsBlocked(latePlayOracle, latePlayTracker));
    CHECK(latePlayOracle.logicalPlayEvents == 1);

    latePlayCorrelation.sourceSequence.store(
        501, std::memory_order_release);
    const std::uint64_t newerPauseEpoch = NextCommandEpoch(latePlayOracle);
    NativeOperationTicket newerPause;
    REQUIRE(latePlayTracker.TryBeginPublicPause(
        newerPauseEpoch, latePlayCorrelation,
        CaptureEntrySequence(latePlayCorrelation),
        &newerPause) == NativeOperationStartResult::Started);
    BeginPauseIntent(latePlayOracle, newerPauseEpoch, newerPause);
    const NativeOperationTicket newerPauseAtSource =
        latePlayTracker.CompletePauseAtSource(502);
    ObservePlaybackStateChanged(
        latePlayOracle, WMP::MediaPlaybackState::Paused, 502,
        newerPauseAtSource);
    REQUIRE(latePlayOracle.logicalState == LogicalMediaState::Paused);
    CHECK(latePlayOracle.logicalPauseEvents == 1);
    CHECK(latePlayOracle.lastCompletedPauseOperationId ==
          newerPause.operationId);

    // The same fail-closed interval exists for both legs of Stop. Even after
    // both source callbacks consumed their tickets, Pause must not start or
    // clear the Stop epoch until the queued reducers complete the join.
    MediaState queuedStopOracle;
    queuedStopOracle.commandEpoch = 60;
    queuedStopOracle.nativeState = WMP::MediaPlaybackState::Playing;
    queuedStopOracle.logicalState = LogicalMediaState::Playing;
    NativeOperationTracker queuedStopTracker;
    NativeEventCorrelation queuedStopCorrelation;
    queuedStopCorrelation.sourceSequence.store(
        600, std::memory_order_release);
    NativeOperationTicket queuedStopPause;
    NativeOperationTicket queuedStopSeek;
    const std::uint64_t queuedStopEpoch =
        NextCommandEpoch(queuedStopOracle);
    REQUIRE(queuedStopTracker.TryBeginStop(
        queuedStopEpoch, queuedStopCorrelation,
        CaptureEntrySequence(queuedStopCorrelation), true,
        &queuedStopPause, &queuedStopSeek) ==
        NativeOperationStartResult::Started);
    BeginStopIntent(queuedStopOracle, queuedStopEpoch, false,
                    queuedStopPause, queuedStopSeek);
    const NativeOperationTicket queuedStopPauseAtSource =
        queuedStopTracker.CompletePauseAtSource(601);
    const NativeOperationTicket queuedStopSeekAtSource =
        queuedStopTracker.CompleteSeekAtSource(602);
    CHECK(!queuedStopTracker.HasPending());
    CHECK(HasActiveReducerIntent(queuedStopOracle));
    CHECK(PlaybackCommandIsBlocked(queuedStopOracle, queuedStopTracker));
    CHECK(queuedStopOracle.commandEpoch == queuedStopEpoch);
    CHECK(queuedStopOracle.logicalState == LogicalMediaState::Playing);
    CHECK(queuedStopOracle.logicalPauseEvents == 0);
    CHECK(queuedStopOracle.logicalStopEvents == 0);
    ObservePlaybackStateChanged(
        queuedStopOracle, WMP::MediaPlaybackState::Paused, 601,
        queuedStopPauseAtSource);
    CHECK(PlaybackCommandIsBlocked(queuedStopOracle, queuedStopTracker));
    ObserveSeekCompleted(
        queuedStopOracle, 602, queuedStopSeekAtSource);
    CHECK(!HasActiveReducerIntent(queuedStopOracle));
    CHECK(!PlaybackCommandIsBlocked(queuedStopOracle, queuedStopTracker));
    CHECK(queuedStopOracle.logicalStopEvents == 1);
    CHECK(queuedStopOracle.logicalPauseEvents == 0);
    CHECK(queuedStopOracle.logicalState == LogicalMediaState::Stopped);

    // A source delegate may enter and stamp its sequence before a reservation,
    // then resume inside Complete* afterwards. Sequence <= reservationBarrier
    // must leave every newly reserved slot intact; the causally later native
    // completion consumes the exact PublicPlay/PublicPause/PublicSeek or Stop
    // ticket instead.
    const std::shared_ptr<NativeEventCorrelation> enteredCorrelation =
        std::make_shared<NativeEventCorrelation>();
    enteredCorrelation->sourceSequence.store(
        699, std::memory_order_release);

    NativeOperationTracker enteredPlayTracker;
    NativeOperationTicket enteredPlayTicket;
    const std::uint64_t enteredPlaySequence =
        StampNativeEvent(enteredCorrelation);
    REQUIRE(enteredPlayTracker.TryBeginPublicPlay(
        70, *enteredCorrelation, enteredPlaySequence,
        &enteredPlayTicket) == NativeOperationStartResult::Started);
    CHECK(enteredPlayTicket.reservationBarrier == enteredPlaySequence);
    CHECK(!enteredPlayTracker.CompletePlayAtSource(
        enteredPlaySequence).IsValid());
    CHECK(enteredPlayTracker.HasPending());
    const std::uint64_t completedPlaySequence =
        StampNativeEvent(enteredCorrelation);
    const NativeOperationTicket completedEnteredPlay =
        enteredPlayTracker.CompletePlayAtSource(completedPlaySequence);
    CHECK(completedEnteredPlay.operationId == enteredPlayTicket.operationId);
    CHECK(completedEnteredPlay.commandEpoch == 70);
    CHECK(completedEnteredPlay.reservationBarrier == enteredPlaySequence);

    NativeOperationTracker enteredPauseTracker;
    NativeOperationTicket enteredPauseTicket;
    const std::uint64_t enteredPauseSequence =
        StampNativeEvent(enteredCorrelation);
    REQUIRE(enteredPauseTracker.TryBeginPublicPause(
        71, *enteredCorrelation, enteredPauseSequence,
        &enteredPauseTicket) == NativeOperationStartResult::Started);
    CHECK(enteredPauseTicket.reservationBarrier == enteredPauseSequence);
    CHECK(!enteredPauseTracker.CompletePauseAtSource(
        enteredPauseSequence).IsValid());
    CHECK(enteredPauseTracker.HasPending());
    const std::uint64_t completedPauseSequence =
        StampNativeEvent(enteredCorrelation);
    const NativeOperationTicket completedEnteredPause =
        enteredPauseTracker.CompletePauseAtSource(completedPauseSequence);
    CHECK(completedEnteredPause.operationId ==
          enteredPauseTicket.operationId);
    CHECK(completedEnteredPause.commandEpoch == 71);
    CHECK(completedEnteredPause.reservationBarrier == enteredPauseSequence);

    NativeOperationTracker enteredSeekTracker;
    NativeOperationTicket enteredSeekTicket;
    const std::uint64_t enteredSeekSequence =
        StampNativeEvent(enteredCorrelation);
    REQUIRE(enteredSeekTracker.TryBeginPublicSeek(
        72, *enteredCorrelation, enteredSeekSequence,
        &enteredSeekTicket) == NativeOperationStartResult::Started);
    CHECK(enteredSeekTicket.reservationBarrier == enteredSeekSequence);
    CHECK(!enteredSeekTracker.CompleteSeekAtSource(
        enteredSeekSequence).IsValid());
    CHECK(enteredSeekTracker.HasPending());
    const std::uint64_t completedSeekSequence =
        StampNativeEvent(enteredCorrelation);
    const NativeOperationTicket completedEnteredSeek =
        enteredSeekTracker.CompleteSeekAtSource(completedSeekSequence);
    CHECK(completedEnteredSeek.operationId == enteredSeekTicket.operationId);
    CHECK(completedEnteredSeek.commandEpoch == 72);
    CHECK(completedEnteredSeek.reservationBarrier == enteredSeekSequence);

    NativeOperationTracker enteredStopTracker;
    NativeOperationTicket enteredStopPause;
    NativeOperationTicket enteredStopSeek;
    const std::uint64_t enteredStopSequence =
        StampNativeEvent(enteredCorrelation);
    REQUIRE(enteredStopTracker.TryBeginStop(
        73, *enteredCorrelation, enteredStopSequence, true,
        &enteredStopPause, &enteredStopSeek) ==
        NativeOperationStartResult::Started);
    CHECK(enteredStopPause.reservationBarrier == enteredStopSequence);
    CHECK(enteredStopSeek.reservationBarrier == enteredStopSequence);
    CHECK(!enteredStopTracker.CompletePauseAtSource(
        enteredStopSequence).IsValid());
    CHECK(!enteredStopTracker.CompleteSeekAtSource(
        enteredStopSequence).IsValid());
    CHECK(enteredStopTracker.HasPending());
    const std::uint64_t completedStopPauseSequence =
        StampNativeEvent(enteredCorrelation);
    const NativeOperationTicket completedEnteredStopPause =
        enteredStopTracker.CompletePauseAtSource(
            completedStopPauseSequence);
    const std::uint64_t completedStopSeekSequence =
        StampNativeEvent(enteredCorrelation);
    const NativeOperationTicket completedEnteredStopSeek =
        enteredStopTracker.CompleteSeekAtSource(completedStopSeekSequence);
    CHECK(completedEnteredStopPause.operationId ==
          enteredStopPause.operationId);
    CHECK(completedEnteredStopSeek.operationId ==
          enteredStopSeek.operationId);
    CHECK(completedEnteredStopPause.reservationBarrier ==
          enteredStopSequence);
    CHECK(completedEnteredStopSeek.reservationBarrier ==
          enteredStopSequence);
    CHECK(!enteredStopTracker.HasPending());

    // Deterministic state-snapshot TOCTOU hooks. A source entry between the
    // snapshot and reservation invalidates both idempotent validation and
    // ticket publication; a retry from the new sequence converges.
    const std::shared_ptr<NativeEventCorrelation> playSnapshotCorrelation =
        std::make_shared<NativeEventCorrelation>();
    NativeOperationTracker playSnapshotTracker;
    MediaState playSnapshotState;
    playSnapshotState.commandEpoch = 80;
    playSnapshotState.logicalState = LogicalMediaState::Paused;
    const std::uint64_t stalePlaySnapshot =
        CaptureEntrySequence(*playSnapshotCorrelation);
    StampNativeEvent(playSnapshotCorrelation);
    CHECK(!EntrySequenceMatches(
        *playSnapshotCorrelation, stalePlaySnapshot));
    NativeOperationTicket snapshotPlayTicket;
    const std::uint64_t snapshotPlayEpoch =
        NextCommandEpoch(playSnapshotState);
    CHECK(playSnapshotTracker.TryBeginPublicPlay(
        snapshotPlayEpoch, *playSnapshotCorrelation, stalePlaySnapshot,
        &snapshotPlayTicket) == NativeOperationStartResult::StaleSnapshot);
    CHECK(!snapshotPlayTicket.IsValid());
    CHECK(!playSnapshotTracker.HasPending());
    CHECK(playSnapshotState.playEpoch == 0);
    const std::uint64_t retriedPlaySnapshot =
        CaptureEntrySequence(*playSnapshotCorrelation);
    REQUIRE(playSnapshotTracker.TryBeginPublicPlay(
        snapshotPlayEpoch, *playSnapshotCorrelation, retriedPlaySnapshot,
        &snapshotPlayTicket) == NativeOperationStartResult::Started);
    BeginPlayIntent(
        playSnapshotState, snapshotPlayEpoch, snapshotPlayTicket);
    const std::uint64_t snapshotPlayCompletion =
        StampNativeEvent(playSnapshotCorrelation);
    ObservePlaybackStateChanged(
        playSnapshotState, WMP::MediaPlaybackState::Playing,
        snapshotPlayCompletion,
        playSnapshotTracker.CompletePlayAtSource(snapshotPlayCompletion));
    CHECK(playSnapshotState.playEpoch == 0);
    CHECK(playSnapshotState.logicalPlayEvents == 1);

    const std::shared_ptr<NativeEventCorrelation> pauseSnapshotCorrelation =
        std::make_shared<NativeEventCorrelation>();
    NativeOperationTracker pauseSnapshotTracker;
    MediaState pauseSnapshotState;
    pauseSnapshotState.commandEpoch = 90;
    pauseSnapshotState.logicalState = LogicalMediaState::Playing;
    const std::uint64_t stalePauseSnapshot =
        CaptureEntrySequence(*pauseSnapshotCorrelation);
    StampNativeEvent(pauseSnapshotCorrelation);
    CHECK(!EntrySequenceMatches(
        *pauseSnapshotCorrelation, stalePauseSnapshot));
    NativeOperationTicket snapshotPauseTicket;
    const std::uint64_t snapshotPauseEpoch =
        NextCommandEpoch(pauseSnapshotState);
    CHECK(pauseSnapshotTracker.TryBeginPublicPause(
        snapshotPauseEpoch, *pauseSnapshotCorrelation, stalePauseSnapshot,
        &snapshotPauseTicket) == NativeOperationStartResult::StaleSnapshot);
    CHECK(!snapshotPauseTicket.IsValid());
    CHECK(!pauseSnapshotTracker.HasPending());
    CHECK(pauseSnapshotState.pauseEpoch == 0);
    const std::uint64_t retriedPauseSnapshot =
        CaptureEntrySequence(*pauseSnapshotCorrelation);
    REQUIRE(pauseSnapshotTracker.TryBeginPublicPause(
        snapshotPauseEpoch, *pauseSnapshotCorrelation,
        retriedPauseSnapshot, &snapshotPauseTicket) ==
        NativeOperationStartResult::Started);
    BeginPauseIntent(
        pauseSnapshotState, snapshotPauseEpoch, snapshotPauseTicket);
    const std::uint64_t snapshotPauseCompletion =
        StampNativeEvent(pauseSnapshotCorrelation);
    ObservePlaybackStateChanged(
        pauseSnapshotState, WMP::MediaPlaybackState::Paused,
        snapshotPauseCompletion,
        pauseSnapshotTracker.CompletePauseAtSource(
            snapshotPauseCompletion));
    CHECK(pauseSnapshotState.pauseEpoch == 0);
    CHECK(pauseSnapshotState.logicalPauseEvents == 1);

    // Playing -> Paused during the Stop snapshot must discard the original
    // two-leg choice; retry reserves StopSeek only from the validated state.
    const std::shared_ptr<NativeEventCorrelation> stopToPausedCorrelation =
        std::make_shared<NativeEventCorrelation>();
    NativeOperationTracker stopToPausedTracker;
    MediaState stopToPausedState;
    stopToPausedState.commandEpoch = 100;
    stopToPausedState.logicalState = LogicalMediaState::Playing;
    const std::uint64_t stalePlayingStopSnapshot =
        CaptureEntrySequence(*stopToPausedCorrelation);
    StampNativeEvent(stopToPausedCorrelation);
    NativeOperationTicket stopToPausedPause;
    NativeOperationTicket stopToPausedSeek;
    const std::uint64_t stopToPausedEpoch =
        NextCommandEpoch(stopToPausedState);
    CHECK(stopToPausedTracker.TryBeginStop(
        stopToPausedEpoch, *stopToPausedCorrelation,
        stalePlayingStopSnapshot, true,
        &stopToPausedPause, &stopToPausedSeek) ==
        NativeOperationStartResult::StaleSnapshot);
    CHECK(!stopToPausedPause.IsValid());
    CHECK(!stopToPausedSeek.IsValid());
    CHECK(!stopToPausedTracker.HasPending());
    CHECK(stopToPausedState.stopEpoch == 0);
    const std::uint64_t retriedPausedStopSnapshot =
        CaptureEntrySequence(*stopToPausedCorrelation);
    REQUIRE(stopToPausedTracker.TryBeginStop(
        stopToPausedEpoch, *stopToPausedCorrelation,
        retriedPausedStopSnapshot, false,
        &stopToPausedPause, &stopToPausedSeek) ==
        NativeOperationStartResult::Started);
    CHECK(!stopToPausedPause.IsValid());
    REQUIRE(stopToPausedSeek.IsValid());
    BeginStopIntent(stopToPausedState, stopToPausedEpoch, true,
                    stopToPausedPause, stopToPausedSeek);
    const std::uint64_t stopToPausedCompletion =
        StampNativeEvent(stopToPausedCorrelation);
    ObserveSeekCompleted(
        stopToPausedState, stopToPausedCompletion,
        stopToPausedTracker.CompleteSeekAtSource(stopToPausedCompletion));
    CHECK(stopToPausedState.logicalStopEvents == 1);
    CHECK(stopToPausedState.lastCompletedStopPauseOperationId == 0);

    // Paused -> Playing is the symmetric danger: the stale one-leg choice is
    // rejected, and retry must reserve both StopPause and StopSeek.
    const std::shared_ptr<NativeEventCorrelation> stopToPlayingCorrelation =
        std::make_shared<NativeEventCorrelation>();
    NativeOperationTracker stopToPlayingTracker;
    MediaState stopToPlayingState;
    stopToPlayingState.commandEpoch = 110;
    stopToPlayingState.logicalState = LogicalMediaState::Paused;
    const std::uint64_t stalePausedStopSnapshot =
        CaptureEntrySequence(*stopToPlayingCorrelation);
    StampNativeEvent(stopToPlayingCorrelation);
    NativeOperationTicket stopToPlayingPause;
    NativeOperationTicket stopToPlayingSeek;
    const std::uint64_t stopToPlayingEpoch =
        NextCommandEpoch(stopToPlayingState);
    CHECK(stopToPlayingTracker.TryBeginStop(
        stopToPlayingEpoch, *stopToPlayingCorrelation,
        stalePausedStopSnapshot, false,
        &stopToPlayingPause, &stopToPlayingSeek) ==
        NativeOperationStartResult::StaleSnapshot);
    CHECK(!stopToPlayingPause.IsValid());
    CHECK(!stopToPlayingSeek.IsValid());
    CHECK(!stopToPlayingTracker.HasPending());
    CHECK(stopToPlayingState.stopEpoch == 0);
    const std::uint64_t retriedPlayingStopSnapshot =
        CaptureEntrySequence(*stopToPlayingCorrelation);
    REQUIRE(stopToPlayingTracker.TryBeginStop(
        stopToPlayingEpoch, *stopToPlayingCorrelation,
        retriedPlayingStopSnapshot, true,
        &stopToPlayingPause, &stopToPlayingSeek) ==
        NativeOperationStartResult::Started);
    REQUIRE(stopToPlayingPause.IsValid());
    REQUIRE(stopToPlayingSeek.IsValid());
    BeginStopIntent(stopToPlayingState, stopToPlayingEpoch, false,
                    stopToPlayingPause, stopToPlayingSeek);
    const std::uint64_t stopToPlayingPauseCompletion =
        StampNativeEvent(stopToPlayingCorrelation);
    const NativeOperationTicket completedStopToPlayingPause =
        stopToPlayingTracker.CompletePauseAtSource(
            stopToPlayingPauseCompletion);
    const std::uint64_t stopToPlayingSeekCompletion =
        StampNativeEvent(stopToPlayingCorrelation);
    const NativeOperationTicket completedStopToPlayingSeek =
        stopToPlayingTracker.CompleteSeekAtSource(
            stopToPlayingSeekCompletion);
    ObservePlaybackStateChanged(
        stopToPlayingState, WMP::MediaPlaybackState::Paused,
        stopToPlayingPauseCompletion, completedStopToPlayingPause);
    ObserveSeekCompleted(
        stopToPlayingState, stopToPlayingSeekCompletion,
        completedStopToPlayingSeek);
    CHECK(stopToPlayingState.logicalStopEvents == 1);
    CHECK(stopToPlayingState.lastCompletedStopPauseOperationId ==
          stopToPlayingPause.operationId);

    // Continuous source entry cannot spin a public call forever: every one of
    // the same 16 attempts is stale, publishes no ticket or reducer intent,
    // and the caller then fails closed.
    const std::shared_ptr<NativeEventCorrelation> boundedCorrelation =
        std::make_shared<NativeEventCorrelation>();
    NativeOperationTracker boundedTracker;
    MediaState boundedState;
    NativeOperationTicket boundedTicket;
    unsigned staleAttempts = 0;
    for ( ; staleAttempts < NativeSnapshotRetryLimit; ++staleAttempts )
    {
        const std::uint64_t expected =
            CaptureEntrySequence(*boundedCorrelation);
        StampNativeEvent(boundedCorrelation);
        CHECK(boundedTracker.TryBeginPublicPlay(
            120, *boundedCorrelation, expected, &boundedTicket) ==
            NativeOperationStartResult::StaleSnapshot);
        CHECK(!boundedTicket.IsValid());
        CHECK(!boundedTracker.HasPending());
        CHECK(boundedState.playEpoch == 0);
    }
    CHECK(staleAttempts == NativeSnapshotRetryLimit);

    // Play is a state-changing command too. The actual session entry point
    // must reject it while an incompatible intent is pending, without
    // publishing a new command epoch or touching logical state.
    const std::shared_ptr<NativeOperationTracker> liveOperations =
        media.OperationTrackerForTest();
    REQUIRE(liveOperations != nullptr);
    NativeEventCorrelation liveBlockCorrelation;
    NativeOperationTicket blocksPlay;
    REQUIRE(liveOperations->TryBeginPublicSeek(
        state->commandEpoch, liveBlockCorrelation,
        CaptureEntrySequence(liveBlockCorrelation), &blocksPlay) ==
        NativeOperationStartResult::Started);
    const std::uint64_t epochBeforeRejectedPlay = state->commandEpoch;
    const LogicalMediaState stateBeforeRejectedPlay = state->logicalState;
    const unsigned playEventsBeforeRejectedPlay = state->logicalPlayEvents;
    CHECK(!media.Play());
    CHECK(state->commandEpoch == epochBeforeRejectedPlay);
    CHECK(state->logicalState == stateBeforeRejectedPlay);
    CHECK(state->logicalPlayEvents == playEventsBeforeRejectedPlay);
    const NativeOperationTicket releasedPlayBlock =
        liveOperations->CompleteSeekAtSource(1);
    CHECK(releasedPlayBlock.operationId == blocksPlay.operationId);
    CHECK(!liveOperations->HasPending());

    // If a newer Play has already completed after the old source callback
    // consumed its intent but before the old UI callback runs, the valid old
    // ticket is retired rather than allowed to overwrite Playing.
    MediaState playRetirementOracle;
    playRetirementOracle.commandEpoch = 8;
    playRetirementOracle.nativeState = WMP::MediaPlaybackState::Playing;
    playRetirementOracle.logicalState = LogicalMediaState::Playing;
    ObservePlaybackStateChanged(
        playRetirementOracle, WMP::MediaPlaybackState::Paused, 50,
        retiredPublicPause);
    CHECK(playRetirementOracle.nativeState ==
          WMP::MediaPlaybackState::Playing);
    CHECK(playRetirementOracle.logicalState == LogicalMediaState::Playing);
    CHECK(playRetirementOracle.stalePauseIntentEvents == 1);

    // Interleaving (a), seek leg: the old PublicSeek enters after Stop's
    // sequence sample but before reservation. The first reservation is stale
    // and publishes nothing; retry uses the new barrier, and the queued old
    // PublicSeek ticket still cannot satisfy StopSeek.
    MediaState stopSeekOracle;
    stopSeekOracle.commandEpoch = 10;
    stopSeekOracle.logicalState = LogicalMediaState::Playing;
    NativeOperationTracker stopSeekTracker;
    const std::shared_ptr<NativeEventCorrelation> stopSeekCorrelation =
        std::make_shared<NativeEventCorrelation>();
    NativeOperationTicket oldPublicSeek;
    REQUIRE(stopSeekTracker.TryBeginPublicSeek(
        stopSeekOracle.commandEpoch, *stopSeekCorrelation,
        CaptureEntrySequence(*stopSeekCorrelation), &oldPublicSeek) ==
        NativeOperationStartResult::Started);
    const std::uint64_t seekRaceBarrier = 100;
    stopSeekCorrelation->sourceSequence.store(
        seekRaceBarrier, std::memory_order_release);
    const std::uint64_t staleSeekStopSnapshot =
        CaptureEntrySequence(*stopSeekCorrelation);
    const std::uint64_t oldSeekSequence =
        StampNativeEvent(stopSeekCorrelation);
    const NativeOperationTicket oldSeekAtSource =
        stopSeekTracker.CompleteSeekAtSource(oldSeekSequence);
    const std::uint64_t seekStopEpoch = NextCommandEpoch(stopSeekOracle);
    NativeOperationTicket seekStopPause;
    NativeOperationTicket seekStopSeek;
    CHECK(stopSeekTracker.TryBeginStop(
        seekStopEpoch, *stopSeekCorrelation, staleSeekStopSnapshot, true,
        &seekStopPause, &seekStopSeek) ==
        NativeOperationStartResult::StaleSnapshot);
    CHECK(!seekStopPause.IsValid());
    CHECK(!seekStopSeek.IsValid());
    CHECK(stopSeekOracle.stopEpoch == 0);
    REQUIRE(stopSeekTracker.TryBeginStop(
        seekStopEpoch, *stopSeekCorrelation,
        CaptureEntrySequence(*stopSeekCorrelation), true,
        &seekStopPause, &seekStopSeek) ==
        NativeOperationStartResult::Started);
    BeginStopIntent(stopSeekOracle, seekStopEpoch, false,
                    seekStopPause, seekStopSeek);
    ObserveSeekCompleted(
        stopSeekOracle, oldSeekSequence, oldSeekAtSource);
    CHECK(stopSeekOracle.staleStopSeekEvents == 1);
    CHECK(stopSeekOracle.logicalStopEvents == 0);
    const std::uint64_t freshPauseSequence =
        StampNativeEvent(stopSeekCorrelation);
    const NativeOperationTicket freshPauseAtSource =
        stopSeekTracker.CompletePauseAtSource(freshPauseSequence);
    const std::uint64_t freshSeekSequence =
        StampNativeEvent(stopSeekCorrelation);
    const NativeOperationTicket freshSeekAtSource =
        stopSeekTracker.CompleteSeekAtSource(freshSeekSequence);
    ObserveSeekCompleted(
        stopSeekOracle, freshSeekSequence, freshSeekAtSource);
    CHECK(stopSeekOracle.logicalStopEvents == 0);
    ObservePlaybackStateChanged(
        stopSeekOracle, WMP::MediaPlaybackState::Paused, freshPauseSequence,
        freshPauseAtSource);
    CHECK(stopSeekOracle.logicalStopEvents == 1);
    CHECK(stopSeekOracle.lastCompletedStopEpoch == seekStopEpoch);
    CHECK(stopSeekOracle.lastCompletedStopPauseOperationId ==
          seekStopPause.operationId);
    CHECK(stopSeekOracle.lastCompletedStopSeekOperationId ==
          seekStopSeek.operationId);

    // The same stale-snapshot retry is proved independently for an old
    // PublicPause. Its immutable ticket cannot become StopPause evidence.
    MediaState stopPauseOracle;
    stopPauseOracle.commandEpoch = 20;
    stopPauseOracle.logicalState = LogicalMediaState::Playing;
    NativeOperationTracker stopPauseTracker;
    const std::shared_ptr<NativeEventCorrelation> stopPauseCorrelation =
        std::make_shared<NativeEventCorrelation>();
    NativeOperationTicket oldPublicPause;
    REQUIRE(stopPauseTracker.TryBeginPublicPause(
        stopPauseOracle.commandEpoch, *stopPauseCorrelation,
        CaptureEntrySequence(*stopPauseCorrelation), &oldPublicPause) ==
        NativeOperationStartResult::Started);
    const std::uint64_t pauseRaceBarrier = 200;
    stopPauseCorrelation->sourceSequence.store(
        pauseRaceBarrier, std::memory_order_release);
    const std::uint64_t stalePauseStopSnapshot =
        CaptureEntrySequence(*stopPauseCorrelation);
    const std::uint64_t oldPauseSequence =
        StampNativeEvent(stopPauseCorrelation);
    const NativeOperationTicket oldPauseAtSource =
        stopPauseTracker.CompletePauseAtSource(oldPauseSequence);
    const std::uint64_t pauseStopEpoch = NextCommandEpoch(stopPauseOracle);
    NativeOperationTicket pauseStopPause;
    NativeOperationTicket pauseStopSeek;
    CHECK(stopPauseTracker.TryBeginStop(
        pauseStopEpoch, *stopPauseCorrelation, stalePauseStopSnapshot, true,
        &pauseStopPause, &pauseStopSeek) ==
        NativeOperationStartResult::StaleSnapshot);
    CHECK(!pauseStopPause.IsValid());
    CHECK(!pauseStopSeek.IsValid());
    CHECK(stopPauseOracle.stopEpoch == 0);
    REQUIRE(stopPauseTracker.TryBeginStop(
        pauseStopEpoch, *stopPauseCorrelation,
        CaptureEntrySequence(*stopPauseCorrelation), true,
        &pauseStopPause, &pauseStopSeek) ==
        NativeOperationStartResult::Started);
    BeginStopIntent(stopPauseOracle, pauseStopEpoch, false,
                    pauseStopPause, pauseStopSeek);
    ObservePlaybackStateChanged(
        stopPauseOracle, WMP::MediaPlaybackState::Paused, oldPauseSequence,
        oldPauseAtSource);
    CHECK(stopPauseOracle.staleStopPauseEvents == 1);
    CHECK(stopPauseOracle.logicalStopEvents == 0);

    // Stop must also exercise the real playing -> native pause + seek join,
    // not only the already-paused shortcut.
    const unsigned playsBeforeStop = state->logicalPlayEvents;
    REQUIRE(media.Play());
    REQUIRE(DrainUntil(
        [state, playsBeforeStop]()
        {
            return state->nativeState == WMP::MediaPlaybackState::Playing &&
                   state->logicalState == LogicalMediaState::Playing &&
                   state->logicalPlayEvents == playsBeforeStop + 1;
        }, StateWaitRounds));
    DrainDispatch(2);
    CHECK(state->logicalPlayEvents == playsBeforeStop + 1);

    const unsigned stopEvents = state->logicalStopEvents;
    const unsigned stopSeeks = state->seekCompleted;
    const unsigned pausesAtStop = state->logicalPauseEvents;
    REQUIRE(media.Stop());
    const std::uint64_t issuedStopEpoch = state->stopEpoch;
    const std::uint64_t issuedPauseBarrier =
        state->stopPauseAfterSequence;
    const std::uint64_t issuedSeekBarrier =
        state->stopSeekAfterSequence;
    const std::uint64_t issuedStopPauseOperation =
        state->stopPauseOperationId;
    const std::uint64_t issuedStopSeekOperation =
        state->stopSeekOperationId;
    REQUIRE(issuedStopEpoch != 0);
    REQUIRE(issuedStopPauseOperation != 0);
    REQUIRE(issuedStopSeekOperation != 0);
    REQUIRE(media.WaitStoppedAfter(stopEvents, stopSeeks));
    DrainDispatch(5);
    CHECK(state->logicalStopEvents == stopEvents + 1);
    CHECK(state->seekCompleted == stopSeeks + 1);
    CHECK(state->logicalPauseEvents == pausesAtStop);
    CHECK(state->lastCompletedStopEpoch == issuedStopEpoch);
    CHECK(state->lastCompletedStopPauseOperationId ==
          issuedStopPauseOperation);
    CHECK(state->lastCompletedStopSeekOperationId ==
          issuedStopSeekOperation);
    CHECK(state->lastCompletedStopPauseSequence > issuedPauseBarrier);
    CHECK(state->lastCompletedStopSeekSequence > issuedSeekBarrier);

    // A queued callback for generation N must not mutate generation N+1.
    const std::uint64_t waveGeneration = media.LoadGeneration();
    const unsigned staleBefore = fixture.ledger->staleLoad.load();
    const unsigned durationChangesBeforeAvi = state->durationChanges;
    const unsigned sizeChangesBeforeAvi = state->videoSizeChanges;
    const std::shared_ptr<NativeOperationTracker> waveOperations =
        media.OperationTrackerForTest();
    REQUIRE(waveOperations != nullptr);
    NativeEventCorrelation waveCorrelation;
    NativeOperationTicket retiredGenerationIntent;
    REQUIRE(waveOperations->TryBeginPublicSeek(
        state->commandEpoch, waveCorrelation,
        CaptureEntrySequence(waveCorrelation),
        &retiredGenerationIntent) == NativeOperationStartResult::Started);
    media.PostGenerationProbeForTest(waveGeneration);
    REQUIRE(media.LoadUrl(files.AviUrl()));
    const std::shared_ptr<NativeOperationTracker> aviOperations =
        media.OperationTrackerForTest();
    REQUIRE(aviOperations != nullptr);
    CHECK(aviOperations != waveOperations);
    CHECK(!aviOperations->HasPending());
    const NativeOperationTicket retiredGenerationCompletion =
        waveOperations->CompleteSeekAtSource(1);
    CHECK(retiredGenerationCompletion.operationId ==
          retiredGenerationIntent.operationId);
    CHECK(!waveOperations->HasPending());
    CHECK(!aviOperations->HasPending());
    REQUIRE(media.WaitOpened());
    REQUIRE(DrainUntil(
        [&fixture, staleBefore]()
        {
            return fixture.ledger->staleLoad.load() > staleBefore;
        }, StateWaitRounds));
    state = media.State();
    REQUIRE(DrainUntil(
        [&media, state]()
        {
            // Initial metadata is allowed to be final already when
            // MediaOpened fires; Natural*Changed is not guaranteed to repeat
            // the same values afterwards.
            return state->videoWidth == 96 && state->videoHeight == 64 &&
                   Nearly(media.DurationMilliseconds(), 3000, 300);
        }, StateWaitRounds));
    CHECK(state->durationChanges >= durationChangesBeforeAvi);
    CHECK(state->videoSizeChanges >= sizeChangesBeforeAvi);

    // Public Pause and terminal EOF are correlated by intent epoch and source
    // order, never by distance from NaturalDuration. Adjacent sequences model
    // the old "within 25 ms" edge without relying on a media clock. Deliver
    // the later EOF first to prove queue reordering cannot erase the Pause or
    // leave the final logical state Paused.
    MediaState pauseEofCorrelationOracle;
    pauseEofCorrelationOracle.logicalState = LogicalMediaState::Playing;
    NativeOperationTracker pauseEofTracker;
    NativeEventCorrelation pauseEofNativeCorrelation;
    NativeOperationTicket supersededPauseTicket;
    REQUIRE(pauseEofTracker.TryBeginPublicPause(
        1, pauseEofNativeCorrelation,
        CaptureEntrySequence(pauseEofNativeCorrelation),
        &supersededPauseTicket) == NativeOperationStartResult::Started);
    supersededPauseTicket = pauseEofTracker.CompletePauseAtSource(99);
    const std::uint64_t oraclePauseEpoch =
        NextCommandEpoch(pauseEofCorrelationOracle);
    pauseEofNativeCorrelation.sourceSequence.store(
        100, std::memory_order_release);
    NativeOperationTicket oraclePauseTicket;
    REQUIRE(pauseEofTracker.TryBeginPublicPause(
        oraclePauseEpoch, pauseEofNativeCorrelation,
        CaptureEntrySequence(pauseEofNativeCorrelation),
        &oraclePauseTicket) == NativeOperationStartResult::Started);
    BeginPauseIntent(pauseEofCorrelationOracle, oraclePauseEpoch,
                     oraclePauseTicket);
    ObservePlaybackStateChanged(
        pauseEofCorrelationOracle, WMP::MediaPlaybackState::Paused, 99,
        supersededPauseTicket);
    CHECK(pauseEofCorrelationOracle.logicalPauseEvents == 0);
    CHECK(pauseEofCorrelationOracle.stalePauseIntentEvents == 1);
    ObserveMediaEnded(pauseEofCorrelationOracle, 7, 103);
    ObservePlaybackStateChanged(
        pauseEofCorrelationOracle, WMP::MediaPlaybackState::Paused, 102,
        pauseEofTracker.CompletePauseAtSource(102));
    CHECK(pauseEofCorrelationOracle.logicalPauseEvents == 1);
    CHECK(pauseEofCorrelationOracle.lastCompletedPauseEpoch ==
          oraclePauseEpoch);
    CHECK(pauseEofCorrelationOracle.lastCompletedPauseOperationId ==
          oraclePauseTicket.operationId);
    CHECK(pauseEofCorrelationOracle.logicalStopEvents == 1);
    CHECK(pauseEofCorrelationOracle.logicalFinishEvents == 1);
    CHECK(pauseEofCorrelationOracle.logicalState ==
          LogicalMediaState::Stopped);

    // A native terminal Paused without a public Pause intent remains owned by
    // MediaEnded and cannot manufacture a wx Pause event.
    ObservePlaybackStateChanged(
        pauseEofCorrelationOracle, WMP::MediaPlaybackState::Paused, 104,
        NativeOperationTicket{});
    CHECK(pauseEofCorrelationOracle.logicalPauseEvents == 1);
    CHECK(pauseEofCorrelationOracle.uncommandedPausedStates == 1);
    CHECK(pauseEofCorrelationOracle.logicalState ==
          LogicalMediaState::Stopped);

    // EOF is the one place where wx sends the vetoable stop event. A veto
    // suppresses finished/state-changed, but the stop request itself was
    // already delivered exactly once.
    state->allowEofStop = false;
    const unsigned vetoedEnded = state->ended;
    const unsigned vetoedFinished = state->logicalFinishEvents;
    const unsigned vetoedStops = state->logicalStopEvents;
    const unsigned vetoedRequests = state->eofStopRequests;
    const unsigned vetoes = state->eofStopVetoes;
    const unsigned pausesBeforeVetoedEof = state->logicalPauseEvents;
    const unsigned eofSeek = state->seekCompleted;
    REQUIRE(media.SeekMilliseconds(
        std::max<std::int64_t>(0, media.DurationMilliseconds() - 180)));
    REQUIRE(media.WaitSeekAfter(eofSeek));
    DrainDispatch(2);
    CHECK(state->seekCompleted == eofSeek + 1);
    const unsigned eofRateChanges = state->playbackRateChanges;
    REQUIRE(media.SetPlaybackRate(2.0));
    REQUIRE(DrainUntil(
        [state, eofRateChanges]()
        {
            return state->playbackRateChanges > eofRateChanges;
        }, StateWaitRounds));
    const unsigned vetoedPlayEvents = state->logicalPlayEvents;
    REQUIRE(media.Play());
    REQUIRE(DrainUntil(
        [state, vetoedPlayEvents]()
        {
            return state->logicalState == LogicalMediaState::Playing &&
                   state->logicalPlayEvents == vetoedPlayEvents + 1;
        }, StateWaitRounds));
    REQUIRE(DrainUntil(
        [state, vetoedEnded]() { return state->ended > vetoedEnded; },
        MediaWaitRounds));
    DrainDispatch(5);
    CHECK(state->ended == vetoedEnded + 1);
    CHECK(state->logicalPlayEvents == vetoedPlayEvents + 1);
    CHECK(state->endedLoad == media.LoadGeneration());
    CHECK(state->logicalState == LogicalMediaState::Stopped);
    CHECK(state->logicalStopEvents == vetoedStops + 1);
    CHECK(state->eofStopRequests == vetoedRequests + 1);
    CHECK(state->eofStopVetoes == vetoes + 1);
    CHECK(state->logicalFinishEvents == vetoedFinished);
    CHECK(state->logicalPauseEvents == pausesBeforeVetoedEof);

    // Repeat with the same deterministic AVI and allow the EOF stop. It
    // publishes one stop and one finish, never a second queued stop.
    REQUIRE(media.LoadUrl(files.AviUrl()));
    REQUIRE(media.WaitOpened());
    state->allowEofStop = true;
    const unsigned allowedEnded = state->ended;
    const unsigned allowedFinished = state->logicalFinishEvents;
    const unsigned allowedStops = state->logicalStopEvents;
    const unsigned allowedRequests = state->eofStopRequests;
    const unsigned allowedVetoes = state->eofStopVetoes;
    const unsigned pausesBeforeAllowedEof = state->logicalPauseEvents;
    const unsigned allowedSeek = state->seekCompleted;
    REQUIRE(media.SeekMilliseconds(
        std::max<std::int64_t>(0, media.DurationMilliseconds() - 180)));
    REQUIRE(media.WaitSeekAfter(allowedSeek));
    DrainDispatch(2);
    CHECK(state->seekCompleted == allowedSeek + 1);
    const unsigned allowedPlayEvents = state->logicalPlayEvents;
    REQUIRE(media.Play());
    REQUIRE(DrainUntil(
        [state, allowedPlayEvents]()
        {
            return state->logicalState == LogicalMediaState::Playing &&
                   state->logicalPlayEvents == allowedPlayEvents + 1;
        }, StateWaitRounds));
    REQUIRE(DrainUntil(
        [state, allowedEnded]() { return state->ended > allowedEnded; },
        MediaWaitRounds));
    DrainDispatch(5);
    CHECK(state->ended == allowedEnded + 1);
    CHECK(state->logicalPlayEvents == allowedPlayEvents + 1);
    CHECK(state->endedLoad == media.LoadGeneration());
    CHECK(state->logicalState == LogicalMediaState::Stopped);
    CHECK(state->logicalStopEvents == allowedStops + 1);
    CHECK(state->eofStopRequests == allowedRequests + 1);
    CHECK(state->eofStopVetoes == allowedVetoes);
    CHECK(state->logicalFinishEvents == allowedFinished + 1);
    CHECK(state->logicalPauseEvents == pausesBeforeAllowedEof);

    // There is a native MediaFailed notification but wxMediaCtrl has no
    // wxEVT_MEDIA_ERROR. The production mapping must keep this as state/log
    // and must not invent a public event or publish an EOF side effect.
    const unsigned endedBeforeFailure = state->ended;
    const unsigned failedBeforeFailure = state->failed;
    const unsigned eofRequestsBeforeFailure = state->eofStopRequests;
    const unsigned stopsBeforeFailure = state->logicalStopEvents;
    const unsigned finishesBeforeFailure = state->logicalFinishEvents;
    REQUIRE(media.LoadUrl(files.InvalidUrl()));
    const std::uint64_t invalidGeneration = media.LoadGeneration();
    REQUIRE(media.WaitOpenedOrFailed(invalidGeneration));
    DrainDispatch(5);
    CHECK(state->failedLoad == invalidGeneration);
    CHECK(state->failed == failedBeforeFailure + 1);
    CHECK(state->openedLoad != invalidGeneration);
    CHECK(state->endedLoad == 0);
    CHECK(state->ended == endedBeforeFailure);
    CHECK(state->eofStopRequests == eofRequestsBeforeFailure);
    CHECK(state->logicalStopEvents == stopsBeforeFailure);
    CHECK(state->logicalFinishEvents == finishesBeforeFailure);
    CHECK((state->lastError != WMP::MediaPlayerError::Unknown ||
           FAILED(state->lastExtendedError)));

    // A failed source must not poison the player. A new generation recovers.
    REQUIRE(media.LoadUrl(files.WaveUrl()));
    REQUIRE(media.WaitOpened());
    DrainDispatch(5);
    CHECK(media.IsOpened());
    CHECK(state->endedLoad == 0);
    CHECK(state->ended == endedBeforeFailure);
    CHECK(state->eofStopRequests == eofRequestsBeforeFailure);
    CHECK(state->logicalStopEvents == stopsBeforeFailure);
    CHECK(state->logicalFinishEvents == finishesBeforeFailure);
    CHECK(SUCCEEDED(state->callbackHr));
    CheckDispatcherLedger(*fixture.ledger);
    FinishFixture(fixture, before);
}

TEST_CASE("WinUIMedia::SharedSlotGeometryFocusUiaAndCrossXamlRoot",
          "[winui-media][winui-015][HostLifecycle][HostState]"
          "[reparent][focus][z-order][uia]")
{
    LocalMediaFiles files;
    REQUIRE(files.Create());
    const HostSnapshot before = HostSnapshot::Capture();
    TwoTLWMediaFixture fixture;
    REQUIRE(fixture.Create());
    REQUIRE(fixture.session->LoadUrl(files.AviUrl()));
    REQUIRE(fixture.session->WaitOpened());

    MediaPeerSession& media = *fixture.session;
    const MUXC::MediaPlayerElement peer = media.Peer();
    const WMP::MediaPlayer player = media.Player();
    const WMP::MediaPlaybackSession playback = media.Playback();
    const WMC::MediaSource source = media.Source();
    const winrt::com_ptr<IUnknown> peerIdentity =
        GetCanonicalComIdentity(peer);
    const winrt::com_ptr<IUnknown> playerIdentity =
        GetCanonicalComIdentity(player);
    const winrt::com_ptr<IUnknown> playbackIdentity =
        GetCanonicalComIdentity(playback);
    const winrt::com_ptr<IUnknown> sourceIdentity =
        GetCanonicalComIdentity(source);
    const winrt::com_ptr<IUnknown> dispatcherIdentity =
        GetCanonicalComIdentity(media.Dispatcher());
    REQUIRE(peerIdentity != nullptr);
    REQUIRE(playerIdentity != nullptr);
    REQUIRE(playbackIdentity != nullptr);
    REQUIRE(sourceIdentity != nullptr);
    REQUIRE(dispatcherIdentity != nullptr);
    const MUX::XamlRoot rootA = peer.XamlRoot();
    REQUIRE(rootA != nullptr);

    wxWinUISlot *mediaSlot = fixture.hostA->FindSlot(fixture.owner);
    wxWinUISlot *buttonSlot = fixture.hostA->FindSlot(fixture.buttonA);
    REQUIRE(mediaSlot != nullptr);
    REQUIRE(buttonSlot != nullptr);

    const auto checkSemanticTargetAndUia =
        [&](wxWinUITopLevelHost *expectedHost,
            const MUX::XamlRoot& expectedRoot)
        {
            REQUIRE(expectedHost != nullptr);
            REQUIRE(expectedRoot != nullptr);
            wxWinUISlot * const currentSlot =
                expectedHost->FindSlot(fixture.owner);
            REQUIRE(currentSlot != nullptr);
            CHECK(currentSlot->GetContent() == peer);
            CHECK(currentSlot->GetSemanticTarget() == peer);
            CHECK(peer.XamlRoot() == expectedRoot);
            CHECK(wxWinUITLWHostIsInvisibleAccessibilityShell(
                fixture.owner));
            CHECK(wxFindWinFromHandle(GetHwndOf(fixture.owner)) ==
                  fixture.owner);

            // Ask again after every root transfer: this is an exact semantic
            // target/UIA check in the peer's current XamlRoot, not a cached
            // assertion made only before migration.
            const MUXAP::AutomationPeer automationPeer =
                MUXAP::FrameworkElementAutomationPeer::
                    CreatePeerForElement(peer);
            REQUIRE(automationPeer != nullptr);
            CHECK(automationPeer.GetName() ==
                  winrt::hstring(L"offline silent media player"));
            CHECK(std::wstring(automationPeer.GetClassName().c_str()) ==
                  std::wstring(L"NamedContainerAutomationPeer"));
            CHECK(automationPeer.GetAutomationControlType() ==
                  MUXAP::AutomationControlType::Group);
            CHECK(!automationPeer.GetLocalizedControlType().empty());
            CHECK(automationPeer.IsControlElement());
            CHECK(automationPeer.IsContentElement());
            CHECK(automationPeer.IsKeyboardFocusable());
        };
    checkSemanticTargetAndUia(fixture.hostA, rootA);

    CHECK(!media.AreControlsShown());
    CHECK(!media.ShowControls(wxMEDIACTRLPLAYERCONTROLS_STEP));
    CHECK(!media.AreControlsShown());
    CHECK(!media.ShowControls(wxMEDIACTRLPLAYERCONTROLS_VOLUME));
    CHECK(!media.AreControlsShown());
    REQUIRE(media.ShowControls(wxMEDIACTRLPLAYERCONTROLS_DEFAULT));
    CHECK(media.AreControlsShown());
    REQUIRE(media.ShowControls(wxMEDIACTRLPLAYERCONTROLS_NONE));
    CHECK(!media.AreControlsShown());
    REQUIRE(media.ShowControls(wxMEDIACTRLPLAYERCONTROLS_DEFAULT));
    CHECK(media.AreControlsShown());
    fixture.hostA->FlushSync();

    CHECK(mediaSlot->GetRectInTLW().width > 0);
    CHECK(mediaSlot->GetRectInTLW().height > 0);
    fixture.owner->SetSize(wxRect(24, 24, 285, 165));
    fixture.hostA->FlushSync();
    CHECK(std::abs(mediaSlot->GetContainer().Width() -
                   fixture.owner->ToDIP(285)) < 3.0);
    CHECK(std::abs(mediaSlot->GetContainer().Height() -
                   fixture.owner->ToDIP(165)) < 3.0);

    fixture.hostA->SetSlotClipHeight(fixture.owner, 43);
    fixture.hostA->FlushSync();
    const MUXM::RectangleGeometry clip =
        mediaSlot->GetContainer().Clip().try_as<MUXM::RectangleGeometry>();
    REQUIRE(clip != nullptr);
    CHECK(std::abs(clip.Rect().Height - 43.0f) < 1.0f);
    fixture.owner->Hide();
    fixture.hostA->FlushSync();
    CHECK(mediaSlot->GetContainer().Visibility() == MUX::Visibility::Collapsed);
    fixture.owner->Show();
    fixture.hostA->FlushSync();
    CHECK(mediaSlot->GetContainer().Visibility() == MUX::Visibility::Visible);

    fixture.buttonA->SetSize(wxRect(180, 95, 140, 58));
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.owner->Raise();
    fixture.hostA->FlushSync();
    CHECK(MUXC::Canvas::GetZIndex(mediaSlot->GetContainer()) >
          MUXC::Canvas::GetZIndex(buttonSlot->GetContainer()));
    fixture.owner->Lower();
    fixture.hostA->FlushSync();
    CHECK(MUXC::Canvas::GetZIndex(mediaSlot->GetContainer()) <
          MUXC::Canvas::GetZIndex(buttonSlot->GetContainer()));
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);
    fixture.hostA->SetSlotClipHeight(fixture.owner, -1);

    REQUIRE((::GetWindowLongPtrW(GetHwndOf(fixture.owner), GWL_STYLE) &
             WS_TABSTOP) != 0);
    fixture.hostA->FocusSlot(fixture.owner);
    REQUIRE(DrainUntil(
        [&fixture, peer]()
        {
            return fixture.hostA->GetFocusOwner() == fixture.owner &&
                   wxWindow::FindFocus() == fixture.owner &&
                   IsXamlFocusInside(peer, peer.XamlRoot());
        }, StateWaitRounds));
    fixture.hostA->TestTakeFocusRequested(false);
    REQUIRE(DrainUntil(
        [&fixture, peer]()
        {
            return fixture.hostA->GetFocusOwner() == fixture.buttonA &&
                   wxWindow::FindFocus() == fixture.buttonA &&
                   !IsXamlFocusInside(peer, peer.XamlRoot());
        }, StateWaitRounds));
    fixture.hostA->TestTakeFocusRequested(true);
    REQUIRE(DrainUntil(
        [&fixture, peer]()
        {
            return fixture.hostA->GetFocusOwner() == fixture.owner &&
                   wxWindow::FindFocus() == fixture.owner &&
                   IsXamlFocusInside(peer, peer.XamlRoot());
        }, StateWaitRounds));

    const std::shared_ptr<MediaState> mediaState = media.State();
    REQUIRE(mediaState != nullptr);
    const unsigned volumeChanges = mediaState->volumeChanges;
    const unsigned rateChanges = mediaState->playbackRateChanges;
    REQUIRE(media.SetVolume(0.42));
    REQUIRE(media.SetPlaybackRate(1.25));
    REQUIRE(DrainUntil(
        [mediaState, volumeChanges, rateChanges]()
        {
            return mediaState->volumeChanges > volumeChanges &&
                   mediaState->playbackRateChanges > rateChanges;
        }, StateWaitRounds));

    const unsigned playEvents = mediaState->logicalPlayEvents;
    REQUIRE(media.Play());
    REQUIRE(DrainUntil(
        [mediaState, playEvents]()
        {
            return mediaState->logicalState == LogicalMediaState::Playing &&
                   mediaState->logicalPlayEvents == playEvents + 1;
        }, StateWaitRounds));
    DrainDispatch(2);
    CHECK(mediaState->logicalPlayEvents == playEvents + 1);
    const unsigned pauseEvents = mediaState->logicalPauseEvents;
    REQUIRE(media.Pause());
    REQUIRE(DrainUntil(
        [mediaState, pauseEvents]()
        {
            return mediaState->nativeState ==
                       WMP::MediaPlaybackState::Paused &&
                   mediaState->logicalState == LogicalMediaState::Paused &&
                   mediaState->logicalPauseEvents == pauseEvents + 1;
        }, StateWaitRounds));
    DrainDispatch(2);
    CHECK(mediaState->logicalPauseEvents == pauseEvents + 1);

    const unsigned seeksBeforeSnapshot = mediaState->seekCompleted;
    REQUIRE(media.SeekMilliseconds(850));
    REQUIRE(media.WaitSeekAfter(seeksBeforeSnapshot));
    DrainDispatch(2);
    CHECK(mediaState->seekCompleted == seeksBeforeSnapshot + 1);
    REQUIRE(Nearly(media.PositionMilliseconds(), 850, 120));
    const std::int64_t positionBefore = media.PositionMilliseconds();
    const std::uint64_t loadGeneration = media.LoadGeneration();
    CHECK(media.ActiveHandlerCount() == 9);

    const auto checkAttachedGraph =
        [&](const MUX::XamlRoot& expectedRoot,
            std::int64_t expectedPosition)
        {
            REQUIRE(peer.XamlRoot() == expectedRoot);
            CHECK(GetCanonicalComIdentity(media.Peer()).get() ==
                  peerIdentity.get());
            CHECK(GetCanonicalComIdentity(media.AttachedPlayer()).get() ==
                  playerIdentity.get());
            CHECK(GetCanonicalComIdentity(media.AttachedPlayback()).get() ==
                  playbackIdentity.get());
            CHECK(GetCanonicalComIdentity(media.AttachedPeerSource()).get() ==
                  sourceIdentity.get());
            CHECK(GetCanonicalComIdentity(media.AttachedPlayerSource()).get() ==
                  sourceIdentity.get());
            CHECK(GetCanonicalComIdentity(media.PeerDispatcher()).get() ==
                  dispatcherIdentity.get());
            CHECK(GetCanonicalComIdentity(media.Player()).get() ==
                  playerIdentity.get());
            CHECK(GetCanonicalComIdentity(media.Playback()).get() ==
                  playbackIdentity.get());
            CHECK(GetCanonicalComIdentity(media.Source()).get() ==
                  sourceIdentity.get());
            CHECK(GetCanonicalComIdentity(media.Dispatcher()).get() ==
                  dispatcherIdentity.get());
            CHECK(media.LoadGeneration() == loadGeneration);
            CHECK(media.IsOpened());
            CHECK(media.ActiveHandlerCount() == 9);
            CHECK(media.AreControlsShown());
            CHECK(std::abs(media.Volume() - 0.42) < 0.001);
            CHECK(std::abs(media.PlaybackRate() - 1.25) < 0.001);
            CHECK(Nearly(media.PositionMilliseconds(),
                         expectedPosition, 120));
            CHECK(mediaState->nativeState ==
                  WMP::MediaPlaybackState::Paused);
            CHECK(mediaState->logicalState == LogicalMediaState::Paused);
            CHECK(mediaState->failedLoad != loadGeneration);
        };
    checkAttachedGraph(rootA, positionBefore);

    // This is the decisive topology gate. The same MediaPlayerElement and
    // playback graph must survive transfer to another shared XamlRoot.
    REQUIRE(fixture.owner->Reparent(fixture.panelB));
    REQUIRE(DrainUntil(
        [&fixture, peer, rootA]()
        {
            return wxWinUITopLevelHost::FindSlotOwner(fixture.owner) ==
                       fixture.hostB &&
                   fixture.hostB->FindSlot(fixture.owner) != nullptr &&
                   peer.XamlRoot() != nullptr &&
                   peer.XamlRoot() != rootA;
        }, StateWaitRounds));
    CHECK(fixture.hostA->FindSlot(fixture.owner) == nullptr);
    CHECK(fixture.hostA->GetFocusOwner() != fixture.owner);
    CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
          before.pendingFocusMigrations);
    // Reparenting to an inactive TLW does not promise to preserve focus.
    // Reacquire it explicitly and prove the destination host can establish
    // the complete wx/XAML focus identity.
    fixture.hostB->FocusSlot(fixture.owner);
    REQUIRE(DrainUntil(
        [&fixture, peer]()
        {
            return fixture.hostB->GetFocusOwner() == fixture.owner &&
                   wxWindow::FindFocus() == fixture.owner &&
                   IsXamlFocusInside(peer, peer.XamlRoot());
        }, StateWaitRounds));
    mediaSlot = fixture.hostB->FindSlot(fixture.owner);
    REQUIRE(mediaSlot != nullptr);
    const MUX::XamlRoot rootB = peer.XamlRoot();
    REQUIRE(rootB != nullptr);
    REQUIRE(rootB != rootA);
    checkSemanticTargetAndUia(fixture.hostB, rootB);
    checkAttachedGraph(rootB, positionBefore);

    const unsigned playEventsB = mediaState->logicalPlayEvents;
    REQUIRE(media.Play());
    REQUIRE(DrainUntil(
        [mediaState, playEventsB]()
        {
            return mediaState->logicalState == LogicalMediaState::Playing &&
                   mediaState->logicalPlayEvents == playEventsB + 1;
        }, StateWaitRounds));
    DrainDispatch(2);
    CHECK(mediaState->logicalPlayEvents == playEventsB + 1);
    const unsigned pauseEventsB = mediaState->logicalPauseEvents;
    REQUIRE(media.Pause());
    REQUIRE(DrainUntil(
        [mediaState, pauseEventsB]()
        {
            return mediaState->nativeState ==
                       WMP::MediaPlaybackState::Paused &&
                   mediaState->logicalState == LogicalMediaState::Paused &&
                   mediaState->logicalPauseEvents == pauseEventsB + 1;
        }, StateWaitRounds));
    DrainDispatch(2);
    CHECK(mediaState->logicalPauseEvents == pauseEventsB + 1);

    const unsigned seeksBeforeReturn = mediaState->seekCompleted;
    REQUIRE(media.SeekMilliseconds(850));
    REQUIRE(media.WaitSeekAfter(seeksBeforeReturn));
    DrainDispatch(2);
    CHECK(mediaState->seekCompleted == seeksBeforeReturn + 1);
    REQUIRE(Nearly(media.PositionMilliseconds(), 850, 120));
    const std::int64_t positionBeforeReturn = media.PositionMilliseconds();
    REQUIRE(fixture.owner->Reparent(fixture.panelA));
    REQUIRE(DrainUntil(
        [&fixture, peer, rootA]()
        {
            return wxWinUITopLevelHost::FindSlotOwner(fixture.owner) ==
                       fixture.hostA &&
                   fixture.hostA->FindSlot(fixture.owner) != nullptr &&
                   peer.XamlRoot() == rootA;
        }, StateWaitRounds));
    CHECK(fixture.hostB->FindSlot(fixture.owner) == nullptr);
    CHECK(fixture.hostB->GetFocusOwner() != fixture.owner);
    REQUIRE(fixture.hostA->FindSlot(fixture.owner) != nullptr);
    CHECK(peer.XamlRoot() == rootA);
    CHECK(peer.XamlRoot() != rootB);
    CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
          before.pendingFocusMigrations);
    fixture.hostA->FocusSlot(fixture.owner);
    REQUIRE(DrainUntil(
        [&fixture, peer]()
        {
            return fixture.hostA->GetFocusOwner() == fixture.owner &&
                   wxWindow::FindFocus() == fixture.owner &&
                   IsXamlFocusInside(peer, peer.XamlRoot());
        }, StateWaitRounds));
    checkSemanticTargetAndUia(fixture.hostA, rootA);
    checkAttachedGraph(rootA, positionBeforeReturn);
    CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
          before.pendingFocusMigrations);

    CheckDispatcherLedger(*fixture.ledger);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    FinishFixture(fixture, before);
}

TEST_CASE("WinUIMedia::DestroyDuringLoadAndPlayback",
          "[winui-media][winui-015][HostLifecycle][async][destroy]")
{
    LocalMediaFiles files;
    REQUIRE(files.Create());

    SECTION("destroy during load")
    {
        DestroyAtPhase(files, false);
    }
    SECTION("destroy while playing")
    {
        DestroyAtPhase(files, true);
    }
}

TEST_CASE("WinUIMedia::HundredLoadRetireAndRepeatedLoadCycles",
          "[winui-media][winui-015][HostLifecycle][stress][async]")
{
    LocalMediaFiles files;
    REQUIRE(files.Create());
    const HostSnapshot before = HostSnapshot::Capture();
    const auto ledger = std::make_shared<CallbackLedger>();
    std::unique_ptr<wxFrame> frame(
        new wxFrame(nullptr, wxID_ANY, "media-soak",
                    wxDefaultPosition, wxSize(390, 250),
                    wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                        wxFRAME_TOOL_WINDOW));
    wxPanel * const panel = new wxPanel(frame.get());
    wxButton * const button = new wxButton(
        panel, wxID_ANY, "soak-neighbour",
        wxPoint(320, 25), wxSize(55, 40));
    wxUnusedVar(button);
    REQUIRE(ShowOffscreenWithoutActivation(frame.get(), 3));
    REQUIRE(DrainToQuiescence());

    const unsigned steadySlots = wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned steadySlotLifetimes =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned steadyFocusMigrations =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    const wxStopWatch cycles;

    for ( unsigned i = 0; i < 100; ++i )
    {
        CAPTURE(i);
        REQUIRE(RemainingSoakRounds(cycles, 1) != 0);
        std::unique_ptr<wxWinUIXamlHost> owner(
            new wxWinUIXamlHost(panel, wxID_ANY,
                                wxPoint(10, 10), wxSize(295, 170)));
        std::weak_ptr<MediaState> weakState;
        std::shared_ptr<MediaState> retiredState;
        unsigned acceptedAtClose = 0;
        unsigned staleLifetimeBefore = 0;
        {
            MediaPeerSession media;
            REQUIRE(media.Initialize(owner.get(), ledger));
            weakState = media.WeakState();
            REQUIRE(media.LoadUrl(i >= 75 ? files.AviUrl()
                                          : files.WaveUrl()));
            CHECK(media.ActiveHandlerCount() == 9);

            if ( i < 50 )
            {
                // No event-pump turn occurs after Source() and before the
                // session leaves scope. This is a publication/retirement
                // stress path only; it does not claim that the native open
                // has entered. DestroyAtPhase's MediaBinder deferral is the
                // deterministic native-opening gate.
                const std::shared_ptr<MediaState> opening = media.State();
                REQUIRE(opening != nullptr);
                CHECK(opening->openedLoad != media.LoadGeneration());
                CHECK(opening->failedLoad != media.LoadGeneration());
                CHECK(!media.IsOpened());
            }
            else if ( i < 75 )
            {
                const unsigned openRounds =
                    RemainingSoakRounds(cycles, MediaWaitRounds);
                REQUIRE(openRounds != 0);
                REQUIRE(media.WaitOpened(0, openRounds));
                REQUIRE(media.Play());
                const unsigned playRounds =
                    RemainingSoakRounds(cycles, StateWaitRounds);
                REQUIRE(playRounds != 0);
                REQUIRE(DrainUntil(
                    [state = media.State()]()
                    {
                        return state->logicalState ==
                               LogicalMediaState::Playing;
                    }, playRounds));
                CHECK(media.IsMuted());
            }
            else
            {
                // Two real sources on one player, without pumping between
                // the explicit old-generation probe and the replacement.
                const std::uint64_t oldGeneration = media.LoadGeneration();
                const unsigned staleLoadBefore = ledger->staleLoad.load();
                media.PostGenerationProbeForTest(oldGeneration);
                REQUIRE(media.LoadUrl(files.WaveUrl()));
                const unsigned staleRounds =
                    RemainingSoakRounds(cycles, StateWaitRounds);
                REQUIRE(staleRounds != 0);
                REQUIRE(DrainUntil(
                    [ledger, staleLoadBefore]()
                    {
                        return ledger->staleLoad.load() > staleLoadBefore;
                    }, staleRounds));
                const unsigned openRounds =
                    RemainingSoakRounds(cycles, MediaWaitRounds);
                REQUIRE(openRounds != 0);
                REQUIRE(media.WaitOpened(0, openRounds));
                CHECK(media.State()->failedLoad != media.LoadGeneration());
                CHECK(media.ActiveHandlerCount() == 9);
            }

            retiredState = media.State();
            REQUIRE(retiredState != nullptr);
            acceptedAtClose = ledger->accepted.load();
            staleLifetimeBefore = ledger->staleLifetime.load();
            media.PostGenerationProbeForTest(media.LoadGeneration());
        }

        const unsigned lifetimeRounds =
            RemainingSoakRounds(cycles, StateWaitRounds);
        REQUIRE(lifetimeRounds != 0);
        REQUIRE(DrainUntil(
            [ledger, staleLifetimeBefore]()
            {
                return ledger->staleLifetime.load() >
                       staleLifetimeBefore;
            }, lifetimeRounds));
        CHECK(ledger->accepted.load() == acceptedAtClose);
        retiredState.reset();
        const unsigned expiryRounds =
            RemainingSoakRounds(cycles, 250);
        REQUIRE(expiryRounds != 0);
        REQUIRE(DrainUntil([&weakState]() { return weakState.expired(); },
                           expiryRounds));
        owner.reset();
        const unsigned slotRounds =
            RemainingSoakRounds(cycles, 500);
        REQUIRE(slotRounds != 0);
        REQUIRE(DrainUntil(
            [steadySlots, steadySlotLifetimes]()
            {
                return wxWinUITopLevelHost::GetLiveSlotCount() ==
                           steadySlots &&
                       wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                           steadySlotLifetimes;
            }, slotRounds));
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              steadyFocusMigrations);
        CHECK(ledger->handlerAdds.load() ==
              ledger->handlerRevokes.load());
        CHECK(ledger->sessionsInitialized.load() ==
              ledger->sessionsClosed.load());
        CHECK(ledger->teardownStepFailures.load() == 0);

        if ( i % 10 == 9 )
        {
            REQUIRE(RemainingSoakRounds(cycles, 1) != 0);
            REQUIRE(DrainToQuiescence());
            CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots);
            CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                  steadySlotLifetimes);
        }
    }

    INFO("100 MediaPlayerElement load/retire cycles took " <<
         cycles.Time() << " ms");
    CHECK(cycles.Time() < MediaSoakBudgetMilliseconds);
    CHECK(ledger->accepted.load() > 0);
    CHECK(ledger->staleLifetime.load() >= 100);
    CHECK(ledger->staleLoad.load() >= 25);
    CHECK(ledger->sessionsInitialized.load() == 100);
    CHECK(ledger->sessionsClosed.load() == 100);
    CHECK(ledger->handlerAdds.load() == 125 * 9);
    CHECK(ledger->handlerRevokes.load() == 125 * 9);
    CHECK(ledger->sourceCloseAttempts.load() == 125);
    CHECK(ledger->sourceCloseSuccesses.load() == 125);
    CHECK(ledger->playerCloseAttempts.load() == 100);
    CHECK(ledger->playerCloseSuccesses.load() == 100);
    CHECK(ledger->teardownStepAttempts.load() == 125 * 9 + 100 * 6);
    CHECK(ledger->teardownStepSuccesses.load() == 125 * 9 + 100 * 6);
    CHECK(ledger->enqueueFailures.load() == 0);
    CHECK(ledger->callbackExceptions.load() == 0);
    CheckResourceLedger(*ledger);

    frame.reset();
    const unsigned restoreRounds =
        RemainingSoakRounds(cycles, 1000);
    REQUIRE(restoreRounds != 0);
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); },
                       restoreRounds));
    before.CheckRestored();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_MEDIACTRL
