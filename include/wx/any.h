/////////////////////////////////////////////////////////////////////////////
// Name:        wx/any.h
// Purpose:     wxAny class
// Author:      Jaakko Salli
// Modified by:
// Created:     07/05/2009
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ANY_H_
#define _WX_ANY_H_

#include "wx/defs.h"

#if wxUSE_ANY

#include "wx/string.h"
#include "wx/meta/movable.h"
#include "wx/meta/if.h"


// Size of the wxAny value buffer.
enum
{
    WX_ANY_VALUE_BUFFER_SIZE = 16
};

union wxAnyValueBuffer
{
    void*   m_ptr;
    wxByte  m_buffer[WX_ANY_VALUE_BUFFER_SIZE];
};

typedef void (*wxAnyClassInfo)();


//
// wxAnyValueType is base class for value type functionality for C++ data
// types used with wxAny. Usually the default template (wxAnyValueTypeImpl<>)
// will create a satisfactory wxAnyValueType implementation for a data type.
//
class WXDLLIMPEXP_BASE wxAnyValueType
{
public:
    /**
        Default constructor.
    */
    wxAnyValueType();

    /**
        Destructor.
    */
    virtual ~wxAnyValueType()
    {
    }

    /**
        This function is used for internal type matching.
    */
    virtual wxAnyClassInfo GetClassInfo() const = 0;

    /**
        This function is used for internal type matching.
    */
    virtual bool IsSameType(const wxAnyValueType* otherType) const = 0;

    /**
        This function is called every time the data in wxAny
        buffer needs to be freed.
    */
    virtual void DeleteValue(wxAnyValueBuffer& buf) const = 0;

    /**
        Implement this for buffer-to-buffer copy. src.m_ptr can
        be expected to be NULL if value type of previously stored
        data was different.
    */
    virtual void CopyBuffer(const wxAnyValueBuffer& src,
                            wxAnyValueBuffer& dst) const = 0;

    /**
        Convert value into buffer of different type. Return false if
        not possible.
    */
    virtual bool ConvertValue(const wxAnyValueBuffer& src,
                              wxAnyValueType* dstType,
                              wxAnyValueBuffer& dst) const = 0;

    /**
        Use this template function for checking if wxAnyValueType represents
        a specific C++ data type.

        @remarks This template function does not work on some older compilers
                (such as Visual C++ 6.0). For full compiler ccompatibility
                please use wxANY_VALUE_TYPE_CHECK_TYPE(valueTypePtr, T) macro
                instead.

        @see wxAny::CheckType()
    */
    // FIXME-VC6: remove this hack when VC6 is no longer supported
    template <typename T>
    bool CheckType(T* reserved = NULL);
private:
};

//
// This method of checking the type is compatible with VC6
#define wxANY_VALUE_TYPE_CHECK_TYPE(valueTypePtr, T) \
    wxAnyValueTypeImpl<T>::IsSameClass(valueTypePtr)

    //valueTypePtr->CheckType(static_cast<T*>(NULL))


/**
    Helper macro for defining user value types.

    NB: We really cannot compare sm_classInfo directly in IsSameClass(),
        but instead call sm_instance->GetClassInfo(). The former technique
        broke at least on GCC 4.2 (but worked on VC8 shared build).
*/
#define WX_DECLARE_ANY_VALUE_TYPE(CLS) \
    friend class wxAny; \
public: \
    static void sm_classInfo() {} \
 \
    virtual wxAnyClassInfo GetClassInfo() const \
    { \
        return sm_classInfo; \
    } \
    static bool IsSameClass(const wxAnyValueType* otherType) \
    { \
        return sm_instance->GetClassInfo() == otherType->GetClassInfo(); \
    } \
    virtual bool IsSameType(const wxAnyValueType* otherType) const \
    { \
        return IsSameClass(otherType); \
    } \
private: \
    static CLS* sm_instance; \
public: \
    static wxAnyValueType* GetInstance() \
    { \
        return sm_instance; \
    }


