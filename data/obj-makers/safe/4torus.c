#include <stdio.h>
#include <math.h>

int ni, nj;

int index(int i, int j) {
  return ( (i%ni) + (j%nj)*ni );
}

main(int argc, char * argv[]) {
  FILE *out1, *out2;
  int i, j;
  double dtheta, dphi;
  double x, y, z, w;
  double tmp, deg;

  if(argc != 3) {
    printf("Usage : 4torus <ni> <nj>\n");
    exit(0);
  }
  ni = atoi(argv[1]);
  nj = atoi(argv[2]);

  dtheta = 2.0*M_PI/(double)ni;
  dphi = 2.0*M_PI/(double)nj;

  out1 = fopen("4torus.4mesh", "w");
  out2 = fopen("4torus.4off", "w");

  fprintf(out1, "4uvMESH\n%d %d\n", ni,nj);
  fprintf(out2, "4OFF\n%d %d %d\n", ni*nj, 2*ni*nj, 2*ni*nj);

  deg = M_PI / 4.0;
  for(j=0; j<nj; j++) {
    for(i=0; i<ni; i++) {
      x = cos(dtheta * (double)i);
      y = sin(dtheta * (double)i);
      z = cos(dphi * (double)j);
      w = sin(dphi * (double)j);
      tmp = x * cos(deg) - w * sin(deg);
      w = x * sin(deg) + w * cos(deg);
      x = tmp; 
      fprintf(out1, "%f %f %f %f\n", x,y,z,w);
      fprintf(out2, "%f %f %f %f\n", x,y,z,w);
    }
  }
  for(j=0; j<nj; j++) {
    for(i=0; i<ni; i++) {
/*      if((i-j)%2 == 0) { */
      if(1) {
	fprintf(out2, "3 %d %d %d\n", 
		index(i,j), index(i+1,j), index(i+1,j+1));
	fprintf(out2, "3 %d %d %d\n", 
		index(i,j), index(i+1,j+1), index(i,j+1));
      }
      else {
	fprintf(out2, "3 %d %d %d\n", 
		index(i,j), index(i+1,j), index(i,j+1));
	fprintf(out2, "3 %d %d %d\n", 
		index(i+1,j), index(i+1,j+1), index(i,j+1));
      }
    }
  }
}
