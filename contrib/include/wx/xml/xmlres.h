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

class wxMenu;
class wxMenuBar;
class wxDialog;
class wxPanel;
class wxWindow;

class wxXmlResourceHandler;

#include "wx/xml/xml.h"

enum 
{
    wxXML_BINARY,
    wxXML_ARCHIVE
};



class WXDLLEXPORT wxXmlResourceDataRecord
{
    public:
        wxXmlResourceDataRecord() : Doc(NULL), Time(wxDateTime::Now()) {}
        ~wxXmlResourceDataRecord() {delete Doc;}
    
        wxString File;
        wxXmlDocument *Doc;
        wxDateTime Time;
};

WX_DECLARE_OBJARRAY(wxXmlResourceDataRecord, wxXmlResourceDataRecords);

// This class holds XML resources from one or more .xml files 
// (or derived forms, either binary or zipped -- see manual for
// details). 

class WXDLLEXPORT wxXmlResource : public wxObject
{
    public:
        wxXmlResource();
        wxXmlResource(const wxString& filemask, int type);
        ~wxXmlResource();

        // Loads resources from XML files that match given filemask.
        // This method understands VFS (see filesys.h). Type is one of
        // wxXML_TEXT, wxXML_BINARY, wxXML_ARCHIVE and specifies type of
        // data to be expected: 
        //    wxXML_BINARY     - binary version of .xml file, as produced
        //                       by wxXmlDocument::SaveBinary
        //    wxXML_ARCHIVE    - ZIP archive that contains arbitrary number
        //                       of files with .xmb extension
        //                       (this kind of ZIP archive is produced by
        //                       XML resources compiler that ships with wxWin)
        bool Load(const wxString& filemask, int type = wxXML_ARCHIVE);
        
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

        // Returns numeric ID that is equivalent to string id used in XML
        // resource. To be used in event tables
        // Macro XMLID is provided for convenience
        static int GetXMLID(const char *str_id);

    protected:
        // Scans resources list for unloaded files and loads them. Also reloads
        // files that have been modified since last loading.
        void UpdateResources();
        
        // Finds resource (calls UpdateResources) and returns node containing it
        wxXmlNode *FindResource(const wxString& name, const wxString& type);
        
        // Creates resource from info in given node:
        wxObject *CreateResFromNode(wxXmlNode *node, wxObject *parent, wxObject *instance = NULL);

    private:
        wxList m_Handlers;
        wxXmlResourceDataRecords m_Data;
        
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
#define XMLID(str_id) wxXmlResource::GetXMLID(str_id)


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

        // Check "platform" property if it matches this platform
        // that is, if this node 'exists' under this platform
        static bool CheckPlatform(wxXmlNode *node);
        
        void SetParentResource(wxXmlResource *res) { m_Resource = res; }


    protected:

        wxXmlResource *m_Resource;
        wxArrayString m_StyleNames;
        wxArrayInt m_StyleValues;

        // Variables (filled by CreateResource)
        wxXmlNode *m_Node;
        wxObject *m_Parent, *m_Instance;
        wxWindow *m_ParentAsWindow, *m_InstanceAsWindow;
        
        // --- Handy methods:

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
        
        // Gets style flags from text in form "flag | flag2| flag3 |..."
        // Only understads flags added with AddStyle
        int GetStyle(const wxString& param = _T("style"), int defaults = 0);
        
        // Gets text from param and does some convertions:
        // - replaces \n, \r, \t by respective chars (according to C syntax)
        // - replaces $ by & and $$ by $ (needed for $File => &File because of XML)
        // - converts encodings if neccessary
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
        
        wxSize GetSize(const wxString& param = _T("size"));
        wxPoint GetPosition(const wxString& param = _T("pos"));
        
        // Sets common window options:
        void SetupWindow(wxWindow *wnd);
    
        void CreateChildren(wxObject *parent, bool only_this_handler = FALSE,
                          wxXmlNode *children_node = NULL /*stands for
                             GetParamNode("children")*/);
        wxObject *CreateResFromNode(wxXmlNode *node, wxObject *parent, wxObject *instance = NULL)
            { return m_Resource->CreateResFromNode(node, parent, instance); }
};

#define ADD_STYLE(style) AddStyle(_T(#style), style)



#endif // _WX_XMLRES_H_
