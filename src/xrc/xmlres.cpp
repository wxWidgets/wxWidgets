/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xmlres.cpp
// Purpose:     XRC resources
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC

#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/panel.h"
    #include "wx/frame.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/bitmap.h"
    #include "wx/image.h"
    #include "wx/module.h"
    #include "wx/wxcrtvararg.h"
#endif

#ifndef __WXWINCE__
    #include <locale.h>
#endif

#include "wx/vector.h"
#include "wx/wfstream.h"
#include "wx/filesys.h"
#include "wx/filename.h"
#include "wx/tokenzr.h"
#include "wx/fontenum.h"
#include "wx/fontmap.h"
#include "wx/artprov.h"

#include "wx/xml/xml.h"


class wxXmlResourceDataRecord
{
public:
    wxXmlResourceDataRecord() : Doc(NULL) {
#if wxUSE_DATETIME
        Time = wxDateTime::Now();
#endif
    }
    ~wxXmlResourceDataRecord() {delete Doc;}

    wxString File;
    wxXmlDocument *Doc;
#if wxUSE_DATETIME
    wxDateTime Time;
#endif
};

class wxXmlResourceDataRecords : public wxVector<wxXmlResourceDataRecord*>
{
    // this is a class so that it can be forward-declared
};

namespace
{

// helper used by DoFindResource() and elsewhere: returns true if this is an
// object or object_ref node
//
// node must be non-NULL
inline bool IsObjectNode(wxXmlNode *node)
{
    return node->GetType() == wxXML_ELEMENT_NODE &&
             (node->GetName() == wxS("object") ||
                node->GetName() == wxS("object_ref"));
}

} // anonymous namespace


wxXmlResource *wxXmlResource::ms_instance = NULL;

/*static*/ wxXmlResource *wxXmlResource::Get()
{
    if ( !ms_instance )
        ms_instance = new wxXmlResource;
    return ms_instance;
}

/*static*/ wxXmlResource *wxXmlResource::Set(wxXmlResource *res)
{
    wxXmlResource *old = ms_instance;
    ms_instance = res;
    return old;
}

wxXmlResource::wxXmlResource(int flags, const wxString& domain)
{
    m_flags = flags;
    m_version = -1;
    m_data = new wxXmlResourceDataRecords;
    SetDomain(domain);
}

wxXmlResource::wxXmlResource(const wxString& filemask, int flags, const wxString& domain)
{
    m_flags = flags;
    m_version = -1;
    m_data = new wxXmlResourceDataRecords;
    SetDomain(domain);
    Load(filemask);
}

wxXmlResource::~wxXmlResource()
{
    ClearHandlers();

    for ( wxXmlResourceDataRecords::iterator i = m_data->begin();
          i != m_data->end(); ++i )
    {
        delete *i;
    }
    delete m_data;
}

void wxXmlResource::SetDomain(const wxString& domain)
{
    m_domain = domain;
}


/* static */
wxString wxXmlResource::ConvertFileNameToURL(const wxString& filename)
{
    wxString fnd(filename);

    // NB: as Load() and Unload() accept both filenames and URLs (should
    //     probably be changed to filenames only, but embedded resources
    //     currently rely on its ability to handle URLs - FIXME) we need to
    //     determine whether found name is filename and not URL and this is the
    //     fastest/simplest way to do it
    if (wxFileName::FileExists(fnd))
    {
        // Make the name absolute filename, because the app may
        // change working directory later:
        wxFileName fn(fnd);
        if (fn.IsRelative())
        {
            fn.MakeAbsolute();
            fnd = fn.GetFullPath();
        }
#if wxUSE_FILESYSTEM
        fnd = wxFileSystem::FileNameToURL(fnd);
#endif
    }

    return fnd;
}

#if wxUSE_FILESYSTEM

/* static */
bool wxXmlResource::IsArchive(const wxString& filename)
{
    const wxString fnd = filename.Lower();

    return fnd.Matches(wxT("*.zip")) || fnd.Matches(wxT("*.xrs"));
}

#endif // wxUSE_FILESYSTEM

bool wxXmlResource::Load(const wxString& filemask_)
{
    wxString filemask = ConvertFileNameToURL(filemask_);

#if wxUSE_FILESYSTEM
    wxFileSystem fsys;
#   define wxXmlFindFirst  fsys.FindFirst(filemask, wxFILE)
#   define wxXmlFindNext   fsys.FindNext()
#else
#   define wxXmlFindFirst  wxFindFirstFile(filemask, wxFILE)
#   define wxXmlFindNext   wxFindNextFile()
#endif
    wxString fnd = wxXmlFindFirst;
    if ( fnd.empty() )
    {
        wxLogError(_("Cannot load resources from '%s'."), filemask);
        return false;
    }

    while (!fnd.empty())
    {
#if wxUSE_FILESYSTEM
        if ( IsArchive(fnd) )
        {
            if ( !Load(fnd + wxT("#zip:*.xrc")) )
                return false;
        }
        else // a single resource URL
#endif // wxUSE_FILESYSTEM
        {
            wxXmlResourceDataRecord *drec = new wxXmlResourceDataRecord;
            drec->File = fnd;
            Data().push_back(drec);
        }

        fnd = wxXmlFindNext;
    }
#   undef wxXmlFindFirst
#   undef wxXmlFindNext

    return UpdateResources();
}

bool wxXmlResource::Unload(const wxString& filename)
{
    wxASSERT_MSG( !wxIsWild(filename),
                    _T("wildcards not supported by wxXmlResource::Unload()") );

    wxString fnd = ConvertFileNameToURL(filename);
#if wxUSE_FILESYSTEM
    const bool isArchive = IsArchive(fnd);
    if ( isArchive )
        fnd += _T("#zip:");
#endif // wxUSE_FILESYSTEM

    bool unloaded = false;
    for ( wxXmlResourceDataRecords::iterator i = Data().begin();
          i != Data().end(); ++i )
    {
#if wxUSE_FILESYSTEM
        if ( isArchive )
        {
            if ( (*i)->File.StartsWith(fnd) )
                unloaded = true;
            // don't break from the loop, we can have other matching files
        }
        else // a single resource URL
#endif // wxUSE_FILESYSTEM
        {
            if ( (*i)->File == fnd )
            {
                delete *i;
                Data().erase(i);
                unloaded = true;

                // no sense in continuing, there is only one file with this URL
                break;
            }
        }
    }

    return unloaded;
}


IMPLEMENT_ABSTRACT_CLASS(wxXmlResourceHandler, wxObject)

void wxXmlResource::AddHandler(wxXmlResourceHandler *handler)
{
    m_handlers.push_back(handler);
    handler->SetParentResource(this);
}

void wxXmlResource::InsertHandler(wxXmlResourceHandler *handler)
{
    m_handlers.insert(m_handlers.begin(), handler);
    handler->SetParentResource(this);
}



void wxXmlResource::ClearHandlers()
{
    for ( wxVector<wxXmlResourceHandler*>::iterator i = m_handlers.begin();
          i != m_handlers.end(); ++i )
        delete *i;
    m_handlers.clear();
}


