// Scintilla source code edit control
// ScintillaBase.cxx - an enhanced subclass of Editor with calltips, autocomplete and context menu
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "Platform.h"

#include "Scintilla.h"
#include "PropSet.h"
#ifdef SCI_LEXER
#include "SciLexer.h"
#include "Accessor.h"
#include "WindowAccessor.h"
#include "DocumentAccessor.h"
#include "KeyWords.h"
#endif
#include "ContractionState.h"
#include "SVector.h"
#include "CellBuffer.h"
#include "CallTip.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "AutoComplete.h"
#include "Document.h"
#include "Editor.h"
#include "ScintillaBase.h"

ScintillaBase::ScintillaBase() {
#ifdef SCI_LEXER	
	lexLanguage = SCLEX_CONTAINER;
	for (int wl=0;wl<numWordLists;wl++)
		keyWordLists[wl] = new WordList;
#endif
}

ScintillaBase::~ScintillaBase() {
#ifdef SCI_LEXER	
	for (int wl=0;wl<numWordLists;wl++)
		delete keyWordLists[wl];
#endif
}

void ScintillaBase::Finalise() {
    Editor::Finalise();
	popup.Destroy();
}

void ScintillaBase::RefreshColourPalette(Palette &pal, bool want) {
	Editor::RefreshColourPalette(pal, want);
	ct.RefreshColourPalette(pal, want);
}

void ScintillaBase::AddCharUTF(char *s, unsigned int len) {
	bool acActiveBeforeCharAdded = ac.Active();
	if (!acActiveBeforeCharAdded || !ac.IsFillUpChar(*s))
		Editor::AddCharUTF(s, len);
	if (acActiveBeforeCharAdded)
		AutoCompleteChanged(s[0]);
}

void ScintillaBase::Command(int cmdId) {

	switch (cmdId) {

	case idAutoComplete: 	// Nothing to do
		break;

	case idCallTip: 	// Nothing to do
		break;

	case idcmdUndo:
		WndProc(SCI_UNDO, 0, 0);
		break;

	case idcmdRedo:
		WndProc(SCI_REDO, 0, 0);
		break;

	case idcmdCut:
		WndProc(SCI_CUT, 0, 0);
		break;

	case idcmdCopy:
		WndProc(SCI_COPY, 0, 0);
		break;

	case idcmdPaste:
		WndProc(SCI_PASTE, 0, 0);
		break;

	case idcmdDelete:
		WndProc(SCI_CLEAR, 0, 0);
		break;

	case idcmdSelectAll:
		WndProc(SCI_SELECTALL, 0, 0);
		break;
	}
}

int ScintillaBase::KeyCommand(unsigned int iMessage) {
	// Most key commands cancel autocompletion mode
	if (ac.Active()) {
		switch (iMessage) {
			// Except for these
		case SCI_LINEDOWN:
			AutoCompleteMove(1);
			return 0;
		case SCI_LINEUP:
			AutoCompleteMove( -1);
			return 0;
		case SCI_PAGEDOWN:
			AutoCompleteMove(5);
			return 0;
		case SCI_PAGEUP:
			AutoCompleteMove( -5);
			return 0;
		case SCI_VCHOME:
			AutoCompleteMove( -5000);
			return 0;
		case SCI_LINEEND:
			AutoCompleteMove(5000);
			return 0;
		case SCI_DELETEBACK:
			DelCharBack();
			AutoCompleteChanged();
			EnsureCaretVisible();
			return 0;
		case SCI_TAB:
			AutoCompleteCompleted();
			return 0;
		case SCI_NEWLINE:
			AutoCompleteCompleted();
			return 0;

		default:
			ac.Cancel();
		}
	}

	if (ct.inCallTipMode) {
		if (
		    (iMessage != SCI_CHARLEFT) &&
		    (iMessage != SCI_CHARLEFTEXTEND) &&
		    (iMessage != SCI_CHARRIGHT) &&
		    (iMessage != SCI_CHARLEFTEXTEND) &&
		    (iMessage != SCI_EDITTOGGLEOVERTYPE) &&
		    (iMessage != SCI_DELETEBACK)
		) {
			ct.CallTipCancel();
		}
		if (iMessage == SCI_DELETEBACK) {
			if (currentPos <= ct.posStartCallTip) {
				ct.CallTipCancel();
			}
		}
	}
	return Editor::KeyCommand(iMessage);
}

