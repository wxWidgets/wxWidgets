/////////////////////////////////////////////////////////////////////////////
// Name:        variant.cpp
// Purpose:     wxVariant class, container for any type
// Author:      Julian Smart
// Modified by:
// Created:     10/09/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "variant.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_IOSTREAMH
#   include <fstream.h>
#else
#   include <fstream>
#   ifdef _MSC_VER
        using namespace std;
#   endif
#endif

#include "wx/stream.h"
#include "wx/string.h"
#include "wx/variant.h"

IMPLEMENT_ABSTRACT_CLASS(wxVariantData, wxObject)

wxVariant wxNullVariant;

/*
 * wxVariantDataList
 */

class WXDLLEXPORT wxVariantDataList: public wxVariantData
{
DECLARE_DYNAMIC_CLASS(wxVariantDataList)
public:
    wxVariantDataList() {}
    wxVariantDataList(const wxList& list);
    ~wxVariantDataList();

    wxList& GetValue() const { return (wxList&) m_value; }
    void SetValue(const wxList& value) ;

    virtual void Copy(wxVariantData& data);
    virtual bool Eq(wxVariantData& data) const;
    virtual bool Write(ostream& str) const;
    virtual bool Write(wxString& str) const;
    virtual bool Read(istream& str);
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return "list"; };

    void Clear();

protected:
    wxList  m_value;
};

IMPLEMENT_DYNAMIC_CLASS(wxVariantDataList, wxVariantData)

wxVariantDataList::wxVariantDataList(const wxList& list)
{
    SetValue(list);
}

wxVariantDataList::~wxVariantDataList()
{
    Clear();
}

void wxVariantDataList::SetValue(const wxList& value)
{
    Clear();
    wxNode* node = value.First();
    while (node)
    {
        wxVariant* var = (wxVariant*) node->Data();
        m_value.Append(new wxVariant(*var));
        node = node->Next();
    }
}

void wxVariantDataList::Clear()
{
    wxNode* node = m_value.First();
    while (node)
    {
        wxVariant* var = (wxVariant*) node->Data();
        delete var;
        node = node->Next();
    }
    m_value.Clear();
}

void wxVariantDataList::Copy(wxVariantData& data)
{
    wxASSERT_MSG( (data.GetType() == "list"), "wxVariantDataList::Copy: Can't copy to this type of data" );

    wxVariantDataList& listData = (wxVariantDataList&) data;

    listData.Clear();
    wxNode* node = m_value.First();
    while (node)
    {
        wxVariant* var = (wxVariant*) node->Data();
        listData.m_value.Append(new wxVariant(*var));
        node = node->Next();
    }
}

bool wxVariantDataList::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == "list"), "wxVariantDataList::Eq: argument mismatch" );

    wxVariantDataList& listData = (wxVariantDataList&) data;
    wxNode* node1 = m_value.First();
    wxNode* node2 = listData.GetValue().First();
    while (node1 && node2)
    {
        wxVariant* var1 = (wxVariant*) node1->Data();
        wxVariant* var2 = (wxVariant*) node2->Data();
        if ((*var1) != (*var2))
            return FALSE;
        node1 = node1->Next();
        node2 = node2->Next();
    }
    if (node1 || node2) return FALSE;
    return TRUE;
}

bool wxVariantDataList::Write(ostream& str) const
{
    wxString s;
    Write(s);
    str << s;
    return TRUE;
}

bool wxVariantDataList::Write(wxString& str) const
{
    str = "";
    wxNode* node = m_value.First();
    while (node)
    {
        wxVariant* var = (wxVariant*) node->Data();
        if (node != m_value.First())
          str += " ";
        wxString str1;
        str += var->MakeString();
        node = node->Next();
    }

    return TRUE;
}

bool wxVariantDataList::Read(istream& WXUNUSED(str))
{
    wxFAIL_MSG("Unimplemented");
    // TODO
    return FALSE;
}

