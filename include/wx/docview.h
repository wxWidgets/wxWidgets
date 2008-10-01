/////////////////////////////////////////////////////////////////////////////
// Name:        wx/docview.h
// Purpose:     Doc/View classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCH__
#define _WX_DOCH__

#include "wx/defs.h"

#if wxUSE_DOC_VIEW_ARCHITECTURE

#include "wx/list.h"
#include "wx/string.h"
#include "wx/frame.h"

#if wxUSE_PRINTING_ARCHITECTURE
    #include "wx/print.h"
#endif

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxDocument;
class WXDLLIMPEXP_FWD_CORE wxView;
class WXDLLIMPEXP_FWD_CORE wxDocTemplate;
class WXDLLIMPEXP_FWD_CORE wxDocManager;
class WXDLLIMPEXP_FWD_CORE wxPrintInfo;
class WXDLLIMPEXP_FWD_CORE wxCommandProcessor;
class WXDLLIMPEXP_FWD_CORE wxFileHistory;
class WXDLLIMPEXP_FWD_BASE wxConfigBase;

#if wxUSE_STD_IOSTREAM
  #include "wx/iosfwrap.h"
#else
  #include "wx/stream.h"
#endif

// Flags for wxDocManager (can be combined).
enum
{
    wxDOC_NEW    = 1,
    wxDOC_SILENT = 2
};

// Document template flags
enum
{
    wxTEMPLATE_VISIBLE = 1,
    wxTEMPLATE_INVISIBLE,
    wxDEFAULT_TEMPLATE_FLAGS = wxTEMPLATE_VISIBLE
};

#define wxMAX_FILE_HISTORY 9

class WXDLLIMPEXP_CORE wxDocument : public wxEvtHandler
{
public:
    wxDocument(wxDocument *parent = NULL);
    virtual ~wxDocument();

    // accessors
    void SetFilename(const wxString& filename, bool notifyViews = false);
    wxString GetFilename() const { return m_documentFile; }

    void SetTitle(const wxString& title) { m_documentTitle = title; }
    wxString GetTitle() const { return m_documentTitle; }

    void SetDocumentName(const wxString& name) { m_documentTypeName = name; }
    wxString GetDocumentName() const { return m_documentTypeName; }

    bool GetDocumentSaved() const { return m_savedYet; }
    void SetDocumentSaved(bool saved = true) { m_savedYet = saved; }

    virtual bool Close();
    virtual bool Save();
    virtual bool SaveAs();
    virtual bool Revert();

#if wxUSE_STD_IOSTREAM
    virtual wxSTD ostream& SaveObject(wxSTD ostream& stream);
    virtual wxSTD istream& LoadObject(wxSTD istream& stream);
#else
    virtual wxOutputStream& SaveObject(wxOutputStream& stream);
    virtual wxInputStream& LoadObject(wxInputStream& stream);
#endif

    // Called by wxWidgets
    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnOpenDocument(const wxString& filename);
    virtual bool OnNewDocument();
    virtual bool OnCloseDocument();

    // Prompts for saving if about to close a modified document. Returns true
    // if ok to close the document (may have saved in the meantime, or set
    // modified to false)
    virtual bool OnSaveModified();

    // Called by framework if created automatically by the default document
    // manager: gives document a chance to initialise and (usually) create a
    // view
    virtual bool OnCreate(const wxString& path, long flags);

    // By default, creates a base wxCommandProcessor.
    virtual wxCommandProcessor *OnCreateCommandProcessor();
    virtual wxCommandProcessor *GetCommandProcessor() const { return m_commandProcessor; }
    virtual void SetCommandProcessor(wxCommandProcessor *proc) { m_commandProcessor = proc; }

    // Called after a view is added or removed. The default implementation
    // deletes the document if this is there are no more views.
    virtual void OnChangedViewList();

    virtual bool DeleteContents();

