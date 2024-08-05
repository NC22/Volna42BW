#ifndef CatFeed_h
#define CatFeed_h

#include <Arduino.h>

const static uint8_t cat_feed_125x127bw[] PROGMEM = {
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x0, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x1F, 0xFE, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFE, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xFF, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xFF, 0xFF, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0xFF, 0xFF, 0xFE, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFC, 0xFF, 0xFF, 0xFC, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xE7, 0xFF, 0xFF, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0x1F, 0xFF, 0x8F, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xF0, 0xFF, 0xF8, 0x7E, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0x83, 0xFF, 0x3, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xFF, 0xF8, 0x1F, 0xF8, 0x1F, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0x0, 0x1F, 0x0, 0xFE, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xE0, 0x0, 0x78, 0x7, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xFE, 0x0, 0x1, 0x80, 0x3F, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xC0, 0x0, 0x0, 0x1, 0xFE, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFC, 0x0, 0x0, 0x0, 0xF, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xC0, 0x0, 0x0, 0x0, 0x7F, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7C, 0x0, 0x0, 0x0, 0x3, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xE0, 0x0, 0x0, 0x0, 0x1F, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0x80, 0x0, 0x0, 0x0, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFE, 0x0, 0x0, 0x0, 0x7, 0xFC, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xF8, 0x0, 0x0, 0x20, 0x3F, 0xE3, 0xFE, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xF0, 0x0, 0x7, 0x81, 0xFF, 0x1F, 0xF0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x3F, 0xFC, 0xFF, 0xE0, 0x0, 0x7E, 0xF, 0xF8, 0xFF, 0x80, 0x0, 0x7F, 0xFF, 0xFF, 0xC0, 0x1, 0xFF, 0xE7, 0xFF, 0xE0, 0x7, 0xFC, 0x7F, 0xC7, 0xFC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0xFF, 0x3F, 0xFF, 0xE0, 0x7F, 0xF3, 0xFE, 0x3F, 0xE0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7F, 0xF8, 0xFF, 0xFF, 0xC3, 0xFF, 0xFF, 0xF1, 0xFF, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0xFF, 0xC7, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0x8F, 0xFC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1F, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xF0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xFF, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x3F, 0xFE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3F, 0xFF, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xFF, 0xF0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xFF, 0xFC, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xC0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0xFE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xE1, 0x83, 0xFF, 0xFF, 0xFF, 0xFF, 0x8F, 0xFF, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xFC, 0x6, 0xF, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xC0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3F, 0xC7, 0x98, 0xF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xFE, 0x7E, 0x70, 0x1F, 0xFF, 0xFF, 0xF8, 0x7F, 0xFF, 0xFC, 0x3, 0xC0, 0x0, 0x0, 0x0, 0xF, 0xE3, 0xF3, 0xC0, 0x7F, 0xFF, 0xFF, 0x7, 0xFF, 0xFF, 0xE0, 0x31, 0x0, 0x1, 0xF0, 0x0, 0x7F, 0x3F, 0x9F, 0x81, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0x3, 0x4, 0x0, 0x18, 0xC0, 0x3, 0xF8, 0xFC, 0xFF, 0x81, 0xFF, 0xF0, 0xF, 0xFF, 0xFF, 0xF8, 0x10, 0x10, 0x1, 0x83, 0x0, 0x1F, 0xE7, 0xC7, 0xFF, 0x80, 0x0, 0x1, 0xFF, 0xFF, 0xFF, 0x80, 0x80, 0x80, 0x8, 0x8, 0x1, 0xFF, 0xC, 0x7F, 0xFF, 0x0, 0x0, 0x7F, 0xFF, 0xFF, 0xFC, 0x4, 0x4, 0x0, 0x40, 0x40, 0x7, 0xFC, 0x3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x30, 0x60, 0x2, 0x2, 0x0, 0x3F, 0xF8, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x0, 0xC6, 0x0, 0x18, 0x30, 0x3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x1, 0xE0, 0x0, 0x63, 0x0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x0, 0x0, 0x1, 0xF0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xC0, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3F, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x3C, 0x0, 0x1, 0x80, 0x0, 0x0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x3F, 0xF0, 0x0, 0x12, 0x0, 0x0, 0x3E, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0xC0, 0x0, 0x0, 0x0, 0x3, 0xFF, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0x80, 0x0, 0x0, 0x0, 0x1F, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xFE, 0x0, 0x0, 0x0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xF0, 0x0, 0x0, 0x0, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFF, 0x80, 0x0, 0x0, 0x0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xF8, 0x0, 0x0, 0x0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x0, 0x0, 0x0, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x0, 0x0, 0x0, 0x0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x0, 0x0, 0x0, 0x3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x0, 0x0, 0x0, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x0, 0x0, 0x0, 0x0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x0, 0x0, 0x0, 0x0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x0, 0x0, 0x0, 0x0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x2, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x0, 0x0, 0x0, 0x0, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x0, 0x0, 0x0, 0x0, 0x0, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3E, 0x0, 0x0, 0x0, 0x7, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x8, 0x0, 0x0, 0x0, 0x7C, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x60, 0x20, 0x0, 0x0, 0x3, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC4, 0x1, 0x0, 0x0, 0x0, 0xF, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x40, 0x8, 0x0, 0x0, 0x0, 0x7F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x0, 0x40, 0x0, 0x0, 0x1, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x2, 0x0, 0x0, 0x0, 0xF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x0, 0x30, 0x0, 0x0, 0x0, 0x3F, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x1, 0x0, 0x0, 0x0, 0x1, 0xFC, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x0, 0x10, 0x0, 0x0, 0x0, 0x7, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x3F, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x0, 0x10, 0x0, 0x0, 0x0, 0x1, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x7, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0, 0x3F, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x3F, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x1, 0xF8, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1E, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x0, 0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x0, 0x0, 0x0, 0x80, 0x0, 0x40, 0x0, 0x0, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x1, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x8, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x40, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x8, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x40, 0x0, 0x0, 0x4, 0x0, 0x0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x8, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x10, 0x0, 0x0, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x0, 0x0, 0x0, 0x8, 0x0, 0x0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x0, 0x1, 0x0, 0x10, 0x80, 0x0, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x7, 0x80, 0x88, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0, 0x0, 0x3, 0xFF, 0x80, 0x0, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9C, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x1E, 0x0, 0x0, 0x0, 0x0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0  };


imageData cat_feed_125x127bw_settings PROGMEM = {
       (uint8_t *)  cat_feed_125x127bw,
       1985,
       125,
       127,
};


#endif


