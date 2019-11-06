#include <math.h> // for sin, cos
// #include <stdint.h>

#define SCREEN_SIZE 16
#define DIGIT_ROWS 5
#define DIGIT_COLUMNS 3
#define PI 3.14159265358979323846 // from math.h?
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

const uint16_t digits[10] = {31599, 9362, 29671, 29391, 23497, 31183, 31215, 29257, 31727, 31695};
uint16_t screen[SCREEN_SIZE];
uint16_t minutes = 12*60 + 34, seconds = 56;

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
    const uint8_t ystep = (y1 < y2) ? 1 : -1;
    uint8_t y = (uint8_t)y1;

    const uint8_t maxX = (uint8_t)x2;

    for(uint8_t x=(uint8_t)x1; x<maxX; x++)
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

void displayDigitalClockOnScreen()
{
    uint16_t hours;
    hours = minutes / 60;
    minutes = minutes % 60;
    
    screen[0] = ~((1 << MAX(8 - seconds, 0)) - 1) & 0xFF;
    if(seconds > 52)
        screen[0] |= ~((1 << (68 - seconds)) - 1);
    for(uint8_t row=1;row<SCREEN_SIZE-1;row++)
    {
        screen[row] = (row < seconds - 7 ? 1 : 0);
        screen[row] |= (row > 52 - seconds ? 1 << 15 : 0);
    }
    if(seconds < 38)
        screen[SCREEN_SIZE - 1] = (1 << MAX(seconds - 22, 0)) - 1;
    else
        screen[SCREEN_SIZE - 1] = 0xFFFF;

    // Display hours and minutes
    for(uint8_t row=0;row<DIGIT_ROWS;row++)
    {
        screen[row+2] |= ((digits[hours / 10] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 9; // First digit of hours
        screen[row+2] |= ((digits[hours % 10] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 4; // Second digit of hours
        screen[row+9] |= ((digits[minutes / 10] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 9; // First digit of minutes
        screen[row+9] |= ((digits[minutes % 10] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 4; // Second digit of minutes
    }
}

void displayAnalogClockOnScreen()
{
    uint16_t hours;
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

// #include <bitset>
// #include <iostream>
// uint8_t main(void)
// {
//     for(uint8_t i=0;i<61;i++)
//     {
//         seconds = i;
//         displayDigitalClockOnScreen();
//         for(uint8_t i=0;i<SCREEN_SIZE;i++)
//             std::cout << std::bitset<16>(screen[i]).to_string() << std::endl;
//         std::cout << std::endl;
//     }     
//     return 0;
// }