    virtual bool Draw(wxDC&);
    virtual bool IsModified() const { return m_documentModified; }
    virtual void Modify(bool mod) { m_documentModified = mod; }

    virtual bool AddView(wxView *view);
    virtual bool RemoveView(wxView *view);
    wxList& GetViews() { return m_documentViews; }
    const wxList& GetViews() const { return m_documentViews; }
    wxView *GetFirstView() const;

    virtual void UpdateAllViews(wxView *sender = NULL, wxObject *hint = NULL);
    virtual void NotifyClosing();

    // Remove all views (because we're closing the document)
    virtual bool DeleteAllViews();

    // Other stuff
    virtual wxDocManager *GetDocumentManager() const;
    virtual wxDocTemplate *GetDocumentTemplate() const { return m_documentTemplate; }
    virtual void SetDocumentTemplate(wxDocTemplate *temp) { m_documentTemplate = temp; }

    // Get the document name to be shown to the user: the title if there is
    // any, otherwise the filename if the document was saved and, finally,
    // "unnamed" otherwise
    virtual wxString GetUserReadableName() const;

#if WXWIN_COMPATIBILITY_2_8
    // use GetUserReadableName() instead
    wxDEPRECATED_BUT_USED_INTERNALLY(
        virtual bool GetPrintableName(wxString& buf) const
    );
#endif // WXWIN_COMPATIBILITY_2_8

    // Returns a window that can be used as a parent for document-related
    // dialogs. Override if necessary.
    virtual wxWindow *GetDocumentWindow() const;

protected:
    wxList                m_documentViews;
    wxString              m_documentFile;
    wxString              m_documentTitle;
    wxString              m_documentTypeName;
    wxDocTemplate*        m_documentTemplate;
    bool                  m_documentModified;
    wxDocument*           m_documentParent;
    wxCommandProcessor*   m_commandProcessor;
    bool                  m_savedYet;

    // Called by OnSaveDocument and OnOpenDocument to implement standard
    // Save/Load behavior. Re-implement in derived class for custom
    // behavior.
    virtual bool DoSaveDocument(const wxString& file);
    virtual bool DoOpenDocument(const wxString& file);

    // the default implementation of GetUserReadableName()
    wxString DoGetUserReadableName() const;

private:
    DECLARE_ABSTRACT_CLASS(wxDocument)
    DECLARE_NO_COPY_CLASS(wxDocument)
};

class WXDLLIMPEXP_CORE wxView: public wxEvtHandler
{
public:
    wxView();
    virtual ~wxView();

    wxDocument *GetDocument() const { return m_viewDocument; }
    virtual void SetDocument(wxDocument *doc);

    wxString GetViewName() const { return m_viewTypeName; }
    void SetViewName(const wxString& name) { m_viewTypeName = name; }

    wxWindow *GetFrame() const { return m_viewFrame ; }
    void SetFrame(wxWindow *frame) { m_viewFrame = frame; }

    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
    virtual void OnDraw(wxDC *dc) = 0;
    virtual void OnPrint(wxDC *dc, wxObject *info);
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL);
    virtual void OnClosingDocument() {}
    virtual void OnChangeFilename();

    // Called by framework if created automatically by the default document
    // manager class: gives view a chance to initialise
    virtual bool OnCreate(wxDocument *WXUNUSED(doc), long WXUNUSED(flags))
        { return true; }

    // Checks if the view is the last one for the document; if so, asks user
    // to confirm save data (if modified). If ok, deletes itself and returns
    // true.
    virtual bool Close(bool deleteWindow = true);

    // Override to do cleanup/veto close
    virtual bool OnClose(bool deleteWindow);

    // A view's window can call this to notify the view it is (in)active.
    // The function then notifies the document manager.
    virtual void Activate(bool activate);

    wxDocManager *GetDocumentManager() const
        { return m_viewDocument->GetDocumentManager(); }

#if wxUSE_PRINTING_ARCHITECTURE
    virtual wxPrintout *OnCreatePrintout();
