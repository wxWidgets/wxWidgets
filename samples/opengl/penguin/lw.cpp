/*
 * Copyright (C) 1998 Janne Löf <jlof@mail.student.oulu.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef __WXMSW__
#include <windows.h>
#endif

#include "lw.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define wxInt32 int
#define wxUint32 unsigned int

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MK_ID(a,b,c,d) ((((wxUint32)(a))<<24)| \
			(((wxUint32)(b))<<16)| \
			(((wxUint32)(c))<< 8)| \
			(((wxUint32)(d))    ))

#define ID_FORM MK_ID('F','O','R','M')
#define ID_LWOB MK_ID('L','W','O','B')
#define ID_PNTS MK_ID('P','N','T','S')
#define ID_SRFS MK_ID('S','R','F','S')
#define ID_SURF MK_ID('S','U','R','F')
#define ID_POLS MK_ID('P','O','L','S')
#define ID_COLR MK_ID('C','O','L','R')

static wxInt32 read_char(FILE *f)
{
  int c = fgetc(f);
  return c;
}

static wxInt32 read_short(FILE *f)
{
  return (read_char(f)<<8) | read_char(f);
}

static wxInt32 read_long(FILE *f)
{
  return (read_char(f)<<24) | (read_char(f)<<16) | (read_char(f)<<8) | read_char(f);
}

static GLfloat read_float(FILE *f)
{
  wxInt32 x = read_long(f);
  return *(GLfloat*)&x;
}

static int read_string(FILE *f, char *s)
{
  int c;
  int cnt = 0;
  do {
    c = read_char(f);
    if (cnt < LW_MAX_NAME_LEN)
      s[cnt] = c;
    else
      s[LW_MAX_NAME_LEN-1] = 0;
    cnt++;
  } while (c != 0);
  /* if length of string (including \0) is odd skip another byte */
  if (cnt%2) {
    read_char(f);
    cnt++;
  }
  return cnt;
}

static void read_srfs(FILE *f, int nbytes, lwObject *lwo)
{
  int guess_cnt = lwo->material_cnt;

  while (nbytes > 0) {
    lwMaterial *material;

    /* allocate more memory for materials if needed */
    if (guess_cnt <= lwo->material_cnt) {
      guess_cnt += guess_cnt/2 + 4;
      lwo->material = (lwMaterial*) realloc(lwo->material, sizeof(lwMaterial)*guess_cnt);
    }
    material = lwo->material + lwo->material_cnt++;

    /* read name */
    nbytes -= read_string(f,material->name);

    /* defaults */
    material->r = 0.7;
    material->g = 0.7;
    material->b = 0.7;
  }
  lwo->material = (lwMaterial*) realloc(lwo->material, sizeof(lwMaterial)*lwo->material_cnt);
}


static void read_surf(FILE *f, int nbytes, lwObject *lwo)
{
  int i;
  char name[LW_MAX_NAME_LEN];
  lwMaterial *material = NULL;

  /* read surface name */
  nbytes -= read_string(f,name);

  /* find material */
  for (i=0; i< lwo->material_cnt; i++) {
    if (strcmp(lwo->material[i].name,name) == 0) {
      material = &lwo->material[i];
      break;
    }
  }

  /* read values */
  while (nbytes > 0) {
    int id = read_long(f);
    int len = read_short(f);
    nbytes -= 6 + len + (len%2);

    switch (id) {
    case ID_COLR:
      material->r = read_char(f) / 255.0;
      material->g = read_char(f) / 255.0;
      material->b = read_char(f) / 255.0;
      read_char(f); /* dummy */
      break;
    default:
      fseek(f, len+(len%2), SEEK_CUR);
    }
  }
}


static void read_pols(FILE *f, int nbytes, lwObject *lwo)
{
  int guess_cnt = lwo->face_cnt;
  
  while (nbytes > 0) {
    lwFace *face;
    int i;

    /* allocate more memory for polygons if necessary */
    if (guess_cnt <= lwo->face_cnt) {
      guess_cnt += guess_cnt + 4;
      lwo->face = (lwFace*) realloc((void*) lwo->face, sizeof(lwFace)*guess_cnt);
    }
    face = lwo->face + lwo->face_cnt++;

    /* number of points in this face */
    face->index_cnt = read_short(f);
    nbytes -= 2;

    /* allocate space for points */
    face->index = (int*) calloc(sizeof(int)*face->index_cnt,1);
    
    /* read points in */
    for (i=0; i<face->index_cnt; i++) {
      face->index[i] = read_short(f);
      nbytes -= 2;
    }
    
    /* read surface material */
    face->material = read_short(f);
    nbytes -= 2;
    
    /* skip over detail  polygons */
    if (face->material < 0) {
      int det_cnt;
      face->material = -face->material;
      det_cnt = read_short(f);
      nbytes -= 2;
      while (det_cnt-- > 0) {
	int cnt = read_short(f);
	fseek(f, cnt*2+2, SEEK_CUR);
	nbytes -= cnt*2+2;
      }
    }
    face->material -= 1;
  }
  /* readjust to true size */
  lwo->face = (lwFace*) realloc(lwo->face, sizeof(lwFace)*lwo->face_cnt);
}



static void read_pnts(FILE *f, int nbytes, lwObject *lwo)
{
  int i;
  lwo->vertex_cnt = nbytes / 12;
  lwo->vertex = (float*) calloc(sizeof(GLfloat)*lwo->vertex_cnt*3, 1);
  for (i=0; i<lwo->vertex_cnt; i++) {
    lwo->vertex[i*3+0] = read_float(f);
    lwo->vertex[i*3+1] = read_float(f);
    lwo->vertex[i*3+2] = read_float(f);
  }
}






