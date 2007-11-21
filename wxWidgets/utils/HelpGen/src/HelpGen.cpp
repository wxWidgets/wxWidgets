/////////////////////////////////////////////////////////////////////////////
// Name:        HelpGen.cpp
// Purpose:     Main program file for HelpGen
// Author:      Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Modified by:
// Created:     06/01/99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 VZ
// Licence:     wxWindows Licence
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

// wxWidgets
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_UNICODE
    #error "HelpGen doesn't build in Unicode mode"
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/dynarray.h"
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/file.h"
#include "wx/regex.h"
#include "wx/hash.h"

// C++ parsing classes
#include "cjparser.h"

// standard headers
#include <stdio.h>
#include <time.h>

// -----------------------------------------------------------------------------
// private functions
// -----------------------------------------------------------------------------

// return the label for the given function name (i.e. argument of \label)
static wxString MakeLabel(const wxChar *classname, const wxChar *funcname = NULL);

// return the whole \helpref{arg}{arg_label} string
static wxString MakeHelpref(const wxChar *argument);

// [un]quote special TeX characters (in place)
static void TeXFilter(wxString* str);
static void TeXUnfilter(wxString* str); // also trims spaces

// get all comments associated with this context
static wxString GetAllComments(const spContext& ctx);

// get the string with current time (returns pointer to static buffer)
// timeFormat is used for the call of strftime(3)
static const char *GetCurrentTimeFormatted(const char *timeFormat);

// get the string containing the program version
static const wxString GetVersionString();

// -----------------------------------------------------------------------------
// private classes
// -----------------------------------------------------------------------------

// a function documentation entry
struct FunctionDocEntry
{
    FunctionDocEntry(const wxString& name_, const wxString& text_)
        : name(name_), text(text_) { }

    // the function name
    wxString name;

    // the function doc text
    wxString text;

    // sorting stuff
    static int Compare(FunctionDocEntry **pp1, FunctionDocEntry **pp2)
    {
        // the methods should appear in the following order: ctors, dtor, all
        // the rest in the alphabetical order
        bool isCtor1 = (*pp1)->name == classname;
        bool isCtor2 = (*pp2)->name == classname;

        if ( isCtor1 ) {
            if ( isCtor2 ) {
                // we don't order the ctors because we don't know how to do it
                return 0;
            }

            // ctor comes before non-ctor
            return -1;
        }
        else {
            if ( isCtor2 ) {
                // non-ctor must come after ctor
                return 1;
            }

            wxString dtorname = wxString(_T("~")) + classname;

            // there is only one dtor, so the logic here is simpler
            if ( (*pp1)->name == dtorname ) {
                return -1;
            }
            else if ( (*pp2)->name == dtorname ) {
                return 1;
            }

            // two normal methods
            return wxStrcmp((*pp1)->name, (*pp2)->name);
        }
    }

    static wxString classname;
};

wxString FunctionDocEntry::classname;

WX_DECLARE_OBJARRAY(FunctionDocEntry, FunctionDocEntries);

#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(FunctionDocEntries);

// add a function which sanitazes the string before writing it to the file and
// also capable of delaying output and sorting it before really writing it to
// the file (done from FlushAll())
class wxTeXFile : public wxFile
{
public:
    wxTeXFile() { }

    // write a string to file verbatim (should only be used for the strings
    // inside verbatim environment)
    void WriteVerbatim(const wxString& s)
    {
        m_text += s;
    }

    // write a string quoting TeX specials in it
    void WriteTeX(const wxString& s)
    {
        wxString t(s);
        TeXFilter(&t);

        m_text += t;
    }

    // do write everything to file
    bool FlushAll()
    {
        if ( m_text.empty() )
            return true;

        if ( !Write(m_text) ) {
            wxLogError(_T("Failed to output generated documentation."));

            return false;
        }

        m_text.clear();

        return true;
    }

private:
    wxTeXFile(const wxTeXFile&);
    wxTeXFile& operator=(const wxTeXFile&);

    wxString m_text;
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

    // return true if we ignore this function
    bool IgnoreMethod(const wxString& classname,
                      const wxString& funcname) const
    {
        if ( IgnoreClass(classname) )
            return true;

        IgnoreListEntry ignore(classname, funcname);

        return m_ignore.Index(&ignore) != wxNOT_FOUND;
    }