bool wxVariantDataList::Read(wxString& WXUNUSED(str))
{
    wxFAIL_MSG("Unimplemented");
    // TODO
    return FALSE;
}

/*
 * wxVariantDataStringList
 */

class WXDLLEXPORT wxVariantDataStringList: public wxVariantData
{
DECLARE_DYNAMIC_CLASS(wxVariantDataStringList)
public:
    wxVariantDataStringList() {}
    wxVariantDataStringList(const wxStringList& list) { m_value = list; }

    wxStringList& GetValue() const { return (wxStringList&) m_value; }
    void SetValue(const wxStringList& value);

    virtual void Copy(wxVariantData& data);
    virtual bool Eq(wxVariantData& data) const;
    virtual bool Write(ostream& str) const;
    virtual bool Write(wxString& str) const;
    virtual bool Read(istream& str);
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return "stringlist"; };

protected:
    wxStringList  m_value;
};

IMPLEMENT_DYNAMIC_CLASS(wxVariantDataStringList, wxVariantData)

void wxVariantDataStringList::SetValue(const wxStringList& value)
{
    m_value = value;
}

void wxVariantDataStringList::Copy(wxVariantData& data)
{
    wxASSERT_MSG( (data.GetType() == "stringlist"), "wxVariantDataStringList::Copy: Can't copy to this type of data" );

    wxVariantDataStringList& listData = (wxVariantDataStringList&) data;

    listData.m_value = m_value ;
}

bool wxVariantDataStringList::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == "stringlist"), "wxVariantDataStringList::Eq: argument mismatch" );

    wxVariantDataStringList& listData = (wxVariantDataStringList&) data;
    wxNode* node1 = m_value.First();
    wxNode* node2 = listData.GetValue().First();
    while (node1 && node2)
    {
        wxString str1 ((char*) node1->Data());
        wxString str2 ((char*) node2->Data());
        if (str1 != str2)
            return FALSE;
        node1 = node1->Next();
        node2 = node2->Next();
    }
    if (node1 || node2) return FALSE;
    return TRUE;
}

bool wxVariantDataStringList::Write(ostream& str) const
{
    wxString s;
    Write(s);
    str << s;
    return TRUE;
}

bool wxVariantDataStringList::Write(wxString& str) const
{
    str = "";
    wxNode* node = m_value.First();
    while (node)
    {
        char* s = (char*) node->Data();
        if (node != m_value.First())
          str += " ";
        str += s;
        node = node->Next();
    }

    return TRUE;
}

bool wxVariantDataStringList::Read(istream& WXUNUSED(str))
{
    wxFAIL_MSG("Unimplemented");
    // TODO
    return FALSE;
}

bool wxVariantDataStringList::Read(wxString& WXUNUSED(str))
{
    wxFAIL_MSG("Unimplemented");
    // TODO
    return FALSE;
}

/*
 * wxVariantDataLong
 */

class WXDLLEXPORT wxVariantDataLong: public wxVariantData
{
DECLARE_DYNAMIC_CLASS(wxVariantDataLong)
public:
    wxVariantDataLong() { m_value = 0; }
    wxVariantDataLong(long value) { m_value = value; }

    inline long GetValue() const { return m_value; }
    inline void SetValue(long value) { m_value = value; }

    virtual void Copy(wxVariantData& data);
    virtual bool Eq(wxVariantData& data) const;
    virtual bool Write(ostream& str) const;
    virtual bool Write(wxString& str) const;
    virtual bool Write(wxOutputStream &str) const;
    virtual bool Read(istream& str);
    virtual bool Read(wxInputStream& str);
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return "long"; };

protected:
    long m_value;
};

IMPLEMENT_DYNAMIC_CLASS(wxVariantDataLong, wxVariantData)

void wxVariantDataLong::Copy(wxVariantData& data)
{
    wxASSERT_MSG( (data.GetType() == "long"), "wxVariantDataLong::Copy: Can't copy to this type of data" );

    wxVariantDataLong& otherData = (wxVariantDataLong&) data;

    otherData.m_value = m_value;
}

