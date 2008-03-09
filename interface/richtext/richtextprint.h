/////////////////////////////////////////////////////////////////////////////
// Name:        richtext/richtextprint.h
// Purpose:     documentation for wxRichTextHeaderFooterData class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRichTextHeaderFooterData
    @headerfile richtextprint.h wx/richtext/richtextprint.h


    This class represents header and footer data to be passed to the
    wxRichTextPrinting and
    wxRichTextPrintout classes.

    Headers and footers can be specified independently for odd, even or both page
    sides. Different text can be specified
    for left, centre and right locations on the page, and the font and text colour
    can also
    be specified. You can specify the following keywords in header and footer text,
    which will
    be substituted for the actual values during printing and preview.

     @DATE@: the current date.
     @PAGESCNT@: the total number of pages.
     @PAGENUM@: the current page number.
     @TIME@: the current time.
     @TITLE@: the title of the document, as passed to the wxRichTextPrinting or
    wxRichTextLayout constructor.

    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextHeaderFooterData : public wxObject
{
public:
    //@{
    /**
        Constructors.
    */
    wxRichTextHeaderFooterData();
    wxRichTextHeaderFooterData(const wxRichTextHeaderFooterData& data);
    //@}

    /**
        Clears all text.
    */
    void Clear();

    /**
        Copies the data.
    */
    void Copy(const wxRichTextHeaderFooterData& data);

    /**
        Returns the font specified for printing the header and footer.
    */
    const wxFont GetFont();

    /**
        Returns the margin between the text and the footer.
    */
    int GetFooterMargin();

    /**
        Returns the footer text on odd or even pages, and at a given position on the
        page (left, centre or right).
    */
    wxString GetFooterText(wxRichTextOddEvenPage page = wxRICHTEXT_PAGE_EVEN,
                           wxRichTextPageLocation location = wxRICHTEXT_PAGE_CENTRE);

    /**
        Returns the margin between the text and the header.
    */
    int GetHeaderMargin();

    /**
        Returns the header text on odd or even pages, and at a given position on the
        page (left, centre or right).
    */
    wxString GetHeaderText(wxRichTextOddEvenPage page = wxRICHTEXT_PAGE_EVEN,
                           wxRichTextPageLocation location = wxRICHTEXT_PAGE_CENTRE);

    /**
        Returns @true if the header and footer will be shown on the first page.
    */
    bool GetShowOnFirstPage();

    /**
        Helper function for getting the header or footer text, odd or even pages, and
        at a given position on the page (left, centre or right).
    */
    wxString GetText(int headerFooter, wxRichTextOddEvenPage page,
                     wxRichTextPageLocation location);

    /**
        Returns the text colour for drawing the header and footer.
    */
    const wxColour GetTextColour();

    /**
        Initialises the object.
    */
    void Init();

    /**
        Sets the font for drawing the header and footer.
    */
    void SetFont(const wxFont& font);

    /**
        Sets the footer text on odd or even pages, and at a given position on the page
        (left, centre or right).
    */
    void SetFooterText(const wxString& text,
                       wxRichTextOddEvenPage page = wxRICHTEXT_PAGE_ALL,
                       wxRichTextPageLocation location = wxRICHTEXT_PAGE_CENTRE);

    /**
        Sets the header text on odd or even pages, and at a given position on the page
        (left, centre or right).
    */
    void SetHeaderText(const wxString& text,
                       wxRichTextOddEvenPage page = wxRICHTEXT_PAGE_ALL,
                       wxRichTextPageLocation location = wxRICHTEXT_PAGE_CENTRE);

    /**
        Sets the margins between text and header or footer, in tenths of a millimeter.
    */
    void SetMargins(int headerMargin, int footerMargin);

    /**
        Pass @true to show the header or footer on first page (the default).
    */
    void SetShowOnFirstPage(bool showOnFirstPage);

    /**
        Helper function for setting the header or footer text, odd or even pages, and
        at a given position on the page (left, centre or right).
    */
    void SetText(const wxString& text, int headerFooter,
                 wxRichTextOddEvenPage page,
                 wxRichTextPageLocation location);

    /**
        Sets the text colour for drawing the header and footer.
    */
    void SetTextColour(const wxColour& col);

    /**
        Assignment operator.
    */
    void operator operator=(const wxRichTextHeaderFooterData& data);
};


/**
    @class wxRichTextPrintout
    @headerfile richtextprint.h wx/richtext/richtextprint.h

    This class implements print layout for wxRichTextBuffer. Instead of using it
    directly, you
    should normally use the wxRichTextPrinting class.

    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextPrintout : public wxPrintout
{
public:
    /**
        )
        Constructor.
    */
    wxRichTextPrintout();

    /**
        Calculates scaling and text, header and footer rectangles.
    */
    void CalculateScaling(wxDC* dc, wxRect& textRect,
                          wxRect& headerRect,
                          wxRect& footerRect);

    /**
        Returns the header and footer data associated with the printout.
    */
    const wxRichTextHeaderFooterData GetHeaderFooterData();

    /**
        Gets the page information.
    */
    void GetPageInfo(int* minPage, int* maxPage, int* selPageFrom,
                     int* selPageTo);

    /**
        Returns a pointer to the buffer being rendered.
    */
    wxRichTextBuffer* GetRichTextBuffer();

    /**
        Returns @true if the given page exists in the printout.
    */
    bool HasPage(int page);

    /**
        Prepares for printing, laying out the buffer and calculating pagination.
    */
    void OnPreparePrinting();

    /**
        Does the actual printing for this page.
    */
    bool OnPrintPage(int page);

    /**
        Sets the header and footer data associated with the printout.
    */
    void SetHeaderFooterData(const wxRichTextHeaderFooterData& data);

    /**
        Sets margins in 10ths of millimetre. Defaults to 1 inch for margins.
    */
    void SetMargins(int top = 252, int bottom = 252, int left = 252,
                    int right = 252);

    /**
        Sets the buffer to print. wxRichTextPrintout does not manage this pointer; it
        should
        be managed by the calling code, such as wxRichTextPrinting.
    */
    void SetRichTextBuffer(wxRichTextBuffer* buffer);
};


