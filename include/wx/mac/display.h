/////////////////////////////////////////////////////////////////////////////
// Name:        display.h
// Purpose:     wxDisplay class customization for Mac
// Author:      Brian Victor
// Modified by: Royce Mitchell III
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_DISPLAY_H_
#define _WX_MAC_DISPLAY_H_

#include "wx/object.h"
#include "wx/display.h"

class wxDisplayMacPriv;
class wxRect;
class wxString;

class WXDLLEXPORT wxDisplay : public wxDisplayBase
{
public:
    wxDisplay ( size_t index = 0 );

    virtual wxRect GetGeometry() const;
    virtual int GetDepth() const;
    virtual wxString GetName() const;


    ~wxDisplay();

private:
    wxDisplayMacPriv* m_priv;

    DECLARE_NO_COPY_CLASS(wxDisplay);
};

#endif // _WX_MAC_DISPLAY_H_