    // return true if we ignore this class entirely
    bool IgnoreClass(const wxString& classname) const
    {
        IgnoreListEntry ignore(classname, wxEmptyString);

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
public: // FIXME: macro requires it
    WX_DEFINE_SORTED_ARRAY(IgnoreListEntry *, ArrayNamesToIgnore);

protected:
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

    // write out all function docs when there are no more left in this class
    // after sorting them in alphabetical order
    void CloseClass();

    wxString  m_directoryOut,   // directory for the output
              m_fileHeader;     // name of the .h file we parse
    bool      m_overwrite;      // overwrite existing files?
    wxTeXFile m_file;           // file we're writing to now

    // state variables
    bool m_inClass,         // true after file successfully opened
         m_inTypesSection,  // enums & typedefs go there
         m_inMethodSection, // functions go here
         m_isFirstParam;    // first parameter of current function?

    // non empty while parsing a class
    wxString m_classname;

    // these are only non-empty while parsing a method:
    wxString m_funcName,    // the function name
             m_textFunc;    // the function doc text

    // the array containing the documentation entries for the functions in the
    // class currently being parsed
    FunctionDocEntries m_arrayFuncDocs;

    // holders for "saved" documentation
    wxString m_textStoredTypedefs,
             m_textStoredFunctionComment;

    // for enums we have to use an array as we can't intermix the normal text
    // and the text inside verbatim environment
    wxArrayString m_storedEnums,
                  m_storedEnumsVerb;

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

    // returns false on failure
    bool ParseTeXFile(const wxString& filename);

    // returns false if there were any differences
    bool DumpDifferences(spContext *ctxTop) const;

    // get our `ignore' object
    IgnoreNamesHandler& GetIgnoreHandler() { return m_ignoreNames; }

protected:
    // parsing TeX files
    // -----------------

    // returns the length of 'match' if the string 'str' starts with it or 0
    // otherwise
    static size_t TryMatch(const wxChar *str, const wxChar *match);

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
    // which case false is returned and pp points to '\0', otherwise true is
    // returned and pp points to 'c'
    bool SkipUntil(const char **pp, char c);

    // the same as SkipUntil() but only spaces are skipped: on first non space
    // character different from 'c' the function stops and returns false
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

public: // Note: Sun C++ 5.5 requires TypeInfo and ParamInfo to be public

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

    friend class ParamInfo; // for access to TypeInfo

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

public: // FIXME: macro requires it
    WX_DEFINE_ARRAY_PTR(ParamInfo *, ArrayParamInfo);

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

    WX_DEFINE_ARRAY_PTR(MethodInfo *, ArrayMethodInfo);
    WX_DEFINE_ARRAY_PTR(ArrayMethodInfo *, ArrayMethodInfos);

private:
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

// =============================================================================
// implementation
// =============================================================================

static char **g_argv = NULL;

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

    wxLogMessage(
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

    exit(1);
}

int main(int argc, char **argv)
{
    g_argv = argv;

    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");

        return -1;
    }

    enum
    {
        Mode_None,
        Mode_Dump,
        Mode_Diff
    } mode = Mode_None;

    if ( argc < 2 ) {
        usage();
    }

    wxArrayString filesH, filesTeX;
    wxString directoryOut,      // directory for 'dmup' output
             ignoreFile;        // file with classes/functions to ignore
    bool overwrite = false,     // overwrite existing files during 'dump'?
         paramNames = false;    // check param names during 'diff'?

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
                        wxLog::GetActiveTarget()->SetVerbose(false);
                        continue;

                    case 'H':
                        // help requested
                        usage();
                        // doesn't return

                    case 'V':
                        // version requested
                        wxLogMessage("HelpGen version %s\n"
                                     "(c) 1999-2001 Vadim Zeitlin\n",
                                     GetVersionString().c_str());
                        return 0;

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

                        paramNames = true;
                        continue;

                    case 'f':
                        if ( mode != Mode_Dump ) {
                            wxLogError("-f is only valid with dump.");

                            break;
                        }

                        overwrite = true;
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
                        if ( !directoryOut.empty() ) {
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
    if ( !ignoreFile.empty() && mode == Mode_Dump )
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
            ((spFile *)ctxTop)->m_FileName = header;
            visitor.VisitAll(*ctxTop);
            visitor.EndVisit();
        }

#ifdef __WXDEBUG__
        if ( 0 && ctxTop )
            ctxTop->Dump(wxEmptyString);
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

        if ( !ignoreFile.empty() )
            docman.GetIgnoreHandler().AddNamesFromFile(ignoreFile);

        docman.DumpDifferences(ctxTop);
    }

