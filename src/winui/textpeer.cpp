/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/textpeer.cpp
// Purpose:     Shared private contracts for WinUI text peers
// Author:      wxWidgets development team
// Created:     2026-07-25
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/private/textpeer.h"

#include <algorithm>
#include <limits>

namespace
{

std::atomic<std::uint64_t> gs_nextTextCallbackGeneration{ 0 };
std::atomic<std::size_t> gs_liveTextCallbackStates{ 0 };

} // anonymous namespace

wxWinUITextCallbackState::wxWinUITextCallbackState(void *owner)
    : m_owner(owner),
      m_generation(
          gs_nextTextCallbackGeneration.fetch_add(
              1, std::memory_order_relaxed) + 1)
{
    gs_liveTextCallbackStates.fetch_add(1, std::memory_order_relaxed);
}

wxWinUITextCallbackState::~wxWinUITextCallbackState()
{
    Invalidate();
    gs_liveTextCallbackStates.fetch_sub(1, std::memory_order_relaxed);
}

std::size_t wxWinUITextCallbackState::GetLiveCountForTesting()
{
    return gs_liveTextCallbackStates.load(std::memory_order_relaxed);
}

wxWinUITextPositionMap::wxWinUITextPositionMap(
    const wxString& value,
    bool multiline,
    bool twoCharacterNewlines)
    : m_value(NormalizeNewlines(value)),
      m_multiline(multiline),
      m_twoCharacterNewlines(multiline && twoCharacterNewlines)
{
}

long wxWinUITextPositionMap::GetLastPosition() const
{
    const std::size_t maxPosition =
        static_cast<std::size_t>(std::numeric_limits<long>::max());
    std::size_t length = m_value.length();
    if ( m_twoCharacterNewlines )
    {
        const std::size_t newlineCount =
            static_cast<std::size_t>(
                std::count(m_value.begin(), m_value.end(), '\n'));
        if ( newlineCount > maxPosition ||
             length > maxPosition - newlineCount )
        {
            return std::numeric_limits<long>::max();
        }
        length += newlineCount;
    }
    return length > maxPosition
        ? std::numeric_limits<long>::max()
        : static_cast<long>(length);
}

bool wxWinUITextPositionMap::PublicToLogical(
    long position,
    long *logical) const
{
    if ( position < 0 || position > GetLastPosition() )
        return false;

    if ( !m_twoCharacterNewlines )
    {
        if ( logical )
            *logical = position;
        return true;
    }

    long publicPosition = 0;
    for ( std::size_t i = 0; i < m_value.length(); ++i )
    {
        if ( position == publicPosition )
        {
            if ( logical )
                *logical = static_cast<long>(i);
            return true;
        }

        if ( m_value[i] == '\n' )
        {
            // Both insertion positions occupied by CRLF describe the end of
            // the preceding logical line.
            if ( position == publicPosition + 1 )
            {
                if ( logical )
                    *logical = static_cast<long>(i);
                return true;
            }
            publicPosition += 2;
        }
        else
        {
            ++publicPosition;
        }
    }

    if ( position == publicPosition )
    {
        if ( logical )
            *logical = static_cast<long>(m_value.length());
        return true;
    }
    return false;
}

bool wxWinUITextPositionMap::PublicRangeToLogical(
    long from,
    long to,
    long *logicalFrom,
    long *logicalTo) const
{
    if ( from < 0 || to < from || to > GetLastPosition() )
        return false;

    if ( from == to )
    {
        long logical = 0;
        if ( !PublicToLogical(from, &logical) )
            return false;
        if ( logicalFrom )
            *logicalFrom = logical;
        if ( logicalTo )
            *logicalTo = logical;
        return true;
    }

    if ( !m_twoCharacterNewlines )
    {
        if ( logicalFrom )
            *logicalFrom = from;
        if ( logicalTo )
            *logicalTo = to;
        return true;
    }

    auto mapEndpoint = [this](long position, bool end) -> long
    {
        long publicPosition = 0;
        for ( std::size_t i = 0; i < m_value.length(); ++i )
        {
            if ( position == publicPosition )
                return static_cast<long>(i);

            if ( m_value[i] == '\n' )
            {
                if ( position == publicPosition + 1 )
                {
                    // A range touching either half of CRLF owns the logical
                    // newline. Start endpoints round towards it, end
                    // endpoints round past it.
                    return static_cast<long>(i) + (end ? 1 : 0);
                }
                publicPosition += 2;
            }
            else
            {
                ++publicPosition;
            }
        }
        return position == publicPosition
            ? static_cast<long>(m_value.length())
            : -1;
    };

    const long mappedFrom = mapEndpoint(from, false);
    const long mappedTo = mapEndpoint(to, true);
    if ( mappedFrom < 0 || mappedTo < mappedFrom )
        return false;

    if ( logicalFrom )
        *logicalFrom = mappedFrom;
    if ( logicalTo )
        *logicalTo = mappedTo;
    return true;
}

