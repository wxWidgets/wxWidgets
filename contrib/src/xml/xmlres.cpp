/////////////////////////////////////////////////////////////////////////////
// Name:        xmlres.cpp
// Purpose:     XML resources
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

#include "wx/xml/xml.h"
#include "wx/xml/xmlres.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxXmlResourceDataRecords);


wxXmlResource::wxXmlResource(bool use_locale)
{
    m_Handlers.DeleteContents(TRUE);
    m_UseLocale = use_locale;
}
    
wxXmlResource::wxXmlResource(const wxString& filemask, bool use_locale)
{
    m_UseLocale = use_locale;
    m_Handlers.DeleteContents(TRUE);
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
        if (filemask.Lower().Matches("*.zip") ||
            filemask.Lower().Matches("*.rsc"))
        {
            wxFileSystem fs2;
            wxString fnd2;
            
            fnd2 = fs2.FindFirst(fnd + wxT("#zip:*.xmb"), wxFILE);
            while (!!fnd2)
            {
                drec = new wxXmlResourceDataRecord;
                drec->File = fnd2;
                m_Data.Add(drec);
                fnd2 = fs2.FindNext();
            }
        }
        else
#endif
        {
            drec = new wxXmlResourceDataRecord;
            drec->File = fnd;
            m_Data.Add(drec);
        }

        if (iswild)
            fnd = wxXmlFindNext;
        else
            fnd = wxEmptyString;
    }
#   undef wxXmlFindFirst
#   undef wxXmlFindNext
    return TRUE;
}



void wxXmlResource::AddHandler(wxXmlResourceHandler *handler)
{
    m_Handlers.Append(handler);
    handler->SetParentResource(this);
}



void wxXmlResource::ClearHandlers()
{
    m_Handlers.Clear();
}



wxMenu *wxXmlResource::LoadMenu(const wxString& name)
{
    return (wxMenu*)CreateResFromNode(FindResource(name, wxT("wxMenu")), NULL, NULL);
}



wxMenuBar *wxXmlResource::LoadMenuBar(const wxString& name)
{
    return (wxMenuBar*)CreateResFromNode(FindResource(name, wxT("wxMenuBar")), NULL, NULL);
}



wxToolBar *wxXmlResource::LoadToolBar(wxWindow *parent, const wxString& name)
{
    return (wxToolBar*)CreateResFromNode(FindResource(name, wxT("wxToolBar")), parent, NULL);
}



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



void wxXmlResource::ProcessPlatformProperty(wxXmlNode *node)
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
            ProcessPlatformProperty(c);
        else
        {
            node->RemoveChild(c);
            delete c;
        }
        
        c = c->GetNext();
    }
}



void wxXmlResource::UpdateResources()
{
    bool modif;
#   if wxUSE_FILESYSTEM
    wxFSFile *file;
    wxFileSystem fsys;
#   endif

    for (size_t i = 0; i < m_Data.GetCount(); i++)
    {
        modif = (m_Data[i].Doc == NULL);

        if (!modif)
        {
#           if wxUSE_FILESYSTEM
            file = fsys.OpenFile(m_Data[i].File);
            modif = file && file->GetModificationTime() > m_Data[i].Time;
            if (!file)
                wxLogError(_("Cannot open file '%s'."), m_Data[i].File.c_str());
            delete file;
#           else
            modif = wxDateTime(wxFileModificationTime(m_Data[i].File)) > m_Data[i].Time;
#           endif
        }
        
        if (modif)
        {
            wxInputStream *stream;

#           if wxUSE_FILESYSTEM
            file = fsys.OpenFile(m_Data[i].File);
            stream = file->GetStream();
#           else
            stream = new wxFileInputStream(m_Data[i].File);
#           endif
            
            if (stream) 
            {
                delete m_Data[i].Doc;
                m_Data[i].Doc = new wxXmlDocument;
            }
            if (!stream || !m_Data[i].Doc->Load(*stream))
            {
                wxLogError(_("Cannot load resources from file '%s'."), m_Data[i].File.c_str());
                delete m_Data[i].Doc;
                m_Data[i].Doc = NULL;
            }
            else if (m_Data[i].Doc->GetRoot()->GetName() != wxT("resource")) 
            {
                wxLogError(_("Invalid XML resource '%s': doesn't have root node 'resource'."), m_Data[i].File.c_str());
                delete m_Data[i].Doc;
                m_Data[i].Doc = NULL;
            }
            else
                ProcessPlatformProperty(m_Data[i].Doc->GetRoot());

#           if wxUSE_FILESYSTEM
            delete file;
#           else
            delete stream;
#           endif
        }
    }
}



