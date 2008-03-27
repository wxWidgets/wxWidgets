/////////////////////////////////////////////////////////////////////////////
// Name:        xmlparser.cpp
// Purpose:     Parser of the API/interface XML files
// Author:      Francesco Montorsi
// Created:     2008/03/17
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#endif

#include "wx/xml/xml.h"
#include "wx/wfstream.h"
#include "wx/hashmap.h"
#include "wx/filename.h"
#include "xmlparser.h"
#include <errno.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxTypeArray)
WX_DEFINE_OBJARRAY(wxArgumentTypeArray)
WX_DEFINE_OBJARRAY(wxMethodArray)
WX_DEFINE_OBJARRAY(wxClassArray)


#define PROGRESS_RATE             1000     // each PROGRESS_RATE nodes processed print a dot
#define ESTIMATED_NUM_CLASSES     600      // used by both wxXmlInterface-derived classes to prealloc mem


// defined in ifacecheck.cpp
extern bool g_verbose;



// ----------------------------------------------------------------------------
// wxType
// ----------------------------------------------------------------------------

wxType wxEmptyType;

void wxType::SetTypeFromString(const wxString& t)
{
    /*
        TODO: optimize the following code writing a single function
              which works at char-level and does everything in a single pass
    */

    m_strType = t;

    // [] is the same as * for gccxml
    m_strType.Replace("[]", "*");
    m_strType.Replace("long int", "long");      // in wx typically we never write "long int", just "long"

    // make sure the * and & operator always use the same spacing rules
    // (to make sure GetAsString() output is always consistent)
    m_strType.Replace("*", "* ");
    m_strType.Replace("&", "& ");
    m_strType.Replace(" *", "*");
    m_strType.Replace(" &", "&");

    while (m_strType.Contains("  "))
        m_strType.Replace("  ", " ");       // do it once again

    m_strType.Replace(" ,", ",");

    m_strType = m_strType.Strip(wxString::both);
}

bool wxType::IsOk() const
{
    // NB: m_strType can contain the :: operator; think to e.g. the
    //     "reverse_iterator_impl<wxString::const_iterator>" type
    //     It can also contain commas, * and & operators etc

    return !GetClean().IsEmpty();
}

wxString wxType::GetClean() const
{
    wxString ret(m_strType);
    ret.Replace("const", "");
    ret.Replace("static", "");
    ret.Replace("*", "");
    ret.Replace("&", "");
    ret.Replace("[]", "");
    return ret.Strip(wxString::both);
}

bool wxType::operator==(const wxType& m) const
{
    // brain-dead comparison:

    if (GetClean() == m.GetClean() &&
        IsConst() == m.IsConst() &&
        IsStatic() == m.IsStatic() &&
        IsPointer() == m.IsPointer() &&
        IsReference() == m.IsReference())
        return true;

    return false;
}


// ----------------------------------------------------------------------------
// wxArgumentType
// ----------------------------------------------------------------------------

void wxArgumentType::SetDefaultValue(const wxString& defval, const wxString& defvalForCmp)
{
    m_strDefaultValue=defval.Strip(wxString::both);
    m_strDefaultValueForCmp=defvalForCmp.Strip(wxString::both);

    // in order to make valid&simple comparison on argument defaults,
    // we reduce some of the multiple forms in which the same things may appear
    // to a single form:
    if (m_strDefaultValue == "0u")
        m_strDefaultValue = "0";
/*
    if (IsPointer())
        m_strDefaultValue.Replace("0", "NULL");
    else
        m_strDefaultValue.Replace("NULL", "0");
*/

    if (m_strDefaultValue.Contains("wxGetTranslation"))
        m_strDefaultValue = "_(TOFIX)";     // TODO: wxGetTranslation gives problems to gccxml
}

bool wxArgumentType::operator==(const wxArgumentType& m) const
{
    if ((const wxType&)(*this) != (const wxType&)m)
        return false;

    const wxString& def1 = m_strDefaultValueForCmp.IsEmpty() ? m_strDefaultValue : m_strDefaultValueForCmp;
    const wxString& def2 = m.m_strDefaultValueForCmp.IsEmpty() ? m.m_strDefaultValue : m.m_strDefaultValueForCmp;

    if (def1 != def2)
    {
        // maybe the default values are numbers.
        // in this case gccXML gives as default values things like '-0x0000001' instead of just '-1'.
        // To handle these cases, we try to convert the default value strings to numbers:
        long def1val, def2val;
        if (def1.ToLong(&def1val, 0 /* auto-detect */) &&
            def2.ToLong(&def2val, 0 /* auto-detect */))
        {
            if (def1val == def2val)
                return true;        // the default values match
        }

        return false;
    }

    // we deliberately avoid checks on the argument name

    return true;
}


// ----------------------------------------------------------------------------
// wxMethod
// ----------------------------------------------------------------------------

