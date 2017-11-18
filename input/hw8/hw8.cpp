double x[150];
  double y[150];
  double _x[150];
  double _y[150];

  for (int i=0; i < 150; i++){
    x[i] = xy.at(i).first;
    y[i] =  xy.at(i).second;
    _x[i] = minxy.at(i).first;
    _y[i] = minxy.at(i).second;
  }
  
  // 0 for homography, 1 for translation

  const int numFeatures = 150;

  // print all features
  // for (int i=0; i < numFeatures; i++) printf("x,y=(%f,%f)\nx',y'=(%f,%f)\n\n", x[i],y[i],_x[i],_y[i]);

  /** BELOW: all algorithms COMPUTING 2D homography **/
  int goodVectors[150];
  int featureInliers[150];
  for (int i = 0; i < 150; i++)
  {
    goodVectors[i] = 0;
    featureInliers[i] = 0;
  }

  int N = 1000;            // ESTIMATIONS
  const int T_DIST = 5; // DISTANCE THRESHOLD
  int maxInliers = 0;  // STARTING MAX_INLIER
  int minPoints = 4;
  double H[9];
  int bestMatch[4];

  // for every ith estimation
  for (int i = 0; i < N; i++)
  {

    // randomly choose 4 correspondences
    int rCorrs[minPoints] = {(rand() % 150),
                             (rand() % 150),
                             (rand() % 150),
                             (rand() % 150)};

    double xx[minPoints], yy[minPoints], _xx[minPoints], _yy[minPoints];

    // prints chosen 4 correspondences values
    for (int j = 0; j < minPoints; j++)
    {
      bestMatch[j] = rCorrs[j];
      featureInliers[rCorrs[j]] = 0; // initializes initial amount of inliers.
      printf("RANDS=[%d]\n", rCorrs[j]);
      xx[j] = x[rCorrs[j]];
      yy[j] = y[rCorrs[j]];
      _xx[j] = _x[rCorrs[j]];
      _yy[j] = _y[rCorrs[j]];
    }

    double hCurr[9]; // pass as reference

    // compute the homography Hcurr by normalized DLT from 4 point pairs
    HomographyEstimation(
        xx,
        yy,
        _xx,
        _yy,
        4,
        hCurr);

    // sets initial BEST H.
    // if ( i==0 ) std::copy(std::begin(hCurr), std::end(hCurr), std::begin(H));

    // initialze number of inliers
    int numInliers = 0;

    // for each putative correspondence, calculate di (ssd) = d(x', hCurr*x)-d(X', H^-1*x)
    for (int j = 0; j < numFeatures; j++)
    {
      goodVectors[j] = 0;

      // matrix multiplication of 3x3 * 1*3 = 1*3

      double hx[3];
      int dCtr = 0;
      for (int d = 0; d < 9; d += 3)
      {
        hx[dCtr] = (hCurr[d] * x[j]) + (hCurr[d + 1] * y[j]) + (hCurr[d + 2] * 1);
        // printf("Result of Hx @ [%d]: %f\n\n", dCtr, hx[dCtr]);
        dCtr++;
      }

      // convert homogenous coordinates to cartesian coordinates b/c of scale
      double hx_X = hx[0] / hx[2];
      double hx_Y = hx[1] / hx[2];

      double diffX = _x[j] - hx_X;
      double diffY = _y[j] - hx_Y;
      double totalDifference = sqrt(diffX * diffX + diffY * diffY);

      if (totalDifference < T_DIST){
        featureInliers[rCorrs[0]]++;
        goodVectors[j] = 1;
        numInliers++;
      }

      // looks through to find vector with most inlliers
      // turns this and its inliers green!!!
      if (numInliers > maxInliers){
        maxInliers = numInliers;

        for (int k = 0; k < minPoints; k++)
        {
          bestMatch[k] = rCorrs[k];
        }

        for (int k = 0; k < 150; k++)
        {
          if (goodVectors[k] == 0)
            this->line(x[k], _x[k], y[k], _y[k], 1, 0, 0);
          else
            this->line(x[k], _x[k], y[k], _y[k], 0, 0, 1);
        }
      }
    }
  }