wxXmlNode *wxXmlResource::FindResource(const wxString& name, const wxString& classname)
{
    UpdateResources(); //ensure everything is up-to-date
    
    wxString dummy;   
    for (size_t f = 0; f < m_Data.GetCount(); f++)
    {
        if (m_Data[f].Doc == NULL || m_Data[f].Doc->GetRoot() == NULL) continue;
        for (wxXmlNode *node = m_Data[f].Doc->GetRoot()->GetChildren(); 
                                      node; node = node->GetNext())
            if (node->GetType() == wxXML_ELEMENT_NODE &&
                (!classname || 
                  node->GetPropVal(wxT("class"), wxEmptyString) == classname) &&
                node->GetName() == wxT("object") &&
                node->GetPropVal(wxT("name"), &dummy) &&
                dummy == name)
            {
#if wxUSE_FILESYSTEM
                m_CurFileSystem.ChangePathTo(m_Data[f].File);
#endif
                return node;
            }
    }

    wxLogError(_("XML resource '%s' (class '%s') not found!"), 
               name.c_str(), classname.c_str());
    return NULL;
}



wxObject *wxXmlResource::CreateResFromNode(wxXmlNode *node, wxObject *parent, wxObject *instance)
{
    if (node == NULL) return NULL;
    
    wxXmlResourceHandler *handler;
    wxObject *ret;
    wxNode * ND = m_Handlers.GetFirst();
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
        : m_Node(NULL), m_Parent(NULL), m_Instance(NULL), 
          m_ParentAsWindow(NULL), m_InstanceAsWindow(NULL)
{}



wxObject *wxXmlResourceHandler::CreateResource(wxXmlNode *node, wxObject *parent, wxObject *instance)
{
    wxXmlNode *myNode = m_Node;
    wxString myClass = m_Class;
    wxObject *myParent = m_Parent, *myInstance = m_Instance;
    wxWindow *myParentAW = m_ParentAsWindow, *myInstanceAW = m_InstanceAsWindow;
    
    m_Node = node;
    m_Class = node->GetPropVal(wxT("class"), wxEmptyString);
    m_Parent = parent;
    m_Instance = instance;
    m_ParentAsWindow = wxDynamicCast(m_Parent, wxWindow);
    m_InstanceAsWindow = wxDynamicCast(m_Instance, wxWindow);
    
    wxObject *returned = DoCreateResource();
    
    m_Node = myNode;
    m_Class = myClass;
    m_Parent = myParent; m_ParentAsWindow = myParentAW;
    m_Instance = myInstance; m_InstanceAsWindow = myInstanceAW;
    
    return returned;
}


void wxXmlResourceHandler::AddStyle(const wxString& name, int value)
{
    m_StyleNames.Add(name);
    m_StyleValues.Add(value);
}



void wxXmlResourceHandler::AddWindowStyles()
{
    ADD_STYLE(wxSIMPLE_BORDER);
    ADD_STYLE(wxSUNKEN_BORDER);
    ADD_STYLE(wxDOUBLE_BORDER);
    ADD_STYLE(wxRAISED_BORDER);
    ADD_STYLE(wxSTATIC_BORDER);
    ADD_STYLE(wxTRANSPARENT_WINDOW);
    ADD_STYLE(wxWANTS_CHARS);
    ADD_STYLE(wxNO_FULL_REPAINT_ON_RESIZE);
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
        index = m_StyleNames.Index(fl);
        if (index != wxNOT_FOUND)
            style |= m_StyleValues[index];
        else
            wxLogError(_("Unknown style flag ") + fl);
    }
    return style;
}



wxString wxXmlResourceHandler::GetText(const wxString& param)
{
    wxString str1 = GetParamValue(param);
    wxString str2;
    const wxChar *dt;

    for (dt = str1.c_str(); *dt; dt++)
    {
        // Remap $ to &, map $$ to $ (for things like "&File..." -- 
        // this is illegal in XML, so we use "$File..."):
        if (*dt == '$')
            switch (*(++dt))
            {
                case '$' : str2 << '$'; break;
                default  : str2 << '&' << *dt; break;
            }
        // Remap \n to CR, \r LF, \t to TAB:
        else if (*dt == '\\')
            switch (*(++dt))
            {
                case 'n' : str2 << '\n'; break;
                case 't' : str2 << '\t'; break;
                case 'r' : str2 << '\r'; break;
                default  : str2 << '\\' << *dt; break;
            }
        else str2 << *dt;
    }
    
    if (m_Resource->GetUseLocale())
        return wxGetTranslation(str2);
    else
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
    else return XMLID(sid.c_str());
}


wxString wxXmlResourceHandler::GetName()
{
    return m_Node->GetPropVal(wxT("name"), wxT("-1"));
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
        wxLogError(_("XML resource: Incorrect colour specification '%s' for property '%s'."),
                   v.c_str(), param.c_str());
        return wxNullColour;
    }
    
    return wxColour((tmp & 0xFF0000) >> 16 , 
                    (tmp & 0x00FF00) >> 8, 
                    (tmp & 0x0000FF));
}



