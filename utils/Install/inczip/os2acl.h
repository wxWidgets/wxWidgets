/* os2acl.h
 *
 * Author:  Kai Uwe Rommel <rommel@ars.de>
 * Created: Fri Mar 29 1996
 */
 
/* $Id$ */

/*
 * $Log$
 * Revision 1.1.2.1  2000/04/11 12:38:05  BS
 * Added wxInstall a self extracting installation program using wxWindows.
 *
 * Revision 1.1  1996/03/30 09:35:00  rommel
 * Initial revision
 * 
 */

#ifndef _OS2ACL_H
#define _OS2ACL_H

#define ACL_BUFFERSIZE 4096

int acl_get(char *server, const char *resource, char *buffer);
int acl_set(char *server, const char *resource, char *buffer);

#endif /* _OS2ACL_H */

/* end of os2acl.h */
