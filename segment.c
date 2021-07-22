/*
 * File:   segment.c
 * Author: Fernando Sandoval
 *
 * Created on July 21, 2021, 6:44 PM
 */


#include <xc.h>
#include <stdint.h>

uint8_t T_TABLE[] = {
    0b00111111, //0
    0b00000110, //1
    0b01011011, //2
    0b01001111, //3
    0b01100110, //4
    0b01101101, //5
    0b01111101, //6
    0b00000111, //7
    0b01111111, //8
    0b01101111, //9
    0b01110111, //A
    0b01111100, //B
    0b00111001, //C
    0b01011110, //D
    0b01111001, //E
    0b01110001  //F
};

uint8_t SEG_HEX(uint8_t VAR) {
    uint8_t FER = T_TABLE [VAR];
    return FER;
}
