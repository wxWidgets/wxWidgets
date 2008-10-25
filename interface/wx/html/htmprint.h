/////////////////////////////////////////////////////////////////////////////
// Name:        html/htmprint.h
// Purpose:     interface of wxHtmlDCRenderer
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHtmlDCRenderer

    This class can render HTML document into a specified area of a DC.
    You can use it in your own printing code, although use of wxHtmlEasyPrinting
    or wxHtmlPrintout is strongly recommended.

    @library{wxhtml}
    @category{html}
*/
class wxHtmlDCRenderer : public wxObject
{
public:
    /**
        Constructor.
    */
    wxHtmlDCRenderer();

    /**
        Returns the height of the HTML text. This is important if area height
        (see wxHtmlDCRenderer::SetSize) is smaller that total height and thus
        the page cannot fit into it. In that case you're supposed to call
        Render() as long as its return value is smaller than GetTotalHeight()'s.
    */
    int GetTotalHeight();

    /**
        Renders HTML text to the DC.

        @param x,y
            position of upper-left corner of printing rectangle (see SetSize())
        @param from
            y-coordinate of the very first visible cell
        @param dont_render
            if @true then this method only returns y coordinate of the next page
            and does not output anything

        Returned value is y coordinate of first cell than didn't fit onto page.
        Use this value as from in next call to Render() in order to print multipages document.

        @warning
        The Following three methods @b must always be called before any call to
        Render() (preferably in this order):
        - SetDC()
        - SetSize()
        - SetHtmlText()
        <b>Render() changes the DC's user scale and does NOT restore it.</b>
    */
    int Render(int x, int y, int from = 0, int dont_render = false);

    /**
        Assign DC instance to the renderer.

        @a pixel_scale can be used when rendering to high-resolution DCs (e.g. printer)
        to adjust size of pixel metrics.
        (Many dimensions in HTML are given in pixels -- e.g. image sizes. 300x300
        image would be only one inch wide on typical printer. With pixel_scale = 3.0
        it would be 3 inches.)
    */
    void SetDC(wxDC* dc, double pixel_scale = 1.0);

    /**
        Sets fonts. See wxHtmlWindow::SetFonts for detailed description.

        @see SetSize()
    */
    void SetFonts(const wxString& normal_face,
                  const wxString& fixed_face,
                  const int sizes = NULL);

    /**
        Assign text to the renderer. Render() then draws the text onto DC.

        @param html
            HTML text. This is not a filename.
        @param basepath
            base directory (html string would be stored there if it was in file).
            It is used to determine path for loading images, for example.
        @param isdir
            @false if basepath is filename, @true if it is directory name
            (see wxFileSystem for detailed explanation).
    */
    void SetHtmlText(const wxString& html,
                     const wxString& basepath = wxEmptyString,
                     bool isdir = true);

    /**
        Set size of output rectangle, in pixels. Note that you @b can't change
        width of the rectangle between calls to wxHtmlDCRenderer::Render!
        (You can freely change height.)
    */
    void SetSize(int width, int height);
};



/**
    @class wxHtmlEasyPrinting

    This class provides very simple interface to printing architecture.
    It allows you to print HTML documents using only a few commands.

    @note
    Do not create this class on the stack only. You should create an instance
    on app startup and use this instance for all printing operations.
    The reason is that this class stores various settings in it.

    @library{wxhtml}
    @category{html,printing}
*/
class wxHtmlEasyPrinting : public wxObject
{
public:
    /**
        Constructor.

        @param name
            Name of the printing object. Used by preview frames and setup dialogs.
        @param parentWindow
            pointer to the window that will own the preview frame and setup dialogs.
            May be @NULL.
    */
    wxHtmlEasyPrinting(const wxString& name = "Printing",
                       wxWindow* parentWindow = NULL);

    /**
        Returns a pointer to wxPageSetupDialogData instance used by this class.
        You can set its parameters (via SetXXXX methods).
    */
    wxPageSetupDialogData* GetPageSetupData();

    /**
        Gets the parent window for dialogs.
    */
    wxWindow* GetParentWindow() const;

    /**
        Returns pointer to wxPrintData instance used by this class.
        You can set its parameters (via SetXXXX methods).
    */
    wxPrintData* GetPrintData();

    /**
        Display page setup dialog and allows the user to modify settings.
    */
    void PageSetup();

    /**
        Preview HTML file.

        Returns @false in case of error -- call wxPrinter::GetLastError to get detailed
        information about the kind of the error.
    */
    bool PreviewFile(const wxString& htmlfile);

    /**
        Preview HTML text (not file!).

        Returns @false in case of error -- call wxPrinter::GetLastError to get detailed
        information about the kind of the error.

        @param htmltext
            HTML text.
        @param basepath
            base directory (html string would be stored there if it was in file).
            It is used to determine path for loading images, for example.
    */
    bool PreviewText(const wxString& htmltext,
                     const wxString& basepath = wxEmptyString);