#define WX_IMPLEMENT_ANY_VALUE_TYPE(CLS) \
    CLS* CLS::sm_instance = new CLS();


#ifdef __VISUALC6__
    // "non dll-interface class 'xxx' used as base interface
    #pragma warning (push)
    #pragma warning (disable:4275)
#endif

/**
    Following are helper classes for the wxAnyValueTypeImplBase.
*/
namespace wxPrivate
{

template<typename T>
class wxAnyValueTypeOpsMovable
{
public:
    static void DeleteValue(wxAnyValueBuffer& buf)
    {
        wxUnusedVar(buf);
    }

    static void SetValue(const T& value,
                         wxAnyValueBuffer& buf)
    {
        memcpy(buf.m_buffer, &value, sizeof(T));
    }

    static const T& GetValue(const wxAnyValueBuffer& buf)
    {
        return *(reinterpret_cast<const T*>(&buf.m_buffer[0]));
    }
};


template<typename T>
class wxAnyValueTypeOpsGeneric
{
public:
    template<typename T2>
    class DataHolder
    {
    public:
        DataHolder(const T2& value)
        {
            m_value = value;
        }
        virtual ~DataHolder() { }

        T2   m_value;
    private:
        wxDECLARE_NO_COPY_CLASS(DataHolder);
    };

    static void DeleteValue(wxAnyValueBuffer& buf)
    {
        DataHolder<T>* holder = static_cast<DataHolder<T>*>(buf.m_ptr);
        delete holder;
    }

    static void SetValue(const T& value,
                         wxAnyValueBuffer& buf)
    {
        DataHolder<T>* holder = new DataHolder<T>(value);
        buf.m_ptr = holder;
    }

    static const T& GetValue(const wxAnyValueBuffer& buf)
    {
        DataHolder<T>* holder = static_cast<DataHolder<T>*>(buf.m_ptr);
        return holder->m_value;
    }
};

} // namespace wxPrivate


/**
    Intermediate template for the generic value type implementation.
    We can derive from this same value type for multiple actual types
    (for instance, we can have wxAnyValueTypeImplInt for all signed
    integer types), and also easily implement specialized templates
    with specific dynamic type conversion.
*/
template<typename T>
class wxAnyValueTypeImplBase : public wxAnyValueType
{
    typedef typename wxIf< wxIsMovable<T>::value &&
                                sizeof(T) <= WX_ANY_VALUE_BUFFER_SIZE,
                           wxPrivate::wxAnyValueTypeOpsMovable<T>,
                           wxPrivate::wxAnyValueTypeOpsGeneric<T> >::value
            Ops;

public:
    wxAnyValueTypeImplBase() : wxAnyValueType() { }
    virtual ~wxAnyValueTypeImplBase() { }

    virtual void DeleteValue(wxAnyValueBuffer& buf) const
    {
        Ops::DeleteValue(buf);
        buf.m_ptr = NULL;  // This is important
    }

    virtual void CopyBuffer(const wxAnyValueBuffer& src,
                            wxAnyValueBuffer& dst) const
    {
        Ops::DeleteValue(dst);
        Ops::SetValue(Ops::GetValue(src), dst);
    }

    /**
        It is important to reimplement this in any specialized template
        classes that inherit from wxAnyValueTypeImplBase.
    */
    static void SetValue(const T& value,
                         wxAnyValueBuffer& buf)
    {
        Ops::SetValue(value, buf);
    }

    /**
        It is important to reimplement this in any specialized template
        classes that inherit from wxAnyValueTypeImplBase.
    */
    static const T& GetValue(const wxAnyValueBuffer& buf)
    {
        return Ops::GetValue(buf);
    }
};


/*
    Generic value type template. Note that bulk of the implementation
    resides in wxAnyValueTypeImplBase.
*/
template<typename T>
class wxAnyValueTypeImpl : public wxAnyValueTypeImplBase<T>
{
    WX_DECLARE_ANY_VALUE_TYPE(wxAnyValueTypeImpl<T>)
public:
    wxAnyValueTypeImpl() : wxAnyValueTypeImplBase<T>() { }
    virtual ~wxAnyValueTypeImpl() { }

