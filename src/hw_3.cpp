void R2Image::
Blur(double sigma)
{
  // Gaussian blur of the image. Separable solution is preferred
  
  R2Image tempImage(*this);


  
  // Compute the Kernel (1D convolution)
  int size = sigma * 6 + 1; 
  int cWidth = sigma * 3; // 3 one ach side
  double kernel[size];

  std::cout << size;

  double weights = 0.0;
  for (int i=-cWidth; i<=cWidth; i++){
    int idx = i + cWidth;
    kernel[idx] = exp( -(i*i)/sigma*sigma*2 / (M_PI * sigma*sigma*2) );
    std::cout << kernel[idx] << "\n";
    weights += kernel[idx];
  }

  std::cout << "\n Total Weights: " << weights;

  // normalize the kernel
  double kernel_sum = 0.0;
  for (int i=0; i<size; i++){
    kernel[i] /= weights;
    std :: cout << kernel[i] << "\n";
    
    kernel_sum += kernel[i];
  }

  std::cout << kernel_sum;

  /* separable linear filters */

  // y - direction
  int s=1;
  for (int y=cWidth; y < height-cWidth; y++){
    for (int x=0; x < width; x++){
      
      // local vars
      R2Pixel* val = new R2Pixel();
      
      for (int ly=-cWidth; ly <= cWidth; ly++){
        *val += Pixel(x, y+ly) * kernel[ly + cWidth];
        // printf("%d -%d\n",x,y);
      }
      tempImage.Pixel(x,y) = *val;
      tempImage.Pixel(x,y).Clamp();
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
      Pixel(x,y).Clamp();
    }
  }