wxMenu *wxXmlResource::LoadMenu(const wxString& name)
{
    return (wxMenu*)CreateResFromNode(FindResource(name, wxT("wxMenu")), NULL, NULL);
}



wxMenuBar *wxXmlResource::LoadMenuBar(wxWindow *parent, const wxString& name)
{
    return (wxMenuBar*)CreateResFromNode(FindResource(name, wxT("wxMenuBar")), parent, NULL);
}



#if wxUSE_TOOLBAR
wxToolBar *wxXmlResource::LoadToolBar(wxWindow *parent, const wxString& name)
{
    return (wxToolBar*)CreateResFromNode(FindResource(name, wxT("wxToolBar")), parent, NULL);
}
#endif


wxDialog *wxXmlResource::LoadDialog(wxWindow *parent, const wxString& name)
{
    return (wxDialog*)CreateResFromNode(FindResource(name, wxT("wxDialog")), parent, NULL);
}

bool wxXmlResource::LoadDialog(wxDialog *dlg, wxWindow *parent, const wxString& name)
{
    return CreateResFromNode(FindResource(name, wxT("wxDialog")), parent, dlg) != NULL;
}



wxPanel *wxXmlResource::LoadPanel(wxWindow *parent, const wxString& name)
{
    return (wxPanel*)CreateResFromNode(FindResource(name, wxT("wxPanel")), parent, NULL);
}

bool wxXmlResource::LoadPanel(wxPanel *panel, wxWindow *parent, const wxString& name)
{
    return CreateResFromNode(FindResource(name, wxT("wxPanel")), parent, panel) != NULL;
}

wxFrame *wxXmlResource::LoadFrame(wxWindow* parent, const wxString& name)
{
    return (wxFrame*)CreateResFromNode(FindResource(name, wxT("wxFrame")), parent, NULL);
}

bool wxXmlResource::LoadFrame(wxFrame* frame, wxWindow *parent, const wxString& name)
{
    return CreateResFromNode(FindResource(name, wxT("wxFrame")), parent, frame) != NULL;
}

wxBitmap wxXmlResource::LoadBitmap(const wxString& name)
{
    wxBitmap *bmp = (wxBitmap*)CreateResFromNode(
                               FindResource(name, wxT("wxBitmap")), NULL, NULL);
    wxBitmap rt;

    if (bmp) { rt = *bmp; delete bmp; }
    return rt;
}

wxIcon wxXmlResource::LoadIcon(const wxString& name)
{
    wxIcon *icon = (wxIcon*)CreateResFromNode(
                            FindResource(name, wxT("wxIcon")), NULL, NULL);
    wxIcon rt;

    if (icon) { rt = *icon; delete icon; }
    return rt;
}


wxObject *wxXmlResource::LoadObject(wxWindow *parent, const wxString& name, const wxString& classname)
{
    return CreateResFromNode(FindResource(name, classname), parent, NULL);
}

bool wxXmlResource::LoadObject(wxObject *instance, wxWindow *parent, const wxString& name, const wxString& classname)
{
    return CreateResFromNode(FindResource(name, classname), parent, instance) != NULL;
}


bool wxXmlResource::AttachUnknownControl(const wxString& name,
                                         wxWindow *control, wxWindow *parent)
{
    if (parent == NULL)
        parent = control->GetParent();
    wxWindow *container = parent->FindWindow(name + wxT("_container"));
    if (!container)
    {
        wxLogError(_("Cannot find container for unknown control '%s'."), name.c_str());
        return false;
    }
    return control->Reparent(container);
}


static void ProcessPlatformProperty(wxXmlNode *node)
{
    wxString s;
    bool isok;

    wxXmlNode *c = node->GetChildren();
    while (c)
    {
        isok = false;
        if (!c->GetAttribute(wxT("platform"), &s))
            isok = true;
        else
        {
            wxStringTokenizer tkn(s, wxT(" |"));

            while (tkn.HasMoreTokens())
            {
                s = tkn.GetNextToken();
#ifdef __WINDOWS__
                if (s == wxT("win")) isok = true;
#endif
#if defined(__MAC__) || defined(__APPLE__)
                if (s == wxT("mac")) isok = true;
#elif defined(__UNIX__)
                if (s == wxT("unix")) isok = true;
#endif
#ifdef __OS2__
                if (s == wxT("os2")) isok = true;
#endif

                if (isok)
                    break;
            }
        }

        if (isok)
        {
            ProcessPlatformProperty(c);
            c = c->GetNext();
        }
        else
        {
            wxXmlNode *c2 = c->GetNext();
            node->RemoveChild(c);
            delete c;
            c = c2;
        }
    }
}



bool wxXmlResource::UpdateResources()
{
    bool rt = true;
    bool modif;
#   if wxUSE_FILESYSTEM
    wxFSFile *file = NULL;
    wxUnusedVar(file);
    wxFileSystem fsys;
#   endif

    wxString encoding(wxT("UTF-8"));
#if !wxUSE_UNICODE && wxUSE_INTL
    if ( (GetFlags() & wxXRC_USE_LOCALE) == 0 )
    {
        // In case we are not using wxLocale to translate strings, convert the
        // strings GUI's charset. This must not be done when wxXRC_USE_LOCALE
        // is on, because it could break wxGetTranslation lookup.
        encoding = wxLocale::GetSystemEncodingName();
    }
#endif

    for ( wxXmlResourceDataRecords::iterator i = Data().begin();
          i != Data().end(); ++i )
    {
        wxXmlResourceDataRecord* const rec = *i;

        modif = (rec->Doc == NULL);

        if (!modif && !(m_flags & wxXRC_NO_RELOADING))
        {
#           if wxUSE_FILESYSTEM
            file = fsys.OpenFile(rec->File);
#           if wxUSE_DATETIME
            modif = file && file->GetModificationTime() > rec->Time;
#           else // wxUSE_DATETIME
            modif = true;
#           endif // wxUSE_DATETIME
            if (!file)
            {
                wxLogError(_("Cannot open file '%s'."), rec->File);
                rt = false;
            }
            wxDELETE(file);
            wxUnusedVar(file);
#           else // wxUSE_FILESYSTEM
#           if wxUSE_DATETIME
            modif = wxDateTime(wxFileModificationTime(rec->File)) > rec->Time;
#           else // wxUSE_DATETIME
            modif = true;
#           endif // wxUSE_DATETIME
#           endif // wxUSE_FILESYSTEM
        }

        if (modif)
        {
            wxLogTrace(_T("xrc"), _T("opening file '%s'"), rec->File);

            wxInputStream *stream = NULL;

#           if wxUSE_FILESYSTEM
            file = fsys.OpenFile(rec->File);
            if (file)
                stream = file->GetStream();
#           else
            stream = new wxFileInputStream(rec->File);
#           endif

            if (stream)
            {
                delete rec->Doc;
                rec->Doc = new wxXmlDocument;
            }
            if (!stream || !stream->IsOk() || !rec->Doc->Load(*stream, encoding))
            {
                wxLogError(_("Cannot load resources from file '%s'."),
                           rec->File);
                wxDELETE(rec->Doc);
                rt = false;
            }
            else if (rec->Doc->GetRoot()->GetName() != wxT("resource"))
            {
                wxLogError(_("Invalid XRC resource '%s': doesn't have root node 'resource'."), rec->File);
                wxDELETE(rec->Doc);
                rt = false;
            }
            else
            {
                long version;
                int v1, v2, v3, v4;
                wxString verstr = rec->Doc->GetRoot()->GetAttribute(
                                      wxT("version"), wxT("0.0.0.0"));
                if (wxSscanf(verstr.c_str(), wxT("%i.%i.%i.%i"),
                    &v1, &v2, &v3, &v4) == 4)
                    version = v1*256*256*256+v2*256*256+v3*256+v4;
                else
                    version = 0;
                if (m_version == -1)
                    m_version = version;
                if (m_version != version)
                {
                    wxLogError(_("Resource files must have same version number!"));
                    rt = false;
                }

                ProcessPlatformProperty(rec->Doc->GetRoot());
#if wxUSE_DATETIME
#if wxUSE_FILESYSTEM
                rec->Time = file->GetModificationTime();
#else // wxUSE_FILESYSTEM
                rec->Time = wxDateTime(wxFileModificationTime(rec->File));
#endif // wxUSE_FILESYSTEM
#endif // wxUSE_DATETIME
            }

#           if wxUSE_FILESYSTEM
                wxDELETE(file);
                wxUnusedVar(file);
#           else
                wxDELETE(stream);
#           endif
        }
    }

    return rt;
}

