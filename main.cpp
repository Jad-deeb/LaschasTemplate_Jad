#include <iostream>
#include "core/window.h"

// draw a filled in rect
void drawRect(ScreenPixelData* pixelData, int xOffset, int yOffset, int width, int height, unsigned int color);
void drawCircleSimple(ScreenPixelData* pixelData, int xOffset, int yOffset, int radius, unsigned int color);
void drawCircleBresenham(ScreenPixelData* pixelData, int xOffset, int yOffset, int radius, unsigned int color);

// init window width and height, keeping it outside to be accessible in functions if needed
unsigned int windowWidth = 400;
unsigned int windowHeight = 400;

// Entry point of the application
int main(int argc, char* argv[])
{

  Window wnd1(windowWidth, windowHeight, "Wnd1");
  //Window wnd2(400, 400, "Wnd2");

  bool isRunning1;
  //bool isRunning2;

  do
  {
    isRunning1 = wnd1.exec();
    //isRunning2 = wnd2.exec();

    if (isRunning1)
      wnd1.present();

    /*if (isRunning2)
      wnd2.present();*/

    ScreenPixelData* pixelData = wnd1.getPixelData();
    //drawRect(pixelData, 10, 10, 200, 200, );
    drawCircleSimple(pixelData, 200, 200, 100, 0xFF5733);
    
    Sleep(10);

  } while (isRunning1);// || isRunning2);

  return 0;
}

void drawRect(ScreenPixelData* pixelData, int xOffset, int yOffset, int width, int height, unsigned int color)
{
    if (pixelData != nullptr) // make sure pixelData was initialized properly
    {
        for (width = xOffset; width < 300; ++width) // foreach pixel in y
        {
            for (height = yOffset; height < 300; ++height) // foreach pixel in x
            {
                pixelData->data[width + height * pixelData->pitch / 4] = color; // lascha plx explain the /4
            }
        }
    }
}

/*
 * this is actually cool, sauce : https://stackoverflow.com/questions/1201200/fast-algorithm-for-drawing-filled-circles
 */
void drawCircleSimple(ScreenPixelData* pixelData, int xOffset, int yOffset, int radius, unsigned int color)
{
    if (pixelData != nullptr)
    {
        for (int y =-radius; y <= radius; y++) //extent of radius, we go from -radius to radius, filling the diameter in y axis
        {
            for(int x=-radius; x<=radius; x++) // potato potato to x axis
            {
                // since we're doing radius extent, that's radius pow2, thus the x*x.
                if ((x * x) + (y * y) <= (radius * radius)) //brackets for my mental health
                {
                    pixelData->data[xOffset + x + (yOffset + y) * pixelData->pitch / 4] = 0xFF5733;
                }
            }
        }
    }
}

/*
 * bitshifting a binary to the left is the same as mult2 wtf ? left bitshifting rad makes diameter..
 */
void drawCircleBresenham(ScreenPixelData* pixelData, int xOffset, int yOffset, int radius, unsigned int color)
{
	
}