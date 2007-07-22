// Copyright (c) 2007 John Wilmes

#include "wx/toplevel.h"
#include "wx/window.h"

wxTopLevelWindowWeb::wxTopLevelWindowWeb() {
}

wxTopLevelWindowWeb::wxTopLevelWindowWeb(wxWindow* parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name) {
    Create(parent, id, title, pos, size, style, name);
}

wxTopLevelWindowWeb::~wxTopLevelWindowWeb() {
}

bool wxTopLevelWindowWeb::Create(wxWindow* parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name) {
    return true;
}

bool wxTopLevelWindowWeb::Show(bool show) {
    return wxWindowWeb::Show(show);
}
