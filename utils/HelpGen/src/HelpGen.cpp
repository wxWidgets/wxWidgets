/////////////////////////////////////////////////////////////////////////////
// Name:        HelpGen.cpp
// Purpose:     Main program file for HelpGen
// Author:      Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Modified by:
// Created:     06/01/99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 VZ
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

/*
   BUGS

    1. wx/string.h confuses C++ parser terribly
    2. C++ parser doesn't know about virtual functions, nor static ones
    3. param checking is not done for vararg functions
    4. type comparison is dumb: it doesn't know that "char *" is the same
       that "char []" nor that "const char *" is the same as "char const *"

   TODO (+ means fixed), see also the change log at the end of the file.

   (i) small fixes in the current version

   +1. Quote special TeX characters like '&' and '_' (=> derive from wxFile)
    2. Document typedefs
    3. Document global variables
    4. Document #defines
   +5. Program options
    6. Include file name/line number in the "diff" messages?
   +7. Support for vararg functions

   (ii) plans for version 2
    1. Use wxTextFile for direct file access to avoid one scan method problems
    2. Use command line parser class for the options
    3. support for overloaded functions in diff mode (search for OVER)

   (iii) plans for version 3
    1. Merging with existing files
    2. GUI
*/

// =============================================================================
// declarations
// =============================================================================

// -----------------------------------------------------------------------------
// headers
// -----------------------------------------------------------------------------

// wxWindows
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include <wx/string.h>
    #include <wx/log.h>
    #include <wx/dynarray.h>
    #include <wx/wx.h>
#endif // WX_PRECOMP

#include <wx/file.h>

// C++ parsing classes
#include "cjparser.h"

// standard headers
#include <stdio.h>
#include <time.h>

// -----------------------------------------------------------------------------
// global vars
// -----------------------------------------------------------------------------

// just a copy of argv
static char **g_argv = NULL;

class HelpGenApp: public wxApp
{
public:
    HelpGenApp() {};

#if wxUSE_GUI
    bool OnInit();
#else
    int OnRun();
#endif
};

IMPLEMENT_APP(HelpGenApp);

// -----------------------------------------------------------------------------
// private functions
// -----------------------------------------------------------------------------

// return the label for the given function name (i.e. argument of \label)
static wxString MakeLabel(const char *classname, const char *funcname = NULL);

// return the whole \helpref{arg}{arg_label} string
static wxString MakeHelpref(const char *argument);

// [un]quote special TeX characters (in place)
static void TeXFilter(wxString* str);
static void TeXUnfilter(wxString* str); // also trims spaces

// get all comments associated with this context
static wxString GetAllComments(const spContext& ctx);

// get the string with current time (returns pointer to static buffer)
// timeFormat is used for the call of strftime(3)
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

static const char *GetCurrentTime(const char *timeFormat);

// -----------------------------------------------------------------------------
// private classes
// -----------------------------------------------------------------------------

// add a function which sanitazes the string before writing it to the file
class wxTeXFile : public wxFile
{
public:
    wxTeXFile() { }

    bool WriteTeX(const wxString& s)
    {
        wxString t(s);
        TeXFilter(&t);

        return wxFile::Write(t);
    }

private:
    wxTeXFile(const wxTeXFile&);
    wxTeXFile& operator=(const wxTeXFile&);
};

// helper class which manages the classes and function names to ignore for
// the documentation purposes (used by both HelpGenVisitor and DocManager)
class IgnoreNamesHandler
{
public:
    IgnoreNamesHandler() : m_ignore(CompareIgnoreListEntries) { }
    ~IgnoreNamesHandler() { WX_CLEAR_ARRAY(m_ignore); }

    // load file with classes/functions to ignore (add them to the names we
    // already have)
    bool AddNamesFromFile(const wxString& filename);

    // return TRUE if we ignore this function
    bool IgnoreMethod(const wxString& classname,
                      const wxString& funcname) const
    {
        if ( IgnoreClass(classname) )
            return TRUE;

        IgnoreListEntry ignore(classname, funcname);

        return m_ignore.Index(&ignore) != wxNOT_FOUND;
    }

    // return TRUE if we ignore this class entirely
    bool IgnoreClass(const wxString& classname) const
    {
        IgnoreListEntry ignore(classname, "");

        return m_ignore.Index(&ignore) != wxNOT_FOUND;
    }

protected:
    struct IgnoreListEntry
    {
        IgnoreListEntry(const wxString& classname,
                        const wxString& funcname)
            : m_classname(classname), m_funcname(funcname)
        {
        }

        wxString m_classname;
        wxString m_funcname;    // if empty, ignore class entirely
    };

    static int CompareIgnoreListEntries(IgnoreListEntry *first,
                                        IgnoreListEntry *second);

    // for efficiency, let's sort it
    WX_DEFINE_SORTED_ARRAY(IgnoreListEntry *, ArrayNamesToIgnore);

    ArrayNamesToIgnore m_ignore;

private:
    IgnoreNamesHandler(const IgnoreNamesHandler&);
    IgnoreNamesHandler& operator=(const IgnoreNamesHandler&);
};

// visitor implementation which writes all collected data to a .tex file
class HelpGenVisitor : public spVisitor
{
public:
    // ctor
    HelpGenVisitor(const wxString& directoryOut, bool overwrite);

    virtual void VisitFile( spFile& fl );
    virtual void VisitClass( spClass& cl );
    virtual void VisitEnumeration( spEnumeration& en );
    virtual void VisitTypeDef( spTypeDef& td );
    virtual void VisitPreprocessorLine( spPreprocessorLine& pd );
    virtual void VisitAttribute( spAttribute& attr );
    virtual void VisitOperation( spOperation& op );
    virtual void VisitParameter( spParameter& param );

    void EndVisit();

    // get our `ignore' object
    IgnoreNamesHandler& GetIgnoreHandler() { return m_ignoreNames; }

    // shut up g++ warning (ain't it stupid?)
    virtual ~HelpGenVisitor() { }

protected:
    // (re)initialize the state
    void Reset();

    // insert documentation for enums/typedefs coming immediately before the
    // class declaration into the class documentation
    void InsertTypedefDocs();
    void InsertEnumDocs();

    // write the headers for corresponding sections (only once)
    void InsertDataStructuresHeader();
    void InsertMethodsHeader();

    // terminate the function documentation if it was started
    void CloseFunction();

    wxString  m_directoryOut,   // directory for the output
              m_fileHeader;     // name of the .h file we parse
    bool      m_overwrite;      // overwrite existing files?
    wxTeXFile m_file;           // file we're writing to now

    // state variables
    bool m_inClass,         // TRUE after file successfully opened
         m_inTypesSection,  // enums & typedefs go there
         m_inMethodSection, // functions go here
         m_isFirstParam,    // first parameter of current function?
         m_inFunction;      // we're parsing a function declaration

    // holders for "saved" documentation
    wxString m_textStoredEnums,
             m_textStoredTypedefs,
             m_textStoredFunctionComment;