#endif

protected:
    // hook the document into event handlers chain here
    virtual bool TryValidator(wxEvent& event);

    wxDocument*       m_viewDocument;
    wxString          m_viewTypeName;
    wxWindow*         m_viewFrame;

private:
    DECLARE_ABSTRACT_CLASS(wxView)
    DECLARE_NO_COPY_CLASS(wxView)
};

// Represents user interface (and other) properties of documents and views
class WXDLLIMPEXP_CORE wxDocTemplate: public wxObject
{

friend class WXDLLIMPEXP_FWD_CORE wxDocManager;

public:
    // Associate document and view types. They're for identifying what view is
    // associated with what template/document type
    wxDocTemplate(wxDocManager *manager,
                  const wxString& descr,
                  const wxString& filter,
                  const wxString& dir,
                  const wxString& ext,
                  const wxString& docTypeName,
                  const wxString& viewTypeName,
                  wxClassInfo *docClassInfo = NULL,
                  wxClassInfo *viewClassInfo = NULL,
                  long flags = wxDEFAULT_TEMPLATE_FLAGS);

    virtual ~wxDocTemplate();

    // By default, these two member functions dynamically creates document and
    // view using dynamic instance construction. Override these if you need a
    // different method of construction.
    virtual wxDocument *CreateDocument(const wxString& path, long flags = 0);
    virtual wxView *CreateView(wxDocument *doc, long flags = 0);

    // Helper method for CreateDocument; also allows you to do your own document
    // creation
    virtual bool InitDocument(wxDocument* doc, const wxString& path, long flags = 0);

    wxString GetDefaultExtension() const { return m_defaultExt; }
    wxString GetDescription() const { return m_description; }
    wxString GetDirectory() const { return m_directory; }
    wxDocManager *GetDocumentManager() const { return m_documentManager; }
    void SetDocumentManager(wxDocManager *manager) { m_documentManager = manager; }
    wxString GetFileFilter() const { return m_fileFilter; }
    long GetFlags() const { return m_flags; }
    virtual wxString GetViewName() const { return m_viewTypeName; }
    virtual wxString GetDocumentName() const { return m_docTypeName; }

    void SetFileFilter(const wxString& filter) { m_fileFilter = filter; }
    void SetDirectory(const wxString& dir) { m_directory = dir; }
    void SetDescription(const wxString& descr) { m_description = descr; }
    void SetDefaultExtension(const wxString& ext) { m_defaultExt = ext; }
    void SetFlags(long flags) { m_flags = flags; }

    bool IsVisible() const { return ((m_flags & wxTEMPLATE_VISIBLE) == wxTEMPLATE_VISIBLE); }

    wxClassInfo* GetDocClassInfo() const { return m_docClassInfo; }
    wxClassInfo* GetViewClassInfo() const { return m_viewClassInfo; }

    virtual bool FileMatchesTemplate(const wxString& path);

protected:
    long              m_flags;
    wxString          m_fileFilter;
    wxString          m_directory;
    wxString          m_description;
    wxString          m_defaultExt;
    wxString          m_docTypeName;
    wxString          m_viewTypeName;
    wxDocManager*     m_documentManager;

    // For dynamic creation of appropriate instances.
    wxClassInfo*      m_docClassInfo;
    wxClassInfo*      m_viewClassInfo;

    // Called by CreateDocument and CreateView to create the actual document/view object.
    // By default uses the ClassInfo provided to the constructor. Override these functions
    // to provide a different method of creation.
    virtual wxDocument *DoCreateDocument();
    virtual wxView *DoCreateView();

private:
    DECLARE_CLASS(wxDocTemplate)
    DECLARE_NO_COPY_CLASS(wxDocTemplate)
};

// One object of this class may be created in an application, to manage all
// the templates and documents.
class WXDLLIMPEXP_CORE wxDocManager: public wxEvtHandler
{
public:
    // NB: flags are unused, don't pass wxDOC_XXX to this ctor
    wxDocManager(long flags = 0, bool initialize = true);
    virtual ~wxDocManager();

