/*This code is designed to control a MICE (Motorized Interactive Cat Excersizer)
 * Inputs: RFID sensor, x number of ultrasonic sensors
 * Outputs: Control to 2 motors
 * Author: Milien McDermott
 * Date: 04/16/2019
 */

 //login check
#include <AddicoreRFID.h>
#include <SPI.h>

#define	uchar	unsigned char
#define	uint	unsigned int

uchar fifobytes;
uchar fifoValue;

AddicoreRFID myRFID; // create AddicoreRFID object to control the RFID module

/////////////////////////////////////////////////////////////////////
//set the pins
/////////////////////////////////////////////////////////////////////
const int chipSelectPin = 10;
const int NRSTPD = 5;

//Maximum length of the array
#define MAX_LEN 16

void setup() {                
   Serial.begin(9600);                        // RFID reader SOUT pin connected to Serial RX pin at 9600bps 
 
  // start the SPI library:
  SPI.begin();
  
  pinMode(chipSelectPin,OUTPUT);              // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin 
    digitalWrite(chipSelectPin, LOW);         // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);                     // Set digital pin 10 , Not Reset and Power-down
    digitalWrite(NRSTPD, HIGH);

  myRFID.AddicoreRFID_Init();  
}

void loop()
{
  	uchar i, tmp, checksum1;
	uchar status;
        uchar str[MAX_LEN];
        uchar RC_size;
        uchar blockAddr;	//Selection operation block address 0 to 63
        String mynum = "";

        str[1] = 0x4400;
	//Find tags, return tag type
	status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);	
	if (status == MI_OK)
	{
          Serial.println("RFID tag detected");
          Serial.print("Tag Type:\t\t");
          uint tagType = str[0] << 8;
          tagType = tagType + str[1];
          switch (tagType) {
            case 0x4400:
              Serial.println("Mifare UltraLight");
              break;
            case 0x400:
              Serial.println("Mifare One (S50)");
              break;
            case 0x200:
              Serial.println("Mifare One (S70)");
              break;
            case 0x800:
              Serial.println("Mifare Pro (X)");
              break;
            case 0x4403:
              Serial.println("Mifare DESFire");
              break;
            default:
              Serial.println("Unknown");
              break;
          }
	}

	//Anti-collision, return tag serial number 4 bytes
	status = myRFID.AddicoreRFID_Anticoll(str);
	if (status == MI_OK)
	{
          checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
          Serial.print("The tag's number is:\t");
    	    Serial.print(str[0]);
            Serial.print(" , ");
    	    Serial.print(str[1]);
            Serial.print(" , ");
    	    Serial.print(str[2]);
            Serial.print(" , ");
    	    Serial.println(str[3]);

          Serial.print("Read Checksum:\t\t");
    	      Serial.println(str[4]);
          Serial.print("Calculated Checksum:\t");
            Serial.println(checksum1);
            
            // Should really check all pairs, but for now we'll just use the first
            if(str[0] == 197)                      //You can change this to the first byte of your tag by finding the card's ID through the Serial Monitor
            {
                Serial.println("\nHello Craig!\n");
            } else if(str[0] == 244) {             //You can change this to the first byte of your tag by finding the card's ID through the Serial Monitor
                Serial.println("\nHello Erin!\n");
            }
            Serial.println();
            delay(1000);
	}
		
        myRFID.AddicoreRFID_Halt();		   //Command tag into hibernation              

}
