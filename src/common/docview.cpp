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

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "docview.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
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
    #include "wx/intl.h"
#endif


#ifdef __WXGTK__
    #include "wx/mdi.h"
#endif

#if wxUSE_PRINTING_ARCHITECTURE
  #include "wx/prntbase.h"
  #include "wx/printdlg.h"
#endif

#include "wx/msgdlg.h"
#include "wx/choicdlg.h"
#include "wx/docview.h"
#include "wx/confbase.h"
#include "wx/file.h"

#include <stdio.h>
#include <string.h>

#if wxUSE_STD_IOSTREAM
  #include "wx/ioswrap.h"
  #if wxUSE_IOSTREAMH
    #include <fstream.h>
  #else
    #include <fstream>
  #endif
#else
  #include "wx/wfstream.h"
#endif

// ----------------------------------------------------------------------------
// wxWindows macros
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// function prototypes
// ----------------------------------------------------------------------------

static inline wxString FindExtension(const wxChar *path);

// ----------------------------------------------------------------------------
// local constants
// ----------------------------------------------------------------------------

static const wxChar *s_MRUEntryFormat = wxT("&%d %s");

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

static wxString FindExtension(const wxChar *path)
{
    wxString ext;
    wxSplitPath(path, NULL, NULL, &ext);

    // VZ: extensions are considered not case sensitive - is this really a good
    //     idea?
    return ext.MakeLower();
}

// ----------------------------------------------------------------------------
// Definition of wxDocument
// ----------------------------------------------------------------------------

wxDocument::wxDocument(wxDocument *parent)
{
    m_documentModified = FALSE;
    m_documentParent = parent;
    m_documentTemplate = (wxDocTemplate *) NULL;
    m_commandProcessor = (wxCommandProcessor*) NULL;
    m_savedYet = FALSE;
}

bool wxDocument::DeleteContents()
{
    return TRUE;
}

wxDocument::~wxDocument()
{
    DeleteContents();

    if (m_commandProcessor)
        delete m_commandProcessor;

    if (GetDocumentManager())
        GetDocumentManager()->RemoveDocument(this);

    // Not safe to do here, since it'll invoke virtual view functions
    // expecting to see valid derived objects: and by the time we get here,
    // we've called destructors higher up.
    //DeleteAllViews();
}

bool wxDocument::Close()
{
    if (OnSaveModified())
        return OnCloseDocument();
    else
        return FALSE;
}

bool wxDocument::OnCloseDocument()
{
    DeleteContents();
    Modify(FALSE);
    return TRUE;
}

// Note that this implicitly deletes the document when the last view is
// deleted.
bool wxDocument::DeleteAllViews()
{
    wxDocManager* manager = GetDocumentManager();

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
    // If we haven't yet deleted the document (for example
    // if there were no views) then delete it.
    if (manager && manager->GetDocuments().Member(this))
        delete this;

    return TRUE;
}

wxView *wxDocument::GetFirstView() const
{
    if (m_documentViews.Number() == 0)
        return (wxView *) NULL;
    return (wxView *)m_documentViews.First()->Data();
}

wxDocManager *wxDocument::GetDocumentManager() const
{
    return (m_documentTemplate ? m_documentTemplate->GetDocumentManager() : (wxDocManager*) NULL);
}

bool wxDocument::OnNewDocument()
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

bool wxDocument::Save()
{
    bool ret = FALSE;

    if (!IsModified()) return TRUE;
    if (m_documentFile == wxT("") || !m_savedYet)
        ret = SaveAs();
    else
        ret = OnSaveDocument(m_documentFile);
    if ( ret )
        SetDocumentSaved(TRUE);
    return ret;
}

bool wxDocument::SaveAs()
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
    wxString path, name, ext;
    wxSplitPath(fileName, & path, & name, & ext);

    if (ext.IsEmpty() || ext == wxT(""))
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
    if ( !file )
        return FALSE;

    wxString msgTitle;
    if (wxTheApp->GetAppName() != wxT(""))
        msgTitle = wxTheApp->GetAppName();
    else
        msgTitle = wxString(_("File error"));