    virtual bool Initialize();

    // Handlers for common user commands
    void OnFileClose(wxCommandEvent& event);
    void OnFileCloseAll(wxCommandEvent& event);
    void OnFileNew(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& event);
    void OnFileRevert(wxCommandEvent& event);
    void OnFileSave(wxCommandEvent& event);
    void OnFileSaveAs(wxCommandEvent& event);
    void OnPrint(wxCommandEvent& event);
    void OnPreview(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);

    // Handlers for UI update commands
    void OnUpdateFileOpen(wxUpdateUIEvent& event);
    void OnUpdateDisableIfNoDoc(wxUpdateUIEvent& event);
    void OnUpdateFileNew(wxUpdateUIEvent& event);
    void OnUpdateFileSave(wxUpdateUIEvent& event);
    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUpdateRedo(wxUpdateUIEvent& event);

    // called when file format detection didn't work, can be overridden to do
    // something in this case
    virtual void OnOpenFileFailure() { }

    virtual wxDocument *CreateDocument(const wxString& path, long flags = 0);

    // wrapper around CreateDocument() with a more clear name
    wxDocument *CreateNewDocument()
        { return CreateDocument(wxString(), wxDOC_NEW); }

    virtual wxView *CreateView(wxDocument *doc, long flags = 0);
    virtual void DeleteTemplate(wxDocTemplate *temp, long flags = 0);
    virtual bool FlushDoc(wxDocument *doc);
    virtual wxDocTemplate *MatchTemplate(const wxString& path);
    virtual wxDocTemplate *SelectDocumentPath(wxDocTemplate **templates,
            int noTemplates, wxString& path, long flags, bool save = false);
    virtual wxDocTemplate *SelectDocumentType(wxDocTemplate **templates,
            int noTemplates, bool sort = false);
    virtual wxDocTemplate *SelectViewType(wxDocTemplate **templates,
            int noTemplates, bool sort = false);
    virtual wxDocTemplate *FindTemplateForPath(const wxString& path);

    void AssociateTemplate(wxDocTemplate *temp);
    void DisassociateTemplate(wxDocTemplate *temp);

    wxDocument *GetCurrentDocument() const;

    void SetMaxDocsOpen(int n) { m_maxDocsOpen = n; }
    int GetMaxDocsOpen() const { return m_maxDocsOpen; }

    // Add and remove a document from the manager's list
    void AddDocument(wxDocument *doc);
    void RemoveDocument(wxDocument *doc);

    // closes all currently open documents
    bool CloseDocuments(bool force = true);

    // closes the specified document
    bool CloseDocument(wxDocument* doc, bool force = false);

    // Clear remaining documents and templates
    bool Clear(bool force = true);

    // Views or windows should inform the document manager
    // when a view is going in or out of focus
    virtual void ActivateView(wxView *view, bool activate = true);
    virtual wxView *GetCurrentView() const;

    wxList& GetDocuments() { return m_docs; }
    wxList& GetTemplates() { return m_templates; }

    // Return the default name for a new document (by default returns strings
    // in the form "unnamed <counter>" but can be overridden)
    virtual wxString MakeNewDocumentName();

    // Make a frame title (override this to do something different)
    virtual wxString MakeFrameTitle(wxDocument* doc);

    virtual wxFileHistory *OnCreateFileHistory();
    virtual wxFileHistory *GetFileHistory() const { return m_fileHistory; }

    // File history management
    virtual void AddFileToHistory(const wxString& file);
    virtual void RemoveFileFromHistory(size_t i);
    virtual size_t GetHistoryFilesCount() const;
    virtual wxString GetHistoryFile(size_t i) const;
    virtual void FileHistoryUseMenu(wxMenu *menu);
    virtual void FileHistoryRemoveMenu(wxMenu *menu);
#if wxUSE_CONFIG
    virtual void FileHistoryLoad(const wxConfigBase& config);
    virtual void FileHistorySave(wxConfigBase& config);
#endif // wxUSE_CONFIG

