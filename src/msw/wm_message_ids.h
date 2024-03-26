
/*
 * Window Messages
 */

#define WM_NULL                         0x0000
#define WM_CREATE                       0x0001
#define WM_DESTROY                      0x0002
#define WM_MOVE                         0x0003
#define WM_SIZE                         0x0005

#define WM_ACTIVATE                     0x0006

#define WM_SETFOCUS                     0x0007
#define WM_KILLFOCUS                    0x0008
#define WM_ENABLE                       0x000A
#define WM_SETREDRAW                    0x000B
#define WM_SETTEXT                      0x000C
#define WM_GETTEXT                      0x000D
#define WM_GETTEXTLENGTH                0x000E
#define WM_PAINT                        0x000F
#define WM_CLOSE                        0x0010

#define WM_QUERYENDSESSION              0x0011
#define WM_QUERYOPEN                    0x0013
#define WM_ENDSESSION                   0x0016

#define WM_QUIT                         0x0012
#define WM_ERASEBKGND                   0x0014
#define WM_SYSCOLORCHANGE               0x0015
#define WM_SHOWWINDOW                   0x0018
#define WM_WININICHANGE                 0x001A

#define WM_SETTINGCHANGE                WM_WININICHANGE


#define WM_DEVMODECHANGE                0x001B
#define WM_ACTIVATEAPP                  0x001C
#define WM_FONTCHANGE                   0x001D
#define WM_TIMECHANGE                   0x001E
#define WM_CANCELMODE                   0x001F
#define WM_SETCURSOR                    0x0020
#define WM_MOUSEACTIVATE                0x0021
#define WM_CHILDACTIVATE                0x0022
#define WM_QUEUESYNC                    0x0023

#define WM_GETMINMAXINFO                0x0024

#define WM_PAINTICON                    0x0026
#define WM_ICONERASEBKGND               0x0027
#define WM_NEXTDLGCTL                   0x0028
#define WM_SPOOLERSTATUS                0x002A
#define WM_DRAWITEM                     0x002B
#define WM_MEASUREITEM                  0x002C
#define WM_DELETEITEM                   0x002D
#define WM_VKEYTOITEM                   0x002E
#define WM_CHARTOITEM                   0x002F
#define WM_SETFONT                      0x0030
#define WM_GETFONT                      0x0031
#define WM_SETHOTKEY                    0x0032
#define WM_GETHOTKEY                    0x0033
#define WM_QUERYDRAGICON                0x0037
#define WM_COMPAREITEM                  0x0039

#define WM_GETOBJECT                    0x003D

#define WM_COMPACTING                   0x0041
#define WM_COMMNOTIFY                   0x0044  /* no longer suported */
#define WM_WINDOWPOSCHANGING            0x0046
#define WM_WINDOWPOSCHANGED             0x0047

#define WM_POWER                        0x0048

#define WM_COPYDATA                     0x004A
#define WM_CANCELJOURNAL                0x004B

#define WM_NOTIFY                       0x004E
#define WM_INPUTLANGCHANGEREQUEST       0x0050
#define WM_INPUTLANGCHANGE              0x0051
#define WM_TCARD                        0x0052
#define WM_HELP                         0x0053
#define WM_USERCHANGED                  0x0054
#define WM_NOTIFYFORMAT                 0x0055

#define WM_CONTEXTMENU                  0x007B
#define WM_STYLECHANGING                0x007C
#define WM_STYLECHANGED                 0x007D
#define WM_DISPLAYCHANGE                0x007E
#define WM_GETICON                      0x007F
#define WM_SETICON                      0x0080

#define WM_NCCREATE                     0x0081
#define WM_NCDESTROY                    0x0082
#define WM_NCCALCSIZE                   0x0083
#define WM_NCHITTEST                    0x0084
#define WM_NCPAINT                      0x0085
#define WM_NCACTIVATE                   0x0086
#define WM_GETDLGCODE                   0x0087

#define WM_SYNCPAINT                    0x0088

#define WM_NCMOUSEMOVE                  0x00A0
#define WM_NCLBUTTONDOWN                0x00A1
#define WM_NCLBUTTONUP                  0x00A2
#define WM_NCLBUTTONDBLCLK              0x00A3
#define WM_NCRBUTTONDOWN                0x00A4
#define WM_NCRBUTTONUP                  0x00A5
#define WM_NCRBUTTONDBLCLK              0x00A6
#define WM_NCMBUTTONDOWN                0x00A7
#define WM_NCMBUTTONUP                  0x00A8
#define WM_NCMBUTTONDBLCLK              0x00A9

#define WM_NCXBUTTONDOWN                0x00AB
#define WM_NCXBUTTONUP                  0x00AC
#define WM_NCXBUTTONDBLCLK              0x00AD

#define WM_INPUT_DEVICE_CHANGE          0x00FE

#define WM_INPUT                        0x00FF

// #define WM_KEYFIRST                     0x0100
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define WM_CHAR                         0x0102
#define WM_DEADCHAR                     0x0103
#define WM_SYSKEYDOWN                   0x0104
#define WM_SYSKEYUP                     0x0105
#define WM_SYSCHAR                      0x0106
#define WM_SYSDEADCHAR                  0x0107


#define WM_UNICHAR                      0x0109


#define WM_IME_STARTCOMPOSITION         0x010D
#define WM_IME_ENDCOMPOSITION           0x010E
#define WM_IME_COMPOSITION              0x010F

#define WM_INITDIALOG                   0x0110
#define WM_COMMAND                      0x0111
#define WM_SYSCOMMAND                   0x0112
#define WM_TIMER                        0x0113
#define WM_HSCROLL                      0x0114
#define WM_VSCROLL                      0x0115
#define WM_INITMENU                     0x0116
#define WM_INITMENUPOPUP                0x0117

#define WM_GESTURE                      0x0119
#define WM_GESTURENOTIFY                0x011A

#define WM_MENUSELECT                   0x011F
#define WM_MENUCHAR                     0x0120
#define WM_ENTERIDLE                    0x0121

#define WM_MENURBUTTONUP                0x0122
#define WM_MENUDRAG                     0x0123
#define WM_MENUGETOBJECT                0x0124
#define WM_UNINITMENUPOPUP              0x0125
#define WM_MENUCOMMAND                  0x0126

#define WM_CHANGEUISTATE                0x0127
#define WM_UPDATEUISTATE                0x0128
#define WM_QUERYUISTATE                 0x0129

#define WM_CTLCOLORMSGBOX               0x0132
#define WM_CTLCOLOREDIT                 0x0133
#define WM_CTLCOLORLISTBOX              0x0134
#define WM_CTLCOLORBTN                  0x0135
#define WM_CTLCOLORDLG                  0x0136
#define WM_CTLCOLORSCROLLBAR            0x0137
#define WM_CTLCOLORSTATIC               0x0138
#define MN_GETHMENU                     0x01E1

// #define WM_MOUSEFIRST                   0x0200
#define WM_MOUSEMOVE                    0x0200
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_LBUTTONDBLCLK                0x0203
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_RBUTTONDBLCLK                0x0206
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208
#define WM_MBUTTONDBLCLK                0x0209

#define WM_MOUSEWHEEL                   0x020A

#define WM_XBUTTONDOWN                  0x020B
#define WM_XBUTTONUP                    0x020C
#define WM_XBUTTONDBLCLK                0x020D

#define WM_MOUSEHWHEEL                  0x020E

#define WM_PARENTNOTIFY                 0x0210
#define WM_ENTERMENULOOP                0x0211
#define WM_EXITMENULOOP                 0x0212

#define WM_NEXTMENU                     0x0213
#define WM_SIZING                       0x0214
#define WM_CAPTURECHANGED               0x0215
#define WM_MOVING                       0x0216

#define WM_POWERBROADCAST               0x0218

#define WM_DEVICECHANGE                 0x0219

#define WM_MDICREATE                    0x0220
#define WM_MDIDESTROY                   0x0221
#define WM_MDIACTIVATE                  0x0222
#define WM_MDIRESTORE                   0x0223
#define WM_MDINEXT                      0x0224
#define WM_MDIMAXIMIZE                  0x0225
#define WM_MDITILE                      0x0226
#define WM_MDICASCADE                   0x0227
#define WM_MDIICONARRANGE               0x0228
#define WM_MDIGETACTIVE                 0x0229

#define WM_MDISETMENU                   0x0230
#define WM_ENTERSIZEMOVE                0x0231
#define WM_EXITSIZEMOVE                 0x0232
#define WM_DROPFILES                    0x0233
#define WM_MDIREFRESHMENU               0x0234

#define WM_POINTERDEVICECHANGE          0x238
#define WM_POINTERDEVICEINRANGE         0x239
#define WM_POINTERDEVICEOUTOFRANGE      0x23A

#define WM_TOUCH                        0x0240

#define WM_NCPOINTERUPDATE              0x0241
#define WM_NCPOINTERDOWN                0x0242
#define WM_NCPOINTERUP                  0x0243
#define WM_POINTERUPDATE                0x0245
#define WM_POINTERDOWN                  0x0246
#define WM_POINTERUP                    0x0247
#define WM_POINTERENTER                 0x0249
#define WM_POINTERLEAVE                 0x024A
#define WM_POINTERACTIVATE              0x024B
#define WM_POINTERCAPTURECHANGED        0x024C
#define WM_TOUCHHITTESTING              0x024D
#define WM_POINTERWHEEL                 0x024E
#define WM_POINTERHWHEEL                0x024F
#define DM_POINTERHITTEST               0x0250
#define WM_POINTERROUTEDTO              0x0251
#define WM_POINTERROUTEDAWAY            0x0252
#define WM_POINTERROUTEDRELEASED        0x0253

