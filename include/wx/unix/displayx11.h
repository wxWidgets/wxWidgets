/////////////////////////////////////////////////////////////////////////////
// Name:        displayx11.h
// Purpose:     wxDisplay class for Unix/X11
// Author:      Brian Victor
// Modified by: 
// Created:     12/05/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DISPLAYX11_H_
#define _WX_DISPLAYX11_H_

#if wxUSE_DISPLAY

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "display.h"
#endif

class wxRect;
class wxString;
class wxDisplayUnixPriv;

class WXDLLEXPORT wxDisplay : public wxDisplayBase
{

  public:
    wxDisplay ( size_t index = 0 );

    virtual wxRect GetGeometry() const;
    virtual int GetDepth() const;
    virtual wxString GetName() const;

    ~wxDisplay();

  private:
    wxDisplayUnixPriv *m_priv;

    DECLARE_NO_COPY_CLASS(wxDisplay);
};

#endif // wxUSE_DISPLAY

#endif // _WX_GTK_DISPLAY_H_

