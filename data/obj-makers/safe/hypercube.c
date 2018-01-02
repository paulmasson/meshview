#include <stdio.h>

main() {
  int i, j, k, l, m, n, p, k1, k2, count;
  double X[4], x[4][4], r, g, b;
  FILE *out, *out2;
  char filename[100];

  out = fopen("hypercube.list", "w");
  fprintf(out, "LIST\n");

  count = 0;
  for(i=0; i<4; i++) {
    for(j=i+1; j<4; j++) {
      x[0][i] = -1.0; x[1][i] = 1.0; x[2][i] = -1.0; x[3][i] = 1.0;
      x[0][j] = -1.0; x[1][j] = -1.0; x[2][j] = 1.0; x[3][j] = 1.0;
      for(k=0; k<4; k++)
	if(k!=i && k!=j) {
	  k1 = k;
	  break;
	}
      for(k=0; k<4; k++) {
	if(k!=i && k!=j && k!=k1) 
	  k2 = k;
      }
/* now i, j, k1, k2 are four different indices */
      for(k=-1; k<=1; k+=2) {
	for(l=-1; l<=1; l+=2) {
	  sprintf(filename, "/nfs/ireland/proj/apps/meshview/data/hypercube/hypercube-%1d%1d%1d.4mesh", count, (k+1)/2, (l+1)/2);
	  fprintf(out, "%s\n", filename);
	  out2 = fopen(filename, "w");
	  fprintf(out2, "4MESHC\n");
	  x[0][k1] = x[1][k1] = x[2][k1] = x[3][k1] = (double) k;
	  x[0][k2] = x[1][k2] = x[2][k2] = x[3][k2] = (double) l;
	  g = 0.2 * (double)count;
	  r = 0.3 + 0.4 * (k + 1.0)/2.0;
	  b = 0.3 + 0.4 * (l + 1.0)/2.0;
	  fprintf(out2, "%f %f %f %f\n", r, g, b, 1.0);
/*
	  fprintf(out2, "3 3\n");
	  for(m=0; m<3; m++)
	    for(n=0; n<3; n++) {
	      for(p=0; p<4; p++) {
		X[p]  = x[0][p] + m * (x[1][p]-x[0][p])/2.0
		                + n * (x[2][p]-x[0][p])/2.0;
	      }
	      fprintf(out2, "%f %f %f %f\n", 
		      X[0], X[1], X[2], X[3]);
	    }
*/
	  fprintf(out2, "2 2\n");
	  for(m=0; m<4; m++)
	    fprintf(out2, "%f %f %f %f\n",
		    x[m][0], x[m][1], x[m][2], x[m][3]);
	}
      }
      count++;
    }
  }
  fclose(out);
}

