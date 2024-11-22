///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/wordwrap.h
// Purpose:     Simple helper for string word wrapping.
// Author:      Vadim Zeitlin
// Created:     2024-11-22
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_WORDWRAP_H_
#define _WX_PRIVATE_WORDWRAP_H_

#include "wx/string.h"

#include <vector>

// ----------------------------------------------------------------------------
// wxWordWrap helper for wxCmdLineParser
// ----------------------------------------------------------------------------

inline std::vector<wxString> wxWordWrap(const wxString& text, int widthMax)
{
    std::vector<wxString> lines;

    wxString line;
    int lastSpace = -1; // No space in the current line yet.
    for ( auto ch : text )
    {
        // Always honour explicit line breaks.
        if ( ch == '\n' )
        {
            lines.push_back(line);
            line.clear();
            lastSpace = -1;
        }
        else
        {
            const int lineLength = line.length();
            if ( lineLength == widthMax )
            {
                // Can't continue this line.
                if ( lastSpace == -1 )
                {
                    // No space in the line, just break it.
                    lines.push_back(line);
                    line.clear();
                }
                else
                {
                    // Break at the last space.
                    lines.push_back(line.substr(0, lastSpace));

                    // Also skip all the spaces following it, we don't want to
                    // start the new line with spaces.
                    while ( lastSpace < lineLength && line[lastSpace] == ' ' )
                        lastSpace++;

                    line = line.substr(lastSpace);
                    lastSpace = -1;
                }
            }
            else
            {
                if ( ch == ' ' )
                    lastSpace = line.length();
            }

            line += ch;
        }
    }

    if ( !line.empty() )
        lines.push_back(line);

    return lines;
}

#endif // _WX_PRIVATE_WORDWRAP_H_