wxXmlNode *wxXmlResource::DoFindResource(wxXmlNode *parent,
                                         const wxString& name,
                                         const wxString& classname,
                                         bool recursive) const
{
    wxXmlNode *node;

    // first search for match at the top-level nodes (as this is
    // where the resource is most commonly looked for):
    for (node = parent->GetChildren(); node; node = node->GetNext())
    {
        if ( IsObjectNode(node) && node->GetAttribute(wxS("name")) == name )
        {
            // empty class name matches everything
            if ( classname.empty() )
                return node;

            wxString cls(node->GetAttribute(wxS("class")));

            // object_ref may not have 'class' attribute:
            if (cls.empty() && node->GetName() == wxS("object_ref"))
            {
                wxString refName = node->GetAttribute(wxS("ref"));
                if (refName.empty())
                    continue;

                const wxXmlNode * const refNode = GetResourceNode(refName);
                if ( refNode )
                    cls = refNode->GetAttribute(wxS("class"));
            }

            if ( cls == classname )
                return node;
        }
    }

    // then recurse in child nodes
    if ( recursive )
    {
        for (node = parent->GetChildren(); node; node = node->GetNext())
        {
            if ( IsObjectNode(node) )
            {
                wxXmlNode* found = DoFindResource(node, name, classname, true);
                if ( found )
                    return found;
            }
        }
    }

    return NULL;
}

wxXmlNode *wxXmlResource::FindResource(const wxString& name,
                                       const wxString& classname,
                                       bool recursive)
{
    wxString path;
    wxXmlNode * const
        node = GetResourceNodeAndLocation(name, classname, recursive, &path);

    if ( !node )
    {
        wxLogError(_("XRC resource '%s' (class '%s') not found!"),
                   name, classname);
    }
#if wxUSE_FILESYSTEM
    else // node was found
    {
        // ensure that relative paths work correctly when loading this node
        // (which should happen as soon as we return as FindResource() result
        // is always passed to CreateResFromNode())
        m_curFileSystem.ChangePathTo(path);
    }
#endif // wxUSE_FILESYSTEM

    return node;
}

wxXmlNode *
wxXmlResource::GetResourceNodeAndLocation(const wxString& name,
                                          const wxString& classname,
                                          bool recursive,
                                          wxString *path) const
{
    // ensure everything is up-to-date: this is needed to support on-remand
    // reloading of XRC files
    const_cast<wxXmlResource *>(this)->UpdateResources();

    for ( wxXmlResourceDataRecords::const_iterator f = Data().begin();
          f != Data().end(); ++f )
    {
        wxXmlResourceDataRecord *const rec = *f;
        wxXmlDocument * const doc = rec->Doc;
        if ( !doc || !doc->GetRoot() )
            continue;

        wxXmlNode * const
            found = DoFindResource(doc->GetRoot(), name, classname, recursive);
        if ( found )
        {
            if ( path )
                *path = rec->File;

            return found;
        }
    }

    return NULL;
}

static void MergeNodes(wxXmlNode& dest, wxXmlNode& with)
{
    // Merge attributes:
    for ( wxXmlAttribute *attr = with.GetAttributes();
          attr; attr = attr->GetNext() )
    {
        wxXmlAttribute *dattr;
        for (dattr = dest.GetAttributes(); dattr; dattr = dattr->GetNext())
        {

            if ( dattr->GetName() == attr->GetName() )
            {
                dattr->SetValue(attr->GetValue());
                break;
            }
        }

        if ( !dattr )
            dest.AddAttribute(attr->GetName(), attr->GetValue());
   }

    // Merge child nodes:
    for (wxXmlNode* node = with.GetChildren(); node; node = node->GetNext())
    {
        wxString name = node->GetAttribute(wxT("name"), wxEmptyString);
        wxXmlNode *dnode;

        for (dnode = dest.GetChildren(); dnode; dnode = dnode->GetNext() )
        {
            if ( dnode->GetName() == node->GetName() &&
                 dnode->GetAttribute(wxT("name"), wxEmptyString) == name &&
                 dnode->GetType() == node->GetType() )
            {
                MergeNodes(*dnode, *node);
                break;
            }
        }

        if ( !dnode )
        {
            static const wxChar *AT_END = wxT("end");
            wxString insert_pos = node->GetAttribute(wxT("insert_at"), AT_END);
            if ( insert_pos == AT_END )
            {
                dest.AddChild(new wxXmlNode(*node));
            }
            else if ( insert_pos == wxT("begin") )
            {
                dest.InsertChild(new wxXmlNode(*node), dest.GetChildren());
            }
        }
    }

    if ( dest.GetType() == wxXML_TEXT_NODE && with.GetContent().length() )
         dest.SetContent(with.GetContent());
}

wxObject *wxXmlResource::CreateResFromNode(wxXmlNode *node, wxObject *parent,
                                           wxObject *instance,
                                           wxXmlResourceHandler *handlerToUse)
{
    if (node == NULL) return NULL;

    // handling of referenced resource
    if ( node->GetName() == wxT("object_ref") )
    {
        wxString refName = node->GetAttribute(wxT("ref"), wxEmptyString);
        wxXmlNode* refNode = FindResource(refName, wxEmptyString, true);

        if ( !refNode )
        {
            wxLogError(_("Referenced object node with ref=\"%s\" not found!"),
                       refName.c_str());
            return NULL;
        }

        wxXmlNode copy(*refNode);
        MergeNodes(copy, *node);

        return CreateResFromNode(&copy, parent, instance);
    }

    if (handlerToUse)
    {
        if (handlerToUse->CanHandle(node))
        {
            return handlerToUse->CreateResource(node, parent, instance);
        }
    }
    else if (node->GetName() == wxT("object"))
    {
        for ( wxVector<wxXmlResourceHandler*>::iterator h = m_handlers.begin();
              h != m_handlers.end(); ++h )
        {
            wxXmlResourceHandler *handler = *h;
            if (handler->CanHandle(node))
                return handler->CreateResource(node, parent, instance);
        }
    }

    wxLogError(_("No handler found for XML node '%s', class '%s'!"),
               node->GetName().c_str(),
               node->GetAttribute(wxT("class"), wxEmptyString).c_str());
    return NULL;
}


