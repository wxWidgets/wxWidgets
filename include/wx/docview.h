/////////////////////////////////////////////////////////////////////////////
// Name:        docview.h
// Purpose:     Doc/View classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCH__
#define _WX_DOCH__

#ifdef __GNUG__
    #pragma interface "docview.h"
#endif

#include "wx/defs.h"
#include "wx/list.h"
#include "wx/cmndata.h"
#include "wx/string.h"

#if wxUSE_PRINTING_ARCHITECTURE
    #include "wx/print.h"
#endif

class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxDocument;
class WXDLLEXPORT wxView;
class WXDLLEXPORT wxDocTemplate;
class WXDLLEXPORT wxDocManager;
class WXDLLEXPORT wxPrintInfo;
class WXDLLEXPORT wxCommand;
class WXDLLEXPORT wxCommandProcessor;
class WXDLLEXPORT wxFileHistory;
class WXDLLEXPORT wxConfigBase;

#if wxUSE_STD_IOSTREAM
  #include "wx/ioswrap.h"
#else
  #include "wx/stream.h"
#endif

// Document manager flags
enum
{
    wxDOC_SDI = 1,
    wxDOC_MDI,
    wxDOC_NEW,
    wxDOC_SILENT,
    wxDEFAULT_DOCMAN_FLAGS = wxDOC_SDI
};

// Document template flags
enum
{
    wxTEMPLATE_VISIBLE = 1,
    wxTEMPLATE_INVISIBLE,
    wxDEFAULT_TEMPLATE_FLAGS = wxTEMPLATE_VISIBLE
};

#define wxMAX_FILE_HISTORY 9

class WXDLLEXPORT wxDocument : public wxEvtHandler
{
    DECLARE_ABSTRACT_CLASS(wxDocument)

public:
    wxDocument(wxDocument *parent = (wxDocument *) NULL);
    ~wxDocument();

    // accessors
    void SetFilename(const wxString& filename, bool notifyViews = FALSE);
    wxString GetFilename() const { return m_documentFile; }

    void SetTitle(const wxString& title) { m_documentTitle = title; };
    wxString GetTitle() const { return m_documentTitle; }

    void SetDocumentName(const wxString& name) { m_documentTypeName = name; };
    wxString GetDocumentName() const { return m_documentTypeName; }

    bool GetDocumentSaved() const { return m_savedYet; }
    void SetDocumentSaved(bool saved = TRUE) { m_savedYet = saved; }

    virtual bool Close();
    virtual bool Save();
    virtual bool SaveAs();
    virtual bool Revert();

#if wxUSE_STD_IOSTREAM
    virtual ostream& SaveObject(ostream& stream);
    virtual istream& LoadObject(istream& stream);
#else
    virtual bool SaveObject(wxOutputStream& stream);
    virtual bool LoadObject(wxInputStream& stream);
#endif

    // Called by wxWindows
    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnOpenDocument(const wxString& filename);
    virtual bool OnNewDocument();
    virtual bool OnCloseDocument();

    // Prompts for saving if about to close a modified document. Returns TRUE
    // if ok to close the document (may have saved in the meantime, or set
    // modified to FALSE)
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
    wxList& GetViews() const { return (wxList&) m_documentViews; }
    wxView *GetFirstView() const;

    virtual void UpdateAllViews(wxView *sender = (wxView *) NULL, wxObject *hint = (wxObject *) NULL);

    // Remove all views (because we're closing the document)
    virtual bool DeleteAllViews();

    // Other stuff
    virtual wxDocManager *GetDocumentManager() const;
    virtual wxDocTemplate *GetDocumentTemplate() const { return m_documentTemplate; }
    virtual void SetDocumentTemplate(wxDocTemplate *temp) { m_documentTemplate = temp; }

    // Get title, or filename if no title, else [unnamed]
    virtual bool GetPrintableName(wxString& buf) const;

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
};

class WXDLLEXPORT wxView: public wxEvtHandler
{
    DECLARE_ABSTRACT_CLASS(wxView)

public:
    //  wxView(wxDocument *doc = (wxDocument *) NULL);
    wxView();
    ~wxView();

    wxDocument *GetDocument() const { return m_viewDocument; }
    void SetDocument(wxDocument *doc);

