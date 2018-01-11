// Scintilla source code edit control
/** @file ScintillaBase.h
 ** Defines an enhanced subclass of Editor with calltips, autocomplete and context menu.
 **/
// Copyright 1998-2002 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SCINTILLABASE_H
#define SCINTILLABASE_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

#ifdef SCI_LEXER
class LexState;
#endif

/**
 */
class ScintillaBase : public Editor {
protected:
	/** Enumeration of commands and child windows. */
	enum {
		idCallTip=1,
		idAutoComplete=2,

		idcmdUndo=10,
		idcmdRedo=11,
		idcmdCut=12,
		idcmdCopy=13,
		idcmdPaste=14,
		idcmdDelete=15,
		idcmdSelectAll=16
	};

	enum { maxLenInputIME = 200 };

	int displayPopupMenu;
	Menu popup;
	AutoComplete ac;

	CallTip ct;

	int listType;			///< 0 is an autocomplete list
	int maxListWidth;		/// Maximum width of list, in average character widths
	int multiAutoCMode; /// Mode for autocompleting when multiple selections are present

#ifdef SCI_LEXER
	LexState *DocumentLexState();
	void SetLexer(uptr_t wParam);
	void SetLexerLanguage(const char *languageName);
	void Colourise(int start, int end);
#endif

	ScintillaBase();
	// Deleted so ScintillaBase objects can not be copied.
	explicit ScintillaBase(const ScintillaBase &) = delete;
	ScintillaBase &operator=(const ScintillaBase &) = delete;
	virtual ~ScintillaBase();
	void Initialise() override {}
	void Finalise() override;

	void AddCharUTF(const char *s, unsigned int len, bool treatAsDBCS=false) override;
	void Command(int cmdId);
	void CancelModes() override;
	int KeyCommand(unsigned int iMessage) override;

	void AutoCompleteInsert(Sci::Position startPos, int removeLen, const char *text, int textLen);
	void AutoCompleteStart(int lenEntered, const char *list);
	void AutoCompleteCancel();
	void AutoCompleteMove(int delta);
	int AutoCompleteGetCurrent() const;
	int AutoCompleteGetCurrentText(char *buffer) const;
	void AutoCompleteCharacterAdded(char ch);
	void AutoCompleteCharacterDeleted();
	void AutoCompleteCompleted(char ch, unsigned int completionMethod);
	void AutoCompleteMoveToCurrentWord();
	static void AutoCompleteDoubleClick(void *p);

	void CallTipClick();
	void CallTipShow(Point pt, const char *defn);
	virtual void CreateCallTipWindow(PRectangle rc) = 0;

	virtual void AddToPopUp(const char *label, int cmd=0, bool enabled=true) = 0;
	bool ShouldDisplayPopup(Point ptInWindowCoordinates) const;
	void ContextMenu(Point pt);

	void ButtonDownWithModifiers(Point pt, unsigned int curTime, int modifiers) override;
	void ButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt) override;
	void RightButtonDownWithModifiers(Point pt, unsigned int curTime, int modifiers) override;

	void NotifyStyleToNeeded(Sci::Position endStyleNeeded) override;
	void NotifyLexerChanged(Document *doc, void *userData) override;

public:
	// Public so scintilla_send_message can use it
	sptr_t WndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam) override;
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