bool wxVariantDataLong::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == "long"), "wxVariantDataLong::Eq: argument mismatch" );

    wxVariantDataLong& otherData = (wxVariantDataLong&) data;

    return (otherData.m_value == m_value);
}

bool wxVariantDataLong::Write(ostream& str) const
{
    wxString s;
    Write(s);
    str << s;
    return TRUE;
}

bool wxVariantDataLong::Write(wxOutputStream& str) const
{
    str << m_value;
    return TRUE;
}

bool wxVariantDataLong::Write(wxString& str) const
{
    str.Printf("%ld", m_value);
    return TRUE;
}

bool wxVariantDataLong::Read(istream& str)
{
    str >> m_value;
    return TRUE;
}

bool wxVariantDataLong::Read(wxInputStream& str)
{
   str >> m_value;
   return TRUE;
}

bool wxVariantDataLong::Read(wxString& str)
{
    m_value = atol((const char*) str);
    return TRUE;
}

/*
 * wxVariantDataReal
 */

class WXDLLEXPORT wxVariantDataReal: public wxVariantData
{
DECLARE_DYNAMIC_CLASS(wxVariantDataReal)
public:
    wxVariantDataReal() { m_value = 0.0; }
    wxVariantDataReal(double value) { m_value = value; }

    inline double GetValue() const { return m_value; }
    inline void SetValue(double value) { m_value = value; }

    virtual void Copy(wxVariantData& data);
    virtual bool Eq(wxVariantData& data) const;
    virtual bool Write(ostream& str) const;
    virtual bool Write(wxString& str) const;
    virtual bool Write(wxOutputStream &str) const;
    virtual bool Read(istream& str);
    virtual bool Read(wxInputStream& str);
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return "double"; };

protected:
    double m_value;
};

IMPLEMENT_DYNAMIC_CLASS(wxVariantDataReal, wxVariantData)

void wxVariantDataReal::Copy(wxVariantData& data)
{
    wxASSERT_MSG( (data.GetType() == "double"), "wxVariantDataReal::Copy: Can't copy to this type of data" );

    wxVariantDataReal& otherData = (wxVariantDataReal&) data;

    otherData.m_value = m_value;
}

bool wxVariantDataReal::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == "double"), "wxVariantDataReal::Eq: argument mismatch" );

    wxVariantDataReal& otherData = (wxVariantDataReal&) data;

    return (otherData.m_value == m_value);
}

bool wxVariantDataReal::Write(ostream& str) const
{
    wxString s;
    Write(s);
    str << s;
    return TRUE;
}

bool wxVariantDataReal::Write(wxOutputStream& str) const
{
    str << m_value;
    return TRUE;
}

bool wxVariantDataReal::Write(wxString& str) const
{
    str.Printf("%.4f", m_value);
    return TRUE;
}

bool wxVariantDataReal::Read(istream& str)
{
    str >> m_value;
    return TRUE;
}

bool wxVariantDataReal::Read(wxInputStream& str)
{
    str >> (float&)m_value;
    return TRUE;
}

bool wxVariantDataReal::Read(wxString& str)
{
    m_value = atof((const char*) str);
    return TRUE;
}

/*
 * wxVariantDataBool
 */

class WXDLLEXPORT wxVariantDataBool: public wxVariantData
{
DECLARE_DYNAMIC_CLASS(wxVariantDataBool)
public:
    wxVariantDataBool() { m_value = 0; }
    wxVariantDataBool(bool value) { m_value = value; }

    inline bool GetValue() const { return m_value; }
    inline void SetValue(bool value) { m_value = value; }

    virtual void Copy(wxVariantData& data);
    virtual bool Eq(wxVariantData& data) const;
    virtual bool Write(ostream& str) const;
    virtual bool Write(wxOutputStream& str) const;
    virtual bool Write(wxString& str) const;
    virtual bool Read(istream& str);
    virtual bool Read(wxInputStream& str);
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return "bool"; };

protected:
    bool m_value;
};

