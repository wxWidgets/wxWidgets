///////////////////////////////////////////////////////////////////////////////
// Name:        common/cmdline.cpp
// Purpose:     wxCmdLineParser implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.01.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "cmdline.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/dynarray.h"
    #include "wx/filefn.h"
#endif //WX_PRECOMP

#include <ctype.h>

#include "wx/datetime.h"
#include "wx/cmdline.h"

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxString GetTypeName(wxCmdLineParamType type);

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// an internal representation of an option
struct wxCmdLineOption
{
    wxCmdLineOption(wxCmdLineEntryType k,
                    const wxString& shrt,
                    const wxString& lng,
                    const wxString& desc,
                    wxCmdLineParamType typ,
                    int fl)
    {
        kind = k;

        shortName = shrt;
        longName = lng;
        description = desc;

        type = typ;
        flags = fl;

        m_hasVal = FALSE;
    }

    // can't use union easily here, so just store all possible data fields, we
    // don't waste much (might still use union later if the number of supported
    // types increases, so always use the accessor functions and don't access
    // the fields directly!)

    void Check(wxCmdLineParamType WXUNUSED_UNLESS_DEBUG(typ)) const
    {
        wxASSERT_MSG( type == typ, _T("type mismatch in wxCmdLineOption") );
    }

    long GetLongVal() const
        { Check(wxCMD_LINE_VAL_NUMBER); return m_longVal; }
    const wxString& GetStrVal() const
        { Check(wxCMD_LINE_VAL_STRING); return m_strVal;  }
    const wxDateTime& GetDateVal() const
        { Check(wxCMD_LINE_VAL_DATE);   return m_dateVal; }

    void SetLongVal(long val)
        { Check(wxCMD_LINE_VAL_NUMBER); m_longVal = val; m_hasVal = TRUE; }
    void SetStrVal(const wxString& val)
        { Check(wxCMD_LINE_VAL_STRING); m_strVal = val; m_hasVal = TRUE; }
    void SetDateVal(const wxDateTime val)
        { Check(wxCMD_LINE_VAL_DATE); m_dateVal = val; m_hasVal = TRUE; }

    void SetHasValue() { m_hasVal = TRUE; }
    bool HasValue() const { return m_hasVal; }

public:
    wxCmdLineEntryType kind;
    wxString shortName, longName, description;
    wxCmdLineParamType type;
    int flags;

private:
    bool m_hasVal;

    long m_longVal;
    wxString m_strVal;
    wxDateTime m_dateVal;
};

struct wxCmdLineParam
{
    wxCmdLineParam(const wxString& desc,
                   wxCmdLineParamType typ,
                   int fl)
        : description(desc)
    {
        type = typ;
        flags = fl;
    }

    wxString description;
    wxCmdLineParamType type;
    int flags;
};

WX_DECLARE_OBJARRAY(wxCmdLineOption, wxArrayOptions);
WX_DECLARE_OBJARRAY(wxCmdLineParam, wxArrayParams);

#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(wxArrayOptions);
WX_DEFINE_OBJARRAY(wxArrayParams);

// the parser internal state
struct wxCmdLineParserData
{
    // options
    wxString m_switchChars;     // characters which may start an option
    bool m_enableLongOptions;   // TRUE if long options are enabled
    wxString m_logo;            // some extra text to show in Usage()

    // cmd line data
    wxArrayString m_arguments;  // == argv, argc == m_arguments.GetCount()
    wxArrayOptions m_options;   // all possible options and switchrs
    wxArrayParams m_paramDesc;  // description of all possible params
    wxArrayString m_parameters; // all params found

    // methods
    wxCmdLineParserData();
    void SetArguments(int argc, char **argv);
    void SetArguments(const wxString& cmdline);

