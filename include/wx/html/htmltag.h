/////////////////////////////////////////////////////////////////////////////
// Name:        htmltag.h
// Purpose:     wxHtmlTag class (represents single tag)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_HTMLTAG_H_
#define _WX_HTMLTAG_H_

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#if wxUSE_HTML


//--------------------------------------------------------------------------------
// wxHtmlTagsCache
//                  !! INTERNAL STRUCTURE !! Do not use in your program!
//                  This structure contains information on positions of tags
//                  in the string being parsed
//--------------------------------------------------------------------------------

typedef struct {
        int Key;
            // this is "pos" value passed to wxHtmlTag's constructor.
            // it is position of '<' character of the tag
        int End1, End2;
            // end positions for the tag:
            // end1 is '<' of ending tag,
            // end2 is '>' or both are
            // -1 if there is no ending tag for this one...
            // or -2 if this is ending tag  </...>
        char *Name;
            // name of this tag
    } sCacheItem;



class wxHtmlTagsCache : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxHtmlTagsCache)

    private:
        sCacheItem *m_Cache;
        int m_CacheSize;
        int m_CachePos;

    public:
        wxHtmlTagsCache() : wxObject() {m_CacheSize = 0; m_Cache = NULL;}
        wxHtmlTagsCache(const wxString& source);
        ~wxHtmlTagsCache() {free(m_Cache);}

        void QueryTag(int at, int* end1, int* end2);
                // Finds parameters for tag starting at at and fills the variables
};



//--------------------------------------------------------------------------------
// wxHtmlTag
//                  This represents single tag. It is used as internal structure
//                  by wxHtmlParser.
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlTag : public wxObject
{
    DECLARE_CLASS(wxHtmlTag)

    public:
        wxHtmlTag(const wxString& source, int pos, int end_pos, wxHtmlTagsCache* cache);
                // constructs wxHtmlTag object based on HTML tag.
                // The tag begins (with '<' character) at position pos in source
                // end_pos is position where parsing ends (usually end of document)

        inline wxString GetName() const {return m_Name;}
                // Returns tag's name in uppercase.
        
        bool HasParam(const wxString& par) const;
                // Returns TRUE if the tag has given parameter. Parameter
                // should always be in uppercase.
                // Example : <IMG SRC="test.jpg"> HasParam("SRC") returns TRUE
        
        wxString GetParam(const wxString& par, bool with_commas = FALSE) const;
                // Returns value of the param. Value is in uppercase unless it is
                // enclosed with "
                // Example : <P align=right> GetParam("ALIGN") returns (RIGHT)
                //           <P IMG SRC="WhaT.jpg"> GetParam("SRC") returns (WhaT.jpg)
                //                           (or ("WhaT.jpg") if with_commas == TRUE)

        int ScanParam(const wxString& par, char *format, void *param) const;
                // Scans param like scanf() functions family do.
                // Example : ScanParam("COLOR", "\"#%X\"", &clr);
                // This is always with with_commas=FALSE
		// Returns number of scanned values
		// (like sscanf() does)
		// NOTE: unlike scanf family, this function only accepts
		//       *one* parameter !

        inline const wxString& GetAllParams() const {return m_Params;}
                // Returns string containing all params.
        
        inline bool IsEnding() const {return m_Ending;}
                // return TRUE if this is ending tag (</something>) or FALSE
                // if it isn't (<something>)
        
        inline bool HasEnding() const {return m_End1 >= 0;}
                // return TRUE if this is ending tag (</something>) or FALSE
                // if it isn't (<something>)

        inline int GetBeginPos() const {return m_Begin;}
                // returns beginning position of _internal_ block of text
                // See explanation (returned value is marked with *):
                // bla bla bla <MYTAG>* bla bla intenal text</MYTAG> bla bla
        inline int GetEndPos1() const {return m_End1;}
                // returns ending position of _internal_ block of text.
                // bla bla bla <MYTAG> bla bla intenal text*</MYTAG> bla bla
        inline int GetEndPos2() const {return m_End2;}
                // returns end position 2 :
                // bla bla bla <MYTAG> bla bla internal text</MYTAG>* bla bla

    private:
        wxString m_Name, m_Params;
        int m_Begin, m_End1, m_End2;
        bool m_Ending;

};





#endif

#endif // _WX_HTMLTAG_H_

