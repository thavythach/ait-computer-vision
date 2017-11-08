void R2Image::
blendOtherImageHomography(R2Image * otherImage)
{
	// find at least 100 features on this image, and another 100 on the "otherImage". Based on these,
  // compute the matching homography, and blend the transformed "otherImage" into this image with a 50% opacity.

  R2Image image_one(*this);
  R2Image image_two(*this);
  // image_two.Grayscale();
  int *dreams;
  dreams = image_one.Harris(2);
  R2Image image_three(*otherImage);
  image_two.Grayscale();
  image_three.Grayscale();
  std::vector<pair<double,double>> xy;
  std::vector<pair<double,double>> minxy;
std::vector<double> differences;
  
  int ft = 0;
  for (int i = 0; i < 300; i += 2)
  {
    int x = dreams[i];
    int y = dreams[i + 1];
    // printf("--------------------First Image: %d, %d\n", x, y);
    // printf("%f, %f, %f\n",dreams[i].Red(), dreams[i].Blue(), dreams[i].Green());

    // search windows
    double win = 0.1;

    int minX = x;
    int minY = y;
    double minSSD = 1500;
    for (int swX = x - (width * win); swX <= x + (width * win) && swX <= width; swX++)
    {
      for (int swY = y - (height * win); swY <= y + (height * win) && swY <= height; swY++)
      {

        if (swX < 0)
          swX = 0;
        if (swY < 0)
          swY = 0;

        double ssd = 0.0;
        int s = 5;
        for (int ssdX = -s; ssdX <= s; ssdX++)
        {
          for (int ssdY = -s; ssdY <= s; ssdY++)
          {
            double diff = image_two.Pixel(x + ssdX, y + ssdY).Red() - image_three.Pixel(swX + ssdX, swY + ssdY).Red();
            ssd += (diff * diff);
          }
        }
        if (ssd <= minSSD)
        {
          // printf("is this being entered...? %f vs. %f\n", ssd, minSSD);
          minSSD = ssd;
          minX = swX;
          minY = swY;
        }
      }
    }

    // printf("Second Image: %d,%d\n", swX, swY);

    
    // this->line(x, minX, y, minY, 1, 1, 1);
    Pixel(x, y) = R2Pixel(1, 0, 0, 1);
    Pixel(minX, minY) = R2Pixel(0, 1, 0, 1);

    Pixel(x, y) = R2Pixel(1, 0, 0, 1);
    Pixel(minX, minY) = R2Pixel(0, 1, 0, 1);
    
    xy.push_back( std::make_pair(x,y) );
    minxy.push_back( std::make_pair(minX, minY) );

    ft += 1;
    /* for (int lx = -10; lx <= 10; lx++)
     {
      for (int ly = -10; ly <= 10; ly++)
      {
        int xCoord = std::max(0, std::min(x + lx, width - 1));
        int yCoord = std::max(0, std::min(y + ly, height - 1));
        int sxCoord = std::max(0, std::min(minX + lx, width - 1));
        int syCoord = std::max(0, std::min(minY + ly, height - 1));

        int radius = (ly * ly) + (lx * lx);
        if (radius <= 18 && radius >= 8)
        {
          Pixel(xCoord, yCoord) = R2Pixel(1, 0, 0, 1);
          Pixel(sxCoord, syCoord) = R2Pixel(0, 1, 0, 1);
        }
      }
    } */
    
    // printf("--------------------Second Image: %d, %d\n", minX, minY);
    printf("%d features found\n", ft + 1);
  }

  
  // std::vector<int> randsCalled;

  // double dist_threshold = 50.0;

  // randsCalled.push_back(n);

  int goodVectors[150];
  int _inliers[150];
  for (int i=0; i<150;i++){
    goodVectors[i] = 0;
    _inliers[i] = 0;
    // printf("%d\n",goodVectors[i]);
  }

  // 30 iterations
  int iters = 30;
  for (int i=0; i < iters; i++ ){

    double distX = 0.0;
    double distY = 0.0;
    double totalDistance;
    int threshold = 15;
    
    for (int j=0; j < 150; j++){

      // gen random
      int n = (rand() % 150);
      // printf("[n]=%d\n",n);

      double rX = minxy.at(n).first - xy.at(n).first;
      double rY = minxy.at(n).second - xy.at(n).second;

      int inliers = 0;
      for (int k=0; k < 150; k++){
          if (goodVectors[i] == 0){
              distY = minxy.at(i).second - (xy.at(i).second + rY);
              distX = minxy.at(i).first - (xy.at(i).first + rX);
              totalDistance = sqrt(distX*distX + distY*distY);
              
              //printf("[totDist]=%d\n", totalDistance);
              if (totalDistance < threshold){
                  inliers++;
              }
          }
       }
       
       _inliers[n] = inliers;
    }

    int minInliers = 150;
    int outlier = 0;
    for (int j=0; j< 150; j++){
      if ( _inliers[j] < minInliers && goodVectors[j] == 0){
        minInliers = _inliers[j];
        outlier = j;
      }
    }

    if (minInliers < 50){
      goodVectors[outlier] = 1;
    }
  }

  for (int j=0; j<150; j++){
    if ( goodVectors[j] == 0){
      this->line(xy.at(j).first, minxy.at(j).first, xy.at(j).second, minxy.at(j).second, 0, 0, 1);
    } else {
      this->line(xy.at(j).first, minxy.at(j).first, xy.at(j).second, minxy.at(j).second, 1, 0, 0);
    }
  }
}