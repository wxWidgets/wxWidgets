/////////////////////////////////////////////////////////////////////////////
// Name:        editorbase.h
// Purpose:     General interfaces for editor plug-ins.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     11/04/1999
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	GNU General Public License wxWindows licence v2.0
/////////////////////////////////////////////////////////////////////////////

#ifndef __EDITORBASE_G__
#define __EDITORBASE_G__

#include "plugin.h"
#include "wx/window.h"

class wxsSourceEditorPlugin : public wxsComponent
{
protected:
	string mFileName;

public:
	/*** overridables (with default implementations) ***/

	// user-level commands

	virtual void OnOpen( const string& fname ) = 0;
	virtual void OnSave( const string& fname ) = 0;

	virtual void OnCopy() {}
	virtual void OnCut() {}
	virtual void OnPaste() {}
	virtual void OnDelete() {}

	virtual void OnUndo() {}
	virtual void OnRedo() {}

	virtual void SelectAll() {}

	// NOTE:: column -1 should result cursor to appear
	//        at the start of the first word in the line (if any)
	
	virtual void OnGotoLine( int lineNo, int column = -1 ) {}

	// should invoke editor's own "goto-line" dialog
	virtual void OnGotoLine() {}

	virtual void OnProperties() {}

	virtual void OnFind() {}
	virtual void OnFindNext() {}
	virtual void OnFindPrevious() {}
	virtual void OnReplace() {}

	virtual void OnToggleBookmark() {}
	virtual void OnNextBookmark() {}
	virtual void OnPreviousBookmark() {}
	virtual void OnShowBookmarks() {}

	virtual void SetCheckpoint() {}
	virtual bool CheckpointModified() { return TRUE; }

	// UI-updates

	virtual bool CanCopy()  { return FALSE; }
	virtual bool CanCut()   { return FALSE; }
	virtual bool CanPaste() { return FALSE; }
	virtual bool CanUndo()  { return FALSE; }
	virtual bool CanRedo()  { return FALSE; }

	// accesed by framework

	virtual bool IsModified() { return TRUE; }

	// returned buffer is NULL, if operation is not supported
	// by this concrete editor

	virtual void GetAllText( char** ppBuf, size_t* length ) 
		
		{ *ppBuf = NULL; *length = 0; }

	virtual string FindWordAtCursor() = 0;

	// returned line and column are -1s, if operation
	// is not supported this concrete editor

	virtual void GetCursorPos( int* line, int* column )

		{ *line = -1; *column = -1; } 

	virtual void GetPagePos( int* line, int* column )

		{ *line = -1; *column = -1; } 

	virtual void SetCursorPos( int line, int column ) {}

	// returned buffer is NULL, if operation is not supported
	// by this concrete editor,
	// (NOTE: range is given from "fromLine", but not
	//        including tillLine, [fomrLine,tillLine) )

	virtual void GetText( int fromLine, int fromColumn,
		                  int tillLine, int tillColumn,
						  char** ppBuf, size_t* length ) 
		{ ppBuf = NULL;	}

	virtual void InsertText( int line, int column,
							 char* text, size_t lenght )
	{}

	virtual void DeleteText( int fromLine, int fromColumn,
						     int tillLine, int tillColumn  )
	{}

	virtual void PositionToXY( int line, int column, int* x, int* y )

		{ *x = -1; *y = -1; }

	virtual void GetSelectionRange( int* fromLine, int* fromColumn,
								    int* tillLine, int* tillColumn  )
	
		{ *fromLine = -1; // not supported by default 
	    }

	virtual wxSize GetCharacterSize() { return wxSize(-1,-1); }

	virtual bool IsUnixText() 

// default impl., actual implementation should use auto-detection

#ifdef __WINDOWS__
	{ return FALSE; }
#else
	{ return TRUE; }
#endif

	// requests editor to keep cursor blinking, even when
	// the window has lost it's focus

	virtual void HoldCursor( bool hold ) 
		{}

	virtual string GetFileName() { return mFileName; }
	virtual void SetFileName( const string& fname ) { mFileName = fname; } 

	// overriden methods of wxStudioPluginBase
	virtual WXS_PLUGIN_TYPE GetType() {return WXS_EDITOR_PLUGIN;}
    virtual string GetCategory() { return "Editor";}    
};

#endif
	  // __EDITORBASE_G__
