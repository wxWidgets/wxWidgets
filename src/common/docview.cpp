/////////////////////////////////////////////////////////////////////////////
// Name:        docview.cpp
// Purpose:     Document/view classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "docview.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#if wxUSE_DOC_VIEW_ARCHITECTURE

#ifndef WX_PRECOMP
#include "wx/string.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/dc.h"
#include "wx/dialog.h"
#include "wx/menu.h"
#include "wx/list.h"
#include "wx/filedlg.h"
#include <wx/intl.h>
#endif

#ifdef __WXGTK__
#include "wx/mdi.h"
#endif

#include "wx/msgdlg.h"
#include "wx/choicdlg.h"
#include "wx/docview.h"
#include "wx/printdlg.h"
#include "wx/confbase.h"

#include <stdio.h>
#include <string.h>

#include "wx/ioswrap.h"

#if wxUSE_IOSTREAMH
    #include <fstream.h>
#else
    #include <fstream>
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxDocument, wxEvtHandler)
IMPLEMENT_ABSTRACT_CLASS(wxView, wxEvtHandler)
IMPLEMENT_ABSTRACT_CLASS(wxDocTemplate, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxDocManager, wxEvtHandler)
IMPLEMENT_CLASS(wxDocChildFrame, wxFrame)
IMPLEMENT_CLASS(wxDocParentFrame, wxFrame)
#if wxUSE_PRINTING_ARCHITECTURE
IMPLEMENT_DYNAMIC_CLASS(wxDocPrintout, wxPrintout)
#endif
IMPLEMENT_CLASS(wxCommand, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxCommandProcessor, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxFileHistory, wxObject)
// IMPLEMENT_DYNAMIC_CLASS(wxPrintInfo, wxObject)
#endif

/*
 * Definition of wxDocument
 */

wxDocument::wxDocument(wxDocument *parent)
{
  m_documentModified=FALSE;
  m_documentFile="";
  m_documentTitle="";
  m_documentParent=parent;
  m_documentTemplate = (wxDocTemplate *) NULL;
  m_documentTypeName = "";
  m_savedYet = FALSE;
}

bool wxDocument::DeleteContents(void)
{
  return TRUE;
}

wxDocument::~wxDocument(void)
{
  DeleteContents();

  if (m_commandProcessor)
    delete m_commandProcessor;

  GetDocumentManager()->RemoveDocument(this);

  // Not safe to do here, since it'll
  // invoke virtual view functions expecting to see
  // valid derived objects: and by the time we get
  // here, we've called destructors higher up.
//  DeleteAllViews();
}
	
bool wxDocument::Close(void)
{
  if (OnSaveModified())
    return OnCloseDocument();
  else
    return FALSE;
}
	
bool wxDocument::OnCloseDocument(void)
{
  DeleteContents();
  Modify(FALSE);
  return TRUE;
}

// Note that this implicitly deletes the document when
// the last view is deleted.
bool wxDocument::DeleteAllViews(void)
{
  wxNode *node = m_documentViews.First();
  while (node)
  {
    wxView *view = (wxView *)node->Data();
    if (!view->Close())
      return FALSE;

    wxNode *next = node->Next();
      
    delete view; // Deletes node implicitly
    node = next;
  }
  return TRUE;
}

wxView *wxDocument::GetFirstView(void) const
{
  if (m_documentViews.Number() == 0)
    return (wxView *) NULL;
  return (wxView *)m_documentViews.First()->Data();
}

wxDocManager *wxDocument::GetDocumentManager(void) const
{
  return m_documentTemplate->GetDocumentManager();
}

bool wxDocument::OnNewDocument(void)
{
  if (!OnSaveModified())
    return FALSE;
    
  if (OnCloseDocument()==FALSE) return FALSE;
  DeleteContents();
  Modify(FALSE);
  SetDocumentSaved(FALSE);

  wxString name;
  GetDocumentManager()->MakeDefaultName(name);
  SetTitle(name);
  SetFilename(name, TRUE);

  return TRUE;
}

bool wxDocument::Save(void)
{
  bool ret = FALSE;

  if (!IsModified()) return TRUE;
  if (m_documentFile == "" || !m_savedYet)
    ret = SaveAs();
  else
    ret = OnSaveDocument(m_documentFile);
  if ( ret )
    SetDocumentSaved(TRUE);
  return ret;
}
	
bool wxDocument::SaveAs(void)
{
    wxDocTemplate *docTemplate = GetDocumentTemplate();
    if (!docTemplate)
        return FALSE;
    
    wxString tmp = wxFileSelector(_("Save as"),
                                  docTemplate->GetDirectory(),
                                  GetFilename(),
                                  docTemplate->GetDefaultExtension(),
                                  docTemplate->GetFileFilter(),
                                  wxSAVE | wxOVERWRITE_PROMPT,
                                  GetDocumentWindow());
    
    if (tmp.IsEmpty())
        return FALSE;

    wxString fileName(tmp);
    wxString path("");
    wxString name("");
    wxString ext("");
    wxSplitPath(fileName, & path, & name, & ext);

    if (ext.IsEmpty() || ext == "")
    {
        fileName += ".";
        fileName += docTemplate->GetDefaultExtension();
    }

    SetFilename(fileName);
    SetTitle(wxFileNameFromPath(fileName));
    
    GetDocumentManager()->AddFileToHistory(fileName);

    // Notify the views that the filename has changed
    wxNode *node = m_documentViews.First();
    while (node)
    {
      wxView *view = (wxView *)node->Data();
      view->OnChangeFilename();
      node = node->Next();
    }

    return OnSaveDocument(m_documentFile);
}
	
bool wxDocument::OnSaveDocument(const wxString& file)
{
  if (file == "")
    return FALSE;

  wxString msgTitle;
  if (wxTheApp->GetAppName() != "")
    msgTitle = wxTheApp->GetAppName();
  else
    msgTitle = wxString(_("File error"));

  ofstream store(file);
  if (store.fail() || store.bad())
  {
    (void)wxMessageBox(_("Sorry, could not open this file for saving."), msgTitle, wxOK | wxICON_EXCLAMATION,
      GetDocumentWindow());
    // Saving error
    return FALSE;
  }
  if (SaveObject(store)==FALSE)
  {
    (void)wxMessageBox(_("Sorry, could not save this file."), msgTitle, wxOK | wxICON_EXCLAMATION,
      GetDocumentWindow());
    // Saving error
    return FALSE;
  }
  Modify(FALSE);
  SetFilename(file);
  return TRUE;
}
	