bool wxMethod::IsOk() const
{
    // NOTE: m_retType can be a wxEmptyType, and means that this method
    //       is a ctor or a dtor.
    if (!m_retType.IsOk() && m_retType!=wxEmptyType) {
        LogError("'%s' method has invalid return type: %s", m_retType.GetAsString());
        return false;
    }

    if (m_strName.IsEmpty())
        return false;

    // a function can't be both const and static or virtual and static!
    if ((m_bConst && m_bStatic) || ((m_bVirtual || m_bPureVirtual) && m_bStatic)) {
        LogError("'%s' method can't be both const/static or virtual/static", m_strName);
        return false;
    }

    wxASSERT(!m_bPureVirtual || (m_bPureVirtual && m_bVirtual));

    for (unsigned int i=0; i<m_args.GetCount(); i++)
        if (!m_args[i].IsOk()) {
            LogError("'%s' method has invalid %d-th argument type: %s",
                     m_strName, i+1, m_args[i].GetAsString());
            return false;
        }

    // NB: the default value of the arguments can contain pretty much everything
    //     (think to e.g. wxPoint(3+4/2,0)   or   *wxBLACK   or  someClass<type>)
    //     so we don't do any test on their contents
    if (m_args.GetCount()>0)
    {
        bool previousArgHasDefault = m_args[0].HasDefaultValue();
        for (unsigned int i=1; i<m_args.GetCount(); i++)
        {
            if (previousArgHasDefault && !m_args[i].HasDefaultValue()) {
                LogError("'%s' method has %d-th argument which has no default value "
                         "(while the previous one had one!)",
                         m_strName, i+1);
                return false;
            }

            previousArgHasDefault = m_args[i].HasDefaultValue();
        }
    }

    return true;
}

bool wxMethod::operator==(const wxMethod& m) const
{
    if (GetReturnType() != m.GetReturnType() ||
        GetName() != m.GetName() ||
        IsConst() != m.IsConst() ||
        IsStatic() != m.IsStatic() ||
        IsVirtual() != m.IsVirtual() ||
        IsPureVirtual() != m.IsPureVirtual() ||
        IsDeprecated() != m.IsDeprecated())
        return false;

    if (m_args.GetCount()!=m.m_args.GetCount())
        return false;

    for (unsigned int i=0; i<m_args.GetCount(); i++)
        if (m_args[i] != m.m_args[i])
            return false;

    return true;
}

wxString wxMethod::GetAsString(bool bWithArgumentNames) const
{
    wxString ret;

    if (m_retType!=wxEmptyType)
        ret += m_retType.GetAsString() + " ";
    //else; this is a ctor or dtor

    ret += m_strName + "(";

    for (unsigned int i=0; i<m_args.GetCount(); i++)
    {
        ret += m_args[i].GetAsString();

        const wxString& name = m_args[i].GetArgumentName();
        if (bWithArgumentNames && !name.IsEmpty())
            ret += " " + name;

        const wxString& def = m_args[i].GetDefaultValue();
        if (!def.IsEmpty())
            ret += " = " + def;

        ret += ", ";
    }

    if (m_args.GetCount()>0)
        ret = ret.Left(ret.Len()-2);

    ret += ")";

    if (m_bConst)
        ret += " const";
    if (m_bStatic)
        ret = "static " + ret;
    if (m_bVirtual || m_bPureVirtual)
        ret = "virtual " + ret;
    if (m_bPureVirtual)
        ret = ret + " = 0";

    // in doxygen headers we don't need wxDEPRECATED:
    //if (m_bDeprecated)
    //    ret = "wxDEPRECATED( " + ret + " )";

    return ret;
}

void wxMethod::Dump(wxTextOutputStream& stream) const
{
    stream << "[" + m_retType.GetAsString() + "]";
    stream << "[" + m_strName + "]";

    for (unsigned int i=0; i<m_args.GetCount(); i++)
        stream << "[" + m_args[i].GetAsString() + " " + m_args[i].GetArgumentName() +
                  "=" + m_args[i].GetDefaultValue() + "]";

    if (IsConst())
        stream << " CONST";
    if (IsStatic())
        stream << " STATIC";
    if (IsVirtual())
        stream << " VIRTUAL";
    if (IsPureVirtual())
        stream << " PURE-VIRTUAL";
    if (IsDeprecated())
        stream << " DEPRECATED";

    // no final newline
}

// ----------------------------------------------------------------------------
// wxClass
// ----------------------------------------------------------------------------

wxString wxClass::GetNameWithoutTemplate() const
{
    // NB: I'm not sure this is the right terminology for this function!

    if (m_strName.Contains("<"))
        return m_strName.Left(m_strName.Find("<"));
    return m_strName;
}

