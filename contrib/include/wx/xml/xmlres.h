/////////////////////////////////////////////////////////////////////////////
// Name:        xmlres.h
// Purpose:     XML resources
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XMLRES_H_
#define _WX_XMLRES_H_

#ifdef __GNUG__
#pragma interface "xmlres.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/dynarray.h"
#include "wx/datetime.h"
#include "wx/list.h"
#include "wx/gdicmn.h"
#include "wx/filesys.h"
#include "wx/bitmap.h"
#include "wx/icon.h"

class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxDialog;
class WXDLLEXPORT wxPanel;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxToolBar;

class WXDLLEXPORT wxXmlResourceHandler;

#include "wx/xml/xml.h"


class WXDLLEXPORT wxXmlResourceDataRecord
{
    public:
        wxXmlResourceDataRecord() : Doc(NULL), Time(wxDateTime::Now()) {}
        ~wxXmlResourceDataRecord() {delete Doc;}
    
        wxString File;
        wxXmlDocument *Doc;
        wxDateTime Time;
};

WX_DECLARE_EXPORTED_OBJARRAY(wxXmlResourceDataRecord, wxXmlResourceDataRecords);

// This class holds XML resources from one or more .xml files 
// (or derived forms, either binary or zipped -- see manual for
// details). 

class WXDLLEXPORT wxXmlResource : public wxObject
{
    public:
        // Ctor. If use_locale is TRUE, translatable strings are
        // translated via _(). You can disable it by passing use_locale=FALSE
        // (for example if you provide resource file for each locale)
        wxXmlResource(bool use_locale = TRUE);
        wxXmlResource(const wxString& filemask, bool use_locale = TRUE);
        ~wxXmlResource();

        // Loads resources from XML files that match given filemask.
        // This method understands VFS (see filesys.h).
        bool Load(const wxString& filemask);
        
        // Initialize handlers for all supported controls/windows. This will
        // make the executable quite big because it forces linking against
        // most of wxWin library
        void InitAllHandlers();
        
        // Initialize only specific handler (or custom handler). Convention says
        // that handler name is equal to control's name plus 'XmlHandler', e.g.
        // wxTextCtrlXmlHandler, wxHtmlWindowXmlHandler. XML resource compiler
        // (xmlres) can create include file that contains initialization code for 
        // all controls used within the resource.
        void AddHandler(wxXmlResourceHandler *handler);
        
        // Removes all handlers
        void ClearHandlers();
        
        // Loads menu from resource. Returns NULL on failure.
        wxMenu *LoadMenu(const wxString& name);

        // Loads menubar from resource. Returns NULL on failure.
        wxMenuBar *LoadMenuBar(const wxString& name);

#if wxUSE_TOOLBAR
        // Loads toolbar
        wxToolBar *LoadToolBar(wxWindow *parent, const wxString& name);
#endif

        // Loads dialog. dlg points to parent window (if any). Second form
        // is used to finish creation of already existing instance (main reason
        // for this is that you may want to use derived class with new event table)
        // Example (typical usage):
        //      MyDialog dlg;
        //      wxTheXmlResource->LoadDialog(&dlg, mainFrame, "my_dialog");
        //      dlg->ShowModal();
        wxDialog *LoadDialog(wxWindow *parent, const wxString& name);
        bool LoadDialog(wxDialog *dlg, wxWindow *parent, const wxString& name);

        // Loads panel. panel points to parent window (if any). Second form
        // is used to finish creation of already existing instance.
        wxPanel *LoadPanel(wxWindow *parent, const wxString& name);
        bool LoadPanel(wxPanel *panel, wxWindow *parent, const wxString& name);

        bool LoadFrame(wxFrame* frame, wxWindow *parent, const wxString& name);

        // Loads bitmap or icon resource from file:
        wxBitmap LoadBitmap(const wxString& name);
        wxIcon LoadIcon(const wxString& name);

        // Returns numeric ID that is equivalent to string id used in XML
        // resource. To be used in event tables
        // Macro XMLID is provided for convenience
        static int GetXMLID(const char *str_id);

    protected:
        // Scans resources list for unloaded files and loads them. Also reloads
        // files that have been modified since last loading.
        void UpdateResources();
        
        // Finds resource (calls UpdateResources) and returns node containing it
        wxXmlNode *FindResource(const wxString& name, const wxString& classname);
        
        // Creates resource from info in given node:
        wxObject *CreateResFromNode(wxXmlNode *node, wxObject *parent, wxObject *instance = NULL);

        // Remove nodes with property "platform" that does not
        // match current platform
        void ProcessPlatformProperty(wxXmlNode *node);
        
        bool GetUseLocale() { return m_UseLocale; }

    private:
        bool m_UseLocale;
        wxList m_Handlers;
        wxXmlResourceDataRecords m_Data;
#if wxUSE_FILESYSTEM
        wxFileSystem m_CurFileSystem;
        wxFileSystem& GetCurFileSystem() { return m_CurFileSystem; }
#endif

        friend class wxXmlResourceHandler;
};


// Global instance of resource class. For your convenience.
extern wxXmlResource *wxTheXmlResource;

// This macro translates string identifier (as used in XML resource,
// e.g. <menuitem id="my_menu">...</menuitem>) to integer id that is needed by
// wxWindows event tables.
// Example:
//    BEGIN_EVENT_TABLE(MyFrame, wxFrame)
//       EVT_MENU(XMLID("quit"), MyFrame::OnQuit)
//       EVT_MENU(XMLID("about"), MyFrame::OnAbout)
//       EVT_MENU(XMLID("new"), MyFrame::OnNew)
//       EVT_MENU(XMLID("open"), MyFrame::OnOpen)
//    END_EVENT_TABLE()    

