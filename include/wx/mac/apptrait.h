///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/apptrait.h
// Purpose:     standard implementations of wxAppTraits for MacOS
// Author:      Vadim Zeitlin
// Modified by: Stefan Csomor
// Created:     23.06.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_APPTRAIT_H_
#define _WX_MAC_APPTRAIT_H_

// ----------------------------------------------------------------------------
// wxGUI/ConsoleAppTraits: must derive from wxAppTraits, not wxAppTraitsBase
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxConsoleAppTraits : public wxConsoleAppTraitsBase
{
public:
};

#if wxUSE_GUI

class WXDLLEXPORT wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
};

#endif // wxUSE_GUI

#endif // _WX_MAC_APPTRAIT_H_

