// SciTE - Scintilla based Text Editor
// Accessor.h - rapid easy access to contents of a Scintilla
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

enum { wsSpace = 1, wsTab = 2, wsSpaceTab = 4, wsInconsistent=8};

class Accessor {
protected:
	// bufferSize is a trade off between time taken to copy the characters and SendMessage overhead
	// slopSize positions the buffer before the desired position in case there is some backtracking
	enum {bufferSize=4000, slopSize=bufferSize/8};
	char buf[bufferSize+1];
	WindowID id;
	PropSet &props;
	int startPos;
	int endPos;
	int lenDoc;
	int offset;	// Optional but including an offset makes GCC generate better code 
	int codePage;	
	bool InternalIsLeadByte(char ch);
	void Fill(int position);
public:
	Accessor(WindowID id_, PropSet &props_, int offset_=0) : 
			id(id_), props(props_), startPos(0x7FFFFFFF), endPos(0), 
			lenDoc(-1), offset(offset_), codePage(0) {
	}
	void SetCodePage(int codePage_) { codePage = codePage_; }
	char operator[](int position) {
		position += offset;
		if (position < startPos || position >= endPos) {
			Fill(position);
		}
		return buf[position - startPos];
	}
	char SafeGetCharAt(int position, char chDefault=' ') {
		// Safe version of operator[], returning a defined value for invalid position 
		position += offset;
		if (position < startPos || position >= endPos) {
			Fill(position);
			if (position < startPos || position >= endPos) {
				// Position is outside range of document 
				return chDefault;
			}
		}
		return buf[position - startPos];
	}
	bool IsLeadByte(char ch) {
		return codePage && InternalIsLeadByte(ch);
	}
	char StyleAt(int position);
	int GetLine(int position);
	int LineStart(int line);
	int LevelAt(int line);
	int Length();
	void Flush() {
		startPos = 0x7FFFFFFF;
		lenDoc = -1;
	}
	int GetLineState(int line);
	int SetLineState(int line, int state);
	PropSet &GetPropSet() { return props; }
};

class StylingContext;

typedef bool (*PFNIsCommentLeader)(StylingContext &styler, int pos, int len);

class StylingContext : public Accessor {
	char styleBuf[bufferSize];
	int validLen;
	char chFlags;
	char chWhile;
	unsigned int startSeg;
public:
	StylingContext(WindowID id_, PropSet &props_, int offset_=0) : 
		Accessor(id_,props_,offset_), validLen(0), chFlags(0) {}
	void StartAt(unsigned int start, char chMask=31);
	void SetFlags(char chFlags_, char chWhile_) {chFlags = chFlags_; chWhile = chWhile_; };
	unsigned int GetStartSegment() { return startSeg; }
	void StartSegment(unsigned int pos);
	void ColourTo(unsigned int pos, int chAttr);
	int GetLine(int position);
	void SetLevel(int line, int level);
	void Flush();
	int IndentAmount(int line, int *flags, PFNIsCommentLeader pfnIsCommentLeader = 0);
};