#define WM_IME_SETCONTEXT               0x0281
#define WM_IME_NOTIFY                   0x0282
#define WM_IME_CONTROL                  0x0283
#define WM_IME_COMPOSITIONFULL          0x0284
#define WM_IME_SELECT                   0x0285
#define WM_IME_CHAR                     0x0286

#define WM_IME_REQUEST                  0x0288

#define WM_IME_KEYDOWN                  0x0290
#define WM_IME_KEYUP                    0x0291

#define WM_MOUSEHOVER                   0x02A1
#define WM_MOUSELEAVE                   0x02A3

#define WM_NCMOUSEHOVER                 0x02A0
#define WM_NCMOUSELEAVE                 0x02A2

#define WM_WTSSESSION_CHANGE            0x02B1

#define WM_TABLET_FIRST                 0x02c0
#define WM_TABLET_LAST                  0x02df

#define WM_DPICHANGED                   0x02E0

#define WM_DPICHANGED_BEFOREPARENT      0x02E2
#define WM_DPICHANGED_AFTERPARENT       0x02E3
#define WM_GETDPISCALEDSIZE             0x02E4

#define WM_CUT                          0x0300
#define WM_COPY                         0x0301
#define WM_PASTE                        0x0302
#define WM_CLEAR                        0x0303
#define WM_UNDO                         0x0304
#define WM_RENDERFORMAT                 0x0305
#define WM_RENDERALLFORMATS             0x0306
#define WM_DESTROYCLIPBOARD             0x0307
#define WM_DRAWCLIPBOARD                0x0308
#define WM_PAINTCLIPBOARD               0x0309
#define WM_VSCROLLCLIPBOARD             0x030A
#define WM_SIZECLIPBOARD                0x030B
#define WM_ASKCBFORMATNAME              0x030C
#define WM_CHANGECBCHAIN                0x030D
#define WM_HSCROLLCLIPBOARD             0x030E
#define WM_QUERYNEWPALETTE              0x030F
#define WM_PALETTEISCHANGING            0x0310
#define WM_PALETTECHANGED               0x0311
#define WM_HOTKEY                       0x0312

#define WM_PRINT                        0x0317
#define WM_PRINTCLIENT                  0x0318

#define WM_APPCOMMAND                   0x0319

#define WM_THEMECHANGED                 0x031A

#define WM_CLIPBOARDUPDATE              0x031D

#define WM_DWMCOMPOSITIONCHANGED        0x031E
#define WM_DWMNCRENDERINGCHANGED        0x031F
#define WM_DWMCOLORIZATIONCOLORCHANGED  0x0320
#define WM_DWMWINDOWMAXIMIZEDCHANGE     0x0321

#define WM_DWMSENDICONICTHUMBNAIL           0x0323
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP   0x0326

#define WM_GETTITLEBARINFOEX            0x033F

#define WM_HANDHELDFIRST                0x0358
#define WM_HANDHELDLAST                 0x035F

#define WM_AFXFIRST                     0x0360
#define WM_AFXLAST                      0x037F

#define WM_PENWINFIRST                  0x0380
#define WM_PENWINLAST                   0x038F

#define WM_APP                          0x8000

/*
 * NOTE: All Message Numbers below 0x0400 are RESERVED.
 *
 * Private Window Messages Start Here:
 */
#define WM_USER                         0x0400












/*
 * Edit Control Messages
 */
#define EM_GETSEL               0x00B0
#define EM_SETSEL               0x00B1
#define EM_GETRECT              0x00B2
#define EM_SETRECT              0x00B3
#define EM_SETRECTNP            0x00B4
#define EM_SCROLL               0x00B5
#define EM_LINESCROLL           0x00B6
#define EM_SCROLLCARET          0x00B7
#define EM_GETMODIFY            0x00B8
#define EM_SETMODIFY            0x00B9
#define EM_GETLINECOUNT         0x00BA
#define EM_LINEINDEX            0x00BB
#define EM_SETHANDLE            0x00BC
#define EM_GETHANDLE            0x00BD
#define EM_GETTHUMB             0x00BE
#define EM_LINELENGTH           0x00C1
#define EM_REPLACESEL           0x00C2
#define EM_GETLINE              0x00C4
#define EM_LIMITTEXT            0x00C5
#define EM_CANUNDO              0x00C6
#define EM_UNDO                 0x00C7
#define EM_FMTLINES             0x00C8
#define EM_LINEFROMCHAR         0x00C9
#define EM_SETTABSTOPS          0x00CB
#define EM_SETPASSWORDCHAR      0x00CC
#define EM_EMPTYUNDOBUFFER      0x00CD
#define EM_GETFIRSTVISIBLELINE  0x00CE
#define EM_SETREADONLY          0x00CF
#define EM_SETWORDBREAKPROC     0x00D0
#define EM_GETWORDBREAKPROC     0x00D1
#define EM_GETPASSWORDCHAR      0x00D2

#define EM_SETMARGINS           0x00D3
#define EM_GETMARGINS           0x00D4
#define EM_SETLIMITTEXT         EM_LIMITTEXT   /* ;win40 Name change */
#define EM_GETLIMITTEXT         0x00D5
#define EM_POSFROMCHAR          0x00D6
#define EM_CHARFROMPOS          0x00D7

#define EM_SETIMESTATUS         0x00D8
#define EM_GETIMESTATUS         0x00D9

#define EM_ENABLEFEATURE        0x00DA


/*
 * Button Control Messages
 */
#define BM_GETCHECK        0x00F0
#define BM_SETCHECK        0x00F1
#define BM_GETSTATE        0x00F2
#define BM_SETSTATE        0x00F3
#define BM_SETSTYLE        0x00F4

#define BM_CLICK           0x00F5
#define BM_GETIMAGE        0x00F6
#define BM_SETIMAGE        0x00F7

#define BM_SETDONTCLICK    0x00F8

/*
 * Static Control Mesages
 */
#define STM_SETICON         0x0170
#define STM_GETICON         0x0171

#define STM_SETIMAGE        0x0172
#define STM_GETIMAGE        0x0173

/*
 * Listbox messages
 */
#define LB_ADDSTRING            0x0180
#define LB_INSERTSTRING         0x0181
#define LB_DELETESTRING         0x0182
#define LB_SELITEMRANGEEX       0x0183
#define LB_RESETCONTENT         0x0184
#define LB_SETSEL               0x0185
#define LB_SETCURSEL            0x0186
#define LB_GETSEL               0x0187
#define LB_GETCURSEL            0x0188
#define LB_GETTEXT              0x0189
#define LB_GETTEXTLEN           0x018A
#define LB_GETCOUNT             0x018B
#define LB_SELECTSTRING         0x018C
#define LB_DIR                  0x018D
#define LB_GETTOPINDEX          0x018E
#define LB_FINDSTRING           0x018F
#define LB_GETSELCOUNT          0x0190
#define LB_GETSELITEMS          0x0191
#define LB_SETTABSTOPS          0x0192
#define LB_GETHORIZONTALEXTENT  0x0193
#define LB_SETHORIZONTALEXTENT  0x0194
#define LB_SETCOLUMNWIDTH       0x0195
#define LB_ADDFILE              0x0196
#define LB_SETTOPINDEX          0x0197
#define LB_GETITEMRECT          0x0198
#define LB_GETITEMDATA          0x0199
#define LB_SETITEMDATA          0x019A
#define LB_SELITEMRANGE         0x019B
#define LB_SETANCHORINDEX       0x019C
#define LB_GETANCHORINDEX       0x019D
#define LB_SETCARETINDEX        0x019E
#define LB_GETCARETINDEX        0x019F
#define LB_SETITEMHEIGHT        0x01A0
#define LB_GETITEMHEIGHT        0x01A1
#define LB_FINDSTRINGEXACT      0x01A2
#define LB_SETLOCALE            0x01A5
#define LB_GETLOCALE            0x01A6
#define LB_SETCOUNT             0x01A7

#define LB_INITSTORAGE          0x01A8
#define LB_ITEMFROMPOINT        0x01A9

#define LB_MULTIPLEADDSTRING    0x01B1

#define LB_GETLISTBOXINFO       0x01B2


/*
 * Combo Box messages
 */
