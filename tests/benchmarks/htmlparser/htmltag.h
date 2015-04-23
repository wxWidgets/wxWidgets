/////////////////////////////////////////////////////////////////////////////
// Name:        htmltag.h
// Purpose:     wx28HtmlTag class (represents single tag)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HTMLTAG_H_
#define _WX_HTMLTAG_H_

#include "wx/defs.h"

#include "wx/object.h"
#include "wx/arrstr.h"

class  wx28HtmlEntitiesParser;

//-----------------------------------------------------------------------------
// wx28HtmlTagsCache
//          - internal wxHTML class, do not use!
//-----------------------------------------------------------------------------

struct wx28HtmlCacheItem;

class  wx28HtmlTagsCache : public wxObject
{
    wxDECLARE_DYNAMIC_CLASS(wx28HtmlTagsCache);

private:
    wx28HtmlCacheItem *m_Cache;
    int m_CacheSize;
    int m_CachePos;

public:
    wx28HtmlTagsCache() : wxObject() {m_CacheSize = 0; m_Cache = NULL;}
    wx28HtmlTagsCache(const wxString& source);
    virtual ~wx28HtmlTagsCache() {free(m_Cache);}

    // Finds parameters for tag starting at at and fills the variables
    void QueryTag(int at, int* end1, int* end2);

    wxDECLARE_NO_COPY_CLASS(wx28HtmlTagsCache);
};


//--------------------------------------------------------------------------------
// wx28HtmlTag
//                  This represents single tag. It is used as internal structure
//                  by wx28HtmlParser.
//--------------------------------------------------------------------------------

class  wx28HtmlTag : public wxObject
{
    wxDECLARE_CLASS(wx28HtmlTag);

protected:
    // constructs wx28HtmlTag object based on HTML tag.
    // The tag begins (with '<' character) at position pos in source
    // end_pos is position where parsing ends (usually end of document)
    wx28HtmlTag(wx28HtmlTag *parent,
              const wxString& source, int pos, int end_pos,
              wx28HtmlTagsCache *cache,
              wx28HtmlEntitiesParser *entParser);
    friend class wx28HtmlParser;
public:
    virtual ~wx28HtmlTag();

    wx28HtmlTag *GetParent() const {return m_Parent;}
    wx28HtmlTag *GetFirstSibling() const;
    wx28HtmlTag *GetLastSibling() const;
    wx28HtmlTag *GetChildren() const { return m_FirstChild; }
    wx28HtmlTag *GetPreviousSibling() const { return m_Prev; }
    wx28HtmlTag *GetNextSibling() const {return m_Next; }
    // Return next tag, as if tree had been flattened
    wx28HtmlTag *GetNextTag() const;

    // Returns tag's name in uppercase.
    inline wxString GetName() const {return m_Name;}

    // Returns true if the tag has given parameter. Parameter
    // should always be in uppercase.
    // Example : <IMG SRC="test.jpg"> HasParam("SRC") returns true
    bool HasParam(const wxString& par) const;

    // Returns value of the param. Value is in uppercase unless it is
    // enclosed with "
    // Example : <P align=right> GetParam("ALIGN") returns (RIGHT)
    //           <P IMG SRC="WhaT.jpg"> GetParam("SRC") returns (WhaT.jpg)
    //                           (or ("WhaT.jpg") if with_commas == true)
    wxString GetParam(const wxString& par, bool with_commas = false) const;

    bool GetParamAsInt(const wxString& par, int *clr) const;

    // Scans param like scanf() functions family does.
    // Example : ScanParam("COLOR", "\"#%X\"", &clr);
    // This is always with with_commas=false
    // Returns number of scanned values
    // (like sscanf() does)
    // NOTE: unlike scanf family, this function only accepts
    //       *one* parameter !
    int ScanParam(const wxString& par, const wxChar *format, void *param) const;

    // Returns string containing all params.
    wxString GetAllParams() const;

    // return true if this there is matching ending tag
    inline bool HasEnding() const {return m_End1 >= 0;}

    // returns beginning position of _internal_ block of text
    // See explanation (returned value is marked with *):
    // bla bla bla <MYTAG>* bla bla intenal text</MYTAG> bla bla
    inline int GetBeginPos() const {return m_Begin;}
    // returns ending position of _internal_ block of text.
    // bla bla bla <MYTAG> bla bla intenal text*</MYTAG> bla bla
    inline int GetEndPos1() const {return m_End1;}
    // returns end position 2 :
    // bla bla bla <MYTAG> bla bla internal text</MYTAG>* bla bla
    inline int GetEndPos2() const {return m_End2;}

private:
    wxString m_Name;
    int m_Begin, m_End1, m_End2;
    wxArrayString m_ParamNames, m_ParamValues;

    // DOM tree relations:
    wx28HtmlTag *m_Next;
    wx28HtmlTag *m_Prev;
    wx28HtmlTag *m_FirstChild, *m_LastChild;
    wx28HtmlTag *m_Parent;

    wxDECLARE_NO_COPY_CLASS(wx28HtmlTag);
};



#endif // _WX_HTMLTAG_H_

