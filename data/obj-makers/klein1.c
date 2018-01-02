/* From David Banks (1993), A Gallery of Surfaces in Four Dimensions 
   Klein bottle, created by rotating a circle by a half-turn in 5-space
   then projecting down the u-aixs into 4-space. The image of the 
   transparent surface contains an opaque Mobius band

   x = sin(t), 
   y = cos(t), 
   z = cos(s)*cos(t/2), 
   w = sin(s), 
   u = -cos(s)*sin(t/2)

   Compile "cc -o klein1 klein1.c -lm".
   It generates Klein1.4mesh.

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
    printf("Usage : klein1 <nu> <nv>\n");
    exit(1);
  }

  nu = atoi(argv[1]);
  nv = atoi(argv[2]);
  printf("nu = %d, nv = %d\n", nu, nv);

  if ((out = fopen("Klein1.4mesh", "w")) == NULL) {
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
      x = cos(u);
      y = sin(u);
      z = cos(v)*cos(u/2.0);
      w = sin(v);
      fprintf(out, "%f %f %f %f\n", x, y, z, w);
      u = u + du;
    }
    v = v + dv;
  }
}