#define CB_GETEDITSEL               0x0140
#define CB_LIMITTEXT                0x0141
#define CB_SETEDITSEL               0x0142
#define CB_ADDSTRING                0x0143
#define CB_DELETESTRING             0x0144
#define CB_DIR                      0x0145
#define CB_GETCOUNT                 0x0146
#define CB_GETCURSEL                0x0147
#define CB_GETLBTEXT                0x0148
#define CB_GETLBTEXTLEN             0x0149
#define CB_INSERTSTRING             0x014A
#define CB_RESETCONTENT             0x014B
#define CB_FINDSTRING               0x014C
#define CB_SELECTSTRING             0x014D
#define CB_SETCURSEL                0x014E
#define CB_SHOWDROPDOWN             0x014F
#define CB_GETITEMDATA              0x0150
#define CB_SETITEMDATA              0x0151
#define CB_GETDROPPEDCONTROLRECT    0x0152
#define CB_SETITEMHEIGHT            0x0153
#define CB_GETITEMHEIGHT            0x0154
#define CB_SETEXTENDEDUI            0x0155
#define CB_GETEXTENDEDUI            0x0156
#define CB_GETDROPPEDSTATE          0x0157
#define CB_FINDSTRINGEXACT          0x0158
#define CB_SETLOCALE                0x0159
#define CB_GETLOCALE                0x015A

#define CB_GETTOPINDEX              0x015b
#define CB_SETTOPINDEX              0x015c
#define CB_GETHORIZONTALEXTENT      0x015d
#define CB_SETHORIZONTALEXTENT      0x015e
#define CB_GETDROPPEDWIDTH          0x015f
#define CB_SETDROPPEDWIDTH          0x0160
#define CB_INITSTORAGE              0x0161

#define CB_MULTIPLEADDSTRING        0x0163

#define CB_GETCOMBOBOXINFO          0x0164

/*
 * Scroll bar messages
 */
#define SBM_SETPOS                  0x00E0 /*not in win3.1 */
#define SBM_GETPOS                  0x00E1 /*not in win3.1 */
#define SBM_SETRANGE                0x00E2 /*not in win3.1 */
#define SBM_SETRANGEREDRAW          0x00E6 /*not in win3.1 */
#define SBM_GETRANGE                0x00E3 /*not in win3.1 */
#define SBM_ENABLE_ARROWS           0x00E4 /*not in win3.1 */

#define SBM_SETSCROLLINFO           0x00E9
#define SBM_GETSCROLLINFO           0x00EA

#define SBM_GETSCROLLBARINFO        0x00EB
























// Interface for the Windows Common Controls

//====== Ranges for control message IDs =======================================

#define LVM_FIRST               0x1000      // ListView messages
#define TV_FIRST                0x1100      // TreeView messages
#define HDM_FIRST               0x1200      // Header messages
#define TCM_FIRST               0x1300      // Tab control messages

#define PGM_FIRST               0x1400      // Pager control messages

#define ECM_FIRST               0x1500      // Edit control messages
#define BCM_FIRST               0x1600      // Button control messages
#define CBM_FIRST               0x1700      // Combobox control messages

#define CCM_FIRST               0x2000      // Common control shared messages
#define CCM_LAST                (CCM_FIRST + 0x200)


#define CCM_SETBKCOLOR          (CCM_FIRST + 1) // lParam is bkColor
#define CCM_SETCOLORSCHEME      (CCM_FIRST + 2) // lParam is color scheme
#define CCM_GETCOLORSCHEME      (CCM_FIRST + 3) // fills in COLORSCHEME pointed to by lParam
#define CCM_GETDROPTARGET       (CCM_FIRST + 4)
#define CCM_SETUNICODEFORMAT    (CCM_FIRST + 5)
#define CCM_GETUNICODEFORMAT    (CCM_FIRST + 6)
#define CCM_SETVERSION          (CCM_FIRST + 0x7)
#define CCM_GETVERSION          (CCM_FIRST + 0x8)
#define CCM_SETNOTIFYWINDOW     (CCM_FIRST + 0x9) // wParam == hwndParent.

#define CCM_SETWINDOWTHEME      (CCM_FIRST + 0xb)
#define CCM_DPISCALE            (CCM_FIRST + 0xc) // wParam == Awareness

//====== HEADER CONTROL =======================================================

#define HDM_GETITEMCOUNT        (HDM_FIRST + 0)

#define HDM_INSERTITEMA         (HDM_FIRST + 1)
#define HDM_INSERTITEMW         (HDM_FIRST + 10)

#define HDM_DELETEITEM          (HDM_FIRST + 2)

#define HDM_GETITEMA            (HDM_FIRST + 3)
#define HDM_GETITEMW            (HDM_FIRST + 11)

#define HDM_SETITEMA            (HDM_FIRST + 4)
#define HDM_SETITEMW            (HDM_FIRST + 12)

#define HDM_LAYOUT              (HDM_FIRST + 5)

#define HDM_HITTEST             (HDM_FIRST + 6)

#define HDM_GETITEMRECT         (HDM_FIRST + 7)
#define HDM_SETIMAGELIST        (HDM_FIRST + 8)

#define HDM_GETIMAGELIST        (HDM_FIRST + 9)

#define HDM_ORDERTOINDEX        (HDM_FIRST + 15)

#define HDM_CREATEDRAGIMAGE     (HDM_FIRST + 16)

#define HDM_GETORDERARRAY       (HDM_FIRST + 17)

#define HDM_SETORDERARRAY       (HDM_FIRST + 18)

#define HDM_SETHOTDIVIDER          (HDM_FIRST + 19)

#define HDM_SETBITMAPMARGIN          (HDM_FIRST + 20)

#define HDM_GETBITMAPMARGIN          (HDM_FIRST + 21)

#define HDM_SETUNICODEFORMAT   CCM_SETUNICODEFORMAT
#define HDM_GETUNICODEFORMAT   CCM_GETUNICODEFORMAT

#define HDM_SETFILTERCHANGETIMEOUT  (HDM_FIRST+22)

#define HDM_EDITFILTER          (HDM_FIRST+23)

#define HDM_CLEARFILTER         (HDM_FIRST+24)

#define HDM_TRANSLATEACCELERATOR    CCM_TRANSLATEACCELERATOR

#define HDM_GETITEMDROPDOWNRECT (HDM_FIRST+25)  // rect of item's drop down button

#define HDM_GETOVERFLOWRECT (HDM_FIRST+26)  // rect of overflow button

#define HDM_GETFOCUSEDITEM (HDM_FIRST+27)

#define HDM_SETFOCUSEDITEM (HDM_FIRST+28)


//====== TOOLBAR CONTROL ======================================================

#define TB_ENABLEBUTTON         (WM_USER + 1)
#define TB_CHECKBUTTON          (WM_USER + 2)
#define TB_PRESSBUTTON          (WM_USER + 3)
#define TB_HIDEBUTTON           (WM_USER + 4)
#define TB_INDETERMINATE        (WM_USER + 5)
#define TB_MARKBUTTON           (WM_USER + 6)
#define TB_ISBUTTONENABLED      (WM_USER + 9)
#define TB_ISBUTTONCHECKED      (WM_USER + 10)
#define TB_ISBUTTONPRESSED      (WM_USER + 11)
#define TB_ISBUTTONHIDDEN       (WM_USER + 12)
#define TB_ISBUTTONINDETERMINATE (WM_USER + 13)
#define TB_ISBUTTONHIGHLIGHTED  (WM_USER + 14)
#define TB_SETSTATE             (WM_USER + 17)
#define TB_GETSTATE             (WM_USER + 18)
#define TB_ADDBITMAP            (WM_USER + 19)


#define TB_ADDBUTTONSA          (WM_USER + 20)
#define TB_INSERTBUTTONA        (WM_USER + 21)

#define TB_DELETEBUTTON         (WM_USER + 22)
#define TB_GETBUTTON            (WM_USER + 23)
#define TB_BUTTONCOUNT          (WM_USER + 24)
#define TB_COMMANDTOINDEX       (WM_USER + 25)

#define TB_SAVERESTOREA         (WM_USER + 26)
#define TB_SAVERESTOREW         (WM_USER + 76)
#define TB_CUSTOMIZE            (WM_USER + 27)
#define TB_ADDSTRINGA           (WM_USER + 28)
#define TB_ADDSTRINGW           (WM_USER + 77)
#define TB_GETITEMRECT          (WM_USER + 29)
#define TB_BUTTONSTRUCTSIZE     (WM_USER + 30)
#define TB_SETBUTTONSIZE        (WM_USER + 31)
#define TB_SETBITMAPSIZE        (WM_USER + 32)
#define TB_AUTOSIZE             (WM_USER + 33)
#define TB_GETTOOLTIPS          (WM_USER + 35)
#define TB_SETTOOLTIPS          (WM_USER + 36)
#define TB_SETPARENT            (WM_USER + 37)
#define TB_SETROWS              (WM_USER + 39)
#define TB_GETROWS              (WM_USER + 40)
#define TB_SETCMDID             (WM_USER + 42)
#define TB_CHANGEBITMAP         (WM_USER + 43)
#define TB_GETBITMAP            (WM_USER + 44)
#define TB_GETBUTTONTEXTA       (WM_USER + 45)
#define TB_GETBUTTONTEXTW       (WM_USER + 75)
#define TB_REPLACEBITMAP        (WM_USER + 46)
#define TB_SETINDENT            (WM_USER + 47)
#define TB_SETIMAGELIST         (WM_USER + 48)
#define TB_GETIMAGELIST         (WM_USER + 49)
#define TB_LOADIMAGES           (WM_USER + 50)
#define TB_GETRECT              (WM_USER + 51)
#define TB_SETHOTIMAGELIST      (WM_USER + 52)
#define TB_GETHOTIMAGELIST      (WM_USER + 53)
#define TB_SETDISABLEDIMAGELIST (WM_USER + 54)
#define TB_GETDISABLEDIMAGELIST (WM_USER + 55)
#define TB_SETSTYLE             (WM_USER + 56)
#define TB_GETSTYLE             (WM_USER + 57)
#define TB_GETBUTTONSIZE        (WM_USER + 58)
#define TB_SETBUTTONWIDTH       (WM_USER + 59)
#define TB_SETMAXTEXTROWS       (WM_USER + 60)
#define TB_GETTEXTROWS          (WM_USER + 61)

