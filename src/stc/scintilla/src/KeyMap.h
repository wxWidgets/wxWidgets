// Scintilla source code edit control
// KeyMap.h - defines a mapping between keystrokes and commands
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef KEYTOCOMMAND_H
#define KEYTOCOMMAND_H

#define SCI_NORM 0
#define SCI_SHIFT SCMOD_SHIFT
#define SCI_CTRL SCMOD_CTRL
#define SCI_ALT SCMOD_ALT
#define SCI_CSHIFT (SCI_CTRL | SCI_SHIFT)
#define SCI_ASHIFT (SCI_ALT | SCI_SHIFT)

class KeyToCommand {
public:
	int key;
	int modifiers;
	unsigned int msg;
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
	void AssignCmdKey(int key, int modifiers, unsigned int msg);
	unsigned int Find(int key, int modifiers);	// 0 returned on failure
};

#endif
