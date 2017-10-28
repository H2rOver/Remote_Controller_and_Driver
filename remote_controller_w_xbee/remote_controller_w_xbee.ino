/*  remote_controller_w_xbee.ino
 *  created by: Scott Timpe on 10/27/2017
 *  last edited: 10/27/2017 by Scott Timpe
 *  
 *  Creation notes:
 *    Extracted relevant code from xbee packet sender developed by Scott Timpe, Jonathan Midolo,
 *    Nestor Ordonez, and Daniel Benusovich, using Andrew Rapp's xbee library and example code.
 *    https://github.com/andrewrapp/xbee-arduino
 *    
 *  Edit/Update notes:
 *    ...
 *  
 *  Program Description:
 *    This is a simple program to remotely control the drive speed of Red using a potentiometer.
 *  The control arduino reads an analog value from pin A0, converts this reading to byte size,
 *  and transmits the data to Red in a Zigbee packet using an Xbee.
 *  
 *  
 *  Connections:
 *  
 *    Potentiometer left pin to ground
 *    Potentiometer right pin to ground
 *    Potentiometer middle pin to Arduion A0
 *    
 *    Xbee +5V to Arduino 5V
 *    Xbee GND to Arduino/Shared ground
 *    Xbee Tx to Arduino Rx
 *    Xbee Rx to Arduino Tx
 */

#include <XBee.h> // import XBee library

XBee xbee = XBee(); // provides functions for sending and receiving packets
uint8_t payload[] = { '0' }; // array for transmit data ...research ROS X,Y,Z, thing...?

const int POTENTIOMETER = A0;   //analog pin used to read potentiometer

int potentioValue;              //value read

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  xbee.setSerial(Serial);
}

void loop() {
  
  potentioValue = analogRead(POTENTIOMETER)/4;    //to reduce 0-1023 resolution to byte sized 0-255
  payload[0] = potentioValue;
  
  // put your main code here, to run repeatedly:
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a2004154ec07);
  //XBeeAddress64 addr64 = XBeeAddress64(0x0003a200, 0x4154ec07); // note: 0010 works and 0003 works but 0013 doesnot work and changes to 00 7d 33 (fixed with API 2)!
  ZBTxRequest tx = ZBTxRequest(0x0013a2004154ec07, payload, sizeof(payload)); // 64-bit addressing, packet, and packet length
  xbee.send(tx); // send packet to remote radio
}