IMPLEMENT_DYNAMIC_CLASS(wxVariantDataBool, wxVariantData)

void wxVariantDataBool::Copy(wxVariantData& data)
{
    wxASSERT_MSG( (data.GetType() == "bool"), "wxVariantDataBool::Copy: Can't copy to this type of data" );

    wxVariantDataBool& otherData = (wxVariantDataBool&) data;

    otherData.m_value = m_value;
}

bool wxVariantDataBool::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == "bool"), "wxVariantDataBool::Eq: argument mismatch" );

    wxVariantDataBool& otherData = (wxVariantDataBool&) data;

    return (otherData.m_value == m_value);
}

bool wxVariantDataBool::Write(ostream& str) const
{
    wxString s;
    Write(s);
    str << s;
    return TRUE;
}

bool wxVariantDataBool::Write(wxOutputStream& str) const
{
    str << (char)m_value;
    return TRUE;
}

bool wxVariantDataBool::Write(wxString& str) const
{
    str.Printf("%d", (int) m_value);
    return TRUE;
}

bool wxVariantDataBool::Read(istream& WXUNUSED(str))
{
    wxFAIL_MSG("Unimplemented");
//    str >> (long) m_value;
    return FALSE;
}

bool wxVariantDataBool::Read(wxInputStream& str)
{
    str >> (char&)m_value;
    return TRUE;
}

bool wxVariantDataBool::Read(wxString& str)
{
    m_value = (atol((const char*) str) != 0);
    return TRUE;
}

/*
 * wxVariantDataChar
 */

class WXDLLEXPORT wxVariantDataChar: public wxVariantData
{
DECLARE_DYNAMIC_CLASS(wxVariantDataChar)
public:
    wxVariantDataChar() { m_value = 0; }
    wxVariantDataChar(char value) { m_value = value; }

    inline char GetValue() const { return m_value; }
    inline void SetValue(char value) { m_value = value; }

    virtual void Copy(wxVariantData& data);
    virtual bool Eq(wxVariantData& data) const;
    virtual bool Write(ostream& str) const;
    virtual bool Write(wxOutputStream& str) const;
    virtual bool Write(wxString& str) const;
    virtual bool Read(istream& str);
    virtual bool Read(wxInputStream& str);
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return "char"; };

protected:
    char m_value;
};

IMPLEMENT_DYNAMIC_CLASS(wxVariantDataChar, wxVariantData)

void wxVariantDataChar::Copy(wxVariantData& data)
{
    wxASSERT_MSG( (data.GetType() == "char"), "wxVariantDataChar::Copy: Can't copy to this type of data" );

    wxVariantDataChar& otherData = (wxVariantDataChar&) data;

    otherData.m_value = m_value;
}

bool wxVariantDataChar::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == "char"), "wxVariantDataChar::Eq: argument mismatch" );

    wxVariantDataChar& otherData = (wxVariantDataChar&) data;

    return (otherData.m_value == m_value);
}

bool wxVariantDataChar::Write(ostream& str) const
{
    wxString s;
    Write(s);
    str << s;
    return TRUE;
}

bool wxVariantDataChar::Write(wxOutputStream& str) const
{
    str << m_value;
    return TRUE;
}

bool wxVariantDataChar::Write(wxString& str) const
{
    str.Printf("%c", m_value);
    return TRUE;
}

bool wxVariantDataChar::Read(istream& WXUNUSED(str))
{
    wxFAIL_MSG("Unimplemented");
//    str >> m_value;
    return FALSE;
}

bool wxVariantDataChar::Read(wxInputStream& str)
{
    str >> m_value;
    return TRUE;
}

bool wxVariantDataChar::Read(wxString& str)
{
    m_value = str[(size_t)0];
    return TRUE;
}

/*
 * wxVariantDataString
 */

class WXDLLEXPORT wxVariantDataString: public wxVariantData
{
DECLARE_DYNAMIC_CLASS(wxVariantDataString)
public:
    wxVariantDataString() { }
    wxVariantDataString(const wxString& value) { m_value = value; }

