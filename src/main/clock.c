#include <math.h> // for pow, sin, cos

#define SCREEN_SIZE 16
#define DIGIT_ROWS 5
#define DIGIT_COLUMNS 3
#define PI 3.14159265358979323846 // from math.h?

const uint16_t digits[10] = {31599, 25746, 29671, 29391, 23497, 31183, 31215, 29257, 31727, 31695};
uint16_t screen[SCREEN_SIZE];

// Find the first digit of n
int firstDigit(int n) 
{ 
    while (n >= 10)  
        n /= 10; 
    return n; 
}

// Find the last digit of n
int lastDigit(int n) 
{ 
    return (n % 10); 
}

void clearScreen()
{
    for(int i=0;i<SCREEN_SIZE;i++)
    {
        screen[i] = 0;
    }
}

// Bresenham's line algorithm
void drawLine(float x1, float y1, float x2, float y2)
{
  const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
  float temp;
  if(steep)
  {
    temp = x1;
    x1 = y1;
    y1 = temp;

    temp = x2;
    x2 = y2;
    y2 = temp;
  }
 
  if(x1 > x2)
  {
    temp = x1;
    x1 = x2;
    x2 = temp;

    temp = y1;
    y1 = y2;
    y2 = temp;
  }
 
  const float dx = x2 - x1;
  const float dy = fabs(y2 - y1);
 
  float error = dx / 2.0f;
  const int ystep = (y1 < y2) ? 1 : -1;
  int y = (int)y1;
 
  const int maxX = (int)x2;
 
  for(int x=(int)x1; x<maxX; x++)
  {
    if(steep)
    {
        screen[x] |= (1 << (SCREEN_SIZE - y));
    }
    else
    {
        screen[y] |= (1 << (SCREEN_SIZE - x));
    }
    error -= dy;
    if(error < 0)
    {
        y += ystep;
        error += dx;
    }
  }
}

void displayDigitalClockOnScreen(int time)
{
    time %= 60 * 60 * 24;
    int seconds, hours, minutes;
    minutes = time / 60;
    seconds = time % 60;
    hours = minutes / 60;
    minutes = minutes % 60;

    // Display hours and minutes
    for(int row=0;row<DIGIT_ROWS;row++)
    {
        screen[row+2] = ((digits[firstDigit(hours)] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 9; // First digit of hours
        screen[row+2] |= ((digits[lastDigit(hours)] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 4; // Second digit of hours
        screen[row+9] = ((digits[firstDigit(minutes)] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 9; // First digit of minutes
        screen[row+9] |= ((digits[lastDigit(minutes)] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 4; // Second digit of minutes
    }
    
    // Display seconds as a ring around the screen
    for(int s=1;s<=seconds;s++)
    {
        if(s < 17)
            screen[0] |= (1 << (16 - s));
        else if(s < 32)
            screen[s - 16] |= 1;
        else if(s < 47)
            screen[15] |= (1 << (s - 31));
        else if(s < 61)
            screen[61 - s] |= (1 << 15);

    }
    // if(seconds<16)
    // {
    //     for(int i=SCREEN_SIZE;i>SCREEN_SIZE-seconds;i--)
    //         screen[0] += pow(2, i - 1);
    // }
    // else
    // {
    //     screen[0] = 65535;
    //     if(seconds<31)
    //     {
    //         for(int row=1;row<=seconds-16;row++)
    //             screen[row] |= 1;
    //     }
    //     else
    //     {
    //         for(int row=1;row<SCREEN_SIZE-1;row++)
    //             screen[row] |= 1;
    //         if(seconds<47)
    //         {
    //             for(int i=0;i<=seconds-31;i++)
    //                 screen[SCREEN_SIZE - 1] += pow(2, i);
    //         }
    //         else
    //         {
    //             screen[SCREEN_SIZE - 1] = 65535;
    //             for(int row=14;row>60-seconds;row--)
    //                 screen[row] |= 32768;
    //         }   
    //     }
    // }
}

void displayAnalogClockOnScreen(int time)
{
    time %= 60 * 60 * 24;
    int seconds, hours, minutes;
    minutes = time / 60;
    seconds = time % 60;
    hours = minutes / 60;
    minutes = minutes % 60;
    
    const float x0 = 8.25f;
    const float y0 = 8.25f;

    // Hours
    float length = 5.f;
    float angle = 2.5f * PI - (hours / 12.f * 2.f * PI);
    float x1 = x0 + length * cos(angle);
    float y1 = y0 - length * sin(angle);
    drawLine(x0, y0, x1, y1);

    // Minutes
    length = 8.f;
    angle = 2.5f * PI - (minutes / 60.f * 2.f * PI);
    x1 = x0 + length * cos(angle);
    y1 = y0 - length * sin(angle);
    drawLine(x0, y0, x1, y1);

    // Seconds
    angle = 2.5f * PI - (seconds / 60.f * 2.f * PI);
    x1 = x0 + length * cos(angle);
    y1 = y0 - length * sin(angle);
    drawLine(x0, y0, x1, y1);
}

// Example usage:
// #include <iostream>
// int main(void)
// {
//     clearScreen();
//     displayDigitalClockOnScreen(11 * 60*60 + 11 * 60 + 11);
//     displayAnalogClockOnScreen(6 * 60*60 + 15 * 60 + 39);
//     for(int i=0;i<SCREEN_SIZE;i++)
//     {
//         std::cout << screen[i] << std::endl;
//     }
//     return 0;
// }