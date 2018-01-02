/* initCB, exposeCB, resizeCB, 
   and mouse press/release/movement CB motion() */

#define ACTION_C
#include "all.h"

/* momentumProc() is called by the WorkProc. When the scene
 * is in automatic motion, the WorkProc calls this routine.
 * It returns(FALSE) so the WorkProc does not discontinue
 * operation.
 */
/* ARGSUSED */
Boolean momentumProc(XtPointer ptr)
{
  int sleep_flag = 1;

  if(mouse3d_flag == ON) {
    mouse3d_interact();
    sleep_flag = 0;
  } else if(momentum_mode != OFF) {
    if(quat_momentum != OFF)
      transform_quat();
    if(momentum != OFF)
      transform();
  } 
  
  if(frames_mode == ON && frames_transform_flag == ON) {
    frames_play();
    sleep_flag = 0;
  }

  if(sleep_flag)
    usleep(10000); /* suspend for 1/100 second */

  return(FALSE);
}

/* ARGSUSED */
void stop(Widget widget, XEvent *event, String *args, int *num_args)
{
  momentum = OFF;
  quat_momentum = OFF;
}

/* ARGSUSED */
void reset(Widget widget, XEvent *event, String *args, int *num_args)
{
  Reset();
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void center(Widget widget, XEvent *event, String *args, int *num_args)
{
  momentum = OFF;
  quat_momentum = OFF;
  d_rot_mat = IMatGL;
  rot_mat = IMatGL;
  trans_mat = IMatGL;
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if(picker_flag == ON) setPicker();
  draw_all();
}

/* ARGSUSED */
void motion(Widget widget, XEvent *event, String *args, int *num_args) 
{
  static int oldx, oldy, newx, newy;
  XButtonEvent *Bevent = (XButtonEvent *) event;

  if (*num_args != 1)
    XtError("Wrong number of args!");
  newx = Bevent->x;
  newy = Bevent->y;
  
  if(!strcmp(args[0], "Down")) {

    quat_momentum = OFF;
    momentum = OFF;
    dmousex = 0;
    dmousey = 0;
    button_flag = Bevent->button;
    oldx = newx;
    oldy = newy;
  }
  else if(!strcmp(args[0], "Up")) {
    if(abs(dmousex)+abs(dmousey) > 2*m_sense)
      momentum = ON;
  }
  else if(!strcmp(args[0], "Motion")) {
    dmousex = newx - oldx;
    dmousey = oldy - newy; /* window coordinate system is upside down */
    oldx = newx;
    oldy = newy;
    /* sensitivity */
    if(abs(dmousex)+abs(dmousey) <= m_sense)
      return;
    motion_flag = NONE;
    if(button_flag == Button1)
      /* generate the 3D rotation matrix */
      MakeRot3((double)dmousex * m_speed, (double)dmousey * m_speed);
    transform();
  }
  else if(!strcmp(args[0], "CtrlMotion")) {
    dmousex = newx - oldx;
    dmousey = oldy - newy;
    oldx = newx;
    oldy = newy;
    /* sensitivity */
    if(abs(dmousex)+abs(dmousey) <= m_sense)
      return;
    motion_flag = CTRL;
    if(button_flag == Button1)
      /* generate the 3D rotation matrix */
      MakeRot3((double)dmousex * m_speed, (double)dmousey * m_speed);
    else if(button_flag == Button2)
      MakeRot3Z((double)dmousex * m_speed);
    transform();
  }
  else if(!strcmp(args[0], "ShiftMotion")) {
    dmousex = newx - oldx;
    dmousey = oldy - newy;
    oldx = newx;
    oldy = newy;
    /* sensitivity */
    if(abs(dmousex)+abs(dmousey) <= m_sense)
      return;
    motion_flag = SHIFT;
    switch(button_flag) {
    case Button1:
      MakeRot4((double)dmousex * m_speed, (double)dmousey * m_speed, 0.0);
      break;
    case Button2:
      MakeRot4((double)dmousex * m_speed, 0.0, (double) -dmousey * m_speed);
      break;
    case Button3: /* 3D rotation around z-axis */
      MakeRot3Z((double)dmousex * m_speed);
      break;
    }
    transform();
  }
}

/* makes a checker board texture image */
void makeCheckImage() 
{
  int i, j, c;  
  texImageHeight = 64;
  texImageWidth = 64;

  texImage = (GLubyte *) malloc(texImageHeight * texImageWidth * 4);
  
  for(i=0; i<texImageHeight; i++){
    for(j=0; j<texImageWidth; j++){
      c=(((i&0x8)==0)^((j&0x8)==0))*255;
      texImage[(i*texImageWidth + j)*4 + 0] = (GLubyte) c;
      texImage[(i*texImageWidth + j)*4 + 1] = (GLubyte) c;
      texImage[(i*texImageWidth + j)*4 + 2] = (GLubyte) c;
      texImage[(i*texImageWidth + j)*4 + 3] = (GLubyte) 255;
    }
  }
}

/* initCB  
 * The initCB subroutine initializes graphics modes and 
 * transformation matrices. 
 */  
/* ARGSUSED */
void initCB (Widget w, XtPointer client_data, XtPointer call_data)
{
  float s[4];
  const GLubyte *version;
  char *ptr;
  int major_num, minor_num;
  Arg args[1];
  XVisualInfo *vi; 
  GLwDrawingAreaCallbackStruct *call_ptr; 

  call_ptr = (GLwDrawingAreaCallbackStruct *) call_data;

  XtSetArg(args[0], GLwNvisualInfo, &vi);
  XtGetValues(w, args, 1); 
  main_win = XtWindow(w);
  glx_context = glXCreateContext(dpy, vi, 0, GL_TRUE);
/* The last argument above specifies whether rendering is to be done 
   with a direct connection to the graphics system if possible(True) 
   or through the Xserver (False) 
*/

  GLwDrawingAreaMakeCurrent (w, glx_context);

  /* check GL version. string returnes starts: 
     major_number.minor_number */
  version = glGetString(GL_VERSION);
  major_num = strtol ((char *)version, &ptr, 10);
  ptr++; /* skip '.' */
  minor_num = strtol (ptr, NULL, 10);

  if(major_num > 1 || major_num == 1 && minor_num >= 1)
    gl_version_1_1 = True;

  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); /* default */
  glClearDepth(1000.0);

  if(!glIsEnabled(GL_DEPTH_TEST))
     printf("Warning : GL_DEPTH_TEST can't be enabled.\n");

  glw_width = (int) (call_ptr->width);
  glw_height = (int) (call_ptr->height);
  aspect = (double)(glw_width) / (double)(glw_height); 
  myLoadProjection();

  /*-------------------*
   * Set the lighting  *
   * ------------------*/
  s[0] = mats[0] * specular;
  s[1] = mats[1] * specular;
  s[2] = mats[2] * specular;
  s[3] = 1.0;

  glMaterialfv (GL_FRONT_AND_BACK,GL_AMBIENT,mata);
  glMaterialfv (GL_FRONT_AND_BACK,GL_DIFFUSE,matd);
  glMaterialfv (GL_FRONT_AND_BACK,GL_SPECULAR,s);
  glMaterialf (GL_FRONT_AND_BACK,GL_SHININESS,shininess);
  
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_LIGHTING);

  glLightfv(GL_LIGHT0,GL_POSITION,ltp);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,ltc);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
 
  if((FACE_LIST = glGenLists(1)) == 0) {
    printf("Couldn't alocate FACE_LIST\n");
    exit(1);
  }
  if((EDGE_LIST = glGenLists(1)) == 0) {
    printf("Couldn't alocate EDGE_LIST\n");
    exit(1);
  }
  if((NORMAL_LIST = glGenLists(1)) == 0) {
    printf("Couldn't alocate NORMAL_LIST\n");
    exit(1);
  }
  if((VERTEX_LIST = glGenLists(1)) == 0) {
    printf("Couldn't alocate VERTEX_LIST\n");
    exit(1);
  }
  if((VERTEX_CUBE_LIST = glGenLists(1)) == 0) {
    printf("Couldn't alocate VERTEX_CUBE_LIST\n");
    exit(1);
  }
  if((UNIT_SPHERE_LIST = glGenLists(1)) == 0) {
    printf("Couldn't alocate VERTEX_CUBE_LIST\n");
    exit(1);
  }
  if((PO_CUBE_LIST = glGenLists(1)) == 0) {
    printf("Couldn't alocate PO_CUBE_LIST\n");
    exit(1);
  }
  
  makeCheckImage();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, texImageWidth, texImageHeight,
               0, GL_RGBA, GL_UNSIGNED_BYTE, texImage);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  
  workprocid = XtAppAddWorkProc(app_context, momentumProc, NULL);
}

/* exposeCB() and resizeCB() are called when the window
 * is uncovered, moved, or resized.
 */ 
/* ARGSUSED */
void exposeCB (Widget w, XtPointer ptr, XtPointer call_data)
{
  draw_all();
} 

/* ARGSUSED */
void resizeCB (Widget w, XtPointer ptr, XtPointer call_data)
{
  GLwDrawingAreaCallbackStruct *call_ptr;
  
  call_ptr = (GLwDrawingAreaCallbackStruct *) call_data;
  glw_width = (int) (call_ptr->width);
  glw_height = (int) (call_ptr->height);
  aspect = (double)(glw_width) / (double)(glw_height); /* global variable */

  glViewport (0, 0, (GLsizei) glw_width, (GLsizei) glw_height);
  
  myLoadProjection();
  draw_all();
}

