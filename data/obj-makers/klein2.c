/* From David Banks (1993), A Gallery of Surfaces in Four Dimensions 
   Klein bottle, shaped as two crosscaps glued together. This 
   representation rotates a circle by a half-turn while its radius
   varies.

   x = (1.0 + 0.3*cos(t)) * cos(s);
   y = (1.0 + 0.3*cos(t)) * cos(t/2) * sin(s);
   z = -(1.0 + 0.3*cos(t)) * sin(t/2) * sin(s);
   w = cos(t);

   Compile "cc -o klein2 klein2.c -lm".
   It generates Klein2.4mesh.

   Hui Ma, July 12, 1994
*/

#include <stdio.h>
#include <math.h>

main(int argc, char *argv[]) {
  FILE *out;
  int i, j, nu, nv;
  double du, dv, u, v;
  double x, y, z, w;

  if(argc != 3) {
    printf("Usage : klein2 <nu> <nv>\n");
    exit(1);
  }

  nu = atoi(argv[1]);
  nv = atoi(argv[2]);
  printf("nu = %d, nv = %d\n", nu, nv);

  if ((out = fopen("Klein2.4mesh", "w")) == NULL) {
    fprintf(stderr, "error: Could not open file.\n");
    exit(1);
  }
  
  fprintf(out, "4MESH\n%d %d\n",nu, nv);
  du = 2.0*M_PI / (double) (nu-1.0);
  dv = 2.0*M_PI / (double) (nv-1.0);

  v = 0.0;
  for (j=0; j<nv; j++) {
    u = 0.0;
    for (i=0; i<nu; i++) { 
      x = (1.0 + 0.3*cos(u)) * cos(v);
      y = (1.0 + 0.3*cos(u)) * cos(u/2) * sin(v);
      z = -(1.0 + 0.3*cos(u)) * sin(u/2) * sin(v);
      w = cos(u);
      fprintf(out, "%f %f %f %f\n", x, y, z, w);
      u = u + du;
    }
    v = v + dv;
  }
}
