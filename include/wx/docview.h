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
#if wxUSE_CONFIG
class WXDLLEXPORT wxConfigBase;
#endif

#if wxUSE_IOSTREAMH
// N.B. BC++ doesn't have istream.h, ostream.h
#  include <iostream.h>
#else
#  include <istream>
#  include <ostream>
#  ifdef __VISUALC__
      using namespace std;
#  endif
#endif

// Document manager flags
#define wxDOC_SDI       1
#define wxDOC_MDI       2
#define wxDOC_NEW       4
#define wxDOC_SILENT    8
#define wxDEFAULT_DOCMAN_FLAGS      wxDOC_SDI

// Document template flags
#define wxTEMPLATE_VISIBLE      1
#define wxTEMPLATE_INVISIBLE    2
#define wxDEFAULT_TEMPLATE_FLAGS    wxTEMPLATE_VISIBLE

#define wxMAX_FILE_HISTORY      9

class WXDLLEXPORT wxDocument : public wxEvtHandler
{
  DECLARE_ABSTRACT_CLASS(wxDocument)
 public:
  wxDocument(wxDocument *parent = (wxDocument *) NULL);
  ~wxDocument(void);

  void SetFilename(const wxString& filename, bool notifyViews = FALSE);
  inline wxString GetFilename(void) const { return m_documentFile; }
  inline void SetTitle(const wxString& title) { m_documentTitle = title; };
  inline wxString GetTitle(void) const { return m_documentTitle; }
  inline void SetDocumentName(const wxString& name) { m_documentTypeName = name; };
  inline wxString GetDocumentName(void) const { return m_documentTypeName; }
  // Has the document been saved yet?
  inline bool GetDocumentSaved(void) { return m_savedYet; }
  inline void SetDocumentSaved(bool saved = TRUE) { m_savedYet = saved; }

  virtual bool Close(void);
  virtual bool Save(void);
  virtual bool SaveAs(void);
  virtual bool Revert(void);

  virtual ostream& SaveObject(ostream& stream);
  virtual istream& LoadObject(istream& stream);

  // Called by wxWindows
  virtual bool OnSaveDocument(const wxString& filename);
  virtual bool OnOpenDocument(const wxString& filename);
  virtual bool OnNewDocument(void);
  virtual bool OnCloseDocument(void);

  // Prompts for saving if about to close a modified document.
  // Returns TRUE if ok to close the document (may have saved in the
  // meantime, or set modified to FALSE)
  virtual bool OnSaveModified(void);

  // Called by framework if created automatically by the
  // default document manager: gives document a chance to
  // initialise and (usually) create a view
  virtual bool OnCreate(const wxString& path, long flags);

  // By default, creates a base wxCommandProcessor.
  virtual wxCommandProcessor *OnCreateCommandProcessor(void);
  virtual inline wxCommandProcessor *GetCommandProcessor(void) const { return m_commandProcessor; }
  virtual inline void SetCommandProcessor(wxCommandProcessor *proc) { m_commandProcessor = proc; }

  // Called after a view is added or removed.
  // The default implementation deletes the document if this
  // is there are no more views.
  virtual void OnChangedViewList(void);

  virtual bool DeleteContents(void);

  virtual bool Draw(wxDC&);
  virtual inline bool IsModified(void) const { return m_documentModified; }
  virtual inline void Modify(bool mod) { m_documentModified = mod; }

  virtual bool AddView(wxView *view);
  virtual bool RemoveView(wxView *view);
  inline wxList& GetViews(void) const { return (wxList&) m_documentViews; }
  wxView *GetFirstView(void) const;

  virtual void UpdateAllViews(wxView *sender = (wxView *) NULL, wxObject *hint = (wxObject *) NULL);

  // Remove all views (because we're closing the document)
  virtual bool DeleteAllViews(void);

