///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/extensionhook.h
// Purpose:     Registry of handlers for wxObject::WXExtensionHook()
// Author:      Vadim Zeitlin
// Created:     2026-11-22
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_EXTENSIONHOOK_H_
#define _WX_PRIVATE_EXTENSIONHOOK_H_

#include "wx/object.h"

// ----------------------------------------------------------------------------
// Extension hooks for preserving ABI compatibility
// ----------------------------------------------------------------------------

// The code in this file allows to add the equivalent of new virtual functions
// to the existing classes without breaking ABI.
//
// Each "virtual function" is identified by wxExtensionHookId and the code that
// would call it calls wxCallExtensionHook() instead, which in turn calls
// wxObject::WXExtensionHook(). The default implementation of the latter looks
// up the handler registered for the given id and the class of the object,
// using its wxClassInfo and so working for the classes deriving from it
// without defining their own class info too. If no handler is registered for
// the class itself, the handlers for its base classes are tried.
//
// To add a new hook:
//
//  1. Add a new element to wxExtensionHookId enum.
//  2. Specialize wxExtensionHookTraits for it, defining the type of the data
//     passed to and/or returned from the handler.
//  3. Use wxREGISTER_EXTENSION_HOOK() to register the handler for the class
//     (and possibly others for its derived classes).
//  4. Use wxCallExtensionHook() where the function would be called.
//
// For example:
//
//      // Here, in this file:
//      enum wxExtensionHookId : int
//      {
//          ...
//          wxEXT_HOOK_GET_FOO,
//      };
//
//      template <>
//      struct wxExtensionHookTraits<wxEXT_HOOK_GET_FOO>
//      {
//          struct Data { int foo = 0; };
//      };
//
//      // In src/common/foo.cpp:
//      static bool GetFooForMyControl(wxMyControl* ctrl, wxExtensionHookTraits<wxEXT_HOOK_GET_FOO>::Data& data)
//      {
//          data.foo = ctrl->DoGetFoo();
//          return true;
//      }
//
//      wxREGISTER_EXTENSION_HOOK(wxEXT_HOOK_GET_FOO, wxMyControl, GetFooForMyControl);
//
//      // Wherever it's needed:
//      wxExtensionHookTraits<wxEXT_HOOK_GET_FOO>::Data data;
//      if ( !wxCallExtensionHook<wxEXT_HOOK_GET_FOO>(obj, data) )
//          ... use some default value for foo ...
//
// Notes:
//
//  - Registry is not MT-safe, but this doesn't matter as the handlers are only
//    registered at static initialization time before any threads are created.
//
//  - Handlers should be registered in the same file as the one containing
//    wxIMPLEMENT_CLASS() for the class, as otherwise the object file with the
//    registration could be discarded by the linker when using static wx build.
//
//  - Handler returning false is the equivalent of not having any handler at
//    all, i.e. the handlers for the base classes are tried then.

// Identifiers of all the hooks, the values don't matter as long as they're
// unique.
enum wxExtensionHookId : int
{
    // Add new ids here.
    wxEXT_HOOK_MAX
};

// This template must be specialized for each id and define Data type.
template <wxExtensionHookId id>
struct wxExtensionHookTraits;

// Type of the functions stored in the registry.
using wxExtensionHookHandler = bool (*)(wxObject* obj, void* data);

// Register the given handler for the given class: this object must remain
// alive while the handler can be used, so it should be a global or static.
//
// Use wxREGISTER_EXTENSION_HOOK() instead of using this class directly.
class WXDLLIMPEXP_BASE wxExtensionHookRegistrar
{
public:
    wxExtensionHookRegistrar(const wxClassInfo* classInfo,
                             wxExtensionHookId id,
                             wxExtensionHookHandler handler);
    ~wxExtensionHookRegistrar();

    wxExtensionHookRegistrar(const wxExtensionHookRegistrar&) = delete;
    wxExtensionHookRegistrar& operator=(const wxExtensionHookRegistrar&) = delete;

private:
    const wxClassInfo* const m_classInfo;
    const wxExtensionHookId m_id;
};

// Function converting the untyped wxExtensionHookHandler arguments to the
// types used by the given handler.
template <wxExtensionHookId id,
          typename T,
          bool (*F)(T*, typename wxExtensionHookTraits<id>::Data&)>
bool wxExtensionHookTrampoline(wxObject* obj, void* data)
{
    using Data = typename wxExtensionHookTraits<id>::Data;

    return F(static_cast<T*>(obj), *static_cast<Data*>(data));
}

// Register the handler for the extension hook with the given id for the given
// class, func must be a function taking cls pointer and a reference to the
// data type defined by wxExtensionHookTraits<id> and returning bool.
#define wxREGISTER_EXTENSION_HOOK(id, cls, func) \
    static const wxExtensionHookRegistrar \
        wxMAKE_UNIQUE_NAME(wxExtensionHookRegistrar)( \
            wxCLASSINFO(cls), \
            id, \
            &wxExtensionHookTrampoline<id, cls, func> \
        )

// Call the handler for the given hook for the given object, returns true if it
// was handled.
template <wxExtensionHookId id>
inline bool
wxCallExtensionHook(wxObject* obj, typename wxExtensionHookTraits<id>::Data& data)
{
    return obj->WXExtensionHook(id, &data);
}

#endif // _WX_PRIVATE_EXTENSIONHOOK_H_
