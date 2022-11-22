#include <iostream>
#include "core/window.h"

// draw a filled in rect
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
    //drawCircleSimple(pixelData, 200, 200, 100, 0xFF5733);
    drawCricleMidPoint(pixelData, 175, 175, 150);
    
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
 * sauce : https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
 * I was today years old when I learned what an octant is, 1/8th of a circumference. -- I'd say it's average kek.
 * So we draw a single octant, and then copy pasta it x amount of times.
 * We draw a point then compare to see if that points is within the perimeter or not, if not, we skip it, if it is, we draw it.
 * Can't tell you I understand the formula that well, but I understand the general idea behind it :)
 */
void drawCricleMidPoint(ScreenPixelData* pixelData, int xOffset, int yOffset, int radius, unsigned int color)
{
    int x = radius, y = 0;
    int P = 1 - radius; // first point (radius,0)

    while (x>y)
    {
        y++;

        if (P <= 0) // point inside or on perim
        {
            P = P + 2 * y + 1;
        }
        else
        {
            x--;
            P = P + 2 * y - 2 * x + 1;
        }

        // stop when all is printed
        if (x < y)
            break;

        pixelData->data[xOffset + x + (yOffset + y) * pixelData->pitch / 4] = 0xFF5733;
        pixelData->data[xOffset + -x + (yOffset + y) * pixelData->pitch / 4] = 0x00FF33;
        pixelData->data[xOffset + x + (yOffset + -y) * pixelData->pitch / 4] = 0xF057FF;
        pixelData->data[xOffset + -x + (yOffset + -y) * pixelData->pitch / 4] = 0x0000FF;

        if (x != y) // don't redraw points
        {
            pixelData->data[xOffset + y + (yOffset + x) * pixelData->pitch / 4] = 0x00FF33;
            pixelData->data[xOffset + -y + (yOffset + x) * pixelData->pitch / 4] = 0xFF5733;
            pixelData->data[xOffset + y + (yOffset + -x) * pixelData->pitch / 4] = 0x0000FF;
            pixelData->data[xOffset + -y + (yOffset + -x) * pixelData->pitch / 4] = 0xF057FF;
        }

    }
}