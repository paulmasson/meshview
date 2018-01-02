#define LOAD_C
#include "all.h"

#include <strings.h> /* for bzero */

/* local function declarations */
int loadFRAMES(char *, Obj **);
int loadLIST(char *, Obj **);
int loadOFF(char *, Obj **);
int loadMESH(char *, Obj **);
int loadDOT(char *, Obj **);
int loadLINE(char *, Obj **);

/*******************************************************************************
 * local data structure : a queue 
 * used to store values when reading data files
 ******************************************************************************/
typedef struct node{
  double v;
  struct node *next;
} node;

typedef struct {
  node *begin;
  node *end;
  int count;
} queue;

void enqueue(double value, queue *q)
{
  node *tmp;
  
  if((tmp = (node *) malloc(sizeof(node))) == NULL) {
    printf("Warning : malloc failed.\n");
  }
  tmp->v = value;
  tmp->next = NULL;
  if(q->count == 0) {
    q->begin = tmp;  
    q->end = tmp;
  } else {
    (q->end)->next = tmp;
    q->end = tmp;
  }
  q->count++;
}

double dequeue(queue *q) 
{
  node *tmp;
  double v;

  if(q->count == 0) {
    printf("Queue is empty.\n"); /* shouldn't get here */
    exit(0);
  }
  v = (q->begin)->v;
  tmp = q->begin;
  q->begin = (q->begin)->next;
  free(tmp);
  q->count--;
  return v;
}

void qfree(queue *q) 
{
  node *tmp;

  while(q->count > 0) {
    tmp = q->begin;
    q->begin = (q->begin)->next;
    free(tmp);
    q->count--;
  }
}

/*******************************************************************************
 * helper functions to work with objects
 * 
 ******************************************************************************/
void free_obj(Obj *obj) 
{
  int i;

  if(obj != NULL) {
    if(obj->vlist != NULL)
      for(i = 0; i < obj->nvertices; i++)
	myfree(obj->vlist[i].faces);
    if(obj->flist != NULL)
      for(i = 0; i < obj->nfaces; i++)
	myfree(obj->flist[i].vertices);
    myfree(obj->vlist);
    myfree(obj->flist);
    for(i = 0; i < obj->nframes; i++)
      free_obj(obj->frames[i]);
    myfree(obj->frames);
    myfree(obj);
  }
}

#if DEBUG_LOAD
/* needed only when DEBUG */
void print_vertex(Vertex *v)
{
  int i;

  if(v){
    printf("VERTEX %d:\n"
	   "   coord:   %4.2f %4.2f %4.2f %4.2f\n"
	   "   normal:  %4.2f %4.2f %4.2f\n"
	   "   cfront:  %4.2f %4.2f %4.2f %4.2f\n"
	   "   cback:   %4.2f %4.2f %4.2f %4.2f\n"
	   "   tex:     %4.2f %4.2f %4.2f\n"
	   "   Nfaces:  %i [",
	   v->index,
	   v->p4.d[0], v->p4.d[1], v->p4.d[2], v->p4.d[3],
	   v->normal.d[0], v->normal.d[1], v->normal.d[2],
	   v->cfront.f[0], v->cfront.f[1], v->cfront.f[2], v->cfront.f[3],
	   v->cback.f[0], v->cback.f[1], v->cback.f[2], v->cback.f[3],
	   v->tex.d[0], v->tex.d[1], v->tex.d[2],
	   v->Nfaces);
    for(i=0;i<v->Nfaces;i++)
      printf("%d ", v->faces[i]->index);
    printf("]\n\n");
  }
}
void print_face(Face *f) 
{
  int i;

  if(f) {
    printf("FACE %d:\n"
	   "   cfront:  %4.2f %4.2f %4.2f %4.2f\n"
	   "   cback:   %4.2f %4.2f %4.2f %4.2f\n"
	   "   nvert:   %d [",
	   f->index,
	   f->cfront.f[0], f->cfront.f[1], f->cfront.f[2], f->cfront.f[3],
	   f->cback.f[0], f->cback.f[1], f->cback.f[2], f->cback.f[3]
	   , f->nvert);
    for(i=0;i<f->nvert;i++)
      printf("%d ", f->vertices[i]->index);
    printf("]\n\n");
  }
}
void print_obj(Obj *o, char *filename)
{
  int i;

  printf("File %s.\n", filename);
  if(o) {
    switch(o->gformat)
      {
      case FORMAT_OFF:
	printf("Format: OFF.\n");
	break;
      case MESH:
	printf("Format: MESH.\n");
	break;
      default:
	printf("Format: UNKNOWN.\n");
	break;
      }
    switch(o->cformat)
      {       /* UNIFORM (0, 1 or 2 colors per obj) */ 
      case NOCOLOR:
	printf("Color format: NOCOLOR.\n");
	break;
      case UNIFORM1:
	printf("Color format: UNIFORM1.\n");
	break;
      case UNIFORM2:
	printf("Color format: UNIFORM2.\n");
	break;
      default:
	printf("Color format: UNKNOWN.\n");
	break;
      }
    printf("COLOR_FRONT: %4.2f %4.2f %4.2f %4.2f\n"
	   "COLOR_BACK:  %4.2f %4.2f %4.2f %4.2f\n",
	   o->cfront.f[0], o->cfront.f[1], o->cfront.f[2], o->cfront.f[3],
	   o->cback.f[0], o->cback.f[1], o->cback.f[2], o->cback.f[3]);
    if(o->texture_flag == ON)
      printf("TEXTURE COORD: YES\n");
    else
      printf("TEXTURE COORD: NO\n");
    printf("VERTEX COLORS = %d\n", o->vcolor);
    printf("FACE COLORS = %d\n", o->fcolor);
  
    if(o->gformat == MESH) {
      if(o->u_flag == ON)
	printf("U_FLAG: ON\n");
      else
	printf("U_FLAG: OFF\n");
      if(o->v_flag == ON)
	printf("V_FLAG: ON\n");
      else
	printf("V_FLAG: OFF\n");
      printf("\nNU = %d NV = %d\n", o->nu, o->nv);
    }

    printf("\nNVERTICES = %d NFACES = %d\n", o->nvertices, o->nfaces);

    if(o->vlist != NULL)
      for(i = 0; i < o->nvertices; i++)
	print_vertex(&(o->vlist[i]));
    if(o->flist != NULL)
      for(i = 0; i < o->nfaces; i++)
	print_face(&(o->flist[i]));
  }
}
#endif


/*******************************************************************************
 * real stuff
 * 
 ******************************************************************************/