    virtual void FileHistoryAddFilesToMenu();
    virtual void FileHistoryAddFilesToMenu(wxMenu* menu);

    wxString GetLastDirectory() const { return m_lastDirectory; }
    void SetLastDirectory(const wxString& dir) { m_lastDirectory = dir; }

    // Get the current document manager
    static wxDocManager* GetDocumentManager() { return sm_docManager; }

#if WXWIN_COMPATIBILITY_2_8
    // deprecated, override GetDefaultName() instead
    wxDEPRECATED_BUT_USED_INTERNALLY(
        virtual bool MakeDefaultName(wxString& buf)
    );
#endif

#if WXWIN_COMPATIBILITY_2_6
    // deprecated, use GetHistoryFilesCount() instead
    wxDEPRECATED( size_t GetNoHistoryFiles() const );
#endif // WXWIN_COMPATIBILITY_2_6

protected:
    // hook the currently active view into event handlers chain here
    virtual bool TryValidator(wxEvent& event);

    int               m_defaultDocumentNameCounter;
    int               m_maxDocsOpen;
    wxList            m_docs;
    wxList            m_templates;
    wxView*           m_currentView;
    wxFileHistory*    m_fileHistory;
    wxString          m_lastDirectory;
    static wxDocManager* sm_docManager;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxDocManager)
    DECLARE_NO_COPY_CLASS(wxDocManager)
};

#if WXWIN_COMPATIBILITY_2_6
inline size_t wxDocManager::GetNoHistoryFiles() const
{
    return GetHistoryFilesCount();
}
#endif // WXWIN_COMPATIBILITY_2_6

// ----------------------------------------------------------------------------
// A default child frame
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDocChildFrame : public wxFrame
{
public:
    wxDocChildFrame(wxDocument *doc,
                    wxView *view,
                    wxFrame *frame,
                    wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long type = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxT("frame"));
    virtual ~wxDocChildFrame(){}

    void OnActivate(wxActivateEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    wxDocument *GetDocument() const { return m_childDocument; }
    wxView *GetView() const { return m_childView; }
    void SetDocument(wxDocument *doc) { m_childDocument = doc; }
    void SetView(wxView *view) { m_childView = view; }
    bool Destroy() { m_childView = NULL; return wxFrame::Destroy(); }

protected:
    // hook the child view into event handlers chain here
    virtual bool TryValidator(wxEvent& event);

    wxDocument*       m_childDocument;
    wxView*           m_childView;

private:
    DECLARE_CLASS(wxDocChildFrame)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxDocChildFrame)
};