void ScintillaBase::AutoCompleteStart(int lenEntered, const char *list) {
	//Platform::DebugPrintf("AutoComplete %s\n", list);
	ct.CallTipCancel();

	if (ac.chooseSingle) {
		if (list && !strchr(list, ac.GetSeparator())) {
			if (ac.ignoreCase) {
				SetEmptySelection(currentPos - lenEntered);
				pdoc->DeleteChars(currentPos, lenEntered);
				SetEmptySelection(currentPos);
				pdoc->InsertString(currentPos, list);
				SetEmptySelection(currentPos + strlen(list));
			} else {
				SetEmptySelection(currentPos);
				pdoc->InsertString(currentPos, list + lenEntered);
				SetEmptySelection(currentPos + strlen(list + lenEntered));
			}
			return;
		}
	}
	ac.Start(wDraw, idAutoComplete, currentPos, lenEntered);

	PRectangle rcClient = GetClientRectangle();
	Point pt = LocationFromPosition(currentPos-lenEntered);

	int heightLB = 100;
	int widthLB = 100;
	if (pt.x >= rcClient.right - widthLB) {
		HorizontalScrollTo(xOffset + pt.x - rcClient.right + widthLB);
		Redraw();
		pt = LocationFromPosition(currentPos);
	}
	PRectangle rcac;
	rcac.left = pt.x - 5;
	if (pt.y >= rcClient.bottom - heightLB && // Wont fit below.
	    pt.y >= (rcClient.bottom + rcClient.top) / 2) { // and there is more room above.
		rcac.top = pt.y - heightLB;
		if (rcac.top < 0) {
			heightLB += rcac.top;
			rcac.top = 0;
		}
	} else {
		rcac.top = pt.y + vs.lineHeight;
	}
	rcac.right = rcac.left + widthLB;
	rcac.bottom = Platform::Minimum(rcac.top + heightLB, rcClient.bottom);
	ac.lb.SetPositionRelative(rcac, wMain);
	ac.lb.SetFont(vs.styles[STYLE_DEFAULT].font);
	ac.lb.SetAverageCharWidth(vs.styles[STYLE_DEFAULT].aveCharWidth);

	ac.SetList(list);

	// Fiddle the position of the list so it is right next to the target and wide enough for all its strings
	PRectangle rcList = ac.lb.GetDesiredRect();
	int heightAlloced = rcList.bottom - rcList.top;
	widthLB = Platform::Maximum(widthLB, rcList.right - rcList.left);
	// Make an allowance for large strings in list
	rcList.left = pt.x - 5;
	rcList.right = rcList.left + widthLB;
	if (pt.y >= rcClient.bottom - heightLB && // Wont fit below.
	    pt.y >= (rcClient.bottom + rcClient.top) / 2) { // and there is more room above.
		rcList.top = pt.y - heightAlloced;
	} else {
		rcList.top = pt.y + vs.lineHeight;
	}
	rcList.bottom = rcList.top + heightAlloced;
	ac.lb.SetPositionRelative(rcList, wMain);
	ac.Show();
	if (lenEntered != 0) {
		AutoCompleteMoveToCurrentWord();
	}		
}

void ScintillaBase::AutoCompleteCancel() {
	ac.Cancel();
}

void ScintillaBase::AutoCompleteMove(int delta) {
	ac.Move(delta);
}

void ScintillaBase::AutoCompleteMoveToCurrentWord() {
	char wordCurrent[1000];
	int i;
	int startWord = ac.posStart - ac.startLen;
	for (i = startWord; i < currentPos; i++)
		wordCurrent[i - startWord] = pdoc->CharAt(i);
	wordCurrent[i - startWord] = '\0';
	ac.Select(wordCurrent);
}

