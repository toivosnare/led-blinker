
// Use data type where sizeof(led_bit_t) * 8 >= LED_WIDTH.
using led_bit_t = uint16_t;
using idx_t = uint8_t;

const int LED_WIDTH = 3;
const int LED_HEIGHT = 3;
const int LED_COUNT = LED_WIDTH * LED_HEIGHT;
const int LED_BITS_TO_BYTES = (LED_COUNT - 1) / 8 + 1;

const int X_IDX_OFFSET = 5;
const int Y_IDX_OFFSET = 2;

const bool shape1[LED_COUNT] = {0, 0, 1, 0, 0, 0, 0, 0, 0};

const bool shape2[LED_COUNT] = {0, 1, 0, 0, 0, 1, 0, 0, 0};

const bool shape3[LED_COUNT] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

const bool shape4[LED_COUNT] = {0, 0, 0, 1, 0, 0, 0, 1, 0};

const bool shape5[LED_COUNT] = {0, 0, 0, 0, 0, 0, 1, 0, 0};

led_bit_t ledArray[LED_BITS_TO_BYTES];

/**
 * BitArray object stores bits in the most space-efficient manner
 * and does bitwise operations to them using big-endian byte order.
 * NOTE:
 * With direct access to bit_array, do not modify bits outside your
 * defined Bits count as it might cause undefined behaviour, especially
 * with shift operators.
 */
template <uint16_t Bits, uint16_t BYTE_LENGTH = (Bits - 1) / 8 + 1>
class BitArray {
  using bit_array_t = uint8_t;
  using index_t = uint8_t;
  using this_type = BitArray<Bits, BYTE_LENGTH>;

  static_assert(sizeof(bit_array_t) == 1, "bit_array_t must be sized 1 byte.");
  static_assert(BYTE_LENGTH == (Bits - 1) / 8 + 1,
                "Do not modify the value of BYTE_LENGTH.");
  static_assert(Bits > 0, "Bit-size must be > 0.");

public:
  static const uint8_t BITS_IN_BYTE = 8;

public:
  BitArray() {}
  ~BitArray() {}

  // @brief Access array data. Bounds not checked.
  bit_array_t operator[](index_t idx) { return bit_array[idx]; }

  // @brief Shifts bits left specified amount.
  // @param shift The amount to shift.
  // @note Only positive shifts are executed.
  void shift_left(index_t shift) {
    if (shift > 0) {
      for (index_t i = 0; i < shift / BITS_IN_BYTE; ++i)
        shift_left_impl(BITS_IN_BYTE);
      shift_left_impl(shift % BITS_IN_BYTE);
    }
  }

  // @brief Shifts bits right specified amount.
  // @param shift The amount to shift.
  // @note Only positive shifts are executed.
  void shift_right(index_t shift) {
    if (shift > 0) {
      for (index_t i = 0; i < shift / BITS_IN_BYTE; ++i)
        shift_right_impl(BITS_IN_BYTE);
      shift_right_impl(shift % BITS_IN_BYTE);
    }
  }

  // @brief Returns the amount of bytes used by the array.
  auto byte_size() const { return BYTE_LENGTH; }

  // @brief Returns the amount of bits in use.
  // @note If bit_size() % 8 != 0, remaining bits can still
  // be directly accessable and modifiable, but doing so is
  // undefined under class specifications: please use least_signf_byte()
  // to access the last byte and least_signf_byte_mask() with &-operator
  // to modify.
  auto bit_size() const { return Bits; }

  // @brief Returns the mask used for modifying the least significant byte.
  bit_array_t least_signf_byte_mask() const {
    auto rem = Bits % BITS_IN_BYTE;
    if (rem == 0)
      return 0xFF;
    return 0xFF << (BITS_IN_BYTE - rem);
  }

  // @brief Returns the least significant byte using least_signf_byte_mask()
  // with &-operator.
  bit_array_t least_signf_byte() const {
    return bit_array[BYTE_LENGTH - 1] & least_signf_byte_mask();
  }

private:
  // @brief Left bit shift implementation. Behaviour defined for sl values
  // 1 <= sl <= 8.
  void shift_left_impl(index_t sl) {
    for (index_t i = 0; i < BYTE_LENGTH - 1; ++i) {
      bit_array[i] <<= sl;
      bit_array[i] |= bit_array[i + 1] >> (BITS_IN_BYTE - sl);
    }
    bit_array[BYTE_LENGTH - 1] <<= sl;
  }

  // @brief Right bit shift implementation. Behaviour defined for sr values
  // 1 <= sr <= 8.
  void shift_right_impl(index_t sr) {
    for (index_t i = BYTE_LENGTH - 1; i > 0; --i) {
      bit_array[i] >>= sr;
      bit_array[i] |= bit_array[i - 1] << (BITS_IN_BYTE - sr);
    }
    bit_array[0] >>= sr;
    bit_array[BYTE_LENGTH - 1] &= least_signf_byte_mask();
  }

  // @brief Inverts every bit inside defined bounds.
  void not_me() {
    for (index_t i = 0; i < BYTE_LENGTH - 1; ++i)
      bit_array[i] = ~bit_array[i];
    bit_array[BYTE_LENGTH - 1] =
        (~bit_array[BYTE_LENGTH - 1]) & least_signf_byte_mask();
  }

  void and_with(const this_type &other) {
    uint16_t len = shorter(other);
    for (index_t i = 0; i < len; ++i)
      bit_array[i] &= other.bit_array[i];
  }

  void or_with(const this_type &other) {
    uint16_t len = shorter(other);
    for (index_t i = 0; i < len; ++i)
      bit_array[i] |= other.bit_array[i];
  }

  void xor_with(const this_type &other) {
    uint16_t len = shorter(other);
    for (index_t i = 0; i < len; ++i)
      bit_array[i] ^= other.bit_array[i];
  }

  inline uint16_t shorter(const this_type &other) {
    return (BYTE_LENGTH < other.BYTE_LENGTH ? BYTE_LENGTH : other.BYTE_LENGTH);
  }

private:
  // Storage array.
  bit_array_t bit_array[BYTE_LENGTH];
};

// Turns all LEDs off.
inline void clearLedArray_fast() { PORTD = B00011100; }

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

void loop() { printLedArray_fast(0x3, 1000); }
