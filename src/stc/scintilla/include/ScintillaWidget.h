// Scintilla source code edit control
// ScintillaWidget.h - definition of Scintilla widget for GTK+
// Only needed by GTK+ code but is harmless on other platforms.
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SCINTILLAWIDGET_H
#define SCINTILLAWIDGET_H

#if PLAT_GTK

#ifdef __cplusplus
extern "C" {
#endif

#define SCINTILLA(obj)          GTK_CHECK_CAST (obj, scintilla_get_type (), ScintillaObject)
#define SCINTILLA_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, scintilla_get_type (), ScintillaClass)
#define IS_SCINTILLA(obj)       GTK_CHECK_TYPE (obj, scintilla_get_type ())

typedef struct _ScintillaObject ScintillaObject;
typedef struct _ScintillaClass  ScintillaClass;

struct _ScintillaObject {
	GtkFixed vbox;
	void *pscin;
};

struct _ScintillaClass {
	GtkFixedClass parent_class;

	void (* command) (ScintillaObject *ttt);
	void (* notify) (ScintillaObject *ttt);
};

guint		scintilla_get_type	(void);
GtkWidget*	scintilla_new		(void);
void		scintilla_set_id	(ScintillaObject *sci,int id);
long 		scintilla_send_message	(ScintillaObject *sci,int iMessage,int wParam,int lParam);

#ifdef __cplusplus
}
#endif

#endif

#endif