class wxXmlSubclassFactories : public wxVector<wxXmlSubclassFactory*>
{
    // this is a class so that it can be forward-declared
};

wxXmlSubclassFactories *wxXmlResource::ms_subclassFactories = NULL;

/*static*/ void wxXmlResource::AddSubclassFactory(wxXmlSubclassFactory *factory)
{
    if (!ms_subclassFactories)
    {
        ms_subclassFactories = new wxXmlSubclassFactories;
    }
    ms_subclassFactories->push_back(factory);
}

class wxXmlSubclassFactoryCXX : public wxXmlSubclassFactory
{
public:
    ~wxXmlSubclassFactoryCXX() {}

    wxObject *Create(const wxString& className)
    {
        wxClassInfo* classInfo = wxClassInfo::FindClass(className);

        if (classInfo)
            return classInfo->CreateObject();
        else
            return NULL;
    }
};




wxXmlResourceHandler::wxXmlResourceHandler()
        : m_node(NULL), m_parent(NULL), m_instance(NULL),
          m_parentAsWindow(NULL)
{}



wxObject *wxXmlResourceHandler::CreateResource(wxXmlNode *node, wxObject *parent, wxObject *instance)
{
    wxXmlNode *myNode = m_node;
    wxString myClass = m_class;
    wxObject *myParent = m_parent, *myInstance = m_instance;
    wxWindow *myParentAW = m_parentAsWindow;

    m_instance = instance;
    if (!m_instance && node->HasAttribute(wxT("subclass")) &&
        !(m_resource->GetFlags() & wxXRC_NO_SUBCLASSING))
    {
        wxString subclass = node->GetAttribute(wxT("subclass"), wxEmptyString);
        if (!subclass.empty())
        {
            for (wxXmlSubclassFactories::iterator i = wxXmlResource::ms_subclassFactories->begin();
                 i != wxXmlResource::ms_subclassFactories->end(); ++i)
            {
                m_instance = (*i)->Create(subclass);
                if (m_instance)
                    break;
            }

            if (!m_instance)
            {
                wxString name = node->GetAttribute(wxT("name"), wxEmptyString);
                wxLogError(_("Subclass '%s' not found for resource '%s', not subclassing!"),
                           subclass.c_str(), name.c_str());
            }
        }
    }

    m_node = node;
    m_class = node->GetAttribute(wxT("class"), wxEmptyString);
    m_parent = parent;
    m_parentAsWindow = wxDynamicCast(m_parent, wxWindow);

    wxObject *returned = DoCreateResource();

    m_node = myNode;
    m_class = myClass;
    m_parent = myParent; m_parentAsWindow = myParentAW;
    m_instance = myInstance;

    return returned;
}


void wxXmlResourceHandler::AddStyle(const wxString& name, int value)
{
    m_styleNames.Add(name);
    m_styleValues.Add(value);
}



void wxXmlResourceHandler::AddWindowStyles()
{
    XRC_ADD_STYLE(wxCLIP_CHILDREN);

    // the border styles all have the old and new names, recognize both for now
    XRC_ADD_STYLE(wxSIMPLE_BORDER); XRC_ADD_STYLE(wxBORDER_SIMPLE);
    XRC_ADD_STYLE(wxSUNKEN_BORDER); XRC_ADD_STYLE(wxBORDER_SUNKEN);
    XRC_ADD_STYLE(wxDOUBLE_BORDER); XRC_ADD_STYLE(wxBORDER_DOUBLE);
    XRC_ADD_STYLE(wxRAISED_BORDER); XRC_ADD_STYLE(wxBORDER_RAISED);
    XRC_ADD_STYLE(wxSTATIC_BORDER); XRC_ADD_STYLE(wxBORDER_STATIC);
    XRC_ADD_STYLE(wxNO_BORDER);     XRC_ADD_STYLE(wxBORDER_NONE);

    XRC_ADD_STYLE(wxTRANSPARENT_WINDOW);
    XRC_ADD_STYLE(wxWANTS_CHARS);
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxNO_FULL_REPAINT_ON_RESIZE);
    XRC_ADD_STYLE(wxFULL_REPAINT_ON_RESIZE);
    XRC_ADD_STYLE(wxALWAYS_SHOW_SB);
    XRC_ADD_STYLE(wxWS_EX_BLOCK_EVENTS);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
}



bool wxXmlResourceHandler::HasParam(const wxString& param)
{
    return (GetParamNode(param) != NULL);
}


int wxXmlResourceHandler::GetStyle(const wxString& param, int defaults)
{
    wxString s = GetParamValue(param);

    if (!s) return defaults;

    wxStringTokenizer tkn(s, wxT("| \t\n"), wxTOKEN_STRTOK);
    int style = 0;
    int index;
    wxString fl;
    while (tkn.HasMoreTokens())
    {
        fl = tkn.GetNextToken();
        index = m_styleNames.Index(fl);
        if (index != wxNOT_FOUND)
            style |= m_styleValues[index];
        else
            wxLogError(_("Unknown style flag ") + fl);
    }
    return style;
}



wxString wxXmlResourceHandler::GetText(const wxString& param, bool translate)
{
    wxXmlNode *parNode = GetParamNode(param);
    wxString str1(GetNodeContent(parNode));
    wxString str2;

    // "\\" wasn't translated to "\" prior to 2.5.3.0:
    const bool escapeBackslash = (m_resource->CompareVersion(2,5,3,0) >= 0);

    // VS: First version of XRC resources used $ instead of & (which is
    //     illegal in XML), but later I realized that '_' fits this purpose
    //     much better (because &File means "File with F underlined").
    const wxChar amp_char = (m_resource->CompareVersion(2,3,0,1) < 0)
                            ? '$' : '_';

    for ( wxString::const_iterator dt = str1.begin(); dt != str1.end(); ++dt )
    {
        // Remap amp_char to &, map double amp_char to amp_char (for things
        // like "&File..." -- this is illegal in XML, so we use "_File..."):
        if ( *dt == amp_char )
        {
            if ( *(++dt) == amp_char )
                str2 << amp_char;
            else
                str2 << wxT('&') << *dt;
        }
        // Remap \n to CR, \r to LF, \t to TAB, \\ to \:
        else if ( *dt == wxT('\\') )
        {
            switch ( (*(++dt)).GetValue() )
            {
                case wxT('n'):
                    str2 << wxT('\n');
                    break;

                case wxT('t'):
                    str2 << wxT('\t');
                    break;

                case wxT('r'):
                    str2 << wxT('\r');
                    break;

                case wxT('\\') :
                    // "\\" wasn't translated to "\" prior to 2.5.3.0:
                    if ( escapeBackslash )
                    {
                        str2 << wxT('\\');
                        break;
                    }
                    // else fall-through to default: branch below

                default:
                    str2 << wxT('\\') << *dt;
                    break;
            }
        }
        else
        {
            str2 << *dt;
        }
    }

    if (m_resource->GetFlags() & wxXRC_USE_LOCALE)
    {
        if (translate && parNode &&
            parNode->GetAttribute(wxT("translate"), wxEmptyString) != wxT("0"))
        {
            return wxGetTranslation(str2, m_resource->GetDomain());
        }
        else
        {
#if wxUSE_UNICODE
            return str2;
#else
            // The string is internally stored as UTF-8, we have to convert
            // it into system's default encoding so that it can be displayed:
            return wxString(str2.wc_str(wxConvUTF8), wxConvLocal);
#endif
        }
    }

    // If wxXRC_USE_LOCALE is not set, then the string is already in
    // system's default encoding in ANSI build, so we don't have to
    // do anything special here.
    return str2;
}



