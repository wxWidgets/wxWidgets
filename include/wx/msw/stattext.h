/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/stattext.h
// Purpose:     wxStaticText class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATTEXT_H_
#define _WX_STATTEXT_H_

class WXDLLIMPEXP_CORE wxStaticText : public wxStaticTextBase
{
public:
    wxStaticText() { }

    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxStaticTextNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticTextNameStr);

    // override some methods to resize the window properly
    virtual void SetLabel(const wxString& label) wxOVERRIDE;
    virtual bool SetFont( const wxFont &font ) wxOVERRIDE;

    virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = NULL) const wxOVERRIDE;

protected:
    // implement/override some base class virtuals
    virtual void DoSetSize(int x, int y, int w, int h,
                           int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;
    virtual wxSize DoGetBestClientSize() const wxOVERRIDE;

    virtual wxString DoGetLabel() const wxOVERRIDE;
    virtual void DoSetLabel(const wxString& str) wxOVERRIDE;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticText);
};

#endif
    // _WX_STATTEXT_H_
