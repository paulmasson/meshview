#define UI_FRAMES_C
#include <sys/time.h>
#include "all.h"
#include "bitmaps.h" /* bitmaps for buttons */

enum {FR_REW = 1000, FR_PLAYR, FR_STEPR, FR_STOP, FR_PLAY, FR_STEP, FR_FWD};
enum {FR_ONEDIR = 1010, FR_TWODIR, FR_LOOP};
enum {SPLINE_CUBIC = 1120, SPLINE_LINEAR};
enum {FORWARD = 1130, REVERSE, SWITCH_N_WAIT};
enum {NONE_X = 1140, START_X, END_X, CUR_X};
enum {FR_START_L = 1051, FR_START_R, FR_END_L, FR_END_R , FR_CUR_L, FR_CUR_R};
enum {FR_BG, FR_FG, FR_GREEN, FR_BLUE, FR_RED, FR_YELLOW};
enum {FR_RATE, FR_TIME};

float fr_sta, fr_end, fr_cur, fr_speed, fr_time, fr_inttype;
int fr_direction, fr_splinetype, fr_playtype;
int new_limits_flag;


Widget playrBtn, playBtn;
Widget dir1Btn, dir2Btn, loopBtn;
Widget frIndicator, splineTypeMenu, frameRateScale, frameTimeScale, endIntScale;
Widget linearBtn, cubicBtn;
Widget staLbl, endLbl, curLbl;

float idle_interval;
double oldtime;
double idle_start;

int select_flag;
Window fr_win;

Dimension ind_width, ind_height;
Pixel pixs[6];
GC gc;

double getcurtime()
{
  struct timeval tp;
  if(gettimeofday(&tp, NULL) == -1) return -1;
  return tp.tv_sec + tp.tv_usec*0.000001;
}

void allocateColors()
{
  XColor ncolor, dummy;

  XtVaGetValues(framesDialog,
		XmNbackground, &pixs[FR_BG],
		XmNforeground, &pixs[FR_FG],
		NULL);

  if(!XAllocNamedColor(dpy, cmap, "green3", &ncolor, &dummy)) {
    printf("XAllocColor: Couldn't allocate green.\n");
    pixs[FR_GREEN] = pixs[FR_FG];
  } else {
    pixs[FR_GREEN] = ncolor.pixel;
  }

  if(!XAllocNamedColor(dpy, cmap,"blue", &ncolor, &dummy)) {
    printf("XAllocColor: Couldn't allocate blue.\n");
    pixs[FR_BLUE] = pixs[FR_FG];
  } else {
    pixs[FR_BLUE] = ncolor.pixel;
  }

  if(!XAllocNamedColor(dpy, cmap, "red", &ncolor, &dummy)) {
    printf("XAllocColor: Couldn't allocate red.\n");
    pixs[FR_RED] = pixs[FR_FG];
  } else {
    pixs[FR_RED] = ncolor.pixel;
  }
  
  if(!XAllocNamedColor(dpy, cmap, "yellow", &ncolor, &dummy)) {
    printf("XAllocColor: Couldn't allocate yellow.\n");
    pixs[FR_YELLOW] = pixs[FR_FG];
  } else {
    pixs[FR_YELLOW] = ncolor.pixel;
  }
}

Pixmap makePixmap(char *data, int width, int height, int color)
{
  return 
    XCreatePixmapFromBitmapData(dpy, RootWindowOfScreen(scr),
     data, width, height, pixs[color], pixs[FR_BG], DefaultDepthOfScreen(scr));
}

void frames_init()
{
  frames_transform_flag = OFF;

  select_flag = NONE_X;
  XmToggleButtonSetState(playBtn, False, False);
  XmToggleButtonSetState(playrBtn, False, False);
  
  fr_splinetype = SPLINE_LINEAR;
  XtVaSetValues(splineTypeMenu, XmNmenuHistory, linearBtn, NULL);

  fr_playtype = FR_ONEDIR;
  XmToggleButtonSetState(dir1Btn, True, False);
  XmToggleButtonSetState(dir2Btn, False, False);
  XmToggleButtonSetState(loopBtn, False, False);
  XtSetSensitive(dir1Btn, False);
  XtSetSensitive(dir2Btn, True);
  XtSetSensitive(loopBtn, True);

  fr_sta = fr_cur = 0.0;           /* first frame */
  fr_end = olist[0]->nframes-1;    /* last frame (different for loop mode)*/
  new_limits_flag = ON;

  fr_speed = 0.1;
  XtVaSetValues(frameRateScale, XmNvalue, (int) (fr_speed*100.0), NULL);
  fr_time = 1;
  XtVaSetValues(frameTimeScale, XmNvalue, (int) (fr_time*100.0), NULL);
  fr_inttype = FR_RATE;
  XtSetSensitive(frameTimeScale, False);
  idle_interval = 0.5;
  XtVaSetValues(endIntScale, XmNvalue, (int) (idle_interval*10.0), NULL);
  idle_start = 0;
}

float linear(float x0, float x1, float t) {
  return (1-t)*x0 + t*x1;
}

float spline(float x0, float x1, float x2, float x3, float t)
{
  float p10, p11, p12, p20, p21, x;

  p10=linear(x0,x1,t+1);
  p11=linear(x1,x2,t);
  p12=linear(x2,x3,t-1);

  p20=linear(p10,p11,(t+1)/2);
  p21=linear(p11,p12,t/2);

  x=linear(p20,p21,t);
  return x;
}