    // headers included by this file
    wxArrayString m_headers;

    // ignore handler: tells us which classes to ignore for doc generation
    // purposes
    IgnoreNamesHandler m_ignoreNames;

private:
    HelpGenVisitor(const HelpGenVisitor&);
    HelpGenVisitor& operator=(const HelpGenVisitor&);
};

// documentation manager - a class which parses TeX files and remembers the
// functions documented in them and can later compare them with all functions
// found under ctxTop by C++ parser
class DocManager
{
public:
    DocManager(bool checkParamNames);
    ~DocManager();

    // returns FALSE on failure
    bool ParseTeXFile(const wxString& filename);

    // returns FALSE if there were any differences
    bool DumpDifferences(spContext *ctxTop) const;

    // get our `ignore' object
    IgnoreNamesHandler& GetIgnoreHandler() { return m_ignoreNames; }

protected:
    // parsing TeX files
    // -----------------

    // returns the length of 'match' if the string 'str' starts with it or 0
    // otherwise
    static size_t TryMatch(const char *str, const char *match);

    // skip spaces: returns pointer to first non space character (also
    // updates the value of m_line)
    const char *SkipSpaces(const char *p)
    {
        while ( isspace(*p) ) {
            if ( *p++ == '\n' )
                m_line++;
        }

        return p;
    }

    // skips characters until the next 'c' in '*pp' unless it ends before in
    // which case FALSE is returned and pp points to '\0', otherwise TRUE is
    // returned and pp points to 'c'
    bool SkipUntil(const char **pp, char c);

    // the same as SkipUntil() but only spaces are skipped: on first non space
    // character different from 'c' the function stops and returns FALSE
    bool SkipSpaceUntil(const char **pp, char c);

    // extract the string between {} and modify '*pp' to point at the
    // character immediately after the closing '}'. The returned string is empty
    // on error.
    wxString ExtractStringBetweenBraces(const char **pp);

    // the current file and line while we're in ParseTeXFile (for error
    // messages)
    wxString m_filename;
    size_t   m_line;

    // functions and classes to ignore during diff
    // -------------------------------------------

    IgnoreNamesHandler m_ignoreNames;

    // information about all functions documented in the TeX file(s)
    // -------------------------------------------------------------

    // info about a type: for now stored as text string, but must be parsed
    // further later (to know that "char *" == "char []" - TODO)
    class TypeInfo
    {
    public:
        TypeInfo(const wxString& type) : m_type(type) { }

        bool operator==(const wxString& type) const { return m_type == type; }
        bool operator!=(const wxString& type) const { return m_type != type; }

        const wxString& GetName() const { return m_type; }

    private:
        wxString m_type;
    };

    // info abotu a function parameter
    class ParamInfo
    {
    public:
        ParamInfo(const wxString& type,
                  const wxString& name,
                  const wxString& value)
            : m_type(type), m_name(name), m_value(value)
        {
        }

        const TypeInfo& GetType() const { return m_type; }
        const wxString& GetName() const { return m_name; }
        const wxString& GetDefValue() const { return m_value; }

    private:
        TypeInfo m_type;      // type of parameter
        wxString m_name;      // name
        wxString m_value;     // default value
    };

    WX_DEFINE_ARRAY(ParamInfo *, ArrayParamInfo);

    // info about a function
    struct MethodInfo
    {
    public:
        enum MethodFlags
        {
            Const   = 0x0001,
            Virtual = 0x0002,
            Pure    = 0x0004,
            Static  = 0x0008,
            Vararg  = 0x0010
        };

        MethodInfo(const wxString& type,
                   const wxString& name,
                   const ArrayParamInfo& params)
            : m_typeRet(type), m_name(name), m_params(params)
        {
            m_flags = 0;
        }

        void SetFlag(MethodFlags flag) { m_flags |= flag; }

        const TypeInfo& GetType() const { return m_typeRet; }
        const wxString& GetName() const { return m_name; }
        const ParamInfo& GetParam(size_t n) const { return *(m_params[n]); }
        size_t GetParamCount() const { return m_params.GetCount(); }

        bool HasFlag(MethodFlags flag) const { return (m_flags & flag) != 0; }

        ~MethodInfo() { WX_CLEAR_ARRAY(m_params); }

    private:
        TypeInfo m_typeRet;     // return type
        wxString m_name;
        int      m_flags;       // bit mask of the value from the enum above

        ArrayParamInfo m_params;
    };

    WX_DEFINE_ARRAY(MethodInfo *, ArrayMethodInfo);
    WX_DEFINE_ARRAY(ArrayMethodInfo *, ArrayMethodInfos);

    // first array contains the names of all classes we found, the second has a
    // pointer to the array of methods of the given class at the same index as
    // the class name appears in m_classes
    wxArrayString    m_classes;
    ArrayMethodInfos m_methods;

    // are we checking parameter names?
    bool m_checkParamNames;

private:
    DocManager(const DocManager&);
    DocManager& operator=(const DocManager&);
};

// -----------------------------------------------------------------------------
// private functions
// -----------------------------------------------------------------------------

// =============================================================================
// implementation
// =============================================================================

// this function never returns
static void usage()
{
    wxString prog = g_argv[0];
    wxString basename = prog.AfterLast('/');
#ifdef __WXMSW__
    if ( !basename )
        basename = prog.AfterLast('\\');
#endif
    if ( !basename )
        basename = prog;

    wxLogError(
"usage: %s [global options] <mode> [mode options] <files...>\n"
"\n"
"   where global options are:\n"
"       -q          be quiet\n"
"       -v          be verbose\n"
"       -H          give this usage message\n"
"       -V          print the version info\n"
"       -i file     file with classes/function to ignore\n"
"\n"
"   where mode is one of: dump, diff\n"
"\n"
"   dump means generate .tex files for TeX2RTF converter from specified\n"
"   headers files, mode options are:\n"
"       -f          overwrite existing files\n"
"       -o outdir   directory for generated files\n"
"\n"
"   diff means compare the set of methods documented .tex file with the\n"
"   methods declared in the header:\n"
"           %s diff <file.h> <files.tex...>.\n"
"   mode specific options are:\n"
"       -p          do check parameter names (not done by default)\n"
"\n", basename.c_str(), basename.c_str());
#ifndef wxUSE_GUI
    exit(1);
#endif
}

/*
int main(int argc, char **argv)
{
*/