wxString wxWinUITextPositionMap::GetRange(long from, long to) const
{
    if ( from < 0 || to < from || to > GetLastPosition() )
        return wxString();

    const wxString publicValue = ExpandPublicValue(m_value);
    return NormalizeNewlines(publicValue.Mid(from, to - from));
}

bool wxWinUITextPositionMap::ReplaceRange(
    long from,
    long to,
    const wxString& replacement,
    wxString *value,
    long *insertionPoint) const
{
    if ( from < 0 || to < from || to > GetLastPosition() )
        return false;

    const wxString publicValue = ExpandPublicValue(m_value);
    const wxString publicReplacement =
        ExpandPublicValue(NormalizeNewlines(replacement));
    const wxString publicPrefix = publicValue.Left(from);
    const wxString result = NormalizeNewlines(
        publicPrefix + publicReplacement + publicValue.Mid(to));

    if ( value )
        *value = result;

    if ( insertionPoint )
    {
        // Normalize the exact prefix ending at the new caret and convert that
        // prefix back to this port's public coordinate space. This also gives
        // deterministic semantics for insertion between CR and LF.
        const wxString normalizedPrefix =
            NormalizeNewlines(publicPrefix + publicReplacement);
        wxWinUITextPositionMap prefixMap(
            normalizedPrefix, m_multiline, m_twoCharacterNewlines);
        *insertionPoint = prefixMap.GetLastPosition();
    }
    return true;
}

long wxWinUITextPositionMap::LogicalToPublic(long logical) const
{
    if ( logical < 0 ||
         static_cast<std::size_t>(logical) > m_value.length() )
    {
        return -1;
    }

    if ( !m_twoCharacterNewlines )
        return logical;

    long position = logical;
    for ( long i = 0; i < logical; ++i )
    {
        if ( m_value[static_cast<std::size_t>(i)] == '\n' )
            ++position;
    }
    return position;
}

bool wxWinUITextPositionMap::PositionToXY(
    long position,
    long *x,
    long *y) const
{
    if ( position < 0 || position > GetLastPosition() )
        return false;

    if ( !m_multiline )
    {
        if ( x )
            *x = position;
        if ( y )
            *y = 0;
        return true;
    }

    long publicPosition = 0;
    long column = 0;
    long row = 0;
    for ( wxUniChar character : m_value )
    {
        if ( position == publicPosition ||
             (m_twoCharacterNewlines &&
              character == '\n' &&
              position == publicPosition + 1) )
        {
            if ( x )
                *x = column;
            if ( y )
                *y = row;
            return true;
        }

        if ( character == '\n' )
        {
            publicPosition += m_twoCharacterNewlines ? 2 : 1;
            column = 0;
            ++row;
        }
        else
        {
            ++publicPosition;
            ++column;
        }
    }

    if ( position != publicPosition )
        return false;

    if ( x )
        *x = column;
    if ( y )
        *y = row;
    return true;
}

long wxWinUITextPositionMap::XYToPosition(long x, long y) const
{
    if ( x < 0 || y < 0 )
        return -1;

    if ( !m_multiline )
        return y == 0 && x <= GetLastPosition() ? x : -1;

    long row = 0;
    long lineStart = 0;
    long lineLength = 0;
    for ( wxUniChar character : m_value )
    {
        if ( character == '\n' )
        {
            if ( row == y )
                return x <= lineLength ? lineStart + x : -1;

            lineStart += lineLength +
                         (m_twoCharacterNewlines ? 2 : 1);
            lineLength = 0;
            ++row;
        }
        else
        {
            ++lineLength;
        }
    }

    if ( row == y )
        return x <= lineLength ? lineStart + x : -1;
    return -1;
}

wxString wxWinUITextPositionMap::NormalizeNewlines(
    const wxString& value)
{
    wxString normalized;
    normalized.reserve(value.length());
    for ( std::size_t i = 0; i < value.length(); ++i )
    {
        const wxUniChar character = value[i];
        if ( character != '\r' )
        {
            normalized += character;
            continue;
        }

        if ( i + 1 < value.length() && value[i + 1] == '\n' )
            ++i;
        normalized += '\n';
    }
    return normalized;
}

wxString wxWinUITextPositionMap::ExpandPublicValue(
    const wxString& value) const
{
    if ( !m_twoCharacterNewlines )
        return value;

    wxString expanded;
    expanded.reserve(
        value.length() +
        static_cast<std::size_t>(
            std::count(value.begin(), value.end(), '\n')));
    for ( wxUniChar character : value )
    {
        if ( character == '\n' )
            expanded += '\r';
        expanded += character;
    }
    return expanded;
}