void load_settings(char *filename)
{
  FILE *in;
  char key[100];
  int i;

  if((in = fopen(filename, "r"))== NULL) {
    printf("file %s doesn't exist.\n", filename);
    return;
  }  

  fscanf(in, "%s", key);
  if(strcmp(key, "SETTING") != 0){
    printf("Invalid file format.\n");
    fclose(in);
    return;
  } else {
    printf("loading settings from %s ...\n", filename);
  }
  
  while(fscanf(in, "%s", key) != EOF) {
    if(!strcmp(key, "NEAR"))
      fscanf(in, "%lf", &near);
    else if(!strcmp(key, "FAR"))
      fscanf(in, "%lf", &far);
    else if(!strcmp(key, "SCALE"))
      fscanf(in, "%lf", &scale);
    else if(!strcmp(key, "FDIST"))
      fscanf(in, "%lf", &fdist);
    else if(!strcmp(key, "EYE"))
      fscanf(in, "%lf", &eye);
    else if(!strcmp(key, "SPECULAR"))
      fscanf(in, "%lf", &specular);
    else if(!strcmp(key, "SHININESS"))
      fscanf(in, "%lf", &shininess);
    else if(!strcmp(key, "MSENSE"))
      fscanf(in, "%lf", &m_sense);
    else if(!strcmp(key, "MSPEED"))
      fscanf(in, "%lf", &m_speed);
    else if(!strcmp(key, "POLARD"))
      fscanf(in, "%lf", &polard);
    else if(!strcmp(key, "VCUBESIZE")){
      fscanf(in, "%lf", &vcubesize);
      create_cube_dlist(0.03*vcubesize, VERTEX_CUBE);
    }
    else if(!strcmp(key, "SCREENDOORGAP"))
      fscanf(in, "%lf", &scrdoorgap);
    else if(!strcmp(key, "BGRED"))
      fscanf(in, "%f", &BGred);
    else if(!strcmp(key, "BGGREEN"))
      fscanf(in, "%f", &BGgreen);
    else if(!strcmp(key, "BGBLUE"))
      fscanf(in, "%f", &BGblue);
    else if(!strcmp(key, "BGALPHA"))
      fscanf(in, "%f", &BGalpha);
    else if(!strcmp(key, "LIGHTPOS"))
      fscanf(in, "%f%f%f", &(ltp[0]), &(ltp[1]), &(ltp[2]));
    else if(!strcmp(key, "4DMAT")) {
      for(i=0; i<16; i++)
	fscanf(in, "%lf", &(mat4.d[i]));
      From4DTo3D();
      AutoSmooth();
    }
    else if(!strcmp(key, "3DROTMAT"))
      for(i=0; i<16; i++) 
	fscanf(in, "%lf", &(rot_mat.d[i]));
    else if(!strcmp(key, "3DTRANMAT"))
      for(i=0; i<16; i++) 
	fscanf(in, "%lf", &(trans_mat.d[i]));
    else
      printf("Unknown keyword : %s\n", key);
  }
  fclose(in);
  setting_all();
  color_all();
  redraw_flag = ON;
  draw_all();
}

void load_pallete(char *filename)
{

  FILE *in;
  char key[200];
  int i, j;

  if((in = fopen(filename, "r"))== NULL) {
    printf("file %s doesn't exist.\n", filename);
    return;
  }

  fscanf(in, "%s", key);

  if(strcmp(key, "PALETTE") != 0){
    printf("Invalid file format.\n");
    fclose(in);
    return;
  } else {
    printf("loading palette from %s ...\n", filename);
  }

  for(i=0; i<256; i++) {
    for(j=0; j<3; j++)
      fscanf(in, "%f", &(palette[i][j]));
    palette[i][3] = 1.0;
  }
  fclose(in);
  redraw_flag = ON;
  draw_all();
}

void load_texture(char *filename)
{

  FILE *in;
  char str[200], *str_ptr, *end_str;
  int i, j, k, value;
  int numRows, numCols;
  int newRows, newCols;
  int maxVal;
  GLubyte *temp;

  if((in = fopen(filename, "r"))== NULL) {
    printf("file %s doesn't exist.\n", filename);
    return;
  }

  fscanf(in, "%s", str);
  if((str[0] != 'P') || (str[1] != '6' && str[1] != '5')) {
    fprintf(stderr,"Unknown File format\n");
    return;
  }
  
  while(fgets(str, 200, in) != NULL && str[0] == '#'); /*skip comments*/
  str_ptr = str;
  for(;;) {
    numCols = strtol(str_ptr, &end_str, 10);
    if(str_ptr != end_str) break;
    if(fgets(str, 200, in) == NULL) return;
    str_ptr = str;
  }
  str_ptr = str;
  for(;;) {
    str_ptr = end_str;
    numRows = strtol(str_ptr, &end_str, 10);
    if(str_ptr != end_str) break;
    if(fgets(str, 200, in) == NULL) return;
  }
  str_ptr = end_str;
  for(;;) {
    maxVal = strtol(str_ptr, &end_str, 10);
    if(str_ptr != end_str) break;
    if(fgets(str, 200, in) == NULL) return;
    str_ptr = str;
  }

  if(numCols <= 0 || numRows <= 0) {
    printf("invalid height or width of the image\n");
    return;
  }

  temp = (GLubyte *)malloc(numCols * numRows * 3 * sizeof(GLubyte));
  
  for (i = 0; i < numRows; i++)
    for (j = 0; j < numCols; j++)
      for(k = 0; k < 3; k++)
	if((value = fgetc(in)) != EOF)
	  temp[(i*numCols+j)*3+k] = (GLubyte)value;
	else {
	  printf("Bad file: %s\n", filename);
	  return;
	}
  fclose(in);
  
  free(texImage);
  
  newRows = numRows;
  newCols = numCols;
  
  /* Do something weird so that it is always a power of 2: */
  if(numRows <= 2)          newRows = 2;
  else if(numRows <= 4)     newRows = 4;
  else if(numRows <= 8)     newRows = 8;
  else if(numRows <= 16)    newRows = 16;
  else if(numRows <= 32)    newRows = 32;
  else if(numRows <= 64)    newRows = 64;
  else if(numRows <= 128)   newRows = 128;
  else if(numRows <= 256)   newRows = 256;
  else if(numRows <= 512)   newRows = 512;
  else if(numRows > 512)    newRows = 512;

  if(numCols <= 2)          newCols = 2;
  else if(numCols <= 4)     newCols = 4;
  else if(numCols <= 8)     newCols = 8;
  else if(numCols <= 16)    newCols = 16;
  else if(numCols <= 32)    newCols = 32;
  else if(numCols <= 64)    newCols = 64;
  else if(numCols <= 128)   newCols = 128;
  else if(numCols <= 256)   newCols = 256;
  else if(numCols <= 512)   newCols = 512;
  else if(numCols > 512)    newCols = 512;
  
  if (newRows != numRows || newCols != numCols) {

    texImage  = (GLubyte*) malloc ((newRows * newCols) * 3 * sizeof (GLubyte));

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    
    gluScaleImage(GL_RGB, numCols, numRows, GL_UNSIGNED_BYTE, temp,
		  newCols, newRows, GL_UNSIGNED_BYTE, texImage);
    
    texImageWidth = newCols;
    texImageHeight = newRows;
      
    free(temp);
  } else {
    texImageWidth = numCols;
    texImageHeight = numRows;
    texImage = temp;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImageWidth, texImageHeight,
               0, GL_RGB, GL_UNSIGNED_BYTE, texImage);
  
  redraw_flag = ON;
  draw_all();
}


