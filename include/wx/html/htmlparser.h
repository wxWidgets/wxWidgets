/////////////////////////////////////////////////////////////////////////////
// Name:        htmlparser.h
// Purpose:     wxHtmlParser class (generic parser)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __HTMLPARSER_H__
#define __HTMLPARSER_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#if wxUSE_HTML

#include <wx/html/htmltag.h>
#include <wx/filesys.h>

class wxHtmlParser;
class wxHtmlTagHandler;

//--------------------------------------------------------------------------------
// wxHtmlParser
//                  This class handles generic parsing of HTML document : it scans
//                  the document and divide it into blocks of tags (where one block
//                  consists of starting and ending tag and of text between these
//                  2 tags.
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlParser : public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxHtmlParser)

    protected:
        wxString m_Source;
                // source being parsed
        wxHtmlTagsCache *m_Cache;
                // tags cache, used during parsing.
        wxHashTable m_HandlersHash;
        wxList m_HandlersList;
                // handlers that handle particular tags. The table is accessed by
                // key = tag's name.
                // This attribute MUST be filled by derived class otherwise it would
                // be empty and no tags would be recognized
                // (see wxHtmlWinParser for details about filling it)
                // m_HandlersHash is for random access based on knowledge of tag name (BR, P, etc.)
                //      it may (and often does) contain more references to one object
                // m_HandlersList is list of all handlers and it is guaranteed to contain
                //      only one reference to each handler instance.
        wxFileSystem *m_FS;
                // class for opening files (file system)

    public:
        wxHtmlParser() : wxObject(), m_HandlersHash(wxKEY_STRING) {m_FS = NULL; m_Cache = NULL;}
        virtual ~wxHtmlParser();

        void SetFS(wxFileSystem *fs) {m_FS = fs;}
                // Sets the class which will be used for opening files
        wxFileSystem* GetFS() const {return m_FS;}

        wxObject* Parse(const wxString& source);
                // You can simply call this method when you need parsed output.
                // This method does these things:
                // 1. call InitParser(source);
                // 2. call DoParsing();
                // 3. call GetProduct(); (it's return value is then returned)
                // 4. call DoneParser();

        virtual void InitParser(const wxString& source);
                // Sets the source. This must be called before running Parse() method.
        virtual void DoneParser();
                // This must be called after Parse().
        
        void DoParsing(int begin_pos, int end_pos);
        inline void DoParsing() {DoParsing(0, m_Source.Length());};
                // Parses the m_Source from begin_pos to end_pos-1.
                // (in noparams version it parses whole m_Source)

        virtual wxObject* GetProduct() = 0;
                // Returns product of parsing
                // Returned value is result of parsing of the part. The type of this result
                // depends on internal representation in derived parser
                // (see wxHtmlWinParser for details).

        virtual void AddTagHandler(wxHtmlTagHandler *handler);
                // adds handler to the list & hash table of handlers.

        wxString* GetSource() {return &m_Source;}

        virtual wxList* GetTempData() {return NULL;}
                // this method returns list of wxObjects that represents
                // all data allocated by the parser. These can't be freeded
                // by destructor because they must be valid as long as
                // GetProduct's return value is valid - the caller must
                // explicitly call delete MyParser -> GetTempData() to free
                // the memory
                // (this method always sets the list to delete its contents)

    protected:

        virtual void AddText(const char* txt) = 0;
                // Adds text to the output.
                // This is called from Parse() and must be overriden in derived classes.
                // txt is not guaranteed to be only one word. It is largest continuous part of text
                // (= not broken by tags)
                // NOTE : using char* because of speed improvements

        virtual void AddTag(const wxHtmlTag& tag);
                // Adds tag and proceeds it. Parse() may (and usually is) called from this method.
                // This is called from Parse() and may be overriden.
                // Default behavior is that it looks for proper handler in m_Handlers. The tag is
                // ignored if no hander is found.
                // Derived class is *responsible* for filling in m_Handlers table.
};






//--------------------------------------------------------------------------------
// wxHtmlTagHandler
//                  This class (and derived classes) cooperates with wxHtmlParser.
//                  Each recognized tag is passed to handler which is capable
//                  of handling it. Each tag is handled in 3 steps:
//                  1. Handler will modifies state of parser
//                    (using it's public methods)
//                  2. Parser parses source between starting and ending tag
//                  3. Handler restores original state of the parser
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlTagHandler : public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxHtmlTagHandler)

    protected:
        wxHtmlParser *m_Parser;

    public:
        wxHtmlTagHandler() : wxObject () {m_Parser = NULL;};
        
        virtual void SetParser(wxHtmlParser *parser) {m_Parser = parser;}
                // Sets the parser.
                // NOTE : each _instance_ of handler is guaranteed to be called
                // only by one parser. This means you don't have to care about
                // reentrancy.
        
        virtual wxString GetSupportedTags() = 0;
                // Returns list of supported tags. The list is in uppercase and
                // tags are delimited by ','.
                // Example : "I,B,FONT,P"
                //   is capable of handling italic, bold, font and paragraph tags
        
        virtual bool HandleTag(const wxHtmlTag& tag) = 0;
                // This is hadling core method. It does all the Steps 1-3.
                // To process step 2, you can call ParseInner()
                // returned value : TRUE if it called ParseInner(),
                //                  FALSE etherwise

    protected:
        void ParseInner(const wxHtmlTag& tag) {m_Parser -> DoParsing(tag.GetBeginPos(), tag.GetEndPos1());}
                // parses input between beginning and ending tag.
                // m_Parser must be set.
};





#endif // __HTMLPARSER_H__

#endif