bool wxClass::IsValidCtorForThisClass(const wxMethod& m) const
{
    // remember that e.g. the ctor for wxWritableCharTypeBuffer<wchar_t> is
    // named wxWritableCharTypeBuffer, without the <...> part!

    if (m.IsCtor() && m.GetName() == GetNameWithoutTemplate())
        return true;

    return false;
}

bool wxClass::IsValidDtorForThisClass(const wxMethod& m) const
{
    if (m.IsDtor() && m.GetName() == "~" + GetNameWithoutTemplate())
        return true;

    return false;
}

void wxClass::Dump(wxTextOutputStream& out) const
{
    out << m_strName + "\n";

    for (unsigned int i=0; i<m_methods.GetCount(); i++) {

        // dump all our methods
        out << "|- ";
        m_methods[i].Dump(out);
        out << "\n";
    }

    out << "\n";
}

bool wxClass::CheckConsistency() const
{
    for (unsigned int i=0; i<m_methods.GetCount(); i++)
        for (unsigned int j=0; j<m_methods.GetCount(); j++)
            if (i!=j && m_methods[i] == m_methods[j])
            {
                LogError("class %s has two methods with the same prototype: '%s'",
                         m_strName, m_methods[i].GetAsString());
                return false;
            }

    return true;
}

const wxMethod* wxClass::FindMethod(const wxMethod& m) const
{
    for (unsigned int i=0; i<m_methods.GetCount(); i++)
        if (m_methods[i] == m)
            return &m_methods[i];
    return NULL;
}

wxMethodPtrArray wxClass::FindMethodNamed(const wxString& name) const
{
    wxMethodPtrArray ret;

    for (unsigned int i=0; i<m_methods.GetCount(); i++)
        if (m_methods[i].GetName() == name)
            ret.Add(&m_methods[i]);

    return ret;
}


// ----------------------------------------------------------------------------
// wxXmlInterface
// ----------------------------------------------------------------------------

WX_DEFINE_SORTED_ARRAY(wxClass*, wxSortedClassArray);

int CompareWxClassObjects(wxClass *item1, wxClass *item2)
{
    // sort alphabetically
    return item1->GetName().Cmp(item2->GetName());
}

void wxXmlInterface::Dump(const wxString& filename)
{
    wxFFileOutputStream apioutput( filename );
    wxTextOutputStream apiout( apioutput );

    // dump the classes in alphabetical order
    wxSortedClassArray sorted(CompareWxClassObjects);
    sorted.Alloc(m_classes.GetCount());
    for (unsigned int i=0; i<m_classes.GetCount(); i++)
        sorted.Add(&m_classes[i]);

    // now they have been sorted
    for (unsigned int i=0; i<sorted.GetCount(); i++)
        sorted[i]->Dump(apiout);
}

bool wxXmlInterface::CheckParseResults() const
{
    // this check can be quite slow, so do it only for debug releases:
//#ifdef __WXDEBUG__
    for (unsigned int i=0; i<m_classes.GetCount(); i++)
        if (!m_classes[i].CheckConsistency())
            return false;
//#endif

    return true;
}

wxClassPtrArray wxXmlInterface::FindClassesDefinedIn(const wxString& headerfile) const
{
    wxClassPtrArray ret;

    for (unsigned int i=0; i<m_classes.GetCount(); i++)
        if (m_classes[i].GetHeader() == headerfile)
            ret.Add(&m_classes[i]);

    return ret;
}


// ----------------------------------------------------------------------------
// wxXmlGccInterface helper declarations
// ----------------------------------------------------------------------------

#define ATTRIB_CONST        1
#define ATTRIB_REFERENCE    2
#define ATTRIB_POINTER      4
#define ATTRIB_ARRAY        8

#define GCCXML_BASE         35

class toResolveTypeItem
{
public:
    toResolveTypeItem() { attribs=0; }
    toResolveTypeItem(unsigned int refID, unsigned int attribint)
        : ref(refID), attribs(attribint) {}

    unsigned long ref, attribs;
};

#if 1

// for wxToResolveTypeHashMap, keys == gccXML IDs  and  values == toResolveTypeItem
WX_DECLARE_HASH_MAP( unsigned long, toResolveTypeItem,
                     wxIntegerHash, wxIntegerEqual,
                     wxToResolveTypeHashMap );

// for wxClassMemberIdHashMap, keys == gccXML IDs  and  values == wxClass which owns that member ID
WX_DECLARE_HASH_MAP( unsigned long, wxClass*,
                     wxIntegerHash, wxIntegerEqual,
                     wxClassMemberIdHashMap );
#else
#include <map>
typedef std::map<unsigned long, toResolveTypeItem> wxToResolveTypeHashMap;
#endif