void load_data(char *filename, int flag) 
{ 
  Obj *objs[MAXOBJ];
  int i, num = 0;
  char *extn;
  
  extn = strrchr(filename, '.'); 
  extn++; /* find beginning of the extention */
  if(strcmp(extn, "list") == 0) {
    if((num = loadLIST(filename, objs)) > 0 && frames_mode == ON) {
      frames_mode = OFF;
      flag = NEW;
    }
  } else if(strcmp(extn, "off") == 0  ||
	  strcmp(extn, "3off") == 0 ||
	  strcmp(extn, "4off") == 0) {
    if((num = loadOFF(filename, objs)) > 0 && frames_mode == ON) {
      frames_mode = OFF;
      flag = NEW;
    }
  } else if(strcmp(extn, "mesh") == 0  ||
	  strcmp(extn, "3mesh") == 0 ||
	  strcmp(extn, "meshc") == 0 ||
	  strcmp(extn, "4mesh") == 0) {
    if((num = loadMESH(filename, objs)) > 0 && frames_mode == ON) {
      frames_mode = OFF;
      flag = NEW;
    }
  } else if(strcmp(extn, "frm") == 0) {
    if((num = loadFRAMES(filename, objs)) > 0) {
      frames_mode = ON;
      flag = NEW; 
    }
  } else if(strcmp(extn, "dot") == 0) {
    if((num = loadDOT(filename, objs)) > 0 && frames_mode == ON) {
      frames_mode = OFF;
      flag = NEW;
    }
  } else if(strcmp(extn, "line") == 0) {
    if((num = loadLINE(filename, objs)) > 0 && frames_mode == ON) {
      frames_mode = OFF;
      flag = NEW;
    }
  }

  while (num == 0) { /* while (instead of if) allows to use break instead of goto */
    if((num = loadLIST(filename, objs) > 0) ||
       (num = loadOFF(filename, objs) > 0)  ||
       (num = loadMESH(filename, objs) > 0) ||
       (num = loadDOT(filename, objs) > 0) ||
       (num = loadLINE(filename, objs) > 0)) {
      if(frames_mode == ON) {
	frames_mode = OFF;
	flag = NEW;
      }
      break;
    }
    if(num = loadFRAMES(filename, objs) > 0) {
      frames_mode = ON;
      flag = NEW;
      break;
    }
    printf("Couldn't load file %s...\n", filename);
    return;
  }

  if(flag == NEW) { /* delete old objects */
    for(;nobjs > 0; nobjs--)
      free_obj(olist[nobjs-1]);
  }

  for(i = 0; i < num && nobjs < MAXOBJ; i++) {
    olist[nobjs] = objs[i];
    nobjs++;
  }
  
  if(i < num) {
    printf("You exceeded MAX number of objects.\n"
	   "%d  new objects discarded.\n", num - i);
    for(;i < num; i++)
      free_obj(objs[i]);
  }

  if(picker_flag == ON) {
    for(i=0; i<nobjs; i++) {
      if(olist[i]->gformat != MESH){
	picker_close(pickerDialog, NULL, NULL);
	break;
      }
    }
  }

  if(nobjs == 1) {
    if(XtIsManaged(objNumScale))
      XtUnmanageChild(objNumScale);
  } else {
    if(!XtIsManaged(objNumScale))
      XtManageChild(objNumScale);
    XtVaSetValues(objNumScale, XmNmaximum, nobjs-1, NULL);
  }
   
  if(frames_mode == OFF && XtIsManaged(framesDialog))
    XtUnmanageChild(framesDialog);

  if(frames_mode == ON)
    frames_init();

  setup();
  Reset();
  redraw_flag = ON;
  draw_all();
}

int loadFRAMES(char *filename, Obj **objs)
{
  FILE *in;
  char name[200], key[200], *lastslash;
  int num_read = 0, nframes = 0, i, j, ftype;
  struct stat dummy;
  Obj *tobjs[MAXOBJ];

  if((in = fopen(filename, "r"))== NULL) {
    printf("file %s doesn't exist.\n", filename);
    return 0;
  }

  fscanf(in, "%s", key);

  if(strcmp(key, "FRAMES") != 0) {
/*     printf("File %s is not in FRAMES format.\n", filename); */
    fclose(in);
    return 0;
  } 

  if(fscanf(in, "%d", &nframes) == EOF || nframes < 2) {
    printf("File %s number of frames less than 2.\n", filename);
    fclose(in);
    return 0;
  }
  
  if(fscanf(in, "%s", key) == EOF) {  /* read the first entry */
    printf("File %s invalid format.\n", filename);
    fclose(in);
    return 0;
  }

  /* make sure the file exists */
  if(stat(key, &dummy) == -1) {
    strcpy(name, filename);
    lastslash = strrchr(name, '/');
    if(lastslash == NULL){
      printf("Couldn't find %s file.\n", key);
      return 0;
    }
    lastslash++;
    strcpy(lastslash, key);
    if(stat(name, &dummy) == -1) {
      printf("Couldn't find %s file.\n", name);
      return 0;
    }
  } else {
    strcpy(name, key); 
  }
  
  /* load first entry into the object itself and determine how rest of
     entries are read */
  
  if((num_read = loadMESH(name, objs)) == 1)
    ftype = MESH;
  else if((num_read = loadOFF(name, objs)) == 1)
    ftype = FORMAT_OFF;
  else if((num_read = loadDOT(name, objs)) == 1)
    ftype = DOT;
  else if((num_read = loadLINE(name, objs)) == 1)
    ftype = LINE;
  else if((num_read = loadLIST(name, objs)) > 0)
    ftype = LIST;
  else {
    printf("Couldn't load file %s\n", name);
    fclose(in);
    return 0;
  }

  /* allocate memory for frames */
  for(i=0; i<num_read; i++) {
    objs[i]->nframes = 0;
    if((objs[i]->frames = (Obj **)malloc(nframes*sizeof(Obj*))) == NULL) {
      printf("File: %s. Malloc failed.\n", filename);
      for(;i>=0; i--)
	free_obj(objs[i]);
      fclose(in);
      return 0;
    }
  }
  
  for(i=0; i<nframes; i++) {
    
    switch(ftype)
      {
      case MESH:
	if(loadMESH(name, &(objs[0]->frames[i])) != num_read) {
	  printf("File %s: couldn't load frame %d (%s)\n", filename, i, name);
	  for(;num_read > 0; num_read--)
	    free_obj(objs[num_read-1]);
	  fclose(in);
	  return 0;
	}
	objs[0]->nframes++;
	break;
      case FORMAT_OFF:
	if(loadOFF(name,  &(objs[0]->frames[i])) != num_read) {
	  printf("File %s: couldn't load frame %d (%s)\n", filename, i, name);
	  for(;num_read > 0; num_read--)
	    free_obj(objs[num_read-1]);
	  fclose(in);
	  return 0;
	}
	objs[0]->nframes++;
	break;
      case DOT:
	if(loadDOT(name,  &(objs[0]->frames[i])) != num_read) {
	  printf("File %s: couldn't load frame %d (%s)\n", filename, i, name);
	  for(;num_read > 0; num_read--)
	    free_obj(objs[num_read-1]);
	  fclose(in);
	  return 0;
	}
	objs[0]->nframes++;
	break;
       case LINE:
	if(loadLINE(name,  &(objs[0]->frames[i])) != num_read) {
	  printf("File %s: couldn't load frame %d (%s)\n", filename, i, name);
	  for(;num_read > 0; num_read--)
	    free_obj(objs[num_read-1]);
	  fclose(in);
	  return 0;
	}
	objs[0]->nframes++;
	break;
     case LIST:
	if(loadLIST(name, tobjs) != num_read) {
	  printf("File %s: couldn't load frame %d (%s)\n", filename, i, name);
	  for(;num_read > 0; num_read--)
	    free_obj(objs[num_read-1]);
	  for(j = 0; j<MAXOBJ; j++)
	    free_obj(tobjs[j]);
	  fclose(in);
	  return 0;
	} else {
	  for(j=0; j<num_read; j++) {
	    objs[j]->frames[i] = tobjs[j];
	    objs[j]->nframes++;
	  }
	}
	break;
      }

    if(i==nframes-1) break;

    /* get next filename */
    if(fscanf(in, "%s", key) == EOF) {
      printf("File %s: premature EOF.\n", filename);
      for(;num_read > 0; num_read--)
	free_obj(objs[num_read-1]);
      fclose(in);
      return 0;
    }
    
    if(stat(key, &dummy) == -1) {
      strcpy(name, filename);
      lastslash = strrchr(name, '/');
      if(lastslash == NULL){
	printf("Couldn't find %s file.\n", key);
	for(;num_read > 0; num_read--)
	  free_obj(objs[num_read-1]);
	fclose(in);
	return 0;
      }
      lastslash++;
      strcpy(lastslash, key);
      if(stat(name, &dummy) == -1) {
	printf("Couldn't find %s file.\n", name);
	for(;num_read > 0; num_read--)
	  free_obj(objs[num_read-1]);
	fclose(in);
	return 0;
      }
    } else {
      strcpy(name, key); 
    }
  }

  fclose(in);

  for(i = 0; i < num_read; i++)
    for(j = 0; j < nframes; j++)
      if(objs[i]->frames[j]->nvertices != objs[i]->nvertices) {
	printf("File %s obj %d: frames have different number of vertices.\n", 
	       filename, i);
	for(;num_read > 0; num_read--)
	  free_obj(objs[num_read-1]);
	return 0;
      }
  return num_read;
}

