/////////////////////////////////////////////////////////////////////////////
// Name:        xmlres.cpp
// Purpose:     XRC resources
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "xmlres.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dialog.h"
#include "wx/panel.h"
#include "wx/frame.h"
#include "wx/wfstream.h"
#include "wx/filesys.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/tokenzr.h"
#include "wx/fontenum.h"
#include "wx/module.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/fontmap.h"

#include "wx/xrc/xml.h"
#include "wx/xrc/xmlres.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxXmlResourceDataRecords);


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

wxXmlResource::wxXmlResource(int flags)
{
    m_handlers.DeleteContents(TRUE);
    m_flags = flags;
    m_version = -1;
}

wxXmlResource::wxXmlResource(const wxString& filemask, int flags)
{
    m_flags = flags;
    m_version = -1;
    m_handlers.DeleteContents(TRUE);
    Load(filemask);
}

wxXmlResource::~wxXmlResource()
{
    ClearHandlers();
}


bool wxXmlResource::Load(const wxString& filemask)
{
    wxString fnd;
    wxXmlResourceDataRecord *drec;
    bool iswild = wxIsWild(filemask);
    bool rt = TRUE;

#if wxUSE_FILESYSTEM
    wxFileSystem fsys;
#   define wxXmlFindFirst  fsys.FindFirst(filemask, wxFILE)
#   define wxXmlFindNext   fsys.FindNext()
#else
#   define wxXmlFindFirst  wxFindFirstFile(filemask, wxFILE)
#   define wxXmlFindNext   wxFindNextFile()
#endif
    if (iswild)
        fnd = wxXmlFindFirst;
    else
        fnd = filemask;
    while (!!fnd)
    {
#if wxUSE_FILESYSTEM
        if (filemask.Lower().Matches(wxT("*.zip")) ||
            filemask.Lower().Matches(wxT("*.xrs")))
        {
            rt = rt && Load(fnd + wxT("#zip:*.xmlbin"));
            rt = rt && Load(fnd + wxT("#zip:*.xrc"));
        }
        else
#endif
        {
            drec = new wxXmlResourceDataRecord;
            drec->File = fnd;
            m_data.Add(drec);
        }

        if (iswild)
            fnd = wxXmlFindNext;
        else
            fnd = wxEmptyString;
    }
#   undef wxXmlFindFirst
#   undef wxXmlFindNext
    return rt;
}



void wxXmlResource::AddHandler(wxXmlResourceHandler *handler)
{
    m_handlers.Append(handler);
    handler->SetParentResource(this);
}