    virtual bool ConvertValue(const wxAnyValueBuffer& src,
                              wxAnyValueType* dstType,
                              wxAnyValueBuffer& dst) const
    {
        wxUnusedVar(src);
        wxUnusedVar(dstType);
        wxUnusedVar(dst);
        return false;
    }
};

template<typename T>
wxAnyValueTypeImpl<T>* wxAnyValueTypeImpl<T>::sm_instance =
    new wxAnyValueTypeImpl<T>();


//
// Helper macro for using same base value type implementation for multiple
// actual C++ data types.
//
#define WX_ANY_DEFINE_SUB_TYPE(T, CLSTYPE) \
template<> \
class wxAnyValueTypeImpl<T> : public wxAnyValueTypeImpl##CLSTYPE \
{ \
    typedef wxAnyBase##CLSTYPE##Type UseDataType; \
public: \
    wxAnyValueTypeImpl() : wxAnyValueTypeImpl##CLSTYPE() { } \
    virtual ~wxAnyValueTypeImpl() { } \
    static void SetValue(const T& value, wxAnyValueBuffer& buf) \
    { \
        *(reinterpret_cast<UseDataType*>(&buf.m_buffer[0])) = \
            static_cast<UseDataType>(value); \
    } \
    static T GetValue(const wxAnyValueBuffer& buf) \
    { \
        return static_cast<T>( \
            *(reinterpret_cast<const UseDataType*>(&buf.m_buffer[0]))); \
    } \
};


//
//  Integer value types
//

#ifdef wxLongLong_t
    typedef wxLongLong_t wxAnyBaseIntType;
    typedef wxULongLong_t wxAnyBaseUintType;
#else
    typedef long wxAnyBaseIntType;
    typedef unsigned long wxAnyBaseUintType;
#endif


class WXDLLIMPEXP_BASE wxAnyValueTypeImplInt :
    public wxAnyValueTypeImplBase<wxAnyBaseIntType>
{
    WX_DECLARE_ANY_VALUE_TYPE(wxAnyValueTypeImplInt)
public:
    wxAnyValueTypeImplInt() :
        wxAnyValueTypeImplBase<wxAnyBaseIntType>() { }
    virtual ~wxAnyValueTypeImplInt() { }

    virtual bool ConvertValue(const wxAnyValueBuffer& src,
                              wxAnyValueType* dstType,
                              wxAnyValueBuffer& dst) const;
};


class WXDLLIMPEXP_BASE wxAnyValueTypeImplUint :
    public wxAnyValueTypeImplBase<wxAnyBaseUintType>
{
    WX_DECLARE_ANY_VALUE_TYPE(wxAnyValueTypeImplUint)
public:
    wxAnyValueTypeImplUint() :
        wxAnyValueTypeImplBase<wxAnyBaseUintType>() { }
    virtual ~wxAnyValueTypeImplUint() { }

    virtual bool ConvertValue(const wxAnyValueBuffer& src,
                              wxAnyValueType* dstType,
                              wxAnyValueBuffer& dst) const;
};


WX_ANY_DEFINE_SUB_TYPE(signed long, Int)
WX_ANY_DEFINE_SUB_TYPE(signed int, Int)
WX_ANY_DEFINE_SUB_TYPE(signed short, Int)
WX_ANY_DEFINE_SUB_TYPE(signed char, Int)
#ifdef wxLongLong_t
WX_ANY_DEFINE_SUB_TYPE(wxLongLong_t, Int)
#endif

WX_ANY_DEFINE_SUB_TYPE(unsigned long, Uint)
WX_ANY_DEFINE_SUB_TYPE(unsigned int, Uint)
WX_ANY_DEFINE_SUB_TYPE(unsigned short, Uint)
WX_ANY_DEFINE_SUB_TYPE(unsigned char, Uint)
#ifdef wxLongLong_t
WX_ANY_DEFINE_SUB_TYPE(wxULongLong_t, Uint)
#endif