int loadLIST(char *filename, Obj **objs)
{
  FILE *in;
  char name[200], key[200], *lastslash;
  int num_read = 0;
  struct stat dummy;

  if((in = fopen(filename, "r"))== NULL) {
    printf("file %s doesn't exist.\n", filename);
    return num_read;
  }
  
  fscanf(in, "%s", key);
  
  if(strcmp(key, "LIST") != 0) {
/*     printf("File %s is not in LIST format.\n", filename); */
    fclose(in);
    return num_read;
  }

  while(fscanf(in, "%s", key) != EOF) {
    if(stat(key, &dummy) == -1) {
      strcpy(name, filename);
      lastslash = strrchr(name, '/');
      if(lastslash == NULL){
	printf("Couldn't find %s file.\n", key);
	continue;
      }
      lastslash++;
      strcpy(lastslash, key);
      if(stat(name, &dummy) == -1) {
	printf("Couldn't find %s file.\n", name);
	continue;
      }
    } else {
      strcpy(name, key); 
    }
    if(loadMESH(name, &objs[num_read]) == 1 || 
       loadOFF(name, &objs[num_read]) == 1 || 
       loadDOT(name, &objs[num_read]) == 1 || 
       loadLINE(name, &objs[num_read]) == 1){
      num_read++;
    } else {
      printf("Couldn't load file %s\n", name);
    }
    if(num_read >= MAXOBJ) {
      printf("Maximum number of objects in the list exceeded.\n");
      break;
    }
  }
  fclose(in);
  return num_read;
}

