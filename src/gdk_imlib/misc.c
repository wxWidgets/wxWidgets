#define _GNU_SOURCE
#include "../gdk_imlib/gdk_imlib.h"
#include "../gdk_imlib/gdk_imlib_private.h"

gint
gdk_imlib_get_render_type()
{
   if (id)
      return id->render_type;
   else
      return -1;
}

void
gdk_imlib_set_render_type(gint rend_type)
{
   if (id)
     {
	if (id->x.depth > 8)
	   id->render_type = rend_type;
	else
	  {
	     if ((rend_type == RT_PLAIN_TRUECOL) ||
		 (rend_type == RT_DITHER_TRUECOL))
		id->render_type = RT_DITHER_PALETTE_FAST;
	     else
		id->render_type = rend_type;
	  }
	return;
     }
   else
      return;
}

static void
gdk_imlib_set_fast_render(ImlibData * id, Display * disp)
{
   /* Turn off fastrender if there is an endianess diff between */
   /* client and Xserver */
   int                 byt, bit;

   byt = ImageByteOrder(id->x.disp);	/* LSBFirst | MSBFirst */
   bit = BitmapBitOrder(id->x.disp);	/* LSBFirst | MSBFirst */
   /* if little endian && server big */
   if (htonl(1) != 1 && byt == MSBFirst)
      id->fastrend = 0;
   /* if big endian && server little */
   if (htonl(1) == 1 && byt == LSBFirst)
      id->fastrend = 0;
}

static int
gdk_imlib_set_color_map(ImlibData * id, Display * disp)
{
   XSetWindowAttributes at;
   unsigned long       mask;
   int                 newcm = 0;

   at.border_pixel = 0;
   at.backing_store = NotUseful;
   at.background_pixel = 0;
   at.save_under = False;
   at.override_redirect = True;
   mask = CWOverrideRedirect | CWBackPixel | CWBorderPixel |
      CWBackingStore | CWSaveUnder;
   newcm = 0;
   if (id->x.visual != DefaultVisual(disp, id->x.screen))
     {
	Colormap            cm;

	cm = XCreateColormap(id->x.disp, id->x.root,
			     id->x.visual, AllocNone);
	if (cm)
	  {
	     mask |= CWColormap;
	     id->x.root_cmap = cm;
	     at.colormap = cm;
	     newcm = 1;
	  }
     }
   id->x.base_window = XCreateWindow(id->x.disp, id->x.root,
				     -100, -100, 10, 10, 0,
				     id->x.depth, InputOutput,
				     id->x.visual, mask, &at);
   id->x.gdk_win = gdk_window_foreign_new(id->x.base_window);
   if (newcm)
      id->x.gdk_cmap = gdk_colormap_new(gdk_window_get_visual
					(id->x.gdk_win), FALSE);
   else
      id->x.gdk_cmap = gdk_colormap_get_system();
   return newcm;
}

