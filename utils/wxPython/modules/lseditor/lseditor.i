/////////////////////////////////////////////////////////////////////////////
// Name:        utils.i
// Purpose:     SWIG definitions of various utility classes
//
// Author:      Robin Dunn
//
// Created:     25-nov-1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module lseditor

%{
#include "helpers.h"
#include "lseditorpl.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern windows2.i
%extern windows3.i
%extern frames.i
%extern _defs.i
%extern misc.i
%extern gdi.i
%extern controls.i
%extern events.i


%{
#ifdef SEPARATE
    wxString wxPyEmptyStr("");
#endif
%}

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

class wxsLSEditorPlugin
{
public:
	wxsLSEditorPlugin();
	~wxsLSEditorPlugin();

	void Create( wxWindow* parent, wxWindowID id );

	virtual void OnOpen( const char* fname );
	virtual void OnSave( const char* fname );

	virtual void OnCopy();
	virtual void OnCut();
	virtual void OnPaste();
	virtual void OnDelete();

	void OnUndo();
	void OnRedo();

	void SelectAll();
	void OnGotoLine( int lineNo, int column = 0 );
	void OnGotoLine();
	void OnProperties();

	void OnFind();
	void OnFindNext();
	void OnFindPrevious();
	void OnReplace();

	virtual void OnToggleBookmark();
	virtual void OnNextBookmark();
	virtual void OnPreviousBookmark();
	virtual void OnShowBookmarks();

	virtual void SetCheckpoint();
	virtual bool CheckpointModified();

	// UI-updates

	bool CanCopy();
	bool CanCut();
	bool CanPaste();
	bool CanUndo();
	bool CanRedo();

	// accesed by framework
    
    virtual string GetName(){ return "Alex's Language Sensitive Editor"; }

	virtual bool IsModified();

	virtual wxWindow* GetWindow();

	//virtual void GetAllText( char** ppBuf, size_t* length );

	virtual void SetFileName( const char* fname );

	virtual void HoldCursor( bool hold );

	//virtual wxsPluginBase* Clone() { return new wxsLSEditorPlugin(); };

	virtual string FindWordAtCursor();

	virtual void GetCursorPos( int* line, int* column );
	virtual void SetCursorPos( int line, int column );

	virtual void GetPagePos( int* line, int* column );

	virtual void GetText( int fromLine, int fromColumn,
		                  int tillLine, int tillColumn,
						  char** ppBuf, size_t* length );

	virtual void InsertText( int line, int column,
							 char* text, size_t lenght );

	virtual void DeleteText( int fromLine, int fromColumn,
						     int tillLine, int tillColumn  );

	virtual void PositionToXY( int line, int column, int* x, int* y );

	virtual void GetSelectionRange( int* fromLine, int* fromColumn,
								    int* tillLine, int* tillColumn  );

	virtual wxSize GetCharacterSize();

	virtual bool IsUnixText();

	// some extras (just in case..)

	//wxTextEditorModel& GetModel();
	//wxTextEditorView&  GetView();
};

//---------------------------------------------------------------------------

%init %{

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

%}

//---------------------------------------------------------------------------