#define TB_GETOBJECT            (WM_USER + 62) 
#define TB_GETHOTITEM           (WM_USER + 71)
#define TB_SETHOTITEM           (WM_USER + 72) 
#define TB_SETANCHORHIGHLIGHT   (WM_USER + 73) 
#define TB_GETANCHORHIGHLIGHT   (WM_USER + 74)
#define TB_MAPACCELERATORA      (WM_USER + 78) 

#define TB_GETINSERTMARK        (WM_USER + 79)  
#define TB_SETINSERTMARK        (WM_USER + 80)  
#define TB_INSERTMARKHITTEST    (WM_USER + 81)  
#define TB_MOVEBUTTON           (WM_USER + 82)
#define TB_GETMAXSIZE           (WM_USER + 83)  
#define TB_SETEXTENDEDSTYLE     (WM_USER + 84)  
#define TB_GETEXTENDEDSTYLE     (WM_USER + 85)  
#define TB_GETPADDING           (WM_USER + 86)
#define TB_SETPADDING           (WM_USER + 87)
#define TB_SETINSERTMARKCOLOR   (WM_USER + 88)
#define TB_GETINSERTMARKCOLOR   (WM_USER + 89)

#define TB_SETCOLORSCHEME       CCM_SETCOLORSCHEME  
#define TB_GETCOLORSCHEME       CCM_GETCOLORSCHEME  

#define TB_SETUNICODEFORMAT     CCM_SETUNICODEFORMAT
#define TB_GETUNICODEFORMAT     CCM_GETUNICODEFORMAT

#define TB_MAPACCELERATORW      (WM_USER + 90)  

#define TB_GETBITMAPFLAGS       (WM_USER + 41)


#define TB_GETBUTTONINFOW        (WM_USER + 63)
#define TB_SETBUTTONINFOW        (WM_USER + 64)
#define TB_GETBUTTONINFOA        (WM_USER + 65)
#define TB_SETBUTTONINFOA        (WM_USER + 66)

#define TB_INSERTBUTTONW        (WM_USER + 67)
#define TB_ADDBUTTONSW          (WM_USER + 68)

#define TB_HITTEST              (WM_USER + 69)

#define TB_SETDRAWTEXTFLAGS     (WM_USER + 70) 

#define TB_GETSTRINGW           (WM_USER + 91)
#define TB_GETSTRINGA           (WM_USER + 92)

#define TB_SETBOUNDINGSIZE      (WM_USER + 93)
#define TB_SETHOTITEM2          (WM_USER + 94)  
#define TB_HASACCELERATOR       (WM_USER + 95)  
#define TB_SETLISTGAP           (WM_USER + 96)
#define TB_GETIMAGELISTCOUNT    (WM_USER + 98)
#define TB_GETIDEALSIZE         (WM_USER + 99)  

#define TB_TRANSLATEACCELERATOR     CCM_TRANSLATEACCELERATOR

#define TB_GETMETRICS           (WM_USER + 101)
#define TB_SETMETRICS           (WM_USER + 102)

#define TB_GETITEMDROPDOWNRECT  (WM_USER + 103)  // Rect of item's drop down button
#define TB_SETPRESSEDIMAGELIST  (WM_USER + 104)
#define TB_GETPRESSEDIMAGELIST  (WM_USER + 105)

#define TB_SETWINDOWTHEME       CCM_SETWINDOWTHEME


//====== REBAR CONTROL ========================================================

#define RB_INSERTBANDA  (WM_USER +  1)
#define RB_DELETEBAND   (WM_USER +  2)
#define RB_GETBARINFO   (WM_USER +  3)
#define RB_SETBARINFO   (WM_USER +  4)
#define RB_SETBANDINFOA (WM_USER +  6)
#define RB_SETPARENT    (WM_USER +  7)
#define RB_HITTEST      (WM_USER +  8)
#define RB_GETRECT      (WM_USER +  9)
#define RB_INSERTBANDW  (WM_USER +  10)
#define RB_SETBANDINFOW (WM_USER +  11)
#define RB_GETBANDCOUNT (WM_USER +  12)
#define RB_GETROWCOUNT  (WM_USER +  13)
#define RB_GETROWHEIGHT (WM_USER +  14)
#define RB_IDTOINDEX    (WM_USER +  16) // wParam == id
#define RB_GETTOOLTIPS  (WM_USER +  17)
#define RB_SETTOOLTIPS  (WM_USER +  18)
#define RB_SETBKCOLOR   (WM_USER +  19) // sets the default BK color
#define RB_GETBKCOLOR   (WM_USER +  20) // defaults to CLR_NONE
#define RB_SETTEXTCOLOR (WM_USER +  21)
#define RB_GETTEXTCOLOR (WM_USER +  22) // defaults to 0x00000000

#define RB_SIZETORECT   (WM_USER +  23) // resize the rebar/break bands and such to this rect (lparam)

#define RB_SETCOLORSCHEME   CCM_SETCOLORSCHEME  // lParam is color scheme
#define RB_GETCOLORSCHEME   CCM_GETCOLORSCHEME  // fills in COLORSCHEME pointed to by lParam

#define RB_BEGINDRAG    (WM_USER + 24)
#define RB_ENDDRAG      (WM_USER + 25)
#define RB_DRAGMOVE     (WM_USER + 26)
#define RB_GETBARHEIGHT (WM_USER + 27)
#define RB_GETBANDINFOW (WM_USER + 28)
#define RB_GETBANDINFOA (WM_USER + 29)

#define RB_MINIMIZEBAND (WM_USER + 30)
#define RB_MAXIMIZEBAND (WM_USER + 31)

#define RB_GETDROPTARGET (CCM_GETDROPTARGET)

#define RB_GETBANDBORDERS (WM_USER + 34)  // returns in lparam = lprc the amount of edges added to band wparam

#define RB_SHOWBAND     (WM_USER + 35)      // show/hide band
#define RB_SETPALETTE   (WM_USER + 37)
#define RB_GETPALETTE   (WM_USER + 38)
#define RB_MOVEBAND     (WM_USER + 39)

#define RB_SETUNICODEFORMAT     CCM_SETUNICODEFORMAT
#define RB_GETUNICODEFORMAT     CCM_GETUNICODEFORMAT

#define RB_GETBANDMARGINS   (WM_USER + 40)
#define RB_SETWINDOWTHEME       CCM_SETWINDOWTHEME

#define RB_SETEXTENDEDSTYLE (WM_USER + 41)
#define RB_GETEXTENDEDSTYLE (WM_USER + 42)

#define RB_PUSHCHEVRON      (WM_USER + 43)

#define RB_SETBANDWIDTH     (WM_USER + 44)   // set width for docked band

//====== TOOLTIPS CONTROL =====================================================

#define TTM_ACTIVATE            (WM_USER + 1)
#define TTM_SETDELAYTIME        (WM_USER + 3)
#define TTM_ADDTOOLA            (WM_USER + 4)
#define TTM_ADDTOOLW            (WM_USER + 50)
#define TTM_DELTOOLA            (WM_USER + 5)
#define TTM_DELTOOLW            (WM_USER + 51)
#define TTM_NEWTOOLRECTA        (WM_USER + 6)
#define TTM_NEWTOOLRECTW        (WM_USER + 52)
#define TTM_RELAYEVENT          (WM_USER + 7) // Win7: wParam = GetMessageExtraInfo() when relaying WM_MOUSEMOVE

#define TTM_GETTOOLINFOA        (WM_USER + 8)
#define TTM_GETTOOLINFOW        (WM_USER + 53)

#define TTM_SETTOOLINFOA        (WM_USER + 9)
#define TTM_SETTOOLINFOW        (WM_USER + 54)

#define TTM_HITTESTA            (WM_USER +10)
#define TTM_HITTESTW            (WM_USER +55)
#define TTM_GETTEXTA            (WM_USER +11)
#define TTM_GETTEXTW            (WM_USER +56)
#define TTM_UPDATETIPTEXTA      (WM_USER +12)
#define TTM_UPDATETIPTEXTW      (WM_USER +57)
#define TTM_GETTOOLCOUNT        (WM_USER +13)
#define TTM_ENUMTOOLSA          (WM_USER +14)
#define TTM_ENUMTOOLSW          (WM_USER +58)
#define TTM_GETCURRENTTOOLA     (WM_USER + 15)
#define TTM_GETCURRENTTOOLW     (WM_USER + 59)
#define TTM_WINDOWFROMPOINT     (WM_USER + 16)
#define TTM_TRACKACTIVATE       (WM_USER + 17)  // wParam = TRUE/FALSE start end  lparam = LPTOOLINFO
#define TTM_TRACKPOSITION       (WM_USER + 18)  // lParam = dwPos
#define TTM_SETTIPBKCOLOR       (WM_USER + 19)
#define TTM_SETTIPTEXTCOLOR     (WM_USER + 20)
#define TTM_GETDELAYTIME        (WM_USER + 21)
#define TTM_GETTIPBKCOLOR       (WM_USER + 22)
#define TTM_GETTIPTEXTCOLOR     (WM_USER + 23)
#define TTM_SETMAXTIPWIDTH      (WM_USER + 24)
#define TTM_GETMAXTIPWIDTH      (WM_USER + 25)
#define TTM_SETMARGIN           (WM_USER + 26)  // lParam = lprc
#define TTM_GETMARGIN           (WM_USER + 27)  // lParam = lprc
#define TTM_POP                 (WM_USER + 28)
#define TTM_UPDATE              (WM_USER + 29)
#define TTM_GETBUBBLESIZE       (WM_USER + 30)
#define TTM_ADJUSTRECT          (WM_USER + 31)
#define TTM_SETTITLEA           (WM_USER + 32)  // wParam = TTI_*, lParam = char* szTitle
#define TTM_SETTITLEW           (WM_USER + 33)  // wParam = TTI_*, lParam = wchar* szTitle

