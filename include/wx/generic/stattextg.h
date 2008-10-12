/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/stattextg.h
// Purpose:     wxGenericStaticText header
// Author:      Marcin Wojdyr
// Created:     2008-06-26
// Id:          $Id:$
// Copyright:   Marcin Wojdyr
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_STATTEXTG_H_
#define _WX_GENERIC_STATTEXTG_H_

#include "wx/stattext.h"

class WXDLLIMPEXP_CORE wxGenericStaticText : public wxStaticTextBase
{
public:
    wxGenericStaticText() { }

    wxGenericStaticText(wxWindow *parent,
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


    virtual wxSize DoGetBestClientSize() const;
    virtual wxSize DoGetBestSize() const;

    virtual void SetLabel(const wxString& label);
    virtual bool SetFont(const wxFont &font);

    virtual wxString DoGetLabel() const { return m_label; }
    virtual void DoSetLabel(const wxString& label);

protected:
    void OnPaint(wxPaintEvent& event);
    void DoSetSize(int x, int y, int width, int height, int sizeFlags);

private:
    wxString m_label;
    int m_mnemonic;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxGenericStaticText)
};

#endif // _WX_GENERIC_STATTEXTG_H_