int lw_is_lwobject(const char *lw_file)
{
  FILE *f = fopen(lw_file, "rb");
  if (f) {
    wxInt32 form = read_long(f);
    wxInt32 nlen = read_long(f);
    wxInt32 lwob = read_long(f);
    fclose(f);
    if (form == ID_FORM && nlen != 0 && lwob == ID_LWOB)
      return TRUE;
  }
  return FALSE;
}


lwObject *lw_object_read(const char *lw_file)
{
  FILE *f = NULL;
  lwObject *lw_object = NULL;

  wxInt32 form_bytes = 0;
  wxInt32 read_bytes = 0;

  /* open file */
  f = fopen(lw_file, "rb");
  if (f == NULL) {
    return NULL;
  }

  /* check for headers */
  if (read_long(f) != ID_FORM) {
    fclose(f);
    return NULL;
  }
  form_bytes = read_long(f);
  read_bytes += 4;

  if (read_long(f) != ID_LWOB) {
    fclose(f);
    return NULL;
  }

  /* create new lwObject */
  lw_object = (lwObject*) calloc(sizeof(lwObject),1);

  /* read chunks */
  while (read_bytes < form_bytes) {
    wxInt32  id     = read_long(f);
    wxInt32  nbytes = read_long(f);
    read_bytes += 8 + nbytes + (nbytes%2);

    switch (id) {
    case ID_PNTS:
      read_pnts(f, nbytes, lw_object);
      break;
    case ID_POLS:
      read_pols(f, nbytes, lw_object);
      break;
    case ID_SRFS:
      read_srfs(f, nbytes, lw_object);
      break;
    case ID_SURF:
      read_surf(f, nbytes, lw_object);
      break;
    default:
      fseek(f, nbytes + (nbytes%2), SEEK_CUR);
    }
  }

  fclose(f);
  return lw_object;
}



void lw_object_free(lwObject *lw_object)
{
  if (lw_object->face) {
    int i;
    for (i=0; i<lw_object->face_cnt; i++)
      free(lw_object->face[i].index);
    free(lw_object->face);
  }
  free(lw_object->material);
  free(lw_object->vertex);
  free(lw_object);
}





#define PX(i) (lw_object->vertex[face->index[i]*3+0])
#define PY(i) (lw_object->vertex[face->index[i]*3+1])
#define PZ(i) (lw_object->vertex[face->index[i]*3+2])
void lw_object_show(const lwObject *lw_object)
{
  int i,j;
  int prev_index_cnt = -1;
  int prev_material  = -1;
  GLfloat prev_nx = 0;
  GLfloat prev_ny = 0;
  GLfloat prev_nz = 0;

  for (i=0; i<lw_object->face_cnt; i++) {
    GLfloat ax,ay,az,bx,by,bz,nx,ny,nz,r;
    const lwFace *face = lw_object->face+i;

    /* ignore faces with less than 3 points */
    if (face->index_cnt < 3)
      continue;

    /* calculate normal */
    ax = PX(1) - PX(0);
    ay = PY(1) - PY(0);
    az = PZ(1) - PZ(0);

    bx = PX(face->index_cnt-1) - PX(0);
    by = PY(face->index_cnt-1) - PY(0);
    bz = PZ(face->index_cnt-1) - PZ(0);

    nx = ay * bz - az * by;
    ny = az * bx - ax * bz;
    nz = ax * by - ay * bx;

    r = sqrt(nx*nx + ny*ny + nz*nz);
    if (r < 0.000001) /* avoid division by zero */
      continue;
    nx /= r;
    ny /= r;
    nz /= r;

    /* glBegin/glEnd */
    if (prev_index_cnt != face->index_cnt || prev_index_cnt > 4) {
      if (prev_index_cnt > 0) glEnd();
      prev_index_cnt = face->index_cnt;
      switch (face->index_cnt) {
      case 3:
	glBegin(GL_TRIANGLES);
	break;
      case 4:
	glBegin(GL_QUADS);
	break;
      default:
	glBegin(GL_POLYGON);
      }
    }

    /* update material if necessary */
    if (prev_material != face->material) {
      prev_material = face->material;
      glColor3f(lw_object->material[face->material].r,
		lw_object->material[face->material].g,
		lw_object->material[face->material].b);
    }

    /* update normal if necessary */
    if (nx != prev_nx || ny != prev_ny || nz != prev_nz) {
      prev_nx = nx;
      prev_ny = ny;
      prev_nz = nz;
      glNormal3f(nx,ny,nz);
    }

    /* draw polygon/triangle/quad */
    for (j=0; j<face->index_cnt; j++)
      glVertex3f(PX(j),PY(j),PZ(j));

  }

  /* if glBegin was called call glEnd */
  if (prev_index_cnt > 0)
    glEnd();
}


GLfloat lw_object_radius(const lwObject *lwo)
{
  int i;
  double max_radius = 0.0;

  for (i=0; i<lwo->vertex_cnt; i++) {
    GLfloat *v = &lwo->vertex[i*3];
    double r = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (r > max_radius)
      max_radius = r;
  }
  return sqrt(max_radius);
}

void lw_object_scale(lwObject *lwo, GLfloat scale)
{
  int i;

  for (i=0; i<lwo->vertex_cnt; i++) {
    lwo->vertex[i*3+0] *= scale;
    lwo->vertex[i*3+1] *= scale;
    lwo->vertex[i*3+2] *= scale;
  }
}