    int FindOption(const wxString& name);
    int FindOptionByLongName(const wxString& name);
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCmdLineParserData
// ----------------------------------------------------------------------------

wxCmdLineParserData::wxCmdLineParserData()
{
    m_enableLongOptions = TRUE;
#ifdef __UNIX_LIKE__
    m_switchChars = _T("-");
#else // !Unix
    m_switchChars = _T("/-");
#endif
}

void wxCmdLineParserData::SetArguments(int argc, char **argv)
{
    m_arguments.Empty();

    for ( int n = 0; n < argc; n++ )
    {
        m_arguments.Add(argv[n]);
    }
}

void wxCmdLineParserData::SetArguments(const wxString& WXUNUSED(cmdline))
{
    // either use wxMSW wxApp::ConvertToStandardCommandArgs() or move its logic
    // here and use this method from it - but don't duplicate the code

    wxFAIL_MSG(_T("TODO"));
}

int wxCmdLineParserData::FindOption(const wxString& name)
{
    size_t count = m_options.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_options[n].shortName == name )
        {
            // found
            return n;
        }
    }

    return wxNOT_FOUND;
}

int wxCmdLineParserData::FindOptionByLongName(const wxString& name)
{
    size_t count = m_options.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_options[n].longName == name )
        {
            // found
            return n;
        }
    }

    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// construction and destruction
// ----------------------------------------------------------------------------

void wxCmdLineParser::Init()
{
    m_data = new wxCmdLineParserData;
}

void wxCmdLineParser::SetCmdLine(int argc, char **argv)
{
    m_data->SetArguments(argc, argv);
}

void wxCmdLineParser::SetCmdLine(const wxString& cmdline)
{
    m_data->SetArguments(cmdline);
}

wxCmdLineParser::~wxCmdLineParser()
{
    delete m_data;
}

// ----------------------------------------------------------------------------
// options
// ----------------------------------------------------------------------------

void wxCmdLineParser::SetSwitchChars(const wxString& switchChars)
{
    m_data->m_switchChars = switchChars;
}

void wxCmdLineParser::EnableLongOptions(bool enable)
{
    m_data->m_enableLongOptions = enable;
}

void wxCmdLineParser::SetLogo(const wxString& logo)
{
    m_data->m_logo = logo;
}

// ----------------------------------------------------------------------------
// command line construction
// ----------------------------------------------------------------------------

void wxCmdLineParser::SetDesc(const wxCmdLineEntryDesc *desc)
{
    for ( ;; desc++ )
    {
        switch ( desc->kind )
        {
            case wxCMD_LINE_SWITCH:
                AddSwitch(desc->shortName, desc->longName, desc->description,
                          desc->flags);
                break;

            case wxCMD_LINE_OPTION:
                AddOption(desc->shortName, desc->longName, desc->description,
                          desc->type, desc->flags);
                break;

            case wxCMD_LINE_PARAM:
                AddParam(desc->description, desc->type, desc->flags);
                break;

            default:
                wxFAIL_MSG( _T("unknown command line entry type") );
                // still fall through

            case wxCMD_LINE_NONE:
                return;
        }
    }
}

void wxCmdLineParser::AddSwitch(const wxString& shortName,
                                const wxString& longName,
                                const wxString& desc,
                                int flags)
{
    wxASSERT_MSG( m_data->FindOption(shortName) == wxNOT_FOUND,
                  _T("duplicate switch") );

    wxCmdLineOption *option = new wxCmdLineOption(wxCMD_LINE_SWITCH,
                                                  shortName, longName, desc,
                                                  wxCMD_LINE_VAL_NONE, flags);

    m_data->m_options.Add(option);
}

void wxCmdLineParser::AddOption(const wxString& shortName,
                                const wxString& longName,
                                const wxString& desc,
                                wxCmdLineParamType type,
                                int flags)
{
    wxASSERT_MSG( m_data->FindOption(shortName) == wxNOT_FOUND,
                  _T("duplicate option") );

    wxCmdLineOption *option = new wxCmdLineOption(wxCMD_LINE_OPTION,
                                                  shortName, longName, desc,
                                                  type, flags);

    m_data->m_options.Add(option);
}