    inline wxString GetValue() const { return m_value; }
    inline void SetValue(const wxString& value) { m_value = value; }

    virtual void Copy(wxVariantData& data);
    virtual bool Eq(wxVariantData& data) const;
    virtual bool Write(ostream& str) const;
    virtual bool Write(wxOutputStream& str) const;
    virtual bool Write(wxString& str) const;
    virtual bool Read(istream& str);
    virtual bool Read(wxInputStream& str);
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return "string"; };

protected:
    wxString m_value;
};

void wxVariantDataString::Copy(wxVariantData& data)
{
    wxASSERT_MSG( (data.GetType() == "string"), "wxVariantDataString::Copy: Can't copy to this type of data" );

    wxVariantDataString& otherData = (wxVariantDataString&) data;

    otherData.m_value = m_value;
}

bool wxVariantDataString::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == "string"), "wxVariantDataString::Eq: argument mismatch" );

    wxVariantDataString& otherData = (wxVariantDataString&) data;

    return (otherData.m_value == m_value);
}

bool wxVariantDataString::Write(ostream& str) const
{
    str << m_value;
    return TRUE;
}

bool wxVariantDataString::Write(wxOutputStream& str) const
{
    str << (const char*) m_value;
    return TRUE;
}

bool wxVariantDataString::Write(wxString& str) const
{
    str = m_value;
    return TRUE;
}

bool wxVariantDataString::Read(istream& str)
{
    str >> m_value;
    return TRUE;
}

bool wxVariantDataString::Read(wxInputStream& str)
{
    str >> m_value;
    return TRUE;
}

bool wxVariantDataString::Read(wxString& str)
{
    m_value = str;
    return TRUE;
}

IMPLEMENT_DYNAMIC_CLASS(wxVariantDataString, wxVariantData)

/*
 * wxVariant
 */

IMPLEMENT_DYNAMIC_CLASS(wxVariant, wxObject)

// Construction & destruction
wxVariant::wxVariant()
{
    m_data = (wxVariantData*) NULL;
}

wxVariant::wxVariant(double val)
{
    m_data = new wxVariantDataReal(val);
}

wxVariant::wxVariant(long val)
{
    m_data = new wxVariantDataLong(val);
}

wxVariant::wxVariant(bool val)
{
    m_data = new wxVariantDataBool(val);
}

wxVariant::wxVariant(char val)
{
    m_data = new wxVariantDataChar(val);
}

wxVariant::wxVariant(const wxString& val)
{
    m_data = new wxVariantDataString(val);
}

wxVariant::wxVariant(const char* val)
{
    m_data = new wxVariantDataString(wxString(val));
}

/* Causes ambiguity
wxVariant::wxVariant(const wxStringList& val)
{
    m_data = new wxVariantDataStringList(val);
}
*/

wxVariant::wxVariant(const wxList& val) // List of variants
{
    m_data = new wxVariantDataList(val);
}

wxVariant::wxVariant(const wxVariant& variant)
{
    if (!variant.IsNull())
    {
        m_data = (wxVariantData*) variant.GetData()->GetClassInfo()->CreateObject();
        variant.m_data->Copy(*m_data);
    }
}

wxVariant::wxVariant(wxVariantData* data) // User-defined data
{
    m_data = data;
}

wxVariant::~wxVariant()
{
    delete m_data;
}


// Make NULL (i.e. delete the data)
void wxVariant::MakeNull()
{
    delete m_data;
    m_data = NULL;
}

// Generic operators
// Assignment
void wxVariant::operator= (const wxVariant& variant)
{
    if (variant.IsNull())
    {
        MakeNull();
        return;
    }

    if (IsNull() || (GetType() != variant.GetType()))
    {
        if (m_data)
            delete m_data;
        m_data = (wxVariantData*) variant.GetData()->GetClassInfo()->CreateObject();
    }
    GetData()->Copy(* variant.GetData());
}