#if wxUSE_STD_IOSTREAM
    ofstream store(wxString(file.fn_str()).mb_str());
    if (store.fail() || store.bad())
#else
    wxFileOutputStream store(wxString(file.fn_str()));
    if (store.LastError() != wxSTREAM_NOERROR)
#endif
    {
        (void)wxMessageBox(_("Sorry, could not open this file for saving."), msgTitle, wxOK | wxICON_EXCLAMATION,
                           GetDocumentWindow());
        // Saving error
        return FALSE;
    }
    if (!SaveObject(store))
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
    if (wxTheApp->GetAppName() != wxT(""))
        msgTitle = wxTheApp->GetAppName();
    else
        msgTitle = wxString(_("File error"));

#if wxUSE_STD_IOSTREAM
    ifstream store(wxString(file.fn_str()).mb_str());
    if (store.fail() || store.bad())
#else
    wxFileInputStream store(wxString(file.fn_str()));
    if (store.LastError() != wxSTREAM_NOERROR)
#endif
    {
        (void)wxMessageBox(_("Sorry, could not open this file."), msgTitle, wxOK|wxICON_EXCLAMATION,
                           GetDocumentWindow());
        return FALSE;
    }
#if wxUSE_STD_IOSTREAM
    if (!LoadObject(store))
#else
    int res = LoadObject(store).LastError();
    if ((res != wxSTREAM_NOERROR) &&
        (res != wxSTREAM_EOF))
#endif
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

#if wxUSE_STD_IOSTREAM
istream& wxDocument::LoadObject(istream& stream)
#else
wxInputStream& wxDocument::LoadObject(wxInputStream& stream)
#endif
{
    return stream;
}

#if wxUSE_STD_IOSTREAM
ostream& wxDocument::SaveObject(ostream& stream)
#else
wxOutputStream& wxDocument::SaveObject(wxOutputStream& stream)
#endif
{
    return stream;
}

bool wxDocument::Revert()
{
    return FALSE;
}