void wxXmlResource::ClearHandlers()
{
    m_handlers.Clear();
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
    wxDialog *dialog = new wxDialog;
    if (!LoadDialog(dialog, parent, name))
        { delete dialog; return NULL; }
    else return dialog;
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

bool wxXmlResource::AttachUnknownControl(const wxString& name,
                                         wxWindow *control, wxWindow *parent)
{
    if (parent == NULL)
        parent = control->GetParent();
    wxWindow *container = parent->FindWindow(name + wxT("_container"));
    if (!container)
    {
        wxLogError(_("Cannot find container for unknown control '%s'."), name.c_str());
        return FALSE;
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
        isok = FALSE;
        if (!c->GetPropVal(wxT("platform"), &s))
            isok = TRUE;
        else
        {
            wxStringTokenizer tkn(s, " |");

            while (tkn.HasMoreTokens())
            {
                s = tkn.GetNextToken();
                if (
#ifdef __WXMSW__
                    s == wxString(wxT("win"))
#elif defined(__UNIX__)
                    s == wxString(wxT("unix"))
#elif defined(__MAC__)
                    s == wxString(wxT("mac"))
#elif defined(__OS2__)
                    s == wxString(wxT("os2"))
#else
                    FALSE
#endif
              ) isok = TRUE;
            }
        }

        if (isok)
        {
            ProcessPlatformProperty(c);
            c = c->GetNext();
        }
        else
        {
            node->RemoveChild(c);
            wxXmlNode *c2 = c->GetNext();
            delete c;
            c = c2;
        }
    }
}



void wxXmlResource::UpdateResources()
{
    bool modif;
#   if wxUSE_FILESYSTEM
    wxFSFile *file = NULL;
    wxFileSystem fsys;
#   endif

    wxString encoding(wxT("UTF-8"));
#if !wxUSE_UNICODE && wxUSE_INTL
    if ( (GetFlags() & wxXRC_USE_LOCALE) == 0 )
    {
        // In case we are not using wxLocale to translate strings, convert the strings
        // GUI's charset. This must not be done when wxXRC_USE_LOCALE is on, because
        // it could break wxGetTranslation lookup.
        encoding = wxLocale::GetSystemEncodingName();
    }
#endif

    for (size_t i = 0; i < m_data.GetCount(); i++)
    {
        modif = (m_data[i].Doc == NULL);

        if (!modif)
        {
#           if wxUSE_FILESYSTEM
            file = fsys.OpenFile(m_data[i].File);
            modif = file && file->GetModificationTime() > m_data[i].Time;
            if (!file)
                wxLogError(_("Cannot open file '%s'."), m_data[i].File.c_str());
            wxDELETE(file);
#           else
            modif = wxDateTime(wxFileModificationTime(m_data[i].File)) > m_data[i].Time;
#           endif
        }

        if (modif)
        {
            wxInputStream *stream = NULL;

#           if wxUSE_FILESYSTEM
            file = fsys.OpenFile(m_data[i].File);
			if (file)
				stream = file->GetStream();
#           else
            stream = new wxFileInputStream(m_data[i].File);
#           endif

            if (stream)
            {
                delete m_data[i].Doc;
                m_data[i].Doc = new wxXmlDocument;
            }
            if (!stream || !m_data[i].Doc->Load(*stream, encoding))
            {
                wxLogError(_("Cannot load resources from file '%s'."),
                           m_data[i].File.c_str());
                wxDELETE(m_data[i].Doc);
            }
            else if (m_data[i].Doc->GetRoot()->GetName() != wxT("resource"))
            {
                wxLogError(_("Invalid XRC resource '%s': doesn't have root node 'resource'."), m_data[i].File.c_str());
                wxDELETE(m_data[i].Doc);
            }
            else
			{
                long version;
                int v1, v2, v3, v4;
                wxString verstr = m_data[i].Doc->GetRoot()->GetPropVal(
                                      wxT("version"), wxT("0.0.0.0"));
                if (wxSscanf(verstr.c_str(), wxT("%i.%i.%i.%i"),
                    &v1, &v2, &v3, &v4) == 4)
                    version = v1*256*256*256+v2*256*256+v3*256+v4;
                else
                    version = 0;
                if (m_version == -1)
                    m_version = version;
                if (m_version != version)
                    wxLogError(_("Resource files must have same version number!"));

                ProcessPlatformProperty(m_data[i].Doc->GetRoot());
				m_data[i].Time = file->GetModificationTime();
			}

#           if wxUSE_FILESYSTEM
				wxDELETE(file);
#           else
				wxDELETE(stream);
#           endif
        }
    }
}


wxXmlNode *wxXmlResource::DoFindResource(wxXmlNode *parent, 
                                         const wxString& name, 
                                         const wxString& classname, 
                                         bool recursive)
{
    wxString dummy;
    wxXmlNode *node;

    // first search for match at the top-level nodes (as this is
    // where the resource is most commonly looked for):
    for (node = parent->GetChildren(); node; node = node->GetNext())
    {
        if ( node->GetType() == wxXML_ELEMENT_NODE && 
                 (node->GetName() == wxT("object") || 
                  node->GetName() == wxT("object_ref")) &&
                (!classname || 
                 node->GetPropVal(wxT("class"), wxEmptyString) == classname) &&
                node->GetPropVal(wxT("name"), &dummy) && dummy == name )
            return node;
    }

    if ( recursive )
        for (node = parent->GetChildren(); node; node = node->GetNext())
        {
            if ( node->GetType() == wxXML_ELEMENT_NODE && 
                 (node->GetName() == wxT("object") || 
                  node->GetName() == wxT("object_ref")) )
            {
                wxXmlNode* found = DoFindResource(node, name, classname, TRUE);
                if ( found )
                    return found;
            }
        }

   return NULL;
}

wxXmlNode *wxXmlResource::FindResource(const wxString& name, 
                                       const wxString& classname,
                                       bool recursive)
{
    UpdateResources(); //ensure everything is up-to-date

    wxString dummy;
    for (size_t f = 0; f < m_data.GetCount(); f++)
    {
        if ( m_data[f].Doc == NULL || m_data[f].Doc->GetRoot() == NULL )
            continue;

        wxXmlNode* found = DoFindResource(m_data[f].Doc->GetRoot(), 
                                          name, classname, recursive);
        if ( found )
        {
#if wxUSE_FILESYSTEM
            m_curFileSystem.ChangePathTo(m_data[f].File);
#endif
            return found;
        }
    }

    wxLogError(_("XRC resource '%s' (class '%s') not found!"),
               name.c_str(), classname.c_str());
    return NULL;
}

static void MergeNodes(wxXmlNode& dest, wxXmlNode& with)
{
    // Merge properties:
    for (wxXmlProperty *prop = with.GetProperties(); prop; prop = prop->GetNext())
    {
        wxXmlProperty *dprop;
        for (dprop = dest.GetProperties(); dprop; dprop = dprop->GetNext())
        {
       
            if ( dprop->GetName() == prop->GetName() )
            {
                dprop->SetValue(prop->GetValue());
                break;
            }
        }

        if ( !dprop )
            dest.AddProperty(prop->GetName(), prop->GetValue());
   }

    // Merge child nodes:
    for (wxXmlNode* node = with.GetChildren(); node; node = node->GetNext())
    {
        wxString name = node->GetPropVal(wxT("name"), wxEmptyString);
        wxXmlNode *dnode;

        for (dnode = dest.GetChildren(); dnode; dnode = dnode->GetNext() )
        {
            if ( dnode->GetName() == node->GetName() &&
                 dnode->GetPropVal("name", wxEmptyString) == name &&
                 dnode->GetType() == node->GetType() )
            {
                MergeNodes(*dnode, *node);
                break;
            }
        }

        if ( !dnode )
            dest.AddChild(new wxXmlNode(*node));
    }

    if ( dest.GetType() == wxXML_TEXT_NODE && with.GetContent().Length() )
         dest.SetContent(with.GetContent());
}

wxObject *wxXmlResource::CreateResFromNode(wxXmlNode *node, wxObject *parent, wxObject *instance)
{
    if (node == NULL) return NULL;

    // handling of referenced resource
    if ( node->GetName() == wxT("object_ref") )
    {
        wxString refName = node->GetPropVal(wxT("ref"), wxEmptyString);
        wxXmlNode* refNode = FindResource(refName, wxEmptyString, TRUE);

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

    wxXmlResourceHandler *handler;
    wxObject *ret;
    wxNode * ND = m_handlers.GetFirst();
    while (ND)
    {
        handler = (wxXmlResourceHandler*)ND->GetData();
        if (node->GetName() == wxT("object") && handler->CanHandle(node))
        {
            ret = handler->CreateResource(node, parent, instance);
            if (ret) return ret;
        }
        ND = ND->GetNext();
    }

    wxLogError(_("No handler found for XML node '%s', class '%s'!"),
               node->GetName().c_str(),
               node->GetPropVal(wxT("class"), wxEmptyString).c_str());
    return NULL;
}




wxXmlResourceHandler::wxXmlResourceHandler()
        : m_node(NULL), m_parent(NULL), m_instance(NULL),
          m_parentAsWindow(NULL), m_instanceAsWindow(NULL)
{}



wxObject *wxXmlResourceHandler::CreateResource(wxXmlNode *node, wxObject *parent, wxObject *instance)
{
    wxXmlNode *myNode = m_node;
    wxString myClass = m_class;
    wxObject *myParent = m_parent, *myInstance = m_instance;
    wxWindow *myParentAW = m_parentAsWindow, *myInstanceAW = m_instanceAsWindow;

    m_instance = instance;
    if (!m_instance && node->HasProp(wxT("subclass")) && 
        !(m_resource->GetFlags() & wxXRC_NO_SUBCLASSING))
    {
        wxString subclass = node->GetPropVal(wxT("subclass"), wxEmptyString);
        wxClassInfo* classInfo = wxClassInfo::FindClass(subclass);

        if (classInfo)
            m_instance = classInfo->CreateObject();
        
        if (!m_instance)
        {
            wxLogError(_("Subclass '%s' not found for resource '%s', not subclassing!"),
                       subclass.c_str(), node->GetPropVal(wxT("name"), wxEmptyString).c_str());
        }

        m_instance = classInfo->CreateObject();
    }

    m_node = node;
    m_class = node->GetPropVal(wxT("class"), wxEmptyString);
    m_parent = parent;
    m_parentAsWindow = wxDynamicCast(m_parent, wxWindow);
    m_instanceAsWindow = wxDynamicCast(m_instance, wxWindow);

    wxObject *returned = DoCreateResource();

    m_node = myNode;
    m_class = myClass;
    m_parent = myParent; m_parentAsWindow = myParentAW;
    m_instance = myInstance; m_instanceAsWindow = myInstanceAW;

    return returned;
}


void wxXmlResourceHandler::AddStyle(const wxString& name, int value)
{
    m_styleNames.Add(name);
    m_styleValues.Add(value);
}



void wxXmlResourceHandler::AddWindowStyles()
{
    XRC_ADD_STYLE(wxSIMPLE_BORDER);
    XRC_ADD_STYLE(wxSUNKEN_BORDER);
    XRC_ADD_STYLE(wxDOUBLE_BORDER);
    XRC_ADD_STYLE(wxRAISED_BORDER);
    XRC_ADD_STYLE(wxSTATIC_BORDER);
    XRC_ADD_STYLE(wxNO_BORDER);
    XRC_ADD_STYLE(wxTRANSPARENT_WINDOW);
    XRC_ADD_STYLE(wxWANTS_CHARS);
    XRC_ADD_STYLE(wxNO_FULL_REPAINT_ON_RESIZE);
}



bool wxXmlResourceHandler::HasParam(const wxString& param)
{
    return (GetParamNode(param) != NULL);
}


int wxXmlResourceHandler::GetStyle(const wxString& param, int defaults)
{
    wxString s = GetParamValue(param);

    if (!s) return defaults;

    wxStringTokenizer tkn(s, wxT("| "), wxTOKEN_STRTOK);
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



wxString wxXmlResourceHandler::GetText(const wxString& param)
{
    wxString str1;
    wxString str2;
    const wxChar *dt;
    wxChar amp_char;

    if (m_resource->GetFlags() & wxXRC_USE_LOCALE)
        str1 = wxGetTranslation(GetParamValue(param));
    else
        str1 = GetParamValue(param);

    // VS: First version of XRC resources used $ instead of & (which is illegal in XML),
    //     but later I realized that '_' fits this purpose much better (because
    //     &File means "File with F underlined").
    if (m_resource->CompareVersion(2,3,0,1) < 0)
        amp_char = wxT('$');
    else
        amp_char = wxT('_');

    for (dt = str1.c_str(); *dt; dt++)
    {
        // Remap amp_char to &, map double amp_char to amp_char (for things
        // like "&File..." -- this is illegal in XML, so we use "_File..."):
        if (*dt == amp_char)
        {
            if ( *(++dt) == amp_char )
                str2 << amp_char;
            else
                str2 << wxT('&') << *dt;
        }
        // Remap \n to CR, \r to LF, \t to TAB:
        else if (*dt == wxT('\\'))
            switch (*(++dt))
            {
                case wxT('n') : str2 << wxT('\n'); break;
                case wxT('t') : str2 << wxT('\t'); break;
                case wxT('r') : str2 << wxT('\r'); break;
                default       : str2 << wxT('\\') << *dt; break;
            }
        else str2 << *dt;
    }
    
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


int wxXmlResourceHandler::GetID()
{
    wxString sid = GetName();
    long num;

    if (sid == wxT("-1")) return -1;
    else if (sid.IsNumber() && sid.ToLong(&num)) return num;
#define stdID(id) else if (sid == wxT(#id)) return id
    stdID(wxID_OPEN); stdID(wxID_CLOSE); stdID(wxID_NEW);
    stdID(wxID_SAVE); stdID(wxID_SAVEAS); stdID(wxID_REVERT);
    stdID(wxID_EXIT); stdID(wxID_UNDO); stdID(wxID_REDO);
    stdID(wxID_HELP); stdID(wxID_PRINT); stdID(wxID_PRINT_SETUP);
    stdID(wxID_PREVIEW); stdID(wxID_ABOUT); stdID(wxID_HELP_CONTENTS);
    stdID(wxID_HELP_COMMANDS); stdID(wxID_HELP_PROCEDURES);
    stdID(wxID_CUT); stdID(wxID_COPY); stdID(wxID_PASTE);
    stdID(wxID_CLEAR); stdID(wxID_FIND); stdID(wxID_DUPLICATE);
    stdID(wxID_SELECTALL); stdID(wxID_OK); stdID(wxID_CANCEL);
    stdID(wxID_APPLY); stdID(wxID_YES); stdID(wxID_NO);
    stdID(wxID_STATIC); stdID(wxID_FORWARD); stdID(wxID_BACKWARD);
    stdID(wxID_DEFAULT); stdID(wxID_MORE); stdID(wxID_SETUP);
    stdID(wxID_RESET); stdID(wxID_HELP_CONTEXT);
#undef stdID
    else return wxXmlResource::GetXRCID(sid);
}


wxString wxXmlResourceHandler::GetName()
{
    return m_node->GetPropVal(wxT("name"), wxT("-1"));
}



bool wxXmlResourceHandler::GetBool(const wxString& param, bool defaultv)
{
    wxString v = GetParamValue(param);
    v.MakeLower();
    if (!v) return defaultv;
    else return (v == wxT("1"));
}



wxColour wxXmlResourceHandler::GetColour(const wxString& param)
{
    wxString v = GetParamValue(param);
    unsigned long tmp = 0;

    if (v.Length() != 7 || v[0u] != wxT('#') ||
        wxSscanf(v.c_str(), wxT("#%lX"), &tmp) != 1)
    {
        wxLogError(_("XRC resource: Incorrect colour specification '%s' for property '%s'."),
                   v.c_str(), param.c_str());
        return wxNullColour;
    }

    return wxColour((unsigned char) ((tmp & 0xFF0000) >> 16) ,
                    (unsigned char) ((tmp & 0x00FF00) >> 8),
                    (unsigned char) ((tmp & 0x0000FF)));
}



wxBitmap wxXmlResourceHandler::GetBitmap(const wxString& param, wxSize size)
{
    wxString name = GetParamValue(param);
    if (name.IsEmpty()) return wxNullBitmap;
#if wxUSE_FILESYSTEM
    wxFSFile *fsfile = GetCurFileSystem().OpenFile(name);
    if (fsfile == NULL)
    {
        wxLogError(_("XRC resource: Cannot create bitmap from '%s'."), param.c_str());
        return wxNullBitmap;
    }
    wxImage img(*(fsfile->GetStream()));
    delete fsfile;
#else
    wxImage img(GetParamValue(wxT("bitmap")));
#endif
    if (!img.Ok())
    {
        wxLogError(_("XRC resource: Cannot create bitmap from '%s'."), param.c_str());
        return wxNullBitmap;
    }
    if (!(size == wxDefaultSize)) img.Rescale(size.x, size.y);
    return img.ConvertToBitmap();
}



wxIcon wxXmlResourceHandler::GetIcon(const wxString& param, wxSize size)
{
#if wxCHECK_VERSION(2,3,0) || defined(__WXMSW__)
    wxIcon icon;
    icon.CopyFromBitmap(GetBitmap(param, size));
#else
    wxIcon *iconpt;
    wxBitmap bmppt = GetBitmap(param, size);
    iconpt = (wxIcon*)(&bmppt);
    wxIcon icon(*iconpt);
#endif
    return icon;
}



wxXmlNode *wxXmlResourceHandler::GetParamNode(const wxString& param)
{
    wxXmlNode *n = m_node->GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE && n->GetName() == param)
            return n;
        n = n->GetNext();
    }
    return NULL;
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
    if (param.IsEmpty())
        return GetNodeContent(m_node);
    else
        return GetNodeContent(GetParamNode(param));
}



wxSize wxXmlResourceHandler::GetSize(const wxString& param)
{
    wxString s = GetParamValue(param);
    if (s.IsEmpty()) s = wxT("-1,-1");
    bool is_dlg;
    long sx, sy;

    is_dlg = s[s.Length()-1] == wxT('d');
    if (is_dlg) s.RemoveLast();

    if (!s.BeforeFirst(wxT(',')).ToLong(&sx) ||
        !s.AfterLast(wxT(',')).ToLong(&sy))
    {
        wxLogError(_("Cannot parse coordinates from '%s'."), s.c_str());
        return wxDefaultSize;
    }

    if (is_dlg)
    {
        if (m_instanceAsWindow)
            return wxDLG_UNIT(m_instanceAsWindow, wxSize(sx, sy));
        else if (m_parentAsWindow)
            return wxDLG_UNIT(m_parentAsWindow, wxSize(sx, sy));
        else
        {
            wxLogError(_("Cannot convert dialog units: dialog unknown."));
            return wxDefaultSize;
        }
    }
    else return wxSize(sx, sy);
}



wxPoint wxXmlResourceHandler::GetPosition(const wxString& param)
{
    wxSize sz = GetSize(param);
    return wxPoint(sz.x, sz.y);
}



wxCoord wxXmlResourceHandler::GetDimension(const wxString& param, wxCoord defaultv)
{
    wxString s = GetParamValue(param);
    if (s.IsEmpty()) return defaultv;
    bool is_dlg;
    long sx;

    is_dlg = s[s.Length()-1] == wxT('d');
    if (is_dlg) s.RemoveLast();

    if (!s.ToLong(&sx))
    {
        wxLogError(_("Cannot parse dimension from '%s'."), s.c_str());
        return defaultv;
    }

    if (is_dlg)
    {
        if (m_instanceAsWindow)
            return wxDLG_UNIT(m_instanceAsWindow, wxSize(sx, 0)).x;
        else if (m_parentAsWindow)
            return wxDLG_UNIT(m_parentAsWindow, wxSize(sx, 0)).x;
        else
        {
            wxLogError(_("Cannot convert dialog units: dialog unknown."));
            return defaultv;
        }
    }
    else return sx;
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

    long size = GetLong(wxT("size"), 12);

    wxString style = GetParamValue(wxT("style"));
    wxString weight = GetParamValue(wxT("weight"));
    int istyle = wxNORMAL, iweight = wxNORMAL;
    if (style == wxT("italic")) istyle = wxITALIC;
    else if (style == wxT("slant")) istyle = wxSLANT;
    if (weight == wxT("bold")) iweight = wxBOLD;
    else if (weight == wxT("light")) iweight = wxLIGHT;

    wxString family = GetParamValue(wxT("family"));
    int ifamily = wxDEFAULT;
         if (family == wxT("decorative")) ifamily = wxDECORATIVE;
    else if (family == wxT("roman")) ifamily = wxROMAN;
    else if (family == wxT("script")) ifamily = wxSCRIPT;
    else if (family == wxT("swiss")) ifamily = wxSWISS;
    else if (family == wxT("modern")) ifamily = wxMODERN;

    bool underlined = GetBool(wxT("underlined"), FALSE);

    wxString encoding = GetParamValue(wxT("encoding"));
    wxFontMapper mapper;
    wxFontEncoding enc = wxFONTENCODING_DEFAULT;
    if (!encoding.IsEmpty())
        enc = mapper.CharsetToEncoding(encoding);
    if (enc == wxFONTENCODING_SYSTEM)
        enc = wxFONTENCODING_DEFAULT;

    wxString faces = GetParamValue(wxT("face"));
    wxString facename = wxEmptyString;
    wxFontEnumerator enu;
    enu.EnumerateFacenames();
    wxStringTokenizer tk(faces, wxT(","));
    while (tk.HasMoreTokens())
    {
        int index = enu.GetFacenames()->Index(tk.GetNextToken(), FALSE);
        if (index != wxNOT_FOUND)
        {
            facename = (*enu.GetFacenames())[index];
            break;
        }
    }

    m_node = oldnode;

    wxFont font(size, ifamily, istyle, iweight, underlined, facename, enc);
    return font;
}


void wxXmlResourceHandler::SetupWindow(wxWindow *wnd)
{
    //FIXME : add cursor

    if (HasParam(wxT("exstyle")))
        wnd->SetExtraStyle(GetStyle(wxT("exstyle")));
    if (HasParam(wxT("bg")))
        wnd->SetBackgroundColour(GetColour(wxT("bg")));
    if (HasParam(wxT("fg")))
        wnd->SetForegroundColour(GetColour(wxT("fg")));
    if (GetBool(wxT("enabled"), 1) == 0)
        wnd->Enable(FALSE);
    if (GetBool(wxT("focused"), 0) == 1)
        wnd->SetFocus();
    if (GetBool(wxT("hidden"), 0) == 1)
        wnd->Show(FALSE);
#if wxUSE_TOOLTIPS
    if (HasParam(wxT("tooltip")))
        wnd->SetToolTip(GetText(wxT("tooltip")));
#endif
    if (HasParam(wxT("font")))
        wnd->SetFont(GetFont());
}


void wxXmlResourceHandler::CreateChildren(wxObject *parent, bool this_hnd_only)
{
    wxXmlNode *n = m_node->GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE &&
            n->GetName() == wxT("object"))
        {
            if (this_hnd_only && CanHandle(n))
                CreateResource(n, parent, NULL);
            else
                m_resource->CreateResFromNode(n, parent, NULL);
        }
        n = n->GetNext();
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
    int id;
    wxChar *key;
    XRCID_record *next;
};

static XRCID_record *XRCID_Records[XRCID_TABLE_SIZE] = {NULL};

/*static*/ int wxXmlResource::GetXRCID(const wxChar *str_id)
{
    static int XRCID_LastID = wxID_HIGHEST;

    int index = 0;

    for (const wxChar *c = str_id; *c != wxT('\0'); c++) index += (int)*c;
    index %= XRCID_TABLE_SIZE;

    XRCID_record *oldrec = NULL;
    int matchcnt = 0;
    for (XRCID_record *rec = XRCID_Records[index]; rec; rec = rec->next)
    {
        if (wxStrcmp(rec->key, str_id) == 0)
        {
            return rec->id;
        }
        matchcnt++;
        oldrec = rec;
    }

    XRCID_record **rec_var = (oldrec == NULL) ?
                              &XRCID_Records[index] : &oldrec->next;
    *rec_var = new XRCID_record;
    (*rec_var)->id = ++XRCID_LastID;
    (*rec_var)->key = wxStrdup(str_id);
    (*rec_var)->next = NULL;

    return (*rec_var)->id;
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
        CleanXRCID_Record(XRCID_Records[i]);
}








// --------------- module and globals -----------------------------

class wxXmlResourceModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxXmlResourceModule)
public:
    wxXmlResourceModule() {}
    bool OnInit()
    {
        return TRUE;
    }
    void OnExit()
    {
        delete wxXmlResource::Set(NULL);
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


