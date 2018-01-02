#define DRAW_C
#include "all.h"

enum {NO_COLOR, VERT_FRONT, VERT_BACK, FACE_FRONT, FACE_BACK, DEPTH_C, TEX_C };

void draw_sd_face(Face *face, int flag)
{
  int i;
  double midpoint[3] = {0}; 
  double curpoint[3];
  double midnorm[3] = {0};
  double curnorm[3];
  float midcolor[4] = {0};
  float curcolor[4];
  int mpdepth = 0;
  int curdepth;
  double midtex[3] = {0};
  double curtex[3];
  Vertex *vertex;
  int nvert = face->nvert;
  
  for(i=0; i<nvert; i++) {
    vertex = face->vertices[i];
    
    /* find the midpoint and normal */
    midpoint[0] += vertex->p3.d[0];
    midpoint[1] += vertex->p3.d[1];
    midpoint[2] += vertex->p3.d[2];
    midnorm[0] += vertex->normal.d[0];
    midnorm[1] += vertex->normal.d[1];
    midnorm[2] += vertex->normal.d[2];

    /* find the color (texture coord) at the midpoint */
    switch(flag) 
      {
      case VERT_FRONT:
	midcolor[0]+=vertex->cfront.f[0];
	midcolor[1]+=vertex->cfront.f[1];
	midcolor[2]+=vertex->cfront.f[2];
	midcolor[3]+=vertex->cfront.f[3];
	break;
      case VERT_BACK:
	midcolor[0]+=vertex->cback.f[0];
	midcolor[1]+=vertex->cback.f[1];
	midcolor[2]+=vertex->cback.f[2];
	midcolor[3]+=vertex->cback.f[3];
	break;
      case DEPTH_C:
	mpdepth+=vertex->depth;
	break;
      case TEX_C:
	midtex[0]+=vertex->tex.d[0];
	midtex[1]+=vertex->tex.d[1];
	midtex[2]+=vertex->tex.d[2];
	break;
      }
  }

  /* positive sceen door function */
  if(scr_door_mode == SD_PLUS) {

    /* calculate waited midpoint and normal*/
    midpoint[0] = (1-scrdoorgap) * midpoint[0] / nvert;
    midpoint[1] = (1-scrdoorgap) * midpoint[1] / nvert;
    midpoint[2] = (1-scrdoorgap) * midpoint[2] / nvert;
    midnorm[0] = (1-scrdoorgap) * midnorm[0] / nvert;
    midnorm[1] = (1-scrdoorgap) * midnorm[1] / nvert;
    midnorm[2] = (1-scrdoorgap) * midnorm[2] / nvert;
    
    /* calculate waited color (tex coord) at the midpoint */
    switch(flag) 
      {
      case VERT_FRONT:
      case  VERT_BACK:
	midcolor[0] = (1-scrdoorgap) * midcolor[0] / nvert;
	midcolor[1] = (1-scrdoorgap) * midcolor[1] / nvert;
	midcolor[2] = (1-scrdoorgap) * midcolor[2] / nvert;
	midcolor[3] = (1-scrdoorgap) * midcolor[3] / nvert;
	break;
      case DEPTH_C:
	mpdepth = (1-scrdoorgap) * mpdepth / nvert;
	break;
      case TEX_C:
	midtex[0] = (1-scrdoorgap) * midtex[0] / nvert;
	midtex[1] = (1-scrdoorgap) * midtex[1] / nvert;
	midtex[2] = (1-scrdoorgap) * midtex[2] / nvert;
	break;
      }

    glBegin(GL_POLYGON);
    for(i=0; i<nvert; i++) {
      vertex = face->vertices[i];
      
      /* find the new vertex position and normal */
      curpoint[0] = scrdoorgap * vertex->p3.d[0] + midpoint[0];
      curpoint[1] = scrdoorgap * vertex->p3.d[1] + midpoint[1];
      curpoint[2] = scrdoorgap * vertex->p3.d[2] + midpoint[2];
      curnorm[0] = scrdoorgap * vertex->normal.d[0] + midnorm[0];
      curnorm[1] = scrdoorgap * vertex->normal.d[1] + midnorm[1];
      curnorm[2] = scrdoorgap * vertex->normal.d[2] + midnorm[2];
      

      /* find and cpecify color of the new vertex */
      switch(flag)
	{
	case VERT_FRONT:
	  curcolor[0] = scrdoorgap * vertex->cfront.f[0] + midcolor[0];
	  curcolor[1] = scrdoorgap * vertex->cfront.f[1] + midcolor[1];
	  curcolor[2] = scrdoorgap * vertex->cfront.f[2] + midcolor[2];
	  curcolor[3] = scrdoorgap * vertex->cfront.f[3] + midcolor[3];
	  glColor4fv(curcolor);
	  break;
	case VERT_BACK:
	  curcolor[0] = scrdoorgap * vertex->cback.f[0] + midcolor[0];
	  curcolor[1] = scrdoorgap * vertex->cback.f[1] + midcolor[1];
	  curcolor[2] = scrdoorgap * vertex->cback.f[2] + midcolor[2];
	  curcolor[3] = scrdoorgap * vertex->cback.f[3] + midcolor[3];
	  glColor4fv(curcolor);
	  break;
	case DEPTH_C:
	  curdepth = scrdoorgap * vertex->depth + mpdepth;
	  glColor4fv(palette[curdepth]);
	  break;
	case TEX_C:
	  curtex[0] = scrdoorgap * vertex->tex.d[0] + midtex[0];
	  curtex[1] = scrdoorgap * vertex->tex.d[1] + midtex[1];
	  curtex[2] = scrdoorgap * vertex->tex.d[2] + midtex[2];
	  glTexCoord3dv(curtex);
	  break;
	}
      
      glNormal3dv(curnorm);
      glVertex3dv(curpoint);
    }
    glEnd();
    
  /* negative sceen door function */
  } else {  /*  SD_MINUS */

    /* calculate waited midpoint and normal */
    midpoint[0] = scrdoorgap * midpoint[0] / nvert;
    midpoint[1] = scrdoorgap * midpoint[1] / nvert;
    midpoint[2] = scrdoorgap * midpoint[2] / nvert;
    midnorm[0] = scrdoorgap * midnorm[0] / nvert;
    midnorm[1] = scrdoorgap * midnorm[1] / nvert;
    midnorm[2] = scrdoorgap * midnorm[2] / nvert;
        
    /* calculate waited color (tex coord) at the midpoint */
    switch(flag) 
      {
      case VERT_FRONT:
      case  VERT_BACK:
	midcolor[0] = scrdoorgap * midcolor[0] / nvert;
	midcolor[1] = scrdoorgap * midcolor[1] / nvert;
	midcolor[2] = scrdoorgap * midcolor[2] / nvert;
	midcolor[3] = scrdoorgap * midcolor[3] / nvert;
	break;
      case DEPTH_C:
	mpdepth = scrdoorgap * mpdepth / nvert;
	break;
      case TEX_C:
	midtex[0] = scrdoorgap * midtex[0] / nvert;
	midtex[1] = scrdoorgap * midtex[1] / nvert;
	midtex[2] = scrdoorgap * midtex[2] / nvert;
	break;
      } 

    glBegin(GL_QUAD_STRIP);
    for(i=0; i<= nvert; i++) {
      if(i == nvert)
	vertex = face->vertices[0];
      else
	vertex = face->vertices[i];

      /* find the new vertex position and normal */
      curpoint[0] = (1-scrdoorgap) * vertex->p3.d[0] + midpoint[0];
      curpoint[1] = (1-scrdoorgap) * vertex->p3.d[1] + midpoint[1];
      curpoint[2] = (1-scrdoorgap) * vertex->p3.d[2] + midpoint[2];
      curnorm[0] = (1-scrdoorgap) * vertex->normal.d[0] + midnorm[0];
      curnorm[1] = (1-scrdoorgap) * vertex->normal.d[1] + midnorm[1];
      curnorm[2] = (1-scrdoorgap) * vertex->normal.d[2] + midnorm[2];
      
      switch(flag)
	{
	case VERT_FRONT:
	  curcolor[0] = (1-scrdoorgap) * vertex->cfront.f[0] + midcolor[0];
	  curcolor[1] = (1-scrdoorgap) * vertex->cfront.f[1] + midcolor[1];
	  curcolor[2] = (1-scrdoorgap) * vertex->cfront.f[2] + midcolor[2];
	  curcolor[3] = (1-scrdoorgap) * vertex->cfront.f[3] + midcolor[3];
	  glColor4fv(curcolor);
	  break;
	case VERT_BACK:
	  curcolor[0] = (1-scrdoorgap) * vertex->cback.f[0] + midcolor[0];
	  curcolor[1] = (1-scrdoorgap) * vertex->cback.f[1] + midcolor[1];
	  curcolor[2] = (1-scrdoorgap) * vertex->cback.f[2] + midcolor[2];
	  curcolor[3] = (1-scrdoorgap) * vertex->cback.f[3] + midcolor[3];
	  glColor4fv(curcolor);
	  break;
	case DEPTH_C:
	  curdepth = (1-scrdoorgap) * vertex->depth + mpdepth;
	  glColor4fv(palette[curdepth]);
	  break;
	case TEX_C:
	  curtex[0] = (1-scrdoorgap) * vertex->tex.d[0] + midtex[0];
	  curtex[1] = (1-scrdoorgap) * vertex->tex.d[1] + midtex[1];
	  curtex[2] = (1-scrdoorgap) * vertex->tex.d[2] + midtex[2];
	  glTexCoord3dv(curtex);
	  break;
	}
      glNormal3dv(curnorm);
      glVertex3dv(curpoint);

      switch(flag) 
	{
	case VERT_FRONT:
	  glColor4fv(vertex->cfront.f);
	  break;
	case VERT_BACK:
	  glColor4fv(vertex->cback.f);
	  break;
	case DEPTH_C:
	  glColor4fv(palette[vertex->depth]);
	  break;
	case TEX_C:
	  glTexCoord3dv(vertex->tex.d);
	  break;
	}
      glNormal3dv(vertex->normal.d);
      glVertex3dv(vertex->p3.d);
    }
    glEnd();
  }
}

