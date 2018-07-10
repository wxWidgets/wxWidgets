///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/extfield.h
// Purpose:     Declare wxExternalField helper
// Author:      Vadim Zeitlin
// Created:     2017-11-21
// Copyright:   (c) 2017 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_EXTFIELD_H_
#define _WX_PRIVATE_EXTFIELD_H_

// ----------------------------------------------------------------------------
// wxExternalField: store object data outside of it
// ----------------------------------------------------------------------------

// This class allows to store some data without consuming space for the objects
// that don't need it and can be useful for avoiding to add rarely used fields
// to the classes that are used by many objects, e.g. wxWindow.
//
// Note that using this class costs in speed and convenience of access to the
// field, which requires a hash lookup instead of accessing it directly. It
// also only currently works for heap-allocated fields as it's probably never
// worth using it for fields of simple types.
//
// Template parameter Object is the class that "contains" the field, Field is
// the type of the field itself and FieldMap is the hash map, defined
// separately using WX_DECLARE_HASH_MAP(), with Object* as the key and Field*
// as the value type.
template <typename Object, typename Field, typename FieldMap>
class wxExternalField
{
public:
    typedef Object ObjectType;
    typedef Field FieldType;
    typedef FieldMap MapType;

    // Store the field object to be used for the given object, replacing the
    // existing one, if any.
    //
    // This method takes ownership of the field pointer which will be destroyed
    // by EraseForObject().
    static void StoreForObject(ObjectType* obj, FieldType* field)
    {
        const typename MapType::iterator it = ms_map.find(obj);
        if ( it != ms_map.end() )
        {
            delete it->second;
            it->second = field;
        }
        else
        {
            ms_map.insert(typename MapType::value_type(obj, field));
        }
    }

    // Find the object for the corresponding window.
    static FieldType* FromObject(ObjectType* obj)
    {
        const typename MapType::const_iterator it = ms_map.find(obj);
        return it == ms_map.end() ? NULL : it->second;
    }

    // Erase the object used for the corresponding window, return true if there
    // was one or false otherwise.
    static bool EraseForObject(ObjectType* obj)
    {
        const typename MapType::iterator it = ms_map.find(obj);
        if ( it == ms_map.end() )
            return false;

        delete it->second;
        ms_map.erase(it);
        return true;
    }

private:
    static FieldMap ms_map;
};

template <typename O, typename F, typename M>
M wxExternalField<O, F, M>::ms_map;

#endif // _WX_PRIVATE_EXTFIELD_H_