// utility to parse gccXML ID values;
// this function is equivalent to wxString(str).Mid(1).ToULong(&id, GCCXML_BASE)
// but is a little bit faster
bool getID(unsigned long *id, const wxStringCharType* str)
{
    wxStringCharType *end;
#if wxUSE_UNICODE_UTF8
    unsigned long val = strtoul(str+1, &end, GCCXML_BASE);
#else
    unsigned long val = wcstoul(str+1, &end, GCCXML_BASE);
#endif

    // return true only if scan was stopped by the terminating NUL and
    // if the string was not empty to start with and no under/overflow
    // occurred:
    if ( *end != '\0' || end == str+1 || errno == ERANGE || errno == EINVAL )
        return false;

    *id = val;
    return true;
}

// utility specialized to parse efficiently the gccXML list of IDs which occur
// in nodes like <Class> ones... i.e. numeric values separed by " _" token
bool getMemberIDs(wxClassMemberIdHashMap* map, wxClass* p, const wxStringCharType* str)
{
#if wxUSE_UNICODE_UTF8
    size_t len = strlen(str);
#else
    size_t len = wcslen(str);
#endif

    if (len == 0 || str[0] != '_')
        return false;

    const wxStringCharType *curpos = str,
                           *end = str + len;
    wxStringCharType *nexttoken;

    while (curpos < end)
    {
        // curpos always points to the underscore of the next token to parse:
#if wxUSE_UNICODE_UTF8
        unsigned long id = strtoul(curpos+1, &nexttoken, GCCXML_BASE);
#else
        unsigned long id = wcstoul(curpos+1, &nexttoken, GCCXML_BASE);
#endif
        if ( *nexttoken != ' ' || errno == ERANGE || errno == EINVAL )
            return false;

        // advance current position
        curpos = nexttoken + 1;

        // add this ID to the hashmap
        wxClassMemberIdHashMap::value_type v(id, p);
        map->insert(v);
    }

    return true;
}


// ----------------------------------------------------------------------------
// wxXmlGccInterface
// ----------------------------------------------------------------------------