long wxXmlResourceHandler::GetLong(const wxString& param, long defaultv)
{
    long value;
    wxString str1 = GetParamValue(param);

    if (!str1.ToLong(&value))
        value = defaultv;

    return value;
}

float wxXmlResourceHandler::GetFloat(const wxString& param, float defaultv)
{
    wxString str = GetParamValue(param);

#if wxUSE_INTL
    // strings in XRC always use C locale but wxString::ToDouble() uses the
    // current one, so transform the string to it supposing that the only
    // difference between them is the decimal separator
    //
    // TODO: use wxString::ToCDouble() when we have it
    str.Replace(wxT("."), wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT,
                                            wxLOCALE_CAT_NUMBER));
#endif // wxUSE_INTL

    double value;
    if (!str.ToDouble(&value))
        value = defaultv;

    return wx_truncate_cast(float, value);
}


int wxXmlResourceHandler::GetID()
{
    return wxXmlResource::GetXRCID(GetName());
}



wxString wxXmlResourceHandler::GetName()
{
    return m_node->GetAttribute(wxT("name"), wxT("-1"));
}



bool wxXmlResourceHandler::GetBoolAttr(const wxString& attr, bool defaultv)
{
    wxString v;
    return m_node->GetAttribute(attr, &v) ? v == '1' : defaultv;
}

bool wxXmlResourceHandler::GetBool(const wxString& param, bool defaultv)
{
    const wxString v = GetParamValue(param);

    return v.empty() ? defaultv : (v == '1');
}


static wxColour GetSystemColour(const wxString& name)
{
    if (!name.empty())
    {
        #define SYSCLR(clr) \
            if (name == _T(#clr)) return wxSystemSettings::GetColour(clr);
        SYSCLR(wxSYS_COLOUR_SCROLLBAR)
        SYSCLR(wxSYS_COLOUR_BACKGROUND)
        SYSCLR(wxSYS_COLOUR_DESKTOP)
        SYSCLR(wxSYS_COLOUR_ACTIVECAPTION)
        SYSCLR(wxSYS_COLOUR_INACTIVECAPTION)
        SYSCLR(wxSYS_COLOUR_MENU)
        SYSCLR(wxSYS_COLOUR_WINDOW)
        SYSCLR(wxSYS_COLOUR_WINDOWFRAME)
        SYSCLR(wxSYS_COLOUR_MENUTEXT)
        SYSCLR(wxSYS_COLOUR_WINDOWTEXT)
        SYSCLR(wxSYS_COLOUR_CAPTIONTEXT)
        SYSCLR(wxSYS_COLOUR_ACTIVEBORDER)
        SYSCLR(wxSYS_COLOUR_INACTIVEBORDER)
        SYSCLR(wxSYS_COLOUR_APPWORKSPACE)
        SYSCLR(wxSYS_COLOUR_HIGHLIGHT)
        SYSCLR(wxSYS_COLOUR_HIGHLIGHTTEXT)
        SYSCLR(wxSYS_COLOUR_BTNFACE)
        SYSCLR(wxSYS_COLOUR_3DFACE)
        SYSCLR(wxSYS_COLOUR_BTNSHADOW)
        SYSCLR(wxSYS_COLOUR_3DSHADOW)
        SYSCLR(wxSYS_COLOUR_GRAYTEXT)
        SYSCLR(wxSYS_COLOUR_BTNTEXT)
        SYSCLR(wxSYS_COLOUR_INACTIVECAPTIONTEXT)
        SYSCLR(wxSYS_COLOUR_BTNHIGHLIGHT)
        SYSCLR(wxSYS_COLOUR_BTNHILIGHT)
        SYSCLR(wxSYS_COLOUR_3DHIGHLIGHT)
        SYSCLR(wxSYS_COLOUR_3DHILIGHT)
        SYSCLR(wxSYS_COLOUR_3DDKSHADOW)
        SYSCLR(wxSYS_COLOUR_3DLIGHT)
        SYSCLR(wxSYS_COLOUR_INFOTEXT)
        SYSCLR(wxSYS_COLOUR_INFOBK)
        SYSCLR(wxSYS_COLOUR_LISTBOX)
        SYSCLR(wxSYS_COLOUR_HOTLIGHT)
        SYSCLR(wxSYS_COLOUR_GRADIENTACTIVECAPTION)
        SYSCLR(wxSYS_COLOUR_GRADIENTINACTIVECAPTION)
        SYSCLR(wxSYS_COLOUR_MENUHILIGHT)
        SYSCLR(wxSYS_COLOUR_MENUBAR)
        #undef SYSCLR
    }

    return wxNullColour;
}

wxColour wxXmlResourceHandler::GetColour(const wxString& param, const wxColour& defaultv)
{
    wxString v = GetParamValue(param);

    if ( v.empty() )
        return defaultv;

    wxColour clr;

    // wxString -> wxColour conversion
    if (!clr.Set(v))
    {
        // the colour doesn't use #RRGGBB format, check if it is symbolic
        // colour name:
        clr = GetSystemColour(v);
        if (clr.Ok())
            return clr;

        wxLogError(_("XRC resource: Incorrect colour specification '%s' for attribute '%s'."),
                   v.c_str(), param.c_str());
        return wxNullColour;
    }

    return clr;
}



wxBitmap wxXmlResourceHandler::GetBitmap(const wxString& param,
                                         const wxArtClient& defaultArtClient,
                                         wxSize size)
{
    /* If the bitmap is specified as stock item, query wxArtProvider for it: */
    wxXmlNode *bmpNode = GetParamNode(param);
    if ( bmpNode )
    {
        wxString sid = bmpNode->GetAttribute(wxT("stock_id"), wxEmptyString);
        if ( !sid.empty() )
        {
            wxString scl = bmpNode->GetAttribute(wxT("stock_client"), wxEmptyString);
            if (scl.empty())
                scl = defaultArtClient;
            else
                scl = wxART_MAKE_CLIENT_ID_FROM_STR(scl);

            wxBitmap stockArt =
                wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(sid),
                                         scl, size);
            if ( stockArt.Ok() )
                return stockArt;
        }
    }

    /* ...or load the bitmap from file: */
    wxString name = GetParamValue(param);
    if (name.empty()) return wxNullBitmap;
#if wxUSE_FILESYSTEM
    wxFSFile *fsfile = GetCurFileSystem().OpenFile(name, wxFS_READ | wxFS_SEEKABLE);
    if (fsfile == NULL)
    {
        wxLogError(_("XRC resource: Cannot create bitmap from '%s'."),
                   name.c_str());
        return wxNullBitmap;
    }
    wxImage img(*(fsfile->GetStream()));
    delete fsfile;
#else
    wxImage img(name);
#endif

    if (!img.Ok())
    {
        wxLogError(_("XRC resource: Cannot create bitmap from '%s'."),
                   name.c_str());
        return wxNullBitmap;
    }
    if (!(size == wxDefaultSize)) img.Rescale(size.x, size.y);
    return wxBitmap(img);
}


wxIcon wxXmlResourceHandler::GetIcon(const wxString& param,
                                     const wxArtClient& defaultArtClient,
                                     wxSize size)
{
    wxIcon icon;
    icon.CopyFromBitmap(GetBitmap(param, defaultArtClient, size));
    return icon;
}



wxXmlNode *wxXmlResourceHandler::GetParamNode(const wxString& param)
{
    wxCHECK_MSG(m_node, NULL, wxT("You can't access handler data before it was initialized!"));

    wxXmlNode *n = m_node->GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE && n->GetName() == param)
        {
            // TODO: check that there are no other properties/parameters with
            //       the same name and log an error if there are (can't do this
            //       right now as I'm not sure if it's not going to break code
            //       using this function in unintentional way (i.e. for
            //       accessing other things than properties), for example
            //       wxBitmapComboBoxXmlHandler almost surely does
            return n;
        }
        n = n->GetNext();
    }
    return NULL;
}

