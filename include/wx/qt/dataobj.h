/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dataobj.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DATAOBJ_H_
#define _WX_QT_DATAOBJ_H_

// ----------------------------------------------------------------------------
// wxDataObject is the same as wxDataObjectBase under wxQT
// ----------------------------------------------------------------------------

class QMimeData;

class WXDLLIMPEXP_CORE wxDataObject : public wxDataObjectBase
{
public:
    wxDataObject();
    virtual ~wxDataObject();

    virtual bool IsSupportedFormat( const wxDataFormat& format, Direction dir = Get ) const;
	virtual void AddDataTo(QMimeData &mimeData) const;
	virtual bool SetDataFrom(const QMimeData &mimeData);

private:
	virtual void DoSetDataFrom(const QMimeData &mimeData, const wxDataFormat &format);
};

#endif // _WX_QT_DATAOBJ_H_