void frames_draw()
{
  int lastframe = (fr_playtype == FR_LOOP ? olist[0]->nframes 
		   : olist[0]->nframes -1);
  float df; 
  int n0, n1, n2, n3;
  Vertex *v0, *v1, *v2, *v3, *v4;
  int i, j, k;
  char str[20];
  int staX, endX, curX; /* position on frIndicator (in pixels) */

  df = fr_cur - (int)fr_cur;

  if(fr_playtype == FR_LOOP) {
    n1 = (int)(floor(fr_cur)-fr_sta)%(int)(fr_end-fr_sta)+(int)fr_sta;
    n2 = (int)(ceil(fr_cur)-fr_sta)%(int)(fr_end-fr_sta)+(int)fr_sta;
  } else {
    n1 = (int)floor(fr_cur);
    n2 = (int)ceil(fr_cur);
  }
  
  if(fr_playtype == FR_LOOP) {
    n0 = (int)(fr_end-fr_sta+n1-1-fr_sta)%(int)(fr_end-fr_sta)+(int)fr_sta;
    n3 = (int)(n2+1-fr_sta)%(int)(fr_end-fr_sta)+(int)fr_sta;
  } else {
    n0 = max(n1-1, 0);
    n3 = min(n2+1, lastframe);
  }

  for(k=0; k<nobjs; k++)
    switch(fr_splinetype)
      {
      case SPLINE_LINEAR:
	for(i = 0; i < olist[k]->nvertices; i++) {
	  v0 = &(olist[k]->vlist[i]);
	  v1 = &(olist[k]->frames[n1]->vlist[i]);
	  v2 = &(olist[k]->frames[n2]->vlist[i]);
	  for(j = 0; j < 4; j++)
	    v0->p4.d[j] = linear(v1->p4.d[j], v2->p4.d[j], df);
	  if(olist[k]->texture_flag == ON)
	    for(j = 0; j < 3; j++)
	      v0->tex.d[j] = linear(v1->tex.d[j], v2->tex.d[j], df);
	}
	break;
	
      case SPLINE_CUBIC:
	for(i = 0; i < olist[k]->nvertices; i++) {
	  v0 = &(olist[k]->vlist[i]);
	  v1 = &(olist[k]->frames[n0]->vlist[i]);
	  v2 = &(olist[k]->frames[n1]->vlist[i]);
	  v3 = &(olist[k]->frames[n2]->vlist[i]);
	  v4 = &(olist[k]->frames[n3]->vlist[i]);
	  for(j = 0; j < 4; j++)
	    v0->p4.d[j] = 
	      spline(v1->p4.d[j],v2->p4.d[j], v3->p4.d[j],v4->p4.d[j],df);
	  if(olist[k]->texture_flag == ON)
	    for(j = 0; j < 3; j++)
	      v0->tex.d[j] = spline(v1->tex.d[j], v2->tex.d[j], 
				    v3->tex.d[j], v4->tex.d[j], df);
	}
	break;
      }
  
  From4DTo3D();
  AutoSmooth();
  redraw_flag = ON;
  draw_all();

  if(new_limits_flag == ON) {
    sprintf(str, "%4.1f", fr_sta);
    XtVaSetValues (staLbl, XtVaTypedArg, XmNlabelString, XmRString,str,5,NULL);
    sprintf(str, "%4.1f", (fr_playtype == FR_LOOP ? fr_end-1 : fr_end));
    XtVaSetValues (endLbl, XtVaTypedArg, XmNlabelString, XmRString,str,5,NULL);
    new_limits_flag = OFF;
  }
  sprintf(str, "%4.1f", fr_cur);
  XtVaSetValues (curLbl, XtVaTypedArg, XmNlabelString, XmRString, str, 5,NULL);

  staX = ind_width * fr_sta / lastframe;
  endX = ind_width * fr_end / lastframe;
  curX = ind_width * fr_cur / lastframe;

  XClearWindow(dpy, fr_win);

  XSetForeground (dpy, gc, pixs[FR_FG]);
  XFillRectangle(dpy, fr_win, gc, 0, 0, staX, ind_height);
  XFillRectangle(dpy, fr_win, gc, endX, 0, ind_width-endX, ind_height);

  XSetForeground (dpy, gc, pixs[FR_GREEN]);
  XFillRectangle(dpy, fr_win, gc, staX, 0, curX-staX, ind_height);
      
  XSetForeground (dpy, gc, pixs[FR_RED]);
  XDrawLine (dpy, fr_win, gc, curX, 0, curX, ind_height-1);

  switch(select_flag)
    {
    case START_X:
      XSetForeground (dpy, gc, pixs[FR_FG]);
      XDrawLine (dpy, fr_win, gc, staX, 0, staX, ind_height-1);
      XSetForeground (dpy, gc, pixs[FR_YELLOW]);
      XDrawLine (dpy, fr_win, gc, staX-1, 0, staX-1, ind_height-1);
      break;
    case END_X:
      XSetForeground (dpy, gc, pixs[FR_FG]);
      XDrawLine (dpy, fr_win, gc, endX-1, 0, endX-1, ind_height-1);
      XSetForeground (dpy, gc, pixs[FR_YELLOW]);
      XDrawLine (dpy, fr_win, gc, endX, 0, endX, ind_height-1);
      break;
    case CUR_X:
      XSetForeground (dpy, gc, pixs[FR_RED]);
      XFillRectangle(dpy, fr_win, gc, staX, 0, 
		     curX-staX, ind_height);
      break;
    }
}

