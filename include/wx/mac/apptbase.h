///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/apptbase.h
// Purpose:     declaration of wxAppTraits for Mac systems
// Author:      Vadim Zeitlin
// Modified by: Stefan Csomor
// Created:     23.06.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_APPTBASE_H_
#define _WX_MAC_APPTBASE_H_

// ----------------------------------------------------------------------------
// wxAppTraits: the Mac version adds extra hooks needed by Mac code
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAppTraits : public wxAppTraitsBase
{
public:
    // wxExecute() support methods
    // ---------------------------

    // TODO

    // wxThread helpers
    // ----------------


    // TODO

    // other miscellaneous helpers
    // ---------------------------

    // under Mac this function does the same thing for console and GUI
    // applications so we can implement it directly in the base class
    virtual int GetOSVersion(int *verMaj, int *verMin);
};

#endif // _WX_MAC_APPTBASE_H_