bool wxXmlGccInterface::Parse(const wxString& filename)
{
    wxXmlDocument doc;
    wxXmlNode *child;
    int nodes = 0;

    LogMessage("Parsing %s...", filename);

    if (!doc.Load(filename)) {
        LogError("can't load %s", filename);
        return false;
    }

    // start processing the XML file
    if (doc.GetRoot()->GetName() != "GCC_XML") {
        LogError("invalid root node for %s", filename);
        return false;
    }

    wxToResolveTypeHashMap toResolveTypes;
    wxClassMemberIdHashMap members;
    wxTypeIdHashMap types;
    wxTypeIdHashMap files;

    // prealloc quite a lot of memory!
    m_classes.Alloc(ESTIMATED_NUM_CLASSES);

    // build a list of wx classes and in general of all existent types
    child = doc.GetRoot()->GetChildren();
    while (child)
    {
        const wxString& n = child->GetName();

        unsigned long id = 0;
        if (!getID(&id, child->GetAttribute("id")) || (id == 0 && n != "File")) {

            // NOTE: <File> nodes can have an id == "f0"...

            LogError("Invalid id for node %s: %s", n, child->GetAttribute("id"));
            return false;
        }

        if (n == "Class")
        {
            wxString cname = child->GetAttribute("name");
            if (cname.IsEmpty()) {
                LogError("Invalid empty name for '%s' node", n);
                return false;
            }

            // only register wx classes (do remember also the IDs of their members)
            if (cname.StartsWith("wx"))
            {
                // NB: "file" attribute contains an ID value that we'll resolve later
                m_classes.Add(wxClass(cname, child->GetAttribute("file")));

                const wxString& ids = child->GetAttribute("members");
                if (ids.IsEmpty())
                {
                    if (child->GetAttribute("incomplete") != "1") {
                        LogError("Invalid member IDs for '%s' class node: %s",
                                cname, child->GetAttribute("id"));
                        return false;
                    }
                    //else: don't warn the user; it looks like "incomplete" classes
                    //      never have any member...
                }
                else
                {
                    // decode the non-empty list of IDs:
                    if (!getMemberIDs(&members, &m_classes.Last(), ids)) {
                        LogError("Invalid member IDs for '%s' class node: %s",
                                cname, child->GetAttribute("id"));
                        return false;
                    }
                }
            }

            // register this class also as possible return/argument type:
            types[id] = cname;
        }
        else if (n == "PointerType" || n == "ReferenceType" ||
                 n == "CvQualifiedType" || n == "ArrayType")
        {
            unsigned long type = 0;
            if (!getID(&type, child->GetAttribute("type")) || type == 0) {
                LogError("Invalid type for node %s: %s", n, child->GetAttribute("type"));
                return false;
            }

            unsigned long attr = 0;
            if (n == "PointerType")
                attr = ATTRIB_POINTER;
            else if (n == "ReferenceType")
                attr = ATTRIB_REFERENCE;
            else if (n == "CvQualifiedType" && child->GetAttribute("const") == "1")
                attr = ATTRIB_CONST;
            else if (n == "ArrayType")
                attr = ATTRIB_ARRAY;

            // these nodes make reference to other types... we'll resolve them later
            toResolveTypes[id] = toResolveTypeItem(type, attr);
        }
        else if (n == "FunctionType" || n == "MethodType")
        {
            /*
                 TODO: parsing FunctionType and MethodType nodes is not as easy
                       as for other "simple" types.
            */

            wxString argstr;
            wxXmlNode *arg = child->GetChildren();
            while (arg)
            {
                if (arg->GetName() == "Argument")
                    argstr += arg->GetAttribute("type") + ", ";
                arg = arg->GetNext();
            }

            if (argstr.Len() > 0)
                argstr = argstr.Left(argstr.Len()-2);

            // these nodes make reference to other types... we'll resolve them later
            //toResolveTypes[id] = toResolveTypeItem(ret, 0);
            types[id] = child->GetAttribute("returns") + "(" + argstr + ")";
        }
        else if (n == "File")
        {
            if (!child->GetAttribute("id").StartsWith("f")) {
                LogError("Unexpected file ID: %s", child->GetAttribute("id"));
                return false;
            }

            // just ignore this node... all file IDs/names were already parsed
            files[id] = child->GetAttribute("name");
        }
        else
        {
            // we register everything else as a possible return/argument type:
            const wxString& name = child->GetAttribute("name");

            if (!name.IsEmpty())
            {
                //typeIds.Add(id);
                //typeNames.Add(name);
                types[id] = name;
            }
            else
            {
                // this may happen with unnamed structs/union, special ctors,
                // or other exotic things which we are not interested to, since
                // they're never used as return/argument types by wxWidgets methods

                if (g_verbose)
                    LogWarning("Type node '%s' with ID '%s' does not have name attribute",
                               n, child->GetAttribute("id"));

                types[id] = "TOFIX";
            }
        }

        child = child->GetNext();

        // give feedback to the user about the progress...
        if ((++nodes%PROGRESS_RATE)==0) ShowProgress();
    }

    // some nodes with IDs referenced by methods as return/argument types, do reference
    // in turn o ther nodes (see PointerType, ReferenceType and CvQualifierType above);
    // thus we need to resolve their name iteratively:
    while (toResolveTypes.size()>0)
    {
        if (g_verbose)
            LogMessage("%d types were collected; %d types need yet to be resolved...",
                       types.size(), toResolveTypes.size());

        for (wxToResolveTypeHashMap::iterator i = toResolveTypes.begin();
             i != toResolveTypes.end();)
        {
            unsigned long id = i->first;
            unsigned long referenced = i->second.ref;

            wxTypeIdHashMap::iterator primary = types.find(referenced);
            if (primary != types.end())
            {
                // this to-resolve-type references a "primary" type

                wxString newtype = primary->second;
                int attribs = i->second.attribs;

                // attribs may contain a combination of ATTRIB_* flags:
                if (attribs & ATTRIB_CONST)
                    newtype = "const " + newtype;
                if (attribs & ATTRIB_REFERENCE)
                    newtype = newtype + "&";
                if (attribs & ATTRIB_POINTER)
                    newtype = newtype + "*";
                if (attribs & ATTRIB_ARRAY)
                    newtype = newtype + "[]";

                // add the resolved type to the list of "primary" types
                types[id] = newtype;

                // this one has been resolved; erase it through its iterator!
                toResolveTypes.erase(i);

                // now iterator i is invalid; assign it again to the beginning
                i = toResolveTypes.begin();
            }
            else
            {
                // then search in the referenced types themselves:
                wxToResolveTypeHashMap::iterator idx2 = toResolveTypes.find(referenced);
                if (idx2 != toResolveTypes.end())
                {
                    // merge this to-resolve-type with the idx2->second type
                    i->second.ref = idx2->second.ref;
                    i->second.attribs |= idx2->second.attribs;

                    // this type will eventually be solved in the next while() iteration
                    i++;
                }
                else
                {
                    LogError("Cannot solve '%s' reference type!", referenced);
                    return false;
                }
            }
        }
    }

    // resolve header names
    for (unsigned int i=0; i<m_classes.GetCount(); i++)
    {
        unsigned long fileID = 0;
        if (!getID(&fileID, m_classes[i].GetHeader()) || fileID == 0) {
            LogError("invalid header id: %s", m_classes[i].GetHeader());
            return false;
        }

        // search this file
        wxTypeIdHashMap::const_iterator idx = files.find(fileID);
        if (idx == files.end())
        {
            // this is an error!
            LogError("couldn't find file ID '%s'", m_classes[i].GetHeader());
        }
        else
            m_classes[i].SetHeader(idx->second);
    }

    // build the list of the wx methods
    child = doc.GetRoot()->GetChildren();
    while (child)
    {
        wxString n = child->GetName();

        // only register public methods
        if (child->GetAttribute("access") == "public" &&
            (n == "Method" || n == "Constructor" || n == "Destructor" || n == "OperatorMethod"))
        {
            unsigned long id = 0;
            if (!getID(&id, child->GetAttribute("id"))) {
                LogError("invalid ID for node '%s' with ID '%s'", n, child->GetAttribute("id"));
                return false;
            }

            wxClassMemberIdHashMap::const_iterator it = members.find(id);
            if (it != members.end())
            {
                wxClass *p = it->second;

                // this <Method> node is a method of the i-th class!
                wxMethod newfunc;
                if (!ParseMethod(child, types, newfunc)) {
                    LogError("The method '%s' could not be added to class '%s'",
                             child->GetAttribute("demangled"), p->GetName());
                    return false;
                }

                if (newfunc.IsCtor() && !p->IsValidCtorForThisClass(newfunc)) {
                    LogError("The method '%s' does not seem to be a ctor for '%s'",
                             newfunc.GetName(), p->GetName());
                    return false;
                }
                if (newfunc.IsDtor() && !p->IsValidDtorForThisClass(newfunc)) {
                    LogError("The method '%s' does not seem to be a dtor for '%s'",
                             newfunc.GetName(), p->GetName());
                    return false;
                }

                p->AddMethod(newfunc);
            }
        }

        child = child->GetNext();

        // give feedback to the user about the progress...
        if ((++nodes%PROGRESS_RATE)==0) ShowProgress();
    }

    if (!CheckParseResults())
        return false;

    return true;
}

