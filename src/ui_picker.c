#define UI_PICKER_C
#include "all.h"

Dimension dawpo_width, dawpo_height;
Widget pickerLabel;
GC po_gc;
int pk, px, py;


/*ARGSUSED*/
void picker_close(Widget w, XtPointer client_data, XtPointer call_data) {
  picker_flag = OFF;
  XtUnmanageChild(pickerDialog);
  draw_all();
}

void setPicker() { /* need pi, pj, pk, pickerP3, pickerP4 */
  Obj *obj;
  int k, nu, nv;
  double du, dv;
  Vertex *v00, *v01, *v10, *v11;
  char s[200];
  XmString text;
  Point3 p3;
  float pi, pj;
  float scale;

  obj = olist[pk];
  nu = obj->nu;
  nv = obj->nv;
  if(obj->u_flag == ON)    pi = (double) nu * px / dawpo_width;
  else                     pi = (double) (nu-1) * px / dawpo_width;
  if(obj->v_flag == ON)    pj = (double) nv * py / dawpo_height;
  else                     pj = (double) (nv-1) * py / dawpo_height;

  du = pi - (int) pi;
  dv = pj - (int) pj;
  v00 = &(obj->vlist[myidx((int)pi,(int)pj,nu,nv)]);
  v01 = &(obj->vlist[myidx((int)pi+1,(int)pj,nu,nv)]);
  v10 = &(obj->vlist[myidx((int)pi,(int)pj+1,nu,nv)]);
  v11 = &(obj->vlist[myidx((int)pi+1,(int)pj+1,nu,nv)]);
  for(k=0; k<4; k++) 
    pickerP4.d[k] = v00->p4.d[k] * (1.0 - du) * (1.0 - dv) 
      + v01->p4.d[k] * du * (1.0 - dv) 
	+ v10->p4.d[k] * (1.0 - du) * dv 
	  + v11->p4.d[k] * du * dv;
  for(k=0; k<3; k++) 
    pickerP3.d[k] = v00->p3.d[k] * (1.0 - du) * (1.0 - dv) 
      + v01->p3.d[k] * du * (1.0 - dv) 
	+ v10->p3.d[k] * (1.0 - du) * dv 
	  + v11->p3.d[k] * du * dv;

  p3 = Matrix3TimesPoint3(&rot_mat, &pickerP3);
  scale = obj->scale;
  sprintf(s, "u = %.2f, v = %.2f\n3D: %.2f %.2f %.2f\n4D: %.2f %.2f %.2f %.2f",
	  pi, pj, p3.d[0]/scale, p3.d[1]/scale, p3.d[2]/scale, 
	  pickerP4.d[0], pickerP4.d[1], pickerP4.d[2], pickerP4.d[3]
	  );
  text = XmStringCreateLtoR(s, "smallset");
  XtVaSetValues(pickerLabel, XmNlabelString, text, NULL);
}

void drawPoLines() {
  XClearWindow(dpy, po_win);
  XDrawLine(dpy, po_win, po_gc, px, 0, px, dawpo_height);
  XDrawLine(dpy, po_win, po_gc, 0, dawpo_height-py-1, 
	    dawpo_width, dawpo_height-py-1);
}

void resetPicker()
{
  px = py = 0;
  pk = 0;
  XtVaSetValues(objNumScale, XmNvalue, pk, NULL);
  setPicker();
  drawPoLines(); 
} 

/* ARGSUSED */
void motionPo(Widget widget, XEvent *event, String *args, int *num_args) {

  XButtonEvent *Bevent = (XButtonEvent *) event;
  
  if (*num_args != 1)
    XtError("Wrong number of args!");

  px = min(dawpo_width, max(Bevent->x, 0));
  py = min(dawpo_height, max(dawpo_height-Bevent->y, 0.0));

  drawPoLines();
  setPicker();
  draw_all();
}

/* ARGSUSED */
void exposePo(Widget w, XtPointer ptr, XtPointer call_data) {
  
  static int first_time = True;
  XmDrawingAreaCallbackStruct *cbs =
    (XmDrawingAreaCallbackStruct *) call_data;
  XEvent *event = cbs->event;

  if(first_time == True) { /* needed for drawPoLines() */
    po_win = event->xexpose.window;
    px = py = 0;
    pk = 0;
    first_time = False;
  }
  drawPoLines();
} 

/* ARGSUSED */
void resizePo (Widget w, XtPointer ptr, XtPointer call_data) {
 
  XtVaGetValues (w, XmNwidth,&dawpo_width,XmNheight,&dawpo_height,NULL);
  drawPoLines();
} 

/* ARGSUSED */
void ChangeCubeSize(Widget w, XtPointer client_data, XtPointer call_data) {

  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  cubesize = (double) cbs->value * 0.1;
  create_cube_dlist(0.03*cubesize, PICKER_CUBE);
  draw_all();
}

/* ARGSUSED */
void ChangeObjNum(Widget w, XtPointer client_data, XtPointer call_data) {

  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  pk = cbs->value;
  setPicker();
  draw_all();
}

