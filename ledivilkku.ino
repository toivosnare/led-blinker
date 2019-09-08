
// Use data type where sizeof(led_bit_t) * 8 >= LED_WIDTH.
using led_bit_t = uint16_t;
using idx_t = uint8_t;

const int LED_WIDTH = 3;
const int LED_HEIGHT = 3;
const int LED_COUNT = LED_WIDTH * LED_HEIGHT;
const int LED_BITS_TO_BYTES = (LED_COUNT - 1) / 8 + 1;

const int X_IDX_OFFSET = 5;
const int Y_IDX_OFFSET = 2;

const bool shape1[LED_COUNT] = {
  0, 0, 1, 0, 0, 0, 0, 0, 0
};

const bool shape2[LED_COUNT] = {
  0, 1, 0, 0, 0, 1, 0, 0, 0
};

const bool shape3[LED_COUNT] = {
  1, 0, 0, 0, 1, 0, 0, 0, 1
};

const bool shape4[LED_COUNT] = {
  0, 0, 0, 1, 0, 0, 0, 1, 0
};

const bool shape5[LED_COUNT] = {
  0, 0, 0, 0, 0, 0, 1, 0, 0
};

led_bit_t ledArray[LED_BITS_TO_BYTES];

/**
 * BitArray object stores bits in the most space-efficient manner
 * and does bitwise operations to them using little-endian byte order.
 * NOTE:
 * With direct access to bit_array, do not modify bits outside your
 * defined Bits count as it might cause undefined behaviour, especially
 * with shift operators.
 * @param {string} Bits Who wrote the book
 */
template<uint16_t Bits, uint16_t BYTE_LENGTH = (Bits - 1) / 8 + 1>
class BitArray {
    using bit_array_t = uint8_t;
    using index_t = uint8_t;
    static_assert(sizeof(bit_array_t) == 1, "bit_array_t must be sized 1 byte.");
    static_assert(BYTE_LENGTH == (Bits - 1) / 8 + 1, "Do not modify the value of BYTE_LENGTH.");
    static_assert(Bits > 0, "Bit-size must be > 0.");

  public:
    BitArray() {}
    ~BitArray() {}

    // Complete. Max shift == 8.
    void shift_left(index_t sl) {
      for (index_t i = 0; i < BYTE_LENGTH - 1; ++i) {
        bit_array[i] <<= sl;
        bit_array[i] |= bit_array[i + 1] >> (BITS_PER_BYTE - sl);
      }
      bit_array[BYTE_LENGTH - 1] <<= sl;
    }

    // Complete. Max shift == 8.
    void shift_right(index_t sr) {
      for (index_t i = BYTE_LENGTH - 1; i > 0; --i) {
        bit_array[i] >>= sr;
        bit_array[i] |= bit_array[i - 1] << (BITS_PER_BYTE - sr);
      }
      bit_array[0] >>= sr;
    }

    void not_op() {
      for (index_t i = 0; i < BYTE_LENGTH; ++i)
        bit_array[i] = ~bit_array[i];
    }

  public:
    static const uint16_t BITS_PER_BYTE = 8;
    
    bit_array_t bit_array[BYTE_LENGTH];
};


// Turns all LEDs off.
inline void clearLedArray_fast() {
  PORTD = B00011100;
}

// Works correctly when led row has 3 least significant bits
// assigned as LED states.
void printLedRow_fast(led_bit_t led_row, led_bit_t y) {
  // Set all LED's off.
  clearLedArray_fast();
  // Specific for y_idx_offset == 2. Sets y-axis LED source low.
  PORTD &= ~(1 << (y + Y_IDX_OFFSET));
  // and three first bits of led_row to set pin high.
  PORTD |= (led_row << X_IDX_OFFSET) & 0xE0;
}

void printLedArray_fast(led_bit_t led_array, uint32_t rowDelayMicros) {
  for (led_bit_t y = 0; y < LED_HEIGHT; ++y) {
    printLedRow_fast(led_array >> (y * LED_WIDTH), y);
    _delay_us(rowDelayMicros);
  }
}

void clearLedArray() {
  for (int i = Y_IDX_OFFSET; i < 5; ++i)
    digitalWrite(i, HIGH);
  for (int i = X_IDX_OFFSET; i < 8; ++i)
    digitalWrite(i, LOW);
}

void printLedRow(bool *row_leds, idx_t y) {
  // Set all LED's off.
  clearLedArray();
  // Set current row active.
  digitalWrite(y + Y_IDX_OFFSET, LOW);

  // Set selected LED's on.
  for (idx_t x = 0; x < LED_WIDTH; ++x) {
    if (row_leds[x])
      digitalWrite(x + X_IDX_OFFSET, HIGH);
  }
}

// Print array of leds.
void printLedArray(bool *array_leds, unsigned long rowDelayMicros) {
  bool row[3];
  for (idx_t y = 0; y < LED_HEIGHT; ++y) {
    // Copy LED states from array to an array representing
    // single row of LED states.
    for (idx_t i = 0; i < LED_WIDTH; ++i)
      row[i] = array_leds[i + y * LED_WIDTH];
    printLedRow(row, y);
    delayMicroseconds(rowDelayMicros);
  }
}


void setup() {
  for (int i = 2; i < 8; ++i)
    pinMode(i, OUTPUT);
}


void loop() {
  printLedArray_fast(0x3, 1000);
}