#if wxUSE_GUI
bool HelpGenApp::OnInit()
#else
int HelpGenApp::OnRun()
#endif
{
    enum
    {
        Mode_None,
        Mode_Dump,
        Mode_Diff
    } mode = Mode_None;

    g_argv = argv;

    if ( argc < 2 ) {
        usage();
    }

    wxArrayString filesH, filesTeX;
    wxString directoryOut,      // directory for 'dmup' output
             ignoreFile;        // file with classes/functions to ignore
    bool overwrite = FALSE,     // overwrite existing files during 'dump'?
         paramNames = FALSE;    // check param names during 'diff'?

    for ( int current = 1; current < argc ; current++ ) {
        // all options have one letter
        if ( argv[current][0] == '-' ) {
            if ( argv[current][2] == '\0' ) {
                switch ( argv[current][1] ) {
                    case 'v':
                        // be verbose
                        wxLog::GetActiveTarget()->SetVerbose();
                        continue;

                    case 'q':
                        // be quiet
                        wxLog::GetActiveTarget()->SetVerbose(FALSE);
                        continue;

                    case 'H':
                        // help requested
                        usage();

                    case 'i':
                        current++;
                        if ( current >= argc ) {
                            wxLogError("-i option requires an argument.");

                            break;
                        }

                        ignoreFile = argv[current];
                        continue;

                    case 'p':
                        if ( mode != Mode_Diff ) {
                            wxLogError("-p is only valid with diff.");

                            break;
                        }

                        paramNames = TRUE;
                        continue;

                    case 'f':
                        if ( mode != Mode_Dump ) {
                            wxLogError("-f is only valid with dump.");

                            break;
                        }

                        overwrite = TRUE;
                        continue;

                    case 'o':
                        if ( mode != Mode_Dump ) {
                            wxLogError("-o is only valid with dump.");

                            break;
                        }

                        current++;
                        if ( current >= argc ) {
                            wxLogError("-o option requires an argument.");

                            break;
                        }

                        directoryOut = argv[current];
                        if ( !!directoryOut ) {
                            // terminate with a '/' if it doesn't have it
                            switch ( directoryOut.Last() ) {
                                case '/':
#ifdef __WXMSW__
                                case '\\':
#endif
                                    break;

                                default:
                                    directoryOut += '/';
                            }
                        }
                        //else: it's empty, do nothing

                        continue;

                    default:
                        wxLogError("unknown option '%s'", argv[current]);
                        break;
                }
            }
            else {
                wxLogError("only one letter options are allowed, not '%s'.",
                           argv[current]);
            }

            // only get here after a break from switch or from else branch of if

            usage();
        }
        else {
            if ( mode == Mode_None ) {
                if ( strcmp(argv[current], "diff") == 0 )
                    mode = Mode_Diff;
                else if ( strcmp(argv[current], "dump") == 0 )
                    mode = Mode_Dump;
                else {
                    wxLogError("unknown mode '%s'.", argv[current]);

                    usage();
                }
            }
            else {
                if ( mode == Mode_Dump || filesH.IsEmpty() ) {
                    filesH.Add(argv[current]);
                }
                else {
                    // 2nd files and further are TeX files in diff mode
                    wxASSERT( mode == Mode_Diff );

                    filesTeX.Add(argv[current]);
                }
            }
        }
    }

    // create a parser object and a visitor derivation
    CJSourceParser parser;
    HelpGenVisitor visitor(directoryOut, overwrite);
    if ( !!ignoreFile && mode == Mode_Dump )
        visitor.GetIgnoreHandler().AddNamesFromFile(ignoreFile);

    spContext *ctxTop = NULL;

    // parse all header files
    size_t nFiles = filesH.GetCount();
    for ( size_t n = 0; n < nFiles; n++ ) {
        wxString header = filesH[n];
        ctxTop = parser.ParseFile(header);
        if ( !ctxTop ) {
            wxLogWarning("Header file '%s' couldn't be processed.",
                         header.c_str());
        }
        else if ( mode == Mode_Dump ) {
            ((spFile *)ctxTop)->mFileName = header;
            visitor.VisitAll(*ctxTop);
            visitor.EndVisit();
        }

#ifdef __WXDEBUG__
        if ( 0 && ctxTop )
            ctxTop->Dump("");
#endif // __WXDEBUG__
    }

    // parse all TeX files
    if ( mode == Mode_Diff ) {
        if ( !ctxTop ) {
            wxLogError("Can't complete diff.");

            // failure
            return false;
        }

        DocManager docman(paramNames);

        size_t nFiles = filesTeX.GetCount();
        for ( size_t n = 0; n < nFiles; n++ ) {
            wxString file = filesTeX[n];
            if ( !docman.ParseTeXFile(file) ) {
                wxLogWarning("TeX file '%s' couldn't be processed.",
                             file.c_str());
            }
        }

        if ( !!ignoreFile )
            docman.GetIgnoreHandler().AddNamesFromFile(ignoreFile);

        docman.DumpDifferences(ctxTop);
    }

    return false;
}

// -----------------------------------------------------------------------------
// HelpGenVisitor implementation
// -----------------------------------------------------------------------------

HelpGenVisitor::HelpGenVisitor(const wxString& directoryOut,
                               bool overwrite)
              : m_directoryOut(directoryOut)
{
    m_overwrite = overwrite;

    Reset();
}

void HelpGenVisitor::Reset()
{
    m_inClass =
    m_inFunction =
    m_inTypesSection =
    m_inMethodSection = FALSE;

    m_textStoredTypedefs =
    m_textStoredEnums =
    m_textStoredFunctionComment = "";
    m_headers.Empty();
}

void HelpGenVisitor::InsertTypedefDocs()
{
    m_file.WriteTeX(m_textStoredTypedefs);
    m_textStoredTypedefs.Empty();
}

void HelpGenVisitor::InsertEnumDocs()
{
    m_file.WriteTeX(m_textStoredEnums);
    m_textStoredEnums.Empty();
}

void HelpGenVisitor::InsertDataStructuresHeader()
{
    if ( !m_inTypesSection ) {
        m_inTypesSection = TRUE;

        m_file.WriteTeX("\\wxheading{Data structures}\n\n");
    }
}

void HelpGenVisitor::InsertMethodsHeader()
{
    if ( !m_inMethodSection ) {
        m_inMethodSection = TRUE;

        m_file.WriteTeX( "\\latexignore{\\rtfignore{\\wxheading{Members}}}\n\n");
    }
}

void HelpGenVisitor::CloseFunction()
{
    if ( m_inFunction ) {
        m_inFunction = FALSE;

        wxString totalText;
        if ( m_isFirstParam ) {
            // no params found
            totalText << "\\void";
        }

        totalText << "}\n\n";

        if ( !m_textStoredFunctionComment.IsEmpty() )
            totalText << m_textStoredFunctionComment << '\n';

        m_file.WriteTeX(totalText);
    }
}

void HelpGenVisitor::EndVisit()
{
    CloseFunction();

    m_fileHeader.Empty();

    wxLogVerbose("%s: finished generating for the current file.",
                 GetCurrentTime("%H:%M:%S"));
}

void HelpGenVisitor::VisitFile( spFile& file )
{
    m_fileHeader = file.mFileName;
    wxLogVerbose("%s: started generating docs for classes from file '%s'...",
                 GetCurrentTime("%H:%M:%S"), m_fileHeader.c_str());
}

