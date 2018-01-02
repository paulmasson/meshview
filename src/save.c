#define SAVE_C
#include "all.h"

void save_settings(char *filename) 
{
  FILE *out;
  int i, j;

  if((out = fopen(filename, "w")) == NULL) {
    printf("can't create file %s\n", filename);
    return;
  } 

  fprintf(out, "SETTING\n");
  fprintf(out, "NEAR %f\n", near);
  fprintf(out, "FAR %f\n", far);
  fprintf(out, "FDIST %f\n", fdist);
  fprintf(out, "EYE %f\n", eye);
  fprintf(out, "SCALE %f\n", scale);
  fprintf(out, "SPECULAR %f\n", specular);
  fprintf(out, "SHININESS %f\n", shininess);
  fprintf(out, "MSENSE %f\n", m_sense);
  fprintf(out, "MSPEED %f\n", m_speed);
  fprintf(out, "POLARD %f\n", polard);
  fprintf(out, "VCUBESIZE %f\n", vcubesize);
  fprintf(out, "SCREENDOORGAP %f\n", scrdoorgap);
  fprintf(out, "BGRED %f\n", BGred);
  fprintf(out, "BGGREEN %f\n", BGgreen);
  fprintf(out, "BGBLUE %f\n", BGblue);
  fprintf(out, "BGALPHA %f\n", BGalpha);
  fprintf(out, "LIGHTPOS %f %f %f\n", ltp[0], ltp[1], ltp[2]);
  fprintf(out, "4DMAT\n");
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++)
      fprintf(out, " %f", mat4.d[4*i+j]);
    fprintf(out, "\n");
  }
  fprintf(out, "3DROTMAT\n");
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++)
      fprintf(out, " %f", rot_mat.d[4*i+j]);
    fprintf(out,"\n");
  }
  fprintf(out, "3DTRANMAT\n");
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++)
      fprintf(out, " %f", trans_mat.d[4*i+j]);
    fprintf(out,"\n");
  }
  fclose(out);
}

void save_ppm(char *filename)
{
  FILE *out;
  int nx, ny, i, j, k;
  GLubyte *pdata; /* unsigned char, 8 bit */

  out = fopen(filename, "w");
  if(out == NULL) {
    printf("can't create file %s\n", filename);
    return;
  }  

  nx = glw_width;
  ny = glw_height;
  printf("size of window: %dx%d\n", nx, ny);
  
  pdata = (GLubyte *) malloc(sizeof(GLubyte) * nx * ny *3);
  if(pdata == NULL) {
    printf("malloc failed."
	   " Free some memory or reduce the size of the window.\n");
    return;
  }
  glPixelStorei (GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_FRONT); 
  glReadPixels(0, 0, nx, ny, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *) pdata);
  fprintf(out, "P6\n%d %d\n255\n", nx, ny);
  for(j=ny-1; j>=0; j--)
    for(i=0; i<=nx-1; i++)
      for(k=0; k<3; k++) 
	fputc((int) pdata[3*(i+j*nx)+k], out);
  fclose(out);
  free(pdata);
  
  printf("%s has been saved.\n", filename);
}