bool wxXmlGccInterface::ParseMethod(const wxXmlNode *p,
                                    const wxTypeIdHashMap& types,
                                    wxMethod& m)
{
    // get the real name
    wxString name = p->GetAttribute("name").Strip(wxString::both);
    if (p->GetName() == "Destructor")
        name = "~" + name;
    else if (p->GetName() == "OperatorMethod")
        name = "operator" + name;

    // resolve return type
    wxType ret;
    unsigned long retid = 0;
    if (!getID(&retid, p->GetAttribute("returns")) || retid == 0)
    {
        if (p->GetName() != "Destructor" && p->GetName() != "Constructor") {
            LogError("Empty return ID for method '%s', with ID '%s'",
                     name, p->GetAttribute("id"));
            return false;
        }
    }
    else
    {
        wxTypeIdHashMap::const_iterator retidx = types.find(retid);
        if (retidx == types.end()) {
            LogError("Could not find return type ID '%s'", retid);
            return false;
        }

        ret = wxType(retidx->second);
        if (!ret.IsOk()) {
            LogError("Invalid return type '%s' for method '%s', with ID '%s'",
                     retidx->second, name, p->GetAttribute("id"));
            return false;
        }
    }

    // resolve argument types
    wxArgumentTypeArray argtypes;
    wxXmlNode *arg = p->GetChildren();
    while (arg)
    {
        if (arg->GetName() == "Argument")
        {
            unsigned long id = 0;
            if (!getID(&id, arg->GetAttribute("type")) || id == 0) {
                LogError("Invalid argument type ID '%s' for method '%s' with ID %s",
                         arg->GetAttribute("type"), name, p->GetAttribute("id"));
                return false;
            }

            wxTypeIdHashMap::const_iterator idx = types.find(id);
            if (idx == types.end()) {
                LogError("Could not find argument type ID '%s'", id);
                return false;
            }

            argtypes.Add(wxArgumentType(idx->second, arg->GetAttribute("default")));
        }

        arg = arg->GetNext();
    }

    m.SetReturnType(ret);
    m.SetName(name);
    m.SetArgumentTypes(argtypes);
    m.SetConst(p->GetAttribute("const") == "1");
    m.SetStatic(p->GetAttribute("static") == "1");

    // NOTE: gccxml is smart enough to mark as virtual those functions
    //       which are declared virtual in base classes but don't have
    //       the "virtual" keyword explicitely indicated in the derived
    //       classes... so we don't need any further logic for virtuals

    m.SetVirtual(p->GetAttribute("virtual") == "1");
    m.SetPureVirtual(p->GetAttribute("pure_virtual") == "1");
    m.SetDeprecated(p->GetAttribute("attributes") == "deprecated");

    if (!m.IsOk()) {
        LogError("The prototype '%s' is not valid!", m.GetAsString());
        return false;
    }

    return true;
}



// ----------------------------------------------------------------------------
// wxXmlDoxygenInterface global helpers
// ----------------------------------------------------------------------------

