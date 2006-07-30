/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     24/04/1999
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	GNU General Public License
/////////////////////////////////////////////////////////////////////////////
#ifndef __WXSDEFS_G__
#define __WXSDEFS_G__

#include <config.h>


#include "controlarea.h"

#ifdef wxUSE_TEMPLATE_STL
#include <vector>
#include <map>
//using std::map;
//using std::vector;
using namespace std;
#else
#include "wxstldefs.h" // imports predefine StrListT, IntListT containers
#include "wxstlac.h"
#endif
class wxsProject;
class wxsComponent;
class wxsWorkplace;
class wxsOpenedFileInfo;
class wxsWorkplaceListener;
class wxsSourceEditorPlugin;
class wxsSourceInfoPlugin;
class wxsAppListener;
class wxsAppInterface;

class wxFrame;

typedef wxsWorkplaceListener* wxsWorkplaceListenerPtrT;

#ifdef wxUSE_TEMPLATE_STL
typedef vector<wxsWorkplaceListenerPtrT> wxsWorkplaceListenerListT;
#else
typedef WXSTL_VECTOR_SHALLOW_COPY( wxsWorkplaceListenerPtrT ) wxsWorkplaceListenerListT;
#endif

// IDs for the controls and the menu commands
enum
{
    // menu items
    WXS_Quit = 3300,  // FIXEM:: ids..
    WXS_About,
	WXS_Open,
    WXS_Close,
	WXS_OpenWorkplace,
	WXS_Save,
    WXS_SaveAs,
	WXS_SaveAll,
	WXS_CloseWorkplace,
    WXS_SaveWorkplace,
	WXS_NewProject,

	WXS_ShowTips,

	WXS_Undo = 9000, 
	WXS_NextWindow,
	WXS_PreviousWindow,
	WXS_CloseWindow,
	WXS_ListWindows,
	WXS_UpdateBrowser,
	WXS_ToggleWorkplaceWindow,

	WXS_InsertFiles,
	WXS_Test,
    WXS_Settings,

	WXS_ClassView,
	WXS_FileView,
	WXS_GotoEditor,

    WXS_Test1,
    WXS_Test2,

    // controls start here (the numbers are, of course, arbitrary)
    WXS_Text = 1000,

	ID_EDIT_UNDO,       
	ID_EDIT_REDO,		  
	ID_EDIT_CUT,		  
	ID_EDIT_COPY,		  
	ID_EDIT_PASTE,	  
	ID_EDIT_DELETE,	  
	ID_EDIT_SELECT_ALL, 
	ID_EDIT_FIND,		  
 	ID_EDIT_FIND_NEXT,
	ID_EDIT_REPLACE,	  
	ID_EDIT_GOTO,		  
	ID_EDIT_TOGGLE_BM,  
	ID_EDIT_NEXT_BM,	  
	ID_EDIT_PREV_BM,	  
 	ID_EDIT_TOGGLE_BRKPNT,
	ID_EDIT_BOOKMARKS,  
	ID_EDIT_SETFONT,  
    ID_LEFT_SASH,
    ID_BOTTOM_SASH
};

// bitmaps/icons
enum WXS_ICON_ENUM{
    WXS_ICON_class = 1,
    WXS_ICON_priv_mtd,
    WXS_ICON_prot_mtd,
    WXS_ICON_pub_mtd,
    WXS_ICON_priv_mtd_def,
    WXS_ICON_prot_mtd_def,
    WXS_ICON_pub_mtd_def,
    WXS_ICON_priv_var,
    WXS_ICON_prot_var,
    WXS_ICON_pub_var,
    WXS_ICON_pub_pure_mtd,
    WXS_ICON_file,
    WXS_ICON_folder,
    WXS_ICON_mru_folder,
    WXS_ICON_class_gray,
    WXS_ICON_file_gray,
};


typedef char* CharPtrT;

struct UU_cmp
{ 
	inline int operator()(const CharPtrT x, const CharPtrT y ) const { return 0; } 
};

typedef wxBitmap* wxBitmapPtrT;
#ifdef wxUSE_TEMPLATE_STL
typedef map<WXS_ICON_ENUM, wxBitmapPtrT> WXS_BitmapsMapT;
#else
typedef WXSTL_MAP( WXS_ICON_ENUM, wxBitmapPtrT, 
				   LESS_THEN_FUNCTOR(WXS_ICON_ENUM) ) WXS_BitmapsMapT;
#endif

extern WXS_BitmapsMapT WXS_Bitmaps;
#define WXS_ICON(name) (*WXS_Bitmaps[WXS_ICON_##name])


// very general file categories

enum WXS_FILE_CATEGORY
{
	WXS_UNKNOWN_FILE,
	WXS_SOURCE_FILE,
	WXS_RESOURCE_FILE,
	WXS_DOCUMENTATION_FILE,
	WXS_CONFIGURAITON_FILE
};

