///////////////////////////////////////////////////////////////////////////////
// Name:        msw/ole/dataobj.h
// Purpose:     declaration of the wxDataObject class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.05.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_MSW_OLE_DATAOBJ_H
#define   _WX_MSW_OLE_DATAOBJ_H

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

struct IDataObject;

// ----------------------------------------------------------------------------
// wxDataObject is a "smart" and polymorphic piece of data.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDataObject : public wxDataObjectBase
{
public:
    // ctor & dtor
    wxDataObject();
    virtual ~wxDataObject();

    // retrieve IDataObject interface (for other OLE related classes)
    IDataObject *GetInterface() const { return m_pIDataObject; }

    // tell the object that it should be now owned by IDataObject - i.e. when
    // it is deleted, it should delete us as well
    void SetAutoDelete();

    // return TRUE if we support this format in "Get" direction
    bool IsSupportedFormat(const wxDataFormat& format) const;

#ifdef __WXDEBUG__
    // function to return symbolic name of clipboard format (for debug messages)
    static const char *GetFormatName(wxDataFormat format);

    #define wxGetFormatName(format) wxDataObject::GetFormatName(format)
#else // !Debug
    #define wxGetFormatName(format) ""
#endif // Debug/!Debug

private:
    IDataObject *m_pIDataObject; // pointer to the COM interface
};

#endif  //_WX_MSW_OLE_DATAOBJ_H
