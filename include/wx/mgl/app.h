/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_APP_H__
#define __WX_APP_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "app.h"
#endif

#include "wx/frame.h"
#include "wx/icon.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxApp;
class WXDLLEXPORT wxLog;
class WXDLLEXPORT wxEventLoop;

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxApp: public wxAppBase
{
public:
    wxApp();
    ~wxApp();

    /* override for altering the way wxGTK intializes the GUI
     * (palette/visual/colorcube). under wxMSW, OnInitGui() does nothing by
     * default. when overriding this method, the code in it is likely to be
     * platform dependent, otherwise use OnInit(). */
    virtual bool OnInitGui();

    // override base class (pure) virtuals
    virtual bool Initialize(int& argc, wxChar **argv);
    virtual void CleanUp();
    
    virtual void Exit();
    virtual void WakeUpIdle();
    virtual bool Yield(bool onlyIfNeeded = FALSE);

    virtual wxVideoMode GetDisplayMode() const { return m_displayMode; }
    virtual bool SetDisplayMode(const wxVideoMode& mode);

private:
    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()

    wxVideoMode m_displayMode;
};

#endif // __WX_APP_H__

