///////////////////////////////////////////////////////////////////////////////
// Name:        mac/dataobj.h
// Purpose:     declaration of the wxDataObject
// Author:      Stefan Csomor
// Modified by:
// Created:     10/21/99
// RCS-ID:      $Id$
// Copyright:   (c) 1998, 1999 Vadim Zeitlin, Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_DATAOBJ_H_
#define _WX_MAC_DATAOBJ_H_

#ifdef __GNUG__
    #pragma interface "dataobj.h"
#endif

// ----------------------------------------------------------------------------
// wxDataObject is the same as wxDataObjectBase under wxGTK
// ----------------------------------------------------------------------------

class wxDataObject : public wxDataObjectBase
{
public:
    wxDataObject();
    virtual ~wxDataObject();

    virtual bool IsSupportedFormat( const wxDataFormat& eFormat
                                   ,Direction           eDir = Get
                                  ) const
    {
        return(IsSupported( eFormat
                           ,eDir
                          ));
    }

    PDRAGITEM GetInterface(void) const {return m_pDataObject;}
private:
    PDRAGITEM                       m_pDataObject;
};

#endif // _WX_MAC_DATAOBJ_H_

