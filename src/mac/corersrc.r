#include <Types.r>

#if UNIVERSAL_INTERFACES_VERSION > 0x320
	#include <ControlDefinitions.r>
#endif

#define kMacOKAlertResourceID 128
#define kMacYesNoAlertResourceID 129
#define kMacYesNoCancelAlertResourceID 130
#define kMacNoYesAlertResourceID 131
#define kMacNoYesCancelAlertResourceID 132

resource 'ALRT' (kMacOKAlertResourceID, purgeable) {
	{70, 50, 198, 470},
	kMacOKAlertResourceID,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter
};

resource 'ALRT' (kMacYesNoAlertResourceID, purgeable) {
	{70, 50, 198, 470},
	kMacYesNoAlertResourceID,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter
};

resource 'ALRT' (kMacYesNoAlertResourceID + 10, purgeable) {
	{70, 50, 198, 470},
	kMacYesNoAlertResourceID + 10,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter
};

resource 'ALRT' (kMacYesNoAlertResourceID + 20, purgeable) {
	{70, 50, 198, 470},
	kMacYesNoAlertResourceID + 20,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter
};

resource 'ALRT' (kMacYesNoCancelAlertResourceID, purgeable) {
	{70, 50, 198, 470},
	130,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter
};

resource 'ALRT' (kMacNoYesAlertResourceID, purgeable) {
	{70, 50, 198, 470},
	131,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter
};

resource 'ALRT' (kMacNoYesCancelAlertResourceID, purgeable) {
	{70, 50, 198, 470},
	132,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter
};

resource 'DITL' (kMacOKAlertResourceID, purgeable) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{100, 340, 120, 408},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{38, 64, 88, 408},
		StaticText {
			disabled,
			"^1"
		},
		/* [3] */
		{10, 64, 30, 412},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (kMacYesNoAlertResourceID, purgeable) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{100, 340, 120, 408},
		Button {
			enabled,
			"Ja"
		},
		/* [2] */
		{100, 260, 120, 324},
		Button {
			enabled,
			"Nein"
		},
		/* [3] */
		{38, 64, 88, 408},
		StaticText {
			disabled,
			"^1"
		},
		/* [4] */
		{10, 64, 30, 412},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (kMacYesNoAlertResourceID + 10, purgeable) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{100, 340, 120, 408},
		Button {
			enabled,
			"Oui"
		},
		/* [2] */
		{100, 260, 120, 324},
		Button {
			enabled,
			"Non"
		},
		/* [3] */
		{38, 64, 88, 408},
		StaticText {
			disabled,
			"^1"
		},
		/* [4] */
		{10, 64, 30, 412},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (kMacYesNoAlertResourceID + 20, purgeable) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{100, 340, 120, 408},
		Button {
			enabled,
			"Si"
		},
		/* [2] */
		{100, 260, 120, 324},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{38, 64, 88, 408},
		StaticText {
			disabled,
			"^1"
		},
		/* [4] */
		{10, 64, 30, 412},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (kMacYesNoCancelAlertResourceID, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{100, 340, 120, 408},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{100, 260, 120, 324},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{100, 65, 120, 145},
		Button {
			enabled,
			"Cancel"
		},
		/* [4] */
		{38, 64, 88, 408},
		StaticText {
			disabled,
			"^1"
		},
		/* [5] */
		{10, 64, 30, 412},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (kMacNoYesAlertResourceID, purgeable) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{100, 340, 120, 408},
		Button {
			enabled,
			"No"
		},
		/* [2] */
		{100, 260, 120, 324},
		Button {
			enabled,
			"Yes"
		},
		/* [3] */
		{38, 64, 88, 408},
		StaticText {
			disabled,
			"^1"
		},
		/* [4] */
		{10, 64, 30, 412},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (kMacNoYesCancelAlertResourceID, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{100, 340, 120, 408},
		Button {
			enabled,
			"No"
		},
		/* [2] */
		{100, 260, 120, 324},
		Button {
			enabled,
			"Yes"
		},
		/* [3] */
		{100, 65, 120, 145},
		Button {
			enabled,
			"Cancel"
		},
		/* [4] */
		{38, 64, 88, 408},
		StaticText {
			disabled,
			"^1"
		},
		/* [5] */
		{10, 64, 30, 412},
		StaticText {
			disabled,
			"^0"
		}
	}
};

data 'DLGX' (128) {
	$"0743 6869 6361 676F 0000 0000 0000 0000"            /* .Chicago........ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0001 0004 0004 0000 0000"            /* ................ */
	$"0003 0002 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000"                                     /* ...... */
};

