/////////////////////////////////////////////////////////////////////////////
// Name:        gauge95.h
// Purpose:     wxGauge95 class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _GAUGE95_H_
#define _GAUGE95_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "gauge95.h"
#endif

#if wxUSE_GAUGE

WXDLLEXPORT_DATA(extern const wxChar*) wxGaugeNameStr;

// Group box
class WXDLLEXPORT wxGauge95 : public wxGaugeBase
{
public:
    wxGauge95() { }

    wxGauge95(wxWindow *parent,
              wxWindowID id,
              int range,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxGA_HORIZONTAL,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxGaugeNameStr)
    {
        (void)Create(parent, id, range, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                int range,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxGA_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxGaugeNameStr);

    // set gauge range/value
    virtual void SetRange(int range);
    virtual void SetValue(int pos);

    // overriden base class virtuals
    virtual bool SetForegroundColour(const wxColour& col);
    virtual bool SetBackgroundColour(const wxColour& col);

protected:
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;
    virtual wxSize DoGetBestSize() const;


    DECLARE_DYNAMIC_CLASS_NO_COPY(wxGauge95)
};

#endif // wxUSE_GAUGE

#endif
    // _GAUGEMSW_H_