    wxString GetViewName() const { return m_viewTypeName; }
    void SetViewName(const wxString& name) { m_viewTypeName = name; };

    wxFrame *GetFrame() const { return m_viewFrame ; }
    void SetFrame(wxFrame *frame) { m_viewFrame = frame; }

    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
    virtual void OnDraw(wxDC *dc) = 0;
    virtual void OnPrint(wxDC *dc, wxObject *info);
    virtual void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    virtual void OnChangeFilename();

    // Called by framework if created automatically by the default document
    // manager class: gives view a chance to initialise
    virtual bool OnCreate(wxDocument *WXUNUSED(doc), long WXUNUSED(flags)) { return TRUE; };

    // Checks if the view is the last one for the document; if so, asks user
    // to confirm save data (if modified). If ok, deletes itself and returns
    // TRUE.
    virtual bool Close(bool deleteWindow = TRUE);

    // Override to do cleanup/veto close
    virtual bool OnClose(bool deleteWindow);

#if WXWIN_COMPATIBILITY
    // Defeat compiler warning
    bool OnClose() { return wxEvtHandler::OnClose(); }
#endif

    // Extend event processing to search the document's event table
    virtual bool ProcessEvent(wxEvent& event);

    // A view's window can call this to notify the view it is (in)active.
    // The function then notifies the document manager.
    virtual void Activate(bool activate);

    wxDocManager *GetDocumentManager() const
        { return m_viewDocument->GetDocumentManager(); }

#if wxUSE_PRINTING_ARCHITECTURE
    virtual wxPrintout *OnCreatePrintout();
#endif

protected:
    wxDocument*       m_viewDocument;
    wxString          m_viewTypeName;
    wxFrame*          m_viewFrame;
};

// Represents user interface (and other) properties of documents and views
class WXDLLEXPORT wxDocTemplate: public wxObject
{
DECLARE_CLASS(wxDocTemplate)

friend class WXDLLEXPORT wxDocManager;

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
                  wxClassInfo *docClassInfo = (wxClassInfo *) NULL,
                  wxClassInfo *viewClassInfo = (wxClassInfo *)NULL,
                  long flags = wxDEFAULT_TEMPLATE_FLAGS);

    ~wxDocTemplate();

    // By default, these two member functions dynamically creates document and
    // view using dynamic instance construction. Override these if you need a
    // different method of construction.
    virtual wxDocument *CreateDocument(const wxString& path, long flags = 0);
    virtual wxView *CreateView(wxDocument *doc, long flags = 0);

    wxString GetDefaultExtension() const { return m_defaultExt; };
    wxString GetDescription() const { return m_description; }
    wxString GetDirectory() const { return m_directory; };
    wxDocManager *GetDocumentManager() const { return m_documentManager; }
    void SetDocumentManager(wxDocManager *manager) { m_documentManager = manager; }
    wxString GetFileFilter() const { return m_fileFilter; };
    long GetFlags() const { return m_flags; };
    virtual wxString GetViewName() const { return m_viewTypeName; }
    virtual wxString GetDocumentName() const { return m_docTypeName; }

    void SetFileFilter(const wxString& filter) { m_fileFilter = filter; };
    void SetDirectory(const wxString& dir) { m_directory = dir; };
    void SetDescription(const wxString& descr) { m_description = descr; };
    void SetDefaultExtension(const wxString& ext) { m_defaultExt = ext; };
    void SetFlags(long flags) { m_flags = flags; };

    bool IsVisible() const { return ((m_flags & wxTEMPLATE_VISIBLE) == wxTEMPLATE_VISIBLE); }

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
};

