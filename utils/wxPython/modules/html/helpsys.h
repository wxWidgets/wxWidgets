/* htmlsys.h : wxHtmlHelpSystem is an extension of the wxHtmlHelpController.
 * mainly does two things:
 * - extend the interface somewhat so the programmer can dictate most of the
 * look and feel of the htmlhelp frame.
 * - make some protected functions public (adding _ to the function name) so
 * that SWIG can wrap them.
 *
 * Harm van der Heijden 32aug1999
 */
#ifndef __HELPSYS_H__
#define __HELPSYS_H__

#include <wx/defs.h>

#if ! wxUSE_HTML
#error "wxHtml needed"
#endif

#include <wx/toolbar.h>
#include "wx/listbox.h"
#include <wx/html/htmlhelp.h>

class wxHtmlHelpSystem : public wxHtmlHelpController
{
 DECLARE_DYNAMIC_CLASS(wxHtmlHelpSystem)

 public:
   wxHtmlHelpSystem() {};
   ~wxHtmlHelpSystem() {};
   
   bool AddBookParam(const wxString& title, const wxString& contfile, 
		     const wxString& indexfile=wxEmptyString, const wxString& deftopic=wxEmptyString, 
		     const wxString& path=wxEmptyString, bool show_wait_msg=FALSE);
   // Alternative to AddBook(wxString& hhpfile)
   wxToolBar* CreateToolBar(wxFrame* frame);
   // creates a dockable toolbar for the frame, containing hide/show, back and forward buttons
   wxTreeCtrl* CreateContentsTree(wxWindow* parent);
   // creates a treecontrol with imagelist for books, folders etc and id wxID_HTML_TREECTRL
   wxListBox* CreateIndexList(wxWindow* parent);
   // creates a listbox with the right id
   virtual void CreateHelpWindow();
   // Slightly different version than in wxHtmlHelpController; uses helpers above
   // Do nothing if the window already exists
   void SetControls(wxFrame* frame, wxHtmlWindow* htmlwin, 
		    wxTreeCtrl* contents=NULL, wxListBox* index=NULL,
		    wxListBox* searchlist=NULL);
   // alternative for CreateHelpWindow(), sets frame, htmlwindow, contents tree, index
   // listbox and searchlist listbox. If null, their functionality won't be used

   // Some extra accessor functions
   wxFrame* GetFrame() { return m_Frame; }
   wxHtmlWindow* GetHtmlWindow() { return m_HtmlWin; }
   wxTreeCtrl* GetContentsTree() { return m_ContentsBox; }
   wxListBox* GetIndexList() { return m_IndexBox; }
   wxListBox* GetSearchList() { return m_SearchList; }
   wxImageList* GetContentsImageList() { return m_ContentsImageList; }
   // public interface for wxHtmlHelpControllers handlers, so wxPython can call them
   void OnToolbar(wxCommandEvent& event);
   void OnContentsSel(wxTreeEvent& event) {wxHtmlHelpController::OnContentsSel(event);}
   void OnIndexSel(wxCommandEvent& event) {wxHtmlHelpController::OnIndexSel(event);}
   void OnSearchSel(wxCommandEvent& event) {wxHtmlHelpController::OnSearchSel(event);}
   void OnSearch(wxCommandEvent& event) {wxHtmlHelpController::OnSearch(event);}
   void OnCloseWindow(wxCloseEvent& event);

   // some more protected functions that should be accessible from wxPython
   void RefreshLists();
   void CreateContents() { if (!m_ContentsBox) return; wxHtmlHelpController::CreateContents(); }
   // Adds items to m_Contents tree control
   void CreateIndex() { if (! m_IndexBox) return; wxHtmlHelpController::CreateIndex(); }
   // Adds items to m_IndexList

   DECLARE_EVENT_TABLE()
};

#endif
