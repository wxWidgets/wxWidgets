/////////////////////////////////////////////////////////////////////////////
// Name:        envvars.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_envvars Environment Variables

This section describes all environment variables that affect execution of
wxWidgets programs.

@beginDefList
@itemdef{WXLOG_TIME_FORMAT,
         If set, the contents of this variable is set as the initial timestamp
         used for logging, i.e. passed to wxLog::SetTimestamp(), on program
         startup. For example, this can be used to enable milliseconds in the
         timestamps by setting `WXLOG_TIME_FORMAT=%H:%M:%S.%l` or it could also
         be used to use ISO 8601 timestamp format instead of the default
         locale-dependent format. This variable is only used since wxWidgets
         3.3.0.}
@itemdef{WXTRACE,
        This variable can be set to a comma-separated list of trace masks used in
        wxLogTrace calls; wxLog::AddTraceMask is called for every mask
        in the list during wxWidgets initialization. It only has an effect if
        debug logging is enabled, see wxLogTrace().}
@itemdef{WXPREFIX,
        (Unix only.)
        Overrides installation prefix. Normally, the prefix
        is hard-coded and is the same as the value passed to @c configure via
        the @c \--prefix switch when compiling the library (typically
        @c /usr/local or @c /usr). You can set WXPREFIX if you are for example
        distributing a binary version of an application and you don't know in advance
        where it will be installed.}
@itemdef{WXWEBREQUEST_BACKEND,
         This variable can be set to override the choice of the default backend
         used by wxWebRequest, see wxWebSession::New(). Most common use is to
         set it to @c "CURL" to force using libcurl-based implementation under
         MSW or macOS platforms where the native implementation would be chosen
         by default.}
@itemdef{WXSUPPRESS_SIZER_FLAGS_CHECK,
         If set, disables asserts about using invalid sizer flags in the code.
         This can be helpful when running older programs recompiled with
         wxWidgets 3.1 or later, as these asserts are mostly harmless and can
         be safely ignored if the code works as expected.}
@itemdef{WXSUPPRESS_GTK_DIAGNOSTICS,
         If set to a non-zero value, wxApp::GTKSuppressDiagnostics() is called
         on program startup using the numeric value of this variable or the
         default value if it's not a number, so that e.g. setting it to "yes"
         suppresses all GTK diagnostics while setting it to 16 only suppresses
         GTK warning messages.}
@itemdef{WXLANGUAGE,
         This variable can be set to override OS setting of preferred languages
         and make wxUILocale::GetPreferredUILanguages() return the set list
         instead. The format is a superset of GNU's <a
         href="https://www.gnu.org/software/gettext/manual/html_node/The-LANGUAGE-variable.html">LANGUAGE</a>
         variable: a colon-separated list of language tags (which are, in their
         simplest form, just the language names).}
*/

@see wxSystemOptions
