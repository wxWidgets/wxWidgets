/////////////////////////////////////////////////////////////////////////////
// Name:        htmlwinparser.h
// Purpose:     wxHtmlWinParser class (parser to be used with wxHtmlWindow)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __HTMLWINPARSER_H__
#define __HTMLWINPARSER_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#if wxUSE_HTML

#include <wx/module.h>
#include <wx/html/htmlparser.h>
#include <wx/html/htmlcell.h>

class wxHtmlWinParser;
class wxHtmlWinTagHandler;
class wxHtmlTagsModule;

//--------------------------------------------------------------------------------
// wxHtmlWinParser
//                  This class is derived from wxHtmlParser and its mail goal
//                  is to parse HTML input so that it can be displayed in
//                  wxHtmlWindow. It uses special wxHtmlWinTagHandler.
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlWinParser : public wxHtmlParser
{
    DECLARE_DYNAMIC_CLASS(wxHtmlWinParser)

    friend class wxHtmlWindow;

    private:
        wxWindow *m_Window;
                // window we're parsing for
        wxDC *m_DC;
                // Device Context we're parsing for
        static wxList m_Modules;
                // list of tags modules (see wxHtmlTagsModule for details)
                // This list is used to initialize m_Handlers member.

        wxHtmlContainerCell *m_Container;
                // actual container. See Open/CloseContainer for details.

        int m_FontBold, m_FontItalic, m_FontUnderlined, m_FontFixed; // this is not TRUE,FALSE but 1,0, we need it for indexing
        int m_FontSize; /* -2 to +4,  0 is default */
        wxColour m_LinkColor;
        wxColour m_ActualColor;
                // basic font parameters.
        wxString m_Link;
                // actual hypertext link or empty string
        bool m_UseLink;
                // TRUE if m_Link is not empty
        long m_CharHeight, m_CharWidth;
                // average height of normal-sized text
        int m_Align;
                // actual alignment

        wxFont *m_FontsTable[2][2][2][2][7];
                // table of loaded fonts. 1st four indexes are 0 or 1, depending on on/off
                // state of these flags (from left to right):
                // [bold][italic][underlined][fixed_size]
                // last index is font size : from 0 to 7 (remapped from html sizes -2 to +4)
                // Note : this table covers all possible combinations of fonts, but not
                // all of them are used, so many items in table are usually NULL.
        int m_FontsSizes[7];
        wxString m_FontFaceFixed, m_FontFaceNormal;
        int m_ItalicModeFixed, m_ItalicModeNormal;
                // html font sizes and faces of fixed and proportional fonts

    public:
        wxHtmlWinParser() : wxHtmlParser() {wxHtmlWinParser(NULL);}
        wxHtmlWinParser(wxWindow *wnd);

        virtual void InitParser(const wxString& source);
        virtual void DoneParser();
        virtual wxObject* GetProduct();

        virtual void SetDC(wxDC *dc) {m_DC = dc;}
                // Set's the DC used for parsing. If SetDC() is not called,
                // parsing won't proceed
        wxDC *GetDC() {return m_DC;}
        int GetCharHeight() const {return m_CharHeight;}
        int GetCharWidth() const {return m_CharWidth;}
            // NOTE : these functions do _not_ return _actual_
        // height/width. They return h/w of default font
        // for this DC. If you want actual values, call
        // GetDC() -> GetChar...()
        wxWindow *GetWindow() {return m_Window;}
                // returns associated wxWindow

        void SetFonts(wxString normal_face, int normal_italic_mode, wxString fixed_face, int fixed_italic_mode, int *sizes);
                // sets fonts to be used when displaying HTML page.
                // *_italic_mode can be either wxSLANT or wxITALIC
    
        virtual wxList* GetTempData();

        static void AddModule(wxHtmlTagsModule *module);
                // Adds tags module. see wxHtmlTagsModule for details.

        // parsing-related methods. These methods are called by tag handlers:
        wxHtmlContainerCell *GetContainer() const {return m_Container;}
            // Returns pointer to actual container. Common use in tag handler is :
            // m_WParser -> GetContainer() -> InsertCell(new ...);
        wxHtmlContainerCell *OpenContainer();
            // opens new container. This container is sub-container of opened
            // container. Sets GetContainer to newly created container
            // and returns it.
        wxHtmlContainerCell *SetContainer(wxHtmlContainerCell *c);
            // works like OpenContainer except that new container is not created
            // but c is used. You can use this to directly set actual container
        wxHtmlContainerCell *CloseContainer();
            // closes the container and sets actual Container to upper-level
            // container

        int GetFontSize() const {return m_FontSize;}
        void SetFontSize(int s) {m_FontSize = s;}
        int GetFontBold() const {return m_FontBold;}
        void SetFontBold(int x) {m_FontBold = x;}
        int GetFontItalic() const {return m_FontItalic;}
        void SetFontItalic(int x) {m_FontItalic = x;}
        int GetFontUnderlined() const {return m_FontUnderlined;}
        void SetFontUnderlined(int x) {m_FontUnderlined = x;}
        int GetFontFixed() const {return m_FontFixed;}
        void SetFontFixed(int x) {m_FontFixed = x;}

        int GetAlign() const {return m_Align;}
        void SetAlign(int a) {m_Align = a;}
        const wxColour& GetLinkColor() const {return m_LinkColor;}
        void SetLinkColor(const wxColour& clr) {m_LinkColor = clr;}
        const wxColour& GetActualColor() const {return m_ActualColor;}
        void SetActualColor(const wxColour& clr) {m_ActualColor = clr;}
        const wxString& GetLink() const {return m_Link;}
        void SetLink(const wxString& link) {m_Link = link; m_UseLink = link.Length() > 0;}

        virtual wxFont* CreateCurrentFont();
            // creates font depending on m_Font* members.
            // (note : it calls wxHtmlWindow's CreateCurrentFont...)

    protected:
        virtual void AddText(const char *txt);

    private:
        bool m_tmpLastWasSpace;
            // temporary variable used by AddText
};






//--------------------------------------------------------------------------------
// wxHtmlWinTagHandler
//                  This is basicly wxHtmlTagHandler except
//                  it is extended with protected member m_Parser pointing to
//                  the wxHtmlWinParser object
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlWinTagHandler : public wxHtmlTagHandler
{
    DECLARE_ABSTRACT_CLASS(wxHtmlWinTagHandler)

    protected:
        wxHtmlWinParser *m_WParser;
                // same as m_Parser, but overcasted

    public:
        wxHtmlWinTagHandler() : wxHtmlTagHandler() {};
        
        virtual void SetParser(wxHtmlParser *parser) {wxHtmlTagHandler::SetParser(parser); m_WParser = (wxHtmlWinParser*) parser;};
};






//--------------------------------------------------------------------------------
// wxHtmlTagsModule
//                  This is basic of dynamic tag handlers binding.
//                  The class provides methods for filling parser's handlers
//                  hash table.
//                  (See documentation for details)
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlTagsModule : public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxHtmlTagsModule)

    public:
        wxHtmlTagsModule() : wxModule() {};

        virtual bool OnInit();
        virtual void OnExit();

        virtual void FillHandlersTable(wxHtmlWinParser *parser) {}
                // This is called by wxHtmlWinParser.
                // The method must simply call parser->AddTagHandler(new <handler_class_name>);
                // for each handler

};



#endif // __HTMLWINPARSER_H__

#endif



