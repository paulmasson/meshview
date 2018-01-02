#define UI_QUATROT_C
#include "all.h"

Dimension glwquat_width, glwquat_height;
Widget glwquat, quatLabel, dispQBtn, printpQBtn;
GLXContext quat_context;
Window quat_win;
int displ_quat, print_quat;
double quat[4], dq[4];
double quatR[9];

void compute_quatR()
{
  quatR[0] = quat[3]*quat[3]+quat[0]*quat[0]-quat[1]*quat[1]-quat[2]*quat[2];
  quatR[1] = 2*quat[0]*quat[1]+2*quat[3]*quat[2];
  quatR[2] = 2*quat[0]*quat[2]-2*quat[3]*quat[1];

  quatR[3] = 2*quat[0]*quat[1]-2*quat[3]*quat[2];
  quatR[4] = quat[3]*quat[3]-quat[0]*quat[0]+quat[1]*quat[1]-quat[2]*quat[2];
  quatR[5] = 2*quat[1]*quat[2]+2*quat[3]*quat[0];

  quatR[6] = 2*quat[0]*quat[2]+2*quat[3]*quat[1];
  quatR[7] = 2*quat[1]*quat[2]-2*quat[3]*quat[0];
  quatR[8] = quat[3]*quat[3]-quat[0]*quat[0]-quat[1]*quat[1]+quat[2]*quat[2];
}

void quat_init()
{
  quat_momentum = OFF;
  quat[3] = 1.0;
  quat[0] = quat[1] = quat[2] = 0.0;
  compute_quatR();
  displ_quat = True;
  print_quat = True;
  XmToggleButtonSetState(dispQBtn, displ_quat, False);
  XmToggleButtonSetState(printpQBtn, print_quat, False);
}

void quat_draw()
{
  GLenum glerr;
  char str[200];

  /* set lable */
  if(print_quat == True) {
    sprintf(str, 
	    " q = [ % 4.2f % 4.2f % 4.2f % 4.2f ]\n"
	    "\n"
	    "     | % 4.2f % 4.2f % 4.2f |\n"
	    " R = | % 4.2f % 4.2f % 4.2f |\n"
	    "     | % 4.2f % 4.2f % 4.2f |\n",
	    quat[3], quat[0], quat[1], quat[2], 
	    quatR[0], quatR[3], quatR[6], 
	    quatR[1], quatR[4], quatR[7],
	    quatR[2], quatR[5], quatR[8]);
    XtVaSetValues (quatLabel,
		   XtVaTypedArg, XmNlabelString, XmRString,str,200,NULL);
  }
  
  glXMakeCurrent(dpy, quat_win, quat_context);

  /* draw axes */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLineWidth(2.0);

  glBegin(GL_LINES);
  glColor4fv(red);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(quatR[0], quatR[1], quatR[2]);
  glColor4fv(green);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(quatR[3], quatR[4], quatR[5]);
  glColor4fv(blue);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(quatR[6], quatR[7], quatR[8]);
  glEnd();

  /* draw quat vector */
  if(displ_quat == True) {
    glLineWidth(4.0);
    glColor4fv(yellow);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(quat[0], quat[1], quat[2]);
    glEnd();
  }

  while((glerr = glGetError()) != GL_NO_ERROR)
    printf("GL ERROR: %s\n", gluErrorString(glerr));
  
  glXSwapBuffers(dpy, quat_win);

  glXMakeCurrent(dpy, main_win, glx_context);
}

/*ARGSUSED*/
void quatrot_reset(Widget w, XtPointer client_data, XtPointer call_data)
{
  quat_init();
  quat_draw();
} 

/*ARGSUSED*/
void quat_displ(Widget w, XtPointer client_data, XtPointer call_data) 
{
  XmToggleButtonCallbackStruct *cbs = 
    (XmToggleButtonCallbackStruct *) call_data;

  if(cbs->set == True)
    displ_quat = True;
  else 
    displ_quat = False;
  quat_draw();
}

/*ARGSUSED*/
void quat_print(Widget w, XtPointer client_data, XtPointer call_data)
{
  XmToggleButtonCallbackStruct *cbs = 
    (XmToggleButtonCallbackStruct *) call_data;

  if(cbs->set == True)
    print_quat = True;
  else {
    print_quat = False;
    XtVaSetValues(quatLabel, XtVaTypedArg, XmNlabelString, XmRString,
		  " \n \n \n \n \n", 10, NULL);
  }
  quat_draw();
}

