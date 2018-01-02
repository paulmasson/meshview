#include <stdio.h>
#include <math.h>

main(int argc, char * argv[]) {
  FILE *out;
  int ni, nj, i, j;
  double dtheta, dphi;
  double x, y, z;

  if(argc != 3) {
    printf("Usage : sphere <ni> <nj>\n");
    exit(0);
  }
  ni = atoi(argv[1]);
  nj = atoi(argv[2]);

  dtheta = 2.0*M_PI/(double)ni;
  dphi = M_PI/(double)(nj-1);

  out = fopen("sphere.3mesh", "w");
 
  fprintf(out, "uMESH\n%d %d\n", ni,nj);
  for(j=0; j<nj; j++) {
    for(i=0; i<ni; i++) {
      x = cos(dtheta * (double)i) * sin(dphi * (double)j);
      y = sin(dtheta * (double)i) * sin(dphi * (double)j);
      z = cos(dphi * (double)j);
      fprintf(out, "%f %f %f\n", x,y,z);
    }
  }
}
