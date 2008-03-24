/////////////////////////////////////////////////////////////////////////////
// Name:        ifacecheck.cpp
// Purpose:     Interface headers <=> real headers coherence checker
// Author:      Francesco Montorsi
// Created:     2008/03/17
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/cmdline.h"
#include "wx/textfile.h"
#include "wx/stopwatch.h"       // for wxGetLocalTime
#include "xmlparser.h"

// global verbosity flag
bool g_verbose = false;



// ----------------------------------------------------------------------------
// IfaceCheckApp
// ----------------------------------------------------------------------------

#define API_DUMP_FILE           "dump.api.txt"
#define INTERFACE_DUMP_FILE     "dump.interface.txt"

#define MODIFY_SWITCH           "m"
#define DUMP_SWITCH             "dump"
#define HELP_SWITCH             "h"
#define VERBOSE_SWITCH          "v"

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, MODIFY_SWITCH, "modify",
        "modify the interface headers to match the real ones" },
    { wxCMD_LINE_SWITCH, "", DUMP_SWITCH,
        "dump both interface and API to plain text" },
    { wxCMD_LINE_SWITCH, HELP_SWITCH, "help",
        "show help message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, VERBOSE_SWITCH, "verbose",
        "be verbose" },
    { wxCMD_LINE_PARAM,  NULL, NULL,
        "gccXML", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
    { wxCMD_LINE_PARAM,  NULL, NULL,
        "doxygenXML", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
    wxCMD_LINE_DESC_END
};

class IfaceCheckApp : public wxAppConsole
{
public:
    // don't use builtin cmd line parsing:
    virtual bool OnInit() { m_modify=false; return true; }
    virtual int OnRun();

    bool Compare();
    int CompareClasses(const wxClass* iface, const wxClassPtrArray& api);
    void FixMethod(const wxString& header, const wxMethod* iface, const wxMethod* api);

    void ShowProgress();
    void PrintStatistics(long secs);

protected:
    wxXmlGccInterface m_api;                  // "real" headers API
    wxXmlDoxygenInterface m_interface;        // doxygen-commented headers API

    // was the MODIFY_SWITCH passed?
    bool m_modify;
};

IMPLEMENT_APP_CONSOLE(IfaceCheckApp)

int IfaceCheckApp::OnRun()
{
    long startTime = wxGetLocalTime();      // for timing purpose

    // parse the command line...
    wxCmdLineParser parser(g_cmdLineDesc, argc, argv);
    bool ok = true;
    switch (parser.Parse())
    {
        case -1:
            // HELP_SWITCH was passed
            return 0;

        case 0:
            if (parser.Found(VERBOSE_SWITCH))
                g_verbose = true;

            if (!m_api.Parse(parser.GetParam(0)) ||
                !m_interface.Parse(parser.GetParam(1)))
                return 1;

            if (parser.Found(DUMP_SWITCH))
            {
                LogMessage("Dumping real API to '%s'...", API_DUMP_FILE);
                m_api.Dump(API_DUMP_FILE);

                LogMessage("Dumping interface API to '%s'...", INTERFACE_DUMP_FILE);
                m_interface.Dump(INTERFACE_DUMP_FILE);
            }
            else
            {
                if (parser.Found(MODIFY_SWITCH))
                    m_modify = true;

                ok = Compare();
            }

            PrintStatistics(wxGetLocalTime() - startTime);
            return ok ? 0 : 1;
    }

    return 1;
}

void IfaceCheckApp::ShowProgress()
{
    wxPrint(".");
    //fflush(stdout);
}

bool IfaceCheckApp::Compare()
{
    const wxClassArray& interface = m_interface.GetClasses();
    const wxClass* c;
    wxClassPtrArray api;
    int mcount = 0, ccount = 0;

    LogMessage("Comparing the interface API to the real API (%d classes to compare)...",
               interface.GetCount());

    for (unsigned int i=0; i<interface.GetCount(); i++)
    {
        wxString cname = interface[i].GetName();

        api.Empty();

        // search in the real headers for i-th interface class
        // for both class cname and cnameBase as in wxWidgets world, most often
        // class cname is platform-specific while the real public interface of
        // that class is part of the cnameBase class.
        c = m_api.FindClass(cname);
        if (c) api.Add(c);
        c = m_api.FindClass(cname + "Base");
        if (c) api.Add(c);

        if (api.GetCount()>0) {

            // there is a class with exactly the same name!
            mcount += CompareClasses(&interface[i], api);

        } else {

            // shorten the name of the header so the log file is more readable
            wxString header = interface[i].GetHeader().AfterLast('/');

            LogMessage("%s: couldn't find the real interface for the '%s' class",
                       header, cname);
            ccount++;
        }
    }

    LogMessage("%d methods (%.1f%%) of the interface headers do not exist in the real headers",
               mcount, (float)(100.0 * mcount/m_interface.GetMethodCount()));
    LogMessage("%d classes (%.1f%%) of the interface headers do not exist in the real headers",
               ccount, (float)(100.0 * ccount/m_interface.GetClassesCount()));

    return true;
}

int IfaceCheckApp::CompareClasses(const wxClass* iface, const wxClassPtrArray& api)
{
    wxString searchedclasses;
    const wxMethod *real;
    int count = 0;

    wxASSERT(iface && api.GetCount()>0);

    // build a string with the names of the API classes compared to iface
    for (unsigned int j=0; j<api.GetCount(); j++)
        searchedclasses += "/" + api[j]->GetName();
    searchedclasses.Remove(0, 1);

    // shorten the name of the header so the log file is more readable
    wxString header = iface->GetHeader().AfterLast('/');

    for (unsigned int i=0; i<iface->GetMethodCount(); i++)
    {
        const wxMethod& m = iface->GetMethod(i);
        int matches = 0;

        // search in the methods of the api classes provided
        for (unsigned int j=0; j<api.GetCount(); j++)
        {
            real = api[j]->FindMethod(m);
            if (real)
                matches++;                // there is a real matching prototype! It's ok!
        }

        if (matches == 0)
        {
            wxMethodPtrArray overloads;

            // try searching for a method with the same name but with
            // different return type / arguments / qualifiers
            for (unsigned int j=0; j<api.GetCount(); j++)
            {
                wxMethodPtrArray results = api[j]->FindMethodNamed(m.GetName());

                // append "results" array to "overloads"
                WX_APPEND_ARRAY(overloads, results);
            }

            if (overloads.GetCount()==0)
            {
                LogMessage("%s: real '%s' class has no method '%s'",
                            header, searchedclasses, m.GetAsString());
                // we've found no overloads
            }
            else
            {
                // first, output a warning
                wxString warning = header;
                if (overloads.GetCount()>1)
                    warning += wxString::Format(": in the real headers there are %d overloads of '%s' for "
                                                "'%s' all with different signatures:\n",
                                                overloads.GetCount(), m.GetName(), searchedclasses);
                else
                    warning += wxString::Format(": in the real headers there is a method '%s' for '%s'"
                                                " but has different signature:\n",
                                                m.GetName(), searchedclasses);

                warning += "\tdoxy header: " + m.GetAsString();
                for (unsigned int j=0; j<overloads.GetCount(); j++)
                    warning += "\n\treal header: " + overloads[j]->GetAsString();

                wxPrint(warning + "\n");
                count++;

                if (overloads.GetCount()>1)
                {
                    // TODO: decide which of these overloads is the most "similar" to m
                    //       and eventually modify it
                    if (m_modify)
                        wxPrint("\tmanual fix is required\n");
                }
                else
                {
                    wxASSERT(overloads.GetCount() == 1);

                    if (m_modify)
                    {
                        wxPrint("\tfixing it...\n");

                        // try to modify it!
                        FixMethod(iface->GetHeader(), &m, overloads[0]);
                    }
                }
            }

            count++;
        }
    }

    return count;
}

void IfaceCheckApp::FixMethod(const wxString& header, const wxMethod* iface, const wxMethod* api)
{
    wxASSERT(iface && api);

    wxTextFile file;
    if (!file.Open(header)) {
        LogError("can't open the '%s' header file.", header);
        return;
    }

    // GetLocation() returns the line where the last part of the prototype is placed:
    int end = iface->GetLocation()-1;
    if (end <= 0 || end >= (int)file.GetLineCount()) {
        LogWarning("invalid location info for method '%s': %d.",
                   iface->GetAsString(), iface->GetLocation());
        return;
    }

    if (!file.GetLine(end).Contains(iface->GetName())) {
        LogWarning("invalid location info for method '%s': %d.",
                   iface->GetAsString(), iface->GetLocation());
        return;
    }

    // find the start point of this prototype declaration:
    int start = end-1;
    while (start > 0 &&
           !file.GetLine(start).Contains(";") &&
           !file.GetLine(start).Contains("*/"))
        start--;

    if (start <= 0)
    {
        LogError("can't find the beginning of the declaration of '%s' method in '%s' header",
                    iface->GetAsString(), header);
        return;
    }

    // start-th line contains either the declaration of another prototype
    // or the closing tag */ of a doxygen comment; start one line below
    start++;

    // remove the old prototype
    for (int i=start; i<=end; i++)
        file.RemoveLine(start);     // remove (end-start)-nth times the start-th line

#define INDENTATION_STR  wxString("    ")

    // if possible, add also the @deprecated tag in the doxygen comment
    if (file.GetLine(start-1).Contains("*/") && api->IsDeprecated())
    {
        file.RemoveLine(start-1);
        file.InsertLine(INDENTATION_STR + INDENTATION_STR +
                        "@deprecated @todo provide deprecation description", start-1);
        file.InsertLine(INDENTATION_STR + "*/", start++);
    }

    wxMethod tmp(*api);

    // discard API argument names and replace them with those parsed from doxygen XML:
    const wxArgumentTypeArray& doxygenargs = iface->GetArgumentTypes();
    const wxArgumentTypeArray& realargs = api->GetArgumentTypes();
    if (realargs.GetCount() == doxygenargs.GetCount())
    {
        for (unsigned int j=0; j<doxygenargs.GetCount(); j++)
            if (doxygenargs[j]==realargs[j])
                realargs[j].SetArgumentName(doxygenargs[j].GetArgumentName());

        tmp.SetArgumentTypes(realargs);
    }

    // insert the new one
    file.InsertLine(INDENTATION_STR + tmp.GetAsString() + ";", start);

    // now save the modification
    if (!file.Write()) {
        LogError("can't save the '%s' header file.", header);
        return;
    }
}

void IfaceCheckApp::PrintStatistics(long secs)
{
    LogMessage("wx real headers contains declaration of %d classes (%d methods)",
               m_api.GetClassesCount(), m_api.GetMethodCount());
    LogMessage("wx interface headers contains declaration of %d classes (%d methods)",
               m_interface.GetClassesCount(), m_interface.GetMethodCount());
    LogMessage("total processing took %d seconds.", secs);
}

