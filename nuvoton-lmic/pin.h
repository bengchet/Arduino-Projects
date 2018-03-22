static const int NUM_DIO = 3;

struct lmic_pinmap {
    uint8_t nss;
    uint8_t rxtx;
    uint8_t rst;
    uint8_t dio[NUM_DIO];
};

// Use this for any unused pins.
const uint8_t LMIC_UNUSED_PIN = 0xff;

// Declared here, to be defined an initialized by the application
extern const lmic_pinmap lmic_pins;