// Get title, or filename if no title, else unnamed
bool wxDocument::GetPrintableName(wxString& buf) const
{
    if (m_documentTitle != wxT(""))
    {
        buf = m_documentTitle;
        return TRUE;
    }
    else if (m_documentFile != wxT(""))
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

wxWindow *wxDocument::GetDocumentWindow() const
{
    wxView *view = GetFirstView();
    if (view)
        return view->GetFrame();
    else
        return wxTheApp->GetTopWindow();
}

wxCommandProcessor *wxDocument::OnCreateCommandProcessor()
{
    return new wxCommandProcessor;
}

// TRUE if safe to close
bool wxDocument::OnSaveModified()
{
    if (IsModified())
    {
        wxString title;
        GetPrintableName(title);

        wxString msgTitle;
        if (wxTheApp->GetAppName() != wxT(""))
            msgTitle = wxTheApp->GetAppName();
        else
            msgTitle = wxString(_("Warning"));

        wxString prompt;
        prompt.Printf(_("Do you want to save changes to document %s?"),
                (const wxChar *)title);
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
void wxDocument::OnChangedViewList()
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

// ----------------------------------------------------------------------------
// Document view
// ----------------------------------------------------------------------------

wxView::wxView()
{
    //  SetDocument(doc);
    m_viewDocument = (wxDocument*) NULL;

    m_viewTypeName = wxT("");
    m_viewFrame = (wxFrame *) NULL;
}

wxView::~wxView()
{
//    GetDocumentManager()->ActivateView(this, FALSE, TRUE);
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

void wxView::OnChangeFilename()
{
    if (GetFrame() && GetDocument())
    {
        wxString title;

        GetDocument()->GetPrintableName(title);

        GetFrame()->SetTitle(title);
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
wxPrintout *wxView::OnCreatePrintout()
{
    return new wxDocPrintout(this);
}
#endif // wxUSE_PRINTING_ARCHITECTURE

// ----------------------------------------------------------------------------
// wxDocTemplate
// ----------------------------------------------------------------------------

wxDocTemplate::wxDocTemplate(wxDocManager *manager,
                             const wxString& descr,
                             const wxString& filter,
                             const wxString& dir,
                             const wxString& ext,
                             const wxString& docTypeName,
                             const wxString& viewTypeName,
                             wxClassInfo *docClassInfo,
                             wxClassInfo *viewClassInfo,
                             long flags)
{
    m_documentManager = manager;
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

wxDocTemplate::~wxDocTemplate()
{
    m_documentManager->DisassociateTemplate(this);
}

// Tries to dynamically construct an object of the right class.
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
        if (GetDocumentManager()->GetDocuments().Member(doc))
            doc->DeleteAllViews();
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

// The default (very primitive) format detection: check is the extension is
// that of the template
bool wxDocTemplate::FileMatchesTemplate(const wxString& path)
{
    return GetDefaultExtension().IsSameAs(FindExtension(path));
}

// ----------------------------------------------------------------------------
// wxDocManager
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDocManager, wxEvtHandler)
    EVT_MENU(wxID_OPEN, wxDocManager::OnFileOpen)
    EVT_MENU(wxID_CLOSE, wxDocManager::OnFileClose)
    EVT_MENU(wxID_REVERT, wxDocManager::OnFileRevert)
    EVT_MENU(wxID_NEW, wxDocManager::OnFileNew)
    EVT_MENU(wxID_SAVE, wxDocManager::OnFileSave)
    EVT_MENU(wxID_SAVEAS, wxDocManager::OnFileSaveAs)
    EVT_MENU(wxID_UNDO, wxDocManager::OnUndo)
    EVT_MENU(wxID_REDO, wxDocManager::OnRedo)

    EVT_UPDATE_UI(wxID_OPEN, wxDocManager::OnUpdateFileOpen)
    EVT_UPDATE_UI(wxID_CLOSE, wxDocManager::OnUpdateFileClose)
    EVT_UPDATE_UI(wxID_REVERT, wxDocManager::OnUpdateFileRevert)
    EVT_UPDATE_UI(wxID_NEW, wxDocManager::OnUpdateFileNew)
    EVT_UPDATE_UI(wxID_SAVE, wxDocManager::OnUpdateFileSave)
    EVT_UPDATE_UI(wxID_SAVEAS, wxDocManager::OnUpdateFileSaveAs)
    EVT_UPDATE_UI(wxID_UNDO, wxDocManager::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxDocManager::OnUpdateRedo)

#if wxUSE_PRINTING_ARCHITECTURE
    EVT_MENU(wxID_PRINT, wxDocManager::OnPrint)
    EVT_MENU(wxID_PRINT_SETUP, wxDocManager::OnPrintSetup)
    EVT_MENU(wxID_PREVIEW, wxDocManager::OnPreview)

    EVT_UPDATE_UI(wxID_PRINT, wxDocManager::OnUpdatePrint)
    EVT_UPDATE_UI(wxID_PRINT_SETUP, wxDocManager::OnUpdatePrintSetup)
    EVT_UPDATE_UI(wxID_PREVIEW, wxDocManager::OnUpdatePreview)
#endif
END_EVENT_TABLE()

wxDocManager* wxDocManager::sm_docManager = (wxDocManager*) NULL;

wxDocManager::wxDocManager(long flags, bool initialize)
{
    m_defaultDocumentNameCounter = 1;
    m_flags = flags;
    m_currentView = (wxView *) NULL;
    m_maxDocsOpen = 10000;
    m_fileHistory = (wxFileHistory *) NULL;
    if (initialize)
        Initialize();
    sm_docManager = this;
}

wxDocManager::~wxDocManager()
{
    Clear();
    if (m_fileHistory)
        delete m_fileHistory;
    sm_docManager = (wxDocManager*) NULL;
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

bool wxDocManager::Initialize()
{
    m_fileHistory = OnCreateFileHistory();
    return TRUE;
}

wxFileHistory *wxDocManager::OnCreateFileHistory()
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
    if ( !CreateDocument(wxString(""), 0) )
    {
        OnOpenFileFailure();
    }
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
#if wxUSE_PRINTING_ARCHITECTURE
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
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void wxDocManager::OnPrintSetup(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_PRINTING_ARCHITECTURE
    wxWindow *parentWin = wxTheApp->GetTopWindow();
    wxView *view = GetCurrentView();
    if (view)
        parentWin = view->GetFrame();

    wxPrintDialogData data;

    wxPrintDialog printerDialog(parentWin, &data);
    printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
    printerDialog.ShowModal();
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void wxDocManager::OnPreview(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_PRINTING_ARCHITECTURE
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
#endif // wxUSE_PRINTING_ARCHITECTURE
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

// Handlers for UI update commands

void wxDocManager::OnUpdateFileOpen(wxUpdateUIEvent& event)
{
    event.Enable( TRUE );
}

void wxDocManager::OnUpdateFileClose(wxUpdateUIEvent& event)
{
    wxDocument *doc = GetCurrentDocument();
    event.Enable( (doc != (wxDocument*) NULL) );
}

void wxDocManager::OnUpdateFileRevert(wxUpdateUIEvent& event)
{
    wxDocument *doc = GetCurrentDocument();
    event.Enable( (doc != (wxDocument*) NULL) );
}

void wxDocManager::OnUpdateFileNew(wxUpdateUIEvent& event)
{
    event.Enable( TRUE );
}

void wxDocManager::OnUpdateFileSave(wxUpdateUIEvent& event)
{
    wxDocument *doc = GetCurrentDocument();
    event.Enable( (doc != (wxDocument*) NULL) );
}

void wxDocManager::OnUpdateFileSaveAs(wxUpdateUIEvent& event)
{
    wxDocument *doc = GetCurrentDocument();
    event.Enable( (doc != (wxDocument*) NULL) );
}

void wxDocManager::OnUpdateUndo(wxUpdateUIEvent& event)
{
    wxDocument *doc = GetCurrentDocument();
    event.Enable( (doc && doc->GetCommandProcessor() && doc->GetCommandProcessor()->CanUndo()) );
}

void wxDocManager::OnUpdateRedo(wxUpdateUIEvent& event)
{
    wxDocument *doc = GetCurrentDocument();
    event.Enable( (doc && doc->GetCommandProcessor() && doc->GetCommandProcessor()->CanRedo()) );
}

void wxDocManager::OnUpdatePrint(wxUpdateUIEvent& event)
{
    wxDocument *doc = GetCurrentDocument();
    event.Enable( (doc != (wxDocument*) NULL) );
}

void wxDocManager::OnUpdatePrintSetup(wxUpdateUIEvent& event)
{
    event.Enable( TRUE );
}

void wxDocManager::OnUpdatePreview(wxUpdateUIEvent& event)
{
    wxDocument *doc = GetCurrentDocument();
    event.Enable( (doc != (wxDocument*) NULL) );
}

wxView *wxDocManager::GetCurrentView() const
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

    wxString path2(wxT(""));
    if (path != wxT(""))
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
                newDoc->DeleteAllViews();
                // delete newDoc; // Implicitly deleted by DeleteAllViews
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

wxDocument *wxDocManager::GetCurrentDocument() const
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

// Make a frame title (override this to do something different)
// If docName is empty, a document is not currently active.
wxString wxDocManager::MakeFrameTitle(wxDocument* doc)
{
    wxString appName = wxTheApp->GetAppName();
    wxString title;
    if (!doc)
        title = appName;
    else
    {
        wxString docName;
        doc->GetPrintableName(docName);
        title = docName + wxString(_(" - ")) + appName;
    }
    return title;
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

void wxDocManager::RemoveFileFromHistory(int i)
{
    if (m_fileHistory)
        m_fileHistory->RemoveFileFromHistory(i);
}

wxString wxDocManager::GetHistoryFile(int i) const
{
    wxString histFile;

    if (m_fileHistory)
        histFile = m_fileHistory->GetHistoryFile(i);

    return histFile;
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

int wxDocManager::GetNoHistoryFiles() const
{
    if (m_fileHistory)
        return m_fileHistory->GetNoHistoryFiles();
    else
        return 0;
}


// Find out the document template via matching in the document file format
// against that of the template
wxDocTemplate *wxDocManager::FindTemplateForPath(const wxString& path)
{
    wxDocTemplate *theTemplate = (wxDocTemplate *) NULL;

    // Find the template which this extension corresponds to
    int i;
    for (i = 0; i < m_templates.Number(); i++)
    {
        wxDocTemplate *temp = (wxDocTemplate *)m_templates.Nth(i)->Data();
        if ( temp->FileMatchesTemplate(path) )
        {
            theTemplate = temp;
            break;
        }
    }
    return theTemplate;
}

// Try to get a more suitable parent frame than the top window,
// for selection dialogs. Otherwise you may get an unexpected
// window being activated when a dialog is shown.
static wxWindow* wxFindSuitableParent()
{
    wxWindow* parent = wxTheApp->GetTopWindow();

    wxWindow* focusWindow = wxWindow::FindFocus();
    if (focusWindow)
    {
        while (focusWindow &&
                !focusWindow->IsKindOf(CLASSINFO(wxDialog)) &&
                !focusWindow->IsKindOf(CLASSINFO(wxFrame)))

            focusWindow = focusWindow->GetParent();

        if (focusWindow)
            parent = focusWindow;
    }
    return parent;
}

// Prompts user to open a file, using file specs in templates.
// How to implement in wxWindows? Must extend the file selector
// dialog or implement own; OR match the extension to the
// template extension.

wxDocTemplate *wxDocManager::SelectDocumentPath(wxDocTemplate **templates,
#if defined(__WXMSW__) || defined(__WXGTK__)
                                                int noTemplates,
#else
                                                int WXUNUSED(noTemplates),
#endif
                                                wxString& path,
                                                long WXUNUSED(flags),
                                                bool WXUNUSED(save))
{
    // We can only have multiple filters in Windows and GTK
#if defined(__WXMSW__) || defined(__WXGTK__)
    wxString descrBuf;

    int i;
    for (i = 0; i < noTemplates; i++)
    {
        if (templates[i]->IsVisible())
        {
            // add a '|' to separate this filter from the previous one
            if ( !descrBuf.IsEmpty() )
                descrBuf << wxT('|');

            descrBuf << templates[i]->GetDescription()
                << wxT(" (") << templates[i]->GetFileFilter() << wxT(") |")
                << templates[i]->GetFileFilter();
        }
    }
#else
    wxString descrBuf = wxT("*.*");
#endif

    int FilterIndex = -1;

    wxWindow* parent = wxFindSuitableParent();

    wxString pathTmp = wxFileSelectorEx(_("Select a file"),
                                        m_lastDirectory,
                                        wxT(""),
                                        &FilterIndex,
                                        descrBuf,
                                        0,
                                        parent);

    if (!pathTmp.IsEmpty())
    {
        if (!wxFileExists(pathTmp))
        {
            wxString msgTitle;
            if (!wxTheApp->GetAppName().IsEmpty())
                msgTitle = wxTheApp->GetAppName();
            else
                msgTitle = wxString(_("File error"));
            
            (void)wxMessageBox(_("Sorry, could not open this file."), msgTitle, wxOK | wxICON_EXCLAMATION,
                parent);

            path = wxT("");
            return (wxDocTemplate *) NULL;
        }
        m_lastDirectory = wxPathOnly(pathTmp);

        path = pathTmp;

        // first choose the template using the extension, if this fails (i.e.
        // wxFileSelectorEx() didn't fill it), then use the path
        wxDocTemplate *theTemplate = (wxDocTemplate *)NULL;
        if ( FilterIndex != -1 )
            theTemplate = templates[FilterIndex];
        if ( !theTemplate )
            theTemplate = FindTemplateForPath(path);

        return theTemplate;
    }
    else
    {
        path = wxT("");
        return (wxDocTemplate *) NULL;
    }
#if 0
    // In all other windowing systems, until we have more advanced
    // file selectors, we must select the document type (template) first, and
    // _then_ pop up the file selector.
    wxDocTemplate *temp = SelectDocumentType(templates, noTemplates);
    if (!temp)
        return (wxDocTemplate *) NULL;

    wxChar *pathTmp = wxFileSelector(_("Select a file"), wxT(""), wxT(""),
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
#endif // 0
}

wxDocTemplate *wxDocManager::SelectDocumentType(wxDocTemplate **templates,
                                                int noTemplates)
{
    wxChar **strings = new wxChar *[noTemplates];
    wxChar **data = new wxChar *[noTemplates];
    int i;
    int n = 0;
    for (i = 0; i < noTemplates; i++)
    {
        if (templates[i]->IsVisible())
        {
            strings[n] = (wxChar *)templates[i]->m_description.c_str();
            data[n] = (wxChar *)templates[i];
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

    wxWindow* parent = wxFindSuitableParent();

    wxDocTemplate *theTemplate = (wxDocTemplate *)wxGetSingleChoiceData(_("Select a document template"), _("Templates"), n,
            strings, (void **)data, parent);
    delete[] strings;
    delete[] data;
    return theTemplate;
}

wxDocTemplate *wxDocManager::SelectViewType(wxDocTemplate **templates,
        int noTemplates)
{
    wxChar **strings = new wxChar *[noTemplates];
    wxChar **data = new wxChar *[noTemplates];
    int i;
    int n = 0;
    for (i = 0; i < noTemplates; i++)
    {
        if (templates[i]->IsVisible() && (templates[i]->GetViewName() != wxT("")))
        {
            strings[n] = (wxChar *)templates[i]->m_viewTypeName.c_str();
            data[n] = (wxChar *)templates[i];
            n ++;
        }
    }
    wxWindow* parent = wxFindSuitableParent();

    wxDocTemplate *theTemplate = (wxDocTemplate *)wxGetSingleChoiceData(_("Select a document view"), _("Views"), n,
            strings, (void **)data, parent);
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

// ----------------------------------------------------------------------------
// Default document child frame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDocChildFrame, wxFrame)
    EVT_ACTIVATE(wxDocChildFrame::OnActivate)
    EVT_CLOSE(wxDocChildFrame::OnCloseWindow)
END_EVENT_TABLE()

wxDocChildFrame::wxDocChildFrame(wxDocument *doc,
                                 wxView *view,
                                 wxFrame *frame,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
               : wxFrame(frame, id, title, pos, size, style, name)
{
    m_childDocument = doc;
    m_childView = view;
    if (view)
        view->SetFrame(this);
}

wxDocChildFrame::~wxDocChildFrame()
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

// ----------------------------------------------------------------------------
// Default parent frame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDocParentFrame, wxFrame)
    EVT_MENU(wxID_EXIT, wxDocParentFrame::OnExit)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, wxDocParentFrame::OnMRUFile)
    EVT_CLOSE(wxDocParentFrame::OnCloseWindow)
END_EVENT_TABLE()

wxDocParentFrame::wxDocParentFrame(wxDocManager *manager,
                                   wxFrame *frame,
                                   wxWindowID id,
                                   const wxString& title,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxString& name)
                : wxFrame(frame, id, title, pos, size, style, name)
{
    m_docManager = manager;
}

void wxDocParentFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void wxDocParentFrame::OnMRUFile(wxCommandEvent& event)
{
    int n = event.GetSelection() - wxID_FILE1;  // the index in MRU list
    wxString filename(m_docManager->GetHistoryFile(n));
    if ( !filename.IsEmpty() )
    {
        // verify that the file exists before doing anything else
        if ( wxFile::Exists(filename) )
        {
            // try to open it
            (void)m_docManager->CreateDocument(filename, wxDOC_SILENT);
        }
        else
        {
            // remove the bogus filename from the MRU list and notify the user
            // about it
            m_docManager->RemoveFileFromHistory(n);

            wxLogError(_("The file '%s' doesn't exist and couldn't be opened.\nIt has been also removed from the MRU files list."),
                       filename.c_str());
        }
    }
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

wxDocPrintout::wxDocPrintout(wxView *view, const wxString& title)
             : wxPrintout(title)
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

#endif // wxUSE_PRINTING_ARCHITECTURE

// ----------------------------------------------------------------------------
// Command processing framework
// ----------------------------------------------------------------------------

wxCommand::wxCommand(bool canUndoIt, const wxString& name)
{
    m_canUndo = canUndoIt;
    m_commandName = name;
}

wxCommand::~wxCommand()
{
}

// Command processor
wxCommandProcessor::wxCommandProcessor(int maxCommands)
{
    m_maxNoCommands = maxCommands;
    m_currentCommand = (wxNode *) NULL;
    m_commandEditMenu = (wxMenu *) NULL;
}

wxCommandProcessor::~wxCommandProcessor()
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

bool wxCommandProcessor::Undo()
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

bool wxCommandProcessor::Redo()
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

bool wxCommandProcessor::CanUndo() const
{
    if (m_currentCommand)
        return ((wxCommand *)m_currentCommand->Data())->CanUndo();
    return FALSE;
}

bool wxCommandProcessor::CanRedo() const
{
    if ((m_currentCommand != (wxNode*) NULL) && (m_currentCommand->Next() == (wxNode*) NULL))
        return FALSE;

    if ((m_currentCommand != (wxNode*) NULL) && (m_currentCommand->Next() != (wxNode*) NULL))
        return TRUE;

    if ((m_currentCommand == (wxNode*) NULL) && (m_commands.Number() > 0))
        return TRUE;

    return FALSE;
}

void wxCommandProcessor::Initialize()
{
    m_currentCommand = m_commands.Last();
    SetMenuStrings();
}

void wxCommandProcessor::SetMenuStrings()
{
    if (m_commandEditMenu)
    {
        wxString buf;
        if (m_currentCommand)
        {
            wxCommand *command = (wxCommand *)m_currentCommand->Data();
            wxString commandName(command->GetName());
            if (commandName == wxT("")) commandName = _("Unnamed command");
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
                if (redoCommandName == wxT("")) redoCommandName = _("Unnamed command");
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
                if (redoCommandName == wxT("")) redoCommandName = _("Unnamed command");
                buf = wxString(_("&Redo ")) + redoCommandName;
                m_commandEditMenu->SetLabel(wxID_REDO, buf);
                m_commandEditMenu->Enable(wxID_REDO, TRUE);
            }
        }
    }
}

void wxCommandProcessor::ClearCommands()
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

// ----------------------------------------------------------------------------
// File history processor
// ----------------------------------------------------------------------------

wxFileHistory::wxFileHistory(int maxFiles)
{
    m_fileMaxFiles = maxFiles;
    m_fileHistoryN = 0;
    m_fileHistory = new wxChar *[m_fileMaxFiles];
}

wxFileHistory::~wxFileHistory()
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
        m_fileHistory[m_fileMaxFiles-1] = (wxChar *) NULL;
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
            buf.Printf(s_MRUEntryFormat, i+1, m_fileHistory[i]);
            wxNode* node = m_fileMenus.First();
            while (node)
            {
                wxMenu* menu = (wxMenu*) node->Data();
                menu->SetLabel(wxID_FILE1+i, buf);
                node = node->Next();
            }
        }
}

void wxFileHistory::RemoveFileFromHistory(int i)
{
    wxCHECK_RET( i < m_fileHistoryN,
                 wxT("invalid index in wxFileHistory::RemoveFileFromHistory") );

    wxNode* node = m_fileMenus.First();
    while ( node )
    {
        wxMenu* menu = (wxMenu*) node->Data();

        // delete the element from the array (could use memmove() too...)
        delete [] m_fileHistory[i];

        int j;
        for ( j = i; j < m_fileHistoryN - 1; j++ )
        {
            m_fileHistory[j] = m_fileHistory[j + 1];
        }

        // shuffle filenames up
        wxString buf;
        for ( j = i; j < m_fileHistoryN - 1; j++ )
        {
            buf.Printf(s_MRUEntryFormat, j + 1, m_fileHistory[j]);
            menu->SetLabel(wxID_FILE1 + j, buf);
        }

        node = node->Next();

        // delete the last menu item which is unused now
        menu->Delete(wxID_FILE1 + m_fileHistoryN - 1);

        // delete the last separator too if no more files are left
        if ( m_fileHistoryN == 1 )
        {
            wxMenuItemList::Node *node = menu->GetMenuItems().GetLast();
            if ( node )
            {
                wxMenuItem *menuItem = node->GetData();
                if ( menuItem->IsSeparator() )
                {
                    menu->Delete(menuItem);
                }
                //else: should we search backwards for the last separator?
            }
            //else: menu is empty somehow
        }
    }

    m_fileHistoryN--;
}

wxString wxFileHistory::GetHistoryFile(int i) const
{
    wxString s;
    if ( i < m_fileHistoryN )
    {
        s = m_fileHistory[i];
    }
    else
    {
        wxFAIL_MSG( wxT("bad index in wxFileHistory::GetHistoryFile") );
    }

    return s;
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
    buf.Printf(wxT("file%d"), m_fileHistoryN+1);
    wxString historyFile;
    while ((m_fileHistoryN <= m_fileMaxFiles) && config.Read(buf, &historyFile) && (historyFile != wxT("")))
    {
        m_fileHistory[m_fileHistoryN] = copystring((const wxChar*) historyFile);
        m_fileHistoryN ++;
        buf.Printf(wxT("file%d"), m_fileHistoryN+1);
        historyFile = wxT("");
    }
    AddFilesToMenu();
}

void wxFileHistory::Save(wxConfigBase& config)
{
    int i;
    for (i = 0; i < m_fileHistoryN; i++)
    {
        wxString buf;
        buf.Printf(wxT("file%d"), i+1);
        config.Write(buf, wxString(m_fileHistory[i]));
    }
}
#endif // wxUSE_CONFIG

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
                    buf.Printf(s_MRUEntryFormat, i+1, m_fileHistory[i]);
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
                buf.Printf(s_MRUEntryFormat, i+1, m_fileHistory[i]);
                menu->Append(wxID_FILE1+i, buf);
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Permits compatibility with existing file formats and functions that
// manipulate files directly
// ----------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
bool wxTransferFileToStream(const wxString& filename, ostream& stream)
{
    FILE *fd1;
    int ch;

    if ((fd1 = wxFopen (filename.fn_str(), _T("rb"))) == NULL)
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

    if ((fd1 = wxFopen (filename.fn_str(), _T("wb"))) == NULL)
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
#else
bool wxTransferFileToStream(const wxString& filename, wxOutputStream& stream)
{
    FILE *fd1;
    int ch;

    if ((fd1 = wxFopen (filename.fn_str(), wxT("rb"))) == NULL)
        return FALSE;

    while ((ch = getc (fd1)) != EOF)
        stream.PutC((char) ch);

    fclose (fd1);
    return TRUE;
}

bool wxTransferStreamToFile(wxInputStream& stream, const wxString& filename)
{
    FILE *fd1;
    char ch;

    if ((fd1 = wxFopen (filename.fn_str(), wxT("wb"))) == NULL)
    {
        return FALSE;
    }

    int len = stream.StreamSize();
    // TODO: is this the correct test for EOF?
    while (stream.TellI() < (len - 1))
    {
        ch = stream.GetC();
        putc (ch, fd1);
    }
    fclose (fd1);
    return TRUE;
}
#endif

#endif // wxUSE_DOC_VIEW_ARCHITECTURE

