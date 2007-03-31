/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/clipboard.h
// Purpose:     wxClipboard for wxGTK
// Author:      Robert Roebling, Vadim Zeitlin
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
//              (c) 2007 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_CLIPBOARD_H_
#define _WX_GTK_CLIPBOARD_H_

// ----------------------------------------------------------------------------
// wxClipboard
// ----------------------------------------------------------------------------

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
    virtual bool Open();

    // close the clipboard after SetData() and GetData()
    virtual void Close();

    // query whether the clipboard is opened
    virtual bool IsOpened() const;

    // set the clipboard data. all other formats will be deleted.
    virtual bool SetData( wxDataObject *data );

    // add to the clipboard data.
    virtual bool AddData( wxDataObject *data );

    // ask if data in correct format is available
    virtual bool IsSupported( const wxDataFormat& format );

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject& data );

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear();

    // If primary == TRUE, use primary selection in all further ops,
    // primary == FALSE resets it.
    virtual void UsePrimarySelection(bool primary = TRUE)
        { m_usePrimary = primary; }
    

    // implementation from now on
    // --------------------------

    // get our clipboard item (depending on m_usePrimary value)
    GdkAtom GTKGetClipboardAtom() const;

    // get the data object currently being used
    wxDataObject *GTKGetDataObject() { return Data(); }

    // clear the data for the given clipboard kind
    void GTKClearData(Kind kind);

    // called when selection data is received
    void GTKOnSelectionReceived(const GtkSelectionData& sel);

    // called when available target information is received
    bool GTKOnTargetReceived(const wxDataFormat& format);

private:
    // the data object we're currently using
    wxDataObject *& Data()
    {
        return m_usePrimary ? m_dataPrimary : m_dataClipboard;
    }

    // set or unset selection ownership
    bool SetSelectionOwner(bool set = true);

    // add atom to the list of supported targets
    void AddSupportedTarget(GdkAtom atom);

    // check if the given format is supported
    bool DoIsSupported(const wxDataFormat& format);


    // both of these pointers can be non-NULL simultaneously but we only use
    // one of them at any moment depending on m_usePrimary value, use Data()
    // (from inside) or GTKGetDataObject() (from outside) accessors
    wxDataObject *m_dataPrimary,
                 *m_dataClipboard;

    // this is used to temporarily hold the object passed to our GetData() so
    // that GTK callbacks could access it
    wxDataObject *m_receivedData;

    // used to pass information about the format we need from DoIsSupported()
    // to GTKOnTargetReceived()
    GdkAtom m_targetRequested;

    GtkWidget *m_clipboardWidget;  // for getting and offering data
    GtkWidget *m_targetsWidget;    // for getting list of supported formats

    bool m_open;
    bool m_usePrimary;
    bool m_formatSupported;


    DECLARE_DYNAMIC_CLASS(wxClipboard)
};

#endif // _WX_GTK_CLIPBOARD_H_
