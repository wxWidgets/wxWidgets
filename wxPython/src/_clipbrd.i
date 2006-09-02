/////////////////////////////////////////////////////////////////////////////
// Name:        _clipbrd.i
// Purpose:     SWIG definitions for the Clipboard
//
// Author:      Robin Dunn
//
// Created:     31-October-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
%}

DocStr(wxClipboard,
"wx.Clipboard represents the system clipboard and provides methods to
copy data to it or paste data from it.  Normally, you should only use
``wx.TheClipboard`` which is a reference to a global wx.Clipboard
instance.

Call ``wx.TheClipboard``'s `Open` method to get ownership of the
clipboard. If this operation returns True, you now own the
clipboard. Call `SetData` to put data on the clipboard, or `GetData`
to retrieve data from the clipboard.  Call `Close` to close the
clipboard and relinquish ownership. You should keep the clipboard open
only momentarily.

:see: `wx.DataObject`
", "");



class wxClipboard : public wxObject {
public:
    DocCtorStr(
        wxClipboard(),
        "", "");
    
    ~wxClipboard();


    DocDeclStr(
        virtual bool , Open(),
        "Call this function to open the clipboard before calling SetData and
GetData.  Call Close when you have finished with the clipboard.  You
should keep the clipboard open for only a very short time.  Returns
True on success.", "");
    

    DocDeclStr(
        virtual void , Close(),
        "Closes the clipboard.", "");
    

    DocDeclStr(
        virtual bool , IsOpened() const,
        "Query whether the clipboard is opened", "");
    


    %disownarg( wxDataObject *data );
    
    DocDeclStr(
        virtual bool , AddData( wxDataObject *data ),
        "Call this function to add the data object to the clipboard. You may
call this function repeatedly after having cleared the clipboard.
After this function has been called, the clipboard owns the data, so
do not delete the data explicitly.

:see: `wx.DataObject`", "");
    

    DocDeclStr(
        virtual bool , SetData( wxDataObject *data ),
        "Set the clipboard data, this is the same as `Clear` followed by
`AddData`.

:see: `wx.DataObject`", "");
    
    %cleardisown( wxDataObject *data );
    
    
    DocDeclStr(
        virtual bool , IsSupported( const wxDataFormat& format ),
        "Returns True if the given format is available in the data object(s) on
the clipboard.", "");

    DocDeclStr(
        virtual bool , GetData( wxDataObject& data ),
        "Call this function to fill data with data on the clipboard, if
available in the required format. Returns true on success.", "");
    
    
    DocDeclStr(
        virtual void , Clear(),
        "Clears data from the clipboard object and also the system's clipboard
if possible.", "");
    

    DocDeclStr(
        virtual bool , Flush(),
        "Flushes the clipboard: this means that the data which is currently on
clipboard will stay available even after the application exits,
possibly eating memory, otherwise the clipboard will be emptied on
exit.  Returns False if the operation is unsuccesful for any reason.", "");
    

    DocDeclStr(
        virtual void , UsePrimarySelection( bool primary = true ),
        "On platforms supporting it (the X11 based platforms), selects the
so called PRIMARY SELECTION as the clipboard as opposed to the
normal clipboard, if primary is True.", "");


    DocDeclStr(
        static wxClipboard *, Get(),
        "Returns global instance (wxTheClipboard) of the object.", "");

    %property(Data, GetData, SetData, doc="See `GetData` and `SetData`");
    
};


// Previously we just declared wxTheClipboard as a global, but in C++
// is has been changed to be a macro for wxClipboard::Get, but the
// swig generated code will try to evaluate it when it assigns to the
// swig wrapper var so this causes Get to be called too early on
// wxGTK.  So instead we'll create a Python class that can delay the
// Get until it is really needed, which is similar in effect to what
// is really happening on the C++ side too.
%pythoncode {
    class _wxPyDelayedInitWrapper(object):
        def __init__(self, initfunc, *args, **kwargs):
            self._initfunc = initfunc
            self._args = args
            self._kwargs = kwargs
            self._instance = None
        def _checkInstance(self):
            if self._instance is None:
                if wx.GetApp():
                    self._instance = self._initfunc(*self._args, **self._kwargs)        
        def __getattr__(self, name):
            self._checkInstance()
            return getattr(self._instance, name)
        def __repr__(self):
            self._checkInstance()
            return repr(self._instance)
    TheClipboard = _wxPyDelayedInitWrapper(Clipboard.Get)
}


//---------------------------------------------------------------------------


DocStr(wxClipboardLocker,
"A helpful class for opening the clipboard and automatically
closing it when the locker is destroyed.", "");

class wxClipboardLocker
{
public:
    wxClipboardLocker(wxClipboard *clipboard = NULL);
    ~wxClipboardLocker();

    DocStr(__nonzero__,
           "A ClipboardLocker instance evaluates to True if the clipboard was
successfully opened.", "");
    %extend {
        bool __nonzero__()   { return !!(*self); }
    }
};


//---------------------------------------------------------------------------
