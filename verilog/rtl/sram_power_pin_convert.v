// Power pin conversion for SRAM simulation
// Converts USE_POWER_PINS to USE_PG_PIN for SRAM IP

`timescale 1ns / 1ps

`ifdef USE_POWER_PINS
    `define USE_PG_PIN
`endif
