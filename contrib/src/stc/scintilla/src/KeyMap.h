// Scintilla source code edit control
// KeyMap.h - defines a mapping between keystrokes and commands
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef KEYTOCOMMAND_H
#define KEYTOCOMMAND_H

#define SCI_NORM 0
#define SCI_SHIFT SHIFT_PRESSED
#define SCI_CTRL LEFT_CTRL_PRESSED
#define SCI_ALT LEFT_ALT_PRESSED
#define SCI_CSHIFT (SCI_CTRL | SCI_SHIFT)
#define SCI_ASHIFT (SCI_ALT | SCI_SHIFT)

class KeyToCommand {
public:
	int key;
	int modifiers;
	UINT msg;
};

class KeyMap {
	KeyToCommand *kmap;
	int len;
	int alloc;
	static KeyToCommand MapDefault[];
public:
	KeyMap();
	~KeyMap();
	void Clear();
	void AssignCmdKey(int key, int modifiers, UINT msg);
	UINT Find(int key, int modifiers);	// 0 returned on failure
};

#endif