data 'DLGX' (129) {
	$"0743 6869 6361 676F 0000 0000 0000 0000"            /* .Chicago........ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0001 0004 0004 0000 0000"            /* ................ */
	$"0004 0002 0000 0000 0000 0000 0000 0002"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000"                                               /* .. */
};

data 'DLGX' (131) {
	$"0743 6869 6361 676F 0000 0000 0000 0000"            /* .Chicago........ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0001 0004 0004 0000 0000"            /* ................ */
	$"0004 0002 0000 0000 0000 0000 0000 0002"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000"                                               /* .. */
};

data 'DLGX' (132) {
	$"0743 6869 6361 676F 0000 0000 0000 0000"            /* .Chicago........ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0001 0004 0004 0000 0000"            /* ................ */
	$"0005 0002 0000 0000 0000 0000 0000 0002"            /* ................ */
	$"0000 0000 0000 0000 0000 0002 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000"                 /* .............. */
};

resource 'ldes' ( 128 )
{
	versionZero 
	{
		0 , 
		0 ,
		0 , 
		0 , 
		hasVertScroll , 
		noHorizScroll , 
		128 , 
		noGrowSpace , 
	}
} ;

resource 'CURS' (9, "Hand") {
	$"0380 04C0 04C0 04C0 04C0 04C0 74F8 9CAE"
	$"4CAB 240B 2403 1003 0803 0806 0406 0406",
	$"0380 07C0 07C0 07C0 07C0 07C0 77F8 FFFE"
	$"7FFF 3FFF 3FFF 1FFF 0FFF 0FFE 07FE 07FE",
	{0, 7}
};

resource 'CURS' (10, "SizeWE") {
	$"0000 0180 0180 0180 0180 1188 318C 7FFE"
	$"318C 1188 0180 0180 0180 0180",
	$"03C0 03C0 03C0 03C0 1BD8 3BDC 7BFE FFFF"
	$"7FFE 3BDC 1BD8 03C0 03C0 03C0 03C0",
	{7, 7}
};

resource 'CURS' (11, "Size NS") {
	$"0000 0080 01C0 03E0 0080 0080 0080 3FFE"
	$"3FFE 0080 0080 0080 03E0 01C0 0080",
	$"0080 01C0 03E0 07F0 07F0 0180 7FFF 7FFF"
	$"7FFF 7FFF 01C0 07F0 07F0 03E0 01C0 0080",
	{7, 7}
};

// get folder

resource 'DITL' (250, purgeable) {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{157, 253, 177, 333},
		Button {
			enabled,
			"Open"
		},
		/* [2] */
		{126, 253, 146, 333},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		HelpItem {
			disabled,
			HMScanhdlg {
				-6043
			}
		},
		/* [4] */
		{30, 236, 46, 338},
		UserItem {
			enabled
		},
		/* [5] */
		{54, 253, 74, 333},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{82, 253, 102, 333},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{51, 13, 181, 231},
		UserItem {
			enabled
		},
		/* [8] */
		{28, 13, 47, 231},
		UserItem {
			enabled
		},
		/* [9] */
		{113, 252, 114, 334},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{189, 24, 209, 211},
		Button {
			enabled,
			""
		},
		/* [11] */
		{6, 15, 23, 300},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DLOG' (250, purgeable) {
	{0, 0, 215, 344},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	250,
	"" , 0 
};

resource 'STR#' (250) {
	{	/* array StringArray: 3 elements */
		/* [1] */
		"Select \"^0\"",
		/* [2] */
		"Desktop",
		/* [3] */
		"Select ^0"
	}
};

// end of get folder

// get file

resource 'DITL' (251, purgeable) {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{157, 253, 177, 333},
		Button {
			enabled,
			"Open"
		},
		/* [2] */
		{126, 253, 146, 333},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		HelpItem {
			disabled,
			HMScanhdlg {
				-6043
			}
		},
		/* [4] */
		{30, 236, 46, 338},
		UserItem {
			enabled
		},
		/* [5] */
		{54, 253, 74, 333},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{82, 253, 102, 333},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{51, 13, 181, 231},
		UserItem {
			enabled
		},
		/* [8] */
		{28, 13, 47, 231},
		UserItem {
			enabled
		},
		/* [9] */
		{113, 252, 114, 334},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{6, 15, 23, 300},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DLOG' (251, purgeable) {
	{0, 0, 195, 344},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	251,
	"" , 0 
};

resource 'STR#' (251) {
	{	/* array StringArray: 3 elements */
		/* [1] */
		"Select \"^0\"",
		/* [2] */
		"Desktop",
		/* [3] */
		"Select ^0"
	}
};

// end of get file