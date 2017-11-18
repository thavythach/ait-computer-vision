// Source file for image class



// Include files 

#include "R2/R2.h"
#include "R2Pixel.h"
#include "R2Image.h"
#include "svd.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>

////////////////////////////////////////////////////////////////////////
// Constructors/Destructors
////////////////////////////////////////////////////////////////////////
using namespace std;

R2Image::
R2Image(void)
  : pixels(NULL),
    npixels(0),
    width(0), 
    height(0)
{
}



R2Image::
R2Image(const char *filename)
  : pixels(NULL),
    npixels(0),
    width(0), 
    height(0)
{
  // Read image
  Read(filename);
}



R2Image::
R2Image(int width, int height)
  : pixels(NULL),
    npixels(width * height),
    width(width), 
    height(height)
{
  // Allocate pixels
  pixels = new R2Pixel [ npixels ];
  assert(pixels);
}



R2Image::
R2Image(int width, int height, const R2Pixel *p)
  : pixels(NULL),
    npixels(width * height),
    width(width), 
    height(height)
{
  // Allocate pixels
  pixels = new R2Pixel [ npixels ];
  assert(pixels);

  // Copy pixels 
  for (int i = 0; i < npixels; i++) 
    pixels[i] = p[i];
}



R2Image::
R2Image(const R2Image& image)
  : pixels(NULL),
    npixels(image.npixels),
    width(image.width), 
    height(image.height)
    
{
  // Allocate pixels
  pixels = new R2Pixel [ npixels ];
  assert(pixels);

  // Copy pixels 
  for (int i = 0; i < npixels; i++) 
    pixels[i] = image.pixels[i];
}



R2Image::
~R2Image(void)
{
  // Free image pixels
  if (pixels) delete [] pixels;
}



R2Image& R2Image::
operator=(const R2Image& image)
{
  // Delete previous pixels
  if (pixels) { delete [] pixels; pixels = NULL; }

  // Reset width and height
  npixels = image.npixels;
  width = image.width;
  height = image.height;

  // Allocate new pixels
  pixels = new R2Pixel [ npixels ];
  assert(pixels);

  // Copy pixels 
  for (int i = 0; i < npixels; i++) 
    pixels[i] = image.pixels[i];

  // Return image
  return *this;
}