#define TTM_POPUP               (WM_USER + 34)
#define TTM_GETTITLE            (WM_USER + 35) // wParam = 0, lParam = TTGETTITLE*

#define TTM_SETWINDOWTHEME      CCM_SETWINDOWTHEME

//====== STATUS BAR CONTROL ===================================================

#define SB_SETTEXTA             (WM_USER+1)
#define SB_SETTEXTW             (WM_USER+11)
#define SB_GETTEXTA             (WM_USER+2)
#define SB_GETTEXTW             (WM_USER+13)
#define SB_GETTEXTLENGTHA       (WM_USER+3)
#define SB_GETTEXTLENGTHW       (WM_USER+12)

#define SB_SETPARTS             (WM_USER+4)
#define SB_GETPARTS             (WM_USER+6)
#define SB_GETBORDERS           (WM_USER+7)
#define SB_SETMINHEIGHT         (WM_USER+8)
#define SB_SIMPLE               (WM_USER+9)
#define SB_GETRECT              (WM_USER+10)
#define SB_ISSIMPLE             (WM_USER+14)
#define SB_SETICON              (WM_USER+15)
#define SB_SETTIPTEXTA          (WM_USER+16)
#define SB_SETTIPTEXTW          (WM_USER+17)
#define SB_GETTIPTEXTA          (WM_USER+18)
#define SB_GETTIPTEXTW          (WM_USER+19)
#define SB_GETICON              (WM_USER+20)

#define SB_SETUNICODEFORMAT     CCM_SETUNICODEFORMAT
#define SB_GETUNICODEFORMAT     CCM_GETUNICODEFORMAT

#define SB_SETBKCOLOR           CCM_SETBKCOLOR  

//====== TRACKBAR CONTROL =====================================================

#define TBM_GETPOS              (WM_USER)
#define TBM_GETRANGEMIN         (WM_USER+1)
#define TBM_GETRANGEMAX         (WM_USER+2)
#define TBM_GETTIC              (WM_USER+3)
#define TBM_SETTIC              (WM_USER+4)
#define TBM_SETPOS              (WM_USER+5)
#define TBM_SETRANGE            (WM_USER+6)
#define TBM_SETRANGEMIN         (WM_USER+7)
#define TBM_SETRANGEMAX         (WM_USER+8)
#define TBM_CLEARTICS           (WM_USER+9)
#define TBM_SETSEL              (WM_USER+10)
#define TBM_SETSELSTART         (WM_USER+11)
#define TBM_SETSELEND           (WM_USER+12)
#define TBM_GETPTICS            (WM_USER+14)
#define TBM_GETTICPOS           (WM_USER+15)
#define TBM_GETNUMTICS          (WM_USER+16)
#define TBM_GETSELSTART         (WM_USER+17)
#define TBM_GETSELEND           (WM_USER+18)
#define TBM_CLEARSEL            (WM_USER+19)
#define TBM_SETTICFREQ          (WM_USER+20)
#define TBM_SETPAGESIZE         (WM_USER+21)
#define TBM_GETPAGESIZE         (WM_USER+22)
#define TBM_SETLINESIZE         (WM_USER+23)
#define TBM_GETLINESIZE         (WM_USER+24)
#define TBM_GETTHUMBRECT        (WM_USER+25)
#define TBM_GETCHANNELRECT      (WM_USER+26)
#define TBM_SETTHUMBLENGTH      (WM_USER+27)
#define TBM_GETTHUMBLENGTH      (WM_USER+28)
#define TBM_SETTOOLTIPS         (WM_USER+29)
#define TBM_GETTOOLTIPS         (WM_USER+30)
#define TBM_SETTIPSIDE          (WM_USER+31)

#define TBM_SETBUDDY            (WM_USER+32) 
#define TBM_GETBUDDY            (WM_USER+33) 
#define TBM_SETPOSNOTIFY        (WM_USER+34)

#define TBM_SETUNICODEFORMAT    CCM_SETUNICODEFORMAT
#define TBM_GETUNICODEFORMAT    CCM_GETUNICODEFORMAT

//====== DRAG LIST CONTROL ====================================================

#define DL_BEGINDRAG            (WM_USER+133)
#define DL_DRAGGING             (WM_USER+134)
#define DL_DROPPED              (WM_USER+135)
#define DL_CANCELDRAG           (WM_USER+136)

//====== UPDOWN CONTROL =======================================================

#define UDM_SETRANGE            (WM_USER+101)
#define UDM_GETRANGE            (WM_USER+102)
#define UDM_SETPOS              (WM_USER+103)
#define UDM_GETPOS              (WM_USER+104)
#define UDM_SETBUDDY            (WM_USER+105)
#define UDM_GETBUDDY            (WM_USER+106)
#define UDM_SETACCEL            (WM_USER+107)
#define UDM_GETACCEL            (WM_USER+108)
#define UDM_SETBASE             (WM_USER+109)
#define UDM_GETBASE             (WM_USER+110)
#define UDM_SETRANGE32          (WM_USER+111)
#define UDM_GETRANGE32          (WM_USER+112) 

#define UDM_SETUNICODEFORMAT    CCM_SETUNICODEFORMAT
#define UDM_GETUNICODEFORMAT    CCM_GETUNICODEFORMAT

#define UDM_SETPOS32            (WM_USER+113)
#define UDM_GETPOS32            (WM_USER+114)

//====== PROGRESS CONTROL =====================================================

#define PBM_SETRANGE            (WM_USER+1)
#define PBM_SETPOS              (WM_USER+2)
#define PBM_DELTAPOS            (WM_USER+3)
#define PBM_SETSTEP             (WM_USER+4)
#define PBM_STEPIT              (WM_USER+5)
#define PBM_SETRANGE32          (WM_USER+6)  

#define PBM_GETRANGE            (WM_USER+7)  
#define PBM_GETPOS              (WM_USER+8)
#define PBM_SETBARCOLOR         (WM_USER+9)     

#define PBM_SETBKCOLOR          CCM_SETBKCOLOR  

#define PBM_SETMARQUEE          (WM_USER+10)

#define PBM_GETSTEP             (WM_USER+13)
#define PBM_GETBKCOLOR          (WM_USER+14)
#define PBM_GETBARCOLOR         (WM_USER+15)
#define PBM_SETSTATE            (WM_USER+16) 
#define PBM_GETSTATE            (WM_USER+17)

//====== HOTKEY CONTROL =======================================================

#define HKM_SETHOTKEY           (WM_USER+1)
#define HKM_GETHOTKEY           (WM_USER+2)
#define HKM_SETRULES            (WM_USER+3)

//====== SysLink control =========================================

//  LinkWindow messages
#define LM_HITTEST         (WM_USER+0x300)  
#define LM_GETIDEALHEIGHT  (WM_USER+0x301)  
#define LM_SETITEM         (WM_USER+0x302)  
#define LM_GETITEM         (WM_USER+0x303)  
#define LM_GETIDEALSIZE    (LM_GETIDEALHEIGHT) 

//====== LISTVIEW CONTROL =====================================================

#define LVM_SETUNICODEFORMAT     CCM_SETUNICODEFORMAT
#define LVM_GETUNICODEFORMAT     CCM_GETUNICODEFORMAT

#define LVM_GETBKCOLOR          (LVM_FIRST + 0)
#define LVM_SETBKCOLOR          (LVM_FIRST + 1)
#define LVM_GETIMAGELIST        (LVM_FIRST + 2)
#define LVM_SETIMAGELIST        (LVM_FIRST + 3)
#define LVM_GETITEMCOUNT        (LVM_FIRST + 4)

#define LVM_GETITEMA            (LVM_FIRST + 5)
#define LVM_GETITEMW            (LVM_FIRST + 75)

#define LVM_SETITEMA            (LVM_FIRST + 6)
#define LVM_SETITEMW            (LVM_FIRST + 76)

#define LVM_INSERTITEMA         (LVM_FIRST + 7)
#define LVM_INSERTITEMW         (LVM_FIRST + 77)

#define LVM_DELETEITEM          (LVM_FIRST + 8)

#define LVM_DELETEALLITEMS      (LVM_FIRST + 9)

#define LVM_GETCALLBACKMASK     (LVM_FIRST + 10)

