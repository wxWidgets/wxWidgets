/////////////////////////////////////////////////////////////////////////////
// Name:        listimpl.cpp
// Purpose:     second-part of macro based implementation of template lists
// Author:      Vadim Zeitlin
// Modified by:
// Created:     16/11/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#define _DEFINE_LIST(T, name)                   \
    void wx##name##Node::DeleteData()    \
    {                                           \
        delete (T *)GetData();                  \
    }

// redefine the macro so that now it will generate the class implementation
// old value would provoke a compile-time error if this file is not included
#undef  WX_DEFINE_LIST
#define WX_DEFINE_LIST(name) _DEFINE_LIST(_WX_LIST_ITEM_TYPE_##name, name)

// don't pollute preprocessor's name space
//#undef  _DEFINE_LIST