void R2Image::
svdTest(void)
{
	// fit a 2D conic to five points
	R2Point p1(1.2,3.5);
	R2Point p2(2.1,2.2);
	R2Point p3(0.2,1.6);
	R2Point p4(0.0,0.5);
	R2Point p5(-0.2,4.2);

	// build the 5x6 matrix of equations
	double** linEquations = dmatrix(1,5,1,6);

	linEquations[1][1] = p1[0]*p1[0];
	linEquations[1][2] = p1[0]*p1[1];
	linEquations[1][3] = p1[1]*p1[1];
	linEquations[1][4] = p1[0];
	linEquations[1][5] = p1[1];
	linEquations[1][6] = 1.0;

	linEquations[2][1] = p2[0]*p2[0];
	linEquations[2][2] = p2[0]*p2[1];
	linEquations[2][3] = p2[1]*p2[1];
	linEquations[2][4] = p2[0];
	linEquations[2][5] = p2[1];
	linEquations[2][6] = 1.0;

	linEquations[3][1] = p3[0]*p3[0];
	linEquations[3][2] = p3[0]*p3[1];
	linEquations[3][3] = p3[1]*p3[1];
	linEquations[3][4] = p3[0];
	linEquations[3][5] = p3[1];
	linEquations[3][6] = 1.0;
	
	linEquations[4][1] = p4[0]*p4[0];
	linEquations[4][2] = p4[0]*p4[1];
	linEquations[4][3] = p4[1]*p4[1];
	linEquations[4][4] = p4[0];
	linEquations[4][5] = p4[1];
	linEquations[4][6] = 1.0;

	linEquations[5][1] = p5[0]*p5[0];
	linEquations[5][2] = p5[0]*p5[1];
	linEquations[5][3] = p5[1]*p5[1];
	linEquations[5][4] = p5[0];
	linEquations[5][5] = p5[1];
	linEquations[5][6] = 1.0;

	printf("\n Fitting a conic to five points:\n");
	printf("Point #1: %f,%f\n",p1[0],p1[1]);
	printf("Point #2: %f,%f\n",p2[0],p2[1]);
	printf("Point #3: %f,%f\n",p3[0],p3[1]);
	printf("Point #4: %f,%f\n",p4[0],p4[1]);
	printf("Point #5: %f,%f\n",p5[0],p5[1]);

	// compute the SVD
	double singularValues[7]; // 1..6
	double** nullspaceMatrix = dmatrix(1,6,1,6);
	svdcmp(linEquations, 5, 6, singularValues, nullspaceMatrix);

	// get the result
	printf("\n Singular values: %f, %f, %f, %f, %f, %f\n",singularValues[1],singularValues[2],singularValues[3],singularValues[4],singularValues[5],singularValues[6]);

	// find the smallest singular value:
	int smallestIndex = 1;
	for(int i=2;i<7;i++) if(singularValues[i]<singularValues[smallestIndex]) smallestIndex=i;

	// solution is the nullspace of the matrix, which is the column in V corresponding to the smallest singular value (which should be 0)
	printf("Conic coefficients: %f, %f, %f, %f, %f, %f\n",nullspaceMatrix[1][smallestIndex],nullspaceMatrix[2][smallestIndex],nullspaceMatrix[3][smallestIndex],nullspaceMatrix[4][smallestIndex],nullspaceMatrix[5][smallestIndex],nullspaceMatrix[6][smallestIndex]);

	// make sure the solution is correct:
	printf("Equation #1 result: %f\n",	p1[0]*p1[0]*nullspaceMatrix[1][smallestIndex] + 
										p1[0]*p1[1]*nullspaceMatrix[2][smallestIndex] + 
										p1[1]*p1[1]*nullspaceMatrix[3][smallestIndex] + 
										p1[0]*nullspaceMatrix[4][smallestIndex] + 
										p1[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	printf("Equation #2 result: %f\n",	p2[0]*p2[0]*nullspaceMatrix[1][smallestIndex] + 
										p2[0]*p2[1]*nullspaceMatrix[2][smallestIndex] + 
										p2[1]*p2[1]*nullspaceMatrix[3][smallestIndex] + 
										p2[0]*nullspaceMatrix[4][smallestIndex] + 
										p2[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	printf("Equation #3 result: %f\n",	p3[0]*p3[0]*nullspaceMatrix[1][smallestIndex] + 
										p3[0]*p3[1]*nullspaceMatrix[2][smallestIndex] + 
										p3[1]*p3[1]*nullspaceMatrix[3][smallestIndex] + 
										p3[0]*nullspaceMatrix[4][smallestIndex] + 
										p3[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	printf("Equation #4 result: %f\n",	p4[0]*p4[0]*nullspaceMatrix[1][smallestIndex] + 
										p4[0]*p4[1]*nullspaceMatrix[2][smallestIndex] + 
										p4[1]*p4[1]*nullspaceMatrix[3][smallestIndex] + 
										p4[0]*nullspaceMatrix[4][smallestIndex] + 
										p4[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	printf("Equation #5 result: %f\n",	p5[0]*p5[0]*nullspaceMatrix[1][smallestIndex] + 
										p5[0]*p5[1]*nullspaceMatrix[2][smallestIndex] + 
										p5[1]*p5[1]*nullspaceMatrix[3][smallestIndex] + 
										p5[0]*nullspaceMatrix[4][smallestIndex] + 
										p5[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	R2Point test_point(0.34,-2.8);

	printf("A point off the conic: %f\n",	test_point[0]*test_point[0]*nullspaceMatrix[1][smallestIndex] + 
											test_point[0]*test_point[1]*nullspaceMatrix[2][smallestIndex] + 
											test_point[1]*test_point[1]*nullspaceMatrix[3][smallestIndex] + 
											test_point[0]*nullspaceMatrix[4][smallestIndex] + 
											test_point[1]*nullspaceMatrix[5][smallestIndex] + 
											nullspaceMatrix[6][smallestIndex]);

	return;	
}



////////////////////////////////////////////////////////////////////////
// Image processing functions
// YOU IMPLEMENT THE FUNCTIONS IN THIS SECTION
////////////////////////////////////////////////////////////////////////

// Per-pixel Operations ////////////////////////////////////////////////

void R2Image::
Brighten(double factor)
{
  // Brighten the image by multiplying each pixel component by the factor.
  // This is implemented for you as an example of how to access and set pixels
  for (int i = 0; i < width; i++) {
    for (int j = 0;  j < height; j++) {
      Pixel(i,j) *= factor;
      Pixel(i,j).Clamp();
    }
  }
}

void R2Image::
SobelX(void)
{
  R2Image tempImage(*this);
  // Apply the Sobel operator to the image in X direction
  
  int kernel[3][3] = {
              {1,0,-1},
              {2,0,-2},
              {1,0,-1}
              };
  for (int x=1; x <= width-1; x++){
    for (int y=1; y <= height-1; y++){
      R2Pixel* val = new R2Pixel();
      for (int lx=-1; lx <= 1; lx++){
        for (int ly=-1; ly <= 1; ly++){
          *val += tempImage.Pixel(x+lx,y+ly)*kernel[lx+1][ly+1];
        }
      }
      Pixel(x,y) = *val;
      // Pixel(x,y).Clamp();
    }
  }
}

void R2Image::
SobelY(void)
{  
  R2Image tempImage(*this);
	// Apply the Sobel oprator to the image in Y direction
   int kernel[3][3] = {
              {-1,-2,-1},
              {0,0,0},
              {1,2,1}
              };
  for (int x=1; x <= width-1; x++){
    for (int y=1; y <= height-1; y++){
      R2Pixel* val = new R2Pixel();
      for (int lx=-1; lx <= 1; lx++){
        for (int ly=-1; ly <= 1; ly++){
          *val += tempImage.Pixel(x+lx,y+ly)*kernel[lx+1][ly+1];
        }
      }
      Pixel(x,y) = *val;
      // Pixel(x,y).Clamp();
    }
  }
}

void R2Image::
LoG(void)
{
  // Apply the LoG oprator to the image
  
  // FILL IN IMPLEMENTATION HERE (REMOVE PRINT STATEMENT WHEN DONE)
  fprintf(stderr, "LoG() not implemented\n");
}



void R2Image::
ChangeSaturation(double factor)
{
  // Changes the saturation of an image
  // Find a formula that changes the saturation without affecting the image brightness

  // FILL IN IMPLEMENTATION HERE (REMOVE PRINT STATEMENT WHEN DONE)
  fprintf(stderr, "ChangeSaturation(%g) not implemented\n", factor);
}

//Linear filtering ////////////////////////////////////////////////
void R2Image::
Blur(double sigma)
{
  // Gaussian blur of the image. Separable solution is preferred
  /* 
  uncomment to use homography estimation
  int x[] = {0,1,1,0};
  int y[] = {0,0,1,1};
  int _x[] = {1,2,2,1};
  int _y[] = {0,0,1,1};
  int numPoints = 4;
  HomographyEstimation( x, y, _x, _y, numPoints);
  exit(0); */
  
  R2Image tempImage(*this);


  
  // Compute the Kernel (1D convolution)
  int size = sigma * 6 + 1; 
  int cWidth = sigma * 3; // 3 one ach side
  double kernel[size];

  // std::cout << size;

  double weights = 0.0;
  for (int i=-cWidth; i<=cWidth; i++){
    int idx = i + cWidth;
    kernel[idx] = exp( -(i*i)/sigma*sigma*2 / (M_PI * sigma*sigma*2) );
    // std::cout << kernel[idx] << "\n";
    weights += kernel[idx];
  }

  // std::cout << "\n Total Weights: " << weights;

  // normalize the kernel
  double kernel_sum = 0.0;
  for (int i=0; i<size; i++){
    kernel[i] /= weights;
    // std :: cout << kernel[i] << "\n";
    
    kernel_sum += kernel[i];
  }

  // std::cout << kernel_sum;

  /* separable linear filters */

  // y - direction
  for (int y=cWidth; y < height-cWidth; y++){
    for (int x=0; x < width; x++){
      
      // local vars
      R2Pixel* val = new R2Pixel();
      
      for (int ly=-cWidth; ly <= cWidth; ly++){
        *val += Pixel(x, y+ly) * kernel[ly + cWidth];
        // printf("%d -%d\n",x,y);
      }
      tempImage.Pixel(x,y) = *val;
      // tempImage.Pixel(x,y).Clamp();
    }
  }

  // x - direction
  for (int y=0; y < height; y++){
    for (int x=cWidth; x< width-cWidth; x++){

      // local vars
      R2Pixel* val = new R2Pixel();
      
      for (int lx=-cWidth; lx <= cWidth; lx++){
        *val += tempImage.Pixel(x+lx, y) * kernel[lx + cWidth];
      }
      Pixel(x,y) = *val;
      // Pixel(x,y).Clamp();
    }
  }
}

void R2Image::
Grayscale()
{
  R2Image tmp(*this);

  for (int x=0; x <width; x++){
    for (int y=0; y<height; y++){

      double val = (Pixel(x,y).Red() + Pixel(x,y).Blue() + Pixel(x,y).Green()) / 3;
      Pixel(x,y) = R2Pixel(val,val,val,1.0);
    }
  }
}

int* R2Image::
Harris(double sigma)
{
  // Harris corner detector. Make use of the previously developed filters, such as the Gaussian blur filter
  // Output should be 50% grey at flat regions, white at corners and black/dark near edges
  
  printf("Grayscale Initiated!\n");
  this->Grayscale();
  printf("Grayscale Completed!\n");
  
  printf("Creating 3 Temporary Images (solX, solY, Ix_sq, Iy_sq, Ix_Iy) based off original image.\n");  
  R2Image Ix_sq(*this);
  Ix_sq.SobelX();
  R2Image Iy_sq(*this);
  Iy_sq.SobelY();
  R2Image Ix_Iy(*this);
  Ix_Iy.SobelX();

  printf("Finished creating 5 Temporary Images\n");
  
  printf("Beginning Harris Filter Process!\n");

  printf("Populating three temporary images (Ix_sq, Iy_sq, Ix_Iy) using temp images that were sobeled (solX, solY)\n");
  for (int x=0; x <width; x++){
    for (int y=0; y<height; y++){
      Ix_sq.Pixel(x,y) *= Ix_sq.Pixel(x,y);
      Ix_Iy.Pixel(x,y) *= Iy_sq.Pixel(x,y);
      Iy_sq.Pixel(x,y) *= Iy_sq.Pixel(x,y);
    }
  }
  printf("Populating Finished.\n");

  printf("Blurring three temporary images!!!\n");
  Ix_sq.Blur( sigma );
  Iy_sq.Blur( sigma );
  Ix_Iy.Blur( sigma );
  printf("Blur using sigma: %f completed!\n", sigma);

  printf("Initialized three temporary variables: alpha, detected, rmax!\n");
  printf("alpha");
  double alpha = 0.04;
  double detected[width][height];
  double rmax = 0.0;
  printf("Initialization Successful - alpha set to (%f), detected to empty 2D array, rmax set to (%f)!\n", alpha, rmax);
  static int features[300]; 
  
  printf("Begin computing the Harris Value and populate both rmax and detected 2D array!\n");
  for (int x=0; x < width; x++){
    for (int y=0; y < height; y++){
      // printf("x: %d, y: %d!\n", x,y);
      Pixel(x,y) = 
      Ix_sq.Pixel( x,y ) * Iy_sq.Pixel( x,y ) - 
      Ix_Iy.Pixel( x,y ) * Ix_Iy.Pixel (x,y ) - 
      alpha *
      (
        ( Ix_sq.Pixel (x,y ) + Iy_sq.Pixel( x,y ) ) * 
        ( Ix_sq.Pixel( x,y ) + Iy_sq.Pixel( x,y ) )
      )
      + R2Pixel(0.5,0.5,0.5,1); // 0.5 to normalize and see the gray values
      ;
      detected[x][y] = Pixel(x,y).Red(); // if you have grayscale it, all the rgb values are the same
      
      if ( Pixel(x,y).Red() > rmax){
        rmax = Pixel(x,y).Red();
      }

      Pixel(x,y).Clamp();    
    }
  }
  
  printf("Finish computing the Harris Value, rmax, and detected 2D array");
  
  printf("Begin Bruteforcing # of threshold beginning at 1.0 and ends at 0.0 every 0.10 decrementation.\n");


  int features_to_find = 150;
  int i=0;
  int ft=0;
  for (double threshold = 1.0; i != 150 && threshold >= 0.0; threshold -= 0.01){
    for (int x=0; x < width; x++){
      for (int y=0; y < height; y++){
        if (detected[x][y] >= threshold * rmax &&
          detected[x][y] > detected[x-1][y-1] &&
          detected[x][y] > detected[x-1][y+1] &&
          detected[x][y] > detected[x+1][y-1] &&
          detected[x][y] > detected[x+1][y+1]
        ){
          features[ft] = x;
          features[ft+1] = y;
          // printf("%f\n",features[i].Red());
          ft+=2;
          i++;                                  
          for (int lx = -10; lx <= 10; lx++){
            for (int ly = -10; ly <= 10; ly++){
              int xCoord = std::max(0, std::min(x +lx, width-1));
              int yCoord = std::max(0, std::min(y +ly, height-1));
              detected[xCoord][yCoord] = 0;
              
              int radius = (ly*ly) + (lx*lx);
              if (radius <= 18 && radius >= 8){
                Pixel(xCoord,yCoord) = R2Pixel(1,0,0,1);
              }
            }
          }
          
          
        }
        if (i==features_to_find){
            break;
        }
    }
    
  }
  printf("Found (%d) features for %.2f-- \n",i, threshold);
  }
  

  printf("Bruteforce Completed!\n");
  return features;
}


void R2Image::
Sharpen()
{
  // Sharpen an image using a linear filter. Use a kernel of your choosing.
  R2Image tempImage(*this);
   int kernel[3][3] = {
              {0,-1,0},
              {-1,-5,-1},
              {0,-1,0}
  };
  for (int x=1; x <= width-1; x++){
    for (int y=1; y <= height-1; y++){
      R2Pixel* val = new R2Pixel();
      for (int lx=-1; lx <= 1; lx++){
        for (int ly=-1; ly <= 1; ly++){
          *val += tempImage.Pixel(x+lx,y+ly)*kernel[lx+1][ly+1];
        }
      }
      Pixel(x,y) = *val;
      Pixel(x,y).Clamp();
    }
  }
}

void R2Image::
line(int x0, int x1, int y0, int y1, float r, float g, float b)
{
	if(x0>x1)
	{
		int x=y1;
		y1=y0;
		y0=x;

		x=x1;
		x1=x0;
		x0=x;
	}
     int deltax = x1 - x0;
     int deltay = y1 - y0;
     float error = 0;
     float deltaerr = 0.0;
	 if(deltax!=0) deltaerr =fabs(float(float(deltay) / deltax));    // Assume deltax != 0 (line is not vertical),
           // note that this division needs to be done in a way that preserves the fractional part
     int y = y0;
     for(int x=x0;x<=x1;x++)
	 {
		 Pixel(x,y).Reset(r,g,b,1.0);
         error = error + deltaerr;
         if(error>=0.5)
		 {
			 if(deltay>0) y = y + 1;
			 else y = y - 1;

             error = error - 1.0;
		 }
	 }
	 if(x0>3 && x0<width-3 && y0>3 && y0<height-3)
	 {
		 for(int x=x0-3;x<=x0+3;x++)
		 {
			 for(int y=y0-3;y<=y0+3;y++)
			 {
				 Pixel(x,y).Reset(r,g,b,1.0);
			 }
		 }
	 }
}

void R2Image::
HomographyEstimation(double *x, double *y, double *_x, double *_y, int numPoints, double* h){
  printf("Begin Homography Estimation!!!\n");

  // build the 2n x 9 matrix of equations
  double **linEquations = dmatrix(1, 2*numPoints, 1, 9);

  int ctr = 0;
  for (int i=1; i <= numPoints*2; i+=2){
    printf("x,y=[%f,%f]...x',y'=[%f,%f]\n", x[ctr], y[ctr], _x[ctr], _y[ctr]);

    /* 
    e.g. numPoints = 4, 4*2 = 8 ... 1 -> 8
    1 2 
    3 4 
    5 6 
    7 8
    */
    linEquations[i][1] = 0.0;
    linEquations[i][2] = 0.0;
    linEquations[i][3] = 0.0;
    linEquations[i][4] = -1.0 * x[ctr];
    linEquations[i][5] = -1.0 * y[ctr];
    linEquations[i][6] = -1.0 * 1.0;
    linEquations[i][7] = _y[ctr] * x[ctr];
    linEquations[i][8] = _y[ctr] * y[ctr];
    linEquations[i][9] = _y[ctr] * 1.0;

    linEquations[i+1][1] = 1.0 * x[ctr];
    linEquations[i+1][2] = 1.0 * y[ctr];
    linEquations[i+1][3] = 1.0 * 1.0;
    linEquations[i+1][4] = 0.0;
    linEquations[i+1][5] = 0.0;
    linEquations[i+1][6] = 0.0;
    linEquations[i+1][7] = -_x[ctr] * x[ctr];
    linEquations[i+1][8] = -_x[ctr] * y[ctr];
    linEquations[i+1][9] = -_x[ctr] * 1.0;
      
    ctr++;
  }

  // compute the svd
  double singularValues[10]; // 1..9
  double** nullspaceMatrix = dmatrix(1,9,1,9);
  svdcmp(linEquations, numPoints*2, 9, singularValues, nullspaceMatrix);
  
  // find the smallest singular value
  int smallestIndex = 1;
  for (int i=2;i<10;i++)
    if (singularValues[i] < singularValues[smallestIndex]) smallestIndex=i;

  // solution is the nullspace of the matrix, which is the column in V corresponding to the smallest singular value (which should be 0)
  printf("Conic Coefficients...\n");
  for (int i=1; i<10; i++){
    printf("NULLSPACE MATRIX @ [i]=%d, [smallest idx]=%d, [Conic Coefficient]=%f\n", i, smallestIndex, nullspaceMatrix[i][smallestIndex]);
    h[i-1] = nullspaceMatrix[i][smallestIndex];
    printf("added to h-matrix: %f\n", h[i-1]);
  }

  
}

void R2Image::
blendOtherImageTranslated(R2Image * otherImage)
{
	// find at least 100 features on this image, and another 100 on the "otherImage". Based on these,
	// compute the matching translation (pixel precision is OK), and blend the translated "otherImage" 
  // into this image with a 50% opacity.

  R2Image image_one(*this);
  R2Image image_two(*this);
  // image_two.Grayscale();
  int* dreams; 
  dreams = image_one.Harris(2);
  R2Image image_three(*otherImage);
  image_two.Grayscale();
  image_three.Grayscale();


  int ft = 0;
  for (int i=0; i < 300; i+=2){
    int x=dreams[i];
    int y=dreams[i+1];
    printf("--------------------First Image: %d, %d\n", x, y);
    // printf("%f, %f, %f\n",dreams[i].Red(), dreams[i].Blue(), dreams[i].Green());
    
    // search windows
    double win = 0.1;
    
    int minX = x;
    int minY = y;
    double minSSD = 1500;
    for (int swX=x-(width*win) ; swX <= x+(width*win) && swX <= width ; swX++){
      for (int swY=y-(height*win) ; swY <= y+(height*win) && swY <= height; swY++){
        
        if (swX < 0) swX = 0;
        if (swY < 0) swY = 0;

        double ssd = 0.0;
        int s = 5;
        for ( int ssdX=-s ; ssdX <= s ; ssdX++ ){
         for ( int ssdY=-s ; ssdY <= s ; ssdY++ ){
            double diff = image_two.Pixel(x+ssdX,y+ssdY).Red() - image_three.Pixel(swX+ssdX, swY+ssdY).Red();
            ssd += (diff*diff);
          }
        }
        if (ssd <= minSSD){
          printf("is this being entered...? %f vs. %f\n", ssd, minSSD);
          minSSD = ssd;
          minX = swX;
          minY = swY;
        }
      }
    }

    
      
    // printf("Second Image: %d,%d\n", swX, swY);

    this->line(x,minX,y,minY, 1, 1, 1);
    Pixel(x,y) = R2Pixel(1,0,0,1);          
    Pixel(minX, minY) = R2Pixel(0,1,0,1);
    ft += 1;
    for (int lx = -10; lx <= 10; lx++){
      for (int ly = -10; ly <= 10; ly++){
        int xCoord = std::max(0, std::min(x +lx, width-1));
        int yCoord = std::max(0, std::min(y +ly, height-1));
        int sxCoord = std::max(0, std::min(minX +lx, width-1));
        int syCoord = std::max(0, std::min(minY +ly, height-1));
        
        int radius = (ly*ly) + (lx*lx);
        if (radius <= 18 && radius >= 8){
          Pixel(xCoord,yCoord) = R2Pixel(1,0,0,1);
          Pixel(sxCoord, syCoord) = R2Pixel(0,1,0,1);
        }
      }
    }
 


    printf("--------------------Second Image: %d, %d\n", minX, minY);        
    printf("%d features found\n", ft+1);
  }

}

void R2Image::
computeRANSAC(int mode, double *x, double *y, double *_x, double *_y){
  /**
   * (x,y) set
   * (x',y') set
   * 
   * Using this list of items, compute RANSAC the Homography way.
   * */
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

  int N = 1000;         // ESTIMATIONS
  const int T_DIST = 5; // DISTANCE THRESHOLD
  int maxInliers = 0;   // STARTING MAX_INLIER
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

      if (totalDifference < T_DIST)
      {
        featureInliers[rCorrs[0]]++;
        goodVectors[j] = 1;
        numInliers++;
      }

      // looks through to find vector with most inlliers
      // turns this and its inliers green!!!
      if (numInliers > maxInliers)
      {
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
}

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

  // computeRANSAC( 0, x, y, _x, _y );
}



////////////////////////////////////////////////////////////////////////
// I/O Functions
////////////////////////////////////////////////////////////////////////

int R2Image::
Read(const char *filename)
{
  // Initialize everything
  if (pixels) { delete [] pixels; pixels = NULL; }
  npixels = width = height = 0;

  // Parse input filename extension
  char *input_extension;
  if (!(input_extension = (char*)strrchr(filename, '.'))) {
    fprintf(stderr, "Input file has no extension (e.g., .jpg).\n");
    return 0;
  }
  
  // Read file of appropriate type
  if (!strncmp(input_extension, ".bmp", 4)) return ReadBMP(filename);
  else if (!strncmp(input_extension, ".ppm", 4)) return ReadPPM(filename);
  else if (!strncmp(input_extension, ".jpg", 4)) return ReadJPEG(filename);
  else if (!strncmp(input_extension, ".jpeg", 5)) return ReadJPEG(filename);
  
  // Should never get here
  fprintf(stderr, "Unrecognized image file extension");
  return 0;
}



int R2Image::
Write(const char *filename) const
{
  // Parse input filename extension
  char *input_extension;
  if (!(input_extension = (char*)strrchr(filename, '.'))) {
    fprintf(stderr, "Input file has no extension (e.g., .jpg).\n");
    return 0;
  }
  
  // Write file of appropriate type
  if (!strncmp(input_extension, ".bmp", 4)) return WriteBMP(filename);
  else if (!strncmp(input_extension, ".ppm", 4)) return WritePPM(filename, 1);
  else if (!strncmp(input_extension, ".jpg", 5)) return WriteJPEG(filename);
  else if (!strncmp(input_extension, ".jpeg", 5)) return WriteJPEG(filename);

  // Should never get here
  fprintf(stderr, "Unrecognized image file extension");
  return 0;
}



////////////////////////////////////////////////////////////////////////
// BMP I/O
////////////////////////////////////////////////////////////////////////

#if (RN_OS == RN_LINUX) && !WIN32

typedef struct tagBITMAPFILEHEADER {
  unsigned short int bfType;
  unsigned int bfSize;
  unsigned short int bfReserved1;
  unsigned short int bfReserved2;
  unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
  unsigned int biSize;
  int biWidth;
  int biHeight;
  unsigned short int biPlanes;
  unsigned short int biBitCount;
  unsigned int biCompression;
  unsigned int biSizeImage;
  int biXPelsPerMeter;
  int biYPelsPerMeter;
  unsigned int biClrUsed;
  unsigned int biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBTRIPLE {
  unsigned char rgbtBlue;
  unsigned char rgbtGreen;
  unsigned char rgbtRed;
} RGBTRIPLE;

typedef struct tagRGBQUAD {
  unsigned char rgbBlue;
  unsigned char rgbGreen;
  unsigned char rgbRed;
  unsigned char rgbReserved;
} RGBQUAD;

#endif

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#define BMP_BF_TYPE 0x4D42 /* word BM */
#define BMP_BF_OFF_BITS 54 /* 14 for file header + 40 for info header (not sizeof(), but packed size) */
#define BMP_BI_SIZE 40 /* packed size of info header */


static unsigned short int WordReadLE(FILE *fp)
{
  // Read a unsigned short int from a file in little endian format 
  unsigned short int lsb, msb;
  lsb = getc(fp);
  msb = getc(fp);
  return (msb << 8) | lsb;
}



static void WordWriteLE(unsigned short int x, FILE *fp)
{
  // Write a unsigned short int to a file in little endian format
  unsigned char lsb = (unsigned char) (x & 0x00FF); putc(lsb, fp); 
  unsigned char msb = (unsigned char) (x >> 8); putc(msb, fp);
}



static unsigned int DWordReadLE(FILE *fp)
{
  // Read a unsigned int word from a file in little endian format 
  unsigned int b1 = getc(fp);
  unsigned int b2 = getc(fp);
  unsigned int b3 = getc(fp);
  unsigned int b4 = getc(fp);
  return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}



static void DWordWriteLE(unsigned int x, FILE *fp)
{
  // Write a unsigned int to a file in little endian format 
  unsigned char b1 = (x & 0x000000FF); putc(b1, fp);
  unsigned char b2 = ((x >> 8) & 0x000000FF); putc(b2, fp);
  unsigned char b3 = ((x >> 16) & 0x000000FF); putc(b3, fp);
  unsigned char b4 = ((x >> 24) & 0x000000FF); putc(b4, fp);
}



static int LongReadLE(FILE *fp)
{
  // Read a int word from a file in little endian format 
  int b1 = getc(fp);
  int b2 = getc(fp);
  int b3 = getc(fp);
  int b4 = getc(fp);
  return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}



static void LongWriteLE(int x, FILE *fp)
{
  // Write a int to a file in little endian format 
  char b1 = (x & 0x000000FF); putc(b1, fp);
  char b2 = ((x >> 8) & 0x000000FF); putc(b2, fp);
  char b3 = ((x >> 16) & 0x000000FF); putc(b3, fp);
  char b4 = ((x >> 24) & 0x000000FF); putc(b4, fp);
}



int R2Image::
ReadBMP(const char *filename)
{
  // Open file
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  /* Read file header */
  BITMAPFILEHEADER bmfh;
  bmfh.bfType = WordReadLE(fp);
  bmfh.bfSize = DWordReadLE(fp);
  bmfh.bfReserved1 = WordReadLE(fp);
  bmfh.bfReserved2 = WordReadLE(fp);
  bmfh.bfOffBits = DWordReadLE(fp);
  
  /* Check file header */
  assert(bmfh.bfType == BMP_BF_TYPE);
  /* ignore bmfh.bfSize */
  /* ignore bmfh.bfReserved1 */
  /* ignore bmfh.bfReserved2 */
  assert(bmfh.bfOffBits == BMP_BF_OFF_BITS);
  
  /* Read info header */
  BITMAPINFOHEADER bmih;
  bmih.biSize = DWordReadLE(fp);
  bmih.biWidth = LongReadLE(fp);
  bmih.biHeight = LongReadLE(fp);
  bmih.biPlanes = WordReadLE(fp);
  bmih.biBitCount = WordReadLE(fp);
  bmih.biCompression = DWordReadLE(fp);
  bmih.biSizeImage = DWordReadLE(fp);
  bmih.biXPelsPerMeter = LongReadLE(fp);
  bmih.biYPelsPerMeter = LongReadLE(fp);
  bmih.biClrUsed = DWordReadLE(fp);
  bmih.biClrImportant = DWordReadLE(fp);
  
  // Check info header 
  assert(bmih.biSize == BMP_BI_SIZE);
  assert(bmih.biWidth > 0);
  assert(bmih.biHeight > 0);
  assert(bmih.biPlanes == 1);
  assert(bmih.biBitCount == 24);  /* RGB */
  assert(bmih.biCompression == BI_RGB);   /* RGB */
  int lineLength = bmih.biWidth * 3;  /* RGB */
  if ((lineLength % 4) != 0) lineLength = (lineLength / 4 + 1) * 4;
  assert(bmih.biSizeImage == (unsigned int) lineLength * (unsigned int) bmih.biHeight);

  // Assign width, height, and number of pixels
  width = bmih.biWidth;
  height = bmih.biHeight;
  npixels = width * height;

  // Allocate unsigned char buffer for reading pixels
  int rowsize = 3 * width;
  if ((rowsize % 4) != 0) rowsize = (rowsize / 4 + 1) * 4;
  int nbytes = bmih.biSizeImage;
  unsigned char *buffer = new unsigned char [nbytes];
  if (!buffer) {
    fprintf(stderr, "Unable to allocate temporary memory for BMP file");
    fclose(fp);
    return 0;
  }

  // Read buffer 
  fseek(fp, (long) bmfh.bfOffBits, SEEK_SET);
  if (fread(buffer, 1, bmih.biSizeImage, fp) != bmih.biSizeImage) {
    fprintf(stderr, "Error while reading BMP file %s", filename);
    return 0;
  }

  // Close file
  fclose(fp);

  // Allocate pixels for image
  pixels = new R2Pixel [ width * height ];
  if (!pixels) {
    fprintf(stderr, "Unable to allocate memory for BMP file");
    fclose(fp);
    return 0;
  }

  // Assign pixels
  for (int j = 0; j < height; j++) {
    unsigned char *p = &buffer[j * rowsize];
    for (int i = 0; i < width; i++) {
      double b = (double) *(p++) / 255;
      double g = (double) *(p++) / 255;
      double r = (double) *(p++) / 255;
      R2Pixel pixel(r, g, b, 1);
      SetPixel(i, j, pixel);
    }
  }

  // Free unsigned char buffer for reading pixels
  delete [] buffer;

  // Return success
  return 1;
}



int R2Image::
WriteBMP(const char *filename) const
{
  // Open file
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  // Compute number of bytes in row
  int rowsize = 3 * width;
  if ((rowsize % 4) != 0) rowsize = (rowsize / 4 + 1) * 4;

  // Write file header 
  BITMAPFILEHEADER bmfh;
  bmfh.bfType = BMP_BF_TYPE;
  bmfh.bfSize = BMP_BF_OFF_BITS + rowsize * height;
  bmfh.bfReserved1 = 0;
  bmfh.bfReserved2 = 0;
  bmfh.bfOffBits = BMP_BF_OFF_BITS;
  WordWriteLE(bmfh.bfType, fp);
  DWordWriteLE(bmfh.bfSize, fp);
  WordWriteLE(bmfh.bfReserved1, fp);
  WordWriteLE(bmfh.bfReserved2, fp);
  DWordWriteLE(bmfh.bfOffBits, fp);

  // Write info header 
  BITMAPINFOHEADER bmih;
  bmih.biSize = BMP_BI_SIZE;
  bmih.biWidth = width;
  bmih.biHeight = height;
  bmih.biPlanes = 1;
  bmih.biBitCount = 24;       /* RGB */
  bmih.biCompression = BI_RGB;    /* RGB */
  bmih.biSizeImage = rowsize * (unsigned int) bmih.biHeight;  /* RGB */
  bmih.biXPelsPerMeter = 2925;
  bmih.biYPelsPerMeter = 2925;
  bmih.biClrUsed = 0;
  bmih.biClrImportant = 0;
  DWordWriteLE(bmih.biSize, fp);
  LongWriteLE(bmih.biWidth, fp);
  LongWriteLE(bmih.biHeight, fp);
  WordWriteLE(bmih.biPlanes, fp);
  WordWriteLE(bmih.biBitCount, fp);
  DWordWriteLE(bmih.biCompression, fp);
  DWordWriteLE(bmih.biSizeImage, fp);
  LongWriteLE(bmih.biXPelsPerMeter, fp);
  LongWriteLE(bmih.biYPelsPerMeter, fp);
  DWordWriteLE(bmih.biClrUsed, fp);
  DWordWriteLE(bmih.biClrImportant, fp);

  // Write image, swapping blue and red in each pixel
  int pad = rowsize - width * 3;
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      const R2Pixel& pixel = (*this)[i][j];
      double r = 255.0 * pixel.Red();
      double g = 255.0 * pixel.Green();
      double b = 255.0 * pixel.Blue();
      if (r >= 255) r = 255;
      if (g >= 255) g = 255;
      if (b >= 255) b = 255;
      fputc((unsigned char) b, fp);
      fputc((unsigned char) g, fp);
      fputc((unsigned char) r, fp);
    }

    // Pad row
    for (int i = 0; i < pad; i++) fputc(0, fp);
  }
  
  // Close file
  fclose(fp);

  // Return success
  return 1;  
}



////////////////////////////////////////////////////////////////////////
// PPM I/O
////////////////////////////////////////////////////////////////////////

int R2Image::
ReadPPM(const char *filename)
{
  // Open file
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  // Read PPM file magic identifier
  char buffer[128];
  if (!fgets(buffer, 128, fp)) {
    fprintf(stderr, "Unable to read magic id in PPM file");
    fclose(fp);
    return 0;
  }

  // skip comments
  int c = getc(fp);
  while (c == '#') {
    while (c != '\n') c = getc(fp);
    c = getc(fp);
  }
  ungetc(c, fp);

  // Read width and height
  if (fscanf(fp, "%d%d", &width, &height) != 2) {
    fprintf(stderr, "Unable to read width and height in PPM file");
    fclose(fp);
    return 0;
  }
	
  // Read max value
  double max_value;
  if (fscanf(fp, "%lf", &max_value) != 1) {
    fprintf(stderr, "Unable to read max_value in PPM file");
    fclose(fp);
    return 0;
  }
	
  // Allocate image pixels
  pixels = new R2Pixel [ width * height ];
  if (!pixels) {
    fprintf(stderr, "Unable to allocate memory for PPM file");
    fclose(fp);
    return 0;
  }

  // Check if raw or ascii file
  if (!strcmp(buffer, "P6\n")) {
    // Read up to one character of whitespace (\n) after max_value
    int c = getc(fp);
    if (!isspace(c)) putc(c, fp);

    // Read raw image data 
    // First ppm pixel is top-left, so read in opposite scan-line order
    for (int j = height-1; j >= 0; j--) {
      for (int i = 0; i < width; i++) {
        double r = (double) getc(fp) / max_value;
        double g = (double) getc(fp) / max_value;
        double b = (double) getc(fp) / max_value;
        R2Pixel pixel(r, g, b, 1);
        SetPixel(i, j, pixel);
      }
    }
  }
  else {
    // Read asci image data 
    // First ppm pixel is top-left, so read in opposite scan-line order
    for (int j = height-1; j >= 0; j--) {
      for (int i = 0; i < width; i++) {
	// Read pixel values
	int red, green, blue;
	if (fscanf(fp, "%d%d%d", &red, &green, &blue) != 3) {
	  fprintf(stderr, "Unable to read data at (%d,%d) in PPM file", i, j);
	  fclose(fp);
	  return 0;
	}

	// Assign pixel values
	double r = (double) red / max_value;
	double g = (double) green / max_value;
	double b = (double) blue / max_value;
        R2Pixel pixel(r, g, b, 1);
        SetPixel(i, j, pixel);
      }
    }
  }

  // Close file
  fclose(fp);

  // Return success
  return 1;
}



int R2Image::
WritePPM(const char *filename, int ascii) const
{
  // Check type
  if (ascii) {
    // Open file
    FILE *fp = fopen(filename, "w");
    if (!fp) {
      fprintf(stderr, "Unable to open image file: %s", filename);
      return 0;
    }

    // Print PPM image file 
    // First ppm pixel is top-left, so write in opposite scan-line order
    fprintf(fp, "P3\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    for (int j = height-1; j >= 0 ; j--) {
      for (int i = 0; i < width; i++) {
        const R2Pixel& p = (*this)[i][j];
        int r = (int) (255 * p.Red());
        int g = (int) (255 * p.Green());
        int b = (int) (255 * p.Blue());
        fprintf(fp, "%-3d %-3d %-3d  ", r, g, b);
        if (((i+1) % 4) == 0) fprintf(fp, "\n");
      }
      if ((width % 4) != 0) fprintf(fp, "\n");
    }
    fprintf(fp, "\n");

    // Close file
    fclose(fp);
  }
  else {
    // Open file
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
      fprintf(stderr, "Unable to open image file: %s", filename);
      return 0;
    }
    
    // Print PPM image file 
    // First ppm pixel is top-left, so write in opposite scan-line order
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    for (int j = height-1; j >= 0 ; j--) {
      for (int i = 0; i < width; i++) {
        const R2Pixel& p = (*this)[i][j];
        int r = (int) (255 * p.Red());
        int g = (int) (255 * p.Green());
        int b = (int) (255 * p.Blue());
        fprintf(fp, "%c%c%c", r, g, b);
      }
    }
    
    // Close file
    fclose(fp);
  }

  // Return success
  return 1;  
}



////////////////////////////////////////////////////////////////////////
// JPEG I/O
////////////////////////////////////////////////////////////////////////


// #define USE_JPEG
#ifdef USE_JPEG
  extern "C" { 
#   define XMD_H // Otherwise, a conflict with INT32
#   undef FAR // Otherwise, a conflict with windows.h
#   include "jpeg/jpeglib.h"
  };
#endif



int R2Image::
ReadJPEG(const char *filename)
{
#ifdef USE_JPEG
  // Open file
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  // Initialize decompression info
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  // Remember image attributes
  width = cinfo.output_width;
  height = cinfo.output_height;
  npixels = width * height;
  int ncomponents = cinfo.output_components;

  // Allocate pixels for image
  pixels = new R2Pixel [ npixels ];
  if (!pixels) {
    fprintf(stderr, "Unable to allocate memory for BMP file");
    fclose(fp);
    return 0;
  }

  // Allocate unsigned char buffer for reading image
  int rowsize = ncomponents * width;
  if ((rowsize % 4) != 0) rowsize = (rowsize / 4 + 1) * 4;
  int nbytes = rowsize * height;
  unsigned char *buffer = new unsigned char [nbytes];
  if (!buffer) {
    fprintf(stderr, "Unable to allocate temporary memory for JPEG file");
    fclose(fp);
    return 0;
  }

  // Read scan lines 
  // First jpeg pixel is top-left, so read pixels in opposite scan-line order
  while (cinfo.output_scanline < cinfo.output_height) {
    int scanline = cinfo.output_height - cinfo.output_scanline - 1;
    unsigned char *row_pointer = &buffer[scanline * rowsize];
    jpeg_read_scanlines(&cinfo, &row_pointer, 1);
  }

  // Free everything
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  // Close file
  fclose(fp);

  // Assign pixels
  for (int j = 0; j < height; j++) {
    unsigned char *p = &buffer[j * rowsize];
    for (int i = 0; i < width; i++) {
      double r, g, b, a;
      if (ncomponents == 1) {
        r = g = b = (double) *(p++) / 255;
        a = 1;
      }
      else if (ncomponents == 1) {
        r = g = b = (double) *(p++) / 255;
        a = 1;
        p++;
      }
      else if (ncomponents == 3) {
        r = (double) *(p++) / 255;
        g = (double) *(p++) / 255;
        b = (double) *(p++) / 255;
        a = 1;
      }
      else if (ncomponents == 4) {
        r = (double) *(p++) / 255;
        g = (double) *(p++) / 255;
        b = (double) *(p++) / 255;
        a = (double) *(p++) / 255;
      }
      else {
        fprintf(stderr, "Unrecognized number of components in jpeg image: %d\n", ncomponents);
        return 0;
      }
      R2Pixel pixel(r, g, b, a);
      SetPixel(i, j, pixel);
    }
  }

  // Free unsigned char buffer for reading pixels
  delete [] buffer;

  // Return success
  return 1;
#else
  fprintf(stderr, "JPEG not supported");
  return 0;
#endif
}


	

int R2Image::
WriteJPEG(const char *filename) const
{
#ifdef USE_JPEG
  // Open file
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  // Initialize compression info
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, fp);
  cinfo.image_width = width; 	/* image width and height, in pixels */
  cinfo.image_height = height;
  cinfo.input_components = 3;		/* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
  cinfo.dct_method = JDCT_ISLOW;
  jpeg_set_defaults(&cinfo);
  cinfo.optimize_coding = TRUE;
  jpeg_set_quality(&cinfo, 95, TRUE);
  jpeg_start_compress(&cinfo, TRUE);
	
  // Allocate unsigned char buffer for reading image
  int rowsize = 3 * width;
  if ((rowsize % 4) != 0) rowsize = (rowsize / 4 + 1) * 4;
  int nbytes = rowsize * height;
  unsigned char *buffer = new unsigned char [nbytes];
  if (!buffer) {
    fprintf(stderr, "Unable to allocate temporary memory for JPEG file");
    fclose(fp);
    return 0;
  }

  // Fill buffer with pixels
  for (int j = 0; j < height; j++) {
    unsigned char *p = &buffer[j * rowsize];
    for (int i = 0; i < width; i++) {
      const R2Pixel& pixel = (*this)[i][j];
      int r = (int) (255 * pixel.Red());
      int g = (int) (255 * pixel.Green());
      int b = (int) (255 * pixel.Blue());
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      *(p++) = r;
      *(p++) = g;
      *(p++) = b;
    }
  }



  // Output scan lines
  // First jpeg pixel is top-left, so write in opposite scan-line order
  while (cinfo.next_scanline < cinfo.image_height) {
    int scanline = cinfo.image_height - cinfo.next_scanline - 1;
    unsigned char *row_pointer = &buffer[scanline * rowsize];
    jpeg_write_scanlines(&cinfo, &row_pointer, 1);
  }

  // Free everything
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  // Close file
  fclose(fp);

  // Free unsigned char buffer for reading pixels
  delete [] buffer;

  // Return number of bytes written
  return 1;
#else
  fprintf(stderr, "JPEG not supported");
  return 0;
#endif
}