int loadOFF(char *filename, Obj **objret) 
{
  FILE *in;
  char str[1000], *str_ptr, *end_str;
  int texture_flag = OFF;
  int vcolor = 0;
  int fcolor = 0;
  int normal_flag = OFF;
  int dim = 3;
  int cformat = NOCOLOR;
  int nvertices, nfaces, tmp_value;
  Vertex *vlist;
  Face *flist;
  Color4 cfront, cback;
  Obj *obj;

  int i, j;
  double value;
  queue q, *vq;

  q.count = 0;

  if((in = fopen(filename, "r")) == NULL) {
    printf("Couldn't open file %s.\n", filename);
    return 0;
  }

  /* parse the header */
  while(fgets(str, 1000, in) != NULL && str[0] == '#');
  str_ptr = str;

  if(strncmp(str_ptr, "ST", 2) == 0) {
    texture_flag = ON;
    str_ptr+=2;
  }
  
  if(*str_ptr == 'C') {
    vcolor = 1;
    str_ptr++;
  }

  if(*str_ptr == 'C') {
    vcolor = 2;
    str_ptr++;
  }  

  if(*str_ptr == 'D') {
    fcolor = 1;
    str_ptr++;
  }
  
  if(*str_ptr == 'D') {
    fcolor = 2;
    str_ptr++;
  }

  if(*str_ptr == 'N') {
    normal_flag = ON;
    str_ptr++;
  }

  if(*str_ptr == '3') {
    str_ptr++;
  } else if(*str_ptr == '4') {
    dim = 4;
    str_ptr++;
  }

  if(*str_ptr == 'n') {
    printf("File: %s. nOFF format is presently unsupported.\n", filename);
    return 0;
  }

  if(!strncmp(str_ptr, "OFF", 3)) {
    str_ptr += 3;
    if(*str_ptr == 'C' || *str_ptr == 'c') {
      str_ptr++;
      if(*str_ptr == 'C' || *str_ptr == 'c') {
	str_ptr++;
	cformat = UNIFORM2;
      } else {
	cformat = UNIFORM1;
      }
    }
  } else {
/*     printf("File: %s Invalid OFF format.\n", filename); */
    return 0;
  }
  
  /* put all values into a queue */
  while(fgets(str, 1000, in) != NULL) {
    str_ptr = str;
    for(;;) {
      value = strtod(str_ptr, &end_str);
      if(str_ptr == end_str) break;
      str_ptr = end_str;
      enqueue(value, &q);
    }
  }
  fclose(in);

  /* check if there is enough values for object color(s), 
     #vertices, #faces and #edges in a queue */
  if(q.count < (cformat == UNIFORM1 ? 4 : (cformat == UNIFORM2 ? 8 : 0)) + 3) {
    printf("File %s. Not enough valuess.\n", filename);
    qfree(&q);
    return 0;
  }
  
  /* read in object color(s) */
  if(cformat != NOCOLOR) {
    cfront.f[0] = dequeue(&q);
    cfront.f[1] = dequeue(&q);
    cfront.f[2] = dequeue(&q);
    cfront.f[3] = dequeue(&q);
    if(cformat == UNIFORM2) {
      cback.f[0] = dequeue(&q);
      cback.f[1] = dequeue(&q);
      cback.f[2] = dequeue(&q);
      cback.f[3] = dequeue(&q);
    }
    else {
      cback.f[0] = 1.0 - cfront.f[0];
      cback.f[1] = 1.0 - cfront.f[1];
      cback.f[2] = 1.0 - cfront.f[2];
      cback.f[3] = 1.0;
    }
  }

  nvertices = (int)dequeue(&q);
  nfaces = (int)dequeue(&q); 
  dequeue(&q); /* skip number of edges */

  /* check if there's enough values for vertices in a queue */
  if(q.count < nvertices * (dim + (texture_flag == ON ? 2 : 0)
			  + (vcolor == 1 ? 4 : (vcolor == 2 ? 8 : 0))
			  + (normal_flag == ON ? 3 : 0))) {
    printf("File: %s. Not enough values.\n", filename);
    qfree(&q);
    return 0;
  }

  /* allocate and zero out memory */
  obj = (Obj *)malloc(sizeof(Obj));
  vlist = (Vertex*)malloc(sizeof(Vertex) * nvertices);
  flist = (Face*) malloc(sizeof(Face) * nfaces);
  /* queues to store adjaicent faces for each vertex */
  vq = (queue*) malloc(sizeof(queue) * nvertices); 

  if(obj == NULL || vlist == NULL || flist == NULL || vq == NULL) {
    printf("File: %s. Malloc failed.\n", filename);
    qfree(&q);
    myfree(obj);
    myfree(vlist);
    myfree(flist);
    myfree(vq);
    return 0;
  }
  
  bzero(obj, sizeof(Obj));
  bzero(vlist,sizeof(Vertex) * nvertices);
  bzero(flist, sizeof(Face) * nfaces);
  bzero(vq, sizeof(queue) * nvertices);
  
  /* Initialize object */
  obj->nvertices = nvertices;
  obj->nfaces = nfaces;
  obj->vlist = vlist;
  obj->flist = flist;
  obj->gformat = FORMAT_OFF;
  obj->cformat = cformat;
  obj->cfront = cfront;
  obj->cback = cback;
  obj->texture_flag = texture_flag;
  obj->vcolor = vcolor;
  obj->fcolor = fcolor;
  obj->scale = 1.0;

  /* read vertices */
  for(i = 0; i < nvertices; i++) {
    vlist[i].index = i;
    /* all inputs are treated as 4D */
    vlist[i].p4.d[0] = dequeue(&q);
    vlist[i].p4.d[1] = dequeue(&q);
    vlist[i].p4.d[2] = dequeue(&q);
    if(dim == 3)
      vlist[i].p4.d[3] = 0.0;
    else
      vlist[i].p4.d[3] = dequeue(&q);
    if(normal_flag == ON) { /* normals are recalculated later */
      vlist[i].normal.d[0] = dequeue(&q);
      vlist[i].normal.d[1] = dequeue(&q);
      vlist[i].normal.d[2] = dequeue(&q);
    }
    if(vcolor != 0) {
      vlist[i].cfront.f[0] = dequeue(&q);
      vlist[i].cfront.f[1] = dequeue(&q);
      vlist[i].cfront.f[2] = dequeue(&q);
      vlist[i].cfront.f[3] = dequeue(&q);
      if(vcolor == 2) {
        vlist[i].cback.f[0] = dequeue(&q);
        vlist[i].cback.f[1] = dequeue(&q);
        vlist[i].cback.f[2] = dequeue(&q);
        vlist[i].cback.f[3] = dequeue(&q);
      } else {
        vlist[i].cback.f[0] = 1.0 - vlist[i].cfront.f[0];
        vlist[i].cback.f[1] = 1.0 - vlist[i].cfront.f[1];
        vlist[i].cback.f[2] = 1.0 - vlist[i].cfront.f[2];
        vlist[i].cback.f[3] = 1.0;
      }
    }
    if(texture_flag == ON) {
      vlist[i].tex.d[0] = dequeue(&q);
      vlist[i].tex.d[1] = dequeue(&q);
      vlist[i].tex.d[2] = 0.0;
    }
  }
  
  /* read faces */
  for(i = 0; i < nfaces; i++) {
    /* check if queue is not empty */
    if(q.count < 1) {
      printf("File: %s. Not enough values.\n", filename);
      for(j=0; j < nvertices; j++)
	qfree(vq+j);
      myfree(vq);
      qfree(&q);
      free_obj(obj);
      return 0;
    }
    flist[i].index = i;
    flist[i].nvert = (int)dequeue(&q);

    if(flist[i].nvert < 3) {
      printf("File: %s. Face has less than 3 vertices.\n", filename);
      for(j=0; j < nvertices; j++)
	qfree(vq+j);
      myfree(vq);
      qfree(&q);
      free_obj(obj);
      return 0;
    }

    /* check if there's enough values for the face in a queue */
    if(q.count < flist[i].nvert + (fcolor == 1 ? 4 :
				   (fcolor == 2 ? 8 : 0))) {
      printf("File: %s. Not enough values.\n", filename);
      for(j=0; j < nvertices; j++)
	qfree(vq+j);
      myfree(vq);
      qfree(&q);
      free_obj(obj);
      return 0;
    }

    /* allocate and zero out memory for vertex array  */
    if((flist[i].vertices = 
	(Vertex **)malloc(sizeof(Vertex*) * flist[i].nvert)) == NULL) {
      printf("File %s. Malloc failed.\n", filename);
      for(j=0; j < nvertices; j++)
	qfree(vq+j);
      myfree(vq);
      qfree(&q);
      free_obj(obj);
      return 0;
    }
    bzero(flist[i].vertices, sizeof(Vertex*) * flist[i].nvert);
    
    /* read vertex indices for the current face */
    for(j = 0; j < flist[i].nvert; j++) {
      tmp_value = (int)dequeue(&q);
      if(tmp_value < 0 || tmp_value >=  nvertices){
	printf("File: %s. Vertex index is out of range.\n", filename);
	for(j=0; j < nvertices; j++)
	  qfree(vq+j);
	myfree(vq);
	qfree(&q);
	free_obj(obj);
	return 0;
      }
      flist[i].vertices[j] = &vlist[tmp_value];
      enqueue((double)i, &vq[tmp_value]);
    }

    if(fcolor != 0) {
      flist[i].cfront.f[0] = dequeue(&q); 
      flist[i].cfront.f[1] = dequeue(&q);
      flist[i].cfront.f[2] = dequeue(&q); 
      flist[i].cfront.f[3] = dequeue(&q);
      if(fcolor == 2) {
	flist[i].cback.f[0] = dequeue(&q);
	flist[i].cback.f[1] = dequeue(&q);
	flist[i].cback.f[2] = dequeue(&q);	
	flist[i].cback.f[3] = dequeue(&q);
      }
      else {
	flist[i].cback.f[0] = 1.0 - flist[i].cfront.f[0];
	flist[i].cback.f[1] = 1.0 - flist[i].cfront.f[1];
	flist[i].cback.f[2] = 1.0 - flist[i].cfront.f[2];
	flist[i].cback.f[3] = 1.0;
      }
    }
  }

  /* Create Face* array for each vertex */
  for(i = 0; i < nvertices; i++) {
    if((vlist[i].faces = 
	(Face **)malloc(sizeof(Face*) * vq[i].count)) == NULL) {
      printf("File %s. Malloc failed.\n", filename);
      for(j=0; j < nvertices; j++)
	qfree(vq+j);
      myfree(vq);
      qfree(&q);
      free_obj(obj);
      return 0;
    }
    while(vq[i].count > 0) {
      vlist[i].faces[vlist[i].Nfaces] = &flist[(int)dequeue(vq+i)];
      vlist[i].Nfaces++;
    }
  }
  myfree(vq);

  if(q.count > 0) {
    printf("File %s %d numbers left unread.\n", filename, q.count);
    qfree(&q);
  }

  *objret = obj;
  
#if DEBUG_LOAD
  print_obj(obj, filename);
#endif
  
  printf("File %s has been loaded\n", filename);
  return 1;
}