/**
    @class wxRichTextPrinting
    @headerfile richtextprint.h wx/richtext/richtextprint.h

    This class provides a simple interface for performing wxRichTextBuffer printing
    and previewing. It uses wxRichTextPrintout for layout and rendering.

    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextPrinting : public wxObject
{
public:
    /**
        , @b wxWindow*@e parentWindow = @NULL)
        Constructor. Optionally pass a title to be used in the preview frame and
        printing wait dialog, and
        also a parent window for these windows.
    */
    wxRichTextPrinting();

    /**
        A convenience function to get the footer text. See wxRichTextHeaderFooterData
        for details.
    */
    wxString GetFooterText(wxRichTextOddEvenPage page = wxRICHTEXT_PAGE_EVEN,
                           wxRichTextPageLocation location = wxRICHTEXT_PAGE_CENTRE);

    /**
        Returns the internal wxRichTextHeaderFooterData object.
    */
    const wxRichTextHeaderFooterData GetHeaderFooterData();

    /**
        A convenience function to get the header text. See wxRichTextHeaderFooterData
        for details.
    */
    wxString GetHeaderText(wxRichTextOddEvenPage page = wxRICHTEXT_PAGE_EVEN,
                           wxRichTextPageLocation location = wxRICHTEXT_PAGE_CENTRE);

    /**
        Returns a pointer to the internal page setup data.
    */
    wxPageSetupDialogData* GetPageSetupData();

    /**
        Returns the parent window to be used for the preview window and printing wait
        dialog.
    */
    wxWindow* GetParentWindow();

    /**
        Returns the dimensions to be used for the preview window.
    */
    const wxRect GetPreviewRect();

    /**
        Returns a pointer to the internal print data.
    */
    wxPrintData* GetPrintData();

    /**
        Returns the title of the preview window or printing wait caption.
    */
    const wxString GetTitle();

    /**
        Shows the page setup dialog.
    */
    void PageSetup();

    /**
        Shows a preview window for the given buffer. The function takes its own copy of
        @e buffer.
    */
    bool PreviewBuffer(const wxRichTextBuffer& buffer);

    /**
        Shows a preview window for the given file. @a richTextFile can be a text file
        or XML file, or other file
        depending on the available file handlers.
    */
    bool PreviewFile(const wxString& richTextFile);

    /**
        Prints the given buffer. The function takes its own copy of @e buffer.
    */
    bool PrintBuffer(const wxRichTextBuffer& buffer);

    /**
        Prints the given file. @a richTextFile can be a text file or XML file, or other
        file
        depending on the available file handlers.
    */
    bool PrintFile(const wxString& richTextFile);

    /**
        A convenience function to set the footer text. See wxRichTextHeaderFooterData
        for details.
    */
    void SetFooterText(const wxString& text,
                       wxRichTextOddEvenPage page = wxRICHTEXT_PAGE_ALL,
                       wxRichTextPageLocation location = wxRICHTEXT_PAGE_CENTRE);

    /**
        Sets the internal wxRichTextHeaderFooterData object.
    */
    void SetHeaderFooterData(const wxRichTextHeaderFooterData& data);

    /**
        Sets the wxRichTextHeaderFooterData font.
    */
    void SetHeaderFooterFont(const wxFont& font);

    /**
        Sets the wxRichTextHeaderFooterData text colour.
    */
    void SetHeaderFooterTextColour(const wxColour& colour);

    /**
        A convenience function to set the header text. See wxRichTextHeaderFooterData
        for details.
    */
    void SetHeaderText(const wxString& text,
                       wxRichTextOddEvenPage page = wxRICHTEXT_PAGE_ALL,
                       wxRichTextPageLocation location = wxRICHTEXT_PAGE_CENTRE);

    /**
        Sets the page setup data.
    */
    void SetPageSetupData(const wxPageSetupData& pageSetupData);

    /**
        Sets the parent window to be used for the preview window and printing wait
        dialog.
    */
    void SetParentWindow(wxWindow* parent);

    /**
        Sets the dimensions to be used for the preview window.
    */
    void SetPreviewRect(const wxRect& rect);

    /**
        Sets the print data.
    */
    void SetPrintData(const wxPrintData& printData);

    /**
        Pass @true to show the header and footer on the first page.
    */
    void SetShowOnFirstPage(bool show);

    /**
        Pass the title of the preview window or printing wait caption.
    */
    void SetTitle(const wxString& title);
};