    return 0;
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
    m_inTypesSection =
    m_inMethodSection = false;

    m_classname =
    m_funcName =
    m_textFunc =
    m_textStoredTypedefs =
    m_textStoredFunctionComment = wxEmptyString;

    m_arrayFuncDocs.Empty();

    m_storedEnums.Empty();
    m_storedEnumsVerb.Empty();
    m_headers.Empty();
}

void HelpGenVisitor::InsertTypedefDocs()
{
    m_file.WriteTeX(m_textStoredTypedefs);
    m_textStoredTypedefs.Empty();
}

void HelpGenVisitor::InsertEnumDocs()
{
    size_t count = m_storedEnums.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_file.WriteTeX(m_storedEnums[n]);
        m_file.WriteVerbatim(m_storedEnumsVerb[n] + '\n');
    }

    m_storedEnums.Empty();
    m_storedEnumsVerb.Empty();
}

void HelpGenVisitor::InsertDataStructuresHeader()
{
    if ( !m_inTypesSection ) {
        m_inTypesSection = true;

        m_file.WriteVerbatim("\\wxheading{Data structures}\n\n");
    }
}

void HelpGenVisitor::InsertMethodsHeader()
{
    if ( !m_inMethodSection ) {
        m_inMethodSection = true;

        m_file.WriteVerbatim( "\\latexignore{\\rtfignore{\\wxheading{Members}}}\n\n");
    }
}

void HelpGenVisitor::CloseFunction()
{
    if ( !m_funcName.empty() ) {
        if ( m_isFirstParam ) {
            // no params found
            m_textFunc << "\\void";
        }

        m_textFunc << "}\n\n";

        if ( !m_textStoredFunctionComment.empty() ) {
            m_textFunc << m_textStoredFunctionComment << '\n';
        }

        m_arrayFuncDocs.Add(new FunctionDocEntry(m_funcName, m_textFunc));

        m_funcName.clear();
    }
}

void HelpGenVisitor::CloseClass()
{
    CloseFunction();

    if ( m_inClass )
    {
        size_t count = m_arrayFuncDocs.GetCount();
        if ( count )
        {
            size_t n;
            FunctionDocEntry::classname = m_classname;

            m_arrayFuncDocs.Sort(FunctionDocEntry::Compare);

            // Now examine each first line and if it's been seen, cut it
            // off (it's a duplicate \membersection)
            wxHashTable membersections(wxKEY_STRING);

            for ( n = 0; n < count; n++ )
            {
                wxString section(m_arrayFuncDocs[n].text);

                // Strip leading whitespace
                int pos = section.Find(_T("\\membersection"));
                if (pos > -1)
                {
                    section = section.Mid(pos);
                }

                wxString ms(section.BeforeFirst(wxT('\n')));
                if (membersections.Get(ms))
                {
                    m_arrayFuncDocs[n].text = section.AfterFirst(wxT('\n'));
                }
                else
                {
                    membersections.Put(ms.c_str(), & membersections);
                }
            }

            for ( n = 0; n < count; n++ ) {
                m_file.WriteTeX(m_arrayFuncDocs[n].text);
            }

            m_arrayFuncDocs.Empty();
        }

        m_inClass = false;
        m_classname.clear();
    }
    m_file.FlushAll();
}

void HelpGenVisitor::EndVisit()
{
    CloseFunction();

    CloseClass();

    m_fileHeader.Empty();

    m_file.FlushAll();
    if (m_file.IsOpened())
    {
        m_file.Flush();
        m_file.Close();
    }

    wxLogVerbose("%s: finished generating for the current file.",
                 GetCurrentTimeFormatted("%H:%M:%S"));
}

void HelpGenVisitor::VisitFile( spFile& file )
{
    m_fileHeader = file.m_FileName;
    wxLogVerbose("%s: started generating docs for classes from file '%s'...",
                 GetCurrentTimeFormatted("%H:%M:%S"), m_fileHeader.c_str());
}