void drawMESH(Obj *obj, int flag)
{
  int i, j;
  int nu, nv, ni, nj;
  Vertex *vertex;

  if(scr_door_mode == SD_OFF) {
    nu = obj->nu;
    nv = obj->nv;
    ni = obj->u_flag == ON ? nu : nu - 1;
    nj = obj->v_flag == ON ? nv : nv - 1;

    for(i=0; i<=ni-1; i++) {
      glBegin(GL_TRIANGLE_STRIP);
      for(j=0; j<=nj; j++) {
      
	vertex = &(obj->vlist[myidx(i,j,nu,nv)]);
	switch(flag)
	  {
	  case VERT_FRONT:
	    glColor4fv(vertex->cfront.f);
	    break;
	  case VERT_BACK:
	    glColor4fv(vertex->cback.f);
	    break;
	  case DEPTH_C:
	    glColor4fv(palette[vertex->depth]);
	    break;
	  case TEX_C:
	    glTexCoord3dv(vertex->tex.d);
	    break;
	  }
	glNormal3dv(vertex->normal.d);
	glVertex3dv(vertex->p3.d);
	
	vertex = &(obj->vlist[myidx(i+1,j,nu,nv)]);
	switch(flag)
	  {
	  case VERT_FRONT:
	    glColor4fv(vertex->cfront.f);
	    break;
	  case VERT_BACK:
	    glColor4fv(vertex->cback.f);
	    break;
	  case DEPTH_C:
	    glColor4fv(palette[vertex->depth]);
	    break;
	  case TEX_C:
	    glTexCoord3dv(vertex->tex.d);
	    break;
	  }
	glNormal3dv(vertex->normal.d);
	glVertex3dv(vertex->p3.d);
      }
      glEnd();
    }
  } else {
    for(i=0; i<obj->nfaces; i++)
      draw_sd_face(&(obj->flist[i]), flag); 
  }
}