void HelpGenVisitor::VisitClass( spClass& cl )
{
    m_inClass = FALSE; // will be left FALSE on error

    wxString name = cl.GetName();

    if ( m_ignoreNames.IgnoreClass(name) ) {
        wxLogVerbose("Skipping ignored class '%s'.", name.c_str());

        return;
    }

    // the file name is built from the class name by removing the leading "wx"
    // if any and converting it to the lower case
    wxString filename;
    if ( name(0, 2) == "wx" ) {
        filename << name.c_str() + 2;
    }
    else {
        filename << name;
    }

    filename.MakeLower();
    filename += ".tex";
    filename.Prepend(m_directoryOut);

    if ( !m_overwrite && wxFile::Exists(filename) ) {
        wxLogError("Won't overwrite existing file '%s' - please use '-f'.",
                   filename.c_str());

        return;
    }

    m_inClass = m_file.Open(filename, wxFile::write);
    if ( !m_inClass ) {
        wxLogError("Can't generate documentation for the class '%s'.",
                   name.c_str());

        return;
    }

    m_inMethodSection =
    m_inTypesSection = FALSE;

    wxLogInfo("Created new file '%s' for class '%s'.",
              filename.c_str(), name.c_str());

    // the entire text we're writing to file
    wxString totalText;

    // write out the header
    {
        wxString header;
        header.Printf("%%\n"
                      "%% automatically generated by HelpGen from\n"
                      "%% %s at %s\n"
                      "%%\n"
                      "\n"
                      "\n"
                      "\\section{\\class{%s}}\\label{%s}\n",
                      m_fileHeader.c_str(), GetCurrentTime("%d/%b/%y %H:%M:%S"),
                      name.c_str(), wxString(name).MakeLower().c_str());

        totalText << header << '\n';
    }

    // if the header includes other headers they must be related to it... try to
    // automatically generate the "See also" clause
    if ( !m_headers.IsEmpty() ) {
        // correspondence between wxWindows headers and class names
        static const char *headers[] = {
            "object",
            "defs",
            "string",
            "dynarray",
            "file",
            "time",
        };

        // NULL here means not to insert anything in "See also" for the
        // corresponding header
        static const char *classes[] = {
            NULL,
            NULL,
            NULL,
            NULL,
            "wxFile",
            "wxTime",
        };

        wxASSERT_MSG( WXSIZEOF(headers) == WXSIZEOF(classes),
                      "arrays must be in sync!" );

        wxArrayInt interestingClasses;

        size_t count = m_headers.Count(), index;
        for ( size_t n = 0; n < count; n++ ) {
            wxString baseHeaderName = m_headers[n].Before('.');
            if ( baseHeaderName(0, 3) != "wx/" )
                continue;

            baseHeaderName.erase(0, 3);
            for ( index = 0; index < WXSIZEOF(headers); index++ ) {
                if ( Stricmp(baseHeaderName, headers[index]) == 0 )
                    break;
            }

            if ( (index < WXSIZEOF(headers)) && classes[index] ) {
                // interesting header
                interestingClasses.Add(index);
            }
        }

        if ( !interestingClasses.IsEmpty() ) {
            // do generate "See also" clause
            totalText << "\\wxheading{See also:}\n\n";

            count = interestingClasses.Count();
            for ( index = 0; index < count; index++ ) {
                if ( index > 0 )
                    totalText << ", ";

                totalText << MakeHelpref(classes[interestingClasses[index]]);
            }

            totalText << "\n\n";
        }
    }

    // the comment before the class generally explains what is it for so put it
    // in place of the class description
    if ( cl.HasComments() ) {
        wxString comment = GetAllComments(cl);

        totalText << '\n' << comment << '\n';
    }

    // derived from section
    wxString derived = "\\wxheading{Derived from}\n\n";

    const StrListT& baseClasses = cl.mSuperClassNames;
    if ( baseClasses.size() == 0 ) {
        derived << "No base class";
    }
    else {
        bool first = TRUE;
        for ( StrListT::const_iterator i = baseClasses.begin();
              i != baseClasses.end();
              i++ ) {
            if ( !first ) {
                // separate from the previous one
                derived << "\\\\\n";
            }
            else {
                first = FALSE;
            }

            wxString baseclass = *i;
            derived << "\\helpref{" << baseclass << "}";
            derived << "{" << baseclass.MakeLower()  << "}";
        }
    }
    totalText << derived << "\n\n";

    // write all this to file
    m_file.WriteTeX(totalText);

    // if there were any enums/typedefs before, insert their documentation now
    InsertDataStructuresHeader();
    InsertTypedefDocs();
    InsertEnumDocs();
}

void HelpGenVisitor::VisitEnumeration( spEnumeration& en )
{
    CloseFunction();

    if ( m_inMethodSection ) {
        // FIXME that's a bug, but tell the user aboit it nevertheless... we
        // should be smart enough to process even the enums which come after the
        // functions
        wxLogWarning("enum '%s' ignored, please put it before the class "
                     "methods.", en.GetName().c_str());
        return;
    }

    // simply copy the enum text in the docs
    wxString enumeration = GetAllComments(en);
    enumeration << "{\\small \\begin{verbatim}\n"
                << en.mEnumContent
                << "\n\\end{verbatim}}\n";

    // remember for later use if we're not inside a class yet
    if ( !m_inClass ) {
        if ( !m_textStoredEnums.IsEmpty() ) {
            m_textStoredEnums << '\n';
        }

        m_textStoredEnums << enumeration;
    }
    else {
        // write the header for this section if not done yet
        InsertDataStructuresHeader();

        enumeration << '\n';
        m_file.WriteTeX(enumeration);
    }
}

void HelpGenVisitor::VisitTypeDef( spTypeDef& td )
{
    CloseFunction();

    if ( m_inMethodSection ) {
        // FIXME that's a bug, but tell the user aboit it nevertheless...
        wxLogWarning("typedef '%s' ignored, please put it before the class "
                     "methods.", td.GetName().c_str());
        return;
    }

    wxString typedefdoc;
    typedefdoc << "{\\small \\begin{verbatim}\n"
               << "typedef " << td.mOriginalType << ' ' << td.GetName()
               << "\n\\end{verbatim}}\n"
               << GetAllComments(td);

    // remember for later use if we're not inside a class yet
    if ( !m_inClass ) {
        if ( !m_textStoredTypedefs.IsEmpty() ) {
            m_textStoredTypedefs << '\n';
        }

        m_textStoredTypedefs << typedefdoc;
    }
    else {
        // write the header for this section if not done yet
        InsertDataStructuresHeader();

        typedefdoc << '\n';
        m_file.WriteTeX(typedefdoc);
    }
}

void HelpGenVisitor::VisitPreprocessorLine( spPreprocessorLine& pd )
{
    switch ( pd.GetStatementType() ) {
        case SP_PREP_DEF_INCLUDE_FILE:
            m_headers.Add(pd.CPP_GetIncludedFileNeme());
            break;

        case SP_PREP_DEF_DEFINE_SYMBOL:
            // TODO decide if it's a constant and document it if it is
            break;
    }
}