void
gdk_imlib_init()
{
   Display            *disp;
   XWindowAttributes   xwa;
   XVisualInfo         xvi, *xvir;
   char               *homedir;
   char                s[4096];
   char               *s1;
   char               *s2;
   FILE               *f;
   int                 override = 0;
   int                 dither = 0;
   int                 remap = 1;
   int                 num;
   int                 i, max, maxn;
   int                 clas;
   char               *palfile = NULL;
   int                 loadpal;
   int                 vis;
   int                 newcm;

   disp = (Display *) gdk_display;
   if (!disp)
     {
	fprintf(stderr, "gdk_imlib ERROR: gdk has not connected to the display\n");
	return;
     }
   vis = -1;
   loadpal = 0;
   if (id)
      return;
   id = (ImlibData *) malloc(sizeof(ImlibData));
   if (!id)
     {
	fprintf(stderr, "gdk_imlib ERROR: Cannot alloc RAM for Initial data struct\n");
	return;
     }
   id->palette = NULL;
   id->palette_orig = NULL;
   id->fast_rgb = NULL;
   id->fast_err = NULL;
   id->fast_erg = NULL;
   id->fast_erb = NULL;
   id->x.disp = disp;
   id->x.screen = DefaultScreen(disp);	/* the screen number */
   id->x.root = DefaultRootWindow(disp);	/* the root window id */
   id->x.visual = DefaultVisual(disp, id->x.screen);	/* the visual type */
   id->x.depth = DefaultDepth(disp, id->x.screen);	/* the depth of the screen in bpp */
   if (XShmQueryExtension(id->x.disp))
     {
	id->x.shm = 1;
	id->x.shm_event = XShmGetEventBase(id->x.disp) + ShmCompletion;
	id->x.last_xim = NULL;
	id->x.last_sxim = NULL;
	id->max_shm = 0x7fffffff;
	if (XShmPixmapFormat(id->x.disp) == ZPixmap)
	   id->x.shmp = 1;
     }
   else
     {
	id->x.shm = 0;
	id->x.shmp = 0;
     }
   id->cache.on_image = 0;
   id->cache.size_image = 0;
   id->cache.num_image = 0;
   id->cache.used_image = 0;
   id->cache.image = NULL;
   id->cache.on_pixmap = 0;
   id->cache.size_pixmap = 0;
   id->cache.num_pixmap = 0;
   id->cache.used_pixmap = 0;
   id->cache.pixmap = NULL;
   id->byte_order = 0;
   id->fastrend = 0;
   id->hiq = 0;
   id->fallback = 1;
   id->mod.gamma = 256;
   id->mod.brightness = 256;
   id->mod.contrast = 256;
   id->rmod.gamma = 256;
   id->rmod.brightness = 256;
   id->rmod.contrast = 256;
   id->gmod.gamma = 256;
   id->gmod.brightness = 256;
   id->gmod.contrast = 256;
   id->bmod.gamma = 256;
   id->bmod.brightness = 256;
   id->bmod.contrast = 256;
   id->ordered_dither = 1;

   if (XGetWindowAttributes(disp, id->x.root, &xwa))
     {
	if (xwa.colormap)
	   id->x.root_cmap = xwa.colormap;
	else
	   id->x.root_cmap = 0;
     }
   else
      id->x.root_cmap = 0;

   id->num_colors = 0;
   homedir = getenv("HOME");
   g_snprintf(s, sizeof(s), "%s/.imrc", homedir);
   f = fopen(s, "r");
   if (!f)
      f = fopen(SYSTEM_IMRC, "r");
   if (f)
     {
	while (fgets(s, 4096, f))
	  {
	     if (s[0] == '#')
		continue;

	     s1 = strtok(s, " \t\n");

	     /* Blank line ? */

	     if (s1 == NULL)
		continue;

	     s2 = strtok(NULL, " \t\n");
	     if (s2 == NULL)
		s2 = "";	/* NULL argument */

	     if (!strcasecmp("PaletteFile", s1))
	       {
		  palfile = strdup(s2);
	       }
	     else if (!strcasecmp("PaletteOverride", s1))
	       {
		  if (!strcasecmp("yes", s2))
		     override = 1;
		  else
		     override = 0;
	       }
	     else if (!strcasecmp("Dither", s1))
	       {
		  if (!strcasecmp("yes", s2))
		     dither = 1;
		  else
		     dither = 0;
	       }
	     else if (!strcasecmp("Remap", s1))
	       {
		  if (!strcasecmp("fast", s2))
		     remap = 1;
		  else
		     remap = 0;
	       }
	     else if (!strcasecmp("Mit-Shm", s1))
	       {
		  if (!strcasecmp("off", s2))
		    {
		       id->x.shm = 0;
		       id->x.shmp = 0;
		    }
	       }
	     else if (!strcasecmp("SharedPixmaps", s1))
	       {
		  if (!strcasecmp("off", s2))
		     id->x.shmp = 0;
	       }
	     else if (!strcasecmp("FastRender", s1))
	       {
		  if (!strcasecmp("on", s2))
		     id->fastrend = 1;
	       }
	     else if (!strcasecmp("HighQuality", s1))
	       {
		  if (!strcasecmp("on", s2))
		     id->hiq = 1;
	       }
	     else if (!strcasecmp("Shm_Max_Size", s1))
	       {
		  num = atoi(s2);
		  id->max_shm = num;
	       }
	     else if (!strcasecmp("Image_Cache_Size", s1))
	       {
		  num = atoi(s2);
		  id->cache.size_image = num;
	       }
	     else if (!strcasecmp("Pixmap_Cache_Size", s1))
	       {
		  num = atoi(s2);
		  id->cache.size_pixmap = num;
	       }
	     else if (!strcasecmp("Image_Cache", s1))
	       {
		  if (!strcasecmp("on", s2))
		     id->cache.on_image = 1;
	       }
	     else if (!strcasecmp("Pixmap_Cache", s1))
	       {
		  if (!strcasecmp("on", s2))
		     id->cache.on_pixmap = 1;
	       }
	     else if (!strcasecmp("ForceVisualID", s1))
	       {
		  sscanf(s, "%1024s %x", s1, &num);
		  vis = num;
	       }
	     else if (!strcasecmp("Fallback", s1))
	       {
		  if (!strcasecmp("off", s2))
		     id->fallback = 0;
		  else
		     id->fallback = 1;
	       }
	     else if (!strcasecmp("Gamma", s1))
	       {
		  id->mod.gamma = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Brightness", s1))
	       {
		  id->mod.brightness = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Contrast", s1))
	       {
		  id->mod.contrast = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Red_Gamma", s1))
	       {
		  id->rmod.gamma = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Red_Brightness", s1))
	       {
		  id->rmod.brightness = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Red_Contrast", s1))
	       {
		  id->rmod.contrast = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Green_Gamma", s1))
	       {
		  id->gmod.gamma = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Green_Brightness", s1))
	       {
		  id->gmod.brightness = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Green_Contrast", s1))
	       {
		  id->gmod.contrast = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Blue_Gamma", s1))
	       {
		  id->bmod.gamma = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Blue_Brightness", s1))
	       {
		  id->bmod.brightness = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Blue_Contrast", s1))
	       {
		  id->bmod.contrast = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Ordered_Dither", s1))
	       {
		  if (!strcasecmp("off", s2))
		     id->ordered_dither = 0;
		  else
		     id->ordered_dither = 1;
	       }
	  }
	fclose(f);
     }
   /* list all visuals for the default screen */
   xvi.screen = id->x.screen;
   xvir = XGetVisualInfo(disp, VisualScreenMask, &xvi, &num);
   if (vis >= 0)
     {
	/* use the forced visual id */
	maxn = 0;
	for (i = 0; i < num; i++)
	  {
	     if (xvir[i].visualid == (VisualID) vis)
		maxn = i;
	  }
	if (maxn >= 0)
	  {
	     unsigned long       rmsk, gmsk, bmsk;

	     id->x.depth = xvir[maxn].depth;
	     id->x.visual = xvir[maxn].visual;
	     rmsk = xvir[maxn].red_mask;
	     gmsk = xvir[maxn].green_mask;
	     bmsk = xvir[maxn].blue_mask;

	     if ((rmsk > gmsk) && (gmsk > bmsk))
		id->byte_order = BYTE_ORD_24_RGB;
	     else if ((rmsk > bmsk) && (bmsk > gmsk))
		id->byte_order = BYTE_ORD_24_RBG;
	     else if ((bmsk > rmsk) && (rmsk > gmsk))
		id->byte_order = BYTE_ORD_24_BRG;
	     else if ((bmsk > gmsk) && (gmsk > rmsk))
		id->byte_order = BYTE_ORD_24_BGR;
	     else if ((gmsk > rmsk) && (rmsk > bmsk))
		id->byte_order = BYTE_ORD_24_GRB;
	     else if ((gmsk > bmsk) && (bmsk > rmsk))
		id->byte_order = BYTE_ORD_24_GBR;
	     else
		id->byte_order = 0;
	  }
	else
	   fprintf(stderr, "Visual Id no 0x%x specified in the imrc file is invalid on this display.\nUsing Default Visual.\n", vis);
     }
   else
     {
	if (xvir)
	  {
	     /* find the highest bit-depth supported by visuals */
	     max = 0;
	     for (i = 0; i < num; i++)
	       {
		  if (xvir[i].depth > max)
		     max = xvir[i].depth;
	       }
	     if (max > 8)
	       {
		  id->x.depth = max;
		  clas = -1;
		  maxn = -1;
		  for (i = 0; i < num; i++)
		    {
		       if (xvir[i].depth == id->x.depth)
			 {
			    if ((xvir[i].class > clas) && (xvir[i].class != DirectColor))
			      {
				 maxn = i;
				 clas = xvir[i].class;
			      }
			 }
		    }
		  if (maxn >= 0)
		    {
		       unsigned long       rmsk, gmsk, bmsk;

		       id->x.visual = xvir[maxn].visual;
		       rmsk = xvir[maxn].red_mask;
		       gmsk = xvir[maxn].green_mask;
		       bmsk = xvir[maxn].blue_mask;

		       if ((rmsk > gmsk) && (gmsk > bmsk))
			  id->byte_order = BYTE_ORD_24_RGB;
		       else if ((rmsk > bmsk) && (bmsk > gmsk))
			  id->byte_order = BYTE_ORD_24_RBG;
		       else if ((bmsk > rmsk) && (rmsk > gmsk))
			  id->byte_order = BYTE_ORD_24_BRG;
		       else if ((bmsk > gmsk) && (gmsk > rmsk))
			  id->byte_order = BYTE_ORD_24_BGR;
		       else if ((gmsk > rmsk) && (rmsk > bmsk))
			  id->byte_order = BYTE_ORD_24_GRB;
		       else if ((gmsk > bmsk) && (bmsk > rmsk))
			  id->byte_order = BYTE_ORD_24_GBR;
		       else
			  id->byte_order = 0;
		    }
	       }
	  }
     }
   id->x.render_depth = id->x.depth;
   XFree(xvir);

   if (id->x.depth == 16)
     {
	xvi.visual = id->x.visual;
	xvi.visualid = XVisualIDFromVisual(id->x.visual);
	xvir = XGetVisualInfo(disp, VisualIDMask, &xvi, &num);
	if (xvir)
	  {
	     if (xvir->red_mask != 0xf800)
		id->x.render_depth = 15;
	     XFree(xvir);
	  }
     }
   if (id->x.depth <= 8 || override == 1)
      loadpal = 1;

   if (loadpal)
     {
	if (dither == 1)
	  {
	     if (remap == 1)
		id->render_type = RT_DITHER_PALETTE_FAST;
	     else
		id->render_type = RT_DITHER_PALETTE;
	  }
	else
	  {
	     if (remap == 1)
		id->render_type = RT_PLAIN_PALETTE_FAST;
	     else
		id->render_type = RT_PLAIN_PALETTE;
	  }
	/* Should we error this case or default it nicely */
	if (palfile != NULL)
	   gdk_imlib_load_colors(palfile);
	if (id->num_colors == 0)
	  {
	     fprintf(stderr, "gdk_imlib: Cannot Find Palette. A Palette is required for this mode\n");
	     free(id);
	     id = NULL;
	     if (palfile)
		free(palfile);
	     return;
	  }
     }
   else
     {
	if (id->hiq == 1)
	   id->render_type = RT_DITHER_TRUECOL;
	else
	   id->render_type = RT_PLAIN_TRUECOL;
     }

   newcm = gdk_imlib_set_color_map(id, disp);

   gdk_imlib_set_fast_render(id, disp);

   if (palfile)
      free(palfile);
}

void
gdk_imlib_init_params(GdkImlibInitParams * p)
{
   Display            *disp;
   XWindowAttributes   xwa;
   XVisualInfo         xvi, *xvir;
   char               *homedir;
   char                file[4096];
   char                s[4096], *s1, *s2;
   FILE               *f;
   int                 override = 0;
   int                 dither = 0;
   int                 remap = 1;
   int                 num;
   int                 i, max, maxn;
   int                 clas;
   char               *palfile;
   int                 loadpal;
   int                 vis;
   int                 newcm;

   disp = (Display *) gdk_display;
   if (!disp)
     {
	fprintf(stderr, "gdk_imlib ERROR: gdk has not connected to the display\n");
	return;
     }
   vis = -1;
   loadpal = 0;
   if (id)
      return;
   id = (ImlibData *) malloc(sizeof(ImlibData));
   if (!id)
     {
	fprintf(stderr, "gdk_imlib ERROR: Cannot alloc RAM for Initial data struct\n");
	return;
     }
   id->palette = NULL;
   id->palette_orig = NULL;
   id->fast_rgb = NULL;
   id->fast_err = NULL;
   id->fast_erg = NULL;
   id->fast_erb = NULL;
   id->x.disp = disp;
   id->x.screen = DefaultScreen(disp);	/* the screen number */
   id->x.root = DefaultRootWindow(disp);	/* the root window id */
   id->x.visual = DefaultVisual(disp, id->x.screen);	/* the visual type */
   id->x.depth = DefaultDepth(disp, id->x.screen);	/* the depth of the screen in bpp */
   if (XShmQueryExtension(id->x.disp))
     {
	id->x.shm = 1;
	id->x.shm_event = XShmGetEventBase(id->x.disp) + ShmCompletion;
	id->x.last_xim = NULL;
	id->x.last_sxim = NULL;
	id->max_shm = 0x7fffffff;
	if (XShmPixmapFormat(id->x.disp) == ZPixmap)
	   id->x.shmp = 1;
     }
   else
     {
	id->x.shm = 0;
	id->x.shmp = 0;
     }
   id->cache.on_image = 0;
   id->cache.size_image = 0;
   id->cache.num_image = 0;
   id->cache.used_image = 0;
   id->cache.image = NULL;
   id->cache.on_pixmap = 0;
   id->cache.size_pixmap = 0;
   id->cache.num_pixmap = 0;
   id->cache.used_pixmap = 0;
   id->cache.pixmap = NULL;
   id->byte_order = 0;
   id->fastrend = 0;
   id->hiq = 0;
   id->fallback = 1;
   id->mod.gamma = 256;
   id->mod.brightness = 256;
   id->mod.contrast = 256;
   id->rmod.gamma = 256;
   id->rmod.brightness = 256;
   id->rmod.contrast = 256;
   id->gmod.gamma = 256;
   id->gmod.brightness = 256;
   id->gmod.contrast = 256;
   id->bmod.gamma = 256;
   id->bmod.brightness = 256;
   id->bmod.contrast = 256;

   if (XGetWindowAttributes(disp, id->x.root, &xwa))
     {
	if (xwa.colormap)
	   id->x.root_cmap = xwa.colormap;
	else
	   id->x.root_cmap = 0;
     }
   else
      id->x.root_cmap = 0;
   id->num_colors = 0;
   homedir = getenv("HOME");
   g_snprintf(s, sizeof(s), "%s/.imrc", homedir);
   f = fopen(s, "r");
   if (!f)
      f = fopen(SYSTEM_IMRC, "r");
   if (f)
     {
	while (fgets(s, 4096, f))
	  {
	     if (s[0] == '#')
		continue;

	     s1 = strtok(s, " \t\n");

	     /* Blank line ? */

	     if (s1 == NULL)
		continue;

	     s2 = strtok(NULL, " \t\n");
	     if (s2 == NULL)
		s2 = "";	/* NULL argument */

	     if (!strcasecmp("PaletteFile", s1))
	       {
		  palfile = strdup(s2);
	       }
	     else if (!strcasecmp("PaletteOverride", s1))
	       {
		  if (!strcasecmp("yes", s2))
		     override = 1;
		  else
		     override = 0;
	       }
	     else if (!strcasecmp("Dither", s1))
	       {
		  if (!strcasecmp("yes", s2))
		     dither = 1;
		  else
		     dither = 0;
	       }
	     else if (!strcasecmp("Remap", s1))
	       {
		  if (!strcasecmp("fast", s2))
		     remap = 1;
		  else
		     remap = 0;
	       }
	     else if (!strcasecmp("Mit-Shm", s1))
	       {
		  if (!strcasecmp("off", s2))
		    {
		       id->x.shm = 0;
		       id->x.shmp = 0;
		    }
	       }
	     else if (!strcasecmp("SharedPixmaps", s1))
	       {
		  if (!strcasecmp("off", s2))
		     id->x.shmp = 0;
	       }
	     else if (!strcasecmp("FastRender", s1))
	       {
		  if (!strcasecmp("on", s2))
		     id->fastrend = 1;
	       }
	     else if (!strcasecmp("HighQuality", s1))
	       {
		  if (!strcasecmp("on", s2))
		     id->hiq = 1;
	       }
	     else if (!strcasecmp("Shm_Max_Size", s1))
	       {
		  num = atoi(s2);
		  id->max_shm = num;
	       }
	     if (!strcasecmp("Image_Cache_Size", s1))
	       {
		  num = atoi(s2);
		  id->cache.size_image = num;
	       }
	     else if (!strcasecmp("Pixmap_Cache_Size", s1))
	       {
		  num = atoi(s2);
		  id->cache.size_pixmap = num;
	       }
	     else if (!strcasecmp("Image_Cache", s1))
	       {
		  if (!strcasecmp("on", s2))
		     id->cache.on_image = 1;
	       }
	     else if (!strcasecmp("Pixmap_Cache", s1))
	       {
		  if (!strcasecmp("on", s2))
		     id->cache.on_pixmap = 1;
	       }
	     else if (!strcasecmp("ForceVisualID", s1))
	       {
		  sscanf(s, "%1024s %x", s1, &num);
		  vis = num;
	       }
	     else if (!strcasecmp("Fallback", s1))
	       {
		  if (!strcasecmp("off", s2))
		     id->fallback = 0;
		  else
		     id->fallback = 1;
	       }
	     else if (!strcasecmp("Gamma", s1))
	       {
		  id->mod.gamma = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Brightness", s1))
	       {
		  id->mod.brightness = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Contrast", s1))
	       {
		  id->mod.contrast = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Red_Gamma", s1))
	       {
		  id->rmod.gamma = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Red_Brightness", s1))
	       {
		  id->rmod.brightness = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Red_Contrast", s1))
	       {
		  id->rmod.contrast = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Green_Gamma", s1))
	       {
		  id->gmod.gamma = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Green_Brightness", s1))
	       {
		  id->gmod.brightness = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Green_Contrast", s1))
	       {
		  id->gmod.contrast = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Blue_Gamma", s1))
	       {
		  id->bmod.gamma = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Blue_Brightness", s1))
	       {
		  id->bmod.brightness = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Blue_Contrast", s1))
	       {
		  id->bmod.contrast = (int)(256.0 * atof(s2));
	       }
	     else if (!strcasecmp("Ordered_Dither", s1))
	       {
		  if (!strcasecmp("off", s2))
		     id->ordered_dither = 0;
		  else
		     id->ordered_dither = 1;
	       }
	  }
	fclose(f);
     }
   if (p)
     {
	if (p->flags & PARAMS_VISUALID)
	   vis = p->visualid;
	if (p->flags & PARAMS_PALETTEFILE)
	   palfile = strdup(p->palettefile);
	if (p->flags & PARAMS_SHAREDMEM)
	  {
	     if (!p->sharedmem)
	       {
		  id->x.shm = 0;
		  id->x.shmp = 0;
	       }
	     else
	       {
		  id->x.shm = 1;
		  id->x.shmp = 0;
	       }
	  }
	if (p->flags & PARAMS_SHAREDPIXMAPS)
	  {
	     if (id->x.shm)
		id->x.shmp = p->sharedpixmaps;
	  }
	if (p->flags & PARAMS_PALETTEOVERRIDE)
	   override = p->paletteoverride;
	if (p->flags & PARAMS_REMAP)
	   remap = p->remap;
	if (p->flags & PARAMS_FASTRENDER)
	   id->fastrend = p->fastrender;
	if (p->flags & PARAMS_HIQUALITY)
	   id->hiq = p->hiquality;
	if (p->flags & PARAMS_DITHER)
	   dither = p->dither;
	if (p->flags & PARAMS_IMAGECACHESIZE)
	   id->cache.size_image = p->imagecachesize;
	if (p->flags & PARAMS_PIXMAPCACHESIZE)
	   id->cache.size_pixmap = p->pixmapcachesize;
     }
   /* list all visuals for the default screen */
   xvi.screen = id->x.screen;
   xvir = XGetVisualInfo(disp, VisualScreenMask, &xvi, &num);
   if (vis >= 0)
     {
	/* use the forced visual id */
	maxn = 0;
	for (i = 0; i < num; i++)
	  {
	     if (xvir[i].visualid == (VisualID) vis)
		maxn = i;
	  }
	if (maxn >= 0)
	  {
	     unsigned long       rmsk, gmsk, bmsk;

	     id->x.depth = xvir[maxn].depth;
	     id->x.visual = xvir[maxn].visual;
	     rmsk = xvir[maxn].red_mask;
	     gmsk = xvir[maxn].green_mask;
	     bmsk = xvir[maxn].blue_mask;

	     if ((rmsk > gmsk) && (gmsk > bmsk))
		id->byte_order = BYTE_ORD_24_RGB;
	     else if ((rmsk > bmsk) && (bmsk > gmsk))
		id->byte_order = BYTE_ORD_24_RBG;
	     else if ((bmsk > rmsk) && (rmsk > gmsk))
		id->byte_order = BYTE_ORD_24_BRG;
	     else if ((bmsk > gmsk) && (gmsk > rmsk))
		id->byte_order = BYTE_ORD_24_BGR;
	     else if ((gmsk > rmsk) && (rmsk > bmsk))
		id->byte_order = BYTE_ORD_24_GRB;
	     else if ((gmsk > bmsk) && (bmsk > rmsk))
		id->byte_order = BYTE_ORD_24_GBR;
	     else
		id->byte_order = 0;
	  }
	else
	   fprintf(stderr, "Visual Id no 0x%x specified in the imrc file is invalid on this display.\nUsing Default Visual.\n", vis);
     }
   else
     {
	if (xvir)
	  {
	     /* find the highest bit-depth supported by visuals */
	     max = 0;
	     for (i = 0; i < num; i++)
	       {
		  if (xvir[i].depth > max)
		     max = xvir[i].depth;
	       }
	     if (max > 8)
	       {
		  id->x.depth = max;
		  clas = -1;
		  maxn = -1;
		  for (i = 0; i < num; i++)
		    {
		       if (xvir[i].depth == id->x.depth)
			 {
			    if ((xvir[i].class > clas) && (xvir[i].class != DirectColor))
			      {
				 maxn = i;
				 clas = xvir[i].class;
			      }
			 }
		    }
		  if (maxn >= 0)
		    {
		       unsigned long       rmsk, gmsk, bmsk;

		       id->x.visual = xvir[maxn].visual;
		       rmsk = xvir[maxn].red_mask;
		       gmsk = xvir[maxn].green_mask;
		       bmsk = xvir[maxn].blue_mask;

		       if ((rmsk > gmsk) && (gmsk > bmsk))
			  id->byte_order = BYTE_ORD_24_RGB;
		       else if ((rmsk > bmsk) && (bmsk > gmsk))
			  id->byte_order = BYTE_ORD_24_RBG;
		       else if ((bmsk > rmsk) && (rmsk > gmsk))
			  id->byte_order = BYTE_ORD_24_BRG;
		       else if ((bmsk > gmsk) && (gmsk > rmsk))
			  id->byte_order = BYTE_ORD_24_BGR;
		       else if ((gmsk > rmsk) && (rmsk > bmsk))
			  id->byte_order = BYTE_ORD_24_GRB;
		       else if ((gmsk > bmsk) && (bmsk > rmsk))
			  id->byte_order = BYTE_ORD_24_GBR;
		       else
			  id->byte_order = 0;
		    }
	       }
	  }
     }
   id->x.render_depth = id->x.depth;
   XFree(xvir);
   if (id->x.depth == 16)
     {
	xvi.visual = id->x.visual;
	xvi.visualid = XVisualIDFromVisual(id->x.visual);
	xvir = XGetVisualInfo(disp, VisualIDMask, &xvi, &num);
	if (xvir)
	  {
	     if (xvir->red_mask != 0xf800)
		id->x.render_depth = 15;
	     XFree(xvir);
	  }
     }
   if ((id->x.depth <= 8) || (override == 1))
      loadpal = 1;
   if (loadpal)
     {
	if (dither == 1)
	  {
	     if (remap == 1)
		id->render_type = RT_DITHER_PALETTE_FAST;
	     else
		id->render_type = RT_DITHER_PALETTE;
	  }
	else
	  {
	     if (remap == 1)
		id->render_type = RT_PLAIN_PALETTE_FAST;
	     else
		id->render_type = RT_PLAIN_PALETTE;
	  }
	gdk_imlib_load_colors(palfile);
	if (id->num_colors == 0)
	  {
	     fprintf(stderr, "gdk_imlib: Cannot Find Palette. A Palette is required for this mode\n");
	     free(id);
	     id = NULL;
	     if (palfile)
		free(palfile);
	     return;
	  }
     }
   else
     {
	if (id->hiq == 1)
	   id->render_type = RT_DITHER_TRUECOL;
	else
	   id->render_type = RT_PLAIN_TRUECOL;
     }

   newcm = gdk_imlib_set_color_map(id, disp);
   gdk_imlib_set_fast_render(id, disp);
   if (palfile)
      free(palfile);
}

GdkPixmap          *
gdk_imlib_copy_image(GdkImlibImage * im)
{
   GdkPixmap          *p;
   GdkGC              *gc;

   if ((!im) || (!im->pixmap))
      return NULL;
   p = gdk_pixmap_new(id->x.gdk_win, im->width, im->height, id->x.depth);
   gc = gdk_gc_new(p);
   gdk_draw_pixmap(p, gc, im->pixmap, 0, 0, 0, 0, im->width, im->height);
   gdk_gc_destroy(gc);
   return p;
}

GdkPixmap          *
gdk_imlib_move_image(GdkImlibImage * im)
{
   GdkPixmap          *p;

   if (!im)
      return NULL;
   p = im->pixmap;
   im->pixmap = NULL;
   return p;
}

GdkBitmap          *
gdk_imlib_copy_mask(GdkImlibImage * im)
{
   GdkBitmap          *p;
   GdkGC              *gc;

   if ((!im) || (!im->shape_mask))
      return NULL;
   p = gdk_pixmap_new(id->x.gdk_win, im->width, im->height, 1);
   gc = gdk_gc_new(p);
   gdk_draw_pixmap(p, gc, im->shape_mask, 0, 0, 0, 0, im->width, im->height);
   gdk_gc_destroy(gc);
   return p;
}

GdkBitmap          *
gdk_imlib_move_mask(GdkImlibImage * im)
{
   GdkBitmap          *p;

   if (!im)
      return NULL;
   p = im->shape_mask;
   im->shape_mask = NULL;
   return p;
}

void
gdk_imlib_destroy_image(GdkImlibImage * im)
{
   if (im)
     {
	if (id->cache.on_image)
	  {
	     gfree_image(im);
	     gclean_caches();
	  }
	else
	   gnullify_image(im);
     }
}

void
gdk_imlib_kill_image(GdkImlibImage * im)
{
   if (im)
     {
	if (id->cache.on_image)
	  {
	     gfree_image(im);
	     gflush_image(im);
	     gclean_caches();
	  }
	else
	   gnullify_image(im);
     }
}

void
gdk_imlib_free_pixmap(GdkPixmap * pmap)
{
   if (pmap)
     {
	gfree_pixmappmap(pmap);
	gclean_caches();
     }
}

void
gdk_imlib_free_bitmap(GdkBitmap * pmap)
{
   if (pmap)
     {
	gfree_pixmappmap(pmap);
	gclean_caches();
     }
}

void
gdk_imlib_set_image_border(GdkImlibImage * im, GdkImlibBorder * border)
{
   if ((im) && (border))
     {
	im->border.left = border->left;
	im->border.right = border->right;
	im->border.top = border->top;
	im->border.bottom = border->bottom;
	gdirty_pixmaps(im);
     }
}

void
gdk_imlib_get_image_border(GdkImlibImage * im, GdkImlibBorder * border)
{
   if ((im) && (border))
     {
	border->left = im->border.left;
	border->right = im->border.right;
	border->top = im->border.top;
	border->bottom = im->border.bottom;
     }
}

void
gdk_imlib_get_image_shape(GdkImlibImage * im, GdkImlibColor * color)
{
   if ((!im) || (!color))
      return;
   color->r = im->shape_color.r;
   color->g = im->shape_color.g;
   color->b = im->shape_color.b;
}

void
gdk_imlib_set_image_shape(GdkImlibImage * im, GdkImlibColor * color)
{
   if ((!im) || (!color))
      return;
   im->shape_color.r = color->r;
   im->shape_color.g = color->g;
   im->shape_color.b = color->b;
   gdirty_pixmaps(im);
}

gint
gdk_imlib_get_fallback()
{
   if (!id)
      return 0;
   return id->fallback;
}

void
gdk_imlib_set_fallback(gint fallback)
{
   if (!id)
      return;
   id->fallback = fallback;
}

GdkVisual          *
gdk_imlib_get_visual()
{
   if (!id)
      return NULL;
   return gdk_window_get_visual(id->x.gdk_win);
}

GdkColormap        *
gdk_imlib_get_colormap()
{
   if (!id)
      return NULL;
   return (id->x.gdk_cmap);
}

gchar              *
gdk_imlib_get_sysconfig()
{
   if (!id)
      return NULL;
   return strdup(SYSTEM_IMRC);
}