//
// String value type
//
class WXDLLIMPEXP_BASE wxAnyValueTypeImplString :
    public wxAnyValueTypeImplBase<wxString>
{
    WX_DECLARE_ANY_VALUE_TYPE(wxAnyValueTypeImplString)
public:
    wxAnyValueTypeImplString() :
        wxAnyValueTypeImplBase<wxString>() { }
    virtual ~wxAnyValueTypeImplString() { }

    /**
        Convert value into buffer of different type. Return false if
        not possible.
    */
    virtual bool ConvertValue(const wxAnyValueBuffer& src,
                              wxAnyValueType* dstType,
                              wxAnyValueBuffer& dst) const;

};

template<>
class wxAnyValueTypeImpl<wxString> : public wxAnyValueTypeImplString
{
public:
    wxAnyValueTypeImpl() : wxAnyValueTypeImplString() { }
    virtual ~wxAnyValueTypeImpl() { }
};


//
// Bool value type
//
template<>
class WXDLLIMPEXP_BASE wxAnyValueTypeImpl<bool> :
    public wxAnyValueTypeImplBase<bool>
{
    WX_DECLARE_ANY_VALUE_TYPE(wxAnyValueTypeImpl<bool>)
public:
    wxAnyValueTypeImpl() :
        wxAnyValueTypeImplBase<bool>() { }
    virtual ~wxAnyValueTypeImpl() { }

    virtual bool ConvertValue(const wxAnyValueBuffer& src,
                              wxAnyValueType* dstType,
                              wxAnyValueBuffer& dst) const;
};

//
// Floating point value type
//
class WXDLLIMPEXP_BASE wxAnyValueTypeImplDouble :
    public wxAnyValueTypeImplBase<double>
{
    WX_DECLARE_ANY_VALUE_TYPE(wxAnyValueTypeImplDouble)
public:
    wxAnyValueTypeImplDouble() :
        wxAnyValueTypeImplBase<double>() { }
    virtual ~wxAnyValueTypeImplDouble() { }

    virtual bool ConvertValue(const wxAnyValueBuffer& src,
                              wxAnyValueType* dstType,
                              wxAnyValueBuffer& dst) const;
};

// WX_ANY_DEFINE_SUB_TYPE requires this
typedef double wxAnyBaseDoubleType;

WX_ANY_DEFINE_SUB_TYPE(float, Double)
WX_ANY_DEFINE_SUB_TYPE(double, Double)


#ifdef __VISUALC6__
    // Re-enable useless VC6 warnings
    #pragma warning (pop)
#endif


/*
    Let's define a discrete Null value so we don't have to really
    ever check if wxAny.m_type pointer is NULL or not. This is an
    optimization, mostly. Implementation of this value type is
    "hidden" in the source file.
*/
extern WXDLLIMPEXP_DATA_BASE(wxAnyValueType*) wxAnyNullValueType;


//
// We need to implement custom signed/unsigned int equals operators
// for signed/unsigned (eg. wxAny(128UL) == 128L) comparisons to work.
#define WXANY_IMPLEMENT_INT_EQ_OP(TS, TUS) \
bool operator==(TS value) const \
{ \
    if ( wxAnyValueTypeImpl<TS>::IsSameClass(m_type) ) \
        return (value == static_cast<TS> \
                (wxAnyValueTypeImpl<TS>::GetValue(m_buffer))); \
    if ( wxAnyValueTypeImpl<TUS>::IsSameClass(m_type) ) \
        return (value == static_cast<TS> \
                (wxAnyValueTypeImpl<TUS>::GetValue(m_buffer))); \
    return false; \
} \
bool operator==(TUS value) const \
{ \
    if ( wxAnyValueTypeImpl<TUS>::IsSameClass(m_type) ) \
        return (value == static_cast<TUS> \
                (wxAnyValueTypeImpl<TUS>::GetValue(m_buffer))); \
    if ( wxAnyValueTypeImpl<TS>::IsSameClass(m_type) ) \
        return (value == static_cast<TUS> \
                (wxAnyValueTypeImpl<TS>::GetValue(m_buffer))); \
    return false; \
}