void HelpGenVisitor::VisitClass( spClass& cl )
{
    CloseClass();

    if (m_file.IsOpened())
    {
        m_file.Flush();
        m_file.Close();
    }

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
    m_inTypesSection = false;

    wxLogInfo("Created new file '%s' for class '%s'.",
              filename.c_str(), name.c_str());

    // write out the header
    wxString header;
    header.Printf("%%\n"
                  "%% automatically generated by HelpGen %s from\n"
                  "%% %s at %s\n"
                  "%%\n"
                  "\n"
                  "\n"
                  "\\section{\\class{%s}}\\label{%s}\n\n",
                  GetVersionString().c_str(),
                  m_fileHeader.c_str(),
                  GetCurrentTimeFormatted("%d/%b/%y %H:%M:%S"),
                  name.c_str(),
                  wxString(name).MakeLower().c_str());

    m_file.WriteVerbatim(header);

    // the entire text we're writing to file
    wxString totalText;

    // if the header includes other headers they must be related to it... try to
    // automatically generate the "See also" clause
    if ( !m_headers.IsEmpty() ) {
        // correspondence between wxWidgets headers and class names
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

    const StrListT& baseClasses = cl.m_SuperClassNames;
    if ( baseClasses.size() == 0 ) {
        derived << "No base class";
    }
    else {
        bool first = true;
        for ( StrListT::const_iterator i = baseClasses.begin();
              i != baseClasses.end();
              i++ ) {
            if ( !first ) {
                // separate from the previous one
                derived << "\\\\\n";
            }
            else {
                first = false;
            }

            wxString baseclass = *i;
            derived << "\\helpref{" << baseclass << "}";
            derived << "{" << baseclass.MakeLower()  << "}";
        }
    }
    totalText << derived << "\n\n";

    // include file section
    wxString includeFile = "\\wxheading{Include files}\n\n";
    includeFile << "<" << m_fileHeader << ">";

    totalText << includeFile << "\n\n";

    // write all this to file
    m_file.WriteTeX(totalText);

    // if there were any enums/typedefs before, insert their documentation now
    InsertDataStructuresHeader();
    InsertTypedefDocs();
    InsertEnumDocs();

    //m_file.Flush();
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
    wxString enumeration = GetAllComments(en),
             enumerationVerb;

    enumerationVerb << _T("\\begin{verbatim}\n")
                    << en.m_EnumContent
                    << _T("\n\\end{verbatim}\n");

    // remember for later use if we're not inside a class yet
    if ( !m_inClass ) {
        m_storedEnums.Add(enumeration);
        m_storedEnumsVerb.Add(enumerationVerb);
    }
    else {
        // write the header for this section if not done yet
        InsertDataStructuresHeader();

        m_file.WriteTeX(enumeration);
        m_file.WriteVerbatim(enumerationVerb);
        m_file.WriteVerbatim('\n');
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
    typedefdoc << _T("{\\small \\begin{verbatim}\n")
               << _T("typedef ") << td.m_OriginalType << _T(' ') << td.GetName()
               << _T("\n\\end{verbatim}}\n")
               << GetAllComments(td);

    // remember for later use if we're not inside a class yet
    if ( !m_inClass ) {
        if ( !m_textStoredTypedefs.empty() ) {
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

    m_classname = op.GetClass().GetName();
    wxString funcname = op.GetName();

    if ( m_ignoreNames.IgnoreMethod(m_classname, funcname) ) {
        wxLogVerbose("Skipping ignored '%s::%s'.",
                     m_classname.c_str(), funcname.c_str());

        return;
    }

    InsertMethodsHeader();

    // save state info
    m_funcName = funcname;
    m_isFirstParam = true;

    m_textStoredFunctionComment = GetAllComments(op);

    // start function documentation
    wxString totalText;

    // check for the special case of dtor
    wxString dtor;
    if ( (funcname[0u] == '~') && (m_classname == funcname.c_str() + 1) ) {
        dtor.Printf("\\destruct{%s}", m_classname.c_str());
        funcname = dtor;
    }

    m_textFunc.Printf("\n"
        "\\membersection{%s::%s}\\label{%s}\n",
        m_classname.c_str(), funcname.c_str(),
        MakeLabel(m_classname, funcname).c_str());

    wxString constStr;
    if(op.mIsConstant) constStr = _T("const");

    wxString virtualStr;
    if(op.mIsVirtual) virtualStr = _T("virtual ");

    wxString func;
    func.Printf(_T("\n")
                _T("\\%sfunc{%s%s}{%s}{"),
                constStr.c_str(),
                virtualStr.c_str(),
                op.m_RetType.c_str(),
                funcname.c_str());
    m_textFunc += func;
}

void HelpGenVisitor::VisitParameter( spParameter& param )
{
    if ( m_funcName.empty() )
        return;

    if ( m_isFirstParam ) {
        m_isFirstParam = false;
    }
    else {
        m_textFunc << ", ";
    }

    m_textFunc << "\\param{" << param.m_Type << " }{" << param.GetName();
    wxString defvalue = param.m_InitVal;
    if ( !defvalue.empty() ) {
        m_textFunc << " = " << defvalue;
    }

    m_textFunc << '}';
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
                     m_filename.c_str(), (int)m_line);

    }
    else {
        const char *startParam = ++*pp; // skip '{'

        if ( !SkipUntil(pp, '}') ) {
            wxLogWarning("file %s(%d): '}' expected after '\\param'",
                         m_filename.c_str(), (int)m_line);
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
        return false;

    off_t len = file.Length();
    if ( len == wxInvalidOffset )
        return false;

    char *buf = new char[len + 1];
    buf[len] = '\0';

    if ( file.Read(buf, len) == wxInvalidOffset ) {
        delete [] buf;

        return false;
    }

    // reinit everything
    m_line = 1;

    wxLogVerbose("%s: starting to parse doc file '%s'.",
                 GetCurrentTimeFormatted("%H:%M:%S"), m_filename.c_str());

    // the name of the class from the last "\membersection" command: we assume
    // that the following "\func" or "\constfunc" always documents a method of
    // this class (and it should always be like that in wxWidgets documentation)
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
                         m_filename.c_str(), (int)m_line);

            continue;
        }

        current++;

        if ( foundCommand == MemberSect ) {
            // what follows has the form <classname>::<funcname>
            const char *startClass = current;
            if ( !SkipUntil(&current, ':') || *(current + 1) != ':' ) {
                wxLogWarning("file %s(%d): '::' expected after "
                             "\\membersection.", m_filename.c_str(), (int)m_line);
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
                         m_filename.c_str(), (int)m_line);

            continue;
        }

        wxString returnType = wxString(startRetType, current - startRetType);
        TeXUnfilter(&returnType);

        current++;
        if ( !SkipSpaceUntil(&current, '{') ) {
            wxLogWarning("file %s(%d): '{' expected after return type",
                         m_filename.c_str(), (int)m_line);

            continue;
        }

        current++;
        const char *funcEnd = current;
        if ( !SkipUntil(&funcEnd, '}') ) {
            wxLogWarning("file %s(%d): '}' expected after function name",
                         m_filename.c_str(), (int)m_line);

            continue;
        }

        wxString funcName = wxString(current, funcEnd - current);
        current = funcEnd + 1;

        // trim spaces from both sides
        funcName.Trim(false);
        funcName.Trim(true);

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
                             m_filename.c_str(), (int)m_line);

                continue;
            }

            funcName.erase(0, len);
            funcName.Prepend('~');

            if ( !SkipSpaceUntil(&current, '}') ) {
                wxLogWarning("file %s(%d): '}' expected after destructor",
                             m_filename.c_str(), (int)m_line);

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
                         m_filename.c_str(), (int)m_line);

            continue;
        }

        wxArrayString paramNames, paramTypes, paramValues;

        bool isVararg = false;

        current++; // skip '\\'
        lenMatch = TryMatch(current, "void");
        if ( !lenMatch ) {
            lenMatch = TryMatch(current, "param");
            while ( lenMatch && (current - buf < len) ) {
                current += lenMatch;

                // now come {paramtype}{paramname}
                wxString paramType = ExtractStringBetweenBraces(&current);
                if ( !paramType.empty() ) {
                    wxString paramText = ExtractStringBetweenBraces(&current);
                    if ( !paramText.empty() ) {
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
                        isVararg = true;
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
                                 "'\\param'", m_filename.c_str(), (int)m_line);

                    continue;
                }
            }

            // if we got here there was no '\\void', so must have some params
            if ( paramNames.IsEmpty() ) {
                wxLogWarning("file %s(%d): '\\param' or '\\void' expected",
                        m_filename.c_str(), (int)m_line);

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

        wxString constStr;
        if (foundCommand == ConstFunc)
            constStr = _T(" const");

        wxLogVerbose("file %s(%d): found '%s %s::%s(%s)%s'",
                     m_filename.c_str(),
                     (int)m_line,
                     returnType.c_str(),
                     classname.c_str(),
                     funcName.c_str(),
                     paramsAll.c_str(),
                     constStr.c_str());

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
                 GetCurrentTimeFormatted("%H:%M:%S"), m_filename.c_str());

    return true;
}

