#define MAIN_C
#include "all.h"

/* add in later, from mw.src */
static String fallback_resources[] = {
  "meshview*background: gray80",
  "meshview*title: MeshView 1.2",
  "meshview*iconName: MeshView",
  "meshview*fontList: \
-*-courier-*-r-*--14-*=defaultset,\
-*-courier-*-r-*--12-*=smallset,\
-*-courier-*-r-*--18-*=bigset",
  /* main drawing area */
  "meshview*glwidget*width:               600",
  "meshview*glwidget*height:              500",
  "meshview*glwidget*doublebuffer:       TRUE",
  "meshview*glwidget*allocateBackground: TRUE",
  /* picker drawing area */
  "meshview*dawpo*width:                  300",
  "meshview*dawpo*height:                 200",
  "meshview*dawpo*background:    DeepSkyBlue4",
  /* quatrot drawing area */
  "meshview*glwquat*width:                  300",
  "meshview*glwquat*height:                 300",
  "meshview*glwquat*doublebuffer:       TRUE",
  "meshview*glwquat*background:    DeepSkyBlue4",
  /* frames in frames and picker Dialogs*/
  "meshview*frame*shadowType:       SHADOW_IN", 
  "meshview*helpList.height:              500",
  "meshview*pickerHelpList.height:        300",
  "meshview*framesHelpList.height:        300",

  "meshview*helpList*background: White",
  "meshview*pickerHelpList*background: White",
  "meshview*framesHelpList*background: White",
  "meshview*frIndicator.background: White",
  "meshview*aboutBox.background: White",
  "meshview*transient : FALSE",         /* for popup dialogs */
  "meshview*selectColor : Yellow",      /* for toggle buttons */
  "meshview*Face.set : True",           /* for toggle buttons */
  "meshview*Momentum.set : True",       /* for toggle buttons */
  "meshview*Context_free.set : True",   /* for toggle buttons */
  "meshview*Object_unit_size.set : True",    /* for toggle buttons */
  NULL
}; 

void main(int argc, char *argv[]) {
  int i;

  globalVarInit();
  
  XtSetLanguageProc(NULL, NULL, NULL);
/* topLevel and app_context are global variables */
  topLevel = XtAppInitialize(&app_context, /* Application context */
			     "meshview",   /* Application class */
			     NULL, 0,      /* command line option list */
			     &argc, argv,  /* command line args */
			     fallback_resources,
			     NULL,         /* argument list */ 
			     0);           /* number of arguments */  

  dpy = XtDisplay(topLevel);
  scr = XtScreen(topLevel);
  cmap = DefaultColormapOfScreen (scr);
 
  build_ui();
  ui_init();
  XtRealizeWidget(topLevel);

/* read in filenames after XtAppInitialize parsed the argc and argv */
  for(i=1; i<argc; i++)
    load_data(argv[i], ADD);

  XtAppMainLoop(app_context);
}
