/////////////////////////////////////////////////////////////////////////////
// Name:        display.h
// Purpose:     wxDisplay class customization for WXMSW
// Author:      Royce Mitchell III
// Modified by: 
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_DISPLAY_H_
#define _WX_MSW_DISPLAY_H_

class WXDLLEXPORT wxDisplay : public wxDisplayBase
{
public:
    wxDisplay ( size_t index = 0 );

    virtual wxRect GetGeometry() const;
    virtual int GetDepth() const;
    virtual wxString GetName() const;

    DECLARE_NO_COPY_CLASS(wxDisplay);
};

#endif // _WX_MSW_DISPLAY_H_
