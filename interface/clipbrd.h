/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.h
// Purpose:     documentation for wxClipboard class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxClipboard
    @wxheader{clipbrd.h}

    A class for manipulating the clipboard. Note that this is not compatible with
    the
    clipboard class from wxWidgets 1.xx, which has the same name but a different
    implementation.

    To use the clipboard, you call member functions of the global @b wxTheClipboard
    object.

    See also the @ref overview_wxdataobjectoverview for further information.

    Call wxClipboard::Open to get ownership of the clipboard. If this operation
    returns @true, you
    now own the clipboard. Call wxClipboard::SetData to put data
    on the clipboard, or wxClipboard::GetData to
    retrieve data from the clipboard. Call wxClipboard::Close to close
    the clipboard and relinquish ownership. You should keep the clipboard open only
    momentarily.

    For example:

    @code
    // Write some text to the clipboard
      if (wxTheClipboard-Open())
      {
        // This data objects are held by the clipboard,
        // so do not delete them in the app.
        wxTheClipboard-SetData( new wxTextDataObject("Some text") );
        wxTheClipboard-Close();
      }

      // Read some text
      if (wxTheClipboard-Open())
      {
        if (wxTheClipboard-IsSupported( wxDF_TEXT ))
        {
          wxTextDataObject data;
          wxTheClipboard-GetData( data );
          wxMessageBox( data.GetText() );
        }
        wxTheClipboard-Close();
      }
    @endcode

    @library{wxcore}
    @category{dnd}

    @seealso
    @ref overview_wxdndoverview, wxDataObject
*/
class wxClipboard : public wxObject
{
public:
    /**
        Constructor.
    */
    wxClipboard();

    /**
        Destructor.
    */
    ~wxClipboard();

    /**
        Call this function to add the data object to the clipboard. You may call
        this function repeatedly after having cleared the clipboard using Clear().
        After this function has been called, the clipboard owns the data, so do not
        delete
        the data explicitly.
        
        @see SetData()
    */
    bool AddData(wxDataObject* data);

    /**
        Clears the global clipboard object and the system's clipboard if possible.
    */
    void Clear();

    /**
        Call this function to close the clipboard, having opened it with Open().
    */
    void Close();

    /**
        Flushes the clipboard: this means that the data which is currently on
        clipboard will stay available even after the application exits (possibly
        eating memory), otherwise the clipboard will be emptied on exit.
        Returns @false if the operation is unsuccessful for any reason.
    */
    bool Flush();

    /**
        Call this function to fill @a data with data on the clipboard, if available in
        the required
        format. Returns @true on success.
    */
    bool GetData(wxDataObject& data);

    /**
        Returns @true if the clipboard has been opened.
    */
    bool IsOpened();

    /**
        Returns @true if there is data which matches the data format of the given data
        object currently @b available (IsSupported sounds like a misnomer, FIXME: better deprecate this name?) on the clipboard.
    */
    bool IsSupported(const wxDataFormat& format);

    /**
        Returns @true if we are using the primary selection, @false if clipboard
        one.
        See @ref useprimary() UsePrimarySelection for more information.
    */
    bool IsUsingPrimarySelection();

    /**
        Call this function to open the clipboard before calling SetData()
        and GetData().
        Call Close() when you have finished with the clipboard. You
        should keep the clipboard open for only a very short time.
        Returns @true on success. This should be tested (as in the sample shown above).
    */
    bool Open();

    /**
        Call this function to set the data object to the clipboard. This function will
        clear all previous contents in the clipboard, so calling it several times
        does not make any sense.
        After this function has been called, the clipboard owns the data, so do not
        delete
        the data explicitly.
        
        @see AddData()
    */
    bool SetData(wxDataObject* data);

    /**
        On platforms supporting it (all X11-based ports), wxClipboard uses the
        CLIPBOARD X11 selection by default. When this function is called with @true
        argument, all subsequent clipboard operations will use PRIMARY selection until
        this function is called again with @false.
        On the other platforms, there is no PRIMARY selection and so all clipboard
        operations will fail. This allows to implement the standard X11 handling of the
        clipboard which consists in copying data to the CLIPBOARD selection only when
        the user explicitly requests it (i.e. by selection @c "Copy" menu
        command) but putting the currently selected text into the PRIMARY selection
        automatically, without overwriting the normal clipboard contents with the
        currently selected text on the other platforms.
    */
    void UsePrimarySelection(bool primary = true);
};
