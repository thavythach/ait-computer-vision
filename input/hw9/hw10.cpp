// omitted code which is the RANSAC/DLT/

//gvVectors are the inliers 

int gg = 0;
  for (int i=0; i< 150; i++){
    if (gvVectors[i] == 1) gg++;
  }
  printf("max------ %d\n",gg);
  
  double rx[gg];
  double rxx[gg];
  double ry[gg];
  double ryy[gg];

  int ggCtr = 0.0;
  for (int i=0; i < 150; i++){
    if (gvVectors[i] == 1){

      rx[ggCtr] = x[i];
      rxx[ggCtr] = _x[i];
      ry[ggCtr] = y[i];
      ryy[ggCtr] = _y[i];
      ggCtr++;
    }
  }  


  double rH[9]; // refined H

  HomographyEstimation(
      rx,
      ry,
      rxx,
      ryy,
      gg,
      rH
  );

  for (int i=0; i < 9; i++){
    printf("(rmax: %d) - REFINNNNNNNNNNNNNNNNNNNNNNED H vs H: %f vs. %f\n",gg, rH[i], H[i]);
  }


  for (int i=0; i < width; i++){
    for (int j=0; j < height; j++){

      // printf("--------------------------------------(%d,%d)\n", i,j );
      
      double hx[3]; 
      int dCtr = 0;
      for (int d=0; d<9; d+=3){
        hx[dCtr] = (rH[d] * i) + (rH[d + 1] * j) + (rH[d + 2] * 1);
        // printf("Result of Hx @ [%d]: %f\n\n", dCtr, hx[dCtr]);
        dCtr++;
      }

      double u = hx[0] / hx[2]; 
      double v = hx[1] / hx[2];

      // printf("======================UV(%f,%f)\n", u, v);

      if ( u < 0|| v < 0 || u > width || v > height ){
        // Pixel(i, j) = Pixel(i,j);
        // printf("badddddddd pixel");
      } else {
        Pixel(i, j) = Pixel(i, j) * 0.5 + image_three.Pixel(u, v) * 0.5;
      }
    }
  }