// One object of this class may be created in an application, to manage all
// the templates and documents.
class WXDLLEXPORT wxDocManager: public wxEvtHandler
{
    DECLARE_DYNAMIC_CLASS(wxDocManager)

public:
    wxDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS, bool initialize = TRUE);
    ~wxDocManager();

    virtual bool Initialize();

    // Handlers for common user commands
    void OnFileClose(wxCommandEvent& event);
    void OnFileNew(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& event);
    void OnFileRevert(wxCommandEvent& event);
    void OnFileSave(wxCommandEvent& event);
    void OnFileSaveAs(wxCommandEvent& event);
    void OnPrint(wxCommandEvent& event);
    void OnPrintSetup(wxCommandEvent& event);
    void OnPreview(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);

    // Extend event processing to search the view's event table
    virtual bool ProcessEvent(wxEvent& event);

    virtual wxDocument *CreateDocument(const wxString& path, long flags = 0);
    virtual wxView *CreateView(wxDocument *doc, long flags = 0);
    virtual void DeleteTemplate(wxDocTemplate *temp, long flags = 0);
    virtual bool FlushDoc(wxDocument *doc);
    virtual wxDocTemplate *MatchTemplate(const wxString& path);
    virtual wxDocTemplate *SelectDocumentPath(wxDocTemplate **templates,
            int noTemplates, wxString& path, long flags, bool save = FALSE);
    virtual wxDocTemplate *SelectDocumentType(wxDocTemplate **templates,
            int noTemplates);
    virtual wxDocTemplate *SelectViewType(wxDocTemplate **templates,
            int noTemplates);
    virtual wxDocTemplate *FindTemplateForPath(const wxString& path);

    void AssociateTemplate(wxDocTemplate *temp);
    void DisassociateTemplate(wxDocTemplate *temp);

    wxDocument *GetCurrentDocument() const;

    void SetMaxDocsOpen(int n) { m_maxDocsOpen = n; }
    int GetMaxDocsOpen() const { return m_maxDocsOpen; }

    // Add and remove a document from the manager's list
    void AddDocument(wxDocument *doc);
    void RemoveDocument(wxDocument *doc);

    // Clear remaining documents and templates
    bool Clear(bool force = TRUE);

    // Views or windows should inform the document manager
    // when a view is going in or out of focus
    virtual void ActivateView(wxView *view, bool activate = TRUE, bool deleting = FALSE);
    virtual wxView *GetCurrentView() const;

    virtual wxList& GetDocuments() const { return (wxList&) m_docs; }

    // Make a default document name
    virtual bool MakeDefaultName(wxString& buf);

    virtual wxFileHistory *OnCreateFileHistory();
    virtual wxFileHistory *GetFileHistory() const { return m_fileHistory; }

    // File history management
    virtual void AddFileToHistory(const wxString& file);
    virtual int GetNoHistoryFiles() const;
    virtual wxString GetHistoryFile(int i) const;
    virtual void FileHistoryUseMenu(wxMenu *menu);
    virtual void FileHistoryRemoveMenu(wxMenu *menu);
#if wxUSE_CONFIG
    virtual void FileHistoryLoad(wxConfigBase& config);
    virtual void FileHistorySave(wxConfigBase& config);
#endif // wxUSE_CONFIG

    virtual void FileHistoryAddFilesToMenu();
    virtual void FileHistoryAddFilesToMenu(wxMenu* menu);