bool wxXmlResourceHandler::IsOfClass(wxXmlNode *node, const wxString& classname)
{
    return node->GetAttribute(wxT("class"), wxEmptyString) == classname;
}



wxString wxXmlResourceHandler::GetNodeContent(wxXmlNode *node)
{
    wxXmlNode *n = node;
    if (n == NULL) return wxEmptyString;
    n = n->GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_TEXT_NODE ||
            n->GetType() == wxXML_CDATA_SECTION_NODE)
            return n->GetContent();
        n = n->GetNext();
    }
    return wxEmptyString;
}



wxString wxXmlResourceHandler::GetParamValue(const wxString& param)
{
    if (param.empty())
        return GetNodeContent(m_node);
    else
        return GetNodeContent(GetParamNode(param));
}



wxSize wxXmlResourceHandler::GetSize(const wxString& param,
                                     wxWindow *windowToUse)
{
    wxString s = GetParamValue(param);
    if (s.empty()) s = wxT("-1,-1");
    bool is_dlg;
    long sx, sy = 0;

    is_dlg = s[s.length()-1] == wxT('d');
    if (is_dlg) s.RemoveLast();

    if (!s.BeforeFirst(wxT(',')).ToLong(&sx) ||
        !s.AfterLast(wxT(',')).ToLong(&sy))
    {
        wxLogError(_("Cannot parse coordinates from '%s'."), s.c_str());
        return wxDefaultSize;
    }

    if (is_dlg)
    {
        if (windowToUse)
        {
            return wxDLG_UNIT(windowToUse, wxSize(sx, sy));
        }
        else if (m_parentAsWindow)
        {
            return wxDLG_UNIT(m_parentAsWindow, wxSize(sx, sy));
        }
        else
        {
            wxLogError(_("Cannot convert dialog units: dialog unknown."));
            return wxDefaultSize;
        }
    }

    return wxSize(sx, sy);
}



wxPoint wxXmlResourceHandler::GetPosition(const wxString& param)
{
    wxSize sz = GetSize(param);
    return wxPoint(sz.x, sz.y);
}



wxCoord wxXmlResourceHandler::GetDimension(const wxString& param,
                                           wxCoord defaultv,
                                           wxWindow *windowToUse)
{
    wxString s = GetParamValue(param);
    if (s.empty()) return defaultv;
    bool is_dlg;
    long sx;

    is_dlg = s[s.length()-1] == wxT('d');
    if (is_dlg) s.RemoveLast();

    if (!s.ToLong(&sx))
    {
        wxLogError(_("Cannot parse dimension from '%s'."), s.c_str());
        return defaultv;
    }

    if (is_dlg)
    {
        if (windowToUse)
        {
            return wxDLG_UNIT(windowToUse, wxSize(sx, 0)).x;
        }
        else if (m_parentAsWindow)
        {
            return wxDLG_UNIT(m_parentAsWindow, wxSize(sx, 0)).x;
        }
        else
        {
            wxLogError(_("Cannot convert dialog units: dialog unknown."));
            return defaultv;
        }
    }

    return sx;
}


