// Scintilla source code edit control
// KeyMap.cxx  - defines a mapping between keystrokes and commands
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include "Platform.h"

#include "Scintilla.h"

#include "KeyMap.h"

KeyMap::KeyMap() : kmap(0), len(0), alloc(0) {
	for (int i = 0; MapDefault[i].key; i++) {
		AssignCmdKey(MapDefault[i].key, 
			MapDefault[i].modifiers,
             		MapDefault[i].msg);
	}
}

KeyMap::~KeyMap() {
	Clear();
}

void KeyMap::Clear() {
	delete []kmap;
	kmap = 0;
	len = 0;
	alloc = 0;
}

void KeyMap::AssignCmdKey(int key, int modifiers, UINT msg) {
	if ((len+1) >= alloc) {
		KeyToCommand *ktcNew = new KeyToCommand[alloc + 5];
		if (!ktcNew)
			return;
		for (int k=0;k<len;k++)
			ktcNew[k] = kmap[k];
		alloc += 5;
		delete []kmap;
		kmap = ktcNew;
	}
	for (int keyIndex = 0; keyIndex < len; keyIndex++) {
		if ((key == kmap[keyIndex].key) && (modifiers == kmap[keyIndex].modifiers)) {
			kmap[keyIndex].msg = msg;
			return;
		}
	}
	kmap[len].key = key;
	kmap[len].modifiers = modifiers;
	kmap[len].msg = msg;
	len++;
}

UINT KeyMap::Find(int key, int modifiers) {
	for (int i=0; i < len; i++) {
		if ((key == kmap[i].key) && (modifiers == kmap[i].modifiers)) {
			return kmap[i].msg;
		}
	}
	return 0;
}

KeyToCommand KeyMap::MapDefault[] = {
    {VK_DOWN,		SCI_NORM,	SCI_LINEDOWN},
    {VK_DOWN,		SCI_SHIFT,	SCI_LINEDOWNEXTEND},
    {VK_DOWN,		SCI_CTRL,	SCI_LINESCROLLDOWN},
    {VK_UP,			SCI_NORM,	SCI_LINEUP},
    {VK_UP,			SCI_SHIFT,	SCI_LINEUPEXTEND},
    {VK_UP,			SCI_CTRL,	SCI_LINESCROLLUP},
    {VK_LEFT,		SCI_NORM,	SCI_CHARLEFT},
    {VK_LEFT,		SCI_SHIFT,	SCI_CHARLEFTEXTEND},
    {VK_LEFT,		SCI_CTRL,	SCI_WORDLEFT},
    {VK_LEFT,		SCI_CSHIFT,	SCI_WORDLEFTEXTEND},
    {VK_RIGHT,		SCI_NORM,	SCI_CHARRIGHT},
    {VK_RIGHT,		SCI_SHIFT,	SCI_CHARRIGHTEXTEND},
    {VK_RIGHT,		SCI_CTRL,	SCI_WORDRIGHT},
    {VK_RIGHT,		SCI_CSHIFT,	SCI_WORDRIGHTEXTEND},
    {VK_HOME, 		SCI_NORM, 	SCI_VCHOME},
    {VK_HOME, 		SCI_SHIFT, 	SCI_VCHOMEEXTEND},
    {VK_HOME, 		SCI_CTRL, 	SCI_DOCUMENTSTART},
    {VK_HOME, 		SCI_CSHIFT, 	SCI_DOCUMENTSTARTEXTEND},
    {VK_END,	 	SCI_NORM, 	SCI_LINEEND},
    {VK_END,	 	SCI_SHIFT, 	SCI_LINEENDEXTEND},
    {VK_END, 		SCI_CTRL, 	SCI_DOCUMENTEND},
    {VK_END, 		SCI_CSHIFT, 	SCI_DOCUMENTENDEXTEND},
    {VK_PRIOR,		SCI_NORM, 	SCI_PAGEUP},
    {VK_PRIOR,		SCI_SHIFT, 	SCI_PAGEUPEXTEND},
    {VK_NEXT, 		SCI_NORM, 	SCI_PAGEDOWN},
    {VK_NEXT, 		SCI_SHIFT, 	SCI_PAGEDOWNEXTEND},
    {VK_DELETE, 	SCI_NORM,	WM_CLEAR},
    {VK_DELETE, 	SCI_SHIFT,	WM_CUT},
    {VK_DELETE, 	SCI_CTRL,	SCI_DELWORDRIGHT},
    {VK_INSERT, 		SCI_NORM,	SCI_EDITTOGGLEOVERTYPE},
    {VK_INSERT, 		SCI_SHIFT,	WM_PASTE},
    {VK_INSERT, 		SCI_CTRL,	WM_COPY},
    {VK_ESCAPE,  	SCI_NORM,	SCI_CANCEL},
    {VK_BACK,		SCI_NORM, 	SCI_DELETEBACK},
    {VK_BACK,		SCI_CTRL, 	SCI_DELWORDLEFT},
    {'Z', 			SCI_CTRL,	WM_UNDO},
    {'Y', 			SCI_CTRL,	SCI_REDO},
    {'X', 			SCI_CTRL,	WM_CUT},
    {'C', 			SCI_CTRL,	WM_COPY},
    {'V', 			SCI_CTRL,	WM_PASTE},
    {'A', 			SCI_CTRL,	SCI_SELECTALL},
    {VK_TAB,		SCI_NORM,	SCI_TAB},
    {VK_TAB,		SCI_SHIFT,	SCI_BACKTAB},
    {VK_RETURN, 	SCI_NORM,	SCI_NEWLINE},
    {VK_ADD, 		SCI_CTRL,	SCI_ZOOMIN},
    {VK_SUBTRACT,	SCI_CTRL,	SCI_ZOOMOUT},
    {VK_DIVIDE,	SCI_CTRL,	SCI_SETZOOM},
    //'L', 			SCI_CTRL,		SCI_FORMFEED,
    {'L', 			SCI_CTRL,	SCI_LINECUT},
    {'L', 			SCI_CSHIFT,	SCI_LINEDELETE},
    {'T', 			SCI_CTRL,	SCI_LINETRANSPOSE},
    {'U', 			SCI_CTRL,	SCI_LOWERCASE},
    {'U', 			SCI_CSHIFT,	SCI_UPPERCASE},
    {0,0,0},
};

