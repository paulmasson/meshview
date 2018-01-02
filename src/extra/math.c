#define MATH_C
#include "all.h"

/* Part of the 'walker' algorithm for 3D.
   INPUT  : the triangle;
            the edge;
	    the hit point on the edge;
	    the hit direction(unit); 
   OUTPUT : new triangle;
            new edge;
	    new vertex; (either this or above one is NULL)
            new hit point;
            new direction(unit); 
            edge/vertex-hit flag;
*/
void edgeHit3(Face *oldf, Edge *olde,  
	      Point3 *oldp, Vector3 *oldv,
	      Face *newf, Edge *newe, Vertex *newx,
	      Point3 *newp, Vector3 *newv, int *newflag) {
  Vertex *vertex0, *vertex1, *vertex2; /* vertices of the triangle */
  Vector3 v1;       /* vertex1 - vertex0 */
  Vector3 v2;       /* vertex2 - vertex0 */
  Vector3 v3;       /* vertex2 - vertex1 */
  double d11, d12, d22; /* v1 dot v1, v1 dot v2, v2 dot v2 */
  double d33;       /* v3 dot v3 */
  Vector3 qp0;      /* oldp - vertex0 */
  Vector3 qp1;      /* oldp - vertex1 */
  double qp01;      /* qp0 dot v1 */
  double qp02;      /* qp0 dot v2 */
  double d1;        /* oldv dot v1 */
  double d2;        /* oldv dot v2 */
  double dn1;       /* newv dot v1 */
  double dn2;       /* newv dot v2 */
  double dn3;       /* newv dot v3 */
  double alpha, beta, lambda, mu;
  double b1, b2;
  double inv;
  int k;

  vertex0 = olde->vertices[0];
  vertex1 = olde->vertices[1];
  if(olde->faces[0] == oldf)
    vertex2 = olde->vx3rd[0];
  else if(olde->faces[1] == oldf)
    vertex2 = olde->vx3rd[1];
  else {
    printf("Error in edgeHit3: olde doesn't bound oldf.\n");
    exit(1);
  }
  v1 = VertexMinusVertex3(vertex1, vertex0);
  v2 = VertexMinusVertex3(vertex2, vertex0);
  d11 = Dot3(&v1, &v1);
  d12 = Dot3(&v1, &v2);
  d22 = Dot3(&v2, &v2);
  
  qp0 = PointMinusVertex3(oldp, vertex0);
  qp01 = Dot3(&qp0, &v1);
  qp02 = Dot3(&qp0, &v2);
  d1 = Dot3(oldv, &v1);
  d2 = Dot3(oldv, &v2);

  inv = 1.0 / (d11 * d22 - d12 * d12);
  b1 = qp01 + d1;
  b2 = qp02 + d2;
  alpha = (b1 * d22 - b2 * d12) * inv;
  beta = (d11 * b2 - d12 * b1) * inv;
  if(alpha < -TINYEPSILON || beta < -TINYEPSILON)
    printf("Warning: wrong alpha and beta -- %f %f\n",
	   alpha, beta);
  newv->d[0] = alpha * v1.d[0] + beta * v2.d[0] - qp0.d[0]; 
  newv->d[1] = alpha * v1.d[1] + beta * v2.d[1] - qp0.d[1]; 
  newv->d[2] = alpha * v1.d[2] + beta * v2.d[2] - qp0.d[2]; 
  Normalize3(newv);

  dn1 = Dot3(newv, &v1);
  dn2 = Dot3(newv, &v2);

  b1 = Dot3(&qp0, &v2);
  b2 = Dot3(&qp0, newv);
  lambda = (b1-b2*dn2) / (d22-dn2*dn2);
  mu = -(d22*b2-dn2*b1) / (d22-dn2*dn2);
  printf("lambda = %f, mu = %f\n", lambda, mu);

  if(lambda > -TINYEPSILON && lambda < 1.0+TINYEPSILON) {
    if(lambda < TINYEPSILON) {
      printf("Error : input is not an edge-hit\n");
      exit(2);
    }
    else if(lambda > 1.0-TINYEPSILON) {
      *newflag = VERTEX;
      newx = vertex2;
    }
    else {
      newp->d[0] = vertex0->p3.d[0] + lambda * v2.d[0];
      newp->d[1] = vertex0->p3.d[1] + lambda * v2.d[1];
      newp->d[2] = vertex0->p3.d[2] + lambda * v2.d[2];
      *newflag = EDGE;
      newe = NULL;
      for(k=0; k<3; k++) {
	if(oldf->edges[k] != olde) {
	  if( ((oldf->edges[k]->vertices[0] == vertex0) ||
	       (oldf->edges[k]->vertices[0] == vertex2)) &&
	      ((oldf->edges[k]->vertices[1] == vertex0) ||
	       (oldf->edges[k]->vertices[1] == vertex2))   ) {
	    newe = oldf->edges[k];
	    break;
	  }
	}
      }
      if(newe == NULL) {
	printf("strange : didn't find the hit edge\n");
	exit(3);
      }
      if(newe->faces[0] != oldf) 
	newf = newe->faces[0];
      else
	newf = newe->faces[1];
    }
    return;
  }
  /* try another edge */
  v3 = VertexMinusVertex3(vertex2, vertex1);
  d33 = Dot3(&v3, &v3);
  dn3 = Dot3(newv, &v3);
  qp1 = PointMinusVertex3(oldp, vertex1);
  b1 = Dot3(&qp1, &v3);
  b2 = Dot3(&qp1, newv);
  lambda = (b1-b2*dn3) / (d33-dn3*dn3);
  mu = -(d33*b2-dn3*b1) / (d33-dn3*dn3);
  printf("lambda = %f, mu = %f\n", lambda, mu);

  if(lambda > -TINYEPSILON && lambda < 1.0+TINYEPSILON) {
    if(lambda < TINYEPSILON) {
      printf("Error : input is not an edge-hit\n");
      exit(2);
    }
    else if(lambda > 1.0-TINYEPSILON) {
      *newflag = VERTEX;
      newx = vertex2;
    }
    else {
      newp->d[0] = vertex1->p3.d[0] + lambda * v3.d[0];
      newp->d[1] = vertex1->p3.d[1] + lambda * v3.d[1];
      newp->d[2] = vertex1->p3.d[2] + lambda * v3.d[2];
      *newflag = EDGE;
      newe = NULL;
      for(k=0; k<3; k++) {
	if(oldf->edges[k] != olde) {
	  if( ((oldf->edges[k]->vertices[0] == vertex1) ||
	       (oldf->edges[k]->vertices[0] == vertex2)) &&
	      ((oldf->edges[k]->vertices[1] == vertex1) ||
	       (oldf->edges[k]->vertices[1] == vertex2))   ) {
	    newe = oldf->edges[k];
	    break;
	  }
	}
      }
      if(newe == NULL) {
	printf("strange : didn't find the hit edge\n");
	exit(3);
      }
      if(newe->faces[0] != oldf) 
	newf = newe->faces[0];
      else
	newf = newe->faces[1];
    }
  }
}