/*ARGSUSED*/
void initQuat(Widget w, XtPointer client_data, XtPointer call_data)
{
  Arg args[10];
  XVisualInfo *vi;
  GLwDrawingAreaCallbackStruct *cbs = 
    (GLwDrawingAreaCallbackStruct *) call_data;
  glwquat_width = (double) cbs->width;
  glwquat_height = (double) cbs->height;
  
  XtSetArg(args[0], GLwNvisualInfo, &vi);
  XtGetValues(w, args, 1);
  quat_win = XtWindow(w);
  quat_context = glXCreateContext(dpy, vi, 0, GL_TRUE);

  glXMakeCurrent(dpy, quat_win, quat_context);

  glViewport (0, 0, (GLsizei) cbs->width, (GLsizei) cbs->height);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); /* default */
  glClearDepth(1000.0);
  glDisable(GL_LIGHTING);
  glClearColor(0.0, 0.0, 0.0, 1.0);

  if(!glIsEnabled(GL_DEPTH_TEST))
     printf("Warning : GL_DEPTH_TEST can't be enabled.\n");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 3.0);
  glTranslatef(0.0, 0.0, -2.0);
/*   glRotatef(-10.0, 0.0, 1.0, 0.0); */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glXMakeCurrent(dpy, main_win, glx_context); 
}


/* ARGSUSED */
void exposeQuat(Widget w, XtPointer ptr, XtPointer call_data)
{
  quat_draw();
} 

/* ARGSUSED */
void resizeQuat (Widget w, XtPointer ptr, XtPointer call_data)
{
  GLwDrawingAreaCallbackStruct *cbs =
    (GLwDrawingAreaCallbackStruct *) call_data;
  glwquat_width = (double) cbs->width;
  glwquat_height = (double) cbs->height;
  
  glXMakeCurrent(dpy, quat_win, quat_context);
  glViewport (0, 0, (GLsizei) cbs->width, (GLsizei) cbs->height);
  glXMakeCurrent(dpy, main_win, glx_context); 
  
  quat_draw();
}

void transform_quat()
{
  double temp[4];

  temp[0] = 
    quat[3]*dq[0]+dq[3]*quat[0]-quat[1]*dq[2]+quat[2]*dq[1];
  temp[1] = 
    quat[3]*dq[1]+dq[3]*quat[1]-quat[2]*dq[0]+quat[0]*dq[2];
  temp[2] = 
    quat[3]*dq[2]+dq[3]*quat[2]-quat[0]*dq[1]+quat[1]*dq[0];
  temp[3] = 
    quat[3]*dq[3]-quat[0]*dq[0]-quat[1]*dq[1]-quat[2]*dq[2];
  
  quat[0]=temp[0];
  quat[1]=temp[1];
  quat[2]=temp[2];
  quat[3]=temp[3];
  compute_quatR();
  quat_draw();  
  
  mat4 = Matrix4TimesMatrix4(&d_mat4, &mat4);
  From4DTo3D();
  if(picker_flag == ON) setPicker ();
  AutoSmooth();
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void motionQuat(Widget widget, XEvent *event, String *args, int *num_args) 
{
  static int oldx, oldy;
  int newx, newy;
  double dr, dx, dy, D, R = 200.0;

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

  } else if(!strcmp(args[0], "Up")) {

    if(abs(dmousex)+abs(dmousey) > 2*m_sense)
      quat_momentum = ON;
    
  } else if(!strcmp(args[0], "Motion")) {

    dmousex = newx - oldx;
    dmousey = oldy - newy; /* window coordinate system is upside down */
    oldx = newx;
    oldy = newy;
    /* sensitivity */
    if(abs(dmousex)+abs(dmousey) <= m_sense) return;

    dx = dmousex * m_speed;
    dy = dmousey * m_speed;
    
    if(button_flag == Button1) {
      dr = sqrt(dx*dx+dy*dy);  
      D = sqrt(dr*dr + R*R);
      dq[3] = R/D;
      dq[0] = -dy/D;
      dq[1] = dx/D;
      dq[2] = 0;
    } else {
      D = sqrt(dx*dx + R*R);
      dq[3] = R/D;
      dq[0] = 0;
      dq[1] = 0;
      dq[2] = dx/D;
    }

    d_mat4.d[0] =  dq[3];
    d_mat4.d[1] =  dq[2];
    d_mat4.d[2] = -dq[1];
    d_mat4.d[3] = -dq[0];
  
    d_mat4.d[4] = -dq[2];
    d_mat4.d[5] =  dq[3];
    d_mat4.d[6] =  dq[0];
    d_mat4.d[7] = -dq[1];
  
    d_mat4.d[8] =  dq[1];
    d_mat4.d[9] = -dq[0];
    d_mat4.d[10] = dq[3];
    d_mat4.d[11] =-dq[2];
    
    d_mat4.d[12] = dq[0];
    d_mat4.d[13] = dq[1];
    d_mat4.d[14] = dq[2];
    d_mat4.d[15] = dq[3];
    
    transform_quat();
  }
}

