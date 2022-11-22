#include <iostream>
#include "core/window.h"

void setPixel(ScreenPixelData* pixelData, int x, int y, unsigned int color);

void drawRect(ScreenPixelData* pixelData, int xOffset, int yOffset, int width, int height, unsigned int color);
void drawCircleSimple(ScreenPixelData* pixelData, int xOffset, int yOffset, int radius, unsigned int color);
void drawCricleMidPoint(ScreenPixelData* pixelData, int xOffset, int yOffset, int radius, unsigned int color = -1);

// init window width and height, keeping it outside to be accessible in functions if needed
unsigned int windowWidth = 400;
unsigned int windowHeight = 400;

// Entry point of the application
int main(int argc, char* argv[])
{

  Window wnd1(windowWidth, windowHeight, "Wnd1");
  Window wnd2(windowWidth, windowHeight, "Wnd2");
  Window wnd3(windowWidth, windowHeight, "Wnd3");

  bool isRunning1;
  bool isRunning2;
  bool isRunning3;

  do
  {
    isRunning1 = wnd1.exec();
    isRunning2 = wnd2.exec();
    isRunning3 = wnd3.exec();

    if (isRunning1)
      wnd1.present();
      ScreenPixelData* pixelData = wnd1.getPixelData();
      drawRect(pixelData, 10, 10, 200, 100, 0xFF5733);

    if (isRunning2)
      wnd2.present();
  	  ScreenPixelData* pixelData1 = wnd2.getPixelData();
      drawCircleSimple(pixelData1, 200, 200, 100, 0x0000FF);

  	if (isRunning3)
        wnd3.present();
  		ScreenPixelData* pixelData3 = wnd3.getPixelData();
	    drawCricleMidPoint(pixelData3, 175, 175, 150, 0x0000FF);
    
    Sleep(10);

  } while (isRunning1 || isRunning2 || isRunning3);

  return 0;
}

void setPixel(ScreenPixelData* pixelData, int x, int y, unsigned int color)
{
    pixelData->data[x + y * pixelData->pitch / 4] = color;
}

void drawRect(ScreenPixelData* pixelData, int xOffset, int yOffset, int width, int height, unsigned int color)
{
    if (pixelData != nullptr) // make sure pixelData was initialized properly
    {
        for (int y = yOffset; y < height; ++y) // foreach pixel in y
        {
            for (int x = xOffset; x < width; ++x) // foreach pixel in x
            {
                //pixelData->data[width + height * pixelData->pitch / 4] = color; // lascha plx explain the /4
                setPixel(pixelData, x, y, color);
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
                    //pixelData->data[xOffset + x + (yOffset + y) * pixelData->pitch / 4] = color;
                    setPixel(pixelData, (xOffset + x), (yOffset + y), color);
                }
            }
        }
    }
}


/*
 * sauce : https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
 * I was today years old when I learned what an octant is, 1/8th of a circumference. -- I'd say it's average kek.
 * So we draw a single octant, and then copy pasta it x amount of times.
 * We draw a point then compare to see if that points is within the perimeter or not, if not, we skip it, if it is, we draw it.
 * Can't tell you I understand the formula that well, but I understand the general idea behind it :)
 */
void drawCricleMidPoint(ScreenPixelData* pixelData, int xOffset, int yOffset, int radius, unsigned int color)
{
    int x = radius, y = 0;
    // I do understand that it's P in which P is a function, however point makes me visualize it better.
    int point = 1 - radius; // first point (radius,0)

    while (x>y)
    {
        y++;

        if (point <= 0) // point inside or on perim
        {
            point = point + 2 * y + 1; // circle magic madoogle
        }
        else // not inside that shit
        {
            x--;
            point = point + 2 * y - 2 * x + 1; // circle magic madoogle
        }

        // stop when all is printed
        if (x < y)
            break;

        setPixel(pixelData, (xOffset + x), (yOffset + y), color);
        setPixel(pixelData, (xOffset - x), (yOffset + y), color);
        setPixel(pixelData, (xOffset + x), (yOffset - y), color);
        setPixel(pixelData, (xOffset - x), (yOffset - y), color);

        if (x != y) // don't redraw points
        {
            setPixel(pixelData, (xOffset + y), (yOffset + x), color);
            setPixel(pixelData, (xOffset - y), (yOffset + x), color);
            setPixel(pixelData, (xOffset + y), (yOffset - x), color);
            setPixel(pixelData, (xOffset - y), (yOffset - x), color);
        }

    }
}