bool DocManager::DumpDifferences(spContext *ctxTop) const
{
    typedef MMemberListT::const_iterator MemberIndex;

    bool foundDiff = false;

    // flag telling us whether the given class was found at all in the header
    size_t nClass, countClassesInDocs = m_classes.GetCount();
    bool *classExists = new bool[countClassesInDocs];
    for ( nClass = 0; nClass < countClassesInDocs; nClass++ ) {
        classExists[nClass] = false;
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
        const wxString& nameClass = ctxClass->m_Name;
        int index = m_classes.Index(nameClass);
        if ( index == wxNOT_FOUND ) {
            if ( !m_ignoreNames.IgnoreClass(nameClass) ) {
                foundDiff = true;

                wxLogError("Class '%s' is not documented at all.",
                           nameClass.c_str());
            }

            // it makes no sense to check for its functions
            continue;
        }
        else {
            classExists[index] = true;
        }

        // array of method descriptions for this class
        const ArrayMethodInfo& methods = *(m_methods[index]);
        size_t nMethod, countMethods = methods.GetCount();

        // flags telling if we already processed given function
        bool *methodExists = new bool[countMethods];
        for ( nMethod = 0; nMethod < countMethods; nMethod++ ) {
            methodExists[nMethod] = false;
        }

        wxArrayString aOverloadedMethods;

        const MMemberListT& functions = ctxClass->GetMembers();
        for ( MemberIndex j = functions.begin(); j != functions.end(); j++ ) {
            ctx = *j;
            if ( ctx->GetContextType() != SP_CTX_OPERATION )
                continue;

            spOperation *ctxMethod = (spOperation *)ctx;
            const wxString& nameMethod = ctxMethod->m_Name;

            // find all functions with the same name
            wxArrayInt aMethodsWithSameName;
            for ( nMethod = 0; nMethod < countMethods; nMethod++ ) {
                if ( methods[nMethod]->GetName() == nameMethod )
                    aMethodsWithSameName.Add(nMethod);
            }

            if ( aMethodsWithSameName.IsEmpty() && ctxMethod->IsPublic() ) {
                if ( !m_ignoreNames.IgnoreMethod(nameClass, nameMethod) ) {
                    foundDiff = true;

                    wxLogError("'%s::%s' is not documented.",
                               nameClass.c_str(),
                               nameMethod.c_str());
                }

                // don't check params
                continue;
            }
            else if ( aMethodsWithSameName.GetCount() == 1 ) {
                index = (size_t)aMethodsWithSameName[0u];
                methodExists[index] = true;

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
                if ( isVirtual != method.HasFlag(MethodInfo::Virtual) )
                {
                    wxString virtualStr;
                    if(isVirtual)virtualStr = _T("not ");

                    wxLogWarning("'%s::%s' is incorrectly documented as %s"
                                 "virtual.",
                                 nameClass.c_str(),
                                 nameMethod.c_str(),
                                 virtualStr.c_str());
                }

                bool isConst = ctxMethod->mIsConstant;
                if ( isConst != method.HasFlag(MethodInfo::Const) )
                {
                    wxString constStr;
                    if(isConst)constStr = _T("not ");

                    wxLogWarning("'%s::%s' is incorrectly documented as %s"
                                 "constant.",
                                 nameClass.c_str(),
                                 nameMethod.c_str(),
                                 constStr.c_str());
                }

                // check that the params match
                const MMemberListT& params = ctxMethod->GetMembers();

                if ( params.size() != method.GetParamCount() ) {
                    wxLogError("Incorrect number of parameters for '%s::%s' "
                               "in the docs: should be %d instead of %d.",
                               nameClass.c_str(),
                               nameMethod.c_str(),
                               (int)params.size(), (int)method.GetParamCount());
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
                             (param.GetName() != ctxParam->m_Name.c_str()) ) {
                            foundDiff = true;

                            wxLogError("Parameter #%d of '%s::%s' should be "
                                       "'%s' and not '%s'.",
                                       (int)(nParam + 1),
                                       nameClass.c_str(),
                                       nameMethod.c_str(),
                                       ctxParam->m_Name.c_str(),
                                       param.GetName().c_str());

                            continue;
                        }

                        if ( param.GetType() != ctxParam->m_Type ) {
                            foundDiff = true;

                            wxLogError("Type of parameter '%s' of '%s::%s' "
                                       "should be '%s' and not '%s'.",
                                       ctxParam->m_Name.c_str(),
                                       nameClass.c_str(),
                                       nameMethod.c_str(),
                                       ctxParam->m_Type.c_str(),
                                       param.GetType().GetName().c_str());

                            continue;
                        }

                        if ( param.GetDefValue() != ctxParam->m_InitVal.c_str() ) {
                            wxLogWarning("Default value of parameter '%s' of "
                                         "'%s::%s' should be '%s' and not "
                                         "'%s'.",
                                         ctxParam->m_Name.c_str(),
                                         nameClass.c_str(),
                                         nameMethod.c_str(),
                                         ctxParam->m_InitVal.c_str(),
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
                            methodExists[nMethod] = true;
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
                    foundDiff = true;

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
            foundDiff = true;

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
        return false;

    off_t len = file.Length();
    if ( len == wxInvalidOffset )
        return false;

    char *buf = new char[len + 1];
    buf[len] = '\0';

    if ( file.Read(buf, len) == wxInvalidOffset ) {
        delete [] buf;

        return false;
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
                    m_ignore.Add(new IgnoreListEntry(line, wxEmptyString));
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

    return true;
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

    if ( funcname ) {
        // special treatment for operatorXXX() stuff because the C operators
        // are not valid in LaTeX labels
        wxString oper;
        if ( wxString(funcname).StartsWith("operator", &oper) ) {
            label << "operator";

            static const struct
            {
                const char *oper;
                const char *name;
            } operatorNames[] =
            {
                { "=",  "assign" },
                { "==", "equal" },
            };

            size_t n;
            for ( n = 0; n < WXSIZEOF(operatorNames); n++ ) {
                if ( oper == operatorNames[n].oper ) {
                    label << operatorNames[n].name;

                    break;
                }
            }

            if ( n == WXSIZEOF(operatorNames) ) {
                wxLogWarning("unknown operator '%s' - making dummy label.",
                             oper.c_str());

                label << "unknown";
            }
        }
        else // simply use the func name
        {
            label << funcname;
        }
    }

    label.MakeLower();

    return label;
}

static wxString MakeHelpref(const char *argument)
{
    wxString helpref;
    helpref << "\\helpref{" << argument << "}{" << MakeLabel(argument) << '}';

    return helpref;
}

static void TeXFilter(wxString* str)
{
    // TeX special which can be quoted (don't include backslash nor braces as
    // we generate them
    static wxRegEx reNonSpecialSpecials("[#$%&_]"),
                   reAccents("[~^]");

    // just quote
    reNonSpecialSpecials.ReplaceAll(str, "\\\\\\0");

    // can't quote these ones as they produce accents when preceded by
    // backslash, so put them inside verb
    reAccents.ReplaceAll(str, "\\\\verb|\\0|");
}

static void TeXUnfilter(wxString* str)
{
    // FIXME may be done much more quickly
    str->Trim(true);
    str->Trim(false);

    // undo TeXFilter
    static wxRegEx reNonSpecialSpecials("\\\\([#$%&_{}])"),
                   reAccents("\\\\verb\\|([~^])\\|");

    reNonSpecialSpecials.ReplaceAll(str, "\\1");
    reAccents.ReplaceAll(str, "\\1");
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
        comment.Trim(false);
        if ( !comment.empty() &&
              comment == wxString(comment[0u], comment.length() - 1) + '\n' )
            comments << "\n";
        else
            comments << comment;
    }

    return comments;
}

static const char *GetCurrentTimeFormatted(const char *timeFormat)
{
    static char s_timeBuffer[128];
    time_t timeNow;
    struct tm *ptmNow;

    time(&timeNow);
    ptmNow = localtime(&timeNow);

    strftime(s_timeBuffer, WXSIZEOF(s_timeBuffer), timeFormat, ptmNow);

    return s_timeBuffer;
}

static const wxString GetVersionString()
{
    wxString version = "$Revision$";
    wxRegEx("^\\$Revision$$").ReplaceFirst(&version, "\\1");
    return version;
}

/*
   $Log$
   Revision 1.44  2005/05/31 17:47:45  ABX
   More warning and error fixes (work in progress with Tinderbox).

   Revision 1.43  2005/05/31 15:42:43  ABX
   More warning and error fixes (work in progress with Tinderbox).

   Revision 1.42  2005/05/31 15:32:49  ABX
   More warning and error fixes (work in progress with Tinderbox).

   Revision 1.41  2005/05/30 13:06:15  ABX
   More warning and error fixes (work in progress with Tinderbox).

   Revision 1.40  2005/05/30 11:49:32  ABX
   More warning and error fixes (work in progress with Tinderbox).

   Revision 1.39  2005/05/30 09:26:42  ABX
   More warning and error fixes (work in progress with Tinderbox).

   Revision 1.38  2005/05/24 09:06:20  ABX
   More fixes and wxWidgets coding standards.

   Revision 1.37  2005/05/23 15:22:08  ABX
   Initial HelpGen source cleaning.

   Revision 1.36  2005/04/07 19:54:58  MW
   Workarounds to allow compilation by Sun C++ 5.5

   Revision 1.35  2004/12/12 11:03:31  VZ
   give an error message if we're built in Unicode mode (in response to bug 1079224)

   Revision 1.34  2004/11/23 09:53:31  JS
   Changed GPL to wxWindows Licence

   Revision 1.33  2004/11/12 03:30:07  RL

   Cruft cleanup from MJW, strip the tabs out of sound.cpp

   Revision 1.32  2004/11/10 21:02:58  VZ
   new set of fixes for problems due to huge files support: drop wxFileSize_t, use wxFileOffset only, make wxInvalidOffset an int (main part of the patch 1063498)

   Revision 1.31  2004/10/05 15:38:29  ABX
   Warning fixes found under hardest mode of OpenWatcom. Seems clean in Borland, MinGW and DMC.

   Revision 1.30  2004/06/18 19:25:50  ABX
   Small step in making HelpGen up to date unicode application.

   Revision 1.29  2004/06/17 19:00:22  ABX
   Warning fixes. Code cleanup. Whitespaces and tabs removed.

   Revision 1.28  2004/05/25 11:19:57  JS
   More name changes

   Revision 1.27  2003/10/13 17:21:30  MBN
     Compilation fixes.

   Revision 1.26  2003/09/29 15:18:35  MBN
     (Blind) compilation fix for Sun compiler.

   Revision 1.25  2003/09/03 17:39:27  MBN
     Compilation fixes.

   Revision 1.24  2003/08/13 22:59:37  VZ
   compilation fix

   Revision 1.23  2003/06/13 17:05:43  VZ
   quote '|' inside regexes (fixes dump mode); fixed crash due to strange HelpGenApp code

   Revision 1.22  2002/01/21 21:18:50  JS
   Now adds 'include file' heading

   Revision 1.21  2002/01/04 11:06:09  JS
   Fixed missing membersections bug and also bug with functions not being written
   in the right class

   Revision 1.20  2002/01/03 14:23:33  JS
   Added code to make it not duplicate membersections for overloaded functions

   Revision 1.19  2002/01/03 13:34:12  JS
   Added FlushAll to CloseClass, otherwise text was only flushed right at the end,
   and appeared in one file.

   Revision 1.18  2002/01/03 12:02:47  JS
   Added main() and corrected VC++ project settings

   Revision 1.17  2001/11/30 21:43:35  VZ
   now the methods are sorted in the correct order in the generated docs

   Revision 1.16  2001/11/28 19:27:33  VZ
   HelpGen doesn't work in GUI mode

   Revision 1.15  2001/11/22 21:59:58  GD
   use "..." instead of <...> for wx headers

   Revision 1.14  2001/07/19 13:51:29  VZ
   fixes to version string

   Revision 1.13  2001/07/19 13:44:57  VZ
   1. compilation fixes
   2. don't quote special characters inside verbatim environment

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
    for wxWidgets documentation from C++ headers
*/

/* vi: set tw=80 et ts=4 sw=4: */