//
// The wxAny class represents a container for any type. A variant's value
// can be changed at run time, possibly to a different type of value.
//
// As standard, wxAny can store value of almost any type, in a fairly
// optimal manner even.
//
class WXDLLIMPEXP_BASE wxAny
{
public:
    /**
        Default constructor.
    */
    wxAny()
    {
        m_type = wxAnyNullValueType;
    }

    /**
        Destructor.
    */
    ~wxAny()
    {
        m_type->DeleteValue(m_buffer);
    }

    //@{
    /**
        Various constructors.
    */
    wxAny(const char* value)
    {
        m_type = wxAnyNullValueType;
        Assign(wxString(value));
    }
    wxAny(const wchar_t* value)
    {
        m_type = wxAnyNullValueType;
        Assign(wxString(value));
    }

    wxAny(const wxAny& any)
    {
        m_type = wxAnyNullValueType;
        AssignAny(any);
    }

    template<typename T>
    wxAny(const T& value)
    {
        m_type = wxAnyValueTypeImpl<T>::sm_instance;
        wxAnyValueTypeImpl<T>::SetValue(value, m_buffer);
    }
    //@}

    /**
        Use this template function for checking if this wxAny holds
        a specific C++ data type.

        @remarks This template function does not work on some older compilers
                (such as Visual C++ 6.0). For full compiler ccompatibility
                please use wxANY_CHECK_TYPE(any, T) macro instead.

        @see wxAnyValueType::CheckType()
    */
    // FIXME-VC6: remove this hack when VC6 is no longer supported
    template <typename T>
    bool CheckType(T* = NULL)
    {
        return m_type->CheckType<T>();
    }

    /**
        Returns the value type as wxAnyValueType instance.

        @remarks You cannot reliably test whether two wxAnys are of
                 same value type by simply comparing return values
                 of wxAny::GetType(). Instead use
                 wxAnyValueType::CheckType<T>() template function.
    */
    const wxAnyValueType* GetType() const
    {
        return m_type;
    }

    /**
        Tests if wxAny is null (that is, whether there is data).
    */
    bool IsNull() const
    {
        return (m_type == wxAnyNullValueType);
    }

    /**
        Makes wxAny null (that is, clears it).
    */
    void MakeNull()
    {
        m_type->DeleteValue(m_buffer);
        m_type = wxAnyNullValueType;
    }

    //@{
    /**
        Assignment operators.
    */
    wxAny& operator=(const wxAny &any)
    {
        AssignAny(any);
        return *this;
    }

    template<typename T>
    wxAny& operator=(const T &value)
    {
        m_type->DeleteValue(m_buffer);
        m_type = wxAnyValueTypeImpl<T>::sm_instance;
        wxAnyValueTypeImpl<T>::SetValue(value, m_buffer);
        return *this;
    }

    wxAny& operator=(const char* value)
        { Assign(wxString(value)); return *this; }
    wxAny& operator=(const wchar_t* value)
        { Assign(wxString(value)); return *this; }
    //@}

    //@{
    /**
        Equality operators.
    */
    bool operator==(const wxString& value) const
    {
        if ( !wxAnyValueTypeImpl<wxString>::IsSameClass(m_type) )
            return false;

        return value ==
            static_cast<wxString>
                (wxAnyValueTypeImpl<wxString>::GetValue(m_buffer));
    }

    bool operator==(const char* value) const
        { return (*this) == wxString(value); }
    bool operator==(const wchar_t* value) const
        { return (*this) == wxString(value); }

