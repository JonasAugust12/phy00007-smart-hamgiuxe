// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 Your Name

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  pin_t pin_vcc;         // Power supply pin
  pin_t pin_gnd;         // Ground pin
  pin_t pin_out;         // Digital output pin
  timer_t timer;         // Timer for sensor updates
  double distance;       // Current distance to obstacle in cm
  uint32_t distance_attr; // Use an attribute ID directly instead of attr_t type
} chip_state_t;

void chip_timer_callback(void *user_data) {
  chip_state_t *chip = (chip_state_t*)user_data;
  
  // Only process if VCC is high
  if (pin_read(chip->pin_vcc)) {
    // Get simulated distance from attribute
    chip->distance = (double)attr_read(chip->distance_attr);
    
    // Output HIGH (1) if obstacle is within 100cm
    bool obstacle_detected = (chip->distance <= 50.0);
    pin_write(chip->pin_out, obstacle_detected ? 1 : 0);
  } else {
    // If no power, set output low
    pin_write(chip->pin_out, 0);
  }
}

void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  // Initialize pins
  chip->pin_vcc = pin_init("VCC", INPUT);
  chip->pin_gnd = pin_init("GND", INPUT);
  chip->pin_out = pin_init("OUT", OUTPUT);
  
  // Initialize distance attribute directly with ID reference
  chip->distance_attr = attr_init("distance", 1000.0); // Default distance without obstacle
  
  // Set initial state - no obstacle
  pin_write(chip->pin_out, 0);
  
  // Create timer configuration
  timer_config_t timer_config = {
    .callback = chip_timer_callback,
    .user_data = chip
  };
  
  // Initialize and start timer
  chip->timer = timer_init(&timer_config);
  timer_start(chip->timer, 100000, true); // 100ms interval, repeat mode
  
  printf("MD0370 IR Obstacle Sensor initialized!\n");
}
