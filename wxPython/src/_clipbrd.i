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



// wxClipboard represents the system clipboard. Normally, you should use
// wxTheClipboard which is a global pointer to the (unique) clipboard.
//
// Clipboard can be used to copy data to/paste data from. It works together
// with wxDataObject.
class wxClipboard : public wxObject {
public:
    wxClipboard();
    ~wxClipboard();

    // open the clipboard before Add/SetData() and GetData()
    virtual bool Open();

    // close the clipboard after Add/SetData() and GetData()
    virtual void Close();

    // query whether the clipboard is opened
    virtual bool IsOpened() const;

    // add to the clipboard data
    //
    // NB: the clipboard owns the pointer and will delete it, so data must be
    //     allocated on the heap
    virtual bool AddData( wxDataObject *data );

    // set the clipboard data, this is the same as Clear() followed by
    // AddData()
    virtual bool SetData( wxDataObject *data );

    // ask if data in correct format is available
    virtual bool IsSupported( const wxDataFormat& format );

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject& data );
    
    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear();

    // flushes the clipboard: this means that the data which is currently on
    // clipboard will stay available even after the application exits (possibly
    // eating memory), otherwise the clipboard will be emptied on exit
    virtual bool Flush();

    // X11 has two clipboards which get selected by this call. Empty on MSW.
    virtual void UsePrimarySelection( bool primary = False );
};


%immutable;
wxClipboard* const wxTheClipboard;
%mutable;



//---------------------------------------------------------------------------


// helpful class for opening the clipboard and automatically closing it when
// the locker is destroyed
class wxClipboardLocker
{
public:
    wxClipboardLocker(wxClipboard *clipboard = NULL);
    ~wxClipboardLocker();

    //bool operator!() const;

    %extend {
        bool __nonzero__()   { return !!(*self); }
    }
};


//---------------------------------------------------------------------------