void wxCmdLineParser::AddParam(const wxString& desc,
                               wxCmdLineParamType type,
                               int flags)
{
    // do some consistency checks: a required parameter can't follow an
    // optional one and nothing should follow a parameter with MULTIPLE flag
#ifdef __WXDEBUG__
    if ( !m_data->m_paramDesc.IsEmpty() )
    {
        wxCmdLineParam& param = m_data->m_paramDesc.Last();

        wxASSERT_MSG( !(param.flags & wxCMD_LINE_PARAM_MULTIPLE),
                      _T("all parameters after the one with wxCMD_LINE_PARAM_MULTIPLE style will be ignored") );

        if ( !(flags & wxCMD_LINE_PARAM_OPTIONAL) )
        {
            wxASSERT_MSG( !(param.flags & wxCMD_LINE_PARAM_OPTIONAL),
                          _T("a required parameter can't follow an optional one") );
        }
    }
#endif // Debug

    wxCmdLineParam *param = new wxCmdLineParam(desc, type, flags);

    m_data->m_paramDesc.Add(param);
}

// ----------------------------------------------------------------------------
// access to parse command line
// ----------------------------------------------------------------------------

bool wxCmdLineParser::Found(const wxString& name) const
{
    int i = m_data->FindOption(name);
    wxCHECK_MSG( i != wxNOT_FOUND, FALSE, _T("unknown switch") );

    wxCmdLineOption& opt = m_data->m_options[(size_t)i];
    if ( !opt.HasValue() )
        return FALSE;

    return TRUE;
}

bool wxCmdLineParser::Found(const wxString& name, wxString *value) const
{
    int i = m_data->FindOption(name);
    wxCHECK_MSG( i != wxNOT_FOUND, FALSE, _T("unknown option") );

    wxCmdLineOption& opt = m_data->m_options[(size_t)i];
    if ( !opt.HasValue() )
        return FALSE;

    wxCHECK_MSG( value, FALSE, _T("NULL pointer in wxCmdLineOption::Found") );

    *value = opt.GetStrVal();

    return TRUE;
}

bool wxCmdLineParser::Found(const wxString& name, long *value) const
{
    int i = m_data->FindOption(name);
    wxCHECK_MSG( i != wxNOT_FOUND, FALSE, _T("unknown option") );

    wxCmdLineOption& opt = m_data->m_options[(size_t)i];
    if ( !opt.HasValue() )
        return FALSE;

    wxCHECK_MSG( value, FALSE, _T("NULL pointer in wxCmdLineOption::Found") );

    *value = opt.GetLongVal();

    return TRUE;
}

bool wxCmdLineParser::Found(const wxString& name, wxDateTime *value) const
{
    int i = m_data->FindOption(name);
    wxCHECK_MSG( i != wxNOT_FOUND, FALSE, _T("unknown option") );

    wxCmdLineOption& opt = m_data->m_options[(size_t)i];
    if ( !opt.HasValue() )
        return FALSE;

    wxCHECK_MSG( value, FALSE, _T("NULL pointer in wxCmdLineOption::Found") );

    *value = opt.GetDateVal();

    return TRUE;
}

size_t wxCmdLineParser::GetParamCount() const
{
    return m_data->m_parameters.GetCount();
}

wxString wxCmdLineParser::GetParam(size_t n) const
{
    return m_data->m_parameters[n];
}

// ----------------------------------------------------------------------------
// the real work is done here
// ----------------------------------------------------------------------------

