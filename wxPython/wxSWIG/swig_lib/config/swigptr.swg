/* swigptr.cfg
 * $Header$
 *
 * This file contains two functions :
 *
 *       void  _swig_make_hex(char *_c, void *_ptr, char *type)
 *       char *_swig_get_hex(char *_c, void **ptr, char *type)
 *
 * These are used to convert pointers to and from pointer strings
 * and to perform type checking.
 *
 * You can remap these functions by making a file called "swigptr.cfg" in
 * your the same directory as the interface file you are wrapping.
 *
 * IMPORTANT !!! the function _swig_get_hex returns a non-null char pointer
 * in the event of a type error (this is used to generate an error message).
 * If a type is successfully parsed, a NULL pointer is returned.
 *
 * $Log$
 * Revision 1.1  2002/04/29 19:56:50  RD
 * Since I have made several changes to SWIG over the years to accomodate
 * special cases and other things in wxPython, and since I plan on making
 * several more, I've decided to put the SWIG sources in wxPython's CVS
 * instead of relying on maintaining patches.  This effectivly becomes a
 * fork of an obsolete version of SWIG, :-( but since SWIG 1.3 still
 * doesn't have some things I rely on in 1.1, not to mention that my
 * custom patches would all have to be redone, I felt that this is the
 * easier road to take.
 *
 * Revision 1.1.1.1  1999/02/28 02:00:53  beazley
 * Swig1.1
 *
 * Revision 1.5  1996/08/01 16:28:56  dmb
 * Took out unused "dt" variable.
 *
 * Revision 1.4  1996/07/23 14:38:42  dmb
 * Minor change to handling of NULL pointers.
 *
 * Revision 1.3  1996/07/17 15:26:08  dmb
 * Made a minor bug fix so pointers of form _0_Type could be used
 * (as described in the manual).   Disable by compiling with -DNO_ZERO.
 *
 * Revision 1.2  1996/06/10 23:42:10  beazley
 * Added const qualifier.
 *
# Revision 1.1  1996/05/22  17:17:47  beazley
# Initial revision
#
 */

static void 
_swig_make_hex (char *_c, const void *_ptr, char *type)
{
  static char _hex[16] =
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
   'a', 'b', 'c', 'd', 'e', 'f'};
  unsigned long _p, _s;
  char _result[128], *_r;
  _r = _result;
  _p = (unsigned long) _ptr;
  if (_p > 0)
    {
      while (_p > 0)
	{
	  _s = _p & 0xf;
	  *(_r++) = _hex[_s];
	  _p = _p >> 4;
	}
      *_r = '_';
      while (_r >= _result)
	*(_c++) = *(_r--);
    }
  else
    {
      strcpy (_c, "NULL");
    }
  if (_ptr)
    strcpy (_c, type);
}

/* A forward reference; */

static char ***swig_ptr_derived = 0;

static char *
_swig_get_hex (char *_c, void **ptr, char *_t)
{
  unsigned long _p;
  char temp_type[256];
  char *_tt;
  char **eq;
  int i, j, n;
  _p = 0;
  if (*_c == '_')
    {
      _c++;
      while (*_c)
	{
	  if ((*_c >= '0') && (*_c <= '9'))
	    _p = (_p << 4) + (*_c - '0');
	  else if ((*_c >= 'a') && (*_c <= 'f'))
	    _p = (_p << 4) + ((*_c - 'a') + 10);
	  else
	    break;
	  _c++;
	}
#ifdef NO_ZERO
      if (_p == 0)
	{
	  return (char *) _c;
	}
#endif
      _tt = _c;
      if (_t)
	{
	  if (strcmp (_c, _t))
	    {
	      /* Have a type mismatch, we're going to have to do some
	         searching here */
	      i = 0;
	      if (swig_ptr_derived)
		{
		  while (swig_ptr_derived[i])
		    {
		      eq = swig_ptr_derived[i];
		      /* Check type */
		      if (strncmp (_t, eq[0], strlen (eq[0])) == 0)
			{
			  /* Found derived type list for this. */
			  n = strlen (eq[0]);
			  j = 1;
			  while (eq[j])
			    {
			      sprintf (temp_type, "%s%s", eq[j], _t + n);
			      if (strcmp (_c, temp_type) == 0)
				{
				  *ptr = (void *) _p;
				  return (char *) 0;
				}
			      j++;
			    }
			}
		      i++;
		    }
		}
	      *ptr = (void *) _p;
	      return _tt;
	    }
	  else
	    {
	      *ptr = (void *) _p;
	      return (char *) 0;
	    }
	}
      else
	{
	  *ptr = (void *) _p;
	  return (char *) 0;
	}
    }
  else {
#ifdef ALLOW_NULL
      if (strcmp (_c, "NULL") == 0)
	{
	  *ptr = (void *) 0;
	  return (char *) 0;
	}
#endif
      *ptr = (void *) 0;	
      return _c;
    }
}