void drawOFF(Obj *obj, int flag)
{
  int i, j;
  Vertex *vertex;

  for(i=0; i<obj->nfaces; i++) {

    if(flag == FACE_FRONT)
      glColor4fv(obj->flist[i].cfront.f);
    else if(flag == FACE_BACK)
      glColor4fv(obj->flist[i].cback.f);
    
    if(scr_door_mode == SD_OFF) {
      
      glBegin(GL_POLYGON);
      for(j=0; j<obj->flist[i].nvert; j++) {
	vertex = obj->flist[i].vertices[j];
	switch(flag)
	  {
	  case VERT_FRONT:
	    glColor4fv(vertex->cfront.f);
	    break;
	  case VERT_BACK:
	    glColor4fv(vertex->cback.f);
	    break;  
	  case DEPTH_C:
	    glColor4fv(palette[vertex->depth]);
	    break;
	  case TEX_C:
	    glTexCoord3dv(vertex->tex.d);
	    break;
	  }
	glNormal3dv(vertex->normal.d);
	glVertex3dv(vertex->p3.d);
      }
      glEnd();
      
    } else {
      draw_sd_face(&(obj->flist[i]), flag);
    }
  }
}

void drawDOT(Obj *obj, int flag)
{
  int i;
  Vertex *vertex;


  if(obj->size_flag == ON)
    glPointSize(obj->size);
  else
    glPointSize(1.0);
  
  glDisable(GL_LIGHTING);
  glBegin(GL_POINTS);
  
  for(i=0; i<obj->nvertices; i++) {
    vertex = &obj->vlist[i];
    switch(flag)
      {
      case VERT_FRONT:
	glColor4fv(vertex->cfront.f);
	break;
      case DEPTH_C:
	glColor4fv(palette[vertex->depth]);
	break;
      }
    glVertex3dv(vertex->p3.d);
  }
  glEnd();
  glEnable(GL_LIGHTING);
}