bool wxDocument::OnOpenDocument(const wxString& file)
{
  if (!OnSaveModified())
    return FALSE;

  wxString msgTitle;
  if (wxTheApp->GetAppName() != "")
    msgTitle = wxTheApp->GetAppName();
  else
    msgTitle = wxString(_("File error"));

  ifstream store(file);
  if (store.fail() || store.bad())
  {
    (void)wxMessageBox(_("Sorry, could not open this file."), msgTitle, wxOK|wxICON_EXCLAMATION,
     GetDocumentWindow());
    return FALSE;
  }
  if (LoadObject(store)==FALSE)
  {
    (void)wxMessageBox(_("Sorry, could not open this file."), msgTitle, wxOK|wxICON_EXCLAMATION,
      GetDocumentWindow());
    return FALSE;
  }
  SetFilename(file, TRUE);
  Modify(FALSE);
  m_savedYet = TRUE;

  UpdateAllViews();
  
  return TRUE;
}
	
istream& wxDocument::LoadObject(istream& stream)
{
//  wxObject::LoadObject(stream);

  return stream;
}

ostream& wxDocument::SaveObject(ostream& stream)
{
//  wxObject::SaveObject(stream);
  
  return stream;
}

bool wxDocument::Revert(void)
{
  return FALSE;
}


// Get title, or filename if no title, else unnamed
bool wxDocument::GetPrintableName(wxString& buf) const
{
  if (m_documentTitle != "")
  {
    buf = m_documentTitle;
    return TRUE;
  }
  else if (m_documentFile != "")
  {
    buf = wxFileNameFromPath(m_documentFile);
    return TRUE;
  }
  else
  {
    buf = _("unnamed");
    return TRUE;
  }
}

wxWindow *wxDocument::GetDocumentWindow(void) const
{
  wxView *view = GetFirstView();
  if (view)
    return view->GetFrame();
  else
    return wxTheApp->GetTopWindow();
}

wxCommandProcessor *wxDocument::OnCreateCommandProcessor(void)
{
  return new wxCommandProcessor;
}

// TRUE if safe to close
bool wxDocument::OnSaveModified(void)
{
  if (IsModified())
  {
    wxString title;
    GetPrintableName(title);

    wxString msgTitle;
    if (wxTheApp->GetAppName() != "")
      msgTitle = wxTheApp->GetAppName();
    else
      msgTitle = wxString(_("Warning"));

    wxString prompt;
    prompt.Printf(_("Do you want to save changes to document %s?"),
                  (const char *)title);
    int res = wxMessageBox(prompt, msgTitle,
                           wxYES_NO|wxCANCEL|wxICON_QUESTION,
                           GetDocumentWindow());
    if (res == wxNO)
    {
      Modify(FALSE);
      return TRUE;
    }
    else if (res == wxYES)
      return Save();
    else if (res == wxCANCEL)
      return FALSE;
  }
  return TRUE;
}

bool wxDocument::Draw(wxDC& WXUNUSED(context))
{
  return TRUE;
}

bool wxDocument::AddView(wxView *view)
{
  if (!m_documentViews.Member(view))
  {
    m_documentViews.Append(view);
    OnChangedViewList();
  }
  return TRUE;
}

bool wxDocument::RemoveView(wxView *view)
{
  (void)m_documentViews.DeleteObject(view);
  OnChangedViewList();
  return TRUE;
}

bool wxDocument::OnCreate(const wxString& WXUNUSED(path), long flags)
{
  if (GetDocumentTemplate()->CreateView(this, flags))
    return TRUE;
  else
    return FALSE;
}

// Called after a view is added or removed.
// The default implementation deletes the document if
// there are no more views.
void wxDocument::OnChangedViewList(void)
{
  if (m_documentViews.Number() == 0)
  {
    if (OnSaveModified())
    {
      delete this;
    }
  }
}

void wxDocument::UpdateAllViews(wxView *sender, wxObject *hint)
{
  wxNode *node = m_documentViews.First();
  while (node)
  {
    wxView *view = (wxView *)node->Data();
    view->OnUpdate(sender, hint);
    node = node->Next();
  }
}

void wxDocument::SetFilename(const wxString& filename, bool notifyViews)
{
  m_documentFile = filename;
  if ( notifyViews )
  {
    // Notify the views that the filename has changed
    wxNode *node = m_documentViews.First();
    while (node)
    {
      wxView *view = (wxView *)node->Data();
      view->OnChangeFilename();
      node = node->Next();
    }
  }
}


/*
 * Document view
 */
 
wxView::wxView()
{
//  SetDocument(doc);
  m_viewDocument = (wxDocument*) NULL;
  
  m_viewTypeName = "";
  m_viewFrame = (wxFrame *) NULL;
}

wxView::~wxView(void)
{
  GetDocumentManager()->ActivateView(this, FALSE, TRUE);
  m_viewDocument->RemoveView(this);
}

// Extend event processing to search the document's event table
bool wxView::ProcessEvent(wxEvent& event)
{
	if ( !GetDocument() || !GetDocument()->ProcessEvent(event) )
		return wxEvtHandler::ProcessEvent(event);
	else
		return TRUE;
}

void wxView::OnActivateView(bool WXUNUSED(activate), wxView *WXUNUSED(activeView), wxView *WXUNUSED(deactiveView))
{
}

void wxView::OnPrint(wxDC *dc, wxObject *WXUNUSED(info))
{
  OnDraw(dc);
}

void wxView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
}

void wxView::OnChangeFilename(void)
{
  if (GetFrame() && GetDocument())
  {
    wxString name;
    GetDocument()->GetPrintableName(name);

    GetFrame()->SetTitle(name);
  }
}

void wxView::SetDocument(wxDocument *doc)
{
  m_viewDocument = doc;
  if (doc)
    doc->AddView(this);
}

bool wxView::Close(bool deleteWindow)
{
  if (OnClose(deleteWindow))
    return TRUE;
  else
    return FALSE;
}

void wxView::Activate(bool activate)
{
  if (GetDocumentManager())
  {
    OnActivateView(activate, this, GetDocumentManager()->GetCurrentView());
    GetDocumentManager()->ActivateView(this, activate);
  }
}

bool wxView::OnClose(bool WXUNUSED(deleteWindow))
{
  return GetDocument() ? GetDocument()->Close() : TRUE;
}

#if wxUSE_PRINTING_ARCHITECTURE
wxPrintout *wxView::OnCreatePrintout(void)
{
  return new wxDocPrintout(this);
}
#endif