void ScintillaBase::AutoCompleteChanged(char ch) {
	if (ac.IsFillUpChar(ch)) {
		AutoCompleteCompleted(ch);
	} else if (currentPos <= ac.posStart - ac.startLen) {
		ac.Cancel();
	} else if (ac.cancelAtStartPos && currentPos <= ac.posStart) {
		ac.Cancel();
	} else if (ac.IsStopChar(ch)) {
		ac.Cancel();
	} else {
		AutoCompleteMoveToCurrentWord();
	}
}

void ScintillaBase::AutoCompleteCompleted(char fillUp/*='\0'*/) {
	int item = ac.lb.GetSelection();
	char selected[1000];
	if (item != -1) {
		ac.lb.GetValue(item, selected, sizeof(selected));
	}
	ac.Cancel();
	
	if (ac.ignoreCase) {
		if (currentPos != ac.posStart) {
			pdoc->DeleteChars(ac.posStart, currentPos - ac.posStart);
		}
		SetEmptySelection(ac.posStart - ac.startLen);
		pdoc->DeleteChars(ac.posStart - ac.startLen, ac.startLen);
		if (item != -1) {
			SString piece = selected;
			if (fillUp)
				piece += fillUp;
			pdoc->InsertString(currentPos, piece.c_str());
			SetEmptySelection(currentPos + piece.length());
		}
	} else {
		if (currentPos != ac.posStart) {
			pdoc->DeleteChars(ac.posStart, currentPos - ac.posStart);
		}
		SetEmptySelection(ac.posStart);
		if (item != -1) {
			SString piece = selected + ac.startLen;
			if (fillUp)
				piece += fillUp;
			pdoc->InsertString(currentPos, piece.c_str());
			SetEmptySelection(currentPos + piece.length());
		}
	}
}

void ScintillaBase::ContextMenu(Point pt) {
	popup.CreatePopUp();
	AddToPopUp("Undo", idcmdUndo, pdoc->CanUndo());
	AddToPopUp("Redo", idcmdRedo, pdoc->CanRedo());
	AddToPopUp("");
	AddToPopUp("Cut", idcmdCut, currentPos != anchor);
	AddToPopUp("Copy", idcmdCopy, currentPos != anchor);
	AddToPopUp("Paste", idcmdPaste, WndProc(SCI_CANPASTE, 0, 0));
	AddToPopUp("Delete", idcmdDelete, currentPos != anchor);
	AddToPopUp("");
	AddToPopUp("Select All", idcmdSelectAll);
	popup.Show(pt, wMain);
}

void ScintillaBase::CancelModes() {
	AutoCompleteCancel();
	ct.CallTipCancel();
	Editor::CancelModes();
}

void ScintillaBase::ButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt) {
	CancelModes();
	Editor::ButtonDown(pt, curTime, shift, ctrl, alt);
}

#ifdef SCI_LEXER
void ScintillaBase::Colourise(int start, int end) {
	int lengthDoc = Platform::SendScintilla(wMain.GetID(), SCI_GETLENGTH, 0, 0);
	if (end == -1)
		end = lengthDoc;
	int len = end - start;

	//WindowAccessor styler(wMain.GetID(), props);
	DocumentAccessor styler(pdoc, props);

	int styleStart = 0;
	if (start > 0)
		styleStart = styler.StyleAt(start - 1);
	styler.SetCodePage(pdoc->dbcsCodePage);
	
	LexerModule::Colourise(start, len, styleStart, lexLanguage, keyWordLists, styler);
	styler.Flush();
}
#endif

void ScintillaBase::NotifyStyleToNeeded(int endStyleNeeded) {
#ifdef SCI_LEXER
	if (lexLanguage != SCLEX_CONTAINER) {
		int endStyled = Platform::SendScintilla(wMain.GetID(), SCI_GETENDSTYLED, 0, 0);
		int lineEndStyled = Platform::SendScintilla(wMain.GetID(), SCI_LINEFROMPOSITION, endStyled, 0);
		endStyled = Platform::SendScintilla(wMain.GetID(), SCI_POSITIONFROMLINE, lineEndStyled, 0);
		Colourise(endStyled, endStyleNeeded);
		return;
	}
#endif
	Editor::NotifyStyleToNeeded(endStyleNeeded);
}

