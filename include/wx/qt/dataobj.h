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

    // Adds object's data to Qt mime data appropriately for type
    virtual void QtAddDataTo(QMimeData &mimeData) const;
    // Sets object's data from Qt mime data appropriately for type
    virtual bool QtSetDataFrom(const QMimeData &mimeData);

private:
    // Sets object's data from Qt mime data in specific format
    virtual void QtSetDataSingleFormat(const QMimeData &mimeData, const wxDataFormat &format);
};

#endif // _WX_QT_DATAOBJ_H_