/*
 * wxDocTemplate
 */

wxDocTemplate::wxDocTemplate(wxDocManager *manager, const wxString& descr,
  const wxString& filter, const wxString& dir, const wxString& ext,
  const wxString& docTypeName, const wxString& viewTypeName,
  wxClassInfo *docClassInfo, wxClassInfo *viewClassInfo, long flags)
{
  m_documentManager = manager;
  m_flags = flags;
  m_description = descr;
  m_directory = dir;
  m_defaultExt = ext;
  m_fileFilter = filter;
  m_flags = flags;
  m_docTypeName = docTypeName;
  m_viewTypeName = viewTypeName;
  m_documentManager->AssociateTemplate(this);

  m_docClassInfo = docClassInfo;
  m_viewClassInfo = viewClassInfo;
}

wxDocTemplate::~wxDocTemplate(void)
{
  m_documentManager->DisassociateTemplate(this);
}
  
// Tries to dynamically construct an object of the right
// class.
wxDocument *wxDocTemplate::CreateDocument(const wxString& path, long flags)
{
  if (!m_docClassInfo)
    return (wxDocument *) NULL;
  wxDocument *doc = (wxDocument *)m_docClassInfo->CreateObject();
  doc->SetFilename(path);
  doc->SetDocumentTemplate(this);
  GetDocumentManager()->AddDocument(doc);
  doc->SetCommandProcessor(doc->OnCreateCommandProcessor());
  
  if (doc->OnCreate(path, flags))
    return doc;
  else
  {
    delete doc;
    return (wxDocument *) NULL;
  }
}

wxView *wxDocTemplate::CreateView(wxDocument *doc, long flags)
{
  if (!m_viewClassInfo)
    return (wxView *) NULL;
  wxView *view = (wxView *)m_viewClassInfo->CreateObject();
  view->SetDocument(doc);
  if (view->OnCreate(doc, flags))
  {
    return view;
  }
  else
  {
    delete view;
    return (wxView *) NULL;
  }
}

BEGIN_EVENT_TABLE(wxDocManager, wxEvtHandler)
    EVT_MENU(wxID_OPEN, wxDocManager::OnFileOpen)
    EVT_MENU(wxID_CLOSE, wxDocManager::OnFileClose)
    EVT_MENU(wxID_REVERT, wxDocManager::OnFileRevert)
    EVT_MENU(wxID_NEW, wxDocManager::OnFileNew)
    EVT_MENU(wxID_SAVE, wxDocManager::OnFileSave)
    EVT_MENU(wxID_SAVEAS, wxDocManager::OnFileSaveAs)
    EVT_MENU(wxID_UNDO, wxDocManager::OnUndo)
    EVT_MENU(wxID_REDO, wxDocManager::OnRedo)
    EVT_MENU(wxID_PRINT, wxDocManager::OnPrint)
    EVT_MENU(wxID_PRINT_SETUP, wxDocManager::OnPrintSetup)
    EVT_MENU(wxID_PREVIEW, wxDocManager::OnPreview)
END_EVENT_TABLE()

wxDocManager::wxDocManager(long flags, bool initialize)
{
  m_defaultDocumentNameCounter = 1;
  m_flags = flags;
  m_currentView = (wxView *) NULL;
  m_maxDocsOpen = 10000;
  m_fileHistory = (wxFileHistory *) NULL;
  if (initialize)
    Initialize();
}

wxDocManager::~wxDocManager(void)
{
  Clear();
  if (m_fileHistory)
    delete m_fileHistory;
}

bool wxDocManager::Clear(bool force)
{
  wxNode *node = m_docs.First();
  while (node)
  {
    wxDocument *doc = (wxDocument *)node->Data();
    wxNode *next = node->Next();

    if (!doc->Close() && !force)
      return FALSE;

    // Implicitly deletes the document when the last
    // view is removed (deleted)
    doc->DeleteAllViews();

    // Check document is deleted
    if (m_docs.Member(doc))
      delete doc;

    // This assumes that documents are not connected in
    // any way, i.e. deleting one document does NOT
    // delete another.
    node = next;
  }
  node = m_templates.First();
  while (node)
  {
        wxDocTemplate *templ = (wxDocTemplate*) node->Data();
        wxNode* next = node->Next();
        delete templ;
        node = next;
  }
  return TRUE;
}

bool wxDocManager::Initialize(void)
{
  m_fileHistory = OnCreateFileHistory();
  return TRUE;
}

wxFileHistory *wxDocManager::OnCreateFileHistory(void)
{
  return new wxFileHistory;
}

void wxDocManager::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
  wxDocument *doc = GetCurrentDocument();
  if (!doc)
    return;
  if (doc->Close())
  {
    doc->DeleteAllViews();
    if (m_docs.Member(doc))
      delete doc;
  }
}

void wxDocManager::OnFileNew(wxCommandEvent& WXUNUSED(event))
{
  CreateDocument(wxString(""), wxDOC_NEW);
}

void wxDocManager::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
  CreateDocument(wxString(""), 0);
}

void wxDocManager::OnFileRevert(wxCommandEvent& WXUNUSED(event))
{
  wxDocument *doc = GetCurrentDocument();
  if (!doc)
    return;
  doc->Revert();
}

void wxDocManager::OnFileSave(wxCommandEvent& WXUNUSED(event))
{
  wxDocument *doc = GetCurrentDocument();
  if (!doc)
    return;
  doc->Save();
}

void wxDocManager::OnFileSaveAs(wxCommandEvent& WXUNUSED(event))
{
  wxDocument *doc = GetCurrentDocument();
  if (!doc)
    return;
  doc->SaveAs();
}

void wxDocManager::OnPrint(wxCommandEvent& WXUNUSED(event))
{
  wxView *view = GetCurrentView();
  if (!view)
    return;

  wxPrintout *printout = view->OnCreatePrintout();
  if (printout)
  {
    wxPrinter printer;
    printer.Print(view->GetFrame(), printout, TRUE);

    delete printout;
  }
}

void wxDocManager::OnPrintSetup(wxCommandEvent& WXUNUSED(event))
{
  wxWindow *parentWin = wxTheApp->GetTopWindow();
  wxView *view = GetCurrentView();
  if (view)
    parentWin = view->GetFrame();

  wxPrintDialogData data;

  wxPrintDialog printerDialog(parentWin, & data);
  printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
  printerDialog.ShowModal();
}