static wxString GetTextFromChildren(const wxXmlNode *n)
{
    wxString text;

    // consider the tree
    //
    //  <a><b>this</b> is a <b>string</b></a>
    //
    // <a>
    // |- <b>
    // |   |- this
    // |- is a
    // |- <b>
    //     |- string
    //
    // unlike wxXmlNode::GetNodeContent() which would return " is a "
    // this function returns "this is a string"

    wxXmlNode *ref = n->GetChildren();
    while (ref) {
        if (ref->GetType() == wxXML_ELEMENT_NODE)
            text += ref->GetNodeContent();
        else if (ref->GetType() == wxXML_TEXT_NODE)
            text += ref->GetContent();
        else
            LogWarning("Unexpected node type while getting text from '%s' node", n->GetName());

        ref = ref->GetNext();
    }

    return text;
}

static bool HasTextNodeContaining(const wxXmlNode *parent, const wxString& name)
{
    if (!parent)
        return false;

    wxXmlNode *p = parent->GetChildren();
    while (p)
    {
        switch (p->GetType())
        {
            case wxXML_TEXT_NODE:
                if (p->GetContent() == name)
                    return true;
                break;

            case wxXML_ELEMENT_NODE:
                // recurse into this node...
                if (HasTextNodeContaining(p, name))
                    return true;
                break;

            default:
                // skip it
                break;
        }

        p = p->GetNext();
    }

    return false;
}

static const wxXmlNode* FindNodeNamed(const wxXmlNode* parent, const wxString& name)
{
    if (!parent)
        return NULL;

    const wxXmlNode *p = parent->GetChildren();
    while (p)
    {
        if (p->GetName() == name)
            return p;       // found!

        // search recursively in the children of this node
        const wxXmlNode *ret = FindNodeNamed(p, name);
        if (ret)
            return ret;

        p = p->GetNext();
    }

    return NULL;
}

int GetAvailabilityFor(const wxXmlNode *node)
{
    // identify <onlyfor> custom XML tags
    const wxXmlNode* onlyfor = FindNodeNamed(node, "onlyfor");
    if (!onlyfor)
        return wxPORT_UNKNOWN;

    wxArrayString ports = wxSplit(onlyfor->GetNodeContent(), ',');
    int nAvail = wxPORT_UNKNOWN;
    for (unsigned int i=0; i < ports.GetCount(); i++)
    {
        if (!ports[i].StartsWith("wx")) {
            LogError("unexpected port ID '%s'", ports[i]);
            return false;
        }

        nAvail |= wxPlatformInfo::GetPortId(ports[i].Mid(2));
    }

    return nAvail;
}


// ----------------------------------------------------------------------------
// wxXmlDoxygenInterface
// ----------------------------------------------------------------------------

bool wxXmlDoxygenInterface::Parse(const wxString& filename)
{
    wxXmlDocument index;
    wxXmlNode *compound;

    LogMessage("Parsing %s...", filename);

    if (!index.Load(filename)) {
        LogError("can't load %s", filename);
        return false;
    }

    // start processing the index:
    if (index.GetRoot()->GetName() != "doxygenindex") {
        LogError("invalid root node for %s", filename);
        return false;
    }

    m_classes.Alloc(ESTIMATED_NUM_CLASSES);

    // process files referenced by this index file
    compound = index.GetRoot()->GetChildren();
    while (compound)
    {
        if (compound->GetName() == "compound" &&
            compound->GetAttribute("kind") == "class")
        {
            wxString refid = compound->GetAttribute("refid");

            wxFileName fn(filename);
            if (!ParseCompoundDefinition(fn.GetPath(wxPATH_GET_SEPARATOR) + refid + ".xml"))
                return false;
        }

        compound = compound->GetNext();
    }
    //wxPrint("\n");

    if (!CheckParseResults())
        return false;

    return true;
}