  // Other stuff
  virtual wxDocManager *GetDocumentManager(void) const;
  virtual inline wxDocTemplate *GetDocumentTemplate(void) const { return m_documentTemplate; }
  virtual inline void SetDocumentTemplate(wxDocTemplate *temp) { m_documentTemplate = temp; }

  // Get title, or filename if no title, else [unnamed]
  virtual bool GetPrintableName(wxString& buf) const;

  // Returns a window that can be used as a parent for document-related
  // dialogs. Override if necessary.
  virtual wxWindow *GetDocumentWindow(void) const;
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
  wxView(wxDocument *doc = (wxDocument *) NULL);
  ~wxView(void);

  inline wxDocument *GetDocument(void) const { return m_viewDocument; }
  void SetDocument(wxDocument *doc);

  inline wxString GetViewName(void) const { return m_viewTypeName; }
  void SetViewName(const wxString& name) { m_viewTypeName = name; };

  inline wxFrame *GetFrame(void) const { return m_viewFrame ; }
  inline void SetFrame(wxFrame *frame) { m_viewFrame = frame; }

  virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
  virtual void OnDraw(wxDC *dc) = 0;
  virtual void OnPrint(wxDC *dc, wxObject *info);
  virtual void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
  virtual void OnChangeFilename(void);

  // Called by framework if created automatically by the
  // default document manager class: gives view a chance to
  // initialise
  virtual bool OnCreate(wxDocument *WXUNUSED(doc), long WXUNUSED(flags)) { return TRUE; };

  // Checks if the view is the last one for the document; if so,
  // asks user to confirm save data (if modified). If ok,
  // deletes itself and returns TRUE.
  virtual bool Close(bool deleteWindow = TRUE);

  // Override to do cleanup/veto close
  virtual bool OnClose(bool deleteWindow);

#if WXWIN_COMPATIBILITY
  // Defeat compiler warning
  bool OnClose(void) { return wxEvtHandler::OnClose(); }
#endif

  // Extend event processing to search the document's event table
  virtual bool ProcessEvent(wxEvent& event);

  // A view's window can call this to notify the view it is (in)active.
  // The function then notifies the document manager.
  virtual void Activate(bool activate);

  wxDocManager *GetDocumentManager(void) const
    { return m_viewDocument->GetDocumentManager(); }

#if wxUSE_PRINTING_ARCHITECTURE
  virtual wxPrintout *OnCreatePrintout(void);
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

  // Associate document and view types.
  // They're for identifying what view is associated with what
  // template/document type
  wxDocTemplate(wxDocManager *manager, const wxString& descr, const wxString& filter, const wxString& dir,
     const wxString& ext, const wxString& docTypeName, const wxString& viewTypeName,
     wxClassInfo *docClassInfo = (wxClassInfo *) NULL, wxClassInfo *viewClassInfo = (wxClassInfo *)NULL,
     long flags = wxDEFAULT_TEMPLATE_FLAGS);

  ~wxDocTemplate(void);

  // By default, these two member functions dynamically creates document
  // and view using dynamic instance construction.
  // Override these if you need a different method of construction.
  virtual wxDocument *CreateDocument(const wxString& path, long flags = 0);
  virtual wxView *CreateView(wxDocument *doc, long flags = 0);

  inline wxString GetDefaultExtension(void) const { return m_defaultExt; };
  inline wxString GetDescription(void) const { return m_description; }
  inline wxString GetDirectory(void) const { return m_directory; };
  inline wxDocManager *GetDocumentManager(void) const { return m_documentManager; }
  inline void SetDocumentManager(wxDocManager *manager) { m_documentManager = manager; }
  inline wxString GetFileFilter(void) const { return m_fileFilter; };
  inline long GetFlags(void) const { return m_flags; };
  virtual wxString GetViewName(void) const { return m_viewTypeName; }
  virtual wxString GetDocumentName(void) const { return m_docTypeName; }