void HelpGenVisitor::VisitAttribute( spAttribute& attr )
{
    CloseFunction();

    // only document the public member variables
    if ( !m_inClass || !attr.IsPublic() )
        return;

    wxLogWarning("Ignoring member variable '%s'.", attr.GetName().c_str());
}

void HelpGenVisitor::VisitOperation( spOperation& op )
{
    CloseFunction();

    if ( !m_inClass ) {
        // we don't generate docs right now - either we ignore this class
        // entirely or we couldn't open the file
        return;
    }

    if ( !op.IsInClass() ) {
        // TODO document global functions
        wxLogWarning("skipped global function '%s'.", op.GetName().c_str());

        return;
    }

    if ( op.mVisibility == SP_VIS_PRIVATE ) {
        // FIXME should we document protected functions?
        return;
    }

    wxString funcname = op.GetName(),
             classname = op.GetClass().GetName();
    if ( m_ignoreNames.IgnoreMethod(classname, funcname) ) {
        wxLogVerbose("Skipping ignored '%s::%s'.",
                     classname.c_str(), funcname.c_str());

        return;
    }

    InsertMethodsHeader();

    // save state info
    m_inFunction =
    m_isFirstParam = TRUE;

    m_textStoredFunctionComment = GetAllComments(op);

    // start function documentation
    wxString totalText;

    // check for the special case of dtor
    wxString dtor;
    if ( (funcname[0] == '~') && (classname == funcname.c_str() + 1) ) {
        dtor.Printf("\\destruct{%s}", classname.c_str());
        funcname = dtor;
    }

    totalText.Printf("\n"
                     "\\membersection{%s::%s}\\label{%s}\n"
                     "\n"
                     "\\%sfunc{%s%s}{%s}{",
                     classname.c_str(), funcname.c_str(),
                     MakeLabel(classname, funcname).c_str(),
                     op.mIsConstant ? "const" : "",
                     op.mIsVirtual ? "virtual " : "",
                     op.mRetType.c_str(),
                     funcname.c_str());

    m_file.WriteTeX(totalText);
}

void HelpGenVisitor::VisitParameter( spParameter& param )
{
    if ( !m_inFunction )
        return;

    wxString totalText;
    if ( m_isFirstParam ) {
        m_isFirstParam = FALSE;
    }
    else {
        totalText << ", ";
    }

    totalText << "\\param{" << param.mType << " }{" << param.GetName();
    wxString defvalue = param.mInitVal;
    if ( !defvalue.IsEmpty() ) {
        totalText << " = " << defvalue;
    }

    totalText << '}';

    m_file.WriteTeX(totalText);
}

// ---------------------------------------------------------------------------
// DocManager
// ---------------------------------------------------------------------------

DocManager::DocManager(bool checkParamNames)
{
    m_checkParamNames = checkParamNames;
}

size_t DocManager::TryMatch(const char *str, const char *match)
{
    size_t lenMatch = 0;
    while ( str[lenMatch] == match[lenMatch] ) {
        lenMatch++;

        if ( match[lenMatch] == '\0' )
            return lenMatch;
    }

    return 0;
}

bool DocManager::SkipUntil(const char **pp, char c)
{
    const char *p = *pp;
    while ( *p != c ) {
        if ( *p == '\0' )
            break;

        if ( *p == '\n' )
            m_line++;

        p++;
    }

    *pp = p;

    return *p == c;
}

bool DocManager::SkipSpaceUntil(const char **pp, char c)
{
    const char *p = *pp;
    while ( *p != c ) {
        if ( !isspace(*p) || *p == '\0' )
            break;

        if ( *p == '\n' )
            m_line++;

        p++;
    }

    *pp = p;

    return *p == c;
}

wxString DocManager::ExtractStringBetweenBraces(const char **pp)
{
    wxString result;

    if ( !SkipSpaceUntil(pp, '{') ) {
        wxLogWarning("file %s(%d): '{' expected after '\\param'",
                     m_filename.c_str(), m_line);

    }
    else {
        const char *startParam = ++*pp; // skip '{'

        if ( !SkipUntil(pp, '}') ) {
            wxLogWarning("file %s(%d): '}' expected after '\\param'",
                         m_filename.c_str(), m_line);
        }
        else {
            result = wxString(startParam, (*pp)++ - startParam);
        }
    }

    return result;
}