int wxCmdLineParser::Parse()
{
    bool maybeOption = TRUE;    // can the following arg be an option?
    bool ok = TRUE;             // TRUE until an error is detected
    bool helpRequested = FALSE; // TRUE if "-h" was given
    bool hadRepeatableParam = FALSE; // TRUE if found param with MULTIPLE flag

    size_t currentParam = 0;    // the index in m_paramDesc

    size_t countParam = m_data->m_paramDesc.GetCount();

    // parse everything
    wxString arg;
    size_t count = m_data->m_arguments.GetCount();
    for ( size_t n = 1; ok && (n < count); n++ )    // 0 is program name
    {
        arg = m_data->m_arguments[n];

        // special case: "--" should be discarded and all following arguments
        // should be considered as parameters, even if they start with '-' and
        // not like options (this is POSIX-like)
        if ( arg == _T("--") )
        {
            maybeOption = FALSE;

            continue;
        }

        // empty argument or just '-' is not an option but a parameter
        if ( maybeOption && arg.length() > 1 &&
                wxStrchr(m_data->m_switchChars, arg[0u]) )
        {
            bool isLong;
            wxString name;
            int optInd = wxNOT_FOUND;   // init to suppress warnings

            // an option or a switch: find whether it's a long or a short one
            if ( m_data->m_enableLongOptions &&
                    arg[0u] == _T('-') && arg[1u] == _T('-') )
            {
                // a long one
                isLong = TRUE;

                const wxChar *p = arg.c_str() + 2;
                while ( wxIsalnum(*p) || (*p == _T('_')) || (*p == _T('-')) )
                {
                    name += *p++;
                }

                optInd = m_data->FindOptionByLongName(name);
                if ( optInd == wxNOT_FOUND )
                {
                    wxLogError(_("Unknown long option '%s'"), name.c_str());
                }
            }
            else
            {
                isLong = FALSE;

                // a short one: as they can be cumulated, we try to find the
                // longest substring which is a valid option
                const wxChar *p = arg.c_str() + 1;
                while ( wxIsalnum(*p) || (*p == _T('_')) )
                {
                    name += *p++;
                }

                size_t len = name.length();
                do
                {
                    if ( len == 0 )
                    {
                        // we couldn't find a valid option name in the
                        // beginning of this string
                        wxLogError(_("Unknown option '%s'"), name.c_str());

                        break;
                    }
                    else
                    {
                        optInd = m_data->FindOption(name.Left(len));

                        // will try with one character less the next time
                        len--;
                    }
                }
                while ( optInd == wxNOT_FOUND );

                len++;  // compensates extra len-- above
                if ( (optInd != wxNOT_FOUND) && (len != name.length()) )
                {
                    // first of all, the option name is only part of this
                    // string
                    name = name.Left(len);

                    // our option is only part of this argument, there is
                    // something else in it - it is either the value of this
                    // option or other switches if it is a switch
                    if ( m_data->m_options[(size_t)optInd].kind
                            == wxCMD_LINE_SWITCH )
                    {
                        // pretend that all the rest of the argument is the
                        // next argument, in fact
                        wxString arg2 = arg[0u];
                        arg2 += arg.Mid(len + 1); // +1 for leading '-'

                        m_data->m_arguments.Insert(arg2, n + 1);
                        count++;
                    }
                    //else: it's our value, we'll deal with it below
                }
            }

            if ( optInd == wxNOT_FOUND )
            {
                ok = FALSE;

                continue;   // will break, in fact
            }

            wxCmdLineOption& opt = m_data->m_options[(size_t)optInd];
            if ( opt.kind == wxCMD_LINE_SWITCH )
            {
                // nothing more to do
                opt.SetHasValue();

                if ( opt.flags & wxCMD_LINE_OPTION_HELP )
                {
                    helpRequested = TRUE;

                    // it's not an error, but we still stop here
                    ok = FALSE;
                }
            }
            else
            {
                // get the value

                // +1 for leading '-'
                const wxChar *p = arg.c_str() + 1 + name.length();
                if ( isLong )
                {
                    p++;    // for another leading '-'

                    if ( *p++ != _T('=') )
                    {
                        wxLogError(_("Option '%s' requires a value, '=' expected."), name.c_str());

                        ok = FALSE;
                    }
                }
                else
                {
                    switch ( *p )
                    {
                        case _T('='):
                        case _T(':'):
                            // the value follows
                            p++;
                            break;

                        case 0:
                            // the value is in the next argument
                            if ( ++n == count )
                            {
                                // ... but there is none
                                wxLogError(_("Option '%s' requires a value."),
                                           name.c_str());

                                ok = FALSE;
                            }
                            else
                            {
                                // ... take it from there
                                p = m_data->m_arguments[n].c_str();
                            }
                            break;

                        default:
                            // the value is right here: this may be legal or
                            // not depending on the option style
                            if ( opt.flags & wxCMD_LINE_NEEDS_SEPARATOR )
                            {
                                wxLogError(_("Separator expected after the option '%s'."),
                                           name.c_str());

                                ok = FALSE;
                            }
                    }
                }

                if ( ok )
                {
                    wxString value = p;
                    switch ( opt.type )
                    {
                        default:
                            wxFAIL_MSG( _T("unknown option type") );
                            // still fall through

                        case wxCMD_LINE_VAL_STRING:
                            opt.SetStrVal(value);
                            break;

                        case wxCMD_LINE_VAL_NUMBER:
                            {
                                long val;
                                if ( value.ToLong(&val) )
                                {
                                    opt.SetLongVal(val);
                                }
                                else
                                {
                                    wxLogError(_("'%s' is not a correct numeric value for option '%s'."),
                                               value.c_str(), name.c_str());

                                    ok = FALSE;
                                }
                            }
                            break;

                        case wxCMD_LINE_VAL_DATE:
                            {
                                wxDateTime dt;
                                const wxChar *res = dt.ParseDate(value);
                                if ( !res || *res )
                                {
                                    wxLogError(_("Option '%s': '%s' cannot be converted to a date."),
                                               name.c_str(), value.c_str());

                                    ok = FALSE;
                                }
                                else
                                {
                                    opt.SetDateVal(dt);
                                }
                            }
                            break;
                    }
                }
            }
        }
        else
        {
            // a parameter
            if ( currentParam < countParam )
            {
                wxCmdLineParam& param = m_data->m_paramDesc[currentParam];

                // TODO check the param type

                m_data->m_parameters.Add(arg);

                if ( !(param.flags & wxCMD_LINE_PARAM_MULTIPLE) )
                {
                    currentParam++;
                }
                else
                {
                    wxASSERT_MSG( currentParam == countParam - 1,
                                  _T("all parameters after the one with wxCMD_LINE_PARAM_MULTIPLE style are ignored") );

                    // remember that we did have this last repeatable parameter
                    hadRepeatableParam = TRUE;
                }
            }
            else
            {
                wxLogError(_("Unexpected parameter '%s'"), arg.c_str());

                ok = FALSE;
            }
        }
    }

    // verify that all mandatory options were given
    if ( ok )
    {
        size_t countOpt = m_data->m_options.GetCount();
        for ( size_t n = 0; ok && (n < countOpt); n++ )
        {
            wxCmdLineOption& opt = m_data->m_options[n];
            if ( (opt.flags & wxCMD_LINE_OPTION_MANDATORY) && !opt.HasValue() )
            {
                wxString optName;
                if ( !opt.longName )
                {
                    optName = opt.shortName;
                }
                else
                {
                    optName.Printf(_("%s (or %s)"),
                                   opt.shortName.c_str(),
                                   opt.longName.c_str());
                }

                wxLogError(_("The value for the option '%s' must be specified."),
                           optName.c_str());

                ok = FALSE;
            }
        }

        for ( ; ok && (currentParam < countParam); currentParam++ )
        {
            wxCmdLineParam& param = m_data->m_paramDesc[currentParam];
            if ( (currentParam == countParam - 1) &&
                 (param.flags & wxCMD_LINE_PARAM_MULTIPLE) &&
                 hadRepeatableParam )
            {
                // special case: currentParam wasn't incremented, but we did
                // have it, so don't give error
                continue;
            }

            if ( !(param.flags & wxCMD_LINE_PARAM_OPTIONAL) )
            {
                wxLogError(_("The required parameter '%s' was not specified."),
                           param.description.c_str());

                ok = FALSE;
            }
        }
    }

    if ( !ok )
    {
        Usage();
    }

    return ok ? 0 : helpRequested ? -1 : 1;
}