enum WXS_PLUGIN_TYPE
{
	WXS_UNKNOWN_PLUGIN,
	WXS_EDITOR_PLUGIN,
	WXS_CLASSINFO_PLUGIN,
	WXS_CLASSBROWSER_PLUGIN,
	WXS_FILEBROWSER_PLUGIN,
	WXS_TOOL_PLUGIN,
	WXS_OUTPUTTOOL_PLUGIN
};



class wxsAppListener
{
public:
	virtual void OnWindowSwitched( wxWindow* fromWnd, wxWindow* toWnd ) {};
	virtual bool OnCloseWindow( wxWindow* wnd ) { return TRUE; }
    virtual void OnTabSwitched() {};
    virtual void OnPageSwitched() {};
};

class wxsAppInterface
{
public:
    virtual void SetAppListener(wxsAppListener* pLsn) = 0;
    
	virtual void AddEditor(wxsSourceEditorPlugin* editor,wxString title,wxBitmap* pImage = NULL) = 0;
	virtual void ActivateEditor( wxsSourceEditorPlugin* editor ) = 0;
	virtual void CloseEditor( wxsSourceEditorPlugin* editor ) = 0;

    virtual void ShowNextWindow() = 0;
    virtual void ShowPreviousWindow() = 0;
    virtual void CloseActiveWindow() = 0;
    virtual void CloseWindow( wxWindow* wnd ) = 0;
    virtual void ShowWindowList() = 0;
    virtual wxWindow* GetActiveWindow() = 0;
    virtual wxTabbedWindow* GetTabbedWindow() = 0;
	virtual void AddTab(wxsComponent* pContent, wxString tabText, wxBitmap* pImage = NULL) = 0;
    virtual void ShowNextTab() = 0;
    virtual wxPaggedWindow* GetPaggedWindow() = 0;
	virtual void AddPage(wxsComponent* pContent, wxString tabText, wxBitmap* pImage = NULL) = 0;
    virtual void ShowNextPage() = 0;
	virtual wxFrame* GetMainFrame() = 0;
	virtual void SetStatusText( const string& text ) = 0;

	static wxsAppInterface& GetInstance();

protected:
	static wxsAppInterface* mpInstance;
	friend wxsAppInterface& wxsGetApp();
};

// short-cut for wxsAppInterface::GetInstance()

wxsAppInterface& wxsGetApp();

class wxsWorkplaceListener : public wxObject
{
public:
	virtual void OnSubprojectAdded(   wxsProject& subPrj, wxsProject& toPrj   ) {}
	virtual void OnSubprojectRemoved( wxsProject& subPrj, wxsProject& fromPrj ) {}
	virtual void OnProjectCreated( wxsProject& prj ) {}
	virtual void OnRootProjectLoaded() {}
	virtual void OnRootProjectClosed() {}
	virtual void OnFilesAddedToProject( wxsProject& prj, StrListT& files, WXS_FILE_CATEGORY cat ) {}
	virtual void OnFilesRemovedFromProject( wxsProject& prj, StrListT& files, WXS_FILE_CATEGORY cat ) {}

	virtual void OnFileContentChanged( wxsProject& prj, const string& file, WXS_FILE_CATEGORY cat,
									   char* newContent, size_t len ) {}

	virtual void OnFileOpened( wxsOpenedFileInfo& file ) {}
};

typedef wxsProject* wxsProjectPtrT;
#ifdef wxUSE_TEMPLATE_STL
typedef vector<wxsProjectPtrT> wxsProjectListT;
#else
typedef WXSTL_VECTOR_SHALLOW_COPY( wxsProjectPtrT ) wxsProjectListT;
#endif

class wxsProject : public wxObject
{
public:
    // Basic project info
    virtual void SetName(const string& name) = 0;
    virtual void SetFileName(const string& fname) = 0;
    virtual void SetDescription(const string& desc) = 0;
    virtual void SetLanguage(const string& lang) = 0;
	virtual string GetName() = 0;
	virtual string GetFileName() = 0;
	virtual string GetDescription() = 0;
	virtual string GetLanguage() = 0;
    // File manipulation
    virtual bool AddFile( const string& file) = 0;
    virtual bool RemoveFile ( const string& file ) = 0;
	virtual StrListT GetFiles() = 0;
    // Sub-Project manipulation
    virtual void AddSubproject( wxsProject* subPrj) = 0;
    virtual void RemoveSubproject( wxsProject* subPrj) = 0;
	virtual wxsProjectListT& GetSubprojects() = 0;
    // Transient information (parent only exists when loaded
    virtual void SetParent( wxsProject* parentPrj) = 0;
	virtual wxsProject* GetParent() = 0;
    // configuration info storage
    virtual string CreateConfig ( const string& file = "" ) = 0;
    virtual bool SetCurrentConfig ( const string& configkey ) = 0;
    virtual bool AddConfigValue( const string& key, string& value) = 0;
    virtual bool SetConfigValue( const string& key, string& value) = 0;
    virtual bool RemoveConfigValue( const string& key ) = 0;
    virtual string GetConfigValue ( const string& key ) = 0;
    
    
};

