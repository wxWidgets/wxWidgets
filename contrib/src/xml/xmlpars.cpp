/////////////////////////////////////////////////////////////////////////////
// Name:        xmlpars.cpp
// Purpose:     wxXmlDocument - XML parser 
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
// nothing - already in xml.cpp
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wfstream.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/dynlib.h"
#include "wx/xml/xmlio.h"

#include <libxml/parser.h>

// dynamically loaded functions from libxml:
typedef xmlParserCtxtPtr (*type_xmlCreatePushParserCtxt)
             (xmlSAXHandlerPtr sax, void *, const char *, int, const char *);
typedef xmlNodePtr (*type_xmlNewText)(const xmlChar *);
typedef xmlAttrPtr (*type_xmlSetProp)(xmlNodePtr, const xmlChar *, const xmlChar *);
typedef int (*type_xmlParseChunk)(xmlParserCtxtPtr, const char *, int, int);
typedef void (*type_xmlFreeParserCtxt)(xmlParserCtxtPtr);
typedef xmlDocPtr (*type_xmlNewDoc)(const xmlChar *);
typedef void (*type_xmlFreeDoc)(xmlDocPtr);
typedef xmlNodePtr (*type_xmlNewDocNode)(xmlDocPtr, xmlNsPtr, const xmlChar *, const xmlChar *);
typedef void (*type_xmlDocDumpMemory)(xmlDocPtr, xmlChar**, int *);
typedef xmlNodePtr (*type_xmlAddChild)(xmlNodePtr, xmlNodePtr);
typedef xmlNodePtr (*type_xmlNewChild)(xmlNodePtr, xmlNsPtr, const xmlChar *, const xmlChar *);
typedef xmlChar * (*type_xmlNodeListGetString)(xmlDocPtr, xmlNodePtr, int);
typedef xmlNodePtr (*type_xmlDocGetRootElement)(xmlDocPtr);
typedef xmlNodePtr (*type_xmlDocSetRootElement)(xmlDocPtr doc, xmlNodePtr root);
typedef void (*(*type_xmlFree))(void *);
typedef int (*type_xmlKeepBlanksDefault)(int);

static struct
{
    wxDllType Handle;

    type_xmlCreatePushParserCtxt xmlCreatePushParserCtxt;
    type_xmlNewText xmlNewText;
    type_xmlSetProp xmlSetProp;
    type_xmlParseChunk xmlParseChunk;
    type_xmlFreeParserCtxt xmlFreeParserCtxt;
    type_xmlNewDoc xmlNewDoc;
    type_xmlFreeDoc xmlFreeDoc;
    type_xmlNewDocNode xmlNewDocNode;
    type_xmlDocDumpMemory xmlDocDumpMemory;
    type_xmlAddChild xmlAddChild;
    type_xmlNewChild xmlNewChild;
    type_xmlNodeListGetString xmlNodeListGetString;
    type_xmlDocGetRootElement xmlDocGetRootElement;
    type_xmlDocSetRootElement xmlDocSetRootElement;
    type_xmlFree xmlFree;
    type_xmlKeepBlanksDefault xmlKeepBlanksDefault;
} gs_libxmlDLL;

static bool gs_libxmlLoaded = FALSE;
static bool gs_libxmlLoadFailed = FALSE;



static void ReleaseLibxml()
{
    if (gs_libxmlLoaded)
    {
        wxLogDebug("Releasing libxml.so.2");
        wxDllLoader::UnloadLibrary(gs_libxmlDLL.Handle);
    }
    gs_libxmlLoaded = FALSE;
    gs_libxmlLoadFailed = FALSE;
}


