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
   TODO (+ means fixed)

   (i) small fixes in the current version
   
   +1. Quote special TeX characters like '&' and '_' (=> derive from wxFile)
    2. Document typedefs
    3. Document global variables
    4. Document #defines
   +5. Program options

   (ii) plans for version 2
    1. Use wxTextFile for direct file access to avoid one scan method problems
    2. Use command line parsrer class for the options
   
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
#endif // WX_PRECOMP

#include <wx/file.h>

// C++ parsing classes
#include "cjparser.h"

// standard headers
#include <stdio.h>
#include <time.h>

// -----------------------------------------------------------------------------
// private functions
// -----------------------------------------------------------------------------

// return the label for the given function name (i.e. argument of \label)
static wxString MakeLabel(const char *classname, const char *funcname = NULL);

// return the whole \helpref{arg}{arg_label} string
static wxString MakeHelpref(const char *argument);

// quotes special TeX characters in place
static void TeXFilter(wxString* str);

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
    wxTeXFile() : wxFile() { }

    bool WriteTeX(const wxString& s)
    {
        wxString t(s);
        TeXFilter(&t);

        return wxFile::Write(t);
    }
};

class HelpGenVisitor : public spVisitor
{
public:
    // ctor
    HelpGenVisitor();

    virtual void VisitFile( spFile& fl );
    virtual void VisitClass( spClass& cl );
    virtual void VisitEnumeration( spEnumeration& en );
    virtual void VisitTypeDef( spTypeDef& td );
	virtual void VisitPreprocessorLine( spPreprocessorLine& pd );
    virtual void VisitAttribute( spAttribute& attr );
    virtual void VisitOperation( spOperation& op );
    virtual void VisitParameter( spParameter& param );

    void EndVisit();

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

    wxTeXFile m_file;  // file we're writing to now

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
    wxLogError("usage: HelpGen [-q|-v] <header files...>\n");

    exit(1);
}

int main(int argc, char **argv)
{
    if ( argc < 2 ) {
        usage();
    }

    int first;
    for ( first = 1; (first < argc) && argv[first][0] == '-'; first++ ) {
        switch ( argv[first][1] ) {
            case 'v':
                // be verbose
                wxLog::GetActiveTarget()->SetVerbose();
                break;

            case 'q':
                // be quiet
                wxLog::GetActiveTarget()->SetVerbose(false);
                break;

            default:
                usage();
        }
    }

    // create a parser object and a visitor derivation
    CJSourceParser parser;
    HelpGenVisitor visitor;

    // parse all files
    for ( int i = first; i < argc; i++ ) {
        spContext *ctxTop = parser.ParseFile(argv[i]);
        if ( !ctxTop ) {
            wxLogWarning("File '%s' couldn't be processed.", argv[i]);
        }
        else {
            ((spFile *)ctxTop)->mFileName = argv[i];
            visitor.VisitAll(*ctxTop);
            visitor.EndVisit();
        }
    }

    return 0;
}

// -----------------------------------------------------------------------------
// HelpGenVisitor implementation
// -----------------------------------------------------------------------------

HelpGenVisitor::HelpGenVisitor()
{
    Reset();
}

void HelpGenVisitor::Reset()
{
    m_inClass =
    m_inFunction =
    m_inTypesSection =
    m_inMethodSection = false;

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
        m_inTypesSection = true;

        m_file.WriteTeX("\\wxheading{Data structures}\n\n");
    }
}

void HelpGenVisitor::InsertMethodsHeader()
{
    if ( !m_inMethodSection ) {
        m_inMethodSection = true;

        m_file.WriteTeX( "\\latexignore{\\rtfignore{\\wxheading{Members}}}\n\n");
    }
}

void HelpGenVisitor::CloseFunction()
{
    if ( m_inFunction ) {
        m_inFunction = false;

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

    wxLogInfo("%s: finished parsing the current file.",
              GetCurrentTime("%H:%M:%S"));
}

void HelpGenVisitor::VisitFile( spFile& file )
{
    wxLogInfo("%s: started to parse classes from file '%s'...",
              GetCurrentTime("%H:%M:%S"), file.mFileName.c_str());
}

void HelpGenVisitor::VisitClass( spClass& cl )
{
    wxString name = cl.GetName();

    // the file name is built from the class name by removing the leading "wx"
    // if any and converting it to the lower case
    wxString filename = name;
    if ( filename(0, 2) == "wx" ) {
        filename.erase(0, 2);
    }

    filename.MakeLower();
    filename += ".tex";

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

    // the entire text we're writing to file
    wxString totalText;

    // write out the header
    {
        wxString header;
        header.Printf("% automatically generated by HelpGen from %s at %s\n"
                      "\\section{\\class{%s}}\\label{%s}\n",
                      filename.c_str(), GetCurrentTime("%d/%b/%y %H:%M:%S"),
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

    if ( !m_inClass || !op.IsInClass() ) {
        // FIXME that's a bug too
        wxLogWarning("skipped global function '%s'.", op.GetName().c_str());

        return;
    }

    if ( op.mVisibility == SP_VIS_PRIVATE ) {
        // FIXME should we document protected functions?
        return;
    }

    InsertMethodsHeader();

    // save state info
    m_inFunction =
    m_isFirstParam = true;

    m_textStoredFunctionComment = GetAllComments(op);

    // start function documentation
    wxString totalText;
    const char *funcname = op.GetName().c_str();
    const char *classname = op.GetClass().GetName().c_str();
               
    // check for the special case of dtor
    wxString dtor;
    if ( (funcname[0] == '~') && (strcmp(funcname + 1, classname) == 0) ) {
        dtor.Printf("\\destruct{%s}", classname);
        funcname = dtor;
    }

    totalText.Printf("\\membersection{%s::%s}\\label{%s}\n\n"
                     "\\%sfunc{%s%s}{%s}{",
                     classname, funcname,
                     MakeLabel(classname, funcname).c_str(),
                     op.mIsConstant ? "const" : "",
                     op.mIsVirtual ? "virtual " : "",
                     op.mRetType.c_str(),
                     funcname);

    m_file.WriteTeX(totalText);
}

void HelpGenVisitor::VisitParameter( spParameter& param )
{
    if ( !m_inFunction )
        return;

    wxString totalText;
    if ( m_isFirstParam ) {
        m_isFirstParam = false;
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

static void TeXFilter(wxString* str)
{
    // FIXME may be done much more quickly
    str->Replace("&", "\\&");
    str->Replace("_", "\\_");
}

static wxString GetAllComments(const spContext& ctx)
{
    wxString comment;
    const MCommentListT& comments = ctx.GetCommentList();
    for ( MCommentListT::const_iterator i = comments.begin();
            i != comments.end();
            i++ ) {
        comment << (*i)->GetText();
    }

    return comment;
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

/* vi: set tw=80 et ts=4 sw=4: */
