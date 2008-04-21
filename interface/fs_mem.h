/////////////////////////////////////////////////////////////////////////////
// Name:        fs_mem.h
// Purpose:     interface of wxMemoryFSHandler
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMemoryFSHandler
    @wxheader{fs_mem.h}

    This wxFileSystem handler can store arbitrary
    data in memory stream and make them accessible via URL. It is particularly
    suitable for storing bitmaps from resources or included XPM files so that
    they can be used with wxHTML.

    Filenames are prefixed with "memory:", e.g. "memory:myfile.html".

    Example:

    @code
    #ifndef __WXMSW__
    #include "logo.xpm"
    #endif

    void MyFrame::OnAbout(wxCommandEvent&)
    {
        wxBusyCursor bcur;
        wxFileSystem::AddHandler(new wxMemoryFSHandler);
        wxMemoryFSHandler::AddFile("logo.pcx", wxBITMAP(logo), wxBITMAP_TYPE_PCX);
        wxMemoryFSHandler::AddFile("about.htm",
                                   "htmlbodyAbout: "
                                   "img src=\"memory:logo.pcx\"/body/html");

        wxDialog dlg(this, -1, wxString(_("About")));
        wxBoxSizer *topsizer;
        wxHtmlWindow *html;
        topsizer = new wxBoxSizer(wxVERTICAL);
        html = new wxHtmlWindow(, -1, wxDefaultPosition,
                                wxSize(380, 160), wxHW_SCROLLBAR_NEVER);
        html-SetBorders(0);
        html-LoadPage("memory:about.htm");
        html-SetSize(html-GetInternalRepresentation()-GetWidth(),
                      html-GetInternalRepresentation()-GetHeight());
        topsizer-Add(html, 1, wxALL, 10);
        topsizer-Add(new wxStaticLine(, -1), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
        topsizer-Add(new wxButton(, wxID_OK, "Ok"),
                      0, wxALL | wxALIGN_RIGHT, 15);
        dlg.SetAutoLayout(@true);
        dlg.SetSizer(topsizer);
        topsizer-Fit();
        dlg.Centre();
        dlg.ShowModal();

        wxMemoryFSHandler::RemoveFile("logo.pcx");
        wxMemoryFSHandler::RemoveFile("about.htm");
    }
    @endcode

    @library{wxbase}
    @category{FIXME}

    @see wxMemoryFSHandler::AddFileWithMimeType
*/
class wxMemoryFSHandler : public wxFileSystemHandler
{
public:
    /**
        Constructor.
    */
    wxMemoryFSHandler();

    //@{
    /**
        Add file to list of files stored in memory. Stored
        data (bitmap, text or raw data)
        will be copied into private memory stream and available under
        name "memory:" + @e filename.
        The @a type argument is one of @c wxBITMAP_TYPE_XXX constants.
        Note that you must use a @a type value (aka image format)
        that wxWidgets can save (e.g. JPG, PNG, see wxImage
        documentation())!

        @see AddFileWithMimeType()
    */
    static void AddFile(const wxString& filename, wxImage& image,
                        long type);
    static void AddFile(const wxString& filename,
                        const wxBitmap& bitmap,
                        long type);
    //@}

    //@{
    /**
        Like AddFile(), but lets you explicitly
        specify added file's MIME type. This version should be used whenever you know
        the MIME type, because it makes accessing the files faster.

        @since 2.8.5

        @see AddFile()
    */
    static void AddFileWithMimeType(const wxString& filename,
                                    const wxString& textdata,
                                    const wxString& mimetype);
    static void AddFileWithMimeType(const wxString& filename,
                                    const void* binarydata,
                                    size_t size,
                                    const wxString& mimetype);
    //@}

    /**
        Remove file from memory FS and free occupied memory.
    */
    static void RemoveFile(const wxString& filename);
};

