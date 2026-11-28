///////////////////////////////////////////////////////////////////////////////
// Name:        wx/extradata.h
// Purpose:     Declaration of wxWithExtraData mix-in.
// Author:      Vadim Zeitlin
// Created:     2026-11-22
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_EXTRADATA_H_
#define _WX_EXTRADATA_H_

#include "wx/defs.h"

class wxExtraDataStorage;

// ----------------------------------------------------------------------------
// wxWithExtraData: add wxExtraDataStorage to any class
// ----------------------------------------------------------------------------

// This class is only used internally by wxWidgets to allow extending public
// classes without breaking ABI.
//
// Do not use it in your code, it is not part of the public API.
class WXDLLIMPEXP_BASE wxWithExtraData
{
public:
    wxWithExtraData() = default;

    // Copying the object only copies the extra data which supports it, see
    // wxCloneableExtraData.
    wxWithExtraData(const wxWithExtraData& other);
    wxWithExtraData& operator=(const wxWithExtraData& other);

    // Create the storage if it doesn't exist yet.
    wxExtraDataStorage& WXGetExtraDataStorage();

    // Just return null if the storage doesn't exist yet.
    wxExtraDataStorage* WXGetExtraDataStorageIfExists() const
    {
        return m_extraData;
    }

protected:
    // Dtor is protected to prevent deleting the object through this base class
    // pointer which would be undefined behaviour as dtor is not virtual.
    ~wxWithExtraData();

private:
    // Extra data used to preserve ABI compatibility, initially null and only
    // allocated by WXGetExtraDataStorage() when needed.
    wxExtraDataStorage* m_extraData = nullptr;
};

#endif // _WX_EXTRADATA_H_
