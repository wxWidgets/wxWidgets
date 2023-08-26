/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/clipbrd.h
// Purpose:     wxClipboad class and clipboard functions for MSW
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CLIPBRD_H_
#define _WX_CLIPBRD_H_

#if wxUSE_CLIPBOARD

// Deprecated wxMSW-only function superseded by wxClipboard, don't use them and
// use that class instead.
#ifdef WXWIN_COMPATIBILITY_3_2

wxDEPRECATED_MSG("Use wxClipboard") WXDLLIMPEXP_CORE bool wxOpenClipboard();
wxDEPRECATED_MSG("Use wxClipboard") WXDLLIMPEXP_CORE bool wxIsClipboardOpened();
wxDEPRECATED_MSG("Use wxClipboard") WXDLLIMPEXP_CORE bool wxClipboardOpen();
wxDEPRECATED_MSG("Use wxClipboard") WXDLLIMPEXP_CORE bool wxCloseClipboard();
wxDEPRECATED_MSG("Use wxClipboard") WXDLLIMPEXP_CORE bool wxEmptyClipboard();

#endif // WXWIN_COMPATIBILITY_3_2

// Non-deprecated but still wxMSW-specific functions working with clipboard
// formats -- don't use them neither.
WXDLLIMPEXP_CORE bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat);
WXDLLIMPEXP_CORE wxDataFormat wxEnumClipboardFormats(wxDataFormat dataFormat);
WXDLLIMPEXP_CORE int  wxRegisterClipboardFormat(wxChar *formatName);
WXDLLIMPEXP_CORE bool wxGetClipboardFormatName(wxDataFormat dataFormat,
                                          wxChar *formatName,
                                          int maxCount);

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClipboard : public wxClipboardBase
{
public:
    wxClipboard();
    virtual ~wxClipboard();

    // open the clipboard before SetData() and GetData()
    virtual bool Open() override;

    // close the clipboard after SetData() and GetData()
    virtual void Close() override;

    // query whether the clipboard is opened
    virtual bool IsOpened() const override;

    // set the clipboard data. all other formats will be deleted.
    virtual bool SetData( wxDataObject *data ) override;

    // add to the clipboard data.
    virtual bool AddData( wxDataObject *data ) override;

    // ask if data in correct format is available
    virtual bool IsSupported( const wxDataFormat& format ) override;

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject& data ) override;

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear() override;

    // flushes the clipboard: this means that the data which is currently on
    // clipboard will stay available even after the application exits (possibly
    // eating memory), otherwise the clipboard will be emptied on exit
    virtual bool Flush() override;

private:
    IDataObject *m_lastDataObject;
    bool m_isOpened;

    wxDECLARE_DYNAMIC_CLASS(wxClipboard);
};

#endif // wxUSE_CLIPBOARD

#endif // _WX_CLIPBRD_H_
