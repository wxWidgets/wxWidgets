/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/clipbrd.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CLIPBRD_H_
#define _WX_QT_CLIPBRD_H_

class WXDLLIMPEXP_CORE wxClipboard : public wxClipboardBase
{
public:
    wxClipboard();

    virtual bool Open();
    virtual void Close();
    virtual bool IsOpened() const;

    virtual bool AddData( wxDataObject *data );
    virtual bool SetData( wxDataObject *data );
    virtual bool GetData( wxDataObject& data );
    virtual void Clear();

    virtual bool IsSupported( const wxDataFormat& format );

protected:

private:
};

#endif // _WX_QT_CLIPBRD_H_