void wxDocManager::OnPreview(wxCommandEvent& WXUNUSED(event))
{
  wxView *view = GetCurrentView();
  if (!view)
    return;

  wxPrintout *printout = view->OnCreatePrintout();
  if (printout)
  {
    // Pass two printout objects: for preview, and possible printing.
    wxPrintPreviewBase *preview = (wxPrintPreviewBase *) NULL;
    preview = new wxPrintPreview(printout, view->OnCreatePrintout());

    wxPreviewFrame *frame = new wxPreviewFrame(preview, (wxFrame *)wxTheApp->GetTopWindow(), _("Print Preview"),
		wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(TRUE);
  }
}

void wxDocManager::OnUndo(wxCommandEvent& WXUNUSED(event))
{
  wxDocument *doc = GetCurrentDocument();
  if (!doc)
    return;
  if (doc->GetCommandProcessor())
    doc->GetCommandProcessor()->Undo();
}

void wxDocManager::OnRedo(wxCommandEvent& WXUNUSED(event))
{
  wxDocument *doc = GetCurrentDocument();
  if (!doc)
    return;
  if (doc->GetCommandProcessor())
    doc->GetCommandProcessor()->Redo();
}

wxView *wxDocManager::GetCurrentView(void) const
{
    if (m_currentView)
        return m_currentView;
    if (m_docs.Number() == 1)
    {
        wxDocument* doc = (wxDocument*) m_docs.First()->Data();
        return doc->GetFirstView();
    }
    return (wxView *) NULL;
}

// Extend event processing to search the view's event table
bool wxDocManager::ProcessEvent(wxEvent& event)
{
    wxView* view = GetCurrentView();
    if (view)
    {
        if (view->ProcessEvent(event))
            return TRUE;
    }
    return wxEvtHandler::ProcessEvent(event);
}

wxDocument *wxDocManager::CreateDocument(const wxString& path, long flags)
{
  wxDocTemplate **templates = new wxDocTemplate *[m_templates.Number()];
  int i;
  int n = 0;
  for (i = 0; i < m_templates.Number(); i++)
  {
    wxDocTemplate *temp = (wxDocTemplate *)(m_templates.Nth(i)->Data());
    if (temp->IsVisible())
    {
      templates[n] = temp;
      n ++;
    }
  }
  if (n == 0)
  {
    delete[] templates;
    return (wxDocument *) NULL;
  }

  // If we've reached the max number of docs, close the
  // first one.
  if (GetDocuments().Number() >= m_maxDocsOpen)
  {
    wxDocument *doc = (wxDocument *)GetDocuments().First()->Data();
    if (doc->Close())
    {
      // Implicitly deletes the document when
      // the last view is deleted
      doc->DeleteAllViews();

      // Check we're really deleted
      if (m_docs.Member(doc))
        delete doc;
    }
    else
      return (wxDocument *) NULL;
  }
  
  // New document: user chooses a template, unless there's only one.
  if (flags & wxDOC_NEW)
  {
    if (n == 1)
    {
      wxDocTemplate *temp = templates[0];
      delete[] templates;
      wxDocument *newDoc = temp->CreateDocument(path, flags);
      if (newDoc)
      {
        newDoc->SetDocumentName(temp->GetDocumentName());
        newDoc->SetDocumentTemplate(temp);
        newDoc->OnNewDocument();
      }
      return newDoc;
    }

    wxDocTemplate *temp = SelectDocumentType(templates, n);
    delete[] templates;
    if (temp)
    {
      wxDocument *newDoc = temp->CreateDocument(path, flags);
      if (newDoc)
      {
        newDoc->SetDocumentName(temp->GetDocumentName());
        newDoc->SetDocumentTemplate(temp);
        newDoc->OnNewDocument();
      }
      return newDoc;
    }
    else
      return (wxDocument *) NULL;
  }

  // Existing document
  wxDocTemplate *temp = (wxDocTemplate *) NULL;

  wxString path2("");
  if (path != "")
    path2 = path;

  if (flags & wxDOC_SILENT)
    temp = FindTemplateForPath(path2);
  else
    temp = SelectDocumentPath(templates, n, path2, flags);

  delete[] templates;

  if (temp)
  {
    wxDocument *newDoc = temp->CreateDocument(path2, flags);
    if (newDoc)
    {
      newDoc->SetDocumentName(temp->GetDocumentName());
      newDoc->SetDocumentTemplate(temp);
      if (!newDoc->OnOpenDocument(path2))
      {
        delete newDoc;
        return (wxDocument *) NULL;
      }
      AddFileToHistory(path2);
    }
    return newDoc;
  }
  else
    return (wxDocument *) NULL;
}

wxView *wxDocManager::CreateView(wxDocument *doc, long flags)
{
  wxDocTemplate **templates = new wxDocTemplate *[m_templates.Number()];
  int n =0;
  int i;
  for (i = 0; i < m_templates.Number(); i++)
  {
    wxDocTemplate *temp = (wxDocTemplate *)(m_templates.Nth(i)->Data());
    if (temp->IsVisible())
    {
      if (temp->GetDocumentName() == doc->GetDocumentName())
      {
        templates[n] = temp;
        n ++;
      }
    }
  }
  if (n == 0)
  {
    delete[] templates;
    return (wxView *) NULL;
  }
  if (n == 1)
  {
    wxDocTemplate *temp = templates[0];
    delete[] templates;
    wxView *view = temp->CreateView(doc, flags);
    if (view)
      view->SetViewName(temp->GetViewName());
    return view;
  }
  
  wxDocTemplate *temp = SelectViewType(templates, n);
  delete[] templates;
  if (temp)
  {
    wxView *view = temp->CreateView(doc, flags);
    if (view)
      view->SetViewName(temp->GetViewName());
    return view;
  }
  else
    return (wxView *) NULL;
}

// Not yet implemented
void wxDocManager::DeleteTemplate(wxDocTemplate *WXUNUSED(temp), long WXUNUSED(flags))
{
}

// Not yet implemented
bool wxDocManager::FlushDoc(wxDocument *WXUNUSED(doc))
{
  return FALSE;
}

wxDocument *wxDocManager::GetCurrentDocument(void) const
{
  if (m_currentView)
    return m_currentView->GetDocument();
  else
    return (wxDocument *) NULL;
}

// Make a default document name
bool wxDocManager::MakeDefaultName(wxString& name)
{
  name.Printf(_("unnamed%d"), m_defaultDocumentNameCounter);
  m_defaultDocumentNameCounter++;

  return TRUE;
}

// Not yet implemented
wxDocTemplate *wxDocManager::MatchTemplate(const wxString& WXUNUSED(path))
{
  return (wxDocTemplate *) NULL;
}

// File history management
void wxDocManager::AddFileToHistory(const wxString& file)
{
  if (m_fileHistory)
    m_fileHistory->AddFileToHistory(file);
}

wxString wxDocManager::GetHistoryFile(int i) const
{
  if (m_fileHistory)
    return wxString(m_fileHistory->GetHistoryFile(i));
  else
    return wxString("");
}

void wxDocManager::FileHistoryUseMenu(wxMenu *menu)
{
  if (m_fileHistory)
    m_fileHistory->UseMenu(menu);
}

void wxDocManager::FileHistoryRemoveMenu(wxMenu *menu)
{
  if (m_fileHistory)
    m_fileHistory->RemoveMenu(menu);
}

#if wxUSE_CONFIG
void wxDocManager::FileHistoryLoad(wxConfigBase& config)
{
  if (m_fileHistory)
    m_fileHistory->Load(config);
}

void wxDocManager::FileHistorySave(wxConfigBase& config)
{
  if (m_fileHistory)
    m_fileHistory->Save(config);
}
#endif

void wxDocManager::FileHistoryAddFilesToMenu(wxMenu* menu)
{
  if (m_fileHistory)
    m_fileHistory->AddFilesToMenu(menu);
}

void wxDocManager::FileHistoryAddFilesToMenu()
{
  if (m_fileHistory)
    m_fileHistory->AddFilesToMenu();
}

int wxDocManager::GetNoHistoryFiles(void) const
{
  if (m_fileHistory)
    return m_fileHistory->GetNoHistoryFiles();
  else
    return 0;
}

static char *FindExtension(char *path)
{
  static char ext[10];
  int len = strlen(path);
  if (path)
  {
    int i = 0;
    for (i = (len-1); i > 0; i --)
      if (path[i] == '.')
        break;
    if (path[i] == '.')
    {
      int j;
      for (j = i+1; j < len; j++)
        ext[(int)(j-(i+1))] = (char)wxToLower(path[j]); // NOTE Should not use tolower under UNIX
      ext[j-(i+1)] = 0;
      return ext;
    }
    else
      return (char *) NULL;
  }
  else return (char *) NULL;
}


// Given a path, try to find a matching template. Won't
// always work, of course.
wxDocTemplate *wxDocManager::FindTemplateForPath(const wxString& path)
{
  char *theExt = FindExtension((char *)(const char *)path);
  if (!theExt)
    return (wxDocTemplate *) NULL;
  wxDocTemplate *theTemplate = (wxDocTemplate *) NULL;

  if (m_templates.Number() == 1)
    return (wxDocTemplate *)m_templates.First()->Data();

  // Find the template which this extension corresponds to
  int i;
  for (i = 0; i < m_templates.Number(); i++)
  {
    wxDocTemplate *temp = (wxDocTemplate *)m_templates.Nth(i)->Data();
    if (strcmp(temp->GetDefaultExtension(), theExt) == 0)
    {
      theTemplate = temp;
      break;
    }
  }
  return theTemplate;
}

// Prompts user to open a file, using file specs in templates.
// How to implement in wxWindows? Must extend the file selector
// dialog or implement own; OR match the extension to the
// template extension.

#ifdef __WXMSW__
wxDocTemplate *wxDocManager::SelectDocumentPath(wxDocTemplate **templates,
    int noTemplates, wxString& path, long WXUNUSED(flags), bool WXUNUSED(save))
#else
wxDocTemplate *wxDocManager::SelectDocumentPath(wxDocTemplate **WXUNUSED(templates),
    int WXUNUSED(noTemplates), wxString& path, long WXUNUSED(flags), bool WXUNUSED(save))
#endif    
{
  // We can only have multiple filters in Windows
#ifdef __WXMSW__
    wxString descrBuf;

    int i;
    for (i = 0; i < noTemplates; i++)
    {
        if (templates[i]->IsVisible())
        {
            // add a '|' to separate this filter from the previous one
            if ( !descrBuf.IsEmpty() )
                descrBuf << '|';

            descrBuf << templates[i]->GetDescription()
                     << " (" << templates[i]->GetFileFilter() << ") |"
                     << templates[i]->GetFileFilter();
        }
    }
#else
  wxString descrBuf = "*.*";
#endif

  wxString pathTmp = wxFileSelector(_("Select a file"), "", "", "",
                                    descrBuf, 0, wxTheApp->GetTopWindow());

  if (!pathTmp.IsEmpty())
  {
    path = pathTmp;
    char *theExt = FindExtension((char *)(const char *)path);
    if (!theExt)
      return (wxDocTemplate *) NULL;

    // This is dodgy in that we're selecting the template on the
    // basis of the file extension, which may not be a standard
    // one. We really want to know exactly which template was
    // chosen by using a more advanced file selector.
    wxDocTemplate *theTemplate = FindTemplateForPath(path);
    return theTemplate;
  }
  else
  {
    path = "";
    return (wxDocTemplate *) NULL;
  }
#if 0
  // In all other windowing systems, until we have more advanced
  // file selectors, we must select the document type (template) first, and
  // _then_ pop up the file selector.
  wxDocTemplate *temp = SelectDocumentType(templates, noTemplates);
  if (!temp)
    return (wxDocTemplate *) NULL;

  char *pathTmp = wxFileSelector(_("Select a file"), "", "",
     temp->GetDefaultExtension(),
     temp->GetFileFilter(),
     0, wxTheApp->GetTopWindow());

  if (pathTmp)
  {
    path = pathTmp;
    return temp;
  }
  else
    return (wxDocTemplate *) NULL;
#endif
}

wxDocTemplate *wxDocManager::SelectDocumentType(wxDocTemplate **templates,
    int noTemplates)
{
  char **strings = new char *[noTemplates];
  char **data = new char *[noTemplates];
  int i;
  int n = 0;
  for (i = 0; i < noTemplates; i++)
  {
    if (templates[i]->IsVisible())
    {
      strings[n] = WXSTRINGCAST templates[i]->m_description;
      data[n] = (char *)templates[i];
      n ++;
    }
  }
  if (n == 0)
  {
    delete[] strings;
    delete[] data;
    return (wxDocTemplate *) NULL;
  }
  else if (n == 1)
  {
    wxDocTemplate *temp = (wxDocTemplate *)data[0];
    delete[] strings;
    delete[] data;
    return temp;
  }
  
  wxDocTemplate *theTemplate = (wxDocTemplate *)wxGetSingleChoiceData(_("Select a document template"), _("Templates"), n,
    strings, data);
  delete[] strings;
  delete[] data;
  return theTemplate;
}

wxDocTemplate *wxDocManager::SelectViewType(wxDocTemplate **templates,
    int noTemplates)
{
  char **strings = new char *[noTemplates];
  char **data = new char *[noTemplates];
  int i;
  int n = 0;
  for (i = 0; i < noTemplates; i++)
  {
    if (templates[i]->IsVisible() && (templates[i]->GetViewName() != ""))
    {
      strings[n] = WXSTRINGCAST templates[i]->m_viewTypeName;
      data[n] = (char *)templates[i];
      n ++;
    }
  }
  wxDocTemplate *theTemplate = (wxDocTemplate *)wxGetSingleChoiceData(_("Select a document view"), _("Views"), n,
    strings, data);
  delete[] strings;
  delete[] data;
  return theTemplate;
}

void wxDocManager::AssociateTemplate(wxDocTemplate *temp)
{
  if (!m_templates.Member(temp))
    m_templates.Append(temp);
}

void wxDocManager::DisassociateTemplate(wxDocTemplate *temp)
{
  m_templates.DeleteObject(temp);
}

// Add and remove a document from the manager's list
void wxDocManager::AddDocument(wxDocument *doc)
{
  if (!m_docs.Member(doc))
    m_docs.Append(doc);
}

void wxDocManager::RemoveDocument(wxDocument *doc)
{
  m_docs.DeleteObject(doc);
}

// Views or windows should inform the document manager
// when a view is going in or out of focus
void wxDocManager::ActivateView(wxView *view, bool activate, bool WXUNUSED(deleting))
{
  // If we're deactiving, and if we're not actually deleting the view, then
  // don't reset the current view because we may be going to
  // a window without a view.
  // WHAT DID I MEAN BY THAT EXACTLY?
/*
  if (deleting)
  {
    if (m_currentView == view)
      m_currentView = NULL;
  }
  else
*/
  {
    if (activate)
      m_currentView = view;
    else
      m_currentView = (wxView *) NULL;
  }
}

/*
 * Default document child frame
 */

BEGIN_EVENT_TABLE(wxDocChildFrame, wxFrame)
    EVT_ACTIVATE(wxDocChildFrame::OnActivate)
    EVT_CLOSE(wxDocChildFrame::OnCloseWindow)
END_EVENT_TABLE()

wxDocChildFrame::wxDocChildFrame(wxDocument *doc, wxView *view, wxFrame *frame, wxWindowID id, const wxString& title,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name):
    wxFrame(frame, id, title, pos, size, style, name)
{
  m_childDocument = doc;
  m_childView = view;
  if (view)
    view->SetFrame(this);
}

wxDocChildFrame::~wxDocChildFrame(void)
{
}

// Extend event processing to search the view's event table
bool wxDocChildFrame::ProcessEvent(wxEvent& event)
{
    if (m_childView)
        m_childView->Activate(TRUE);

	if ( !m_childView || ! m_childView->ProcessEvent(event) )
    {
        // Only hand up to the parent if it's a menu command
        if (!event.IsKindOf(CLASSINFO(wxCommandEvent)) || !GetParent() || !GetParent()->ProcessEvent(event))
		    return wxEvtHandler::ProcessEvent(event);
        else
            return TRUE;
    }
	else
		return TRUE;
}

void wxDocChildFrame::OnActivate(wxActivateEvent& event)
{
  wxFrame::OnActivate(event);

  if (m_childView)
    m_childView->Activate(event.GetActive());
}

void wxDocChildFrame::OnCloseWindow(wxCloseEvent& event)
{
  if (m_childView)
  {
    bool ans = FALSE;
    if (!event.CanVeto())
      ans = TRUE; // Must delete.
    else
      ans = m_childView->Close(FALSE); // FALSE means don't delete associated window

    if (ans)
    {
      m_childView->Activate(FALSE);
      delete m_childView;
      m_childView = (wxView *) NULL;
      m_childDocument = (wxDocument *) NULL;

      this->Destroy();
    }
    else
      event.Veto();
  }
  else
    event.Veto();
}

/*
 * Default parent frame
 */

BEGIN_EVENT_TABLE(wxDocParentFrame, wxFrame)
    EVT_MENU(wxID_EXIT, wxDocParentFrame::OnExit)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, wxDocParentFrame::OnMRUFile)
    EVT_CLOSE(wxDocParentFrame::OnCloseWindow)