// Get system font index using indexname
static wxFont GetSystemFont(const wxString& name)
{
    if (!name.empty())
    {
        #define SYSFNT(fnt) \
            if (name == _T(#fnt)) return wxSystemSettings::GetFont(fnt);
        SYSFNT(wxSYS_OEM_FIXED_FONT)
        SYSFNT(wxSYS_ANSI_FIXED_FONT)
        SYSFNT(wxSYS_ANSI_VAR_FONT)
        SYSFNT(wxSYS_SYSTEM_FONT)
        SYSFNT(wxSYS_DEVICE_DEFAULT_FONT)
        SYSFNT(wxSYS_DEFAULT_PALETTE)
        SYSFNT(wxSYS_SYSTEM_FIXED_FONT)
        SYSFNT(wxSYS_DEFAULT_GUI_FONT)
        #undef SYSFNT
    }

    return wxNullFont;
}

wxFont wxXmlResourceHandler::GetFont(const wxString& param)
{
    wxXmlNode *font_node = GetParamNode(param);
    if (font_node == NULL)
    {
        wxLogError(_("Cannot find font node '%s'."), param.c_str());
        return wxNullFont;
    }

    wxXmlNode *oldnode = m_node;
    m_node = font_node;

    // font attributes:

    // size
    int isize = -1;
    bool hasSize = HasParam(wxT("size"));
    if (hasSize)
        isize = GetLong(wxT("size"), -1);

    // style
    int istyle = wxNORMAL;
    bool hasStyle = HasParam(wxT("style"));
    if (hasStyle)
    {
        wxString style = GetParamValue(wxT("style"));
        if (style == wxT("italic"))
            istyle = wxITALIC;
        else if (style == wxT("slant"))
            istyle = wxSLANT;
    }

    // weight
    int iweight = wxNORMAL;
    bool hasWeight = HasParam(wxT("weight"));
    if (hasWeight)
    {
        wxString weight = GetParamValue(wxT("weight"));
        if (weight == wxT("bold"))
            iweight = wxBOLD;
        else if (weight == wxT("light"))
            iweight = wxLIGHT;
    }

    // underline
    bool hasUnderlined = HasParam(wxT("underlined"));
    bool underlined = hasUnderlined ? GetBool(wxT("underlined"), false) : false;

    // family and facename
    int ifamily = wxDEFAULT;
    bool hasFamily = HasParam(wxT("family"));
    if (hasFamily)
    {
        wxString family = GetParamValue(wxT("family"));
             if (family == wxT("decorative")) ifamily = wxDECORATIVE;
        else if (family == wxT("roman")) ifamily = wxROMAN;
        else if (family == wxT("script")) ifamily = wxSCRIPT;
        else if (family == wxT("swiss")) ifamily = wxSWISS;
        else if (family == wxT("modern")) ifamily = wxMODERN;
        else if (family == wxT("teletype")) ifamily = wxTELETYPE;
    }


    wxString facename;
    bool hasFacename = HasParam(wxT("face"));
    if (hasFacename)
    {
        wxString faces = GetParamValue(wxT("face"));
        wxStringTokenizer tk(faces, wxT(","));
#if wxUSE_FONTENUM
        wxArrayString facenames(wxFontEnumerator::GetFacenames());
        while (tk.HasMoreTokens())
        {
            int index = facenames.Index(tk.GetNextToken(), false);
            if (index != wxNOT_FOUND)
            {
                facename = facenames[index];
                break;
            }
        }
#else // !wxUSE_FONTENUM
        // just use the first face name if we can't check its availability:
        if (tk.HasMoreTokens())
            facename = tk.GetNextToken();
#endif // wxUSE_FONTENUM/!wxUSE_FONTENUM
    }

    // encoding
    wxFontEncoding enc = wxFONTENCODING_DEFAULT;
    bool hasEncoding = HasParam(wxT("encoding"));
    if (hasEncoding)
    {
        wxString encoding = GetParamValue(wxT("encoding"));
        wxFontMapper mapper;
        if (!encoding.empty())
            enc = mapper.CharsetToEncoding(encoding);
        if (enc == wxFONTENCODING_SYSTEM)
            enc = wxFONTENCODING_DEFAULT;
    }

    // is this font based on a system font?
    wxFont font = GetSystemFont(GetParamValue(wxT("sysfont")));

    if (font.Ok())
    {
        if (hasSize && isize != -1)
            font.SetPointSize(isize);
        else if (HasParam(wxT("relativesize")))
            font.SetPointSize(int(font.GetPointSize() *
                                     GetFloat(wxT("relativesize"))));

        if (hasStyle)
            font.SetStyle(istyle);
        if (hasWeight)
            font.SetWeight(iweight);
        if (hasUnderlined)
            font.SetUnderlined(underlined);
        if (hasFamily)
            font.SetFamily(ifamily);
        if (hasFacename)
            font.SetFaceName(facename);
        if (hasEncoding)
            font.SetDefaultEncoding(enc);
    }
    else // not based on system font
    {
        font = wxFont(isize == -1 ? wxNORMAL_FONT->GetPointSize() : isize,
                      ifamily, istyle, iweight,
                      underlined, facename, enc);
    }

    m_node = oldnode;
    return font;
}


void wxXmlResourceHandler::SetupWindow(wxWindow *wnd)
{
    //FIXME : add cursor

    if (HasParam(wxT("exstyle")))
        // Have to OR it with existing style, since
        // some implementations (e.g. wxGTK) use the extra style
        // during creation
        wnd->SetExtraStyle(wnd->GetExtraStyle() | GetStyle(wxT("exstyle")));
    if (HasParam(wxT("bg")))
        wnd->SetBackgroundColour(GetColour(wxT("bg")));
    if (HasParam(wxT("fg")))
        wnd->SetForegroundColour(GetColour(wxT("fg")));
    if (GetBool(wxT("enabled"), 1) == 0)
        wnd->Enable(false);
    if (GetBool(wxT("focused"), 0) == 1)
        wnd->SetFocus();
    if (GetBool(wxT("hidden"), 0) == 1)
        wnd->Show(false);
#if wxUSE_TOOLTIPS
    if (HasParam(wxT("tooltip")))
        wnd->SetToolTip(GetText(wxT("tooltip")));
#endif
    if (HasParam(wxT("font")))
        wnd->SetFont(GetFont());
    if (HasParam(wxT("help")))
        wnd->SetHelpText(GetText(wxT("help")));
}


void wxXmlResourceHandler::CreateChildren(wxObject *parent, bool this_hnd_only)
{
    for ( wxXmlNode *n = m_node->GetChildren(); n; n = n->GetNext() )
    {
        if ( IsObjectNode(n) )
        {
            m_resource->CreateResFromNode(n, parent, NULL,
                                          this_hnd_only ? this : NULL);
        }
    }
}


void wxXmlResourceHandler::CreateChildrenPrivately(wxObject *parent, wxXmlNode *rootnode)
{
    wxXmlNode *root;
    if (rootnode == NULL) root = m_node; else root = rootnode;
    wxXmlNode *n = root->GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE && CanHandle(n))
        {
            CreateResource(n, parent, NULL);
        }
        n = n->GetNext();
    }
}







// --------------- XRCID implementation -----------------------------

#define XRCID_TABLE_SIZE     1024


struct XRCID_record
{
    /* Hold the id so that once an id is allocated for a name, it
       does not get created again by NewControlId at least
       until we are done with it */
    wxWindowIDRef id;
    char *key;
    XRCID_record *next;
};

static XRCID_record *XRCID_Records[XRCID_TABLE_SIZE] = {NULL};

static int XRCID_Lookup(const char *str_id, int value_if_not_found = wxID_NONE)
{
    int index = 0;

    for (const char *c = str_id; *c != '\0'; c++) index += (int)*c;
    index %= XRCID_TABLE_SIZE;

    XRCID_record *oldrec = NULL;
    for (XRCID_record *rec = XRCID_Records[index]; rec; rec = rec->next)
    {
        if (wxStrcmp(rec->key, str_id) == 0)
        {
            return rec->id;
        }
        oldrec = rec;
    }

    XRCID_record **rec_var = (oldrec == NULL) ?
                              &XRCID_Records[index] : &oldrec->next;
    *rec_var = new XRCID_record;
    (*rec_var)->key = wxStrdup(str_id);
    (*rec_var)->next = NULL;

    char *end;
    if (value_if_not_found != wxID_NONE)
        (*rec_var)->id = value_if_not_found;
    else
    {
        int asint = wxStrtol(str_id, &end, 10);
        if (*str_id && *end == 0)
        {
            // if str_id was integer, keep it verbosely:
            (*rec_var)->id = asint;
        }
        else
        {
            (*rec_var)->id = wxWindowBase::NewControlId();
        }
    }

    return (*rec_var)->id;
}

static void AddStdXRCID_Records();

/*static*/
int wxXmlResource::DoGetXRCID(const char *str_id, int value_if_not_found)
{
    static bool s_stdIDsAdded = false;

    if ( !s_stdIDsAdded )
    {
        s_stdIDsAdded = true;
        AddStdXRCID_Records();
    }

    return XRCID_Lookup(str_id, value_if_not_found);
}

/* static */
wxString wxXmlResource::FindXRCIDById(int numId)
{
    for ( int i = 0; i < XRCID_TABLE_SIZE; i++ )
    {
        for ( XRCID_record *rec = XRCID_Records[i]; rec; rec = rec->next )
        {
            if ( rec->id == numId )
                return wxString(rec->key);
        }
    }

    return wxString();
}

static void CleanXRCID_Record(XRCID_record *rec)
{
    if (rec)
    {
        CleanXRCID_Record(rec->next);

        free(rec->key);
        delete rec;
    }
}

