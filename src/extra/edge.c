  Edge *etmp;
/* mesh gformat */    
etmp  = (Edge*) malloc(sizeof(Edge) * nfaces * 3);


#if 0
  printf("creating edges ...\n");
  /* forming the edges */
  nedges = 0;
  for(i=0; i<nfaces; i++) {
    for(k=0; k<3; k++) {
      /* check if the edge formed by vertices k and k+1(mod 3) of 
	 current face has been created before */
      found = FALSE;
      for(j=0; j<nedges; j++) {
	if( (flist[i].vertices[k] == etmp[j].vertices[0] ||
	     flist[i].vertices[k] == etmp[j].vertices[1]) &&
	   (flist[i].vertices[idx[k+1]] == etmp[j].vertices[0] ||
	    flist[i].vertices[idx[k+1]] == etmp[j].vertices[1])   ) {
	  flist[i].edges[k] = &(etmp[j]);
	  etmp[j].faces[1] = &(flist[i]);
	  etmp[j].vx3rd[1] = flist[i].vertices[idx[k+2]];
	  found = TRUE;
	  break;
	}	
      }
      if(found == FALSE) {
	etmp[nedges].index = nedges;
	etmp[nedges].vertices[0] = flist[i].vertices[k];
	etmp[nedges].vertices[1] = flist[i].vertices[idx[k+1]];
	etmp[nedges].faces[0] = &(flist[i]);
	etmp[nedges].vx3rd[0] = flist[i].vertices[idx[k+2]];
	flist[i].edges[k] = &(etmp[nedges]);
	nedges++;
      }
    }
  }
  /* copy etmp to elist */
  elist = (Edge*) malloc(sizeof(Edge) * nedges);
  for(i=0; i<nedges; i++)
    elist[i] = etmp[i];
  free(etmp);
  qfree(qbegin);
#endif