bool DocManager::ParseTeXFile(const wxString& filename)
{
    m_filename = filename;

    wxFile file(m_filename, wxFile::read);
    if ( !file.IsOpened() )
        return FALSE;

    off_t len = file.Length();
    if ( len == wxInvalidOffset )
        return FALSE;

    char *buf = new char[len + 1];
    buf[len] = '\0';

    if ( file.Read(buf, len) == wxInvalidOffset ) {
        delete [] buf;

        return FALSE;
    }

    // reinit everything
    m_line = 1;

    wxLogVerbose("%s: starting to parse doc file '%s'.",
                 GetCurrentTime("%H:%M:%S"), m_filename.c_str());

    // the name of the class from the last "\membersection" command: we assume
    // that the following "\func" or "\constfunc" always documents a method of
    // this class (and it should always be like that in wxWindows documentation)
    wxString classname;

    for ( const char *current = buf; current - buf < len; current++ ) {
        // FIXME parsing is awfully inefficient

        if ( *current == '%' ) {
            // comment, skip until the end of line
            current++;
            SkipUntil(&current, '\n');

            continue;
        }

        // all the command we're interested in start with '\\'
        while ( *current != '\\' && *current != '\0' ) {
            if ( *current++ == '\n' )
                m_line++;
        }

        if ( *current == '\0' ) {
            // no more TeX commands left
            break;
        }

        current++; // skip '\\'

        enum
        {
            Nothing,
            Func,
            ConstFunc,
            MemberSect
        } foundCommand = Nothing;

        size_t lenMatch = TryMatch(current, "func");
        if ( lenMatch ) {
            foundCommand = Func;
        }
        else {
            lenMatch = TryMatch(current, "constfunc");
            if ( lenMatch )
                foundCommand = ConstFunc;
            else {
                lenMatch = TryMatch(current, "membersection");

                if ( lenMatch )
                    foundCommand = MemberSect;
            }
        }

        if ( foundCommand == Nothing )
            continue;

        current += lenMatch;

        if ( !SkipSpaceUntil(&current, '{') ) {
            wxLogWarning("file %s(%d): '{' expected after \\func, "
                         "\\constfunc or \\membersection.",
                         m_filename.c_str(), m_line);

            continue;
        }

        current++;

        if ( foundCommand == MemberSect ) {
            // what follows has the form <classname>::<funcname>
            const char *startClass = current;
            if ( !SkipUntil(&current, ':') || *(current + 1) != ':' ) {
                wxLogWarning("file %s(%d): '::' expected after "
                             "\\membersection.", m_filename.c_str(), m_line);
            }
            else {
                classname = wxString(startClass, current - startClass);
                TeXUnfilter(&classname);
            }

            continue;
        }

        // extract the return type
        const char *startRetType = current;

        if ( !SkipUntil(&current, '}') ) {
            wxLogWarning("file %s(%d): '}' expected after return type",
                         m_filename.c_str(), m_line);

            continue;
        }

        wxString returnType = wxString(startRetType, current - startRetType);
        TeXUnfilter(&returnType);

        current++;
        if ( !SkipSpaceUntil(&current, '{') ) { 
            wxLogWarning("file %s(%d): '{' expected after return type",
                         m_filename.c_str(), m_line);

            continue;
        }

        current++;
        const char *funcEnd = current;
        if ( !SkipUntil(&funcEnd, '}') ) {
            wxLogWarning("file %s(%d): '}' expected after function name",
                         m_filename.c_str(), m_line);

            continue;
        }

        wxString funcName = wxString(current, funcEnd - current);
        current = funcEnd + 1;

        // trim spaces from both sides
        funcName.Trim(FALSE);
        funcName.Trim(TRUE);

        // special cases: '$...$' may be used for LaTeX inline math, remove the
        // '$'s
        if ( funcName.Find('$') != wxNOT_FOUND ) {
            wxString name;
            for ( const char *p = funcName.c_str(); *p != '\0'; p++ ) {
                if ( *p != '$' && !isspace(*p) )
                    name += *p;
            }

            funcName = name;
        }

        // \destruct{foo} is really ~foo
        if ( funcName[0u] == '\\' ) {
            size_t len = strlen("\\destruct{");
            if ( funcName(0, len) != "\\destruct{" ) {
                wxLogWarning("file %s(%d): \\destruct expected",
                             m_filename.c_str(), m_line);

                continue;
            }

            funcName.erase(0, len);
            funcName.Prepend('~');

            if ( !SkipSpaceUntil(&current, '}') ) {
                wxLogWarning("file %s(%d): '}' expected after destructor",
                             m_filename.c_str(), m_line);

                continue;
            }

            funcEnd++;  // there is an extra '}' to count
        }

        TeXUnfilter(&funcName);

        // extract params
        current = funcEnd + 1; // skip '}'
        if ( !SkipSpaceUntil(&current, '{') ||
             (current++, !SkipSpaceUntil(&current, '\\')) ) {
            wxLogWarning("file %s(%d): '\\param' or '\\void' expected",
                         m_filename.c_str(), m_line);

            continue;
        }

        wxArrayString paramNames, paramTypes, paramValues;

        bool isVararg = FALSE;

        current++; // skip '\\'
        lenMatch = TryMatch(current, "void");
        if ( !lenMatch ) {
            lenMatch = TryMatch(current, "param");
            while ( lenMatch && (current - buf < len) ) {
                current += lenMatch;

                // now come {paramtype}{paramname}
                wxString paramType = ExtractStringBetweenBraces(&current);
                if ( !!paramType ) {
                    wxString paramText = ExtractStringBetweenBraces(&current);
                    if ( !!paramText ) {
                        // the param declaration may contain default value
                        wxString paramName = paramText.BeforeFirst('='),
                                 paramValue = paramText.AfterFirst('=');

                        // sanitize all strings
                        TeXUnfilter(&paramValue);
                        TeXUnfilter(&paramName);
                        TeXUnfilter(&paramType);

                        paramValues.Add(paramValue);
                        paramNames.Add(paramName);
                        paramTypes.Add(paramType);
                    }
                }
                else {
                    // vararg function?
                    wxString paramText = ExtractStringBetweenBraces(&current);
                    if ( paramText == "..." ) {
                        isVararg = TRUE;
                    }
                    else {
                        wxLogWarning("Parameters of '%s::%s' are in "
                                     "incorrect form.",
                                     classname.c_str(), funcName.c_str());
                    }
                }

                // what's next?
                current = SkipSpaces(current);
                if ( *current == ',' || *current == '}' ) {
                    current = SkipSpaces(++current);

                    lenMatch = TryMatch(current, "\\param");
                }
                else {
                    wxLogWarning("file %s(%d): ',' or '}' expected after "
                                 "'\\param'", m_filename.c_str(), m_line);

                    continue;
                }
            }

            // if we got here there was no '\\void', so must have some params
            if ( paramNames.IsEmpty() ) {
                wxLogWarning("file %s(%d): '\\param' or '\\void' expected",
                        m_filename.c_str(), m_line);

                continue;
            }
        }

        // verbose diagnostic output
        wxString paramsAll;
        size_t param, paramCount = paramNames.GetCount();
        for ( param = 0; param < paramCount; param++ ) {
            if ( param != 0 ) {
                paramsAll << ", ";
            }

            paramsAll << paramTypes[param] << ' ' << paramNames[param];
        }

        wxLogVerbose("file %s(%d): found '%s %s::%s(%s)%s'",
                     m_filename.c_str(), m_line,
                     returnType.c_str(),
                     classname.c_str(),
                     funcName.c_str(),
                     paramsAll.c_str(),
                     foundCommand == ConstFunc ? " const" : "");

        // store the info about the just found function
        ArrayMethodInfo *methods;
        int index = m_classes.Index(classname);
        if ( index == wxNOT_FOUND ) {
            m_classes.Add(classname);

            methods = new ArrayMethodInfo;
            m_methods.Add(methods);
        }
        else {
            methods = m_methods[(size_t)index];
        }

        ArrayParamInfo params;
        for ( param = 0; param < paramCount; param++ ) {
            params.Add(new ParamInfo(paramTypes[param],
                                     paramNames[param],
                                     paramValues[param]));
        }

        MethodInfo *method = new MethodInfo(returnType, funcName, params);
        if ( foundCommand == ConstFunc )
            method->SetFlag(MethodInfo::Const);
        if ( isVararg )
            method->SetFlag(MethodInfo::Vararg);

        methods->Add(method);
    }

    delete [] buf;

    wxLogVerbose("%s: finished parsing doc file '%s'.\n",
                 GetCurrentTime("%H:%M:%S"), m_filename.c_str());

    return TRUE;
}