int loadMESH(char *filename, Obj **objret) 
{
  FILE *in;
  char str[1000], *str_ptr, *end_str;
  int zmesh_flag = OFF;
  int texture_flag = OFF;
  int vcolor = 0;
  int normal_flag = OFF;
  int dim = 3;
  int cformat = NOCOLOR;
  int nvertices, nfaces;
  Vertex *vlist;
  Face *flist;
  Color4 cfront, cback;
  int nu, nv;
  int u_flag = OFF;
  int v_flag = OFF;
  Obj *obj;

  int i, j, ni, nj, count, *vnfaces;
  int k00, k01, k10, k11;
  double value;
  queue q;

  q.count = 0;

  if((in = fopen(filename, "r")) == NULL) {
    printf("Couldn't open file %s.\n", filename);
    return 0;
  }
  
  /* parse the header */
  while(fgets(str, 1000, in) != NULL && str[0] == '#');
  str_ptr = str;
  
  if(*str_ptr == 'U') {
    texture_flag = ON;
    str_ptr++;
  }
  
  if(*str_ptr == 'C') {
    vcolor = 1;
    str_ptr++;
  }

  if(*str_ptr == 'C') {
    vcolor = 2;
    str_ptr++;
  }
  
  if(*str_ptr == 'N') {
    normal_flag = ON;
    str_ptr++;
  }

  if(*str_ptr == 'Z') {
    zmesh_flag = ON;
    str_ptr++;
  }
   
  if(*str_ptr == '3') {
    str_ptr++;
  } else if(*str_ptr == '4') {
    if(zmesh_flag == ON) {
      printf("File %s. Invalid MESH format [Z4MESH].\n", filename);
      return 0;
    }
    dim = 4;
    str_ptr++;
  }

  if(*str_ptr == 'u') {
    u_flag = ON;
    str_ptr++;
  }

  if(*str_ptr == 'v') {
    v_flag = ON;
    str_ptr++;
  }

  if(*str_ptr == 'n') {
    printf("File %s. nMESH format is presently unsupported.\n", filename);
    return 0;
  }
  
  if(!strncmp(str_ptr, "MESH", 4)) {
    str_ptr += 4;
    if(*str_ptr == 'C' || *str_ptr == 'c') {
      str_ptr++;
      if(*str_ptr == 'C' || *str_ptr == 'c') {
	str_ptr++;
	cformat = UNIFORM2;
      } else {
	cformat = UNIFORM1;
      }
    }
  } else {
/*     printf("File: %s Invalid MESH format.\n", filename); */
    return 0;
  }

  /* put all values into a queue */
  while(fgets(str, 1000, in) != NULL) {
    str_ptr = str;
    for(;;) {
      value = strtod(str_ptr, &end_str);
      if(str_ptr == end_str) break;
      str_ptr = end_str;
      enqueue(value, &q);
    }
  }
  fclose(in);
  
  /*check if there are enough values for object color(s) and Nu Nv in a queue*/
  if(q.count < (cformat == UNIFORM1 ? 4 : (cformat == UNIFORM2 ? 8 : 0)) + 2) {
    printf("File %s. Not enough valuess.\n", filename);
    qfree(&q);
    return 0;
  }
  
  /* read in object color(s) */
  if(cformat != NOCOLOR) {
    cfront.f[0] = dequeue(&q);
    cfront.f[1] = dequeue(&q);
    cfront.f[2] = dequeue(&q);
    cfront.f[3] = dequeue(&q);
    if(cformat == UNIFORM2) {
      cback.f[0] = dequeue(&q);
      cback.f[1] = dequeue(&q);
      cback.f[2] = dequeue(&q);
      cback.f[3] = dequeue(&q);
    }
    else {
      cback.f[0] = 1.0 - cfront.f[0];
      cback.f[1] = 1.0 - cfront.f[1];
      cback.f[2] = 1.0 - cfront.f[2];
      cback.f[3] = 1.0;
    }
  }

  nu = (int)dequeue(&q);
  nv = (int)dequeue(&q);
  nvertices = nu*nv;
  ni = u_flag == ON ? nu : nu-1;
  nj = v_flag == ON ? nv : nv-1;
  nfaces = ni * nj;

  /* check if we have enough values for vertices and face colors*/
  if(q.count < nvertices * ((zmesh_flag == ON ? 1 : dim) +
			    (texture_flag == ON ? 3 : 0) +
			    (vcolor == 1 ? 4 : (vcolor == 2 ? 8 : 0)) +
			    (normal_flag == ON ? 3 : 0))) {
    printf("File: %s. Not enough values.\n", filename);
    qfree(&q);
    return 0;
  }

  /* allocate and zero out memory */
  obj = (Obj *)malloc(sizeof(Obj));
  vlist = (Vertex*)malloc(sizeof(Vertex) * nvertices);
  flist = (Face*) malloc(sizeof(Face) * nfaces);
  vnfaces = (int *)malloc(sizeof(int) * nvertices);

  if(obj == NULL || vlist == NULL || flist == NULL || vnfaces == NULL) {
    printf("Malloc failed. File: %s.\n", filename);
    qfree(&q);
    myfree(obj);
    myfree(vlist);
    myfree(flist);
    myfree(vnfaces);
    return 0;
  } 
  
  bzero(obj, sizeof(Obj));
  bzero(vlist,sizeof(Vertex) * nvertices);
  bzero(flist, sizeof(Face) * nfaces);
  bzero(vnfaces, sizeof(int) * nvertices);

  /* Initialize object */
  obj->nvertices = nvertices;
  obj->nfaces = nfaces;
  obj->vlist = vlist;
  obj->flist = flist;
  obj->gformat = MESH;
  obj->cformat = cformat;
  obj->cfront = cfront;
  obj->cback = cback;
  obj->texture_flag = texture_flag;
  obj->vcolor = vcolor;
  obj->fcolor = 0;
  obj->nu = nu;
  obj->nv = nv;
  obj->u_flag = u_flag;
  obj->v_flag = v_flag;
  obj->scale = 1.0;

  /* Compute number of adjaicent faces for each vertex */
  for(i=0; i<nu; i++)
    for(j=0; j<nv; j++)
      vnfaces[myidx2(i,j,nu,nv)] = 4;
  if(u_flag == OFF)
    for(j=0; j<nv; j++) {
      vnfaces[myidx2(0,j,nu,nv)] = 2;
      vnfaces[myidx2(nu-1,j,nu,nv)] = 2;
    }
  if(v_flag == OFF)
    for(i=0; i<nu; i++) {
      vnfaces[myidx2(i,0,nu,nv)] = 2;
      vnfaces[myidx2(i,nv-1,nu,nv)] = 2;
    }
  if(u_flag == OFF && v_flag == OFF) {
    vnfaces[myidx2(0,0,nu,nv)] = 1;
    vnfaces[myidx2(nu-1,0,nu,nv)] = 1;
    vnfaces[myidx2(0,nv-1,nu,nv)] = 1;
    vnfaces[myidx2(nu-1,nv-1,nu,nv)] = 1;
  }


  for(i=0; i<nvertices; i++) {
    vlist[i].faces = (Face **) malloc(sizeof(Face *) * vnfaces[i]);
    if(vlist[i].faces == NULL) {
      printf("Malloc failed. File: %s.\n", filename);
      qfree(&q);
      free_obj(obj);
      return 0;
    }
    bzero(vlist[i].faces, sizeof(Face *) * vnfaces[i]);
  }

  for(i=0; i<nfaces; i++) {
    flist[i].vertices = (Vertex **)malloc(sizeof(Vertex *) * 4);
    /* each face has 4 vertices */
    if(flist[i].vertices == NULL) {
      printf("Malloc failed. File: %s.\n", filename);
      qfree(&q);
      free_obj(obj);
      return 0;
    }
    bzero(flist[i].vertices, sizeof(Vertex *) * 4);
  }

  /* read vertices */
  for(i=0; i<nvertices; i++) {
    vlist[i].index = i;
    /* all inputs are treated as 4D */
    if(zmesh_flag == ON) {
      vlist[i].p4.d[0] = i % nu;
      vlist[i].p4.d[1] = i / nu;
    } else {
      vlist[i].p4.d[0] = dequeue(&q);
      vlist[i].p4.d[1] = dequeue(&q);
    }
    vlist[i].p4.d[2] = dequeue(&q);
    if(dim == 3)
      vlist[i].p4.d[3] = 0.0;
    else
      vlist[i].p4.d[3] = dequeue(&q);
    if(normal_flag == ON) { /* normals are ignored later */
      vlist[i].normal.d[0] = dequeue(&q);
      vlist[i].normal.d[1] = dequeue(&q);
      vlist[i].normal.d[2] = dequeue(&q);
    }
    if(vcolor >= 1) {
      vlist[i].cfront.f[0] = dequeue(&q);
      vlist[i].cfront.f[1] = dequeue(&q);
      vlist[i].cfront.f[2] = dequeue(&q);
      vlist[i].cfront.f[3] = dequeue(&q);
      if(vcolor >= 2) {
	vlist[i].cback.f[0] = dequeue(&q);
	vlist[i].cback.f[1] = dequeue(&q);
	vlist[i].cback.f[2] = dequeue(&q);
	vlist[i].cback.f[3] = dequeue(&q);
      }
      else {
	vlist[i].cback.f[0] = 1.0 - vlist[i].cfront.f[0];
	vlist[i].cback.f[1] = 1.0 - vlist[i].cfront.f[1];
	vlist[i].cback.f[2] = 1.0 - vlist[i].cfront.f[2];
	vlist[i].cback.f[3] = 1.0;
      }
    }
    if(texture_flag == ON) {
      vlist[i].tex.d[0] = dequeue(&q);
      vlist[i].tex.d[1] = dequeue(&q);
      vlist[i].tex.d[2] = dequeue(&q);
    }
  }

  /* make faces */
  for(j=0, count=0; j<nj; j++) {   /* careful with count usage */
    for(i=0; i<ni; i++, count++) { /* careful with count usage */
      k00 = myidx(i,j,nu,nv);
      k01 = myidx(i,j+1,nu,nv);
      k10 = myidx(i+1,j,nu,nv);
      k11 = myidx(i+1,j+1,nu,nv);
      
      flist[count].index = count;
      flist[count].nvert = 4;
      flist[count].vertices[0] = &(vlist[k00]);
      vlist[k00].faces[vlist[k00].Nfaces] = &(flist[count]);
      vlist[k00].Nfaces++;
      flist[count].vertices[1] = &(vlist[k10]);;
      vlist[k10].faces[vlist[k10].Nfaces] = &(flist[count]);
      vlist[k10].Nfaces++;
      flist[count].vertices[2] = &(vlist[k11]);
      vlist[k11].faces[vlist[k11].Nfaces] = &(flist[count]);
      vlist[k11].Nfaces++;
      flist[count].vertices[3] = &(vlist[k01]);
      vlist[k01].faces[vlist[k01].Nfaces] = &(flist[count]);
      vlist[k01].Nfaces++;
    }
  }

  if(q.count > 0) {
    printf("File %s %d numbers left unread.\n", filename, q.count);
    qfree(&q);
  }
  
  *objret = obj;
  
#if DEBUG_LOAD
  print_obj(obj, filename);
#endif

  printf("File %s has been loaded\n", filename);
  return 1;
}

