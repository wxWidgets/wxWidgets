/////////////////////////////////////////////////////////////////////////////
// Name:        xmlparser.h
// Purpose:     Parser of the API/interface XML files
// Author:      Francesco Montorsi
// Created:     2008/03/17
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include <wx/txtstrm.h>
#include <wx/dynarray.h>
#include <wx/xml/xml.h>

// helper macros
#define LogMessage(fmt, ...)   { wxPrintf(fmt "\n", __VA_ARGS__); fflush(stdout); }
#define LogWarning(fmt, ...)   { wxPrintf(fmt "\n", __VA_ARGS__); fflush(stdout); }
#define LogError(fmt, ...)     { wxPrintf("ERROR: " fmt "\n", __VA_ARGS__); fflush(stdout); }
#define wxPrint(str)           { wxPrintf(str); fflush(stdout); }


// ----------------------------------------------------------------------------
// Represents a type with or without const/static/ qualifier
// and with or without & and * operators
// ----------------------------------------------------------------------------
class wxType
{
public:
    wxType() {}
    wxType(const wxString& type)
        { SetFromString(type); }

    void SetFromString(const wxString& t);
    wxString GetAsString() const
        { return m_strType; }

    wxString GetClean() const;

    bool IsConst() const
        { return m_strType.Contains("const"); }
    bool IsStatic() const
        { return m_strType.Contains("static"); }
    bool IsPointer() const
        { return m_strType.Contains("*"); }
    bool IsReference() const
        { return m_strType.Contains("&"); }

    bool operator==(const wxType& m) const;
    bool operator!=(const wxType& m) const
        { return !(*this == m); }

    bool IsOk() const;

protected:
    wxString m_strType;
};

extern wxType wxEmptyType;
WX_DECLARE_OBJARRAY(wxType, wxTypeArray);



// ----------------------------------------------------------------------------
// Represents a single prototype of a class' member.
// ----------------------------------------------------------------------------
class wxMethod
{
public:
    wxMethod()
        { m_bConst=m_bVirtual=m_bStatic=m_bDeprecated=false; m_nLine=-1; }

    wxMethod(const wxType& rettype, const wxString& name,
             const wxTypeArray& arguments, const wxArrayString& defaults,
             bool isConst, bool isStatic, bool isVirtual)
        : m_retType(rettype), m_strName(name.Strip(wxString::both)),
          m_bConst(isConst), m_bStatic(isStatic), m_bVirtual(isVirtual)
        { SetArgumentTypes(arguments,defaults); m_nLine=-1; }


public:     // getters

    //void SetFromString(const wxString& proto);
    wxString GetAsString() const;

    // parser of the prototype:
    // all these functions return strings with spaces stripped
    wxType GetReturnType() const
        { return m_retType; }
    wxString GetName() const
        { return m_strName; }
    wxTypeArray GetArgumentTypes() const
        { return m_args; }
    wxArrayString GetArgumentDefaults() const
        { return m_argDefaults; }
    int GetLocation() const
        { return m_nLine; }

    bool IsConst() const
        { return m_bConst; }
    bool IsStatic() const
        { return m_bStatic; }
    bool IsVirtual() const
        { return m_bVirtual; }

    bool IsOk() const;
    bool IsCtor() const
        { return m_retType==wxEmptyType && !m_strName.StartsWith("~"); }
    bool IsDtor() const
        { return m_retType==wxEmptyType && m_strName.StartsWith("~"); }

    bool IsDeprecated() const
        { return m_bDeprecated; }


public:     // setters

    void SetReturnType(const wxType& t)
        { m_retType=t; }
    void SetName(const wxString& name)
        { m_strName=name; }
    void SetArgumentTypes(const wxTypeArray& arr, const wxArrayString& defaults);
    void SetConst(bool c = true)
        { m_bConst=c; }
    void SetStatic(bool c = true)
        { m_bStatic=c; }
    void SetVirtual(bool c = true)
        { m_bVirtual=c; }
    void SetDeprecated(bool c = true)
        { m_bDeprecated=c; }
    void SetLocation(int lineNumber)
        { m_nLine=lineNumber; }

public:     // misc

    bool operator==(const wxMethod&) const;
    bool operator!=(const wxMethod& m) const
        { return !(*this == m); }

    void Dump(wxTextOutputStream& stream) const;

protected:
    wxType m_retType;
    wxString m_strName;
    wxTypeArray m_args;
    wxArrayString m_argDefaults;
    bool m_bConst;
    bool m_bStatic;
    bool m_bVirtual;
    bool m_bDeprecated;
    int m_nLine;
};

WX_DECLARE_OBJARRAY(wxMethod, wxMethodArray);
WX_DEFINE_ARRAY(const wxMethod*, wxMethodPtrArray);


// ----------------------------------------------------------------------------
// Represents a class of the wx API/interface.
// ----------------------------------------------------------------------------
class wxClass
{
public:
    wxClass() {}
    wxClass(const wxString& name, const wxString& headername)
        : m_strName(name), m_strHeader(headername) {}

