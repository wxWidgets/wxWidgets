///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/widgetpath.h
// Purpose:     wxGtkWidgetPath class declaration
// Author:      Vadim Zeitlin
// Created:     2018-05-31
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_WIDGETPATH_H_
#define _WX_GTK_PRIVATE_WIDGETPATH_H_

// ----------------------------------------------------------------------------
// RAII wrapper calling g_widget_path_unref() automatically
// ----------------------------------------------------------------------------

class wxGtkWidgetPath
{
public:
    wxGtkWidgetPath() : m_path(gtk_widget_path_new()) { }
    ~wxGtkWidgetPath() { gtk_widget_path_unref(m_path); }

    operator GtkWidgetPath*() { return m_path; }
    operator const GtkWidgetPath*() const { return m_path; }

private:
    GtkWidgetPath * const m_path;

    // copying could be implemented by using g_widget_path_ref() but for now
    // there is no need for it so don't implement it
    wxDECLARE_NO_COPY_CLASS(wxGtkWidgetPath);
};

#endif // _WX_GTK_PRIVATE_WIDGETPATH_H_
