// Scintilla source code edit control
/** @file StyleContext.cxx
 ** Lexer infrastructure.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// This file is in the public domain.

// All languages handled so far can treat all characters >= 0x80 as one class
// which just continues the current token or starts an identifier if in default.
// DBCS treated specially as the second character can be < 0x80 and hence 
// syntactically significant. UTF-8 avoids this as all trail bytes are >= 0x80
class StyleContext {
	Accessor &styler;
	int endPos;
	StyleContext& operator=(const StyleContext&) {
		return *this;
	}
public:
	int currentPos;
	bool atLineStart;
	bool atLineEnd;
	int state;
	int chPrev;
	int ch;
	int chNext;

	StyleContext(unsigned int startPos, int length,
                        int initStyle, Accessor &styler_, char chMask=31) : 
		styler(styler_),
		endPos(startPos + length),
		currentPos(startPos), 
		atLineStart(true),
		atLineEnd(false),
		state(initStyle), 
		chPrev(0),
		ch(0), 
		chNext(0) {
		styler.StartAt(startPos, chMask);
		styler.StartSegment(startPos);
		int pos = currentPos;
		ch = static_cast<unsigned char>(styler.SafeGetCharAt(pos));
		if (styler.IsLeadByte(static_cast<char>(ch))) {
			pos++;
			ch = ch << 8;
			ch |= static_cast<unsigned char>(styler.SafeGetCharAt(pos));
		}
		chNext = static_cast<unsigned char>(styler.SafeGetCharAt(pos+1));
		if (styler.IsLeadByte(static_cast<char>(chNext))) {
			chNext = chNext << 8;
			chNext |= static_cast<unsigned char>(styler.SafeGetCharAt(pos+2));
		}
		atLineEnd = (ch == '\r' && chNext != '\n') || (ch == '\n') || (currentPos >= endPos);
	}
	void Complete() {
		styler.ColourTo(currentPos - 1, state);
	}
	bool More() {
		return currentPos <= endPos;
	}
	void Forward() {
		atLineStart = atLineEnd;
		// A lot of this is repeated from the constructor - TODO: merge code
		chPrev = ch;
		currentPos++;
		if (ch >= 0x100)
			currentPos++;
		ch = chNext;
		chNext = static_cast<unsigned char>(styler.SafeGetCharAt(currentPos+1));
		if (styler.IsLeadByte(static_cast<char>(chNext))) {
			chNext = chNext << 8;
			chNext |= static_cast<unsigned char>(styler.SafeGetCharAt(currentPos + 2));
		}
		// Trigger on CR only (Mac style) or either on LF from CR+LF (Dos/Win) or on LF alone (Unix)
		// Avoid triggering two times on Dos/Win
		// End of line
		atLineEnd = (ch == '\r' && chNext != '\n') || (ch == '\n') || (currentPos >= endPos);
	}
	void ChangeState(int state_) {
		state = state_;
	}
	void SetState(int state_) {
		styler.ColourTo(currentPos - 1, state);
		state = state_;
	}
	void ForwardSetState(int state_) {
		Forward();
		styler.ColourTo(currentPos - 1, state);
		state = state_;
	}
	int LengthCurrent() {
		return currentPos - styler.GetStartSegment();
	}
	int GetRelative(int n) {
		return styler.SafeGetCharAt(currentPos+n);
	}
	bool Match(char ch0) {
		return ch == ch0;
	}
	bool Match(char ch0, char ch1) {
		return (ch == ch0) && (chNext == ch1);
	}
	bool Match(const char *s) {
		if (ch != *s)
			return false;
		s++;
		if (chNext != *s)
			return false;
		s++;
		for (int n=2; *s; n++) {
			if (*s != styler.SafeGetCharAt(currentPos+n))
				return false;
			s++;
		}
		return true;
	}
	// Non-inline
	void GetCurrent(char *s, int len);
	void GetCurrentLowered(char *s, int len);
};

inline bool IsASpace(unsigned int ch) {
    return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

inline bool IsADigit(unsigned int ch) {
	return (ch >= '0') && (ch <= '9');
}
