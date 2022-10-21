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
        m_next = nullptr;
    }

    // Allow installing our own log writer function, we don't do it by default
    // because this results in a fatal error if the application had already
    // called g_log_set_writer_func() on its own.
    static void Allow() { ms_allowed = true; }

    // Function to call to install this filter as the active one if we're
    // allowed to do this, i.e. if Allow() had been called before.
    //
    // Does nothing and just returns false if run-time glib version is too old.
    bool Install();

protected:
    // Function to override in the derived class to actually filter: return
    // true if the message should be suppressed or false if it should be passed
    // through to the default writer (which may, or not, show it).
    virtual bool Filter(GLogLevelFlags log_level,
                        const GLogField* fields,
                        gsize n_fields) const = 0;

    // Typically called from the derived class dtor to stop using this filter.
    void Uninstall();

private:
    // The function used as glib log writer.
    static GLogWriterOutput
    wx_log_writer(GLogLevelFlags   log_level,
                  const GLogField *fields,
                  gsize            n_fields,
                  gpointer         user_data);

    // False initially, indicating that we're not allowed to install our own
    // logging function.
    static bool ms_allowed;

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
                gsize WXUNUSED(n_fields)) const override
    {
        return log_level & m_logLevelToIgnore;
    }

private:
    int m_logLevelToIgnore;

    wxDECLARE_NO_COPY_CLASS(LogFilterByLevel);
};

// LogFilterByMessage filters out all the messages with the specified content.
class LogFilterByMessage : public LogFilter
{
public:
    // Objects of this class are supposed to be created with literal strings as
    // argument, so don't bother copying the string but just use the pointer.
    explicit LogFilterByMessage(const char* message)
        : m_message(message)
    {
        // We shouldn't warn about anything if Install() failed.
        m_warnNotFiltered = Install();
    }

    // Remove this filter when the object goes out of scope.
    //
    // The dtor also checks if we actually filtered the message and logs a
    // trace message with the "gtklog" mask if we didn't: this allows checking
    // if the filter is actually being used.
    ~LogFilterByMessage();

protected:
    bool Filter(GLogLevelFlags WXUNUSED(log_level),
                const GLogField* fields,
                gsize n_fields) const override;

private:
    const char* const m_message;

    mutable bool m_warnNotFiltered;

    wxDECLARE_NO_COPY_CLASS(LogFilterByMessage);
};

#else // !wxHAS_GLIB_LOG_WRITER

// Provide stubs to avoid having to use preprocessor checks in the code using
// these classes.
class LogFilterByMessage
{
public:
    explicit LogFilterByMessage(const char* WXUNUSED(message)) { }
};

#endif // wxHAS_GLIB_LOG_WRITER/!wxHAS_GLIB_LOG_WRITER

} // namespace wxGTKImpl

#endif // _WX_GTK_PRIVATE_LOG_H_
