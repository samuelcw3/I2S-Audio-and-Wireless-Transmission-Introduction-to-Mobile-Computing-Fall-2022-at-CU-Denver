#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

RF24 radio(9, 10); // CE, CSN
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const byte addresses [][6] = {"00001", "00002"};    //Setting the two addresses. One for transmitting and one for receiving
struct data{
  int readValue;
};

data values;
void setup() {
  pinMode(2,OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  
  radio.begin();                            //Starting the radio communication
  radio.openWritingPipe(addresses[0]);      //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[1]);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX);            //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.

  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Testing"));

  display.display();

  radio.startListening();                    //This sets the module as receiver
  delay(1000);
}
void loop() 
{
  delay(5);
  
  if (radio.available())                     //Looking for incoming data
  {
    radio.read(&values, sizeof(values));
    display.clearDisplay();
    display.setTextSize(4);             
    display.setTextColor(SSD1306_WHITE);        
    display.setCursor(0,0);             
    display.print(values.readValue);
    display.print (" ");
    display.setTextSize(2);

    display.display();
    analogWrite(3,map(values.readValue,0,100,0,255));
    digitalWrite(2,values.relay);
    delay(5);

  }
}