END_EVENT_TABLE()

wxDocParentFrame::wxDocParentFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name):
  wxFrame(frame, id, title, pos, size, style, name)
{
  m_docManager = manager;
}

void wxDocParentFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void wxDocParentFrame::OnMRUFile(wxCommandEvent& event)
{
      wxString f(m_docManager->GetHistoryFile(event.GetSelection() - wxID_FILE1));
      if (f != "")
        (void)m_docManager->CreateDocument(f, wxDOC_SILENT);
}

// Extend event processing to search the view's event table
bool wxDocParentFrame::ProcessEvent(wxEvent& event)
{
    // Try the document manager, then do default processing
    if (!m_docManager || !m_docManager->ProcessEvent(event))
        return wxEvtHandler::ProcessEvent(event);
    else
        return TRUE;
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
void wxDocParentFrame::OnCloseWindow(wxCloseEvent& event)
{
  if (m_docManager->Clear(!event.CanVeto()))
  {
    this->Destroy();
  }
  else
    event.Veto();
}

#if wxUSE_PRINTING_ARCHITECTURE

wxDocPrintout::wxDocPrintout(wxView *view, const wxString& title):
  wxPrintout(WXSTRINGCAST title)
{
  m_printoutView = view;
}

bool wxDocPrintout::OnPrintPage(int WXUNUSED(page))
{
  wxDC *dc = GetDC();
  
  // Get the logical pixels per inch of screen and printer
  int ppiScreenX, ppiScreenY;
  GetPPIScreen(&ppiScreenX, &ppiScreenY);
  int ppiPrinterX, ppiPrinterY;
  GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

  // This scales the DC so that the printout roughly represents the
  // the screen scaling. The text point size _should_ be the right size
  // but in fact is too small for some reason. This is a detail that will
  // need to be addressed at some point but can be fudged for the
  // moment.
  float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

  // Now we have to check in case our real page size is reduced
  // (e.g. because we're drawing to a print preview memory DC)
  int pageWidth, pageHeight;
  int w, h;
  dc->GetSize(&w, &h);
  GetPageSizePixels(&pageWidth, &pageHeight);

  // If printer pageWidth == current DC width, then this doesn't
  // change. But w might be the preview bitmap width, so scale down.
  float overallScale = scale * (float)(w/(float)pageWidth);
  dc->SetUserScale(overallScale, overallScale);

  if (m_printoutView)
  {
    m_printoutView->OnDraw(dc);
  }
  return TRUE;
}

bool wxDocPrintout::HasPage(int pageNum)
{
  return (pageNum == 1);
}

bool wxDocPrintout::OnBeginDocument(int startPage, int endPage)
{
  if (!wxPrintout::OnBeginDocument(startPage, endPage))
    return FALSE;

  return TRUE;
}

void wxDocPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
  *minPage = 1;
  *maxPage = 1;
  *selPageFrom = 1;
  *selPageTo = 1;
}