#define LVM_SETCALLBACKMASK     (LVM_FIRST + 11)

#define LVM_GETNEXTITEM         (LVM_FIRST + 12)

#define LVM_FINDITEMA           (LVM_FIRST + 13)
#define LVM_FINDITEMW           (LVM_FIRST + 83)

#define LVM_GETITEMRECT         (LVM_FIRST + 14)

#define LVM_SETITEMPOSITION     (LVM_FIRST + 15)

#define LVM_GETITEMPOSITION     (LVM_FIRST + 16)

#define LVM_GETSTRINGWIDTHA     (LVM_FIRST + 17)
#define LVM_GETSTRINGWIDTHW     (LVM_FIRST + 87)

#define LVM_HITTEST             (LVM_FIRST + 18)

#define LVM_ENSUREVISIBLE       (LVM_FIRST + 19)

#define LVM_SCROLL              (LVM_FIRST + 20)

#define LVM_REDRAWITEMS         (LVM_FIRST + 21)

#define LVM_ARRANGE             (LVM_FIRST + 22)

#define LVM_EDITLABELA          (LVM_FIRST + 23)
#define LVM_EDITLABELW          (LVM_FIRST + 118)

#define LVM_GETEDITCONTROL      (LVM_FIRST + 24)

#define LVM_GETCOLUMNA          (LVM_FIRST + 25)
#define LVM_GETCOLUMNW          (LVM_FIRST + 95)

#define LVM_SETCOLUMNA          (LVM_FIRST + 26)
#define LVM_SETCOLUMNW          (LVM_FIRST + 96)

#define LVM_INSERTCOLUMNA       (LVM_FIRST + 27)
#define LVM_INSERTCOLUMNW       (LVM_FIRST + 97)

#define LVM_DELETECOLUMN        (LVM_FIRST + 28)

#define LVM_GETCOLUMNWIDTH      (LVM_FIRST + 29)

#define LVM_SETCOLUMNWIDTH          (LVM_FIRST + 30)

#define LVM_GETHEADER               (LVM_FIRST + 31)

#define LVM_CREATEDRAGIMAGE     (LVM_FIRST + 33)

#define LVM_GETVIEWRECT         (LVM_FIRST + 34)

#define LVM_GETTEXTCOLOR        (LVM_FIRST + 35)

#define LVM_SETTEXTCOLOR        (LVM_FIRST + 36)

#define LVM_GETTEXTBKCOLOR      (LVM_FIRST + 37)

#define LVM_SETTEXTBKCOLOR      (LVM_FIRST + 38)

#define LVM_GETTOPINDEX         (LVM_FIRST + 39)

#define LVM_GETCOUNTPERPAGE     (LVM_FIRST + 40)

#define LVM_GETORIGIN           (LVM_FIRST + 41)

#define LVM_UPDATE              (LVM_FIRST + 42)

#define LVM_SETITEMSTATE        (LVM_FIRST + 43)

#define LVM_GETITEMSTATE        (LVM_FIRST + 44)

#define LVM_GETITEMTEXTA        (LVM_FIRST + 45)
#define LVM_GETITEMTEXTW        (LVM_FIRST + 115)

#define LVM_SETITEMTEXTA        (LVM_FIRST + 46)
#define LVM_SETITEMTEXTW        (LVM_FIRST + 116)

#define LVM_SETITEMCOUNT        (LVM_FIRST + 47)

#define LVM_SORTITEMS           (LVM_FIRST + 48)

#define LVM_SETITEMPOSITION32   (LVM_FIRST + 49)

#define LVM_GETSELECTEDCOUNT    (LVM_FIRST + 50)

#define LVM_GETITEMSPACING      (LVM_FIRST + 51)

#define LVM_GETISEARCHSTRINGA   (LVM_FIRST + 52)
#define LVM_GETISEARCHSTRINGW   (LVM_FIRST + 117)

#define LVM_SETICONSPACING      (LVM_FIRST + 53)

#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 54)   

#define LVM_GETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 55)

#define LVM_GETSUBITEMRECT      (LVM_FIRST + 56)

#define LVM_SUBITEMHITTEST      (LVM_FIRST + 57)

#define LVM_SETCOLUMNORDERARRAY (LVM_FIRST + 58)

#define LVM_GETCOLUMNORDERARRAY (LVM_FIRST + 59)

#define LVM_SETHOTITEM  (LVM_FIRST + 60)

#define LVM_GETHOTITEM  (LVM_FIRST + 61)

#define LVM_SETHOTCURSOR  (LVM_FIRST + 62)

#define LVM_GETHOTCURSOR  (LVM_FIRST + 63)

#define LVM_APPROXIMATEVIEWRECT (LVM_FIRST + 64)

#define LVM_SETWORKAREAS         (LVM_FIRST + 65)

#define LVM_GETWORKAREAS        (LVM_FIRST + 70)

#define LVM_GETNUMBEROFWORKAREAS  (LVM_FIRST + 73)

#define LVM_GETSELECTIONMARK    (LVM_FIRST + 66)

#define LVM_SETSELECTIONMARK    (LVM_FIRST + 67)

#define LVM_SETHOVERTIME        (LVM_FIRST + 71)

#define LVM_GETHOVERTIME        (LVM_FIRST + 72)

#define LVM_SETTOOLTIPS       (LVM_FIRST + 74)

#define LVM_GETTOOLTIPS       (LVM_FIRST + 78)

#define LVM_SORTITEMSEX          (LVM_FIRST + 81)

#define LVM_SETBKIMAGEA         (LVM_FIRST + 68)
#define LVM_SETBKIMAGEW         (LVM_FIRST + 138)
#define LVM_GETBKIMAGEA         (LVM_FIRST + 69)
#define LVM_GETBKIMAGEW         (LVM_FIRST + 139)

#define LVM_SETSELECTEDCOLUMN         (LVM_FIRST + 140)

#define LVM_SETVIEW         (LVM_FIRST + 142)

#define LVM_GETVIEW         (LVM_FIRST + 143)
#define LVM_INSERTGROUP         (LVM_FIRST + 145)

#define LVM_SETGROUPINFO         (LVM_FIRST + 147)

#define LVM_GETGROUPINFO         (LVM_FIRST + 149)

#define LVM_REMOVEGROUP         (LVM_FIRST + 150)

#define LVM_MOVEGROUP         (LVM_FIRST + 151)

#define LVM_GETGROUPCOUNT         (LVM_FIRST + 152)

#define LVM_GETGROUPINFOBYINDEX         (LVM_FIRST + 153)

#define LVM_MOVEITEMTOGROUP            (LVM_FIRST + 154)

#define LVM_GETGROUPRECT               (LVM_FIRST + 98)

#define LVM_SETGROUPMETRICS         (LVM_FIRST + 155)

#define LVM_GETGROUPMETRICS         (LVM_FIRST + 156)

#define LVM_ENABLEGROUPVIEW         (LVM_FIRST + 157)

#define LVM_SORTGROUPS         (LVM_FIRST + 158)

#define LVM_INSERTGROUPSORTED           (LVM_FIRST + 159)

#define LVM_REMOVEALLGROUPS             (LVM_FIRST + 160)

#define LVM_HASGROUP                    (LVM_FIRST + 161)

#define LVM_GETGROUPSTATE               (LVM_FIRST + 92)

#define LVM_GETFOCUSEDGROUP             (LVM_FIRST + 93)

#define LVM_SETTILEVIEWINFO                 (LVM_FIRST + 162)

#define LVM_GETTILEVIEWINFO                 (LVM_FIRST + 163)

#define LVM_SETTILEINFO                     (LVM_FIRST + 164)

#define LVM_GETTILEINFO                     (LVM_FIRST + 165)

#define LVM_SETINSERTMARK                   (LVM_FIRST + 166)

#define LVM_GETINSERTMARK                   (LVM_FIRST + 167)

#define LVM_INSERTMARKHITTEST               (LVM_FIRST + 168)

#define LVM_GETINSERTMARKRECT               (LVM_FIRST + 169)

#define LVM_SETINSERTMARKCOLOR                 (LVM_FIRST + 170)

#define LVM_GETINSERTMARKCOLOR                 (LVM_FIRST + 171)

#define  LVM_SETINFOTIP         (LVM_FIRST + 173)

#define LVM_GETSELECTEDCOLUMN   (LVM_FIRST + 174)

#define LVM_ISGROUPVIEWENABLED  (LVM_FIRST + 175)

#define LVM_GETOUTLINECOLOR     (LVM_FIRST + 176)

#define LVM_SETOUTLINECOLOR     (LVM_FIRST + 177)

#define LVM_CANCELEDITLABEL     (LVM_FIRST + 179)

#define LVM_MAPINDEXTOID     (LVM_FIRST + 180)

#define LVM_MAPIDTOINDEX     (LVM_FIRST + 181)

#define LVM_ISITEMVISIBLE    (LVM_FIRST + 182)

#define LVM_GETEMPTYTEXT (LVM_FIRST + 204)

#define LVM_GETFOOTERRECT (LVM_FIRST + 205)

#define LVM_GETFOOTERINFO (LVM_FIRST + 206)

#define LVM_GETFOOTERITEMRECT (LVM_FIRST + 207)

#define LVM_GETFOOTERITEM (LVM_FIRST + 208)

