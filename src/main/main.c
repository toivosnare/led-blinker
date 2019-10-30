/*
* ledikello.c
*
* Created: 30/10/2019 15.41.52
* Author : Elias
*/

#define F_CPU 8000000

/************************************************************************/
/*
Katodit - GND | top to down
PA3
PA4
PA5
PA6
PA7
PE0
PE1
PE2
PC7
PC6
PC5
PC4
PC3
PC2
PC1
PC0
Anodit 4,5V | left to right
PA2
PD7
PA1
PD6
PA0
PD5
PB0
PD4
PB1
PD3
PB2
PD2
PB3
PD1
PB4
PD0
*/
/************************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define ROWS 16
#define ROW_WAIT_US 250
#define USECS_PER_TIMER 65536 / F_CPU // 8192
#define DEBOUNCE_TIME_MS 20

volatile uint16_t usecs, secs, mins;

void setup();
void tick_sleep();
void debounce();
void check_button();
void clear_all();
void clear_for_draw();
void set_row(uint8_t);
void set_column(uint16_t);

int main(void)
{
	setup();
	/* Replace with your application code */
	while (1)
	{
		//check_button();
		for (uint8_t i = 0; i < ROWS; ++i) {
			// Clear.
			clear_for_draw();
			// Render.
			set_row(i);
			// Wait.
			_delay_us(ROW_WAIT_US);
		}
	}
}

void setup() {
	// Set LED pins as OUTPUT.
	// Cathodes.
	DDRA |= 0xF8;
	DDRE |= 0x07;
	DDRC |= 0xFF;
	// Anodes.
	DDRA |= 0x07;
	DDRD |= 0xFF;
	DDRB |= 0x1F;
	// Set pins LOW.
	clear_all();
	// Reset timing numbers.
	usecs = 0;
	secs = 0;
	mins = 0;
	// Enable interrupts.
	sei();
}

// Timed interrupt which increment the clock and checks for 
// button press.
ISR(TIMER0_OVF_vect) {
	usecs += USECS_PER_TIMER;
	
}

void tick_sleep() {
	sleep_mode();
}

void debounce() {
	_delay_ms(DEBOUNCE_TIME_MS);
}

void check_button() {
	// Set button pin as input.
	DDRD &= ~(1 << PD2);
	// Make sure input pull-up is enabled.
	PORTD |= (1 << PD2);
	
	// If button is pressed.
	if (~PIND & (1 << PD2)) {
		clear_for_draw();
		debounce();
		// Wait to not press.
		while(~PIND & (1 << PD2));
		debounce();
	}
	// Set button pin back to output.
	DDRD |= (1 << PD2);
}

// Sets all LED output pins to LOW.
// Called as button is pressed?
void clear_all() {
	// Cathodes.
	PORTA &= ~0xF8;
	PORTE &= ~0x07;
	PORTC &= ~0xFF;
	// Anodes.
	PORTA &= ~0x07;
	PORTD &= ~0xFF;
	PORTB &= ~0x1F;
}

// Sets anode pins to LOW and cathode pins to HIGH.
// Used as an intermediate step when rendering line by line.
// NOTE: Has to be called before every line render. Even after clear_all().
void clear_for_draw() {
	// Cathodes.
	PORTA |= 0xF8;
	PORTE |= 0x07;
	PORTC |= 0xFF;
	// Anodes.
	PORTA &= ~0x07;
	PORTD &= ~0xFF;
	PORTB &= ~0x1F;
}

// Takes number between 0-15 to activate certain row. Numbers over 15 are make
// undefined behavior. 
void set_row(uint8_t row) {
	if (row < 5) {
		PORTA &= ~(1 << (row + PA3));
	}
	else if (row < 8) {
		PORTE &= ~(1 << (row - PA5));
	}
	else {
		PORTC &= ~(1 << (15 - row));
	}
}


void set_column(uint16_t col) {
	
}