int loadDOT(char *filename, Obj **objret) 
{
  FILE *in;
  char str[1000], *str_ptr, *end_str;
  int vcolor = 0;
  int dim = 3;
  int cformat = NOCOLOR;
  int nvertices;
  int size_flag;
  float size = 1.0;
  Vertex *vlist;
  Color4 cfront;
  Obj *obj;

  int i;
  double value;
  queue q;

  q.count = 0;

  if((in = fopen(filename, "r")) == NULL) {
    printf("Couldn't open file %s.\n", filename);
    return 0;
  }

  /* parse the header */
  while(fgets(str, 1000, in) != NULL && str[0] == '#');
  str_ptr = str;
  
  if(*str_ptr == 'S') {
    size_flag = ON;
    str_ptr++;
  }

  if(*str_ptr == 'C') {
    vcolor = 1;
    str_ptr++;
  }

  if(*str_ptr == '3') {
    str_ptr++;
  } else if(*str_ptr == '4') {
    dim = 4;
    str_ptr++;
  }

  if(!strncmp(str_ptr, "DOT", 3)) {
    str_ptr += 3;
    if(*str_ptr == 'C' || *str_ptr == 'c') {
      str_ptr++;
      cformat = UNIFORM1;
    }
  } else {
 /*    printf("File: %s Invalid DOT format.\n", filename); */
    return 0;
  }
  
  /* put all values into a queue */
  while(fgets(str, 1000, in) != NULL) {
    str_ptr = str;
    for(;;) {
      value = strtod(str_ptr, &end_str);
      if(str_ptr == end_str) break;
      str_ptr = end_str;
      enqueue(value, &q);
    }
  }
  fclose(in);

  /* check if there is enough values for object color and #points */
  if(q.count < (cformat == UNIFORM1 ? 4 : 0) + (size_flag == ON ? 1 : 0) +1){
    printf("File %s. Not enough values.\n", filename);
    qfree(&q);
    return 0;
  }
  
  /* read in object color(s) */
  if(cformat == UNIFORM1) {
    cfront.f[0] = dequeue(&q);
    cfront.f[1] = dequeue(&q);
    cfront.f[2] = dequeue(&q);
    cfront.f[3] = dequeue(&q);
  }

  if(size_flag == ON) size = dequeue(&q);
  if(size <= 0) size = 1.0;

  nvertices = (int)dequeue(&q);

  /* check if there's enough values for vertices in a queue */
  if(q.count < nvertices * (dim  + (vcolor == 1 ? 4 : 0))) {
    printf("File: %s. Not enough values.\n", filename);
    qfree(&q);
    return 0;
  }

  /* allocate and zero out memory */
  obj = (Obj *)malloc(sizeof(Obj));
  vlist = (Vertex*)malloc(sizeof(Vertex) * nvertices);

  if(obj == NULL || vlist == NULL) {
    printf("File: %s. Malloc failed.\n", filename);
    qfree(&q);
    myfree(obj);
    myfree(vlist);
    return 0;
  }
  
  bzero(obj, sizeof(Obj));
  bzero(vlist,sizeof(Vertex) * nvertices);
  
  /* Initialize object */
  obj->gformat = DOT;
  obj->cformat = cformat;
  obj->cfront = cfront;
  obj->texture_flag = OFF;
  obj->vcolor = vcolor;
  obj->fcolor = OFF;
  obj->nvertices = nvertices;
  obj->nfaces = 0;
  obj->size_flag = size_flag;
  obj->size = size;
  obj->vlist = vlist;
  obj->scale = 1.0;

  /* read vertices */
  for(i = 0; i < nvertices; i++) {
    vlist[i].index = i;
    /* all inputs are treated as 4D */
    vlist[i].p4.d[0] = dequeue(&q);
    vlist[i].p4.d[1] = dequeue(&q);
    vlist[i].p4.d[2] = dequeue(&q);
    if(dim == 3)
      vlist[i].p4.d[3] = 0.0;
    else
      vlist[i].p4.d[3] = dequeue(&q);
    if(vcolor == 1) {
      vlist[i].cfront.f[0] = dequeue(&q);
      vlist[i].cfront.f[1] = dequeue(&q);
      vlist[i].cfront.f[2] = dequeue(&q);
      vlist[i].cfront.f[3] = dequeue(&q);
    }
  }

  if(q.count > 0) {
    printf("File %s %d numbers left unread.\n", filename, q.count);
    qfree(&q);
  }

  *objret = obj;
  
#if DEBUG_LOAD
  print_obj(obj, filename);
#endif
  
  printf("File %s has been loaded\n", filename);
  return 1;
}