/* Part of the 'walker' algorithm for 3D.
   INPUT  : the triangle;
            the vertex;
	    the hit direction(unit); 
   OUTPUT : new triangle;
            new edge;
	    new vertex; (either this or above one is NULL)
            new hit vertex;
            new direction(unit); 
            edge/vertex-hit flag;
*/
void vertexHit3(Face *oldf, Vertex *oldx, Vector3 *oldv,
		Face *newf, Edge *newe, Vertex *newx,
		Point3 *newp, Vector3 *newv, int *newflag) {
  Vertex *vertex1, *vertex2; /* vertices of the triangle */
  Vector3 v1;       /* vertex1 - oldx */
  Vector3 v2;       /* vertex2 - oldx */
  Vector3 v3;       /* vertex2 - vertex1 */
  double d11, d12, d22; /* v1 dot v1, v1 dot v2, v2 dot v2 */
  double d33;       /* v3 dot v3 */
#if 0
  double d1;        /* oldv dot v1 */
  double d2;        /* oldv dot v2 */
  double dn1;       /* newv dot v1 */
  double dn2;       /* newv dot v2 */
#endif
  double dn3;       /* newv dot v3 */
  double alpha, beta, lambda, mu;
  double b1, b2;
  double inv;
  Face *face;
  int i, k;

  for(i=0; i<oldx->Nfaces; i++) {
    face = oldx->faces[i];
    for(k=0; k<3; k++) {
      if(face->vertices[k] == oldx) {
	vertex1 = face->vertices[idx[k+1]];
	vertex2 = face->vertices[idx[k+2]];
	break;
      }
    }
    v1 = VertexMinusVertex3(vertex1, oldx);
    v2 = VertexMinusVertex3(vertex2, oldx);
    d11 = Dot3(&v1, &v1);
    d12 = Dot3(&v1, &v2);
    d22 = Dot3(&v2, &v2);
  
    b1 = Dot3(oldv, &v1);
    b2 = Dot3(oldv, &v2);
    inv = 1.0 / (d11 * d22 - d12 * d12);
    alpha = (b1 * d22 - b2 * d12) * inv;
    beta = (d11 * b2 - d12 * b1) * inv;
    printf("alpha = %f, beta = %f\n", alpha, beta);
    if(alpha > -TINYEPSILON && beta > -TINYEPSILON) { 
      /* hit this face */
      newv = oldv;
      v3 = VertexMinusVertex3(vertex2, vertex1);
      d33 = Dot3(&v3, &v3);
      dn3 = Dot3(newv, &v3);
      b1 = -Dot3(&v1, &v3);
      b2 = -Dot3(&v1, newv);
      lambda = (b1-b2*dn3) / (d33-dn3*dn3);
      mu = -(d33*b2-dn3*b1) / (d33-dn3*dn3);
      printf("lambda = %f, mu = %f\n", lambda, mu);

      if(lambda > -TINYEPSILON && lambda < 1.0+TINYEPSILON) {
	if(lambda < TINYEPSILON) {
	  *newflag = VERTEX;
	  newx = vertex1;
	}
	else if(lambda > 1.0-TINYEPSILON) {
	  *newflag = VERTEX;
	  newx = vertex2;
	}
	else {
	  newp->d[0] = vertex1->p3.d[0] + lambda * v3.d[0];
	  newp->d[1] = vertex1->p3.d[1] + lambda * v3.d[1];
	  newp->d[2] = vertex1->p3.d[2] + lambda * v3.d[2];
	  *newflag = EDGE;
	  newe = NULL;
	  for(k=0; k<3; k++) {
	    if( ((oldf->edges[k]->vertices[0] == vertex1) ||
		 (oldf->edges[k]->vertices[0] == vertex2)) &&
	       ((oldf->edges[k]->vertices[1] == vertex1) ||
		(oldf->edges[k]->vertices[1] == vertex2))   ) {
	      newe = oldf->edges[k];
	      break;
	    }
	  }
	  if(newe == NULL) {
	    printf("strange : didn't find the hit edge\n");
	    exit(3);
	  }
	  if(newe->faces[0] != oldf) 
	    newf = newe->faces[0];
	  else
	    newf = newe->faces[1];
	}
      }
      else {
	printf("error : lambda not between 0 and 1.\n");
	exit(2);
      }
      return;
   }
  }
}
