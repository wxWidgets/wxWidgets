///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/log.h
// Purpose:     Support for filtering GTK log messages.
// Author:      Vadim Zeitlin
// Created:     2022-05-11
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_LOG_H_
#define _WX_GTK_PRIVATE_LOG_H_

#include <glib.h>

// Support for custom log writers is only available in glib 2.50 or later.
#if GLIB_CHECK_VERSION(2, 50, 0)
    #define wxHAS_GLIB_LOG_WRITER
#endif

namespace wxGTKImpl
{

#ifdef wxHAS_GLIB_LOG_WRITER

// LogFilter is the base class for filtering GTK log messages
//
// Note that all members of this class are defined in src/gtk/app.cpp.
class LogFilter
{
public:
    LogFilter()
    {
        m_next = NULL;
    }

    // Function to call to install this filter as the active one.
    // Does nothing if run-time glib version is too old.
    void Install();

protected:
    // Function to override in the derived class to actually filter: return
    // true if the message should be suppressed or false if it should be passed
    // through to the default writer (which may, or not, show it).
    virtual bool Filter(GLogLevelFlags log_level,
                        const GLogField* fields,
                        gsize n_fields) const = 0;

private:
    // The function used as glib log writer.
    static GLogWriterOutput
    wx_log_writer(GLogLevelFlags   log_level,
                  const GLogField *fields,
                  gsize            n_fields,
                  gpointer         user_data);

    // False initially, set to true when we install wx_log_writer() as the log
    // writer. Once we do it, we never change it any more.
    static bool ms_installed;

    // We maintain a simple linked list of log filters and this is the head of
    // this list.
    static LogFilter* ms_first;

    // Next entry in the linked list, may be null.
    LogFilter* m_next;

    wxDECLARE_NO_COPY_CLASS(LogFilter);
};

// LogFilterByLevel filters out all the messages at the specified level.
class LogFilterByLevel : public LogFilter
{
public:
    LogFilterByLevel() { }

    void SetLevelToIgnore(int flags)
    {
        m_logLevelToIgnore = flags;
    }

protected:
    bool Filter(GLogLevelFlags log_level,
                const GLogField* WXUNUSED(fields),
                gsize WXUNUSED(n_fields)) const wxOVERRIDE
    {
        return log_level & m_logLevelToIgnore;
    }

private:
    int m_logLevelToIgnore;

    wxDECLARE_NO_COPY_CLASS(LogFilterByLevel);
};

#endif // wxHAS_GLIB_LOG_WRITER

} // namespace wxGTKImpl

#endif // _WX_GTK_PRIVATE_LOG_H_