static bool LoadLibxml()
{
    if (gs_libxmlLoaded) return TRUE;
    if (gs_libxmlLoadFailed) return FALSE;
    gs_libxmlLoadFailed = TRUE;

    wxLogDebug("Loading libxml.so.2...");
    {
    wxLogNull lg;
#ifdef __UNIX__
    gs_libxmlDLL.Handle = 
        wxDllLoader::LoadLibrary(_T("wxlibxml.so.2"), &gs_libxmlLoaded);
    if (!gs_libxmlLoaded) gs_libxmlDLL.Handle = 
        wxDllLoader::LoadLibrary(_T("libxml.so.2"), &gs_libxmlLoaded);
#endif
#ifdef __WXMSW__
    gs_libxmlDLL.Handle = 
        wxDllLoader::LoadLibrary(_T("wxlibxml2.dll"), &gs_libxmlLoaded);
    if (!gs_libxmlLoaded) gs_libxmlDLL.Handle = 
        wxDllLoader::LoadLibrary(_T("libxml2.dll"), &gs_libxmlLoaded);
#endif
    }
    
    if (!gs_libxmlLoaded) 
    {
        wxLogError(_("Failed to load libxml shared library."));
        return FALSE;
    }
    
#define LOAD_SYMBOL(sym) \
    gs_libxmlDLL.sym = \
        (type_##sym)wxDllLoader::GetSymbol(gs_libxmlDLL.Handle, _T(#sym)); \
    if (!gs_libxmlDLL.sym) \
    { \
        ReleaseLibxml(); \
        wxLogError(_("Failed to load libxml shared library.")); \
        return FALSE; \
    }

    LOAD_SYMBOL(xmlCreatePushParserCtxt)
    LOAD_SYMBOL(xmlNewText)
    LOAD_SYMBOL(xmlSetProp)
    LOAD_SYMBOL(xmlParseChunk)
    LOAD_SYMBOL(xmlFreeParserCtxt)
    LOAD_SYMBOL(xmlNewDoc)
    LOAD_SYMBOL(xmlFreeDoc)
    LOAD_SYMBOL(xmlNewDocNode)
    LOAD_SYMBOL(xmlDocDumpMemory)
    LOAD_SYMBOL(xmlAddChild)
    LOAD_SYMBOL(xmlNewChild)
    LOAD_SYMBOL(xmlNodeListGetString)
    LOAD_SYMBOL(xmlDocGetRootElement)
    LOAD_SYMBOL(xmlDocSetRootElement)
    LOAD_SYMBOL(xmlFree)
    LOAD_SYMBOL(xmlKeepBlanksDefault)

#undef LOAD_SYMBOL    

    gs_libxmlLoadFailed = FALSE;

    wxLogDebug("...succeed");
    return TRUE;
}




bool wxXmlIOHandlerLibxml::CanLoad(wxInputStream& stream)
{
    if (!LoadLibxml()) return FALSE;
    char cheader[7];
    cheader[6] = 0;
    stream.Read(cheader, 6);
    stream.SeekI(-6, wxFromCurrent);
    return strcmp(cheader, "<?xml ") == 0;
}



bool wxXmlIOHandlerLibxml::CanSave()
{
    return LoadLibxml();
}



static wxXmlProperty *CreateWXProperty(xmlDocPtr doc, xmlAttrPtr attr)
{
    if (attr == NULL) return NULL;
    
    unsigned char *val = 
        gs_libxmlDLL.xmlNodeListGetString(doc, attr->children, 1);
    wxXmlProperty *prop = 
        new wxXmlProperty(attr->name, val, CreateWXProperty(doc, attr->next));
    (*gs_libxmlDLL.xmlFree)(val);
    return prop;
}



static wxXmlNode *CreateWXNode(xmlDocPtr doc, wxXmlNode *parent, xmlNodePtr node)
{
    if (node == NULL) return NULL;
    
    wxXmlNode *nd = new wxXmlNode(parent, (wxXmlNodeType)node->type,
                          node->name, node->content, 
                          CreateWXProperty(doc, node->properties),
                          CreateWXNode(doc, parent, node->next));
    CreateWXNode(doc, nd, node->children);
    return nd;
}



bool wxXmlIOHandlerLibxml::Load(wxInputStream& stream, wxXmlDocument& doc)
{
    if (!LoadLibxml()) return FALSE;

    xmlDocPtr dc;
    xmlParserCtxtPtr ctxt;
    
    char buffer[1024];
    int  res;
    
    res = stream.Read(buffer, 4).LastRead();
    if (res > 0)
    {
        bool okay = TRUE;
        gs_libxmlDLL.xmlKeepBlanksDefault(0);
        ctxt = gs_libxmlDLL.xmlCreatePushParserCtxt(NULL, NULL, 
                                               buffer, res, ""/*docname*/);
        while ((res = stream.Read(buffer, 1024).LastRead()) > 0) 
            if (gs_libxmlDLL.xmlParseChunk(ctxt, buffer, res, 0) != 0) 
               okay = FALSE;
        if (gs_libxmlDLL.xmlParseChunk(ctxt, buffer, 0, 1) != 0) okay = FALSE;
        dc = ctxt->myDoc;
        gs_libxmlDLL.xmlFreeParserCtxt(ctxt);

        doc.SetVersion(dc->version);
        doc.SetEncoding(dc->encoding);
        doc.SetRoot(CreateWXNode(dc, NULL, gs_libxmlDLL.xmlDocGetRootElement(dc)));

        gs_libxmlDLL.xmlFreeDoc(dc);
        
        return okay;
    }
    else return FALSE;
}



static void CreateLibxmlNode(xmlNodePtr node, wxXmlNode *wxnode)
{
    node->type = (xmlElementType)wxnode->GetType();

    wxXmlProperty *prop = wxnode->GetProperties();  
    while (prop)
    {
        gs_libxmlDLL.xmlSetProp(node, (xmlChar*)prop->GetName().mb_str(), 
                         (xmlChar*)prop->GetValue().mb_str());
        prop = prop->GetNext();
    }
    
    wxXmlNode *child = wxnode->GetChildren();
    xmlNodePtr n;
    xmlChar *content, *name;
    
    while (child)
    {
        name = (xmlChar*)child->GetName().mb_str();
        if (!child->GetContent()) content = NULL;
        else content = (xmlChar*)child->GetContent().mb_str();
        if (child->GetType() == wxXML_TEXT_NODE)
            gs_libxmlDLL.xmlAddChild(node, n = gs_libxmlDLL.xmlNewText(content));
        else
            n = gs_libxmlDLL.xmlNewChild(node, NULL, name, content);
        CreateLibxmlNode(n, child);
        child = child->GetNext();
    }
}



bool wxXmlIOHandlerLibxml::Save(wxOutputStream& stream, const wxXmlDocument& doc)
{
    if (!LoadLibxml()) return FALSE;

    xmlDocPtr dc;

    wxASSERT_MSG(doc.GetRoot() != NULL, _("Trying to save empty document!"));
    
    gs_libxmlDLL.xmlKeepBlanksDefault(0);
    dc = gs_libxmlDLL.xmlNewDoc((xmlChar*)doc.GetVersion().mb_str());
    
    gs_libxmlDLL.xmlDocSetRootElement(dc, 
         gs_libxmlDLL.xmlNewDocNode(dc, NULL, 
                        (xmlChar*)doc.GetRoot()->GetName().mb_str(), NULL));
    CreateLibxmlNode(gs_libxmlDLL.xmlDocGetRootElement(dc), doc.GetRoot());

    xmlChar *buffer;
    int size;

    gs_libxmlDLL.xmlDocDumpMemory(dc, &buffer, &size);
    gs_libxmlDLL.xmlFreeDoc(dc);
    stream.Write(buffer, size);
    (*gs_libxmlDLL.xmlFree)(buffer);
    return stream.LastWrite() == (unsigned)size;
}




#include "wx/module.h"

class wxXmlLibxmlModule: public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxXmlLibxmlModule)
    public:
        wxXmlLibxmlModule() {}
        bool OnInit() { return TRUE; }
        void OnExit() { ReleaseLibxml(); }
};

IMPLEMENT_DYNAMIC_CLASS(wxXmlLibxmlModule, wxModule)
