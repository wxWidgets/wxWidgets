/////////////////////////////////////////////////////////////////////////////
// Name:        wx/toplevel.h
// Purpose:     Top level window, abstraction of wxFrame and wxDialog
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_UNIV_TOPLEVEL_H__
#define __WX_UNIV_TOPLEVEL_H__

#ifdef __GNUG__
    #pragma interface "univtoplevel.h"
#endif


//-----------------------------------------------------------------------------
// wxTopLevelWindow
//-----------------------------------------------------------------------------

class wxTopLevelWindow : public wxTopLevelWindowNative
{
public:
    // construction
    wxTopLevelWindow() { Init(); }
    wxTopLevelWindow(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = wxFrameNameStr)
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    // implement base class pure virtuals
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);

    // implementation from now on
    // --------------------------

    DECLARE_DYNAMIC_CLASS(wxTopLevelWindow)

protected:
    // common part of all ctors
    void Init();
};

#endif // __WX_UNIV_TOPLEVEL_H__
