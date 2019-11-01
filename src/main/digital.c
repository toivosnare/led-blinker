#include <math.h> // For pow function

#define SCREEN_SIZE 16
#define DIGIT_ROWS 5
#define DIGIT_COLUMNS 3

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

// Display digital clock time (in seconds) as pixels on 16x16 screen
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
        screen[row+9] |= ((digits[firstDigit(minutes)] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 9; // First digit of minutes
        screen[row+9] |= ((digits[lastDigit(minutes)] >> ((DIGIT_ROWS - 1 - row) * DIGIT_COLUMNS)) & 7) << 4; // Second digit of minutes
    }
    
    // Display seconds as a ring around the screen
    if(seconds<16)
    {
        for(int i=SCREEN_SIZE;i>SCREEN_SIZE-seconds;i--)
        {
            screen[0] += pow(2, i - 1);
        }
    }
    else
    {
        screen[0] = 65535;
        if(seconds<31)
        {
            for(int row=1;row<=seconds-16;row++)
                screen[row] |= 1;
        }
        else
        {
            for(int row=1;row<SCREEN_SIZE-1;row++)
                screen[row] |= 1;
            if(seconds<47)
            {
                for(int i=0;i<=seconds-31;i++)
                {
                    screen[SCREEN_SIZE - 1] += pow(2, i);
                }
            }
            else
            {
                screen[SCREEN_SIZE - 1] = 65535;
                
                for(int row=14;row>60-seconds;row--)
                {
                    screen[row] |= 32768;
                }
            }   
        }
    }
}

// Example usage:
#include <iostream>

int main(void)
{
    clearScreen();
    displayDigitalClockOnScreen(12 * 60*60 + 34 * 60 + 56);
    for(int i=0;i<SCREEN_SIZE;i++)
    {
        std::cout << screen[i] << std::endl;
    }
    return 0;
}