bool DocManager::DumpDifferences(spContext *ctxTop) const
{
    typedef MMemberListT::const_iterator MemberIndex;

    bool foundDiff = FALSE;

    // flag telling us whether the given class was found at all in the header
    size_t nClass, countClassesInDocs = m_classes.GetCount();
    bool *classExists = new bool[countClassesInDocs];
    for ( nClass = 0; nClass < countClassesInDocs; nClass++ ) {
        classExists[nClass] = FALSE;
    }

    // ctxTop is normally an spFile
    wxASSERT( ctxTop->GetContextType() == SP_CTX_FILE );

    const MMemberListT& classes = ctxTop->GetMembers();
    for ( MemberIndex i = classes.begin(); i != classes.end(); i++ ) {
        spContext *ctx = *i;
        if ( ctx->GetContextType() != SP_CTX_CLASS ) {
            // TODO process also global functions, macros, ...
            continue;
        }

        spClass *ctxClass = (spClass *)ctx;
        const wxString& nameClass = ctxClass->mName;
        int index = m_classes.Index(nameClass);
        if ( index == wxNOT_FOUND ) {
            if ( !m_ignoreNames.IgnoreClass(nameClass) ) {
                foundDiff = TRUE;

                wxLogError("Class '%s' is not documented at all.",
                           nameClass.c_str());
            }

            // it makes no sense to check for its functions
            continue;
        }
        else {
            classExists[index] = TRUE;
        }

        // array of method descriptions for this class
        const ArrayMethodInfo& methods = *(m_methods[index]);
        size_t nMethod, countMethods = methods.GetCount();

        // flags telling if we already processed given function
        bool *methodExists = new bool[countMethods];
        for ( nMethod = 0; nMethod < countMethods; nMethod++ ) {
            methodExists[nMethod] = FALSE;
        }

        wxArrayString aOverloadedMethods;

        const MMemberListT& functions = ctxClass->GetMembers();
        for ( MemberIndex j = functions.begin(); j != functions.end(); j++ ) {
            ctx = *j;
            if ( ctx->GetContextType() != SP_CTX_OPERATION )
                continue;

            spOperation *ctxMethod = (spOperation *)ctx;
            const wxString& nameMethod = ctxMethod->mName;

            // find all functions with the same name
            wxArrayInt aMethodsWithSameName;
            for ( nMethod = 0; nMethod < countMethods; nMethod++ ) {
                if ( methods[nMethod]->GetName() == nameMethod )
                    aMethodsWithSameName.Add(nMethod);
            }

            if ( aMethodsWithSameName.IsEmpty() && ctxMethod->IsPublic() ) {
                if ( !m_ignoreNames.IgnoreMethod(nameClass, nameMethod) ) {
                    foundDiff = TRUE;

                    wxLogError("'%s::%s' is not documented.",
                               nameClass.c_str(),
                               nameMethod.c_str());
                }

                // don't check params
                continue;
            }
            else if ( aMethodsWithSameName.GetCount() == 1 ) {
                index = (size_t)aMethodsWithSameName[0u];
                methodExists[index] = TRUE;

                if ( m_ignoreNames.IgnoreMethod(nameClass, nameMethod) )
                    continue;

                if ( !ctxMethod->IsPublic() ) {
                    wxLogWarning("'%s::%s' is documented but not public.",
                                 nameClass.c_str(),
                                 nameMethod.c_str());
                }

                // check that the flags match
                const MethodInfo& method = *(methods[index]);

                bool isVirtual = ctxMethod->mIsVirtual;
                if ( isVirtual != method.HasFlag(MethodInfo::Virtual) ) {
                    wxLogWarning("'%s::%s' is incorrectly documented as %s"
                                 "virtual.",
                                 nameClass.c_str(),
                                 nameMethod.c_str(),
                                 isVirtual ? "not " : "");
                }

                bool isConst = ctxMethod->mIsConstant;
                if ( isConst != method.HasFlag(MethodInfo::Const) ) {
                    wxLogWarning("'%s::%s' is incorrectly documented as %s"
                                 "constant.",
                                 nameClass.c_str(),
                                 nameMethod.c_str(),
                                 isConst ? "not " : "");
                }

                // check that the params match
                const MMemberListT& params = ctxMethod->GetMembers();

                if ( params.size() != method.GetParamCount() ) {
                    wxLogError("Incorrect number of parameters for '%s::%s' "
                               "in the docs: should be %d instead of %d.",
                               nameClass.c_str(),
                               nameMethod.c_str(),
                               params.size(), method.GetParamCount());
                }
                else {
                    size_t nParam = 0;
                    for ( MemberIndex k = params.begin();
                          k != params.end();
                          k++, nParam++ ) {
                        ctx = *k;

                        // what else can a function have?
                        wxASSERT( ctx->GetContextType() == SP_CTX_PARAMETER );

                        spParameter *ctxParam = (spParameter *)ctx;
                        const ParamInfo& param = method.GetParam(nParam);
                        if ( m_checkParamNames &&
                             (param.GetName() != ctxParam->mName) ) {
                            foundDiff = TRUE;

                            wxLogError("Parameter #%d of '%s::%s' should be "
                                       "'%s' and not '%s'.",
                                       nParam + 1,
                                       nameClass.c_str(),
                                       nameMethod.c_str(),
                                       ctxParam->mName.c_str(),
                                       param.GetName().c_str());

                            continue;
                        }

                        if ( param.GetType() != ctxParam->mType ) {
                            foundDiff = TRUE;

                            wxLogError("Type of parameter '%s' of '%s::%s' "
                                       "should be '%s' and not '%s'.",
                                       ctxParam->mName.c_str(),
                                       nameClass.c_str(),
                                       nameMethod.c_str(),
                                       ctxParam->mType.c_str(),
                                       param.GetType().GetName().c_str());

                            continue;
                        }

                        if ( param.GetDefValue() != ctxParam->mInitVal ) {
                            wxLogWarning("Default value of parameter '%s' of "
                                         "'%s::%s' should be '%s' and not "
                                         "'%s'.",
                                         ctxParam->mName.c_str(),
                                         nameClass.c_str(),
                                         nameMethod.c_str(),
                                         ctxParam->mInitVal.c_str(),
                                         param.GetDefValue().c_str());
                        }
                    }
                }
            }
            else {
                // TODO OVER add real support for overloaded methods

                if ( m_ignoreNames.IgnoreMethod(nameClass, nameMethod) )
                    continue;

                if ( aOverloadedMethods.Index(nameMethod) == wxNOT_FOUND ) {
                    // mark all methods with this name as existing
                    for ( nMethod = 0; nMethod < countMethods; nMethod++ ) {
                        if ( methods[nMethod]->GetName() == nameMethod )
                            methodExists[nMethod] = TRUE;
                    }

                    aOverloadedMethods.Add(nameMethod);

                    wxLogVerbose("'%s::%s' is overloaded and I'm too "
                                 "stupid to find the right match - skipping "
                                 "the param and flags checks.",
                                 nameClass.c_str(),
                                 nameMethod.c_str());
                }
                //else: warning already given
            }
        }

        for ( nMethod = 0; nMethod < countMethods; nMethod++ ) {
            if ( !methodExists[nMethod] ) {
                const wxString& nameMethod = methods[nMethod]->GetName();
                if ( !m_ignoreNames.IgnoreMethod(nameClass, nameMethod) ) {
                    foundDiff = TRUE;

                    wxLogError("'%s::%s' is documented but doesn't exist.",
                               nameClass.c_str(),
                               nameMethod.c_str());
                }
            }
        }

        delete [] methodExists;
    }

    // check that all classes we found in the docs really exist
    for ( nClass = 0; nClass < countClassesInDocs; nClass++ ) {
        if ( !classExists[nClass] ) {
            foundDiff = TRUE;

            wxLogError("Class '%s' is documented but doesn't exist.",
                       m_classes[nClass].c_str());
        }
    }

    delete [] classExists;

    return !foundDiff;
}