// Assignment using data, e.g.
// myVariant = new wxStringVariantData("hello")
void wxVariant::operator= (wxVariantData* variantData)
{
    MakeNull();
    m_data = variantData;
}

bool wxVariant::operator== (const wxVariant& variant) const
{
    if (IsNull() || variant.IsNull())
        return (IsNull() == variant.IsNull());

    return (GetData()->Eq(* variant.GetData()));
}

bool wxVariant::operator!= (const wxVariant& variant) const
{
    return (!(*this == variant));
}


// Specific operators
bool wxVariant::operator== (double value) const
{
    double thisValue;
    if (!Convert(&thisValue))
        return FALSE;
    else
        return (value == thisValue);
}

bool wxVariant::operator!= (double value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (double value)
{
    if (GetType() == "double")
    {
        ((wxVariantDataReal*)GetData())->SetValue(value);
    }
    else
    {
        if (m_data)
            delete m_data;
        m_data = new wxVariantDataReal(value);
    }
}

bool wxVariant::operator== (long value) const
{
    long thisValue;
    if (!Convert(&thisValue))
        return FALSE;
    else
        return (value == thisValue);
}

bool wxVariant::operator!= (long value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (long value)
{
    if (GetType() == "long")
    {
        ((wxVariantDataLong*)GetData())->SetValue(value);
    }
    else
    {
        if (m_data)
            delete m_data;
        m_data = new wxVariantDataLong(value);
    }
}

bool wxVariant::operator== (char value) const
{
    char thisValue;
    if (!Convert(&thisValue))
        return FALSE;
    else
        return (value == thisValue);
}

bool wxVariant::operator!= (char value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (char value)
{
    if (GetType() == "char")
    {
        ((wxVariantDataChar*)GetData())->SetValue(value);
    }
    else
    {
        if (m_data)
            delete m_data;
        m_data = new wxVariantDataChar(value);
    }
}

bool wxVariant::operator== (bool value) const
{
    bool thisValue;
    if (!Convert(&thisValue))
        return FALSE;
    else
        return (value == thisValue);
}

bool wxVariant::operator!= (bool value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (bool value)
{
    if (GetType() == "bool")
    {
        ((wxVariantDataBool*)GetData())->SetValue(value);
    }
    else
    {
        if (m_data)
            delete m_data;
        m_data = new wxVariantDataBool(value);
    }
}

bool wxVariant::operator== (const wxString& value) const
{
    wxString thisValue;
    if (!Convert(&thisValue))
        return FALSE;
    else
        return (value == thisValue);
}

bool wxVariant::operator!= (const wxString& value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (const wxString& value)
{
    if (GetType() == "string")
    {
        ((wxVariantDataString*)GetData())->SetValue(value);
    }
    else
    {
        if (m_data)
            delete m_data;
        m_data = new wxVariantDataString(value);
    }
}

void wxVariant::operator= (const char* value)
{
    if (GetType() == "string")
    {
        ((wxVariantDataString*)GetData())->SetValue(wxString(value));
    }
    else
    {
        if (m_data)
            delete m_data;
        m_data = new wxVariantDataString(wxString(value));
    }
}

bool wxVariant::operator== (const wxStringList& value) const
{
    wxASSERT_MSG( (GetType() == "stringlist"), "Invalid type for == operator" );

    wxVariantDataStringList other(value);
    return (m_data->Eq(other));
}

bool wxVariant::operator!= (const wxStringList& value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (const wxStringList& value)
{
    if (GetType() == "stringlist")
    {
        ((wxVariantDataStringList*)GetData())->SetValue(value);
    }
    else
    {
        if (m_data)
            delete m_data;
        m_data = new wxVariantDataStringList(value);
    }
}

bool wxVariant::operator== (const wxList& value) const
{
    wxASSERT_MSG( (GetType() == "list"), "Invalid type for == operator" );

    wxVariantDataList other(value);
    return (m_data->Eq(other));
}

bool wxVariant::operator!= (const wxList& value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (const wxList& value)
{
    if (GetType() == "list")
    {
        ((wxVariantDataList*)GetData())->SetValue(value);
    }
    else
    {
        if (m_data)
            delete m_data;
        m_data = new wxVariantDataList(value);
    }
}


// Treat a list variant as an array
wxVariant wxVariant::operator[] (size_t idx) const
{
    wxASSERT_MSG( (GetType() == "list" || GetType() == "stringlist"), "Invalid type for array operator" );

    if (GetType() == "list")
    {
        wxVariantDataList* data = (wxVariantDataList*) m_data;
        wxASSERT_MSG( (idx < (size_t) data->GetValue().Number()), "Invalid index for array" );
        return * (wxVariant*) (data->GetValue().Nth(idx)->Data());
    }
    else if (GetType() == "stringlist")
    {
        wxVariantDataStringList* data = (wxVariantDataStringList*) m_data;
        wxASSERT_MSG( (idx < (size_t) data->GetValue().Number()), "Invalid index for array" );

        wxVariant variant( wxString( (char*) (data->GetValue().Nth(idx)->Data()) ));
        return variant;
    }
    return wxNullVariant;
}

wxVariant& wxVariant::operator[] (size_t idx)
{
    // We can't return a reference to a variant for a string list, since the string
    // is actually stored as a char*, not a variant.

    wxASSERT_MSG( (GetType() == "list"), "Invalid type for array operator" );

    wxVariantDataList* data = (wxVariantDataList*) m_data;
    wxASSERT_MSG( (idx < (size_t) data->GetValue().Number()), "Invalid index for array" );

    return * (wxVariant*) (data->GetValue().Nth(idx)->Data());
}

// Return the number of elements in a list
int wxVariant::GetCount() const
{
    wxASSERT_MSG( (GetType() == "list" || GetType() == "stringlist"), "Invalid type for GetCount()" );

    if (GetType() == "list")
    {
        wxVariantDataList* data = (wxVariantDataList*) m_data;
        return data->GetValue().Number();
    }
    else if (GetType() == "stringlist")
    {
        wxVariantDataStringList* data = (wxVariantDataStringList*) m_data;
        return data->GetValue().Number();
    }
    return 0;
}

wxString wxVariant::MakeString() const
{
    if (!IsNull())
    {
        wxString str;
        if (GetData()->Write(str))
            return str;
    }
    return wxString("");
}

// Accessors

void wxVariant::SetData(wxVariantData* data)
{
    if (m_data) delete m_data;
    m_data = data;
}


// Returns a string representing the type of the variant,
// e.g. "string", "bool", "stringlist", "list", "double", "long"
wxString wxVariant::GetType() const
{
    if (IsNull())
        return wxString("null");
    else
        return m_data->GetType();
}


bool wxVariant::IsType(const wxString& type) const
{
    return (GetType() == type);
}


// Value accessors
double wxVariant::GetReal() const
{
    double value;
    if (Convert(& value))
        return value;
    else
    {
        wxFAIL_MSG("Could not convert to a real number");
        return 0.0;
    }
}

long wxVariant::GetInteger() const
{
    long value;
    if (Convert(& value))
        return value;
    else
    {
        wxFAIL_MSG("Could not convert to an integer");
        return 0;
    }
}

char wxVariant::GetChar() const
{
    char value;
    if (Convert(& value))
        return value;
    else
    {
        wxFAIL_MSG("Could not convert to a char");
        return 0;
    }
}

bool wxVariant::GetBool() const
{
    bool value;
    if (Convert(& value))
        return value;
    else
    {
        wxFAIL_MSG("Could not convert to a bool");
        return 0;
    }
}

wxString wxVariant::GetString() const
{
    wxString value;
    if (Convert(& value))
        return value;
    else
    {
        wxFAIL_MSG("Could not convert to a string");
        return wxString("");
    }
}

wxList& wxVariant::GetList() const
{
    wxASSERT( (GetType() == "list") );

    return (wxList&) ((wxVariantDataList*) m_data)->GetValue();
}

wxStringList& wxVariant::GetStringList() const
{
    wxASSERT( (GetType() == "stringlist") );

    return (wxStringList&) ((wxVariantDataStringList*) m_data)->GetValue();
}

// Append to list
void wxVariant::Append(const wxVariant& value)
{
    wxList& list = GetList();

    list.Append(new wxVariant(value));
}

// Insert at front of list
void wxVariant::Insert(const wxVariant& value)
{
    wxList& list = GetList();

    list.Insert(new wxVariant(value));
}

// Returns TRUE if the variant is a member of the list
bool wxVariant::Member(const wxVariant& value) const
{
    wxList& list = GetList();

    wxNode* node = list.First();
    while (node)
    {
        wxVariant* other = (wxVariant*) node->Data();
        if (value == *other)
            return TRUE;
        node = node->Next();
    }
    return FALSE;
}

// Deletes the nth element of the list
bool wxVariant::Delete(int item)
{
    wxList& list = GetList();

    wxASSERT_MSG( (item < list.Number()), "Invalid index to Delete" );
    wxNode* node = list.Nth(item);
    wxVariant* variant = (wxVariant*) node->Data();
    delete variant;
    delete node;
    return TRUE;
}

// Clear list
void wxVariant::ClearList()
{
    if (!IsNull() && (GetType() == "list"))
    {
        ((wxVariantDataList*) m_data)->Clear();
    }
    else
    {
        if (GetType() != "list")
        {
            delete m_data;
            m_data = NULL;
        }
        m_data = new wxVariantDataList;
    }
}

// Type conversion
bool wxVariant::Convert(long* value) const
{
    wxString type(GetType());
    if (type == "double")
        *value = (long) (((wxVariantDataReal*)GetData())->GetValue());
    else if (type == "long")
        *value = ((wxVariantDataLong*)GetData())->GetValue();
    else if (type == "bool")
        *value = (long) (((wxVariantDataBool*)GetData())->GetValue());
    else if (type == "string")
        *value = atol((const char*) ((wxVariantDataString*)GetData())->GetValue());
    else
        return FALSE;

    return TRUE;
}

bool wxVariant::Convert(bool* value) const
{
    wxString type(GetType());
    if (type == "double")
        *value = ((int) (((wxVariantDataReal*)GetData())->GetValue()) != 0);
    else if (type == "long")
        *value = (((wxVariantDataLong*)GetData())->GetValue() != 0);
    else if (type == "bool")
        *value = ((wxVariantDataBool*)GetData())->GetValue();
    else if (type == "string")
    {
        wxString val(((wxVariantDataString*)GetData())->GetValue());
        val.MakeLower();
        if (val == "true" || val == "yes")
            *value = TRUE;
        else if (val == "false" || val == "no")
            *value = FALSE;
        else
            return FALSE;
    }
    else
        return FALSE;

    return TRUE;
}

bool wxVariant::Convert(double* value) const
{
    wxString type(GetType());
    if (type == "double")
        *value = ((wxVariantDataReal*)GetData())->GetValue();
    else if (type == "long")
        *value = (double) (((wxVariantDataLong*)GetData())->GetValue());
    else if (type == "bool")
        *value = (double) (((wxVariantDataBool*)GetData())->GetValue());
    else if (type == "string")
        *value = (double) atof((const char*) ((wxVariantDataString*)GetData())->GetValue());
    else
        return FALSE;

    return TRUE;
}

bool wxVariant::Convert(char* value) const
{
    wxString type(GetType());
    if (type == "char")
        *value = ((wxVariantDataChar*)GetData())->GetValue();
    else if (type == "long")
        *value = (char) (((wxVariantDataLong*)GetData())->GetValue());
    else if (type == "bool")
        *value = (char) (((wxVariantDataBool*)GetData())->GetValue());
    else
        return FALSE;

    return TRUE;
}

bool wxVariant::Convert(wxString* value) const
{
    *value = MakeString();
    return TRUE;
}