void frames_play()
{
  float step;

  double newtime = getcurtime();

  if(fr_inttype == FR_RATE) {
    step = fr_speed;
  } else {
    step = (newtime-oldtime)/fr_time;
    oldtime = newtime;
  }

  if((newtime - idle_start) < idle_interval)  return;

  switch(fr_direction)
      {
      case SWITCH_N_WAIT:
	if(fr_cur == fr_end) {
	  fr_cur = fr_sta;
	  fr_direction = FORWARD;
	} else {
	  fr_cur = fr_end;
	  fr_direction = REVERSE;
	}
	idle_start = newtime;
	break;

      case FORWARD:
	fr_cur += step;
	if(fr_cur > fr_end) {
	  switch(fr_playtype)
	    {
	    case FR_ONEDIR:
	      fr_cur = fr_end;
	      idle_start = newtime;
	      fr_direction = SWITCH_N_WAIT;
	      break;
	    case FR_TWODIR:
	      fr_cur = fr_end;
	      idle_start = newtime;
	      fr_direction = REVERSE;
	      XmToggleButtonSetState(playBtn, False, False);
	      XmToggleButtonSetState(playrBtn, True, False);
	      break;
	    case FR_LOOP:
	      fr_cur = fr_sta;
	      break;
	    }	
	}
	break;

      case REVERSE:
	fr_cur -= step;
	if(fr_cur < fr_sta) {
	  switch(fr_playtype)
	    {
	    case FR_ONEDIR:
	      fr_cur = fr_sta;
	      idle_start = newtime;
	      fr_direction = SWITCH_N_WAIT;
	      break;
	    case FR_TWODIR:
	      fr_cur = fr_sta;
	      idle_start = newtime;
	      fr_direction = FORWARD;
	      XmToggleButtonSetState(playrBtn, False, False);
	      XmToggleButtonSetState(playBtn, True, False);
	      break;
	    case FR_LOOP:
	      fr_cur = fr_end;
	      break;
	    }	
	}
	break;
      }
  frames_draw();
}

/*ARGSUSED*/
void frames_close(Widget w, XtPointer client_data, XtPointer call_data) {
  XtUnmanageChild(w);
}

/*ARGSUSED*/
void frames_reset(Widget w, XtPointer client_data, XtPointer call_data) {
  frames_init();
  frames_draw();
}

/*ARGSUSED*/
void frames_control(Widget w, XtPointer client_data, XtPointer call_data)
{
  int flag = (int) client_data;
  XmToggleButtonCallbackStruct *cbs = 
    (XmToggleButtonCallbackStruct *) call_data;

  switch(flag)
    {
    case FR_REW:
      frames_transform_flag = OFF;
      fr_cur = fr_sta;
      XmToggleButtonSetState(playBtn, False, False);
      XmToggleButtonSetState(playrBtn, False, False);
      break;

    case FR_STEPR:
      frames_transform_flag = OFF; 
      XmToggleButtonSetState(playBtn, False, False);
      XmToggleButtonSetState(playrBtn, False, False);      
      fr_cur -= fr_speed;
      if(fr_cur < fr_sta) {
	switch(fr_playtype)
	  {
	  case FR_ONEDIR:
	  case FR_TWODIR:
	    fr_cur = fr_sta;
	    break;
	  case FR_LOOP:
	    fr_cur = fr_end;
	    break;
	  }
      }
      break;

    case FR_PLAYR:
      if(cbs->set == True){
	fr_direction = REVERSE;
	frames_transform_flag = ON;
	XmToggleButtonSetState(playBtn, False, False);
	oldtime = getcurtime();
      } else {
	frames_transform_flag = OFF;
      }
      break;

    case FR_STOP:
      frames_transform_flag = OFF;
      XmToggleButtonSetState(playBtn, False, False);
      XmToggleButtonSetState(playrBtn, False, False);
      break;

    case FR_PLAY:
      if(cbs->set == True){
	fr_direction = FORWARD;
	frames_transform_flag = ON;
	XmToggleButtonSetState(playrBtn, False, False);
	oldtime = getcurtime();
      } else {
	frames_transform_flag = OFF;
      }     
      break;

    case FR_STEP:
      frames_transform_flag = OFF; 
      XmToggleButtonSetState(playBtn, False, False);
      XmToggleButtonSetState(playrBtn, False, False);      
      fr_cur += fr_speed;
      if(fr_cur > fr_end) {
	switch(fr_playtype)
	  {
	  case FR_ONEDIR:
	  case FR_TWODIR:
	    fr_cur = fr_end;
	    break;
	  case FR_LOOP:
	    fr_cur = fr_sta;
	    break;
	  }
      }
      break;

    case FR_FWD:
      frames_transform_flag = OFF;
      fr_cur = fr_end;
      XmToggleButtonSetState(playBtn, False, False);
      XmToggleButtonSetState(playrBtn, False, False);
      break;

    case FR_ONEDIR:
      if(cbs->set == True){
	XmToggleButtonSetState(dir2Btn, False, False);
	XmToggleButtonSetState(loopBtn, False, False);
	XtSetSensitive(dir2Btn, True);
	XtSetSensitive(loopBtn, True);
	XtSetSensitive(dir1Btn, False);
	if(fr_playtype == FR_LOOP) fr_end--;
	fr_playtype = FR_ONEDIR;
      }
      break;

    case FR_TWODIR:
      if(cbs->set == True){
	XmToggleButtonSetState(dir1Btn, False, False);
	XmToggleButtonSetState(loopBtn, False, False);
	XtSetSensitive(dir1Btn, True);
	XtSetSensitive(loopBtn, True);
	XtSetSensitive(dir2Btn, False);
	if(fr_playtype == FR_LOOP) fr_end--;
	fr_playtype = FR_TWODIR;
      }
      break;

    case FR_LOOP:
      if(cbs->set == True){
	XmToggleButtonSetState(dir1Btn, False, False);
	XmToggleButtonSetState(dir2Btn, False, False);
	XtSetSensitive(dir1Btn, True);
	XtSetSensitive(dir2Btn, True);
	XtSetSensitive(loopBtn, False);
	fr_playtype = FR_LOOP;
	/* allign with frames */
	fr_sta = floor(fr_sta); 
	fr_end = ceil(fr_end+1); /* last frame == first frame */
	new_limits_flag = ON;
      } 
      break;
    }
  idle_start = 0;
  frames_draw();
}