// ----------------------------------------------------------------------------
// A default parent frame
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDocParentFrame : public wxFrame
{
public:
    wxDocParentFrame();
    wxDocParentFrame(wxDocManager *manager,
                     wxFrame *frame,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = wxFrameNameStr);

    bool Create(wxDocManager *manager,
                wxFrame *frame,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    wxDocManager *GetDocumentManager() const { return m_docManager; }

    void OnExit(wxCommandEvent& event);
    void OnMRUFile(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

protected:
    // hook the document manager into event handling chain here
    virtual bool TryValidator(wxEvent& event);

    wxDocManager *m_docManager;

private:
    typedef wxFrame base_type;
    DECLARE_CLASS(wxDocParentFrame)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxDocParentFrame)
};

// ----------------------------------------------------------------------------
// Provide simple default printing facilities
// ----------------------------------------------------------------------------

#if wxUSE_PRINTING_ARCHITECTURE
class WXDLLIMPEXP_CORE wxDocPrintout : public wxPrintout
{
public:
    wxDocPrintout(wxView *view = NULL, const wxString& title = wxT("Printout"));
    bool OnPrintPage(int page);
    bool HasPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

    virtual wxView *GetView() { return m_printoutView; }

protected:
    wxView*       m_printoutView;

private:
    DECLARE_DYNAMIC_CLASS(wxDocPrintout)
    DECLARE_NO_COPY_CLASS(wxDocPrintout)
};
#endif // wxUSE_PRINTING_ARCHITECTURE

// ----------------------------------------------------------------------------
// File history management
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFileHistory : public wxObject
{
public:
    wxFileHistory(size_t maxFiles = 9, wxWindowID idBase = wxID_FILE1);

    // Operations
    virtual void AddFileToHistory(const wxString& file);
    virtual void RemoveFileFromHistory(size_t i);
    virtual int GetMaxFiles() const { return (int)m_fileMaxFiles; }
    virtual void UseMenu(wxMenu *menu);

    // Remove menu from the list (MDI child may be closing)
    virtual void RemoveMenu(wxMenu *menu);

#if wxUSE_CONFIG
    virtual void Load(const wxConfigBase& config);
    virtual void Save(wxConfigBase& config);
#endif // wxUSE_CONFIG

    virtual void AddFilesToMenu();
    virtual void AddFilesToMenu(wxMenu* menu); // Single menu

    // Accessors
    virtual wxString GetHistoryFile(size_t i) const { return m_fileHistory[i]; }
    virtual size_t GetCount() const { return m_fileHistory.GetCount(); }

    const wxList& GetMenus() const { return m_fileMenus; }

    // Set/get base id
    void SetBaseId(wxWindowID baseId) { m_idBase = baseId; }
    wxWindowID GetBaseId() const { return m_idBase; }

#if WXWIN_COMPATIBILITY_2_6
    // deprecated, use GetCount() instead
    wxDEPRECATED( size_t GetNoHistoryFiles() const );
#endif // WXWIN_COMPATIBILITY_2_6

protected:
    // Last n files
    wxArrayString     m_fileHistory;

    // Menus to maintain (may need several for an MDI app)
    wxList            m_fileMenus;

    // Max files to maintain
    size_t            m_fileMaxFiles;

private:
    // The ID of the first history menu item (Doesn't have to be wxID_FILE1)
    wxWindowID m_idBase;

    DECLARE_DYNAMIC_CLASS(wxFileHistory)
    DECLARE_NO_COPY_CLASS(wxFileHistory)
};

#if WXWIN_COMPATIBILITY_2_6
inline size_t wxFileHistory::GetNoHistoryFiles() const
{
    return m_fileHistory.GetCount();
}
#endif // WXWIN_COMPATIBILITY_2_6

#if wxUSE_STD_IOSTREAM
// For compatibility with existing file formats:
// converts from/to a stream to/from a temporary file.
bool WXDLLIMPEXP_CORE wxTransferFileToStream(const wxString& filename, wxSTD ostream& stream);
bool WXDLLIMPEXP_CORE wxTransferStreamToFile(wxSTD istream& stream, const wxString& filename);
#else
// For compatibility with existing file formats:
// converts from/to a stream to/from a temporary file.
bool WXDLLIMPEXP_CORE wxTransferFileToStream(const wxString& filename, wxOutputStream& stream);
bool WXDLLIMPEXP_CORE wxTransferStreamToFile(wxInputStream& stream, const wxString& filename);
#endif // wxUSE_STD_IOSTREAM


// these flags are not used anywhere by wxWidgets and kept only for an unlikely
// case of existing user code using them for its own purposes
#ifdef WXWIN_COMPATIBILITY_2_8
enum
{
    wxDOC_SDI = 1,
    wxDOC_MDI,
    wxDEFAULT_DOCMAN_FLAGS = wxDOC_SDI
};
#endif // WXWIN_COMPATIBILITY_2_8

#endif // wxUSE_DOC_VIEW_ARCHITECTURE

#endif // _WX_DOCH__
