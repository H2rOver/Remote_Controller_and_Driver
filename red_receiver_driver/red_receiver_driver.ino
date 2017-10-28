/*  red_receiver_driver.ino
 *  created by: Scott Timpe on 10/27/2017
 *  last edited: 10/27/2017 by Scott Timpe
 *  
 *  Creation notes:
 *    Extracted relevant code from xbee packet receiver developed by Scott Timpe, Jonathan Midolo,
 *    Nestor Ordonez, and Daniel Benusovich, using Andrew Rapp's xbee library and example code.
 *    https://github.com/andrewrapp/xbee-arduino
 *    Combined this with MotorControl functions developed by Garen <lastname> and packaged by 
 *    Daniel Benusovich.
 *    
 *    
 *  Edit/Update notes:
 *    ...
 *  
 *  Program Description:
 *    This is a simple program to receive drive speed values from a remote controler using an Xbee.
 *    Red will receive a Zigbee packet that contains a byte value of the speed in which to drive the motors.
 *    Red will then drive forward at this speed and adjust as new speed values come in.
 *    Furtheremore, this program implements a simple navigatioin routine. When the feelers
 *  
 *  
 *  Connections:
 *  
 *    Left Feeler to Arduino pin 12
 *    Right Feeler to Arduion pin 13
 *    
 *    Xbee +5V to Arduino 5V
 *    Xbee GND to Arduino/Shared ground
 *    Xbee Tx to Arduino Rx
 *    Xbee Rx to Arduino Tx
 *    
 *    Various other pin connections for motor controllers. (See MotorControl.cpp & PinDeclarations.h)
 */

#include <XBee.h>
#include <SoftwareSerial.h>
#include <MotorControl.h>
#include <PinDeclarations.h>

MotorControl Red = MotorControl();

int feeler_contact_L = 0;
int feeler_contact_R = 0;

int driverSpeed = 0;

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

// Define NewSoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 9;
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
SoftwareSerial nss(ssRX, ssTX);

void setup() {
  Serial.print("begin routine");
  // set all the motor control pins to outputs  
  pinMode(LEFT_FEELER, INPUT_PULLUP);
  pinMode(RIGHT_FEELER, INPUT_PULLUP);

  // start serial
  Serial.begin(9600);
  xbee.setSerial(Serial);
  nss.begin(9600);

  Red.motorForward(driverSpeed);
}

void loop() {

/*** begin xbee code ***/
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
           
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet
        
        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx);
        Serial.println("got an rx packet");
            
        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
            // the sender got an ACK
            Serial.println("packet acknowledged");
        } else {
          Serial.println("packet not acknowledged");
        }
        
        Serial.print("checksum is ");
        Serial.println(rx.getChecksum(), HEX);

        Serial.print("packet length is ");
        Serial.println(rx.getPacketLength(), DEC);
        
         for (int i = 0; i < rx.getDataLength(); i++) {
          driverSpeed = rx.getData()[i];
          Serial.print("payload [");
          Serial.print(i, DEC);
          Serial.print("] is ");
          Serial.println(driverSpeed);
        }
        
       for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
        Serial.print("frame data [");
        Serial.print(i, DEC);
        Serial.print("] is ");
        Serial.println(xbee.getResponse().getFrameData()[i], HEX);
      }
      }
    } else if (xbee.getResponse().isError()) {
      Serial.print("error code:");
      Serial.println(xbee.getResponse().getErrorCode());
    }
/*** end xbee code ***/

  //active low inputs
  feeler_contact_L = not(digitalRead(LEFT_FEELER));
  feeler_contact_R = not(digitalRead(RIGHT_FEELER));

  //primitive, automated naviagation routine when feelers make contact.
  if(feeler_contact_L || feeler_contact_R)
  {
    Red.motorOff();
    delay(100);
    Red.motorBackward(128);
    delay(2000);
    Red.motorLeft(255);
    delay(1300);
    Red.motorForward(255);
    delay(2000);
    Red.motorRight(255);
    delay(1000);
   }
  else
  {
    //drive forward at value received in xbee packet
    Red.motorForward(driverSpeed);
  }
}
