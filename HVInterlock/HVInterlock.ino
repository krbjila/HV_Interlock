#include <EEPROM.h>

// Pins
#define POWER 2
#define ENABLE 0
#define VOLTAGE 15

// Configuration constants
#define POWER_BYTE 0 		  // EEPROM location for shutoff state 
#define ENABLE_BYTE 1     // EEPROM location for enable state
#define BAUD_RATE 115200 	// USB serial baud rate
#define BUFFER_SIZE 100		// Size of serial buffer

// Serial buffer
char buffer[BUFFER_SIZE];
int buf_i = 0;

double v_input = 0;

double get_voltage() {
  return -9.99937 + 20021.1 * analogRead(VOLTAGE);
}

void set_HV_enable(bool enable) {
  digitalWrite(ENABLE, !enable);
  EEPROM.write(ENABLE_BYTE, enable);
}

void set_HV_power(bool enable) {
  digitalWrite(POWER, !enable);
  EEPROM.write(POWER_BYTE, enable);
}

void setup() {
  analogReference(EXTERNAL); // Use 2.048 bit reference for ADC
  analogReadAveraging(10); // Average 10 ADC readings
  analogReadResolution(12); // Read 12 ADC bits

  pinMode(POWER, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(VOLTAGE, INPUT);

  set_HV_enable(EEPROM.read(ENABLE_BYTE));
  set_HV_power(EEPROM.read(POWER_BYTE));
}


void loop() {
	// Updates the temperature, PID, and output.
	v_input = get_voltage();

	// If input is available on the serial port
	while(Serial.available()) {
		// Handles buffer overflow by clearing buffer if too full
		if(buf_i > BUFFER_SIZE - 3) {
			buf_i = 0;
			continue;
		}
		// Otherwise, writes next character to buffer and parse the
		// resulting string on a newline.
		if((buffer[buf_i++] = Serial.read()) == '\n') {
			buffer[buf_i++] = '\0';
			switch(buffer[0]) {
				case 'v': // send current voltage over serial
					Serial.println(v_input);
					break;
				case 'E': // enable HV outputs
					set_HV_enable(true);
					break;
				case 'e': // disable HV outputs
					set_HV_enable(false);
					break;
        case 'O': // turn on HV power
					set_HV_power(true);
					break;
        case 'o': // turn off HV power
					set_HV_power(false);
					break;
			}
			buf_i = 0;
		}
	}
}
