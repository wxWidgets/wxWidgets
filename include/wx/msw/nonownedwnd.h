///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/nonownedwnd.h
// Purpose:     wxNonOwnedWindow declaration for wxMSW.
// Author:      Vadim Zeitlin
// Created:     2011-10-09
// RCS-ID:      $Id: wxhead.h,v 1.12 2010-04-22 12:44:51 zeitlin Exp $
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_NONOWNEDWND_H_
#define _WX_MSW_NONOWNEDWND_H_

// ----------------------------------------------------------------------------
// wxNonOwnedWindow
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNonOwnedWindow : public wxNonOwnedWindowBase
{
public:
    virtual bool SetShape(const wxRegion& region);
};

#endif // _WX_MSW_NONOWNEDWND_H_