protected:
    long              m_flags;
    int               m_defaultDocumentNameCounter;
    int               m_maxDocsOpen;
    wxList            m_docs;
    wxList            m_templates;
    wxView*           m_currentView;
    wxFileHistory*    m_fileHistory;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// A default child frame
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDocChildFrame : public wxFrame
{
    DECLARE_CLASS(wxDocChildFrame)

public:
    wxDocChildFrame(wxDocument *doc,
                    wxView *view,
                    wxFrame *frame,
                    wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long type = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = "frame");
    ~wxDocChildFrame();

    // Extend event processing to search the view's event table
    virtual bool ProcessEvent(wxEvent& event);

    void OnActivate(wxActivateEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    wxDocument *GetDocument() const { return m_childDocument; }
    wxView *GetView() const { return m_childView; }
    void SetDocument(wxDocument *doc) { m_childDocument = doc; }
    void SetView(wxView *view) { m_childView = view; }

protected:
    wxDocument*       m_childDocument;
    wxView*           m_childView;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// A default parent frame
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDocParentFrame : public wxFrame
{
    DECLARE_CLASS(wxDocParentFrame)

public:
    wxDocParentFrame(wxDocManager *manager,
                     wxFrame *frame,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long type = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = "frame");

    // Extend event processing to search the document manager's event table
    virtual bool ProcessEvent(wxEvent& event);

    wxDocManager *GetDocumentManager() const { return m_docManager; }

    void OnExit(wxCommandEvent& event);
    void OnMRUFile(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

protected:
    wxDocManager *m_docManager;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// Provide simple default printing facilities
// ----------------------------------------------------------------------------

#if wxUSE_PRINTING_ARCHITECTURE
class WXDLLEXPORT wxDocPrintout : public wxPrintout
{
    DECLARE_DYNAMIC_CLASS(wxDocPrintout)

public:
    wxDocPrintout(wxView *view = (wxView *) NULL, const wxString& title = "Printout");
    bool OnPrintPage(int page);
    bool HasPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

    virtual wxView *GetView() { return m_printoutView; }

protected:
    wxView*       m_printoutView;
};
#endif // wxUSE_PRINTING_ARCHITECTURE

// ----------------------------------------------------------------------------
// Command processing framework
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCommand : public wxObject
{
    DECLARE_CLASS(wxCommand)

public:
    wxCommand(bool canUndoIt = FALSE, const wxString& name = "");
    ~wxCommand();

    // Override this to perform a command
    virtual bool Do() = 0;

    // Override this to undo a command
    virtual bool Undo() = 0;

    virtual bool CanUndo() const { return m_canUndo; }
    virtual wxString GetName() const { return m_commandName; }

protected:
    bool     m_canUndo;
    wxString m_commandName;
};

class WXDLLEXPORT wxCommandProcessor : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxCommandProcessor)

public:
    wxCommandProcessor(int maxCommands = 100);
    ~wxCommandProcessor();

    // Pass a command to the processor. The processor calls Do(); if
    // successful, is appended to the command history unless storeIt is FALSE.
    virtual bool Submit(wxCommand *command, bool storeIt = TRUE);
    virtual bool Undo();
    virtual bool Redo();
    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    // Call this to manage an edit menu.
    void SetEditMenu(wxMenu *menu) { m_commandEditMenu = menu; }
    wxMenu *GetEditMenu() const { return m_commandEditMenu; }
    virtual void SetMenuStrings();
    virtual void Initialize();

    wxList& GetCommands() const { return (wxList&) m_commands; }
    int GetMaxCommands() const { return m_maxNoCommands; }
    virtual void ClearCommands();

protected:
    int           m_maxNoCommands;
    wxList        m_commands;
    wxNode*       m_currentCommand;
    wxMenu*       m_commandEditMenu;
};

// ----------------------------------------------------------------------------
// File history management
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFileHistory : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxFileHistory)

public:
    wxFileHistory(int maxFiles = 9);
    ~wxFileHistory();

    // Operations
    virtual void AddFileToHistory(const wxString& file);
    virtual int GetMaxFiles() const { return m_fileMaxFiles; }
    virtual void UseMenu(wxMenu *menu);

    // Remove menu from the list (MDI child may be closing)
    virtual void RemoveMenu(wxMenu *menu);

#if wxUSE_CONFIG
    virtual void Load(wxConfigBase& config);
    virtual void Save(wxConfigBase& config);
#endif // wxUSE_CONFIG

    virtual void AddFilesToMenu();
    virtual void AddFilesToMenu(wxMenu* menu); // Single menu

    // Accessors
    virtual wxString GetHistoryFile(int i) const;

    // A synonym for GetNoHistoryFiles
    virtual int GetCount() const { return m_fileHistoryN; }
    int GetNoHistoryFiles() const { return m_fileHistoryN; }

    wxList& GetMenus() const { return (wxList&) m_fileMenus; }

protected:
    // Last n files
    wxChar**          m_fileHistory;
    // Number of files saved
    int               m_fileHistoryN;
    // Menus to maintain (may need several for an MDI app)
    wxList            m_fileMenus;
    // Max files to maintain
    int               m_fileMaxFiles;
};

#if wxUSE_STD_IOSTREAM
// For compatibility with existing file formats:
// converts from/to a stream to/from a temporary file.
bool WXDLLEXPORT wxTransferFileToStream(const wxString& filename, ostream& stream);
bool WXDLLEXPORT wxTransferStreamToFile(istream& stream, const wxString& filename);
#endif

#endif // _WX_DOCH__