static void CleanXRCID_Records()
{
    for (int i = 0; i < XRCID_TABLE_SIZE; i++)
    {
        CleanXRCID_Record(XRCID_Records[i]);
        XRCID_Records[i] = NULL;
    }
}

static void AddStdXRCID_Records()
{
#define stdID(id) XRCID_Lookup(#id, id)
    stdID(-1);

    stdID(wxID_ANY);
    stdID(wxID_SEPARATOR);

    stdID(wxID_OPEN);
    stdID(wxID_CLOSE);
    stdID(wxID_NEW);
    stdID(wxID_SAVE);
    stdID(wxID_SAVEAS);
    stdID(wxID_REVERT);
    stdID(wxID_EXIT);
    stdID(wxID_UNDO);
    stdID(wxID_REDO);
    stdID(wxID_HELP);
    stdID(wxID_PRINT);
    stdID(wxID_PRINT_SETUP);
    stdID(wxID_PAGE_SETUP);
    stdID(wxID_PREVIEW);
    stdID(wxID_ABOUT);
    stdID(wxID_HELP_CONTENTS);
    stdID(wxID_HELP_COMMANDS);
    stdID(wxID_HELP_PROCEDURES);
    stdID(wxID_HELP_CONTEXT);
    stdID(wxID_CLOSE_ALL);
    stdID(wxID_PREFERENCES);
    stdID(wxID_EDIT);
    stdID(wxID_CUT);
    stdID(wxID_COPY);
    stdID(wxID_PASTE);
    stdID(wxID_CLEAR);
    stdID(wxID_FIND);
    stdID(wxID_DUPLICATE);
    stdID(wxID_SELECTALL);
    stdID(wxID_DELETE);
    stdID(wxID_REPLACE);
    stdID(wxID_REPLACE_ALL);
    stdID(wxID_PROPERTIES);
    stdID(wxID_VIEW_DETAILS);
    stdID(wxID_VIEW_LARGEICONS);
    stdID(wxID_VIEW_SMALLICONS);
    stdID(wxID_VIEW_LIST);
    stdID(wxID_VIEW_SORTDATE);
    stdID(wxID_VIEW_SORTNAME);
    stdID(wxID_VIEW_SORTSIZE);
    stdID(wxID_VIEW_SORTTYPE);
    stdID(wxID_FILE1);
    stdID(wxID_FILE2);
    stdID(wxID_FILE3);
    stdID(wxID_FILE4);
    stdID(wxID_FILE5);
    stdID(wxID_FILE6);
    stdID(wxID_FILE7);
    stdID(wxID_FILE8);
    stdID(wxID_FILE9);
    stdID(wxID_OK);
    stdID(wxID_CANCEL);
    stdID(wxID_APPLY);
    stdID(wxID_YES);
    stdID(wxID_NO);
    stdID(wxID_STATIC);
    stdID(wxID_FORWARD);
    stdID(wxID_BACKWARD);
    stdID(wxID_DEFAULT);
    stdID(wxID_MORE);
    stdID(wxID_SETUP);
    stdID(wxID_RESET);
    stdID(wxID_CONTEXT_HELP);
    stdID(wxID_YESTOALL);
    stdID(wxID_NOTOALL);
    stdID(wxID_ABORT);
    stdID(wxID_RETRY);
    stdID(wxID_IGNORE);
    stdID(wxID_ADD);
    stdID(wxID_REMOVE);
    stdID(wxID_UP);
    stdID(wxID_DOWN);
    stdID(wxID_HOME);
    stdID(wxID_REFRESH);
    stdID(wxID_STOP);
    stdID(wxID_INDEX);
    stdID(wxID_BOLD);
    stdID(wxID_ITALIC);
    stdID(wxID_JUSTIFY_CENTER);
    stdID(wxID_JUSTIFY_FILL);
    stdID(wxID_JUSTIFY_RIGHT);
    stdID(wxID_JUSTIFY_LEFT);
    stdID(wxID_UNDERLINE);
    stdID(wxID_INDENT);
    stdID(wxID_UNINDENT);
    stdID(wxID_ZOOM_100);
    stdID(wxID_ZOOM_FIT);
    stdID(wxID_ZOOM_IN);
    stdID(wxID_ZOOM_OUT);
    stdID(wxID_UNDELETE);
    stdID(wxID_REVERT_TO_SAVED);
    stdID(wxID_SYSTEM_MENU);
    stdID(wxID_CLOSE_FRAME);
    stdID(wxID_MOVE_FRAME);
    stdID(wxID_RESIZE_FRAME);
    stdID(wxID_MAXIMIZE_FRAME);
    stdID(wxID_ICONIZE_FRAME);
    stdID(wxID_RESTORE_FRAME);
    stdID(wxID_CDROM);
    stdID(wxID_CONVERT);
    stdID(wxID_EXECUTE);
    stdID(wxID_FLOPPY);
    stdID(wxID_HARDDISK);
    stdID(wxID_BOTTOM);
    stdID(wxID_FIRST);
    stdID(wxID_LAST);
    stdID(wxID_TOP);
    stdID(wxID_INFO);
    stdID(wxID_JUMP_TO);
    stdID(wxID_NETWORK);
    stdID(wxID_SELECT_COLOR);
    stdID(wxID_SELECT_FONT);
    stdID(wxID_SORT_ASCENDING);
    stdID(wxID_SORT_DESCENDING);
    stdID(wxID_SPELL_CHECK);
    stdID(wxID_STRIKETHROUGH);

#undef stdID
}





// --------------- module and globals -----------------------------

// normally we would do the cleanup from wxXmlResourceModule::OnExit() but it
// can happen that some XRC records have been created because of the use of
// XRCID() in event tables, which happens during static objects initialization,
// but then the application initialization failed and so the wx modules were
// neither initialized nor cleaned up -- this static object does the cleanup in
// this case
static struct wxXRCStaticCleanup
{
    ~wxXRCStaticCleanup() { CleanXRCID_Records(); }
} s_staticCleanup;

class wxXmlResourceModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxXmlResourceModule)
public:
    wxXmlResourceModule() {}
    bool OnInit()
    {
        wxXmlResource::AddSubclassFactory(new wxXmlSubclassFactoryCXX);
        return true;
    }
    void OnExit()
    {
        delete wxXmlResource::Set(NULL);
        if(wxXmlResource::ms_subclassFactories)
        {
            for ( wxXmlSubclassFactories::iterator i = wxXmlResource::ms_subclassFactories->begin();
                  i != wxXmlResource::ms_subclassFactories->end(); ++i )
            {
                delete *i;
            }
            wxDELETE(wxXmlResource::ms_subclassFactories);
        }
        CleanXRCID_Records();
    }
};

IMPLEMENT_DYNAMIC_CLASS(wxXmlResourceModule, wxModule)


// When wxXml is loaded dynamically after the application is already running
// then the built-in module system won't pick this one up.  Add it manually.
void wxXmlInitResourceModule()
{
    wxModule* module = new wxXmlResourceModule;
    module->Init();
    wxModule::RegisterModule(module);
}

#endif // wxUSE_XRC