#endif

/*
 * Command processing framework
 */

wxCommand::wxCommand(bool canUndoIt, const wxString& name)
{
  m_canUndo = canUndoIt;
  m_commandName = name;
}

wxCommand::~wxCommand(void)
{
}

// Command processor
wxCommandProcessor::wxCommandProcessor(int maxCommands)
{
  m_maxNoCommands = maxCommands;
  m_currentCommand = (wxNode *) NULL;
  m_commandEditMenu = (wxMenu *) NULL;
}

wxCommandProcessor::~wxCommandProcessor(void)
{
  ClearCommands();
}

// Pass a command to the processor. The processor calls Do();
// if successful, is appended to the command history unless
// storeIt is FALSE.
bool wxCommandProcessor::Submit(wxCommand *command, bool storeIt)
{
  bool success = command->Do();
  if (success && storeIt)
  {
    if (m_commands.Number() == m_maxNoCommands)
    {
      wxNode *firstNode = m_commands.First();
      wxCommand *firstCommand = (wxCommand *)firstNode->Data();
      delete firstCommand;
      delete firstNode;
    }

    // Correct a bug: we must chop off the current 'branch'
    // so that we're at the end of the command list.
    if (!m_currentCommand)
      ClearCommands();
    else
    {
      wxNode *node = m_currentCommand->Next();
      while (node)
      {
        wxNode *next = node->Next();
        delete (wxCommand *)node->Data();
        delete node;
        node = next;
      }
    }
    
    m_commands.Append(command);
    m_currentCommand = m_commands.Last();
    SetMenuStrings();
  }
  return success;
}