void drawLINE(Obj *obj, int flag)
{
  int i, j;
  Vertex *vertex;

  if(obj->size_flag == ON)
    glLineWidth(obj->size);
  else
    glLineWidth(1.0);

  glDisable(GL_LIGHTING);

  if(obj->gformat == LINE) {
    
    glBegin(GL_LINES);
    for(i=0; i<obj->nfaces; i++) {
	
      for(j=0; j<obj->flist[i].nvert; j++) {
	vertex = obj->flist[i].vertices[j];
	switch(flag)
	  {
	  case VERT_FRONT:
	    glColor4fv(vertex->cfront.f);
	    break;
	  case DEPTH_C:
	    glColor4fv(palette[vertex->depth]);
	    break;
	  }
	glVertex3dv(vertex->p3.d);
      }
    }
    glEnd();

  } else {

    if(obj->gformat == LINE_STRIP) 
      glBegin(GL_LINE_STRIP);
    else
      glBegin(GL_LINE_LOOP);
    
    for(i=0; i<obj->nvertices; i++) {
      
      vertex = &(obj->vlist[i]);
      switch(flag)
	{
	case VERT_FRONT:
	  glColor4fv(vertex->cfront.f);
	  break;
	case DEPTH_C:
	  glColor4fv(palette[vertex->depth]);
	  break;
	}
      glVertex3dv(vertex->p3.d);
    }
    glEnd();
  }

  glEnable(GL_LIGHTING);
}