/*ARGSUSED*/
void changeFrType(Widget w, XtPointer client_data, XtPointer call_data) {
  fr_splinetype = (int) client_data;
  frames_draw();
}

/*ARGSUSED*/
void frames_draw_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
  static int first_time = True;

  XmDrawingAreaCallbackStruct *cbs =
    (XmDrawingAreaCallbackStruct *) call_data;
  
  XEvent *event = cbs->event;

  switch(cbs->reason)
    {
    case XmCR_EXPOSE:
      if(first_time == True) { /* needed for frames_draw() */
	fr_win = event->xexpose.window;
	first_time = False;
      }
      frames_draw();
      break;
    case XmCR_RESIZE:
      XtVaGetValues (frIndicator, 
		     XmNwidth, &ind_width,
		     XmNheight, &ind_height,
		     NULL);
      break;
    }
}

/* Action procedure to respond to any of the events from the
 * translation table for frIndicator widget.
 */
void trackFr(Widget widget, XEvent *event, String *args, int *num_args)
{
  int staX, endX, butX;
  int lastframe;

  XButtonEvent *bevent = (XButtonEvent *) event;
  
  if (*num_args != 1)
    XtError ("Wrong number of args!");
  
  lastframe = (fr_playtype == FR_LOOP ? olist[0]->nframes : olist[0]->nframes-1);
  staX = ind_width * fr_sta / lastframe;
  endX = ind_width * fr_end / lastframe;    
  butX = bevent->x;

  if (!strcmp (args[0], "down1")) { 

    select_flag = NONE_X;
    if(staX-5 <= butX && butX <= staX+5)
      select_flag = START_X; /* mouse within 5 pixels from the start point */
    if(endX-5 <= butX && butX <= endX+5
       && (select_flag == NONE_X || butX >= endX))
      select_flag = END_X;   /* mouse within 5 pixels from the end point */

  } else  if (!strcmp (args[0], "down2")) {

    if(staX <= butX && butX <= endX) {
      fr_cur = (float)butX / ind_width * lastframe;
      select_flag = CUR_X; 
    }

  } else if (!strcmp (args[0], "up")) {

    select_flag = NONE_X;

  } else { /* motion */

    switch(select_flag)
      {

      case START_X:
	fr_sta = (float)butX/ind_width * lastframe;
	/* allign the starting point with a frame for loop mode */
	if(fr_playtype == FR_LOOP) fr_sta = rint(fr_sta);
	/* check limit conditions */
	if(fr_sta >= fr_end){
	  if(fr_playtype == FR_LOOP) fr_sta = fr_end - 1.0;
	  else fr_sta = fr_end;
	}
	if(fr_sta < 0.0) fr_sta = 0.0;
	/* check current point location */
	if(fr_cur < fr_sta) fr_cur = fr_sta;
	new_limits_flag = ON;
	break;

      case END_X:
	fr_end = (float)butX/ind_width * lastframe;
	/* allign the starting point with a frame for loop mode */
	if(fr_playtype == FR_LOOP) fr_end = rint(fr_end);
	/* check limit conditions */
	if(fr_end <= fr_sta) {
	  if(fr_playtype == FR_LOOP) fr_end = fr_sta + 1.0;
	  else fr_end = fr_sta;
	}
	if(fr_end > lastframe) fr_end = lastframe;
	if(fr_cur > fr_end) fr_cur = fr_end;
	new_limits_flag = ON;
	break;

      case CUR_X:
	fr_cur = (float)butX/ind_width * lastframe;
	if(fr_cur > fr_end) fr_cur = fr_end;
	if(fr_cur < fr_sta) fr_cur = fr_sta;
	break;
      }
  }
  frames_draw();
}

/*ARGSUSED*/
void change_limits(Widget w, XtPointer client_data, XtPointer call_data) {

  int flag = (int) client_data;
  int lastframe;
  
  switch(flag)
    {
    case FR_START_L:
      if(fr_playtype == FR_LOOP) fr_sta -= 1.0;
      else fr_sta -= fr_speed;
      if(fr_sta < 0.0) fr_sta = 0.0;
      new_limits_flag = ON;
      break;
      
    case FR_START_R:
      if(fr_playtype == FR_LOOP) fr_sta += 1.0;
      else fr_sta += fr_speed;
      if(fr_sta >= fr_end){
	if(fr_playtype == FR_LOOP) fr_sta = fr_end - 1.0;
	else fr_sta = fr_end;
      }
      if(fr_cur < fr_sta) fr_cur = fr_sta;
      new_limits_flag = ON;
      break;

    case FR_CUR_L:
      fr_cur  -= fr_speed;
      if(fr_cur < fr_sta) fr_cur = fr_sta;
      break;

    case FR_CUR_R:
      fr_cur += fr_speed;
      if(fr_cur > fr_end) fr_cur = fr_end;
      break;

    case FR_END_L:
      if(fr_playtype == FR_LOOP) fr_end -= 1.0;
      else fr_end -= fr_speed;
      if(fr_end <= fr_sta) {
	if(fr_playtype == FR_LOOP) fr_end = fr_sta + 1.0;
	else fr_end = fr_sta;
      }
      if(fr_cur > fr_end) fr_cur = fr_end;
      new_limits_flag = ON;
      break;

    case FR_END_R:
      lastframe = (fr_playtype == FR_LOOP ? olist[0]->nframes
		   : olist[0]->nframes-1);
      if(fr_playtype == FR_LOOP) fr_end += 1.0;
      else fr_end += fr_speed;
      if(fr_end > lastframe) fr_end = lastframe;
      new_limits_flag = ON;
      break;
    }
  frames_draw();
}


