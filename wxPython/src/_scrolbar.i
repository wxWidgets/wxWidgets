/////////////////////////////////////////////////////////////////////////////
// Name:        _scrolbar.i
// Purpose:     SWIG interface defs for wxScrollBar
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(ScrollBarNameStr);

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxScrollBar);

class wxScrollBar : public wxControl {
public:
    %pythonAppend wxScrollBar         "self._setOORInfo(self)"
    %pythonAppend wxScrollBar()       ""

    wxScrollBar(wxWindow* parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyScrollBarNameStr);
    %name(PreScrollBar)wxScrollBar();

    bool Create(wxWindow* parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyScrollBarNameStr);

    virtual int GetThumbPosition() const;
    virtual int GetThumbSize() const;
    %pythoncode { GetThumbLength = GetThumbSize };
    virtual int GetPageSize() const;
    virtual int GetRange() const;

    bool IsVertical() const { return (m_windowStyle & wxVERTICAL) != 0; }

    virtual void SetThumbPosition(int viewStart);
    virtual void SetScrollbar(int position, int thumbSize,
                              int range, int pageSize,
                              bool refresh = true);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};

//---------------------------------------------------------------------------
