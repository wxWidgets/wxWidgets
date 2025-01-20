///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/log.h
// Purpose:     Private wxLog-related declarations.
// Author:      Vadim Zeitlin
// Created:     2025-01-19 (extracted from log.cpp)
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_LOG_H_
#define _WX_PRIVATE_LOG_H_

#include "wx/log.h"
#include "wx/msgout.h"

// ----------------------------------------------------------------------------
// wxLogOutputBest: wxLog wrapper around wxMessageOutputBest
// ----------------------------------------------------------------------------

class wxLogOutputBest : public wxLog
{
public:
    wxLogOutputBest() = default;

protected:
    virtual void DoLogText(const wxString& msg) override
    {
        wxMessageOutputBest().Output(msg);
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxLogOutputBest);
};

#endif // _WX_PRIVATE_LOG_H_
