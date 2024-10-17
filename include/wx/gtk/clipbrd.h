/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/clipbrd.h
// Purpose:     wxClipboard for wxGTK
// Author:      Robert Roebling, Vadim Zeitlin
// Copyright:   (c) 1998 Robert Roebling
//              (c) 2007 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_CLIPBOARD_H_
#define _WX_GTK_CLIPBOARD_H_

// ----------------------------------------------------------------------------
// wxClipboard
// ----------------------------------------------------------------------------

#include "wx/weakref.h"

class WXDLLIMPEXP_CORE wxClipboard : public wxClipboardBase
{
public:
    // there are several clipboards in X11 (and in GDK)
    enum Kind
    {
        Primary,
        Clipboard
    };

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

    // ask if data in correct format is available
    virtual bool IsSupportedAsync( wxEvtHandler *sink ) override;

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject& data ) override;

    // flushes the clipboard; that means that the data which is currently on
    // clipboard will stay available even after the application exits (possibly
    // eating memory), otherwise the clipboard will be emptied on exit
    virtual bool Flush() override;

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear() override;



    // implementation from now on
    // --------------------------

    // get our clipboard item (depending on m_usePrimary value)
    GdkAtom GTKGetClipboardAtom() const;

    // get the data object currently being requested
    wxDataObject *GTKGetDataObject( GdkAtom atom );

    // clear the data for the given clipboard kind
    void GTKClearData(Kind kind);

    // called when selection data is received
    void GTKOnSelectionReceived(const GtkSelectionData& sel);

    // called when available target information is received
    bool GTKOnTargetReceived(const wxDataFormat& format);

private:
    // the data object for the specific selection
    wxDataObject *& Data(Kind kind)
    {
        return kind == Primary ? m_dataPrimary : m_dataClipboard;
    }

    // the data object we're currently using
    wxDataObject *& Data()
    {
        return Data(m_usePrimary ? Primary : Clipboard);
    }


    // set or unset selection ownership
    bool SetSelectionOwner(bool set = true);

    // get the atom corresponding to the given format if it's supported
    GdkAtom DoGetTarget(const wxDataFormat& format);

    // just check if the given format is supported
    bool DoIsSupported(const wxDataFormat& format);


    // both of these pointers can be non-null simultaneously but we only use
    // one of them at any moment depending on m_usePrimary value, use Data()
    // (from inside) or GTKGetDataObject() (from outside) accessors
    wxDataObject *m_dataPrimary,
                 *m_dataClipboard;

    // this is used to temporarily hold the object passed to our GetData() so
    // that GTK callbacks could access it
    wxDataObject *m_receivedData;

    // used to pass information about the format we need from DoIsSupported()
    // to GTKOnTargetReceived() and return the supported format from the latter
    GdkAtom m_targetRequested;

    GtkWidget *m_clipboardWidget;  // for getting and offering data
    GtkWidget *m_targetsWidget;    // for getting list of supported formats

    // ID of the connection to "selection_get" signal, initially 0.
    unsigned long m_idSelectionGetHandler;

    bool m_open;
    bool m_formatSupported;

public:
    // async stuff
    wxEvtHandlerRef    m_sink;
private:
    GtkWidget         *m_targetsWidgetAsync;  // for getting list of supported formats

    wxDECLARE_DYNAMIC_CLASS(wxClipboard);
};

#endif // _WX_GTK_CLIPBOARD_H_