void draw_faces() 
{
  int k;
  Obj *obj;

  if(redraw_flag == ON) {
    
    switch(draw_color_mode) 
      {
      case SAME:
	glNewList(FACE_LIST, GL_COMPILE_AND_EXECUTE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	if(gl_version_1_1 == True) {
	  glEnable(GL_POLYGON_OFFSET_FILL);
	  glPolygonOffset(1.0, 1.0);
	}

	for(k=0; k<nobjs; k++) {
	  obj = olist[k];

	  switch(obj->gformat) 
	    {
	    case MESH:
	      if(obj->vcolor > 0) {
		drawMESH(obj, VERT_FRONT);
	      } else {
		if(obj->cformat != NOCOLOR)
		  glColor4fv(obj->cfront.f);
		else
		  glColor4fv(c_default_front);
		drawMESH(obj, NO_COLOR);
	      }
	      break;
	    case FORMAT_OFF:	  
	      if(obj->vcolor > 0) {
		drawOFF(obj, VERT_FRONT);
	      } else if(obj->fcolor > 0){
		drawOFF(obj, FACE_FRONT);
	      } else {
		if(obj->cformat != NOCOLOR)
		  glColor4fv(obj->cfront.f);
		else
		  glColor4fv(c_default_front);
		drawOFF(obj, NO_COLOR);
	      }
	      break;
	    case DOT:	
	      if(obj->vcolor > 0) {
		drawDOT(obj, VERT_FRONT);
	      } else {
		if(obj->cformat != NOCOLOR)
		  glColor4fv(obj->cfront.f);
		else
		  glColor4fv(c_default_front);
		drawDOT(obj, NO_COLOR);
	      }
	      break;
	    case LINE:
	    case LINE_LOOP:
	    case LINE_STRIP:
	      if(obj->vcolor > 0) {
		drawLINE(obj, VERT_FRONT);
	      } else {
		if(obj->cformat != NOCOLOR)
		  glColor4fv(obj->cfront.f);
		else
		  glColor4fv(c_default_front);
		drawLINE(obj, NO_COLOR);
	      }
	      break;
	    }
	}
	
	if(gl_version_1_1 == True) {
	  glDisable(GL_POLYGON_OFFSET_FILL);
	}

	glDisable(GL_COLOR_MATERIAL);
	glEndList();
	break;
	
      case DIFF:
	glNewList(FACE_LIST, GL_COMPILE_AND_EXECUTE);

	if(gl_version_1_1 == True) {
	  glEnable(GL_POLYGON_OFFSET_FILL);
	  glPolygonOffset(1.0, 1.0);
	}

	for(k=0; k<nobjs; k++) {
	  obj = olist[k];
	  
	  if(obj->gformat == DOT) {
	    if(obj->vcolor > 0) {
	      drawDOT(obj, VERT_FRONT);
	    } else {
	      if(obj->cformat != NOCOLOR)
		glColor4fv(obj->cfront.f);
	      else
		glColor4fv(c_default_front);
	      drawDOT(obj, NO_COLOR);
	    }
	  } else if (obj->gformat == LINE ||
		     obj->gformat == LINE_STRIP ||
		     obj->gformat == LINE_LOOP) {
	    if(obj->vcolor > 0) {
	      drawLINE(obj, VERT_FRONT);
	    } else {
	      if(obj->cformat != NOCOLOR)
		glColor4fv(obj->cfront.f);
	      else
		glColor4fv(c_default_front);
	      drawLINE(obj, NO_COLOR);
	    }
	  } else if(obj->vcolor < 2 && obj->fcolor < 2) {
	    if(obj->cformat != NOCOLOR) {
	      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, obj->cfront.f);
	      glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, obj->cback.f);
	    } else {
	      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c_default_front);
	      glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, c_default_back);
	    }
	    
	    switch(obj->gformat) 
	      {
	      case MESH:
		drawMESH(obj, NO_COLOR);
		break;
	      case FORMAT_OFF:
		drawOFF(obj, NO_COLOR);
		break;
	      }
	    
	  } else { /* CMESH or COFF */
	    
	    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	    glEnable(GL_COLOR_MATERIAL);
	    glEnable(GL_CULL_FACE);
	    glCullFace(GL_BACK);
	    
	    switch(obj->gformat) 
	      {
	      case MESH:
		drawMESH(obj, VERT_FRONT);
		break;
	      case FORMAT_OFF:
		if(obj->vcolor >= 2)
		  drawOFF(obj, VERT_FRONT);
		else
		  drawOFF(obj, FACE_FRONT);
		break;
	      }
	    
	    glCullFace(GL_FRONT);
	    switch(obj->gformat) 
	      {
	      case MESH:
		drawMESH(obj, VERT_BACK);
		  break;
	      case FORMAT_OFF:
		if(obj->vcolor >= 2)
		  drawOFF(obj, VERT_BACK);
		else
		  drawOFF(obj, FACE_BACK);
		break;
	      }
	    glDisable(GL_CULL_FACE);
	    glDisable(GL_COLOR_MATERIAL);
	  }
	}

	if(gl_version_1_1 == True) {
	  glDisable(GL_POLYGON_OFFSET_FILL);
	}

	glEndList();
	break;
	
      case DEPTH:
	glNewList(FACE_LIST, GL_COMPILE_AND_EXECUTE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	if(gl_version_1_1 == True) {
	  glEnable(GL_POLYGON_OFFSET_FILL);
	  glPolygonOffset(1.0, 1.0);
	}

	for(k=0; k<nobjs; k++) {
	  obj = olist[k];
	  switch(obj->gformat) 
	    {
	    case MESH:
	      drawMESH(obj, DEPTH_C);
	      break;
	    case FORMAT_OFF:
	      drawOFF(obj, DEPTH_C);
	      break;
	    case DOT:	
	      drawDOT(obj, DEPTH_C);
	      break;
	    case LINE:
	    case LINE_LOOP:
	    case LINE_STRIP:
	      drawLINE(obj, DEPTH_C);
	      break;
	    }
	}

	if(gl_version_1_1 == True) {
	  glDisable(GL_POLYGON_OFFSET_FILL);
	}

	glDisable(GL_COLOR_MATERIAL);
	glEndList();
	break;
	
      case TEXTURE:
	glNewList(FACE_LIST, GL_COMPILE_AND_EXECUTE);

	if(gl_version_1_1 == True) {
	  glEnable(GL_POLYGON_OFFSET_FILL);
	  glPolygonOffset(1.0, 1.0);
	}

 	for(k=0; k<nobjs; k++) {
	  obj = olist[k];

	  if(obj->gformat == DOT) {
	    if(obj->vcolor > 0) {
	      drawDOT(obj, VERT_FRONT);
	    } else {
	      if(obj->cformat != NOCOLOR)
		glColor4fv(obj->cfront.f);
	      else
		glColor4fv(c_default_front);
	      drawDOT(obj, NO_COLOR);
	    }
	  } else if (obj->gformat == LINE ||
		     obj->gformat == LINE_STRIP ||
		     obj->gformat == LINE_LOOP) {
	    if(obj->vcolor > 0) {
	      drawLINE(obj, VERT_FRONT);
	    } else {
	      if(obj->cformat != NOCOLOR)
		glColor4fv(obj->cfront.f);
	      else
		glColor4fv(c_default_front);
	      drawLINE(obj, NO_COLOR);
	    }
	  } else if(obj->texture_flag == ON) {
	    glEnable(GL_TEXTURE_2D);	    
	    switch(obj->gformat) 
	      {
	      case MESH:
		drawMESH(obj, TEX_C);
		break;
	      case FORMAT_OFF:
		drawOFF(obj, TEX_C);
		break;
	      }
	    glDisable(GL_TEXTURE_2D);
	  } else {
	    if(obj->cformat != NOCOLOR)
	      glMaterialfv(GL_FRONT_AND_BACK, 
			   GL_AMBIENT_AND_DIFFUSE, obj->cfront.f);
	    else
	      glMaterialfv(GL_FRONT_AND_BACK, 
			   GL_AMBIENT_AND_DIFFUSE, c_default_front);
	    switch(obj->gformat) 
	      {
	      case MESH:
		drawMESH(obj, NO_COLOR);
		break;
	      case FORMAT_OFF:
		drawOFF(obj, NO_COLOR);
	      }
	  }
	}  

	if(gl_version_1_1 == True) {
	  glDisable(GL_POLYGON_OFFSET_FILL);
	}

	glEndList();
	break;
      }
  } else {
    glCallList(FACE_LIST);
  }
}

