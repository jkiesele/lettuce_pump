#include <Arduino.h>

// Constants for pump control
const unsigned long pump_seconds = 20; // Pump runs for 2 seconds; 20 best value
const unsigned long pump_interval_seconds = 1800; // Interval between pumps, 10 seconds for testing; 60*60 best value
const unsigned long wait_between_cycles_hours = 24; // Wait between cycles, 24 hours
int pump_count_per_cycle = 5; // Number of times the pump should run per cycle

/*
* The pump should run pump_count_per_cycle times for pump_seconds and wait between each pumping pump_interval_seconds.
* After pump_count_per_cycle times, the pump should wait for wait_between_cycles_hours before starting the next cycle.
*/

// Function to delay by seconds
void delay_seconds(unsigned long seconds) {
  for (unsigned long i = 0; i < seconds; i++) {
    delay(1000);
  }
}


// Function to turn on the pump
void turn_on_pump() {
  PORTB |= (1 << PB3); // Set PB3 to high
}

// Function to turn off the pump
void turn_off_pump() {
  PORTB &= ~(1 << PB3); // Set PB3 to low
}

// Function to check if there is enough water
int enough_water() {
  return !(PINB & (1 << PB4)); // Check if PB4 is high
}

// Function to control the pump
void pump() {
  // Turn on the pump and check water level every second
  unsigned long start = millis();
  turn_on_pump();
  while (millis() - start < pump_seconds * 1000) {
    if (!enough_water()) {
      turn_off_pump();
      return;
    }
  }
  turn_off_pump(); // Ensure the pump is turned off after the duration
}

void setup() {
  // Set PB4 as input with pull-up resistor
  DDRB &= ~(1 << PB4);
  PORTB |= (1 << PB4);

  // Set PB3 as output
  DDRB |= (1 << PB3);

  // Startup alive signal
  turn_on_pump();
  delay(1000);
  turn_off_pump();
  delay(1000);
}

void loop() {
  static unsigned long last_pump = 0;
  static unsigned long last_cycle = 0;
  static int pump_count = 0;

  bool time_to_pump = millis() - last_pump > pump_interval_seconds * 1000 || last_pump == 0;
  bool time_to_cycle = millis() - last_cycle > wait_between_cycles_hours * 60 * 60 * 1000 || last_cycle == 0;

  // If cycle time is reached, reset pump_count and last_cycle
  if (time_to_cycle) {
    pump_count = 0; // Reset pump count, so that the pump can start again
    time_to_pump = true; // Set time_to_pump to true, so that the pump can start immediately
    last_cycle = millis();
  }

  // Check if it is time to pump
  if (time_to_pump && pump_count < pump_count_per_cycle) {
    pump();
    last_pump = millis();
    pump_count++; // Increment pump count, this will turn off the pump after pump_count_per_cycle times
  }

}



/*
scratch
static unsigned long last_pump = 0;
  bool time_to_pump = millis() - last_pump > pump_interval_seconds * 1000 || last_pump == 0;

  // Check if it is time to pump
  if (enough_water() && time_to_pump) {
    pump();
    last_pump = millis();
  }

  // If not enough water, reset last_pump and wait
  if (!enough_water()) {
    last_pump = 0;
    delay_seconds(60); // Wait a minute
  }

*/