bool wxCommandProcessor::Undo(void)
{
  if (m_currentCommand)
  {
    wxCommand *command = (wxCommand *)m_currentCommand->Data();
    if (command->CanUndo())
    {
      bool success = command->Undo();
      if (success)
      {
        m_currentCommand = m_currentCommand->Previous();
        SetMenuStrings();
        return TRUE;
      }
    }
  }
  return FALSE;
}

bool wxCommandProcessor::Redo(void)
{
  wxCommand *redoCommand = (wxCommand *) NULL;
  wxNode *redoNode = (wxNode *) NULL;
  if (m_currentCommand && m_currentCommand->Next())
  {
    redoCommand = (wxCommand *)m_currentCommand->Next()->Data();
    redoNode = m_currentCommand->Next();
  }
  else
  {
    if (m_commands.Number() > 0)
    {
      redoCommand = (wxCommand *)m_commands.First()->Data();
      redoNode = m_commands.First();
    }
  }

  if (redoCommand)
  {
    bool success = redoCommand->Do();
    if (success)
    {
      m_currentCommand = redoNode;
      SetMenuStrings();
      return TRUE;
    }
  }
  return FALSE;
}

bool wxCommandProcessor::CanUndo(void) const
{
  if (m_currentCommand)
    return ((wxCommand *)m_currentCommand->Data())->CanUndo();
  return FALSE;
}

bool wxCommandProcessor::CanRedo(void) const
{
    if ((m_currentCommand != (wxNode*) NULL) && (m_currentCommand->Next() == (wxNode*) NULL))
        return FALSE;

    if ((m_currentCommand != (wxNode*) NULL) && (m_currentCommand->Next() != (wxNode*) NULL))
        return TRUE;

    if ((m_currentCommand == (wxNode*) NULL) && (m_commands.Number() > 0))
        return TRUE;

    return FALSE;
}

void wxCommandProcessor::Initialize(void)
{
  m_currentCommand = m_commands.Last();
  SetMenuStrings();
}

void wxCommandProcessor::SetMenuStrings(void)
{
  if (m_commandEditMenu)
  {
    wxString buf;
    if (m_currentCommand)
    {
      wxCommand *command = (wxCommand *)m_currentCommand->Data();
      wxString commandName(command->GetName());
      if (commandName == "") commandName = _("Unnamed command");
      bool canUndo = command->CanUndo();
      if (canUndo)
        buf = wxString(_("&Undo ")) + commandName;
      else
        buf = wxString(_("Can't &Undo ")) + commandName;
        
      m_commandEditMenu->SetLabel(wxID_UNDO, buf);
      m_commandEditMenu->Enable(wxID_UNDO, canUndo);
      
      // We can redo, if we're not at the end of the history.
      if (m_currentCommand->Next())
      {
        wxCommand *redoCommand = (wxCommand *)m_currentCommand->Next()->Data();
        wxString redoCommandName(redoCommand->GetName());
        if (redoCommandName == "") redoCommandName = _("Unnamed command");
        buf = wxString(_("&Redo ")) + redoCommandName;
        m_commandEditMenu->SetLabel(wxID_REDO, buf);
        m_commandEditMenu->Enable(wxID_REDO, TRUE);
      }
      else
      {
        m_commandEditMenu->SetLabel(wxID_REDO, _("&Redo"));
        m_commandEditMenu->Enable(wxID_REDO, FALSE);
      }
    }
    else
    {
      m_commandEditMenu->SetLabel(wxID_UNDO, _("&Undo"));
      m_commandEditMenu->Enable(wxID_UNDO, FALSE);

      if (m_commands.Number() == 0)
      {
        m_commandEditMenu->SetLabel(wxID_REDO, _("&Redo"));
        m_commandEditMenu->Enable(wxID_REDO, FALSE);
      }
      else
      {
        // currentCommand is NULL but there are commands: this means that
        // we've undone to the start of the list, but can redo the first.
        wxCommand *redoCommand = (wxCommand *)m_commands.First()->Data();
        wxString redoCommandName(redoCommand->GetName());
        if (redoCommandName == "") redoCommandName = _("Unnamed command");
        buf = wxString(_("&Redo ")) + redoCommandName;
        m_commandEditMenu->SetLabel(wxID_REDO, buf);
        m_commandEditMenu->Enable(wxID_REDO, TRUE);
      }
    }
  }
}