/* build picker panel */
void build_uiPicker(Widget menuBar) 
{
  int n;
  Arg args[10];
  Widget pane, frame, rowcol, pickerHelpBox, cubeSizeScale;
  Widget temp;
  XColor col_out, dummy;
  XGCValues gcv;
  int gc_flags = 0;
  XmString text;

  String translationsPo = /* for the DrawingArea widget */
    "<Btn1Motion> : motionPo(MotionPo)    \n"
    "<Btn1Down>   : motionPo(Down)";
  XtActionsRec actionsPo = { "motionPo", (XtActionProc) motionPo };

  /* Add the "motionPo" action/function used by the DrawingArea widget */
  XtAppAddActions(app_context, &actionsPo, 1);

  n = 0;
  XtSetArg(args[n], XmNtitle, "picker"); n++;
  pickerDialog = XmCreateMessageDialog(menuBar, "pickerDialog", args, n);
  temp = XmMessageBoxGetChild (pickerDialog, XmDIALOG_CANCEL_BUTTON);
  XtUnmanageChild (temp);
  XtVaSetValues(pickerDialog,
		XtVaTypedArg, XmNokLabelString, XmRString, "Close", 6,
		NULL);
  XtAddCallback(pickerDialog, XmNokCallback, picker_close, 0);

  n = 0;  /* pickerHelpBox */
  XtSetArg(args[n], XmNtitle, "picker help"); n++;
  pickerHelpBox = XmCreateMessageDialog(pickerDialog, "pickerHelpBox", 
					args, n);
  temp = XmMessageBoxGetChild (pickerHelpBox, XmDIALOG_CANCEL_BUTTON);
  XtUnmanageChild (temp);
  temp = XmMessageBoxGetChild (pickerHelpBox, XmDIALOG_HELP_BUTTON);
  XtUnmanageChild (temp);
  XtAddCallback(pickerDialog, XmNhelpCallback, ShowBox, pickerHelpBox);
  /* pickerHelpList, global, initialized in ui_init() */
  pickerHelpList = XmCreateScrolledList(pickerHelpBox, "pickerHelpList", 
					NULL, 0);
  XtManageChild(pickerHelpList);  

  pane = XtVaCreateManagedWidget("pane", xmPanedWindowWidgetClass,
				 pickerDialog, 
				 XmNsashWidth, 1,
				 XmNsashHeight, 1,
				 NULL);
  frame = XmCreateFrame(pane, "frame", NULL, 0);
  XtManageChild(frame);

  /* Create a GC for drawing (callback). */
  if(!XAllocNamedColor(dpy, cmap, "Crimson", &col_out, &dummy)) {
    printf("XAllocColor: Couldn't allocate crimson.\n");
  } else {
    gcv.foreground = col_out.pixel;
    gc_flags = GCForeground;
  }
  po_gc = XCreateGC (dpy, RootWindowOfScreen(scr), gc_flags, &gcv);

  /* create a drawing area */  
  dawpo = XtVaCreateManagedWidget("dawpo",
				  xmDrawingAreaWidgetClass, frame,
				  XmNtranslations, 
				  XtParseTranslationTable(translationsPo),
				  NULL);
  XtAddCallback(dawpo, XmNexposeCallback, exposePo, (XtPointer) NULL);
  XtAddCallback(dawpo, XmNresizeCallback, resizePo, (XtPointer) NULL);
  XtVaGetValues(dawpo, XmNwidth,&dawpo_width,XmNheight,&dawpo_height,NULL);  

  pickerLabel = XmCreateLabel(pane, "pickerLabel", NULL, 0);
  XtManageChild(pickerLabel);
  /* create space of 3 lines */
  text = XmStringCreateLtoR(" \n \n ", "smallset");
  XtVaSetValues(pickerLabel, XmNlabelString, text, NULL);

  rowcol = XtVaCreateManagedWidget ("rowcol",
		xmRowColumnWidgetClass, pane,
		XmNorientation, XmVERTICAL,
		NULL);
  cubeSizeScale = XtVaCreateManagedWidget ("cubeSizeScale",
	xmScaleWidgetClass, rowcol,
	XtVaTypedArg, XmNtitleString, XmRString, "size of cube", 20,
	XmNmaximum,   100,
	XmNminimum,   1,
	XmNvalue,   (int) cubesize * 10,
	XmNdecimalPoints, 1,
        XmNscaleMultiple, 2,
	XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,				   
        NULL);
  XtAddCallback(cubeSizeScale, XmNdragCallback, ChangeCubeSize, NULL);
  XtAddCallback(cubeSizeScale, XmNvalueChangedCallback, ChangeCubeSize, NULL);

  objNumScale = XtVaCreateManagedWidget ("objNumScale",
	xmScaleWidgetClass, rowcol,
	XtVaTypedArg, XmNtitleString, XmRString, "No. of objects", 30,
	XmNminimum,   0,
	XmNmaximum,   10,
	XmNvalue,   0,
	XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,				   
        NULL);
  XtAddCallback(objNumScale, XmNdragCallback, ChangeObjNum, NULL);
  XtAddCallback(objNumScale, XmNvalueChangedCallback, ChangeObjNum, NULL);
}