  inline void SetFileFilter(const wxString& filter) { m_fileFilter = filter; };
  inline void SetDirectory(const wxString& dir) { m_directory = dir; };
  inline void SetDescription(const wxString& descr) { m_description = descr; };
  inline void SetDefaultExtension(const wxString& ext) { m_defaultExt = ext; };
  inline void SetFlags(long flags) { m_flags = flags; };

  inline bool IsVisible(void) const { return ((m_flags & wxTEMPLATE_VISIBLE) == wxTEMPLATE_VISIBLE); }

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

// One object of this class may be created in an application,
// to manage all the templates and documents.
class WXDLLEXPORT wxDocManager: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxDocManager)
 public:
  wxDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS, bool initialize = TRUE);
  ~wxDocManager(void);

  virtual bool Initialize(void);

  // Handlers for common user commands
//   virtual void OldOnMenuCommand(int command);

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

  wxDocument *GetCurrentDocument(void) const;

  inline void SetMaxDocsOpen(int n) { m_maxDocsOpen = n; }
  inline int GetMaxDocsOpen(void) const { return m_maxDocsOpen; }

  // Add and remove a document from the manager's list
  void AddDocument(wxDocument *doc);
  void RemoveDocument(wxDocument *doc);

  // Clear remaining documents and templates
  bool Clear(bool force = TRUE);

  // Views or windows should inform the document manager
  // when a view is going in or out of focus
  virtual void ActivateView(wxView *view, bool activate = TRUE, bool deleting = FALSE);
  virtual wxView *GetCurrentView(void) const;

  virtual inline wxList& GetDocuments(void) const { return (wxList&) m_docs; }

  // Make a default document name
  virtual bool MakeDefaultName(wxString& buf);

  virtual wxFileHistory *OnCreateFileHistory(void);
  virtual inline wxFileHistory *GetFileHistory(void) const { return m_fileHistory; }

  // File history management
  virtual void AddFileToHistory(const wxString& file);
  virtual int GetNoHistoryFiles(void) const;
  virtual wxString GetHistoryFile(int i) const;
  virtual void FileHistoryUseMenu(wxMenu *menu);
  virtual void FileHistoryRemoveMenu(wxMenu *menu);
#if wxUSE_CONFIG
  virtual void FileHistoryLoad(wxConfigBase& config);
  virtual void FileHistorySave(wxConfigBase& config);
#endif
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

/*
 * A default child frame
 */

class WXDLLEXPORT wxDocChildFrame: public wxFrame
{
  DECLARE_CLASS(wxDocChildFrame)

 public:
  wxDocChildFrame(wxDocument *doc, wxView *view, wxFrame *frame, wxWindowID id, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long type = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame");
  ~wxDocChildFrame(void);

  // Extend event processing to search the view's event table
  virtual bool ProcessEvent(wxEvent& event);

  void OnActivate(wxActivateEvent& event);
  void OnCloseWindow(wxCloseEvent& event);

  inline wxDocument *GetDocument(void) const { return m_childDocument; }
  inline wxView *GetView(void) const { return m_childView; }
  inline void SetDocument(wxDocument *doc) { m_childDocument = doc; }
  inline void SetView(wxView *view) { m_childView = view; }
 protected:
  wxDocument*       m_childDocument;
  wxView*           m_childView;

DECLARE_EVENT_TABLE()

};

/*
 * A default parent frame
 */
 
class WXDLLEXPORT wxDocParentFrame: public wxFrame
{
  DECLARE_CLASS(wxDocParentFrame)
 public:
  wxDocParentFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long type = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame");

  // Extend event processing to search the document manager's event table
  virtual bool ProcessEvent(wxEvent& event);

  wxDocManager *GetDocumentManager(void) const { return m_docManager; }

  void OnExit(wxCommandEvent& event);
  void OnMRUFile(wxCommandEvent& event);
  void OnCloseWindow(wxCloseEvent& event);