void draw_edges() 
{
  int i, j, k;
  Obj *obj;
  Face *face;

  glLineWidth(1);
  if(redraw_flag == ON) {

    glNewList(EDGE_LIST, GL_COMPILE_AND_EXECUTE);
    glDisable(GL_LIGHTING);
    glColor4fv(black);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for(k=0; k<nobjs; k++) {
      obj = olist[k];
      if(obj->gformat == DOT)
	continue;
      for(i=0; i<obj->nfaces; i++) {
      	face = &(obj->flist[i]);
	if(obj->gformat == LINE)
	  glBegin(GL_LINES);
	else
	  glBegin(GL_POLYGON);
	for(j=0; j<obj->flist[i].nvert; j++)
	  glVertex3dv(face->vertices[j]->p3.d);
	glEnd();
      }
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LIGHTING);
    glEndList();
  } else {
    glCallList(EDGE_LIST);
  }
}

void draw_normal() {

  int i, k;
  double x, y, z, nx, ny, nz;
  double scale = 0.2;
  Vertex *v;

  if(redraw_flag == ON) {

    glNewList(NORMAL_LIST, GL_COMPILE_AND_EXECUTE);
    glDisable(GL_LIGHTING);
    glColor4fv(cyan);
    for(k=0; k<nobjs; k++) {
      if(olist[k]->gformat == DOT || olist[k]->gformat == LINE)
	continue;
      glBegin(GL_LINES);
      for(i=0; i<olist[k]->nvertices; i++) {
	v = &(olist[k]->vlist[i]);
	x = v->p3.d[0];
	y = v->p3.d[1];
	z = v->p3.d[2];
	nx = scale * v->normal.d[0];
	ny = scale * v->normal.d[1];
	nz = scale * v->normal.d[2];
	glVertex3d(x-nx, y-ny, z-nz);
	glVertex3d(x+nx, y+ny, z+nz);
      }
      glEnd();
    }
    glEnable(GL_LIGHTING);
    glEndList();
  } else {
    glCallList(NORMAL_LIST);
  }
}