/* ARGSUSED */
void allResetQuat(Widget widget, XEvent *event, String *args, int *num_args) 
{
  if(!strcmp(args[0], "All"))
    reset(widget, event, args, num_args);
  quat_init();
  quat_draw();
} 

/* build quatrot panel */
void build_uiQuatrot(Widget menuBar) 
{
  int n;
  Arg args[10];
  Widget pane, frame;
  Widget temp;
  String translationsQuat =
    "<Btn1Motion>     : motionQuat(Motion) \n"
    "<Btn1Down>       : motionQuat(Down)   \n"
    "<Btn1Up>         : motionQuat(Up)     \n"
    "<Btn2Motion>     : motionQuat(Motion) \n"
    "<Btn2Down>       : motionQuat(Down)   \n"
    "<Btn2Up>         : motionQuat(Up)     \n"
    "~Shift<Btn3Down> : allResetQuat(Panel)\n"
    "Shift<Btn3Down>  : allResetQuat(All)  \n";

  XtActionsRec actionsQuat[] = {
    { "motionQuat",   (XtActionProc) motionQuat },
    { "allResetQuat", (XtActionProc) allResetQuat },
  };
  
  /* Add the "motionQuat" action/function used by the DrawingArea widget */
  XtAppAddActions(app_context, actionsQuat, XtNumber(actionsQuat));

  n = 0;
  XtSetArg(args[n], XmNtitle, "quaternion rotation"); n++;
  quatrotDialog = XmCreateMessageDialog(menuBar, "quatrotDialog", args, n);
  temp = XmMessageBoxGetChild (quatrotDialog, XmDIALOG_CANCEL_BUTTON);
  XtUnmanageChild (temp);
  XtVaSetValues(quatrotDialog,
		XtVaTypedArg, XmNokLabelString, XmRString, "Close", 6,
		XtVaTypedArg, XmNhelpLabelString, XmRString, "Reset", 6,
		NULL);
  XtAddCallback(quatrotDialog, XmNhelpCallback, quatrot_reset, 0);

  pane = XtVaCreateManagedWidget("pane", xmPanedWindowWidgetClass,
				 quatrotDialog, 
				 XmNsashWidth, 10,
				 XmNsashHeight, 10,
				 XmNallowResize, True,
				 NULL);
  frame = XmCreateFrame(pane, "frame", NULL, 0);
  XtManageChild(frame);
  
  /* create a glw drawing area */
  n = 0;
  XtSetArg(args[n], GLwNrgba, TRUE); n++;
  XtSetArg(args[n], XmNtranslations,
	   XtParseTranslationTable(translationsQuat)); n++;
  glwquat = GLwCreateMDrawingArea(frame, "glwquat", args, n);
  XtManageChild (glwquat);
  XtAddCallback(glwquat, GLwNginitCallback, initQuat, (XtPointer) NULL);
  XtAddCallback(glwquat, GLwNexposeCallback, exposeQuat, (XtPointer) NULL);
  XtAddCallback(glwquat, GLwNresizeCallback, resizeQuat, (XtPointer) NULL);

  dispQBtn = XtVaCreateManagedWidget("dispQBtn",
				     xmToggleButtonWidgetClass, pane,
				     XtVaTypedArg, XmNlabelString, 
				     XmRString, 
				     "Display Quarternion", 25,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNtopAttachment, XmATTACH_FORM,
				     XmNbottomAttachment, XmATTACH_FORM,
				     NULL);
  XtAddCallback(dispQBtn, XmNvalueChangedCallback, quat_displ, NULL);

  quatLabel = XmCreateLabel(pane, "quatrotLabel", NULL, 0);
  XtManageChild(quatLabel);
  /* create space of 5 lines */
  XtVaSetValues(quatLabel, XtVaTypedArg, XmNlabelString, XmRString,
		" \n \n \n \n \n", 10, NULL);

  printpQBtn = XtVaCreateManagedWidget("printpQBtn",
				       xmToggleButtonWidgetClass, pane,
				       XtVaTypedArg, XmNlabelString, 
				       XmRString, 
				       "Print Quarternion", 25,
				       XmNrightAttachment, XmATTACH_FORM,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNbottomAttachment, XmATTACH_FORM,
				       NULL);
  XtAddCallback(printpQBtn, XmNvalueChangedCallback, quat_print, NULL);

}