    /**
        Print HTML file.

        Returns @false in case of error -- call wxPrinter::GetLastError to get detailed
        information about the kind of the error.
    */
    bool PrintFile(const wxString& htmlfile);

    /**
        Print HTML text (not file!).

        Returns @false in case of error -- call wxPrinter::GetLastError to get detailed
        information about the kind of the error.

        @param htmltext
            HTML text.
        @param basepath
            base directory (html string would be stored there if it was in file).
            It is used to determine path for loading images, for example.
    */
    bool PrintText(const wxString& htmltext,
                   const wxString& basepath = wxEmptyString);

    /**
        Sets fonts. See wxHtmlWindow::SetFonts for detailed description.
    */
    void SetFonts(const wxString& normal_face,
                  const wxString& fixed_face,
                  const int sizes = NULL);

    /**
        Set page footer. The following macros can be used inside it:
         @@DATE@ is replaced by the current date in default format
         @@PAGENUM@ is replaced by page number
         @@PAGESCNT@ is replaced by total number of pages
         @@TIME@ is replaced by the current time in default format
         @@TITLE@ is replaced with the title of the document

        @param footer
            HTML text to be used as footer.
        @param pg
            one of wxPAGE_ODD, wxPAGE_EVEN and wxPAGE_ALL constants.
    */
    void SetFooter(const wxString& footer, int pg = wxPAGE_ALL);

    /**
        Set page header. The following macros can be used inside it:
        - @@DATE@ is replaced by the current date in default format
        - @@PAGENUM@ is replaced by page number
        - @@PAGESCNT@ is replaced by total number of pages
        - @@TIME@ is replaced by the current time in default format
        - @@TITLE@ is replaced with the title of the document

        @param header
            HTML text to be used as header.
        @param pg
            one of wxPAGE_ODD, wxPAGE_EVEN and wxPAGE_ALL constants.
    */
    void SetHeader(const wxString& header, int pg = wxPAGE_ALL);

    /**
        Sets the parent window for dialogs.
    */
    void SetParentWindow(wxWindow* window);
};



/**
    @class wxHtmlPrintout

    This class serves as printout class for HTML documents.

    @library{wxhtml}
    @category{html,printing}
*/
class wxHtmlPrintout : public wxPrintout
{
public:
    /**
        Constructor.
    */
    wxHtmlPrintout(const wxString& title = "Printout");

    /**
        Adds a filter to the static list of filters for wxHtmlPrintout.
        See wxHtmlFilter for further information.
    */
    static void AddFilter(wxHtmlFilter* filter);

    /**
        Sets fonts. See wxHtmlWindow::SetFonts for detailed description.
    */
    void SetFonts(const wxString& normal_face,
                  const wxString& fixed_face,
                  const int sizes = NULL);

    /**
        Set page footer. The following macros can be used inside it:
        - @@DATE@ is replaced by the current date in default format
        - @@PAGENUM@ is replaced by page number
        - @@PAGESCNT@ is replaced by total number of pages
        - @@TIME@ is replaced by the current time in default format
        - @@TITLE@ is replaced with the title of the document

        @param footer
            HTML text to be used as footer.
        @param pg
            one of wxPAGE_ODD, wxPAGE_EVEN and wxPAGE_ALL constants.
    */
    void SetFooter(const wxString& footer, int pg = wxPAGE_ALL);

    /**
        Set page header. The following macros can be used inside it:
        - @@DATE@ is replaced by the current date in default format
        - @@PAGENUM@ is replaced by page number
        - @@PAGESCNT@ is replaced by total number of pages
        - @@TIME@ is replaced by the current time in default format
        - @@TITLE@ is replaced with the title of the document

        @param header
            HTML text to be used as header.
        @param pg
            one of wxPAGE_ODD, wxPAGE_EVEN and wxPAGE_ALL constants.
    */
    void SetHeader(const wxString& header, int pg = wxPAGE_ALL);

    /**
        Prepare the class for printing this HTML @b file.
        The file may be located on any virtual file system or it may be normal file.
    */
    void SetHtmlFile(const wxString& htmlfile);

    /**
        Prepare the class for printing this HTML text.

        @param html
            HTML text. (NOT file!)
        @param basepath
            base directory (html string would be stored there if it was in file).
            It is used to determine path for loading images, for example.
        @param isdir
            @false if basepath is filename, @true if it is directory name
            (see wxFileSystem for detailed explanation).
    */
    void SetHtmlText(const wxString& html,
                     const wxString& basepath = wxEmptyString,
                     bool isdir = true);

    /**
        Sets margins in millimeters.
        Defaults to 1 inch for margins and 0.5cm for space between text and header
        and/or footer.
    */
    void SetMargins(float top = 25.2, float bottom = 25.2,
                    float left = 25.2,
                    float right = 25.2,
                    float spaces = 5);
};