void create_cube_dlist(double size, int flag) {

  if(flag == VERTEX_CUBE)
    glNewList(VERTEX_CUBE_LIST, GL_COMPILE);
  else
    glNewList(PO_CUBE_LIST, GL_COMPILE);
  glBegin(GL_TRIANGLE_STRIP);
  glNormal3f(0.0,0.0,1.0); 
  glVertex3d(-size,-size,size);
  glVertex3d(size,-size,size);
  glVertex3d(-size,size,size);
  glVertex3d(size,size,size);
  glEnd();
  glBegin(GL_TRIANGLE_STRIP);
  glNormal3f(0.0,0.0,-1.0);
  glVertex3d(-size,size,-size);
  glVertex3d(size,size,-size);
  glVertex3d(-size,-size,-size);
  glVertex3d(size,-size,-size);
  glEnd();
  glBegin(GL_TRIANGLE_STRIP);
  glNormal3f(0.0,1.0,0.0);
  glVertex3d(-size,size,size);
  glVertex3d(size,size,size);
  glVertex3d(-size,size,-size);
  glVertex3d(size,size,-size);
  glEnd();
  glBegin(GL_TRIANGLE_STRIP);
  glNormal3f(0.0,-1.0,0.0); 
  glVertex3d(-size,-size,-size);
  glVertex3d(size,-size,-size);
  glVertex3d(-size,-size,size);
  glVertex3d(size,-size,size);
  glEnd();
  glBegin(GL_TRIANGLE_STRIP);
  glNormal3f(1.0,0.0,0.0);
  glVertex3d(size,-size,size);
  glVertex3d(size,-size,-size);
  glVertex3d(size,size,size);
  glVertex3d(size,size,-size);
  glEnd();
  glBegin(GL_TRIANGLE_STRIP);
  glNormal3f(-1.0,0.0,0.0);
  glVertex3d(-size,-size,size);
  glVertex3d(-size,size,size);
  glVertex3d(-size,-size,-size);
  glVertex3d(-size,size,-size);
  glEnd();
  glEndList();
}

void draw_po_cube() {
  double d0, d1, d2;

  d0 = pickerP3.d[0];
  d1 = pickerP3.d[1];
  d2 = pickerP3.d[2];

  glShadeModel(GL_FLAT);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslated(d0, d1, d2);
  glShadeModel(GL_FLAT);
  glCallList(PO_CUBE_LIST);
  glShadeModel(draw_shade_mode);
  glPopMatrix();
  glShadeModel(draw_shade_mode);
} 

void draw_vertex() {
  int i, k;
  double d0, d1, d2;

  if(redraw_flag == ON) {

    glNewList(VERTEX_LIST, GL_COMPILE_AND_EXECUTE);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, yellow);
    glShadeModel(GL_FLAT);

    for(k=0; k<nobjs; k++) {
      for(i=0; i<olist[k]->nvertices; i++) {
	d0 = olist[k]->vlist[i].p3.d[0];
	d1 = olist[k]->vlist[i].p3.d[1];
	d2 = olist[k]->vlist[i].p3.d[2];

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(d0, d1, d2);
	glCallList(VERTEX_CUBE_LIST);
	glPopMatrix();
      }
    }
    glShadeModel(draw_shade_mode);
    glEndList();
  } else {
    glCallList(VERTEX_LIST);
  }
}

void draw_sphere() {
  
  int i, j;
  const double pi = 3.14159265;
  double x1, x2, y1, y2, z1, z2, r1, r2;

  glLineWidth(1);
  if(redraw_flag == ON) {

    glNewList(UNIT_SPHERE_LIST, GL_COMPILE_AND_EXECUTE);
    glDisable(GL_LIGHTING);
    glColor4fv(black);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    /* make sphere as set of quad strips */
    for(i=0; i<sph_rows; i++) {
      y1 = sph_size*cos((double)(i*pi/sph_rows));
      y2 = sph_size*cos((double)((i+1)*pi/sph_rows));
      r1 = sph_size*sin((double)(i*pi/sph_rows));
      r2 = sph_size*sin((double)((i+1)*pi/sph_rows));

      glBegin(GL_QUAD_STRIP);
      for(j=0; j<=sph_rows*2; j++){
	z1 = r1*cos(2.0*(double)pi/(2.0*sph_rows)*j);
	x1 = r1*sin(2.0*(double)pi/(2.0*sph_rows)*j); 
	z2 = r2*cos(2.0*(double)pi/(2.0*sph_rows)*j);
	x2 = r2*sin(2.0*(double)pi/(2.0*sph_rows)*j);
	glVertex3d(x1, y1, z1);
	glVertex3d(x2, y2, z2);
      }
      glEnd();
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LIGHTING);
    glEndList();

  } else {
    glCallList(UNIT_SPHERE_LIST);
  }
}

