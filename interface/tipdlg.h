/////////////////////////////////////////////////////////////////////////////
// Name:        tipdlg.h
// Purpose:     documentation for wxTipProvider class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTipProvider
    @wxheader{tipdlg.h}

    This is the class used together with wxShowTip function.
    It must implement wxTipProvider::GetTip function and return the
    current tip from it (different tip each time it is called).

    You will never use this class yourself, but you need it to show startup tips
    with wxShowTip. Also, if you want to get the tips text from elsewhere than a
    simple text file, you will want to derive a new class from wxTipProvider and
    use it instead of the one returned by wxCreateFileTipProvider.

    @library{wxadv}
    @category{FIXME}

    @seealso
    @ref overview_tipsoverview "Startup tips overview", ::wxShowTip
*/
class wxTipProvider
{
public:
    /**
        Constructor.
        
        @param currentTip
            The starting tip index.
    */
    wxTipProvider(size_t currentTip);

    /**
        Returns the index of the current tip (i.e. the one which would be returned by
        GetTip).
        The program usually remembers the value returned by this function after calling
        wxShowTip. Note that it is not the same as the value which
        was passed to wxShowTip + 1 because the user might have pressed the "Next"
        button in the tip dialog.
    */
    size_t GetCurrentTip();

    /**
        Return the text of the current tip and pass to the next one. This function is
        pure virtual, it should be implemented in the derived classes.
    */
    wxString GetTip();

    /**
        Returns a modified tip. This function will be called immediately after read,
        and before being check whether it is a comment, an empty string or a string
        to translate. You can optionally override this in your custom user-derived
        class
        to optionally to modify the tip as soon as it is read. You can return any
        modification to the string. If you return wxEmptyString, then this tip is
        skipped, and the next one is read.
    */
    virtual wxString PreProcessTip(const wxString& tip);
};


// ============================================================================
// Global functions/macros
// ============================================================================

/**
    This function creates a wxTipProvider which may be
    used with wxShowTip.

    @param filename
        The name of the file containing the tips, one per line
    @param currentTip
        The index of the first tip to show - normally this index
        is remembered between the 2 program runs.

    @see @ref overview_tipsoverview "Tips overview"
*/
wxTipProvider* wxCreateFileTipProvider(const wxString& filename,
                                       size_t currentTip);