/*ARGSUSED*/
void changeFrRate(Widget w, XtPointer client_data, XtPointer call_data) {

  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  fr_speed = cbs->value * 0.01;
}

/*ARGSUSED*/
void change_interp(Widget w, XtPointer client_data, XtPointer call_data) {

  switch((int)client_data)
    {
    case FR_RATE:
      XtSetSensitive(frameTimeScale, False);
      XtSetSensitive(frameRateScale, True);
      fr_inttype = FR_RATE;
      break;
    case FR_TIME:
      XtSetSensitive(frameRateScale, False);
      XtSetSensitive(frameTimeScale, True);
      fr_inttype = FR_TIME;
      break;
    }
}

/*ARGSUSED*/
void changeFrTime(Widget w, XtPointer client_data, XtPointer call_data) {

  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  fr_time = cbs->value * 0.01;
}

/*ARGSUSED*/
void changeWaitInt(Widget w, XtPointer client_data, XtPointer call_data) {

  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  idle_interval = cbs->value * 0.1;
}

/* build key frames panel */
void build_uiFrames(Widget menuBar)
{
  int n;
  Arg args[10];
  Widget form, frame;
  Widget rewBtn, steprBtn, stopBtn, stepBtn, fwdBtn;
  Widget staLBtn, staRBtn, curLBtn, curRBtn, endLBtn, endRBtn, temp;
  Widget rateBtn, intLbl, timeBtn, framesHelpBox;
  Pixmap px0, px1;

  String translationsFr = /* for the DrawingArea widget */
    "<Btn1Down>:   trackFr(down1)\n"
    "<Btn2Down>:   trackFr(down2)\n"
    "<Btn1Up>:     trackFr(up)\n"
    "<Btn2Up>:     trackFr(up)\n"
    "<Btn1Motion>: trackFr(motion)\n"
    "<Btn2Motion>: trackFr(motion)";
  XtActionsRec actionFr = {"trackFr", (XtActionProc) trackFr };


  /* Add the "trackFr" action/function used by the DrawingArea widget */
  XtAppAddActions (app_context, &actionFr, 1);

  n = 0;  /* create frames dialog */
  XtSetArg(args[n], XmNtitle, "Key frames"); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  framesDialog = XmCreateMessageDialog(menuBar, "framesDialog", args, n);
  XtVaSetValues(framesDialog,
		XtVaTypedArg, XmNokLabelString, XmRString, "Close", 6,
		XtVaTypedArg, XmNcancelLabelString, XmRString, "Reset", 6,
		NULL);
  /* make dialog shell non resizable but leave minimize button */
  XtVaSetValues(XtParent(framesDialog),
		XmNmwmDecorations,  
		MWM_DECOR_BORDER | MWM_DECOR_TITLE | 
		MWM_DECOR_MENU | MWM_DECOR_MINIMIZE ,
		XmNmwmFunctions,
		MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE | MWM_FUNC_CLOSE,
		NULL);
  XtAddCallback(framesDialog, XmNokCallback, frames_close, 0);
  XtAddCallback(framesDialog, XmNcancelCallback, frames_reset, 0);

  n = 0;  /* framesHelpBox */
  XtSetArg(args[n], XmNtitle, "Key frames help"); n++;
  framesHelpBox = XmCreateMessageDialog(framesDialog, "framesHelpBox", 
					args, n);
  temp = XmMessageBoxGetChild (framesHelpBox, XmDIALOG_CANCEL_BUTTON);
  XtUnmanageChild (temp);
  temp = XmMessageBoxGetChild (framesHelpBox, XmDIALOG_HELP_BUTTON);
  XtUnmanageChild (temp);
  XtAddCallback(framesDialog, XmNhelpCallback, ShowBox, framesHelpBox);
  /* framesHelpList, global, initialized in ui_init() */
  framesHelpList = XmCreateScrolledList(framesHelpBox, "framesHelpList", 
					NULL, 0);
  XtManageChild(framesHelpList);  

  allocateColors();
  
  form = XtVaCreateManagedWidget ("FrameForm",
				  xmFormWidgetClass, framesDialog,
				  NULL);
  /* create control buttons: */
  /* REW, STEP_BACK, PLAY_BACK, STOP, PLAY_FWD, STEP_FWD, FWD */
  /* ONE_DIRECTION, TWO_DIRECTIONS, LOOP */
  /* create a pixmap for each button */

  /* Rewind */
  px0 = makePixmap((char *)rew_bits, rew_width, rew_height, FR_FG);
  px1 = makePixmap((char *)rew_bits, rew_width, rew_height, FR_GREEN);
  rewBtn = XtVaCreateManagedWidget("rewBtn",
				   xmPushButtonWidgetClass, form, 
				   XmNlabelType, XmPIXMAP,
				   XmNlabelPixmap, px0, 
				   XmNarmPixmap, px1,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNleftAttachment, XmATTACH_FORM,
				   NULL);
  XtAddCallback(rewBtn, XmNarmCallback, frames_control, (XtPointer) FR_REW);
  
  /* Step Back */
  px0 = makePixmap((char *)stepr_bits, stepr_width, stepr_height, FR_FG);
  px1 = makePixmap((char *)stepr_bits, stepr_width, stepr_height, FR_GREEN);
  steprBtn = XtVaCreateManagedWidget("steprBtn",
				     xmPushButtonWidgetClass, form, 
				     XmNlabelType, XmPIXMAP,
				     XmNlabelPixmap, px0,
				     XmNarmPixmap, px1,
				     XmNtopAttachment, XmATTACH_FORM,
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, rewBtn,
				     XmNleftOffset, 3,
				     NULL);
  XtAddCallback(steprBtn, XmNarmCallback, frames_control,(XtPointer) FR_STEPR);

  /* Play Back */
  px0 = makePixmap((char *)playr_bits, playr_width, playr_height, FR_FG);
  px1 = makePixmap((char *)playr_bits, playr_width, playr_height, FR_GREEN);
  playrBtn = XtVaCreateManagedWidget("playrBtn",
				     xmToggleButtonWidgetClass, form, 
				     XmNlabelType, XmPIXMAP,
				     XmNlabelPixmap, px0,
				     XmNselectPixmap, px1,
				     XmNindicatorOn, False,
				     XmNtopAttachment, XmATTACH_FORM,
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, steprBtn,
				     XmNleftOffset, 3,
				     NULL);
  XtAddCallback(playrBtn, XmNvalueChangedCallback, frames_control,
		(XtPointer) FR_PLAYR);

  /* Stop */
  px0 = makePixmap((char *)stop_bits, stop_width, stop_height, FR_FG);
  px1 = makePixmap((char *)stop_bits, stop_width, stop_height, FR_RED);
  stopBtn = XtVaCreateManagedWidget("stopBtn",
				    xmPushButtonWidgetClass, form, 
				    XmNlabelType, XmPIXMAP,
				    XmNlabelPixmap, px0,
				    XmNarmPixmap, px1,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNleftAttachment, XmATTACH_WIDGET,
				    XmNleftWidget, playrBtn,
				    XmNleftOffset, 3,
				    NULL);
  XtAddCallback(stopBtn, XmNarmCallback, frames_control, (XtPointer) FR_STOP);

  /* Play */
  px0 = makePixmap((char *)play_bits, play_width, play_height, FR_FG);
  px1 = makePixmap((char *)play_bits, play_width, play_height, FR_GREEN);
  playBtn = XtVaCreateManagedWidget("playBtn",
				    xmToggleButtonWidgetClass, form, 
				    XmNlabelType, XmPIXMAP,
				    XmNlabelPixmap, px0,
				    XmNselectPixmap, px1,
				    XmNindicatorOn, False,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNleftAttachment, XmATTACH_WIDGET,
				    XmNleftWidget, stopBtn,
				    XmNleftOffset, 3,
				    NULL);
  XtAddCallback(playBtn, XmNvalueChangedCallback, frames_control, 
		(XtPointer) FR_PLAY);

  /* Step */
  px0 = makePixmap((char *)step_bits, step_width, step_height, FR_FG);
  px1 = makePixmap((char *)step_bits, step_width, step_height, FR_GREEN);
  stepBtn = XtVaCreateManagedWidget("Step",
				    xmPushButtonWidgetClass, form, 
				    XmNlabelType, XmPIXMAP,
				    XmNlabelPixmap, px0,
				    XmNarmPixmap, px1,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNleftAttachment, XmATTACH_WIDGET,
				    XmNleftWidget, playBtn,
				    XmNleftOffset, 3,
				    NULL);
  XtAddCallback(stepBtn, XmNarmCallback, frames_control, (XtPointer) FR_STEP);

  /* Forward */ 
  px0 = makePixmap((char *)fwd_bits, fwd_width, fwd_height, FR_FG);
  px1 = makePixmap((char *)fwd_bits, fwd_width, fwd_height, FR_GREEN);
  fwdBtn = XtVaCreateManagedWidget("Fwd",
				   xmPushButtonWidgetClass, form, 
				   XmNlabelType, XmPIXMAP,
				   XmNlabelPixmap, px0,
				   XmNarmPixmap, px1,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNleftAttachment, XmATTACH_WIDGET,
				   XmNleftWidget, stepBtn,
				   XmNleftOffset, 3,
				   NULL);
  XtAddCallback(fwdBtn, XmNarmCallback, frames_control, (XtPointer) FR_FWD);
    
  /* One direction motion */
  px0 = makePixmap((char *)dir1_bits, dir1_width, dir1_height, FR_FG);
  px1 = makePixmap((char *)dir1_bits, dir1_width, dir1_height, FR_BLUE);
  dir1Btn = XtVaCreateManagedWidget("One dir",
				    xmToggleButtonWidgetClass, form, 
				    XmNlabelType, XmPIXMAP,
				    XmNlabelPixmap, px0,
				    XmNselectPixmap, px1,
				    XmNlabelInsensitivePixmap, px1,
				    XmNindicatorOn, False,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNtopWidget, rewBtn,
				    XmNtopOffset, 5,
				    NULL);
  XtAddCallback(dir1Btn, XmNvalueChangedCallback, frames_control, 
		(XtPointer) FR_ONEDIR);

  /* Two direction motion */
  px0 = makePixmap((char *)dir2_bits, dir2_width, dir2_height, FR_FG);
  px1 = makePixmap((char *)dir2_bits, dir2_width, dir2_height, FR_BLUE);
  dir2Btn = XtVaCreateManagedWidget("Two dir",
				    xmToggleButtonWidgetClass, form, 
				    XmNlabelType, XmPIXMAP,
				    XmNlabelPixmap, px0,
				    XmNselectPixmap, px1,
				    XmNlabelInsensitivePixmap, px1,
				    XmNindicatorOn, False,
				    XmNleftAttachment, XmATTACH_WIDGET,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget, steprBtn,
				    XmNleftWidget, dir1Btn,
				    XmNleftOffset, 3,
				    XmNtopOffset, 5,
				    NULL);
  XtAddCallback(dir2Btn, XmNvalueChangedCallback, frames_control, 
		(XtPointer) FR_TWODIR);
  
  /* Loop */
  px0 = makePixmap((char *)loop_bits, loop_width, loop_height, FR_FG);
  px1 = makePixmap((char *)loop_bits, loop_width, loop_height, FR_BLUE);
  loopBtn = XtVaCreateManagedWidget("Loop",
				    xmToggleButtonWidgetClass, form, 
				    XmNlabelType, XmPIXMAP,
				    XmNlabelPixmap, px0,
				    XmNselectPixmap, px1,
				    XmNlabelInsensitivePixmap, px1,
				    XmNindicatorOn, False,
				    XmNleftAttachment, XmATTACH_WIDGET,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget, playrBtn,
				    XmNleftWidget, dir2Btn,
				    XmNleftOffset, 3,
				    XmNtopOffset, 5,
				    NULL);
  XtAddCallback(loopBtn, XmNvalueChangedCallback, frames_control, 
		(XtPointer) FR_LOOP);
  
  /* Spline Type Menu (Linear or Cubic) */
  /* Have to create it here (need handles for buttons in reset)*/

  temp = XmCreatePulldownMenu (form, "_pulldown", NULL, 0);

  n = 0;
  XtSetArg (args[n], XmNsubMenuId, temp); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg (args[n], XmNtopWidget, fwdBtn); n++;
  splineTypeMenu = XmCreateOptionMenu (form, "splineTypeMenu", args, n);
  
  linearBtn = XtVaCreateManagedWidget ("Linear",
				       xmPushButtonGadgetClass, temp,
				       XmNmnemonic, 'L',
				       NULL);
  XtAddCallback (linearBtn,XmNactivateCallback,changeFrType,
		 (XtPointer)SPLINE_LINEAR);

  cubicBtn = XtVaCreateManagedWidget ("Cubic",
				      xmPushButtonGadgetClass, temp,
				      XmNmnemonic, 'C',
				      NULL);
  XtAddCallback (cubicBtn,XmNactivateCallback,changeFrType,
		 (XtPointer)SPLINE_CUBIC);

  XtManageChild(splineTypeMenu);

  /* Frame and drawing area */
  frame =  XtVaCreateManagedWidget("Frame",
				   xmFrameWidgetClass, form,
				   XmNshadowThickness, 2,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, splineTypeMenu,
				   XmNtopOffset, 10,
				   NULL);
  
  /* Create a GC for drawing (callback). */
  gc = XCreateGC (dpy, RootWindowOfScreen (scr), 0, NULL);
  
  frIndicator = XtVaCreateManagedWidget("frIndicator",
				       xmDrawingAreaWidgetClass, frame,
				       XmNtranslations, 
				       XtParseTranslationTable(translationsFr),
				       XmNheight, 10,
				       XmNwidth, 250,
				       NULL);
  XtAddCallback(frIndicator, XmNexposeCallback, frames_draw_cb, NULL);
  XtAddCallback(frIndicator, XmNresizeCallback, frames_draw_cb, NULL);
  XtVaGetValues(frIndicator, XmNwidth, &ind_width,XmNheight, &ind_height,NULL);

  /* Create buttons and labels for start, end and current points */
  /* start left button */
  staLBtn = XtVaCreateManagedWidget ("staLBtn",
				     xmArrowButtonWidgetClass, form,
				     XmNarrowDirection, XmARROW_LEFT,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNtopWidget, frame,
				     XmNtopOffset, 3,
				     NULL);
  XtAddCallback(staLBtn, XmNarmCallback, change_limits, (XtPointer)FR_START_L);
  
  /* start lable */
  staLbl = XtVaCreateManagedWidget  ("staLbl",
				     xmLabelGadgetClass, form,
				     XtVaTypedArg, XmNlabelString,
				     XmRString, "    ", 5, 
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, staLBtn,
				     XmNtopWidget, frame,
				     XmNtopOffset, 8,
				     NULL);

  /* start right button */
  staRBtn = XtVaCreateManagedWidget ("staRBtn",
				     xmArrowButtonWidgetClass, form,
				     XmNarrowDirection, XmARROW_RIGHT,
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, staLbl,
				     XmNtopWidget, frame,
				     XmNtopOffset, 3,
				     NULL);
  XtAddCallback(staRBtn, XmNarmCallback, change_limits, (XtPointer)FR_START_R);
  
  /* current left button */
  curLBtn = XtVaCreateManagedWidget ("curLBtn",
				     xmArrowButtonWidgetClass, form,
				     XmNarrowDirection, XmARROW_LEFT,
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, staRBtn,
				     XmNtopWidget, frame,
				     XmNleftOffset, 29,
				     XmNtopOffset, 3,
				     NULL);
  XtAddCallback(curLBtn, XmNarmCallback, change_limits, (XtPointer) FR_CUR_L);

  /* current lable */
  curLbl = XtVaCreateManagedWidget  ("curLbl",
				     xmLabelGadgetClass, form,
				     XtVaTypedArg, XmNlabelString,
				     XmRString, "    ", 5, 
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, curLBtn,
				     XmNtopWidget, frame,
				     XmNtopOffset, 8,
				     NULL);

  /* current right button */
  curRBtn = XtVaCreateManagedWidget ("curRBtn",
				     xmArrowButtonWidgetClass, form,
				     XmNarrowDirection, XmARROW_RIGHT,
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, curLbl,
				     XmNtopWidget, frame,
				     XmNtopOffset, 3,
				     NULL);
  XtAddCallback(curRBtn, XmNarmCallback, change_limits, (XtPointer) FR_CUR_R);

  /* end left button */
  endLBtn = XtVaCreateManagedWidget ("endLBtn",
				     xmArrowButtonGadgetClass, form,
				     XmNarrowDirection, XmARROW_LEFT,
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, curRBtn,
				     XmNtopWidget, frame,
				     XmNleftOffset, 29,
				     XmNtopOffset, 3,
				     NULL);
  XtAddCallback(endLBtn, XmNarmCallback, change_limits, (XtPointer) FR_END_L);

  /* end lable */
  endLbl = XtVaCreateManagedWidget  ("endLbl",
				     xmLabelGadgetClass, form,
				     XtVaTypedArg, XmNlabelString,
				     XmRString, "    ", 5, 
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, endLBtn,
				     XmNtopWidget, frame,
				     XmNtopOffset, 8,
				     NULL);


  /* end right button */
  endRBtn = XtVaCreateManagedWidget ("endRBtn",
				     xmArrowButtonGadgetClass, form,
				     XmNarrowDirection, XmARROW_RIGHT,
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, endLbl,
				     XmNtopWidget, frame,
				     XmNtopOffset, 3,
				     NULL);
  XtAddCallback(endRBtn, XmNarmCallback, change_limits, (XtPointer) FR_END_R);

  /* Separator */
  temp =  XtVaCreateManagedWidget("Separator",
				   xmSeparatorWidgetClass, form,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, staLBtn,
				   XmNtopOffset, 10,
				   NULL);

  /* Frame and drawing area */
  frame =  XtVaCreateManagedWidget("ControlFrame",
				   xmFrameWidgetClass, form,
				   XmNshadowThickness, 5,
				   XmNshadowType, XmSHADOW_IN,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, temp,
				   XmNtopOffset, 10,
				   NULL);

  form = XtVaCreateManagedWidget ("FrameControlForm",
				  xmFormWidgetClass, frame,
				  NULL);

  frameRateScale = XtVaCreateManagedWidget ("frameRateScale",
					xmScaleWidgetClass, form,
					XtVaTypedArg, XmNtitleString, 
					XmRString, 
					"step per drawing (frames)",25,
					XmNmaximum,   100,
					XmNminimum,   1,
					XmNdecimalPoints, 2,
					XmNscaleMultiple, 1,
					XmNshowValue, True,
					XmNorientation, XmHORIZONTAL,
					XmNleftAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNtopAttachment, XmATTACH_FORM,
					NULL);
  XtAddCallback (frameRateScale, XmNdragCallback, changeFrRate, NULL);
  XtAddCallback (frameRateScale, XmNvalueChangedCallback, changeFrRate, NULL);

  /* rate top button */
  rateBtn = XtVaCreateManagedWidget ("rateBtn",
				     xmArrowButtonGadgetClass, form,
				     XmNarrowDirection, XmARROW_UP,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNtopWidget, frameRateScale,
				     XmNleftOffset, 20,
				     NULL);
  XtAddCallback(rateBtn, XmNarmCallback, change_interp, (XtPointer) FR_RATE);

  /* interpolation type lable */
  intLbl = XtVaCreateManagedWidget  ("intLbl",
				     xmLabelGadgetClass, form,
				     XtVaTypedArg, XmNlabelString,
				     XmRString, "type of interpolation", 30, 
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, rateBtn,
				     XmNtopWidget, frameRateScale,
				     XmNtopOffset, 5,
				     XmNleftOffset, 10,
				     NULL);  

  /* time botom button */
  timeBtn = XtVaCreateManagedWidget ("timeBtn",
				     xmArrowButtonGadgetClass, form,
				     XmNarrowDirection, XmARROW_DOWN,
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, intLbl,
				     XmNtopWidget, frameRateScale,
				     XmNleftOffset, 10,
				     NULL);
  XtAddCallback(timeBtn, XmNarmCallback, change_interp, (XtPointer) FR_TIME);

  frameTimeScale = XtVaCreateManagedWidget ("frameTimeScale",
					xmScaleWidgetClass, form,
					XtVaTypedArg, XmNtitleString, 
					XmRString, 
					"time between frames (sec)",25,
					XmNmaximum, 1000,
					XmNminimum, 1,
					XmNdecimalPoints, 2,
					XmNscaleMultiple, 1,
					XmNshowValue, True,
					XmNorientation, XmHORIZONTAL,
					XmNleftAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNtopAttachment, XmATTACH_WIDGET,
					XmNtopWidget, rateBtn,
					NULL);
  XtAddCallback (frameTimeScale, XmNdragCallback, changeFrTime, NULL);
  XtAddCallback (frameTimeScale, XmNvalueChangedCallback, changeFrTime, NULL);

  endIntScale = XtVaCreateManagedWidget ("frameTimeScale",
					xmScaleWidgetClass, form,
					XtVaTypedArg, XmNtitleString, 
					XmRString, 
					"time to wait at endpoints (sec)",35,
					XmNmaximum,   50,
					XmNminimum,   0,
					XmNdecimalPoints, 1,
					XmNscaleMultiple, 1,
					XmNshowValue, True,
					XmNorientation, XmHORIZONTAL,
					XmNleftAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNtopAttachment, XmATTACH_WIDGET,
					XmNtopWidget, frameTimeScale,
					XmNtopOffset, 10,
					NULL);
  XtAddCallback (endIntScale, XmNdragCallback, changeWaitInt, NULL);
  XtAddCallback (endIntScale, XmNvalueChangedCallback, changeWaitInt, NULL);

}