 protected:
  wxDocManager *m_docManager;

DECLARE_EVENT_TABLE()
};

/*
 * Provide simple default printing facilities
 */

#if wxUSE_PRINTING_ARCHITECTURE
class WXDLLEXPORT wxDocPrintout: public wxPrintout
{
  DECLARE_DYNAMIC_CLASS(wxDocPrintout)
 public:
  wxDocPrintout(wxView *view = (wxView *) NULL, const wxString& title = "Printout");
  bool OnPrintPage(int page);
  bool HasPage(int page);
  bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

  virtual inline wxView *GetView(void) { return m_printoutView; }
 protected:
  wxView*       m_printoutView;
};
#endif

/*
 * Command processing framework
 */

class WXDLLEXPORT wxCommand: public wxObject
{
  DECLARE_CLASS(wxCommand)
 public:
  wxCommand(bool canUndoIt = FALSE, const wxString& name = "");
  ~wxCommand(void);

  // Override this to perform a command
  virtual bool Do(void) = 0;

  // Override this to undo a command
  virtual bool Undo(void) = 0;

  virtual inline bool CanUndo(void) const { return m_canUndo; }
  virtual inline wxString GetName(void) const { return m_commandName; }
 protected:
  bool              m_canUndo;
  wxString          m_commandName;
};

class WXDLLEXPORT wxCommandProcessor: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxCommandProcessor)
 public:
  wxCommandProcessor(int maxCommands = 100);
  ~wxCommandProcessor(void);

  // Pass a command to the processor. The processor calls Do();
  // if successful, is appended to the command history unless
  // storeIt is FALSE.
  virtual bool Submit(wxCommand *command, bool storeIt = TRUE);
  virtual bool Undo(void);
  virtual bool Redo(void);
  virtual bool CanUndo(void) const;
  virtual bool CanRedo(void) const;

  // Call this to manage an edit menu.
  inline void SetEditMenu(wxMenu *menu) { m_commandEditMenu = menu; }
  inline wxMenu *GetEditMenu(void) const { return m_commandEditMenu; }
  virtual void SetMenuStrings(void);
  virtual void Initialize(void);

  inline wxList& GetCommands(void) const { return (wxList&) m_commands; }
  inline int GetMaxCommands(void) const { return m_maxNoCommands; }
  virtual void ClearCommands(void);

 protected:
  int           m_maxNoCommands;
  wxList        m_commands;
  wxNode*       m_currentCommand;
  wxMenu*       m_commandEditMenu;
};

// File history management

class WXDLLEXPORT wxFileHistory: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxFileHistory)
 public:
  wxFileHistory(int maxFiles = 9);
  ~wxFileHistory(void);

// Operations
  virtual void AddFileToHistory(const wxString& file);
  virtual int GetMaxFiles(void) const { return m_fileMaxFiles; }
  virtual void UseMenu(wxMenu *menu);

  // Remove menu from the list (MDI child may be closing)
  virtual void RemoveMenu(wxMenu *menu);

#if wxUSE_CONFIG
  virtual void Load(wxConfigBase& config);
  virtual void Save(wxConfigBase& config);
#endif

  virtual void AddFilesToMenu();
  virtual void AddFilesToMenu(wxMenu* menu); // Single menu

// Accessors
  virtual wxString GetHistoryFile(int i) const;

  // A synonym for GetNoHistoryFiles
  virtual int GetCount() const { return m_fileHistoryN; }
  inline int GetNoHistoryFiles(void) const { return m_fileHistoryN; }

  inline wxList& GetMenus() const { return (wxList&) m_fileMenus; }

 protected:
  // Last n files
  char**            m_fileHistory;
  // Number of files saved
  int               m_fileHistoryN;
  // Menus to maintain (may need several for an MDI app)
  wxList            m_fileMenus;
  // Max files to maintain
  int               m_fileMaxFiles;
};

// For compatibility with existing file formats:
// converts from/to a stream to/from a temporary file.
bool WXDLLEXPORT wxTransferFileToStream(const wxString& filename, ostream& stream);
bool WXDLLEXPORT wxTransferStreamToFile(istream& stream, const wxString& filename);


#endif
