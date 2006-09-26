/////////////////////////////////////////////////////////////////////////////
// Name:        wx/variant.h
// Purpose:     wxVariant class, container for any type
// Author:      Julian Smart
// Modified by:
// Created:     10/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VARIANT_H_
#define _WX_VARIANT_H_

#include "wx/defs.h"

#if wxUSE_VARIANT

#include "wx/object.h"
#include "wx/string.h"
#include "wx/arrstr.h"
#include "wx/list.h"

#if wxUSE_DATETIME
    #include "wx/datetime.h"
#endif // wxUSE_DATETIME

#if wxUSE_ODBC
    #include "wx/db.h"  // will #include sqltypes.h
#endif //ODBC

#include "wx/iosfwrap.h"

/*
 * wxVariantData stores the actual data in a wxVariant object,
 * to allow it to store any type of data.
 * Derive from this to provide custom data handling.
 *
 * TODO: in order to replace wxPropertyValue, we would need
 * to consider adding constructors that take pointers to C++ variables,
 * or removing that functionality from the wxProperty library.
 * Essentially wxPropertyValue takes on some of the wxValidator functionality
 * by storing pointers and not just actual values, allowing update of C++ data
 * to be handled automatically. Perhaps there's another way of doing this without
 * overloading wxVariant with unnecessary functionality.
 */

class WXDLLIMPEXP_BASE wxVariantData: public wxObject
{
DECLARE_ABSTRACT_CLASS(wxVariantData)
public:

// Construction & destruction
    wxVariantData() {}

// Override these to provide common functionality
    // Copy to data
    virtual void Copy(wxVariantData& data) = 0;
    virtual bool Eq(wxVariantData& data) const = 0;
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const = 0;
#endif
    virtual bool Write(wxString& str) const = 0;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str) = 0;
#endif
    virtual bool Read(wxString& str) = 0;
    // What type is it? Return a string name.
    virtual wxString GetType() const = 0;
    // If it based on wxObject return the ClassInfo.
    virtual wxClassInfo* GetValueClassInfo() { return NULL; }
};

/*
 * wxVariant can store any kind of data, but has some basic types
 * built in.
 * NOTE: this eventually should have a reference-counting implementation.
 * PLEASE, if you change it to ref-counting, make sure it doesn't involve bloating
 * this class too much.
 */

class WXDLLIMPEXP_BASE wxVariant: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxVariant)
public:

// Construction & destruction
    wxVariant();
    wxVariant(double val, const wxString& name = wxEmptyString);
    wxVariant(long val, const wxString& name = wxEmptyString);
#ifdef HAVE_BOOL
    wxVariant(bool val, const wxString& name = wxEmptyString);
#endif
    wxVariant(char val, const wxString& name = wxEmptyString);
    wxVariant(const wxString& val, const wxString& name = wxEmptyString);
    wxVariant(const wxChar* val, const wxString& name = wxEmptyString); // Necessary or VC++ assumes bool!
#if WXWIN_COMPATIBILITY_2_4
    wxDEPRECATED( wxVariant(const wxStringList& val, const wxString& name = wxEmptyString) );
#endif
    wxVariant(const wxList& val, const wxString& name = wxEmptyString); // List of variants
    wxVariant(void* ptr, const wxString& name = wxEmptyString); // void* (general purpose)
    wxVariant(wxObject* ptr, const wxString& name = wxEmptyString); //wxObject
    wxVariant(wxVariantData* data, const wxString& name = wxEmptyString); // User-defined data
#if wxUSE_DATETIME
    wxVariant(const wxDateTime& val, const wxString& name = wxEmptyString); // Date
#endif // wxUSE_DATETIME
    wxVariant(const wxArrayString& val, const wxString& name = wxEmptyString); // String array
#if wxUSE_ODBC
    wxVariant(const DATE_STRUCT* valptr, const wxString& name = wxEmptyString); // DateTime
    wxVariant(const TIME_STRUCT* valptr, const wxString& name = wxEmptyString); // DateTime
    wxVariant(const TIMESTAMP_STRUCT* valptr, const wxString& name = wxEmptyString); // DateTime
#endif

    wxVariant(const wxVariant& variant);
    virtual ~wxVariant();

// Generic operators
    // Assignment
    void operator= (const wxVariant& variant);

#if wxUSE_DATETIME
    bool operator== (const wxDateTime& value) const;
    bool operator!= (const wxDateTime& value) const;
    void operator= (const wxDateTime& value) ;
#endif // wxUSE_DATETIME

    bool operator== (const wxArrayString& value) const;
    bool operator!= (const wxArrayString& value) const;
    void operator= (const wxArrayString& value) ;
#if wxUSE_ODBC
    void operator= (const DATE_STRUCT* value) ;
    void operator= (const TIME_STRUCT* value) ;
    void operator= (const TIMESTAMP_STRUCT* value) ;