bool wxXmlDoxygenInterface::ParseCompoundDefinition(const wxString& filename)
{
    wxXmlDocument doc;
    wxXmlNode *child;
    int nodes = 0;

    if (g_verbose)
        LogMessage("Parsing %s...", filename);

    if (!doc.Load(filename)) {
        LogError("can't load %s", filename);
        return false;
    }

    // start processing this compound definition XML
    if (doc.GetRoot()->GetName() != "doxygen") {
        LogError("invalid root node for %s", filename);
        return false;
    }

    // build a list of wx classes
    child = doc.GetRoot()->GetChildren();
    while (child)
    {
        if (child->GetName() == "compounddef" &&
            child->GetAttribute("kind") == "class")
        {
            // parse this class
            wxClass klass;
            wxString absoluteFile, header;

            wxXmlNode *subchild = child->GetChildren();
            while (subchild)
            {
                if (subchild->GetName() == "sectiondef" &&
                    subchild->GetAttribute("kind") == "public-func")
                {

                    wxXmlNode *membernode = subchild->GetChildren();
                    while (membernode)
                    {
                        if (membernode->GetName() == "memberdef" &&
                            membernode->GetAttribute("kind") == "function")
                        {

                            wxMethod m;
                            if (!ParseMethod(membernode, m, header)) {
                                LogError("The method '%s' could not be added to class '%s'",
                                         m.GetName(), klass.GetName());
                                return false;
                            }

                            if (absoluteFile.IsEmpty())
                                absoluteFile = header;
                            else if (header != absoluteFile)
                            {
                                LogError("The method '%s' is documented in a different "
                                            "file from others (which belong to '%s') ?",
                                            header, absoluteFile);
                                return false;
                            }

                            klass.AddMethod(m);
                        }

                        membernode = membernode->GetNext();
                    }

                    // all methods of this class were taken from the header "absoluteFile":
                    klass.SetHeader(absoluteFile);
                }
                else if (subchild->GetName() == "compoundname")
                {
                    klass.SetName(subchild->GetNodeContent());
                }
                /*else if (subchild->GetName() == "includes")
                {
                    // NOTE: we'll get the header from the <location> tags
                    //       scattered inside <memberdef> tags instead of
                    //       this <includes> tag since it does not contain
                    //       the absolute path of the header

                    klass.SetHeader(subchild->GetNodeContent());
                }*/
                else if (subchild->GetName() == "detaileddescription")
                {
                    // identify <onlyfor> custom XML tags
                    klass.SetAvailability(GetAvailabilityFor(subchild));
                }

                subchild = subchild->GetNext();
            }

            // add a new class
            if (klass.IsOk())
                m_classes.Add(klass);
            else if (g_verbose)
                LogWarning("discarding class '%s' with %d methods...",
                        klass.GetName(), klass.GetMethodCount());
        }

        child = child->GetNext();

        // give feedback to the user about the progress...
        if ((++nodes%PROGRESS_RATE)==0) ShowProgress();
    }

    return true;
}

bool wxXmlDoxygenInterface::ParseMethod(const wxXmlNode* p, wxMethod& m, wxString& header)
{
    wxArgumentTypeArray args;
    long line;

    wxXmlNode *child = p->GetChildren();
    while (child)
    {
        if (child->GetName() == "name")
            m.SetName(child->GetNodeContent());
        else if (child->GetName() == "type")
            m.SetReturnType(wxType(GetTextFromChildren(child)));
        else if (child->GetName() == "param")
        {
            wxString typestr, namestr, defstr, arrstr;
            wxXmlNode *n = child->GetChildren();
            while (n)
            {
                if (n->GetName() == "type")
                    // if the <type> node has children, they should be all TEXT and <ref> nodes
                    // and we need to take the text they contain, in the order they appear
                    typestr = GetTextFromChildren(n);
                else if (n->GetName() == "declname")
                    namestr = GetTextFromChildren(n);
                else if (n->GetName() == "defval")
                    defstr = GetTextFromChildren(n).Strip(wxString::both);
                else if (n->GetName() == "array")
                    arrstr = GetTextFromChildren(n);

                n = n->GetNext();
            }

            if (typestr.IsEmpty()) {
                LogError("cannot find type node for a param in method '%s'", m.GetName());
                return false;
            }

            wxArgumentType newarg(typestr + arrstr, defstr, namestr);

            // can we use preprocessor output to transform the default value
            // into the same form which gets processed by wxXmlGccInterface?
            wxStringHashMap::const_iterator it = m_preproc.find(defstr);
            if (it != m_preproc.end())
                newarg.SetDefaultValue(defstr, it->second);

            args.Add(newarg);
        }
        else if (child->GetName() == "location")
        {
            line = -1;
            if (child->GetAttribute("line").ToLong(&line))
                m.SetLocation((int)line);
            header = child->GetAttribute("file");
        }
        else if (child->GetName() == "detaileddescription")
        {
            // when a method has a @deprecated tag inside its description,
            // Doxygen outputs somewhere nested inside <detaileddescription>
            // a <xreftitle>Deprecated</xreftitle> tag.
            m.SetDeprecated(HasTextNodeContaining(child, "Deprecated"));

            // identify <onlyfor> custom XML tags
            m.SetAvailability(GetAvailabilityFor(child));
        }

        child = child->GetNext();
    }

    m.SetArgumentTypes(args);
    m.SetConst(p->GetAttribute("const")=="yes");
    m.SetStatic(p->GetAttribute("static")=="yes");

    // NOTE: Doxygen is smart enough to mark as virtual those functions
    //       which are declared virtual in base classes but don't have
    //       the "virtual" keyword explicitely indicated in the derived
    //       classes... so we don't need any further logic for virtuals

    m.SetVirtual(p->GetAttribute("virt")=="virtual");
    m.SetPureVirtual(p->GetAttribute("virt")=="pure-virtual");

    if (!m.IsOk()) {
        LogError("The prototype '%s' is not valid!", m.GetAsString());
        return false;
    }

    return true;
}