wxBitmap wxXmlResourceHandler::GetBitmap(const wxString& param, wxSize size)
{
    wxString name = GetParamValue(param);
    if (name.IsEmpty()) return wxNullBitmap;
#if wxUSE_FILESYSTEM
    wxFSFile *fsfile = GetCurFileSystem().OpenFile(name);
    if (fsfile == NULL)
    {
        wxLogError(_("XML resource: Cannot create bitmap from '%s'."), param.mb_str());
        return wxNullBitmap;
    }    
    wxImage img(*(fsfile->GetStream()));
    delete fsfile;
#else
    wxImage img(GetParamValue(wxT("bitmap")));
#endif
    if (!img.Ok()) 
    {
        wxLogError(_("XML resource: Cannot create bitmap from '%s'."), param.mb_str());
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
    wxXmlNode *n = m_Node->GetChildren();
    
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
        return GetNodeContent(m_Node);
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
        wxLogError(_("Cannot parse coordinates from '%s'."), s.mb_str());
        return wxDefaultSize;
    }
    
    if (is_dlg)
    {
        if (m_InstanceAsWindow)
            return wxDLG_UNIT(m_InstanceAsWindow, wxSize(sx, sy));
        else if (m_ParentAsWindow)
            return wxDLG_UNIT(m_ParentAsWindow, wxSize(sx, sy));
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
        wxLogError(_("Cannot parse dimension from '%s'."), s.mb_str());
        return defaultv;
    }
    
    if (is_dlg)
    {
        if (m_InstanceAsWindow)
            return wxDLG_UNIT(m_InstanceAsWindow, wxSize(sx, 0)).x;
        else if (m_ParentAsWindow)
            return wxDLG_UNIT(m_ParentAsWindow, wxSize(sx, 0)).x;
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
        wxLogError(_("Cannot find font node '%s'."), param.mb_str());
        return wxNullFont;
    }
    
    wxXmlNode *oldnode = m_Node;
    m_Node = font_node;

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
    if (!encoding.IsEmpty()) enc = mapper.CharsetToEncoding(encoding);
    if (enc == wxFONTENCODING_SYSTEM) enc = wxFONTENCODING_SYSTEM;

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
    
    m_Node = oldnode;
    
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
    wxXmlNode *n = m_Node->GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE &&
            n->GetName() == wxT("object"))
        {        
            if (this_hnd_only && CanHandle(n))
                CreateResource(n, parent, NULL);
            else
                m_Resource->CreateResFromNode(n, parent, NULL);
        }
        n = n->GetNext();
    }
}


void wxXmlResourceHandler::CreateChildrenPrivately(wxObject *parent, wxXmlNode *rootnode)
{
    wxXmlNode *root;
    if (rootnode == NULL) root = m_Node; else root = rootnode;
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









// --------------- XMLID implementation -----------------------------

#define XMLID_TABLE_SIZE     1024


struct XMLID_record
{
    int id;
    char *key;
    XMLID_record *next;
};

static XMLID_record *XMLID_Records[XMLID_TABLE_SIZE] = {NULL};

/*static*/ int wxXmlResource::GetXMLID(const char *str_id)
{
    static int XMLID_LastID = wxID_HIGHEST;
    
    int index = 0;
    
    for (const char *c = str_id; *c != '\0'; c++) index += (int)*c;
    index %= XMLID_TABLE_SIZE;
    
    XMLID_record *oldrec = NULL;
    int matchcnt = 0;
    for (XMLID_record *rec = XMLID_Records[index]; rec; rec = rec->next)
    {
        if (strcmp(rec->key, str_id) == 0)
        {
            return rec->id;
        }
        matchcnt++;
        oldrec = rec;
    }
    
    XMLID_record **rec_var = (oldrec == NULL) ? 
                              &XMLID_Records[index] : &oldrec->next;
    *rec_var = new XMLID_record;
    (*rec_var)->id = ++XMLID_LastID;
    (*rec_var)->key = strdup(str_id);
    (*rec_var)->next = NULL;
    
    return (*rec_var)->id;
}


static void CleanXMLID_Record(XMLID_record *rec)
{
    if (rec)
    {
        CleanXMLID_Record(rec->next);
        free (rec->key);
        delete rec;
    }
}

static void CleanXMLID_Records()
{
    for (int i = 0; i < XMLID_TABLE_SIZE; i++)
        CleanXMLID_Record(XMLID_Records[i]);
}








// --------------- module and globals -----------------------------


static wxXmlResource gs_XmlResource;

wxXmlResource *wxTheXmlResource = &gs_XmlResource;


class wxXmlResourceModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxXmlResourceModule)
public:
    wxXmlResourceModule() {}
    bool OnInit() {return TRUE;}
    void OnExit() 
    {
        wxTheXmlResource->ClearHandlers();
        CleanXMLID_Records();
    }
};

IMPLEMENT_DYNAMIC_CLASS(wxXmlResourceModule, wxModule)