class wxsOpenedFileInfo : public wxObject
{
public:
	string      mFullName;
	wxsProject* mpProject; // NULL, if file does not belong to any project
	wxsSourceEditorPlugin* mpEditor;
	WXS_FILE_CATEGORY      mCategory;
	bool                   mIsSaved;

public:
	wxsOpenedFileInfo() : mIsSaved( TRUE ), mpProject( NULL ) {}

	wxsProject* GetProject() { return mpProject; }
	const string& GetFullName() { return mFullName; }
	wxsSourceEditorPlugin& GetEditor() { return *mpEditor; }
	WXS_FILE_CATEGORY GetCategory() { return mCategory; }
};

typedef wxsOpenedFileInfo* wxsOpenedFileInfoPtrT;
#ifdef wxUSE_TEMPLATE_STL
typedef vector<wxsOpenedFileInfoPtrT> wxsOpenedFileInfoListT;
#else
typedef WXSTL_VECTOR_SHALLOW_COPY( wxsOpenedFileInfoPtrT ) wxsOpenedFileInfoListT;
#endif

// abstract interface

class wxsWorkplace : public wxObject
{
public:
	virtual void CreateProject( const string& name, const string& projectFile, const wxsProject* parent=NULL) = 0;

	virtual void LoadRootProject( const string& projectFile ) = 0;
	virtual void SaveProject( wxsProject& prj ) = 0;
	virtual void CloseRootProject() = 0;

	virtual wxsProject& GetRootProject() = 0;
	virtual wxsProject* FindProjectByName( const string& name ) = 0;
	virtual wxsOpenedFileInfo* FindFileByEditor( wxsSourceEditorPlugin& editor ) = 0;

	virtual void AddWorkpalceListener( wxsWorkplaceListener* pListener ) = 0;
	
	virtual void AddSubproject( wxsProject& intoPrj,const string& projectFile ) = 0;
	virtual void RemoveSubproject( wxsProject& prj ) = 0;

	virtual void AddFilesToProject( wxsProject& prj, StrListT& files ) = 0;
	virtual void RemoveFilesFromProject( wxsProject& prj, StrListT& files ) = 0;

	virtual void NotifyFileContentChanged( wxsProject& prj, const string& file, 
										   char* newContent, size_t len ) = 0;

	virtual void NotifyFileContentChanged( wxsOpenedFileInfo& info ) = 0;
	virtual void NotifyEditorDeactivated( wxsSourceEditorPlugin& editor ) = 0;

	// if pPrj is NULL, the specified file does not belong to any projects,
	// i.e. it blongs to the workplace

	virtual wxsOpenedFileInfo*
		OpenFileInEditor( const string& file, wxsProject* pPrj = NULL,
                          int line = -1, int column = -1) = 0;

	virtual void GetCurrentFileContent( wxsProject& prj, const string& file,
										char** buf, size_t& len 
									  ) = 0;

	// returns FALSE, if source was already up-to-date
	virtual bool SyncSourceInfo() = 0;

	virtual wxsProject* FindSubproject( wxsProject& parentPrj, const string& prjName ) = 0;
	virtual wxsOpenedFileInfo* FindOpenedFile( const string& name ) = 0;
	virtual bool FileIsOpened( wxsProject& prj, const string& file ) = 0;
	virtual wxsSourceEditorPlugin* GetSourceEditor( wxsProject& prj, const string& file ) = 0;
	virtual wxsSourceInfoPlugin* GetSourceInfoPlugin() = 0;

	static wxsWorkplace& GetInstance() { return *wxsWorkplace::mpInstance; }

	virtual wxsOpenedFileInfoListT& GetOpenedFiles() = 0;
	virtual wxsOpenedFileInfo* GetActiveFile() = 0;

	virtual void SaveFile( wxsOpenedFileInfo& info ) = 0;
	virtual void SaveAllFiles() = 0;

	virtual bool CloseFile( wxsOpenedFileInfo& info, bool closeWindow = TRUE ) = 0;
	virtual bool CloseAllFiles() = 0;

	virtual bool CloseInProgress() = 0;

	virtual WXS_FILE_CATEGORY GetFileCategory( const string& file ) = 0;

protected:
	static wxsWorkplace* mpInstance;
	friend wxsWorkplace& wxsGetWorkplace();
};

// short-cut for wxsWorkplace::GetInstance()

wxsWorkplace& wxsGetWorkplace();

#endif
