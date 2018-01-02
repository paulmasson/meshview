/* Got equations from George Francis via WWW of geom.umn.edu. 
   Blaine Lawson's class Tau of ruled, minimal surfaces on S3
   have this quaternionic parameterization : 
     (u,v) -> e^{2iu}*cos(v) + j e^{biu}*sin(v)
   which can yield the once-twisted annulus, Clifford torus, 
   3.5-twisted Moebius band, and Brehm's smooth knotbox.

   Hui Ma, July 11, 1994
*/


#include <stdio.h>
#include <math.h>

main(int argc, char *argv[]) {
  int i, j;
  int nu, nv, theta, phi;
  char *filename;
  double x, y, z, w;
  double du, dv, u, v, b;
  FILE *out; 

  if(argc != 7) {
    printf("Usage : snail <nu> <nv> <b*10> <theta> <phi> <filename>.\n");
    exit(0);
  }

  nu = atoi(argv[1]);
  nv = atoi(argv[2]);
  b = (double) atoi(argv[3]) * 0.1;
  theta = atoi(argv[4]);
  phi = atoi(argv[5]);
  filename = argv[6];

  printf("b = %f\n", b);

  out = fopen(filename, "w");
  if(out == NULL) {
    printf("can't open %s\n", filename);
    exit(2);
  }
  fprintf(out, "4MESH\n%d %d\n", nu, nv);

  du = 2.0*M_PI / (double) nu * (double) theta / 360.0;  
  dv = 2.0*M_PI / (double) nv * (double) phi / 360.0;
  
  v = 0.0;
  for(j=0; j<nv; j++) {
    u = 0.0;
    for(i=0; i<nu; i++) {
      x = cos(2.0*u) * cos(v);
      y = sin(2.0*u) * cos(v);
      z = cos(b*u) * sin(v);
      w = -sin(b*u) * sin(v);
      fprintf(out, "%f %f %f %f\n", x, y, z, w);
      u += du;
    }
    v += dv;
  }
  fclose(out);
}
	       
