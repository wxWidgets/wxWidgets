///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/toplevel.h
// Purpose:     declares wxTopLevelWindowNative class
// Author:      Peter Most
// Modified by:
// Created:     09.08.09
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TOPLEVEL_H_
#define _WX_QT_TOPLEVEL_H_

class WXDLLIMPEXP_CORE wxTopLevelWindowNative : public wxTopLevelWindowBase
{
public:
    wxTopLevelWindowNative();
    wxTopLevelWindowNative(wxWindow *parent,
               wxWindowID winid,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxFrameNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);
    
    virtual void Maximize(bool maximize = true);
    virtual void Restore();
    virtual void Iconize(bool iconize = true);
    virtual bool IsMaximized() const;
    virtual bool IsIconized() const;

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);
    virtual bool IsFullScreen() const;
    virtual void SetTitle(const wxString& title);
    virtual wxString GetTitle() const;
};
    
#endif // _WX_QT_TOPLEVEL_H_
