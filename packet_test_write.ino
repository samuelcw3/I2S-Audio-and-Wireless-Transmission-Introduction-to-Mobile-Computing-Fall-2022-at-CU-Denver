#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(4, 5,6000000); // CE, CSN, SPI - note this speed is determined by clock divisor in RF24 library (defaults to 8, so 48MHz/8=6Mbps)
const byte addresses [][6] = {"00001", "00002"};    //Setting the two addresses. One for transmitting and one for receiving

//nrf24l01 max packet size is 32 bytes, so create an array of 16 bit values for the test
struct data{
  int16_t readValue[16];
};

data values;
//variables to track time
unsigned long time1,time2;
void setup() {
  //start nrf communication
  radio.begin(); 
  //writing address                       
  radio.openWritingPipe(addresses[1]);
  //rcving address
  radio.openReadingPipe(1, addresses[0]); 
  //set to max power to get best chance at good data transfer
  radio.setPALevel(RF24_PA_MAX);          
  //set to max data rate
  radio.setDataRate(RF24_2MBPS);
  //turn off auto acknowledge. This slows down things significantly if on
  radio.setAutoAck(false);

  //begin serial communication. Only used to print final result
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  //set module as transmitter
  radio.stopListening();

  Serial.println("test starting");
  //initialize data values, note avoid using zero as this is the
  for (int i = 0;i<16;i++){
    values.readValue[i]=i+1;
  }
  //test print data values
  for (int i = 0;i<16;i++){
    Serial.print(values.readValue[i]);
  }
  //Serial.println("starting test in 10 seconds");
  //delay(10000);
}

void loop() {
    //start timing
    //time1=micros();
    radio.startWrite(&values, sizeof(values),false); 
    //time1=micros()-time1;
    //Serial.println(time1);


}