#define XMLID(str_id) \
    wxXmlResource::GetXMLID(wxT(str_id))


// This macro returns pointer to particular control in dialog
// created using XML resources. You can use it to set/get values from
// controls.
// Example:
//    wxDialog dlg;
//    wxTheXmlResource->LoadDialog(&dlg, mainFrame, "my_dialog");
//    XMLCTRL(dlg, "my_textctrl", wxTextCtrl)->SetValue(wxT("default value"));

#define XMLCTRL(window, id, type) \
    ((type*)((window).FindWindow(XMLID(id))))



class WXDLLEXPORT wxXmlResourceHandler : public wxObject
{
    public:
        wxXmlResourceHandler();
        virtual ~wxXmlResourceHandler() {}

        // Creates object (menu, dialog, control, ...) from XML node.
        // Should check for validity.
        // parent is higher-level object (usually window, dialog or panel)
        // that is often neccessary to create resource
        // if instance != NULL it should not create new instance via 'new' but
        // rather use this one and call its Create method
        wxObject *CreateResource(wxXmlNode *node, wxObject *parent, wxObject *instance);

        // This one is called from CreateResource after variables
        // were filled
        virtual wxObject *DoCreateResource() = 0;
        
        // Returns TRUE if it understands this node and can create
        // resource from it, FALSE otherwise.
        virtual bool CanHandle(wxXmlNode *node) = 0;

        void SetParentResource(wxXmlResource *res) { m_Resource = res; }


    protected:

        wxXmlResource *m_Resource;
        wxArrayString m_StyleNames;
        wxArrayInt m_StyleValues;

        // Variables (filled by CreateResource)
        wxXmlNode *m_Node;
        wxString m_Class;
        wxObject *m_Parent, *m_Instance;
        wxWindow *m_ParentAsWindow, *m_InstanceAsWindow;
        
        // --- Handy methods:

        // Returns true if the node has property class equal to classname,
        // e.g. <object class="wxDialog">
        bool IsOfClass(wxXmlNode *node, const wxString& classname)
            { return node->GetPropVal(wxT("class"), wxEmptyString) == classname; }

        // Gets node content from wxXML_ENTITY_NODE
        // (the problem is, <tag>content<tag> is represented as
        // wxXML_ENTITY_NODE name="tag", content=""
        //    |-- wxXML_TEXT_NODE or 
        //        wxXML_CDATA_SECTION_NODE name="" content="content"
        wxString GetNodeContent(wxXmlNode *node);

        // Check to see if a param exists
        bool HasParam(const wxString& param);      

        // Finds the node or returns NULL
        wxXmlNode *GetParamNode(const wxString& param);
        wxString GetParamValue(const wxString& param);
            
        // Add style flag (e.g. wxMB_DOCKABLE) to list of flags
        // understood by this handler
        void AddStyle(const wxString& name, int value);
	
        // Add styles common to all wxWindow-derived classes
        void AddWindowStyles();
        
        // Gets style flags from text in form "flag | flag2| flag3 |..."
        // Only understads flags added with AddStyle
        int GetStyle(const wxString& param = wxT("style"), int defaults = 0);
        
        // Gets text from param and does some convertions:
        // - replaces \n, \r, \t by respective chars (according to C syntax)
        // - replaces $ by & and $$ by $ (needed for $File => &File because of XML)
        // - calls wxGetTranslations (unless disabled in wxXmlResource)
        wxString GetText(const wxString& param);

        // Return XMLID
        int GetID();
        wxString GetName();

        // Get bool flag (1,t,yes,on,true are TRUE, everything else is FALSE)
        bool GetBool(const wxString& param, bool defaultv = FALSE);

        // Get integer value from param
        long GetLong( const wxString& param, long defaultv = 0 );
        
        // Get colour in HTML syntax (#RRGGBB)
        wxColour GetColour(const wxString& param);
        
        // Get size/position (may be in dlg units):
        wxSize GetSize(const wxString& param = wxT("size"));
        wxPoint GetPosition(const wxString& param = wxT("pos"));

        // Get dimension (may be in dlg units):
        wxCoord GetDimension(const wxString& param, wxCoord defaultv = 0);
        
        // Get bitmap:
        wxBitmap GetBitmap(const wxString& param = wxT("bitmap"), wxSize size = wxDefaultSize);
        wxIcon GetIcon(const wxString& param = wxT("icon"), wxSize size = wxDefaultSize);
        
        // Get font:
        wxFont GetFont(const wxString& param = wxT("font"));
        
        // Sets common window options:
        void SetupWindow(wxWindow *wnd);
    
        void CreateChildren(wxObject *parent, bool this_hnd_only = FALSE);
        void CreateChildrenPrivately(wxObject *parent, wxXmlNode *rootnode = NULL);
        wxObject *CreateResFromNode(wxXmlNode *node, wxObject *parent, wxObject *instance = NULL)
            { return m_Resource->CreateResFromNode(node, parent, instance); }

        // helper
        wxFileSystem& GetCurFileSystem() { return m_Resource->GetCurFileSystem(); }
};

#define ADD_STYLE(style) AddStyle(wxT(#style), style)



#endif // _WX_XMLRES_H_
