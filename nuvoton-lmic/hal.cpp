/*******************************************************************************
   Copyright (c) 2015 Matthijs Kooijman
   All rights reserved. This program and the accompanying materials
   are made available under the terms of the Eclipse Public License v1.0
   which accompanies this distribution, and is available at
   http://www.eclipse.org/legal/epl-v10.html

   This the HAL to run LMIC on top of the Arduino environment.
 *******************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "pin.h"
#include "lmic.h"
#include "hal.h"
#include "config.h"
#include <stdio.h>

// -----------------------------------------------------------------------------
// I/O

static bool dio_states[NUM_DIO] = {0};

static void hal_io_init () {
  // NSS and DIO0 are required, DIO1 is required for LoRa, DIO2 for FSK
  ASSERT(lmic_pins.nss != LMIC_UNUSED_PIN);
  ASSERT(lmic_pins.dio[0] != LMIC_UNUSED_PIN);
  ASSERT(lmic_pins.dio[1] != LMIC_UNUSED_PIN || lmic_pins.dio[2] != LMIC_UNUSED_PIN);

  pinMode(lmic_pins.nss, OUTPUT);

  if (lmic_pins.rxtx != LMIC_UNUSED_PIN)
    pinMode(lmic_pins.rxtx, OUTPUT);
  if (lmic_pins.rst != LMIC_UNUSED_PIN)
    pinMode(lmic_pins.rst, OUTPUT);

  pinMode(lmic_pins.dio[0], INPUT);
  if (lmic_pins.dio[1] != LMIC_UNUSED_PIN)
    pinMode(lmic_pins.dio[1], INPUT);
  if (lmic_pins.dio[2] != LMIC_UNUSED_PIN)
    pinMode(lmic_pins.dio[2], INPUT);
}

// val == 1  => tx 1
void hal_pin_rxtx (u1_t val) {
  if (lmic_pins.rxtx != LMIC_UNUSED_PIN)
    digitalWrite(lmic_pins.rxtx, val);
}

// set radio RST pin to given value (or keep floating!)
void hal_pin_rst (u1_t val) {
  if (lmic_pins.rst == LMIC_UNUSED_PIN)
    return;

  if (val == 0 || val == 1) { // drive pin
    pinMode(lmic_pins.rst, OUTPUT);
    digitalWrite(lmic_pins.rst, val);
  } else { // keep pin floating
    pinMode(lmic_pins.rst, INPUT);
  }
}


static void hal_io_check() {
  uint8_t i;
    for (i = 0; i < NUM_DIO; ++i) {
      if (lmic_pins.dio[i] == LMIC_UNUSED_PIN)
          continue;
      if (dio_states[i] != digitalRead(lmic_pins.dio[i])) {
          dio_states[i] = !dio_states[i];
          if (dio_states[i]){
              radio_irq_handler(i);
       }
      }
    }
}

// -----------------------------------------------------------------------------
// SPI

//static const SPISettings settings(10E6, MSBFIRST, SPI_MODE0);

static void hal_spi_init () {
  SPI.begin();
}

void hal_pin_nss (u1_t val) {
  digitalWrite(lmic_pins.nss, val);
}

// perform SPI transaction with radio
u1_t hal_spi (u1_t out) {
  u1_t res = SPI.transfer(out);
  return res;
}

// -----------------------------------------------------------------------------
// TIME

static struct {
    int irqlevel;
    u4_t ticks;
} HAL;

static void tim4_handler(){
  //overflow, so increase 1 tick
  HAL.ticks++;
}

static void hal_time_init () {
  // Nothing to do
  // Set ticks for US_PER_OSTICK
  Timer4.initialize(US_PER_OSTICK);
  Timer4.attachInterrupt(tim4_handler);
}

u4_t hal_ticks () {
  
  hal_disableIRQs();
  uint32_t t = HAL.ticks;
  hal_enableIRQs();
 
  return t;
}

// Returns the number of ticks until time. Negative values indicate that
// time has already passed.
static u2_t delta_time(u4_t time) {

  u4_t t = hal_ticks();
  s4_t d = time - t;
  if(d <= 0)
    return 0; //in the past
  else if((d>>16)!=0)
    return 0xffff;
  else
    return (u2_t)d;
}

void hal_waitUntil (u4_t time) {
  while ( delta_time(time) != 0 ); // busy wait until timestamp is reached
}

// check and rewind for target time
u1_t hal_checkTimer (u4_t time) {
  return delta_time(time) <= 5;
}

void hal_disableIRQs () {
  __disable_irq();
  //++HAL.irqlevel;
}

void hal_enableIRQs () {
  //if (--HAL.irqlevel <= 0) {
    __enable_irq();
    hal_io_check();
  //}
}

void hal_sleep () {
  // Not implemented
}

// -----------------------------------------------------------------------------

#if defined(LMIC_PRINTF_TO)
// avr-libc provides an alternative (simpler) way to override STDOUT
static int uart_putchar (char c, FILE *)
{
  LMIC_PRINTF_TO.write(c) ;
  return 0 ;
}

void hal_printf_init() {
  // create a FILE structure to reference our UART output function
  static FILE uartout;
  memset(&uartout, 0, sizeof(uartout));

  // fill in the UART file descriptor with pointer to writer.
#if !defined(__NUVOTON__)
  fdev_setup_stream (&uartout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
#endif
  // The uart is the standard output device STDOUT.
  stdout = &uartout ;
}
#endif // defined(LMIC_PRINTF_TO)

void hal_init () {
  // configure radio I/O and interrupt handler
  hal_io_init();
  // configure radio SPI
  hal_spi_init();
  // configure timer and interrupt handler
  hal_time_init();

#if defined(LMIC_PRINTF_TO)
  // printf support
  hal_printf_init();
#endif
  
}

void hal_failed (const char *file, u2_t line) {
#if defined(LMIC_FAILURE_TO)
  LMIC_FAILURE_TO.println("FAILURE ");
  LMIC_FAILURE_TO.print(file);
  LMIC_FAILURE_TO.print(':');
  LMIC_FAILURE_TO.println(line);
  LMIC_FAILURE_TO.flush();
#endif
  hal_disableIRQs();
  while (1);
}