// ----------------------------------------------------------------------------
// give the usage message
// ----------------------------------------------------------------------------

void wxCmdLineParser::Usage()
{
    wxString appname = wxTheApp->GetAppName();
    if ( !appname )
    {
        wxCHECK_RET( !m_data->m_arguments.IsEmpty(), _T("no program name") );

        appname = wxFileNameFromPath(m_data->m_arguments[0]);
        wxStripExtension(appname);
    }

    // we construct the brief cmd line desc on the fly, but not the detailed
    // help message below because we want to align the options descriptions
    // and for this we must first know the longest one of them
    wxString brief;
    wxArrayString namesOptions, descOptions;
    brief.Printf(_("Usage: %s"), appname.c_str());

    // the switch char is usually '-' but this can be changed with
    // SetSwitchChars() and then the first one of possible chars is used
    wxChar chSwitch = !m_data->m_switchChars ? _T('-')
                                             : m_data->m_switchChars[0u];

    size_t n, count = m_data->m_options.GetCount();
    for ( n = 0; n < count; n++ )
    {
        wxCmdLineOption& opt = m_data->m_options[n];

        brief << _T(' ');
        if ( !(opt.flags & wxCMD_LINE_OPTION_MANDATORY) )
        {
            brief << _T('[');
        }

        brief << chSwitch << opt.shortName;

        wxString option;
        option << _T("  ") << chSwitch << opt.shortName;
        if ( !!opt.longName )
        {
            option << _T("  --") << opt.longName;
        }

        if ( opt.kind != wxCMD_LINE_SWITCH )
        {
            wxString val;
            val << _T('<') << GetTypeName(opt.type) << _T('>');
            brief << _T(' ') << val;
            option << (!opt.longName ? _T(':') : _T('=')) << val;
        }

        if ( !(opt.flags & wxCMD_LINE_OPTION_MANDATORY) )
        {
            brief << _T(']');
        }

        namesOptions.Add(option);
        descOptions.Add(opt.description);
    }

    count = m_data->m_paramDesc.GetCount();
    for ( n = 0; n < count; n++ )
    {
        wxCmdLineParam& param = m_data->m_paramDesc[n];

        brief << _T(' ');
        if ( param.flags & wxCMD_LINE_PARAM_OPTIONAL )
        {
            brief << _T('[');
        }

        brief << param.description;

        if ( param.flags & wxCMD_LINE_PARAM_MULTIPLE )
        {
            brief << _T("...");
        }

        if ( param.flags & wxCMD_LINE_PARAM_OPTIONAL )
        {
            brief << _T(']');
        }
    }

    if ( !!m_data->m_logo )
    {
        wxLogMessage(m_data->m_logo);
    }

    wxLogMessage(brief);

    // now construct the detailed help message
    size_t len, lenMax = 0;
    count = namesOptions.GetCount();
    for ( n = 0; n < count; n++ )
    {
        len = namesOptions[n].length();
        if ( len > lenMax )
            lenMax = len;
    }

    wxString detailed;
    for ( n = 0; n < count; n++ )
    {
        len = namesOptions[n].length();
        detailed << namesOptions[n]
                 << wxString(_T(' '), lenMax - len) << _T('\t')
                 << descOptions[n]
                 << _T('\n');
    }

    wxLogMessage(detailed);
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

static wxString GetTypeName(wxCmdLineParamType type)
{
    wxString s;
    switch ( type )
    {
        default:
            wxFAIL_MSG( _T("unknown option type") );
            // still fall through

        case wxCMD_LINE_VAL_STRING: s = _("str"); break;
        case wxCMD_LINE_VAL_NUMBER: s = _("num"); break;
        case wxCMD_LINE_VAL_DATE:   s = _("date"); break;
    }

    return s;
}