#endif

    // Assignment using data, e.g.
    // myVariant = new wxStringVariantData("hello");
    void operator= (wxVariantData* variantData);
    bool operator== (const wxVariant& variant) const;
    bool operator!= (const wxVariant& variant) const;

// Specific operators
    bool operator== (double value) const;
    bool operator!= (double value) const;
    void operator= (double value) ;
    bool operator== (long value) const;
    bool operator!= (long value) const;
    void operator= (long value) ;
    bool operator== (char value) const;
    bool operator!= (char value) const;
    void operator= (char value) ;
#ifdef HAVE_BOOL
    bool operator== (bool value) const;
    bool operator!= (bool value) const;
    void operator= (bool value) ;
#endif
    bool operator== (const wxString& value) const;
    bool operator!= (const wxString& value) const;
    void operator= (const wxString& value) ;
    void operator= (const wxChar* value) ; // Necessary or VC++ assumes bool!
#if WXWIN_COMPATIBILITY_2_4
    wxDEPRECATED( bool operator== (const wxStringList& value) const );
    wxDEPRECATED( bool operator!= (const wxStringList& value) const );
    wxDEPRECATED( void operator= (const wxStringList& value) );
#endif
    bool operator== (const wxList& value) const;
    bool operator!= (const wxList& value) const;
    void operator= (const wxList& value) ;
    bool operator== (void* value) const;
    bool operator!= (void* value) const;
    void operator= (void* value) ;

    // Treat a list variant as an array
    wxVariant operator[] (size_t idx) const;
    wxVariant& operator[] (size_t idx) ;

    // Implicit conversion to a wxString
    inline operator wxString () const {  return MakeString(); }
    wxString MakeString() const;

    // Other implicit conversions
    inline operator double () const {  return GetDouble(); }
    inline operator char () const {  return GetChar(); }
    inline operator long () const {  return GetLong(); }
    inline operator bool () const {  return GetBool(); }
    inline operator void* () const {  return GetVoidPtr(); }
    // No implicit conversion to wxObject, as that would really
    //  confuse people between conversion to our contained data
    //  and downcasting to see our base type.
#if wxUSE_DATETIME
    inline operator wxDateTime () const { return GetDateTime(); }
#endif // wxUSE_DATETIME

// Accessors
    // Sets/gets name
    inline void SetName(const wxString& name) { m_name = name; }
    inline const wxString& GetName() const { return m_name; }

    // Tests whether there is data
    inline bool IsNull() const { return (m_data == (wxVariantData*) NULL); }

    wxVariantData* GetData() const { return m_data; }
    void SetData(wxVariantData* data) ;

    // Returns a string representing the type of the variant,
    // e.g. "string", "bool", "stringlist", "list", "double", "long"
    wxString GetType() const;

    bool IsType(const wxString& type) const;
    bool IsValueKindOf(const wxClassInfo* type) const;

    // Return the number of elements in a list
    size_t GetCount() const;

// Value accessors
    double GetReal() const ;
    inline double GetDouble() const { return GetReal(); };
    long GetInteger() const ;
    inline long GetLong() const { return GetInteger(); };
    char GetChar() const ;
    bool GetBool() const ;
    wxString GetString() const ;
    wxList& GetList() const ;
#if WXWIN_COMPATIBILITY_2_4
    wxDEPRECATED( wxStringList& GetStringList() const );
#endif
    void* GetVoidPtr() const;
    wxObject* GetWxObjectPtr() const;
#if wxUSE_DATETIME
    wxDateTime GetDateTime() const ;
#endif // wxUSE_DATETIME
    wxArrayString GetArrayString() const;

// Operations
    // Make NULL (i.e. delete the data)
    void MakeNull();

    // Make empty list
    void NullList();

    // Append to list
    void Append(const wxVariant& value);

    // Insert at front of list
    void Insert(const wxVariant& value);

    // Returns true if the variant is a member of the list
    bool Member(const wxVariant& value) const;

    // Deletes the nth element of the list
    bool Delete(size_t item);

    // Clear list
    void ClearList();

// Implementation
public:
// Type conversion
    bool Convert(long* value) const;
    bool Convert(bool* value) const;
    bool Convert(double* value) const;
    bool Convert(wxString* value) const;
    bool Convert(char* value) const;
#if wxUSE_DATETIME
    bool Convert(wxDateTime* value) const;
#endif // wxUSE_DATETIME

// Attributes
protected:
    wxVariantData*  m_data;
    wxString        m_name;
};

//Since we want type safety wxVariant we need to fetch and dynamic_cast
//in a seemingly safe way so the compiler can check, so we define
//a dynamic_cast /wxDynamicCast analogue.

#define wxGetVariantCast(var,classname) \
    ((classname*)(var.IsValueKindOf(&classname::ms_classInfo) ?\
                  var.GetWxObjectPtr() : NULL));

extern wxVariant WXDLLIMPEXP_BASE wxNullVariant;

#endif // wxUSE_VARIANT

#endif // _WX_VARIANT_H_