DocManager::~DocManager()
{
    WX_CLEAR_ARRAY(m_methods);
}

// ---------------------------------------------------------------------------
// IgnoreNamesHandler implementation
// ---------------------------------------------------------------------------

int IgnoreNamesHandler::CompareIgnoreListEntries(IgnoreListEntry *first,
                                                 IgnoreListEntry *second)
{
    // first compare the classes
    int rc = first->m_classname.Cmp(second->m_classname);
    if ( rc == 0 )
        rc = first->m_funcname.Cmp(second->m_funcname);

    return rc;
}

bool IgnoreNamesHandler::AddNamesFromFile(const wxString& filename)
{
    wxFile file(filename, wxFile::read);
    if ( !file.IsOpened() )
        return FALSE;

    off_t len = file.Length();
    if ( len == wxInvalidOffset )
        return FALSE;

    char *buf = new char[len + 1];
    buf[len] = '\0';

    if ( file.Read(buf, len) == wxInvalidOffset ) {
        delete [] buf;

        return FALSE;
    }

    wxString line;
    for ( const char *current = buf; ; current++ ) {
#ifdef __WXMSW__
        // skip DOS line separator
        if ( *current == '\r' )
            current++;
#endif // wxMSW

        if ( *current == '\n' || *current == '\0' ) {
            if ( line[0u] != '#' ) {
                if ( line.Find(':') != wxNOT_FOUND ) {
                    wxString classname = line.BeforeFirst(':'),
                             funcname = line.AfterLast(':');
                    m_ignore.Add(new IgnoreListEntry(classname, funcname));
                }
                else {
                    // entire class
                    m_ignore.Add(new IgnoreListEntry(line, ""));
                }
            }
            //else: comment

            if ( *current == '\0' )
                break;

            line.Empty();
        }
        else {
            line += *current;
        }
    }

    delete [] buf;

    return TRUE;
}

// -----------------------------------------------------------------------------
// global function implementation
// -----------------------------------------------------------------------------

static wxString MakeLabel(const char *classname, const char *funcname)
{
    wxString label(classname);
    if ( funcname && funcname[0] == '\\' ) {
        // we may have some special TeX macro - so far only \destruct exists,
        // but may be later others will be added
        static const char *macros[] = { "destruct" };
        static const char *replacement[] = { "dtor" };

        size_t n;
        for ( n = 0; n < WXSIZEOF(macros); n++ ) {
            if ( strncmp(funcname + 1, macros[n], strlen(macros[n])) == 0 ) {
                // found
                break;
            }
        }

        if ( n == WXSIZEOF(macros) ) {
            wxLogWarning("unknown function name '%s' - leaving as is.",
                         funcname);
        }
        else {
            funcname = replacement[n];
        }
    }

    if ( funcname )
        label << funcname;

    label.MakeLower();

    return label;
}

static wxString MakeHelpref(const char *argument)
{
    wxString helpref;
    helpref << "\\helpref{" << argument << "}{" << MakeLabel(argument) << '}';

    return helpref;
}

static void TeXUnfilter(wxString* str)
{
    // FIXME may be done much more quickly
    str->Trim(TRUE);
    str->Trim(FALSE);

    str->Replace("\\&", "&");
    str->Replace("\\_", "_");
}

static void TeXFilter(wxString* str)
{
    // FIXME may be done much more quickly
    str->Replace("&", "\\&");
    str->Replace("_", "\\_");
}

static wxString GetAllComments(const spContext& ctx)
{
    wxString comments;
    const MCommentListT& commentsList = ctx.GetCommentList();
    for ( MCommentListT::const_iterator i = commentsList.begin();
          i != commentsList.end();
          i++ ) {
        wxString comment = (*i)->GetText();

        // don't take comments like "// ----------" &c
        comment.Trim(FALSE);
        if ( !!comment &&
              comment == wxString(comment[0u], comment.length() - 1) + '\n' )
            comments << "\n";
        else
            comments << comment;
    }

    return comments;
}

static const char *GetCurrentTime(const char *timeFormat)
{
    static char s_timeBuffer[128];
    time_t timeNow;
    struct tm *ptmNow;

    time(&timeNow);
    ptmNow = localtime(&timeNow);

    strftime(s_timeBuffer, WXSIZEOF(s_timeBuffer), timeFormat, ptmNow);

    return s_timeBuffer;
}

/*
   $Log$
   Revision 1.12  2000/10/09 13:53:33  juliansmart
   Doc corrections; added HelpGen project files

   Revision 1.11  2000/07/15 19:50:42  cvsuser
   merged 2.2 branch

   Revision 1.10.2.2  2000/03/27 15:33:10  VZ
   don't trasnform output dir name to lower case

   Revision 1.10  2000/03/11 10:05:23  VS
   now compiles with wxBase

   Revision 1.9  2000/01/16 13:25:21  VS
   compilation fixes (gcc)

   Revision 1.8  1999/09/13 14:29:39  JS

   Made HelpGen into a wxWin app (still uses command-line args); moved includes
   into src for simplicity; added VC++ 5 project file

   Revision 1.7  1999/02/21 22:32:32  VZ
   1. more C++ parser fixes - now it almost parses wx/string.h
    a) #if/#ifdef/#else (very) limited support
    b) param type fix - now indirection chars are correctly handled
    c) class/struct/union distinction
    d) public/private fixes
    e) Dump() function added - very useful for debugging

   2. option to ignore parameter names during 'diff' (in fact, they're ignored
      by default, and this option switches it on)

   Revision 1.6  1999/02/20 23:00:26  VZ
   1. new 'diff' mode which seems to work
   2. output files are not overwritten in 'dmup' mode
   3. fixes for better handling of const functions and operators
    ----------------------------
    revision 1.5
    date: 1999/02/15 23:07:25;  author: VZ;  state: Exp;  lines: +106 -45
    1. Parser improvements
     a) const and virtual methods are parsed correctly (not static yet)
     b) "const" which is part of the return type is not swallowed

    2. HelpGen improvements: -o outputdir parameter added to the cmd line,
       "//---------" kind comments discarded now.
    ----------------------------
    revision 1.4
    date: 1999/01/13 14:23:31;  author: JS;  state: Exp;  lines: +4 -4

    some tweaks to HelpGen
    ----------------------------
    revision 1.3
    date: 1999/01/09 20:18:03;  author: JS;  state: Exp;  lines: +7 -2

    HelpGen starting to compile with VC++
    ----------------------------
    revision 1.2
    date: 1999/01/08 19:46:22;  author: VZ;  state: Exp;  lines: +208 -35

    supports typedefs, generates "See also:" and adds "virtual " for virtual
    functions
    ----------------------------
    revision 1.1
    date: 1999/01/08 17:45:55;  author: VZ;  state: Exp;

    HelpGen is a prototype of the tool for automatic generation of the .tex files
    for wxWindows documentation from C++ headers
*/

/* vi: set tw=80 et ts=4 sw=4: */