    //
    // We need to implement custom signed/unsigned int equals operators
    // for signed/unsigned (eg. wxAny(128UL) == 128L) comparisons to work.
    WXANY_IMPLEMENT_INT_EQ_OP(signed char, unsigned char)
    WXANY_IMPLEMENT_INT_EQ_OP(signed short, unsigned short)
    WXANY_IMPLEMENT_INT_EQ_OP(signed int, unsigned int)
    WXANY_IMPLEMENT_INT_EQ_OP(signed long, unsigned long)
#ifdef wxLongLong_t
    WXANY_IMPLEMENT_INT_EQ_OP(wxLongLong_t, wxULongLong_t)
#endif

    bool operator==(float value) const
    {
        if ( !wxAnyValueTypeImpl<float>::IsSameClass(m_type) )
            return false;

        return value ==
            static_cast<float>
                (wxAnyValueTypeImpl<float>::GetValue(m_buffer));
    }

    bool operator==(double value) const
    {
        if ( !wxAnyValueTypeImpl<double>::IsSameClass(m_type) )
            return false;

        return value ==
            static_cast<double>
                (wxAnyValueTypeImpl<double>::GetValue(m_buffer));
    }

    bool operator==(bool value) const
    {
        if ( !wxAnyValueTypeImpl<bool>::IsSameClass(m_type) )
            return false;

        return value == (wxAnyValueTypeImpl<bool>::GetValue(m_buffer));
    }

    //@}

    //@{
    /**
        Inequality operators (implement as template).
    */
    template<typename T>
    bool operator!=(const T& value) const
        { return !((*this) == value); }
    //@}

    /**
        This template function converts wxAny into given type. No dynamic
        conversion is performed, so if the type is incorrect an assertion
        failure will occur in debug builds, and a bogus value is returned
        in release ones.

        @remarks This template function does not work on some older compilers
                (such as Visual C++ 6.0). For full compiler ccompatibility
                please use wxANY_AS(any, T) macro instead.
    */
    // FIXME-VC6: remove this hack when VC6 is no longer supported
    template<typename T>
    T As(T* = NULL) const
    {
        if ( !wxAnyValueTypeImpl<T>::IsSameClass(m_type) )
            wxFAIL_MSG("Incorrect or non-convertible data type");
        return static_cast<T>(wxAnyValueTypeImpl<T>::GetValue(m_buffer));
    }

    /**
        Template function that etrieves and converts the value of this
        variant to the type that T* value is.

        @return Returns @true if conversion was succesfull.
    */
    template<typename T>
    bool GetAs(T* value) const
    {
        if ( !wxAnyValueTypeImpl<T>::IsSameClass(m_type) )
        {
            wxAnyValueType* otherType =
                wxAnyValueTypeImpl<T>::sm_instance;
            wxAnyValueBuffer temp_buf;

            if ( !m_type->ConvertValue(m_buffer, otherType, temp_buf) )
                return false;

            *value =
                static_cast<T>(wxAnyValueTypeImpl<T>::GetValue(temp_buf));
            otherType->DeleteValue(temp_buf);

            return true;
        }
        *value = static_cast<T>(wxAnyValueTypeImpl<T>::GetValue(m_buffer));
        return true;
    }

private:
    // Assignment functions
    void AssignAny(const wxAny &any);

    template<typename T>
    void Assign(const T &value)
    {
        m_type->DeleteValue(m_buffer);
        m_type = wxAnyValueTypeImpl<T>::sm_instance;
        wxAnyValueTypeImpl<T>::SetValue(value, m_buffer);
    }

    // Data
    wxAnyValueType*     m_type;
    wxAnyValueBuffer    m_buffer;
};


//
// This method of checking the type is compatible with VC6
#define wxANY_CHECK_TYPE(any, T) \
    wxANY_VALUE_TYPE_CHECK_TYPE(any.GetType(), T)


//
// This method of getting the value is compatible with VC6
#define wxANY_AS(any, T) \
    any.As(static_cast<T*>(NULL))


template<typename T>
inline bool wxAnyValueType::CheckType(T* reserved)
{
    wxUnusedVar(reserved);
    return wxAnyValueTypeImpl<T>::IsSameClass(this);
}



#endif // wxUSE_ANY

#endif // _WX_ANY_H_
