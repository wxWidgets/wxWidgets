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

   (ii) plans for version 2
    1. Use wxTextFile for direct file access to avoid one scan method problems
   
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
    #include <wx/file.h>
#endif // WX_PRECOMP

// C++ parsing classes
#include "cjparser.h"

// standard headers
#include <stdio.h>
#include <time.h>

// -----------------------------------------------------------------------------
// private functions
// -----------------------------------------------------------------------------

// return the label for the given function name
static wxString MakeLabel(const char *classname, const char *funcname);
    
// quotes special TeX characters in place
static void TeXFilter(wxString* str);

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
};

// -----------------------------------------------------------------------------
// private functions
// -----------------------------------------------------------------------------

// =============================================================================
// implementation
// =============================================================================

int main(int argc, char **argv)
{
    if ( argc < 2 ) {
        wxLogError("usage: %s <header files...>\n", argv[0]);

        return 1;
    }

    // be verbose
    wxLog::GetActiveTarget()->SetVerbose();

    // create a parser object and a visitor derivation
    CJSourceParser parser;
    HelpGenVisitor visitor;

    // parse all files
    for ( int i = 1; i < argc; i++ ) {
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
}

void HelpGenVisitor::VisitFile( spFile& file )
{
    wxLogInfo("Parsing classes from file '%s'...", file.mFileName.c_str());
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
        time_t timeNow = time(NULL);
        wxString header;
        header.Printf("% automatically generated by HelpGen from %s at "
                      "%s" // no '\n' here because ctime() inserts one
                      "\\section{\\class{%s}}\\label{%s}\n",
                      filename.c_str(), ctime(&timeNow),
                      name.c_str(), wxString(name).MakeLower().c_str());

        totalText << header << '\n';
    }

    // the comment before the class generally explains what is it for so put it
    // in place of the class description
    if ( cl.HasComments() ) {
        wxString comment;
        const MCommentListT& comments = cl.GetCommentList();
        for ( MCommentListT::const_iterator i = comments.begin();
              i != comments.end();
              i++ ) {
            comment << (*i)->GetText();
        }

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
            derived << "\\helpref{" << baseclass << "}"
                       "{ " << baseclass.MakeLower()  << "}";
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
    wxString enumeration;
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

    wxFAIL_MSG("don't know how to document typedefs yet");
}

void HelpGenVisitor::VisitAttribute( spAttribute& attr )
{
    CloseFunction();

    // only document the public member variables
    if ( !m_inClass || !attr.IsPublic() )
        return;

    wxFAIL_MSG("don't know how to document member vars yet");
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

    m_textStoredFunctionComment.Empty();
    const MCommentListT& comments = op.GetCommentList();
    for ( MCommentListT::const_iterator i = comments.begin();
          i != comments.end();
          i++ ) {
        m_textStoredFunctionComment << (*i)->GetText();
    }

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

    totalText.Printf("\\membersection{%s::%s}\\label{%s}\n"
                     "\\%sfunc{%s}{%s}{",
                     classname, funcname,
                     MakeLabel(classname, funcname).c_str(),
                     op.mIsConstant ? "const" : "",
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
    if ( funcname[0] == '\\' ) {
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

    label << funcname;

    label.MakeLower();

    return label;
}

static void TeXFilter(wxString* str)
{
    // FIXME may be done much more quickly
    str->Replace("&", "\\&");
    str->Replace("_", "\\_");
}

/* vi: set tw=80 et ts=4 sw=4: */
