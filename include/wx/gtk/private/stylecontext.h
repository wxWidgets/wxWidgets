///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/stylecontext.h
// Purpose:     GtkStyleContext helper class
// Author:      Paul Cornett
// Created:     2018-06-04
// Copyright:   (c) 2018 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_STYLECONTEXT_H_
#define _WX_GTK_PRIVATE_STYLECONTEXT_H_

#ifdef __WXGTK3__

class wxGtkStyleContext
{
public:
    explicit wxGtkStyleContext(double scale = 1);
    ~wxGtkStyleContext();
    wxGtkStyleContext& Add(GType type, const char* objectName, ...) G_GNUC_NULL_TERMINATED;
    wxGtkStyleContext& Add(const char* objectName);
    wxGtkStyleContext& AddButton();
    wxGtkStyleContext& AddCheckButton();
    wxGtkStyleContext& AddHeaderbar();
    wxGtkStyleContext& AddLabel();
    wxGtkStyleContext& AddMenu();
    wxGtkStyleContext& AddMenuItem();
    wxGtkStyleContext& AddTextview(const char* child1 = NULL, const char* child2 = NULL);
    wxGtkStyleContext& AddTooltip();
    wxGtkStyleContext& AddTreeview();
#if GTK_CHECK_VERSION(3,20,0)
    wxGtkStyleContext& AddTreeviewHeaderButton(int pos);
#endif // GTK >= 3.20
    wxGtkStyleContext& AddWindow(const char* className2 = NULL);
    void Bg(wxColour& color, int state = GTK_STATE_FLAG_NORMAL) const;
    void Fg(wxColour& color, int state = GTK_STATE_FLAG_NORMAL) const;
    void Border(wxColour& color) const;
    operator GtkStyleContext*() { return m_context; }

private:
    GtkStyleContext* m_context;
    GtkWidgetPath* const m_path;
    const int m_scale;

    wxDECLARE_NO_COPY_CLASS(wxGtkStyleContext);
};

#endif // __WXGTK3__
#endif // _WX_GTK_PRIVATE_STYLECONTEXT_H_