long ScintillaBase::WndProc(unsigned int iMessage, unsigned long wParam, long lParam) {
	switch (iMessage) {
	case SCI_AUTOCSHOW:
		AutoCompleteStart(wParam, reinterpret_cast<const char *>(lParam));
		break;

	case SCI_AUTOCCANCEL:
		AutoCompleteCancel();
		break;

	case SCI_AUTOCACTIVE:
		return ac.Active();

	case SCI_AUTOCPOSSTART:
		return ac.posStart;

	case SCI_AUTOCCOMPLETE:
		AutoCompleteCompleted();
		break;

	case SCI_AUTOCSETSEPARATOR:
		ac.SetSeparator(static_cast<char>(wParam));
		break;

	case SCI_AUTOCGETSEPARATOR:
		return ac.GetSeparator();

	case SCI_AUTOCSTOPS:
		ac.SetStopChars(reinterpret_cast<char *>(lParam));
		break;
		
	case SCI_AUTOCSELECT:
		ac.Select(reinterpret_cast<char *>(lParam));
		break;
	
	case SCI_AUTOCSETCANCELATSTART:
		ac.cancelAtStartPos = wParam;
		break;
	
	case SCI_AUTOCGETCANCELATSTART:
		return ac.cancelAtStartPos;

	case SCI_AUTOCSETFILLUPS:
		ac.SetFillUpChars(reinterpret_cast<char *>(lParam));
		break;

	case SCI_AUTOCSETCHOOSESINGLE:
		ac.chooseSingle = wParam;
		break;

	case SCI_AUTOCGETCHOOSESINGLE:
		return ac.chooseSingle;
		
	case SCI_AUTOCSETIGNORECASE:
		ac.ignoreCase = wParam;
		break;
		
	case SCI_AUTOCGETIGNORECASE:
		return ac.ignoreCase;
		
	case SCI_CALLTIPSHOW: {
			AutoCompleteCancel();
			if (!ct.wCallTip.Created()) {
				PRectangle rc = ct.CallTipStart(currentPos, LocationFromPosition(wParam),
				                                reinterpret_cast<char *>(lParam),
				                                vs.styles[STYLE_DEFAULT].fontName, vs.styles[STYLE_DEFAULT].size);
				// If the call-tip window would be out of the client
				// space, adjust so it displays above the text.
				PRectangle rcClient = GetClientRectangle();
				if (rc.bottom > rcClient.bottom) {
					int offset = vs.lineHeight + rc.Height();
					rc.top -= offset;
					rc.bottom -= offset;
				}
				// Now display the window.
				CreateCallTipWindow(rc);
				ct.wCallTip.SetPositionRelative(rc, wDraw);
				ct.wCallTip.Show();
			}
		}
		break;

	case SCI_CALLTIPCANCEL:
		ct.CallTipCancel();
		break;

	case SCI_CALLTIPACTIVE:
		return ct.inCallTipMode;

	case SCI_CALLTIPPOSSTART:
		return ct.posStartCallTip;

	case SCI_CALLTIPSETHLT:
		ct.SetHighlight(wParam, lParam);
		break;

	case SCI_CALLTIPSETBACK:
		ct.colourBG = Colour(wParam);
		InvalidateStyleRedraw();
		break;
		
#ifdef SCI_LEXER
	case SCI_SETLEXER:
		lexLanguage = wParam;
		break;
		
	case SCI_GETLEXER:
		return lexLanguage;
		
	case SCI_COLOURISE:
		Colourise(wParam, lParam);
		Redraw();
		break;
		
	case SCI_SETPROPERTY:
		props.Set(reinterpret_cast<const char *>(wParam), 
			reinterpret_cast<const char *>(lParam));
		break;
		
	case SCI_SETKEYWORDS:
		if (wParam < numWordLists) {
			keyWordLists[wParam]->Clear();
			keyWordLists[wParam]->Set(reinterpret_cast<const char *>(lParam));
		}
		break;
#endif

	default:
		return Editor::WndProc(iMessage, wParam, lParam);
	}
	return 0l;
}
