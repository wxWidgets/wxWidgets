///////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:     wxTextCtrlBase class - the interface of wxTextCtrl
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.07.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTCTRL_H_BASE_
#define _WX_TEXTCTRL_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "textctrlbase.h"
#endif

#include "wx/defs.h"

#if wxUSE_TEXTCTRL

#include "wx/control.h"         // the base class

// 16-bit Borland 4.0 doesn't seem to allow multiple inheritance with wxWindow
// and streambuf: it complains about deriving a huge class from the huge class
// streambuf. !! Also, can't use streambuf if making or using a DLL :-(

#if (defined(__BORLANDC__)) || defined(__MWERKS__) || defined(_WINDLL) || defined(WXUSINGDLL) || defined(WXMAKINGDLL)
    #define NO_TEXT_WINDOW_STREAM
#endif

#ifndef NO_TEXT_WINDOW_STREAM
    #if wxUSE_STD_IOSTREAM
        #include "wx/ioswrap.h"    // for iostream classes if we need them
    #else // !wxUSE_STD_IOSTREAM
        // can't compile this feature in if we don't use streams at all
        #define NO_TEXT_WINDOW_STREAM
    #endif // wxUSE_STD_IOSTREAM/!wxUSE_STD_IOSTREAM
#endif

class WXDLLEXPORT wxTextCtrl;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxTextCtrlNameStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

// ----------------------------------------------------------------------------
// wxTextCtrl: a single or multiple line text zone where user can enter and
// edit text
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTextCtrlBase : public wxControl
#ifndef NO_TEXT_WINDOW_STREAM
                                 , public streambuf
#endif

{
public:
    // creation
    // --------

    wxTextCtrlBase();
    ~wxTextCtrlBase();

    // accessors
    // ---------

    virtual wxString GetValue() const = 0;
    virtual void SetValue(const wxString& value) = 0;

    virtual int GetLineLength(long lineNo) const = 0;
    virtual wxString GetLineText(long lineNo) const = 0;
    virtual int GetNumberOfLines() const = 0;

    virtual bool IsModified() const = 0;
    virtual bool IsEditable() const = 0;

    // If the return values from and to are the same, there is no selection.
    virtual void GetSelection(long* from, long* to) const = 0;

    // operations
    // ----------

    // editing
    virtual void Clear() = 0;
    virtual void Replace(long from, long to, const wxString& value) = 0;
    virtual void Remove(long from, long to) = 0;

    // load/save the controls contents from/to the file
    virtual bool LoadFile(const wxString& file);
    virtual bool SaveFile(const wxString& file = wxEmptyString);

    // clears the dirty flag
    virtual void DiscardEdits() = 0;

    // writing text inserts it at the current position, appending always
    // inserts it at the end
    virtual void WriteText(const wxString& text) = 0;
    virtual void AppendText(const wxString& text) = 0;

    // translate between the position (which is just an index in the text ctrl
    // considering all its contents as a single strings) and (x, y) coordinates
    // which represent column and line.
    virtual long XYToPosition(long x, long y) const = 0;
    virtual bool PositionToXY(long pos, long *x, long *y) const = 0;

    virtual void ShowPosition(long pos) = 0;

    // Clipboard operations
    virtual void Copy() = 0;
    virtual void Cut() = 0;
    virtual void Paste() = 0;

    virtual bool CanCopy() const;
    virtual bool CanCut() const;
    virtual bool CanPaste() const;

    // Undo/redo
    virtual void Undo() = 0;
    virtual void Redo() = 0;

    virtual bool CanUndo() const = 0;
    virtual bool CanRedo() const = 0;

    // Insertion point
    virtual void SetInsertionPoint(long pos) = 0;
    virtual void SetInsertionPointEnd() = 0;
    virtual long GetInsertionPoint() const = 0;
    virtual long GetLastPosition() const = 0;

    virtual void SetSelection(long from, long to) = 0;
    virtual void SetEditable(bool editable) = 0;

    // streambuf methods
#ifndef NO_TEXT_WINDOW_STREAM
    int overflow(int i);
    int sync();
    int underflow();
#endif // NO_TEXT_WINDOW_STREAM

    // stream-like insertion operators: these are always available, whether we
    // were, or not, compiled with streambuf support
    wxTextCtrl& operator<<(const wxString& s);
    wxTextCtrl& operator<<(int i);
    wxTextCtrl& operator<<(long i);
    wxTextCtrl& operator<<(float f);
    wxTextCtrl& operator<<(double d);
    wxTextCtrl& operator<<(const wxChar c);

    // obsolete functions
#if WXWIN_COMPATIBILITY
    bool Modified() const { return IsModified(); }
#endif

protected:
    // the name of the last file loaded with LoadFile() which will be used by
    // SaveFile() by default
    wxString m_filename;

private:
#ifndef NO_TEXT_WINDOW_STREAM
#if !wxUSE_IOSTREAMH
    char *m_streambuf;
#endif
#endif
};

// ----------------------------------------------------------------------------
// include the platform-dependent class definition
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/textctrl.h"
#elif defined(__WXMSW__)
    #include "wx/msw/textctrl.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/textctrl.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/textctrl.h"
#elif defined(__WXQT__)
    #include "wx/qt/textctrl.h"
#elif defined(__WXMAC__)
    #include "wx/mac/textctrl.h"
#elif defined(__WXPM__)
    #include "wx/os2/textctrl.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/textctrl.h"
#endif

#endif // wxUSE_TEXTCTRL

#endif
    // _WX_TEXTCTRL_H_BASE_
