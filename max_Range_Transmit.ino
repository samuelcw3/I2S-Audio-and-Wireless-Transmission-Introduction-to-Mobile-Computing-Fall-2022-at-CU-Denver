#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(9, 10); // CE, CSN
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

const byte addresses [][6] = {"00001", "00002"};  //Setting the two addresses. One for transmitting and one for receiving
const int potPin = 6;


struct data{
  int readValue;
};

data values;
void setup() {
  pinMode(5,INPUT);
  Serial.begin(115200);
  radio.begin();                           //Starting the radio communication
  radio.openWritingPipe(addresses[1]);     //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[0]);  //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX); //You can set it as minimum or maximum depending on the distance between the transmitter and receiver. 
  radio.setDataRate(RF24_2MBPS);
  radio.setAutoAck(false);
  delay(1000);
  values.readValue = analogRead(potPin);
}
void loop() 
{  
  values.relay = digitalRead(5);
  values.readValue = analogRead(potPin);
  values.readValue = map(values.readValue,0,1023,0,100);


  radio.stopListening();                             //This sets the module as transmitter
  radio.startWrite(&values, sizeof(values),false);
  Serial.println(values.readValue);
  
  delay(500);
}