#define LVM_GETITEMINDEXRECT    (LVM_FIRST + 209)

#define LVM_SETITEMINDEXSTATE   (LVM_FIRST + 210)

#define LVM_GETNEXTITEMINDEX    (LVM_FIRST + 211)

//====== TREEVIEW CONTROL =====================================================

#define TVM_INSERTITEMA         (TV_FIRST + 0)
#define TVM_INSERTITEMW         (TV_FIRST + 50)

#define TVM_DELETEITEM          (TV_FIRST + 1)

#define TVM_EXPAND              (TV_FIRST + 2)

#define TVM_GETITEMRECT         (TV_FIRST + 4)

#define TVM_GETCOUNT            (TV_FIRST + 5)

#define TVM_GETINDENT           (TV_FIRST + 6)

#define TVM_SETINDENT           (TV_FIRST + 7)

#define TVM_GETIMAGELIST        (TV_FIRST + 8)

#define TVM_SETIMAGELIST        (TV_FIRST + 9)

#define TVM_GETNEXTITEM         (TV_FIRST + 10)

#define TVM_SELECTITEM          (TV_FIRST + 11)

#define TVM_GETITEMA            (TV_FIRST + 12)
#define TVM_GETITEMW            (TV_FIRST + 62)

#define TVM_SETITEMA            (TV_FIRST + 13)
#define TVM_SETITEMW            (TV_FIRST + 63)

#define TVM_EDITLABELA          (TV_FIRST + 14)
#define TVM_EDITLABELW          (TV_FIRST + 65)

#define TVM_GETEDITCONTROL      (TV_FIRST + 15)

#define TVM_GETVISIBLECOUNT     (TV_FIRST + 16)

#define TVM_HITTEST             (TV_FIRST + 17)

#define TVM_CREATEDRAGIMAGE     (TV_FIRST + 18)

#define TVM_SORTCHILDREN        (TV_FIRST + 19)

#define TVM_ENSUREVISIBLE       (TV_FIRST + 20)

#define TVM_SORTCHILDRENCB      (TV_FIRST + 21)

#define TVM_ENDEDITLABELNOW     (TV_FIRST + 22)

#define TVM_GETISEARCHSTRINGA   (TV_FIRST + 23)
#define TVM_GETISEARCHSTRINGW   (TV_FIRST + 64)

#define TVM_SETTOOLTIPS         (TV_FIRST + 24)

#define TVM_GETTOOLTIPS         (TV_FIRST + 25)

#define TVM_SETINSERTMARK       (TV_FIRST + 26)

#define TVM_SETUNICODEFORMAT     CCM_SETUNICODEFORMAT

#define TVM_GETUNICODEFORMAT     CCM_GETUNICODEFORMAT

#define TVM_SETITEMHEIGHT         (TV_FIRST + 27)

#define TVM_GETITEMHEIGHT         (TV_FIRST + 28)

#define TVM_SETBKCOLOR              (TV_FIRST + 29)

#define TVM_SETTEXTCOLOR              (TV_FIRST + 30)

#define TVM_GETBKCOLOR              (TV_FIRST + 31)

#define TVM_GETTEXTCOLOR              (TV_FIRST + 32)

#define TVM_SETSCROLLTIME              (TV_FIRST + 33)

#define TVM_GETSCROLLTIME              (TV_FIRST + 34)

#define TVM_SETINSERTMARKCOLOR              (TV_FIRST + 37)

#define TVM_GETINSERTMARKCOLOR              (TV_FIRST + 38)

#define TVM_SETBORDER         (TV_FIRST + 35)

#define TVM_GETITEMSTATE        (TV_FIRST + 39)

#define TVM_SETLINECOLOR            (TV_FIRST + 40)

#define TVM_GETLINECOLOR            (TV_FIRST + 41)

#define TVM_MAPACCIDTOHTREEITEM     (TV_FIRST + 42)

#define TVM_MAPHTREEITEMTOACCID     (TV_FIRST + 43)

#define TVM_SETEXTENDEDSTYLE      (TV_FIRST + 44)

#define TVM_GETEXTENDEDSTYLE      (TV_FIRST + 45)

#define TVM_SETAUTOSCROLLINFO   (TV_FIRST + 59)

#define TVM_SETHOT                 (TV_FIRST + 58)

#define TVM_GETSELECTEDCOUNT       (TV_FIRST + 70)

#define TVM_SHOWINFOTIP            (TV_FIRST + 71)

#define TVM_GETITEMPARTRECT         (TV_FIRST + 72)

////////////////////  ComboBoxEx ////////////////////////////////


#define CBEM_INSERTITEMA        (WM_USER + 1)
#define CBEM_SETIMAGELIST       (WM_USER + 2)
#define CBEM_GETIMAGELIST       (WM_USER + 3)
#define CBEM_GETITEMA           (WM_USER + 4)
#define CBEM_SETITEMA           (WM_USER + 5)

#define CBEM_DELETEITEM         CB_DELETESTRING

#define CBEM_GETCOMBOCONTROL    (WM_USER + 6)
#define CBEM_GETEDITCONTROL     (WM_USER + 7)
#define CBEM_SETEXSTYLE         (WM_USER + 8)  
#define CBEM_SETEXTENDEDSTYLE   (WM_USER + 14) 
#define CBEM_GETEXSTYLE         (WM_USER + 9) 
#define CBEM_GETEXTENDEDSTYLE   (WM_USER + 9)

#define CBEM_SETUNICODEFORMAT   CCM_SETUNICODEFORMAT
#define CBEM_GETUNICODEFORMAT   CCM_GETUNICODEFORMAT

#define CBEM_HASEDITCHANGED     (WM_USER + 10)
#define CBEM_INSERTITEMW        (WM_USER + 11)
#define CBEM_SETITEMW           (WM_USER + 12)
#define CBEM_GETITEMW           (WM_USER + 13)

#define CBEM_SETWINDOWTHEME     CCM_SETWINDOWTHEME

//====== TAB CONTROL ==========================================================

#define TCM_GETIMAGELIST        (TCM_FIRST + 2)

#define TCM_SETIMAGELIST        (TCM_FIRST + 3)

#define TCM_GETITEMCOUNT        (TCM_FIRST + 4)

#define TCM_GETITEMA            (TCM_FIRST + 5)
#define TCM_GETITEMW            (TCM_FIRST + 60)

#define TCM_SETITEMA            (TCM_FIRST + 6)
#define TCM_SETITEMW            (TCM_FIRST + 61)

#define TCM_INSERTITEMA         (TCM_FIRST + 7)
#define TCM_INSERTITEMW         (TCM_FIRST + 62)

#define TCM_DELETEITEM          (TCM_FIRST + 8)

#define TCM_DELETEALLITEMS      (TCM_FIRST + 9)

#define TCM_GETITEMRECT         (TCM_FIRST + 10)

#define TCM_GETCURSEL           (TCM_FIRST + 11)

#define TCM_SETCURSEL           (TCM_FIRST + 12)

#define TCM_HITTEST             (TCM_FIRST + 13)

#define TCM_SETITEMEXTRA        (TCM_FIRST + 14)

#define TCM_ADJUSTRECT          (TCM_FIRST + 40)

#define TCM_SETITEMSIZE         (TCM_FIRST + 41)

#define TCM_REMOVEIMAGE         (TCM_FIRST + 42)

#define TCM_SETPADDING          (TCM_FIRST + 43)

#define TCM_GETROWCOUNT         (TCM_FIRST + 44)

#define TCM_GETTOOLTIPS         (TCM_FIRST + 45)

#define TCM_SETTOOLTIPS         (TCM_FIRST + 46)

#define TCM_GETCURFOCUS         (TCM_FIRST + 47)

#define TCM_SETCURFOCUS         (TCM_FIRST + 48)

#define TCM_SETMINTABWIDTH      (TCM_FIRST + 49)

#define TCM_DESELECTALL         (TCM_FIRST + 50)

#define TCM_HIGHLIGHTITEM       (TCM_FIRST + 51)

#define TCM_SETEXTENDEDSTYLE    (TCM_FIRST + 52)  

#define TCM_GETEXTENDEDSTYLE    (TCM_FIRST + 53)

#define TCM_SETUNICODEFORMAT     CCM_SETUNICODEFORMAT
#define TCM_GETUNICODEFORMAT     CCM_GETUNICODEFORMAT

//====== ANIMATE CONTROL ======================================================

#define ACM_OPENA               (WM_USER+100)
#define ACM_OPENW               (WM_USER+103)

#define ACM_PLAY                (WM_USER+101)
#define ACM_STOP                (WM_USER+102)
#define ACM_ISPLAYING           (WM_USER+104)

//====== MONTHCAL CONTROL ======================================================

#define MCM_FIRST           0x1000

