#define UI_SETTING_C
#include "all.h"

void color_value(Widget scale_w, XtPointer client_data, XtPointer call_data) {
    int rgb = (int) client_data;
    XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

    switch (rgb) {
        case RED :
            color.red = (cbs->value << 8);
            break;
        case GREEN :
            color.green = (cbs->value << 8);
            break;
        case BLUE :
            color.blue = (cbs->value << 8);
    }

    /* reuse the same color again and again */
    XFreeColors (dpy, cmap, &color.pixel, 1, 0);
    if (!XAllocColor (dpy, cmap, &color)) {
        puts ("Couldn't XAllocColor!"); 
	exit(1);
    }
    XtVaSetValues (colorwindow, XmNbackground, color.pixel, NULL);
}

void build_uiSetting(Widget menuBar) {
  int n;
  Arg args[100];
  Widget rowcolS, rowcolC, rowcol;
  Widget bgButton, colorSelectDialog;
  XtVarArgsList arglist;

  n = 0;
  XtSetArg(args[n], XmNtitle, "setting1"); n++;
  setting1Dialog = XmCreateMessageDialog(menuBar, "setting1Dialog", 
					args, n);
  XtVaSetValues(setting1Dialog, XtVaTypedArg, XmNhelpLabelString, 
		XmRString, "Default", 8, NULL);
  XtAddCallback(setting1Dialog, XmNokCallback, setting1_ok, 0);
  XtAddCallback(setting1Dialog, XmNcancelCallback, setting1_cancel, 0);
  XtAddCallback(setting1Dialog, XmNhelpCallback, setting1_default, 0);

/* RowColumn widget for settingDialog */
  rowcolS = XtVaCreateManagedWidget ("rowcolS",
             xmRowColumnWidgetClass, setting1Dialog,
	     XmNorientation, XmVERTICAL,
	     NULL); 
  nearScale = XtVaCreateManagedWidget ("nearScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "Near", 5,
        XmNmaximum,   399, /* 1 smaller than Far max */
        XmNminimum,   1,
	XmNdecimalPoints, 1,
        XmNscaleMultiple, 5,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,				   
        NULL);
  XtAddCallback (nearScale, XmNdragCallback, new_proj, (XtPointer) NEAR);
  XtAddCallback (nearScale, XmNvalueChangedCallback, new_proj,(XtPointer)NEAR);
  farScale = XtVaCreateManagedWidget ("farScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "Far", 4,
        XmNmaximum,   400,
        XmNminimum,   2,   /* 1 bigger than Near min */
	XmNdecimalPoints, 1,
        XmNscaleMultiple, 5,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,				   
	NULL);
  XtAddCallback (farScale, XmNdragCallback, new_proj, (XtPointer) FAR);
  XtAddCallback (farScale, XmNvalueChangedCallback, new_proj, (XtPointer) FAR);
  fdistScale = XtVaCreateManagedWidget ("fdistScale",
	xmScaleWidgetClass, rowcolS,
	XtVaTypedArg, XmNtitleString, XmRString, "Fdist", 4,
	XmNmaximum,   350,
	XmNminimum,   10,
	XmNdecimalPoints, 1,
        XmNscaleMultiple, 2,
	XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,				   
        NULL);
  XtAddCallback (fdistScale, XmNdragCallback, new_proj, (XtPointer) FDIST);
  XtAddCallback (fdistScale, XmNvalueChangedCallback, new_proj, 
		 (XtPointer) FDIST);
  eyeScale = XtVaCreateManagedWidget ("eyeScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "Eye distance (Stereo)", 20,
        XmNmaximum,   100,
        XmNminimum,   0,
	XmNdecimalPoints, 1,
        XmNscaleMultiple, 2,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,				   
	NULL);
  XtAddCallback (eyeScale, XmNdragCallback, new_proj, (XtPointer) EYE);
  XtAddCallback (eyeScale, XmNvalueChangedCallback, new_proj, (XtPointer) EYE);
  scaleScale = XtVaCreateManagedWidget ("scaleScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "Scale", 6,
        XmNmaximum,   50,
        XmNminimum,   1,
	XmNdecimalPoints, 1,
        XmNscaleMultiple, 5,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,				   
	NULL);
  XtAddCallback (scaleScale, XmNdragCallback, new_scale, NULL);
  XtAddCallback (scaleScale, XmNvalueChangedCallback, new_scale, NULL);

  polardScale = XtVaCreateManagedWidget ("polardScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "polar distance", 20,
        XmNmaximum,   500,
        XmNminimum,   1,
	XmNdecimalPoints, 2,
        XmNscaleMultiple, 5,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,
        NULL);
  XtAddCallback (polardScale, XmNdragCallback, 
		 new_polard, (XtPointer) NULL);
  XtAddCallback (polardScale, XmNvalueChangedCallback, 
		 new_polard, (XtPointer) NULL);

  /* second settings dialog */
  n = 0;
  XtSetArg(args[n], XmNtitle, "setting2"); n++;
  setting2Dialog = XmCreateMessageDialog(menuBar, "setting2Dialog", 
					args, n);
  XtVaSetValues(setting2Dialog, XtVaTypedArg, XmNhelpLabelString, 
		XmRString, "Default", 8, NULL);
  XtAddCallback(setting2Dialog, XmNokCallback, setting2_ok, 0);
  XtAddCallback(setting2Dialog, XmNcancelCallback, setting2_cancel, 0);
  XtAddCallback(setting2Dialog, XmNhelpCallback, setting2_default, 0);

/* RowColumn widget for settingDialog */
  rowcolS = XtVaCreateManagedWidget ("rowcolS",
             xmRowColumnWidgetClass, setting2Dialog,
	     XmNorientation, XmVERTICAL,
	     NULL); 
  specularScale = XtVaCreateManagedWidget ("specularScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "Specular", 9,
        XmNmaximum,   100,
        XmNminimum,   0,
	XmNdecimalPoints, 2,
        XmNscaleMultiple, 10,
	XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,				   
        NULL);
  XtAddCallback (specularScale, XmNdragCallback, 
		 new_lighting, (XtPointer) SPECULAR );
  XtAddCallback (specularScale, XmNvalueChangedCallback, 
		 new_lighting, (XtPointer) SPECULAR);
  shininessScale = XtVaCreateManagedWidget ("shininessScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "Shininess", 10,
        XmNmaximum,   128,
        XmNminimum,   1,
	XmNdecimalPoints, 0,
        XmNscaleMultiple, 10,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,
        NULL);
  XtAddCallback (shininessScale, XmNdragCallback, 
		 new_lighting, (XtPointer) SHININESS );
  XtAddCallback (shininessScale, XmNvalueChangedCallback, 
		 new_lighting, (XtPointer) SHININESS);
  sensitiveScale = XtVaCreateManagedWidget ("sensitiveScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "mouse-sensitivity", 20,
        XmNmaximum,   10,
        XmNminimum,   1,
	XmNdecimalPoints, 0,
        XmNscaleMultiple, 1,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,
        NULL);
  XtAddCallback (sensitiveScale, XmNdragCallback, 
		 new_sense, (XtPointer) NULL);
  XtAddCallback (sensitiveScale, XmNvalueChangedCallback, 
		 new_sense, (XtPointer) NULL);
  speedScale = XtVaCreateManagedWidget ("speedScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "mouse-speed", 20,
        XmNmaximum,   100,
        XmNminimum,   1,
	XmNdecimalPoints, 1,
        XmNscaleMultiple, 5,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,
        NULL);
  XtAddCallback (speedScale, XmNdragCallback, 
		 new_speed, (XtPointer) NULL);
  XtAddCallback (speedScale, XmNvalueChangedCallback, 
		 new_speed, (XtPointer) NULL);
  vcubesizeScale = XtVaCreateManagedWidget ("vcubesizeScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "vertex cube size", 20,
        XmNmaximum,   500,
        XmNminimum,   10,
	XmNdecimalPoints, 2,
        XmNscaleMultiple, 1,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,
        NULL);
  XtAddCallback (vcubesizeScale, XmNdragCallback, 
		 new_vcube_size, (XtPointer) NULL);
  XtAddCallback (vcubesizeScale, XmNvalueChangedCallback, 
		 new_vcube_size, (XtPointer) NULL);
  scrDoorScale = XtVaCreateManagedWidget ("scrDoorScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "screen door gap", 20,
        XmNmaximum,   100,
        XmNminimum,   1,
	XmNdecimalPoints, 2,
        XmNscaleMultiple, 1,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,
        NULL);
  XtAddCallback (scrDoorScale, XmNdragCallback, 
		 changeScrDoorGap, (XtPointer) NULL);
  XtAddCallback (scrDoorScale, XmNvalueChangedCallback, 
		 changeScrDoorGap, (XtPointer) NULL);
  sphRowsScale = XtVaCreateManagedWidget ("sphRowsScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "unit sphere num of rows", 20,
        XmNmaximum,   32,
        XmNminimum,   4,
	XmNdecimalPoints, 0,
        XmNscaleMultiple, 2,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,
        NULL);
  XtAddCallback (sphRowsScale, XmNdragCallback, 
		 changeSphRows, (XtPointer) NULL);
  XtAddCallback (sphRowsScale, XmNvalueChangedCallback, 
		 changeSphRows, (XtPointer) NULL);

  sphSizeScale = XtVaCreateManagedWidget ("sphSizeScale",
        xmScaleWidgetClass, rowcolS,
        XtVaTypedArg, XmNtitleString, XmRString, "unit sphere size", 20,
        XmNmaximum,   150,
        XmNminimum,   50,
	XmNdecimalPoints, 2,
        XmNscaleMultiple, 1,
        XmNshowValue, True,
        XmNorientation, XmHORIZONTAL,
        NULL);
  XtAddCallback (sphSizeScale, XmNdragCallback, 
		 changeSphSize, (XtPointer) NULL);
  XtAddCallback (sphSizeScale, XmNvalueChangedCallback, 
		 changeSphSize, (XtPointer) NULL);

   bgButton = XtVaCreateManagedWidget ("Background",
                  xmPushButtonWidgetClass, rowcolS,
                  XtVaTypedArg, XmNtitleString, XmRString, "Background", 11,
		  NULL);
 
  n = 0;
  XtSetArg(args[n], XmNtitle, "color"); n++;
  colorSelectDialog = XmCreateMessageDialog(bgButton, "colorSelectDialog", 
					    args, n);
  XtVaSetValues(colorSelectDialog,
		XtVaTypedArg, XmNhelpLabelString, XmRString, "Default", 8,
		NULL);
  XtAddCallback(colorSelectDialog, XmNokCallback, colorSelect_ok, 0);
  XtAddCallback(colorSelectDialog, XmNcancelCallback, colorSelect_cancel, 0);
  XtAddCallback(colorSelectDialog, XmNhelpCallback, colorSelect_default, 0);

  color.flags = DoRed | DoGreen | DoBlue;
/* initialize first color */
   XAllocColor (dpy, cmap, &color);
/* RowColumn widget for colorSelectDialog */
  rowcolC = XtVaCreateManagedWidget ("rowcolC",
             xmRowColumnWidgetClass, colorSelectDialog,
	     XmNorientation, XmVERTICAL,
	     NULL);
  colorwindow = XtVaCreateManagedWidget ("colorwindow",
	widgetClass,   rowcolC,
        XmNheight,     100,
        XmNbackground, color.pixel,
        NULL);
/* use rowcol again to create another Rowcolumn under the 1st */
  rowcol = XtVaCreateManagedWidget ("rowcol", 
        xmRowColumnWidgetClass, rowcolC,
        XmNorientation, XmVERTICAL,
        NULL);
  arglist = XtVaCreateArgsList (NULL,
	XmNshowValue, True,
	XmNmaximum, 255,
        XmNscaleMultiple, 5,
        XmNorientation, XmHORIZONTAL,				   
        NULL);
  redScale = XtVaCreateManagedWidget ("redScale",
        xmScaleWidgetClass, rowcol,
        XtVaNestedList, arglist,
        XtVaTypedArg, XmNtitleString, XmRString, "Red", 4,
        XtVaTypedArg, XmNforeground, XmRString, "Red", 4,
        NULL);
  XtAddCallback(redScale, XmNdragCallback, color_value, (XtPointer) RED);
  XtAddCallback(redScale, XmNvalueChangedCallback,color_value,(XtPointer) RED);
  XtAddCallback(redScale, XmNdragCallback, bg_color, (XtPointer) RED);
  XtAddCallback(redScale, XmNvalueChangedCallback, bg_color, (XtPointer) RED);

  greenScale = XtVaCreateManagedWidget ("greenScale",
	xmScaleWidgetClass, rowcol,
        XtVaNestedList, arglist,
        XtVaTypedArg, XmNtitleString, XmRString, "Green", 6,
        XtVaTypedArg, XmNforeground, XmRString, "Green", 6,
        NULL);
  XtAddCallback(greenScale, XmNdragCallback, color_value, (XtPointer) GREEN);
  XtAddCallback(greenScale, XmNvalueChangedCallback,
		color_value, (XtPointer) GREEN);
  XtAddCallback(greenScale, XmNdragCallback, bg_color, (XtPointer) GREEN);
  XtAddCallback(greenScale, XmNvalueChangedCallback, 
		bg_color, (XtPointer) GREEN);

  blueScale = XtVaCreateManagedWidget ("blueScale",
        xmScaleWidgetClass, rowcol,
        XtVaNestedList, arglist,
        XtVaTypedArg, XmNtitleString, XmRString, "Blue", 5,
        XtVaTypedArg, XmNforeground, XmRString, "Blue", 5,
        NULL);
  XtAddCallback (blueScale, XmNdragCallback, color_value, (XtPointer) BLUE);
  XtAddCallback (blueScale, XmNvalueChangedCallback, 
		 color_value, (XtPointer) BLUE);
  XtAddCallback (blueScale, XmNdragCallback, bg_color, (XtPointer) BLUE);
  XtAddCallback (blueScale, XmNvalueChangedCallback, bg_color, 
		 (XtPointer) BLUE);
  XtFree (arglist);
  XtAddCallback (bgButton, XmNactivateCallback, ShowBox, colorSelectDialog);
}