void draw_palette() {
  int i; 
  double p[3];
  double x0, x1, y0, y1, dy;
  
  x0 = 0.05; x1= 0.07;
  y0 = 0.2; y1= 0.8; dy = (y1-y0) / 255.0;

  glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glBegin(GL_TRIANGLE_STRIP);
  for(i=0; i<256; i++) { 
    p[0] = x0;
    p[1] = y0 + i * dy;;
    p[2] = 0.0;
    glColor4fv(palette[i]);
    glVertex3dv(p);
    p[0] = x1;
    glVertex3dv(p);
  }
  glEnd();
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glEnable(GL_LIGHTING);
}

void draw_lighting() 
{
  glDisable(GL_LIGHTING);
  glColor4fv(yellow);
  glLineWidth(2.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadMatrixd(trans_mat.d);
  glBegin(GL_LINES);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(ltp[0]*100.0, ltp[1]*100.0, ltp[2]*100.0);
  glEnd();
  glPopMatrix();
  glEnable(GL_LIGHTING);
}

void draw_axis() {
  double scale = 1.03;

  glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-scale, scale*7, -scale, scale*7, -scale, scale);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixd(rot_mat.d);
  glLineWidth(2.0);

  glBegin(GL_LINES);
  glColor4fv(red);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(mat4.d[0], mat4.d[1], mat4.d[2]);
  glColor4fv(green);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(mat4.d[4], mat4.d[5], mat4.d[6]);
  glColor4fv(blue);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(mat4.d[8], mat4.d[9], mat4.d[10]);
  glColor4fv(white);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(mat4.d[12], mat4.d[13], mat4.d[14]);
  glEnd();
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glEnable(GL_LIGHTING);
}

void draw_all() {
  GLenum glerr;

  glClearColor(BGred, BGgreen, BGblue, BGalpha);
  glShadeModel(draw_shade_mode);

#ifndef NO_STEREO
  if(stereo_mode == TRUE) {
    stereo_left_buffer();
    glViewport(0, glw_height/2.0, glw_width, glw_height/2.0);
    stereoproj(LEFT_EYE);
    
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if(draw_edge_flag == ON)
      draw_edges();
    if(draw_face_flag == ON)
      draw_faces();
    if(draw_vertex_flag == ON)
      draw_vertex();
    if(draw_normal_flag == ON)
      draw_normal();
    if(draw_unitsph_flag == ON)
      draw_sphere();
    if(draw_palette_flag == ON)
      draw_palette();
    if(draw_lighting_flag == ON)
      draw_lighting();
    if(picker_flag == ON)
      draw_po_cube();
    if(draw_axis_flag == ON)
      draw_axis();
    /*necessary for stereo. glFlush allows buffer to switch before 
      all commands are processed */
    glFinish(); 
 
    stereo_right_buffer();
    stereoproj(RIGHT_EYE);
  }   
#endif

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
  if(draw_edge_flag == ON)
    draw_edges();
  if(draw_face_flag == ON)
    draw_faces();
  if(draw_vertex_flag == ON)
    draw_vertex();
  if(draw_normal_flag == ON)
    draw_normal();
  if(draw_unitsph_flag == ON)
    draw_sphere();
  if(draw_palette_flag == ON)
    draw_palette();
  if(draw_lighting_flag == ON)
    draw_lighting();
  if(picker_flag == ON)
    draw_po_cube();
  if(draw_axis_flag == ON)
    draw_axis();
  if(redraw_flag == ON)
    while((glerr = glGetError()) != GL_NO_ERROR)
      printf("GL ERROR: %s\n", gluErrorString(glerr));
  glFlush();
  
  redraw_flag = OFF;
  glXSwapBuffers(dpy, main_win);
}