void wxCommandProcessor::ClearCommands(void)
{
  wxNode *node = m_commands.First();
  while (node)
  {
    wxCommand *command = (wxCommand *)node->Data();
    delete command;
    delete node;
    node = m_commands.First();
  }
  m_currentCommand = (wxNode *) NULL;
}


/*
 * File history processor
 */

wxFileHistory::wxFileHistory(int maxFiles)
{
  m_fileMaxFiles = maxFiles;
  m_fileHistoryN = 0;
  m_fileHistory = new char *[m_fileMaxFiles];
}

wxFileHistory::~wxFileHistory(void)
{
  int i;
  for (i = 0; i < m_fileHistoryN; i++)
    delete[] m_fileHistory[i];
  delete[] m_fileHistory;
}

// File history management
void wxFileHistory::AddFileToHistory(const wxString& file)
{
  int i;
  // Check we don't already have this file
  for (i = 0; i < m_fileHistoryN; i++)
  {
      if (m_fileHistory[i] && wxString(m_fileHistory[i]) == file)
          return;
  }

  // Add to the project file history:
  // Move existing files (if any) down so we can insert file at beginning.
  
  // First delete filename that has popped off the end of the array (if any)
  if (m_fileHistoryN == m_fileMaxFiles)
  {
    delete[] m_fileHistory[m_fileMaxFiles-1];
    m_fileHistory[m_fileMaxFiles-1] = (char *) NULL;
  }
  if (m_fileHistoryN < m_fileMaxFiles)
  {
    wxNode* node = m_fileMenus.First();
    while (node)
    {
        wxMenu* menu = (wxMenu*) node->Data();
        if (m_fileHistoryN == 0)
            menu->AppendSeparator();
        menu->Append(wxID_FILE1+m_fileHistoryN, _("[EMPTY]"));
        node = node->Next();
    }
    m_fileHistoryN ++;
  }
  // Shuffle filenames down
  for (i = (m_fileHistoryN-1); i > 0; i--)
  {
    m_fileHistory[i] = m_fileHistory[i-1];
  }
  m_fileHistory[0] = copystring(file);

  for (i = 0; i < m_fileHistoryN; i++)
    if (m_fileHistory[i])
    {
      wxString buf;
      buf.Printf("&%d %s", i+1, m_fileHistory[i]);
      wxNode* node = m_fileMenus.First();
      while (node)
      {
        wxMenu* menu = (wxMenu*) node->Data();
        menu->SetLabel(wxID_FILE1+i, buf);
        node = node->Next();
      }
    }
}

wxString wxFileHistory::GetHistoryFile(int i) const
{
  if (i < m_fileHistoryN)
    return wxString(m_fileHistory[i]);
  else
    return wxString("");
}

void wxFileHistory::UseMenu(wxMenu *menu)
{
  if (!m_fileMenus.Member(menu))
    m_fileMenus.Append(menu);
}

void wxFileHistory::RemoveMenu(wxMenu *menu)
{
  m_fileMenus.DeleteObject(menu);
}

#if wxUSE_CONFIG
void wxFileHistory::Load(wxConfigBase& config)
{
  m_fileHistoryN = 0;
  wxString buf;
  buf.Printf("file%d", m_fileHistoryN+1);
  wxString historyFile;
  while ((m_fileHistoryN <= m_fileMaxFiles) && config.Read(buf, &historyFile) && (historyFile != ""))
  {
    m_fileHistory[m_fileHistoryN] = copystring((const char*) historyFile);
    m_fileHistoryN ++;
    buf.Printf("file%d", m_fileHistoryN+1);
    historyFile = "";
  }
  AddFilesToMenu();
}

void wxFileHistory::Save(wxConfigBase& config)
{
  int i;
  for (i = 0; i < m_fileHistoryN; i++)
  {
    wxString buf;
    buf.Printf("file%d", i+1);
    config.Write(buf, wxString(m_fileHistory[i]));
  }
}
#endif

void wxFileHistory::AddFilesToMenu()
{
    if (m_fileHistoryN > 0)
    {
        wxNode* node = m_fileMenus.First();
        while (node)
        {
            wxMenu* menu = (wxMenu*) node->Data();
            menu->AppendSeparator();
            int i;
            for (i = 0; i < m_fileHistoryN; i++)
            {
                if (m_fileHistory[i])
                {
                    wxString buf;
                    buf.Printf("&%d %s", i+1, m_fileHistory[i]);
                    menu->Append(wxID_FILE1+i, buf);
                }
            }
            node = node->Next();
        }
    }
}

void wxFileHistory::AddFilesToMenu(wxMenu* menu)
{
    if (m_fileHistoryN > 0)
    {
        menu->AppendSeparator();
        int i;
        for (i = 0; i < m_fileHistoryN; i++)
        {
            if (m_fileHistory[i])
            {
                wxString buf;
                buf.Printf("&%d %s", i+1, m_fileHistory[i]);
                menu->Append(wxID_FILE1+i, buf);
            }
        }
    }
}

#if 0
/*
 * wxPrintInfo
 */

wxPrintInfo::wxPrintInfo(void)
{
  pageNumber = 1;
}

wxPrintInfo::~wxPrintInfo(void)
{
}
#endif

/*
 * Permits compatibility with existing file formats and functions
 * that manipulate files directly
 */
 
bool wxTransferFileToStream(const wxString& filename, ostream& stream)
{
  FILE *fd1;
  int ch;

  if ((fd1 = fopen (WXSTRINGCAST filename, "rb")) == NULL)
    return FALSE;

  while ((ch = getc (fd1)) != EOF)
    stream << (unsigned char)ch;

  fclose (fd1);
  return TRUE;
}

bool wxTransferStreamToFile(istream& stream, const wxString& filename)
{
  FILE *fd1;
  int ch;

  if ((fd1 = fopen (WXSTRINGCAST filename, "wb")) == NULL)
    {
      return FALSE;
    }

  while (!stream.eof())
  {
    ch = stream.get();
    if (!stream.eof())
      putc (ch, fd1);
  }
  fclose (fd1);
  return TRUE;
}

#endif
  // End wxUSE_DOC_VIEW_ARCHITECTURE