    void AddMethod(const wxMethod& func)
        { m_methods.Add(func); }

    void SetHeader(const wxString& header)
        { m_strHeader=header; }
    void SetName(const wxString& name)
        { m_strName=name; }
    wxString GetName() const
        { return m_strName; }
    wxString GetHeader() const
        { return m_strHeader; }
    wxString GetNameWithoutTemplate() const;

    bool IsOk() const
        { return !m_strName.IsEmpty() && !m_methods.IsEmpty(); }

    bool IsValidCtorForThisClass(const wxMethod& m) const;
    bool IsValidDtorForThisClass(const wxMethod& m) const;

    unsigned int GetMethodCount() const
        { return m_methods.GetCount(); }
    wxMethod& GetMethod(unsigned int n) const
        { return m_methods[n]; }
    wxMethod& GetLastMethod() const
        { return m_methods.Last(); }

    // returns a single result (the first, which is also the only
    // one if CheckConsistency() return true)
    const wxMethod* FindMethod(const wxMethod& m) const;

    // returns an array of pointers to the overloaded methods with the
    // same given name
    wxMethodPtrArray FindMethodNamed(const wxString& m) const;

    // dumps all methods to the given output stream
    void Dump(wxTextOutputStream& stream) const;

    // slow check
    bool CheckConsistency() const;

protected:
    wxString m_strName;
    wxString m_strHeader;
    wxMethodArray m_methods;
};

WX_DECLARE_OBJARRAY(wxClass, wxClassArray);
WX_DEFINE_ARRAY(const wxClass*, wxClassPtrArray);



// ----------------------------------------------------------------------------
// wxXmlInterface
// ----------------------------------------------------------------------------
class wxXmlInterface
{
public:
    wxXmlInterface() {}

    const wxClass* FindClass(const wxString& classname) const
        {
            for (unsigned int i=0; i<m_classes.GetCount(); i++)
                if (m_classes[i].GetName() == classname)
                    return &m_classes[i];
            return NULL;
        }

    void Dump(const wxString& filename);

    const wxClassArray& GetClasses() const
        { return m_classes; }

    unsigned int GetClassesCount() const
        { return m_classes.GetCount(); }

    unsigned int GetMethodCount() const
        {
            unsigned int methods = 0;
            for (unsigned i=0; i < m_classes.GetCount(); i++)
                methods += m_classes[i].GetMethodCount();
            return methods;
        }

    void ShowProgress()
        { /*wxPrint(".");*/ }

    bool CheckParseResults() const;

protected:
    wxClassArray m_classes;
};

#if 1
// for wxTypeIdHashMap, keys == gccxml IDs  and  values == associated type strings
// e.g. key = "0x123f" and value = "const wxAboutDialogInfo&"
WX_DECLARE_HASH_MAP( unsigned long, wxString,
                     wxIntegerHash, wxIntegerEqual,
                     wxTypeIdHashMap );
#else
#include <map>
typedef std::basic_string<char> stlString;
typedef std::map<unsigned long, stlString> wxTypeIdHashMap;
#endif


// ----------------------------------------------------------------------------
// Represents the real interface of wxWidgets
// Loads it from the XML produced by gccXML: http://www.gccxml.org
// ----------------------------------------------------------------------------
class wxXmlGccInterface : public wxXmlInterface
{
public:
    wxXmlGccInterface() {}

    // !!SPEEDUP-TODO!!
    // Using wxXmlDocument::Load as is, all the types contained in the
    // the entire gccXML file are stored in memory while parsing;
    // however we are only interested to wx's own structs/classes/funcs/etc
    // so that we could use the file IDs to avoid loading stuff which does
    // not belong to wx. See the very end of the gccXML file: it contains
    // a set of <File> nodes referenced by all nodes above.

    bool Parse(const wxString& filename);
    bool ParseMethod(const wxXmlNode *p,
                     const wxTypeIdHashMap& types,
                     wxMethod& m);
};


// ----------------------------------------------------------------------------
// Represents the interface of the doxygen headers of wxWidgets
// Loads it from the XML produced by Doxygen: http://www.doxygen.org
// ----------------------------------------------------------------------------
class wxXmlDoxygenInterface : public wxXmlInterface
{
public:
    wxXmlDoxygenInterface() {}

    // !!SPEEDUP-TODO!!
    // Using wxXmlDocument::Load as is, the entire XML file is parsed
    // and an entire tree of wxXmlNodes is built in memory.
    // We need however only small portions of the Doxygen-generated XML: to speedup the
    // processing we could detach the expat callbacks when we detect the beginning of a
    // node we're not interested about, or just don't create a wxXmlNode for it!
    // This needs a modification of wxXml API.

    bool Parse(const wxString& filename);
    bool ParseCompoundDefinition(const wxString& filename);
    bool ParseMethod(const wxXmlNode*, wxMethod&, wxString& header);
};



#endif      // _XMLPARSER_H_