int loadLINE(char *filename, Obj **objret) 
{
  FILE *in;
  char str[1000], *str_ptr, *end_str;
  int vcolor = 0;
  int dim = 3;
  int gformat;
  int cformat = NOCOLOR;
  int nvertices, nfaces;
  int size_flag;
  float size = 1.0;
  Vertex *vlist;
  Face *flist;    /* faces are used to hold lines */
  Color4 cfront;
  Obj *obj;

  int i, j, tmp_value;
  double value;
  queue q;

  q.count = 0;

  if((in = fopen(filename, "r")) == NULL) {
    printf("Couldn't open file %s.\n", filename);
    return 0;
  }

  /* parse the header */
  while(fgets(str, 1000, in) != NULL && str[0] == '#');
  str_ptr = str;
  
  if(*str_ptr == 'S') {
    size_flag = ON;
    str_ptr++;
  }

  if(*str_ptr == 'C') {
    vcolor = 1;
    str_ptr++;
  }

  if(*str_ptr == '3') {
    str_ptr++;
  } else if(*str_ptr == '4') {
    dim = 4;
    str_ptr++;
  }

  if(!strncmp(str_ptr, "LINE", 4)) {
    str_ptr += 4;
    gformat = LINE;
  } else {
/*     printf("File: %s Invalid LINE format.\n", filename); */
    return 0;
  }
  
  if(!strncmp(str_ptr, "_LOOP", 5)) {
    str_ptr += 5;
    gformat = LINE_LOOP;    
  } else if(!strncmp(str_ptr, "_STRIP", 6)) {
    str_ptr += 6;
    gformat = LINE_STRIP;
  }

  if(*str_ptr == 'C' || *str_ptr == 'c') {
    str_ptr++;
    cformat = UNIFORM1;
  }

  /* put all values into a queue */
  while(fgets(str, 1000, in) != NULL) {
    str_ptr = str;
    for(;;) {
      value = strtod(str_ptr, &end_str);
      if(str_ptr == end_str) break;
      str_ptr = end_str;
      enqueue(value, &q);
    }
  }
  fclose(in);

  /* check if there is enough values for object color and #points */
  if(q.count<(cformat == UNIFORM1 ? 4 : 0)+(size_flag == ON ? 1 : 0)+2){
    printf("File %s. Not enough values.\n", filename);
    qfree(&q);
    return 0;
  }
  
  /* read in object color(s) */
  if(cformat == UNIFORM1) {
    cfront.f[0] = dequeue(&q);
    cfront.f[1] = dequeue(&q);
    cfront.f[2] = dequeue(&q);
    cfront.f[3] = dequeue(&q);
  }

  if(size_flag == ON) size = dequeue(&q);
  if(size <= 0) size = 1.0;

  nvertices = (int)dequeue(&q);
  nfaces = (int)dequeue(&q);

  /* check if there's enough values for vertices in a queue */
  if(q.count<nvertices*(dim  + (vcolor == 1 ? 4 : 0))+(gformat == LINE ? nfaces * 2 : 0)) {
    printf("File: %s. Not enough values.\n", filename);
    qfree(&q);
    return 0;
  }

  /* allocate and zero out memory */
  obj = (Obj *)malloc(sizeof(Obj));
  vlist = (Vertex*)malloc(sizeof(Vertex) * nvertices);
  flist = (Face*) malloc(sizeof(Face) * nfaces);

  if(obj == NULL || vlist == NULL) {
    printf("File: %s. Malloc failed.\n", filename);
    qfree(&q);
    myfree(obj);
    myfree(vlist);
    myfree(flist);
    return 0;
  }
  
  bzero(obj, sizeof(Obj));
  bzero(vlist,sizeof(Vertex) * nvertices);
  
  /* Initialize object */
  obj->gformat = gformat;
  obj->cformat = cformat;
  obj->cfront = cfront;
  obj->texture_flag = OFF;
  obj->vcolor = vcolor;
  obj->fcolor = OFF;
  obj->nvertices = nvertices;
  obj->nfaces = nfaces;
  obj->size_flag = size_flag;
  obj->size = size;
  obj->vlist = vlist;
  obj->flist = flist;
  obj->scale = 1.0;

  /* read vertices */
  for(i = 0; i < nvertices; i++) {
    vlist[i].index = i;
    /* all inputs are treated as 4D */
    vlist[i].p4.d[0] = dequeue(&q);
    vlist[i].p4.d[1] = dequeue(&q);
    vlist[i].p4.d[2] = dequeue(&q);
    if(dim == 3)
      vlist[i].p4.d[3] = 0.0;
    else
      vlist[i].p4.d[3] = dequeue(&q);
    if(vcolor == 1) {
      vlist[i].cfront.f[0] = dequeue(&q);
      vlist[i].cfront.f[1] = dequeue(&q);
      vlist[i].cfront.f[2] = dequeue(&q);
      vlist[i].cfront.f[3] = dequeue(&q);
    }
  }

  if(gformat != LINE_LOOP && gformat != LINE_STRIP) {

    /* read lines */
    for(i = 0; i < nfaces; i++) {
      
      flist[i].index = i;
      flist[i].nvert = 2;
      
      /* allocate and zero out memory for vertex array  */
      if((flist[i].vertices = (Vertex **)malloc(sizeof(Vertex*) * 2)) == NULL) {
	printf("File %s. Malloc failed.\n", filename);
	qfree(&q);
	free_obj(obj);
	return 0;
      }
      bzero(flist[i].vertices, sizeof(Vertex*) * 2);
      
      /* read vertex indices for the current face */
      for(j = 0; j < 2; j++) {
	tmp_value = (int)dequeue(&q);
	if(tmp_value < 0 || tmp_value >=  nvertices){
	printf("File: %s. Vertex index is out of range.\n", filename);
	qfree(&q);
	free_obj(obj);
	return 0;
	}
	flist[i].vertices[j] = &vlist[tmp_value];
      }
    }
  }

  if(q.count > 0) {
    printf("File %s %d numbers left unread.\n", filename, q.count);
    qfree(&q);
  }

  *objret = obj;
  
#if DEBUG_LOAD
  print_obj(obj, filename);
#endif
  
  printf("File %s has been loaded\n", filename);
  return 1;
}