#define MCM_GETCURSEL       (MCM_FIRST + 1)
#define MCM_SETCURSEL       (MCM_FIRST + 2)
#define MCM_GETMAXSELCOUNT  (MCM_FIRST + 3)
#define MCM_SETMAXSELCOUNT  (MCM_FIRST + 4)
#define MCM_GETSELRANGE     (MCM_FIRST + 5)
#define MCM_SETSELRANGE     (MCM_FIRST + 6)
#define MCM_GETMONTHRANGE   (MCM_FIRST + 7)
#define MCM_SETDAYSTATE     (MCM_FIRST + 8)
#define MCM_GETMINREQRECT   (MCM_FIRST + 9)
#define MCM_SETCOLOR            (MCM_FIRST + 10)
#define MCM_GETCOLOR            (MCM_FIRST + 11)
#define MCM_SETTODAY    (MCM_FIRST + 12)
#define MCM_GETTODAY    (MCM_FIRST + 13)
#define MCM_HITTEST          (MCM_FIRST + 14)
#define MCM_SETFIRSTDAYOFWEEK (MCM_FIRST + 15)
#define MCM_GETFIRSTDAYOFWEEK (MCM_FIRST + 16)
#define MCM_GETRANGE (MCM_FIRST + 17)
#define MCM_SETRANGE (MCM_FIRST + 18)
#define MCM_GETMONTHDELTA (MCM_FIRST + 19)
#define MCM_SETMONTHDELTA (MCM_FIRST + 20)
#define MCM_GETMAXTODAYWIDTH (MCM_FIRST + 21)

#define MCM_SETUNICODEFORMAT     CCM_SETUNICODEFORMAT

#define MCM_GETUNICODEFORMAT     CCM_GETUNICODEFORMAT

#define MCM_GETCURRENTVIEW (MCM_FIRST + 22)

#define MCM_GETCALENDARCOUNT (MCM_FIRST + 23)

#define MCM_GETCALENDARGRIDINFO (MCM_FIRST + 24)

#define MCM_GETCALID (MCM_FIRST + 27)
#define MCM_SETCALID (MCM_FIRST + 28)

#define MCM_SIZERECTTOMIN (MCM_FIRST + 29)

#define MCM_SETCALENDARBORDER (MCM_FIRST + 30)

#define MCM_GETCALENDARBORDER (MCM_FIRST + 31)

#define MCM_SETCURRENTVIEW (MCM_FIRST + 32)

//====== DATETIMEPICK CONTROL ==================================================

#define DTM_FIRST        0x1000

#define DTM_GETSYSTEMTIME   (DTM_FIRST + 1)

#define DTM_SETSYSTEMTIME   (DTM_FIRST + 2)

#define DTM_GETRANGE (DTM_FIRST + 3)

#define DTM_SETRANGE (DTM_FIRST + 4)

#define DTM_SETFORMATA (DTM_FIRST + 5)
#define DTM_SETFORMATW (DTM_FIRST + 50)

#define DTM_SETMCCOLOR    (DTM_FIRST + 6)

#define DTM_GETMCCOLOR    (DTM_FIRST + 7)

#define DTM_GETMONTHCAL   (DTM_FIRST + 8)

#define DTM_SETMCFONT     (DTM_FIRST + 9)

#define DTM_GETMCFONT     (DTM_FIRST + 10)

#define DTM_SETMCSTYLE    (DTM_FIRST + 11)

#define DTM_GETMCSTYLE    (DTM_FIRST + 12)

#define DTM_CLOSEMONTHCAL (DTM_FIRST + 13)

#define DTM_GETDATETIMEPICKERINFO (DTM_FIRST + 14)

#define DTM_GETIDEALSIZE (DTM_FIRST + 15)

///////////////////////////////////////////////
//    IP Address edit control

// Messages sent to IPAddress controls

#define IPM_CLEARADDRESS (WM_USER+100) 
#define IPM_SETADDRESS   (WM_USER+101) 
#define IPM_GETADDRESS   (WM_USER+102) 
#define IPM_SETRANGE (WM_USER+103) 
#define IPM_SETFOCUS (WM_USER+104) 
#define IPM_ISBLANK  (WM_USER+105) 

//  ====================== Pager Control =============================

#define PGM_SETCHILD            (PGM_FIRST + 1)  // lParam == hwnd

#define PGM_RECALCSIZE          (PGM_FIRST + 2)

#define PGM_FORWARDMOUSE        (PGM_FIRST + 3)

#define PGM_SETBKCOLOR          (PGM_FIRST + 4)

#define PGM_GETBKCOLOR          (PGM_FIRST + 5)

#define PGM_SETBORDER          (PGM_FIRST + 6)

#define PGM_GETBORDER          (PGM_FIRST + 7)

#define PGM_SETPOS              (PGM_FIRST + 8)

#define PGM_GETPOS              (PGM_FIRST + 9)

#define PGM_SETBUTTONSIZE       (PGM_FIRST + 10)

#define PGM_GETBUTTONSIZE       (PGM_FIRST + 11)

#define PGM_GETBUTTONSTATE      (PGM_FIRST + 12)

#define PGM_GETDROPTARGET       CCM_GETDROPTARGET

#define PGM_SETSCROLLINFO      (PGM_FIRST + 13)

// ====================== Button Control =============================

#define BCM_GETIDEALSIZE        (BCM_FIRST + 0x0001)

#define BCM_SETIMAGELIST        (BCM_FIRST + 0x0002)

#define BCM_GETIMAGELIST        (BCM_FIRST + 0x0003)

#define BCM_SETTEXTMARGIN       (BCM_FIRST + 0x0004)

#define BCM_GETTEXTMARGIN       (BCM_FIRST + 0x0005)

#define BCM_SETDROPDOWNSTATE     (BCM_FIRST + 0x0006)

#define BCM_SETSPLITINFO         (BCM_FIRST + 0x0007)

#define BCM_GETSPLITINFO         (BCM_FIRST + 0x0008)

#define BCM_SETNOTE              (BCM_FIRST + 0x0009)

#define BCM_GETNOTE              (BCM_FIRST + 0x000A)

#define BCM_GETNOTELENGTH        (BCM_FIRST + 0x000B)

#define BCM_SETSHIELD            (BCM_FIRST + 0x000C)

// ====================== Edit Control =============================

#define EM_SETCUEBANNER     (ECM_FIRST + 1)     
#define EM_GETCUEBANNER     (ECM_FIRST + 2)     

#define EM_SHOWBALLOONTIP   (ECM_FIRST + 3)     
#define EM_HIDEBALLOONTIP   (ECM_FIRST + 4)     

#define EM_SETHILITE        (ECM_FIRST + 5)

#define EM_GETHILITE        (ECM_FIRST + 6)

#define EM_NOSETFOCUS       (ECM_FIRST + 7)

#define EM_TAKEFOCUS        (ECM_FIRST + 8)

#define EM_SETEXTENDEDSTYLE     (ECM_FIRST + 10)

#define EM_GETEXTENDEDSTYLE     (ECM_FIRST + 11)

#define EM_SETENDOFLINE         (ECM_FIRST + 12)

#define EM_GETENDOFLINE         (ECM_FIRST + 13)

#define EM_ENABLESEARCHWEB      (ECM_FIRST + 14)

#define EM_SEARCHWEB            (ECM_FIRST + 15)

#define EM_SETCARETINDEX        (ECM_FIRST + 17)

#define EM_GETCARETINDEX        (ECM_FIRST + 18)

// We want to reuse the same messages as richedit.h
// which is why these are outside of the ECM_FIRST-ECM_LAST range.
#define EM_GETZOOM              (WM_USER + 224)
#define EM_SETZOOM              (WM_USER + 225)

#define EM_FILELINEFROMCHAR     (ECM_FIRST + 19)
#define EM_FILELINEINDEX        (ECM_FIRST + 20)
#define EM_FILELINELENGTH       (ECM_FIRST + 21)
#define EM_GETFILELINE          (ECM_FIRST + 22)
#define EM_GETFILELINECOUNT        (ECM_FIRST + 23)

// ====================== Combobox Control =============================

#define CB_SETMINVISIBLE        (CBM_FIRST + 1)
#define CB_GETMINVISIBLE        (CBM_FIRST + 2)
#define CB_SETCUEBANNER         (CBM_FIRST + 3)
#define CB_GETCUEBANNER         (CBM_FIRST + 4)

// ===================== Task Dialog =========================

#define TDM_NAVIGATE_PAGE                   WM_USER+101
#define TDM_CLICK_BUTTON                    WM_USER+102 
#define TDM_SET_MARQUEE_PROGRESS_BAR        WM_USER+103 
#define TDM_SET_PROGRESS_BAR_STATE          WM_USER+104 
#define TDM_SET_PROGRESS_BAR_RANGE          WM_USER+105 
#define TDM_SET_PROGRESS_BAR_POS            WM_USER+106 
#define TDM_SET_PROGRESS_BAR_MARQUEE        WM_USER+107 
#define TDM_SET_ELEMENT_TEXT                WM_USER+108 
#define TDM_CLICK_RADIO_BUTTON              WM_USER+110 
#define TDM_ENABLE_BUTTON                   WM_USER+111 
#define TDM_ENABLE_RADIO_BUTTON             WM_USER+112 
#define TDM_CLICK_VERIFICATION              WM_USER+113 
#define TDM_UPDATE_ELEMENT_TEXT             WM_USER+114 
#define TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE WM_USER+115 
#define TDM_UPDATE_ICON                     WM_USER+116 




































// missing from the Windows header files (MSVC2019, latest SDK, etc. as per January 2022)
// but previously listed in the wxWidgets WM debugger function:

#define WM_SYSTEMERROR 0x0017

#define CCM_TRANSLATEACCELERATOR  0x200A





