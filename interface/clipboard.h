/////////////////////////////////////////////////////////////////////////////
    // Name:        clipboard.h
    // Purpose:     documentation for global functions
    // Author:      wxWidgets team
    // RCS-ID:      $Id$
    // Licence:     wxWindows license
    /////////////////////////////////////////////////////////////////////////////
    
    /**
    Gets the name of a registered clipboard format, and puts it into the buffer @e
    formatName which is of maximum
    length @e maxCount. @e dataFormat must not specify a predefined clipboard
    format.
*/
bool wxGetClipboardFormatName(int dataFormat,
                              const wxString& formatName,
                              int maxCount);


    /**
    Gets data from the clipboard.
    
    @e dataFormat may be one of:
    
     wxCF_TEXT or wxCF_OEMTEXT: returns a pointer to new memory containing a
    null-terminated text string.
     wxCF_BITMAP: returns a new wxBitmap.
    
    The clipboard must have previously been opened for this call to succeed.
*/
wxObject * wxGetClipboardData(int dataFormat);

/**
    Returns @true if the given data format is available on the clipboard.
*/
bool wxIsClipboardFormatAvailable(int dataFormat);

/**
    Opens the clipboard for passing data to it or getting data from it.
*/
bool wxOpenClipboard();

/**
    Empties the clipboard.
*/
bool wxEmptyClipboard();

/**
    Returns @true if this application has already opened the clipboard.
*/
bool wxClipboardOpen();

/**
    Registers the clipboard data format name and returns an identifier.
*/
int wxRegisterClipboardFormat(const wxString& formatName);

/**
    Closes the clipboard to allow other applications to use it.
*/
bool wxCloseClipboard();

/**
    Enumerates the formats found in a list of available formats that belong
    to the clipboard. Each call to this  function specifies a known
    available format; the function returns the format that appears next in
    the list.
    
    @e dataFormat specifies a known format. If this parameter is zero,
    the function returns the first format in the list.
    
    The return value specifies the next known clipboard data format if the
    function is successful. It is zero if the @e dataFormat parameter specifies
    the last  format in the list of available formats, or if the clipboard
    is not open.
    
    Before it enumerates the formats function, an application must open the
    clipboard by using the
    wxOpenClipboard function.
*/
int wxEnumClipboardFormats(int dataFormat);

