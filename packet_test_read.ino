
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(4, 5,6000000); // CE, CSN,SPI transfer speed - this comes from the base clock divider implemented in the RF24 library
const byte addresses [][6] = {"00001", "00002"};    //Setting the two addresses. One for transmitting and one for receiving

//nrf24l01 max packet size is 32 bytes, so create an array of 16 bit values for the test
struct data{
  int16_t readValue[16];
};

data values;
//variables to track time and packets received
unsigned long time1,time2;
float avgTime, avgAccuracy, avgCount, totalCount,dataRate;
int count, totalPackets, toAvg = 10, toAvgCount;
data packets[500];
void setup() {
  //start nrf communication
  radio.begin(); 
  //writing address
  radio.openWritingPipe(addresses[0]);   
  //rcving address  
  radio.openReadingPipe(1, addresses[1]);   
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
  //set module as rcvr
  radio.startListening(); 

  //immediately go into main loop to collect data, store packets to check correctness later
  time2=millis();
  time1 = micros();
}

void loop() {
    //wait for available data and read as soon as possible
    //time1 and micros statements were used to determine the total amount of time taken to read each packet
    //this came out to an average of 187 us per packet
    //time1=micros();
    if (radio.available()){
      radio.read(&values, sizeof(values));  
      packets[count]=values;
      count=count+1;
    }
    //time1 = micros()-time1;
    //Serial.println(time1);
    //if timeout has occurred, or if 500 messages received, calculate error
    if(millis()-time2>5000 || count>499){
      time1=micros()-time1;
      //Serial.println(count);
      //check values
      int checkSum=0;
      for(int i=0;i<500;i++){
        for(int j =0;j<16;j++){
          if(packets[i].readValue[j]==j+1){
            checkSum=checkSum+1;
          }
        }
      }
      //reset packet values for next test
      for(int i=0;i<499;i++){
        for(int j =0;j<16;j++){
          packets[i].readValue[j]=-1;
        }
      }
      //if 10 sets of tests have occurred, print results and reset
      if(toAvgCount<10){
        avgAccuracy = avgAccuracy+checkSum;
        avgTime = avgTime+time1;
        totalCount = totalCount+count; 
        toAvgCount=toAvgCount+1;
      }
      else if (toAvgCount ==10){
        avgAccuracy = avgAccuracy/(totalCount*16)*100;
        avgCount = totalCount/10/500*100;
        avgTime = avgTime/10;
        dataRate = avgCount/100*avgAccuracy/100*500*32*8/(avgTime/1000/1000)/1024;

        Serial.println("10 sets of Packet Tests performed. Results: ");
        Serial.print ("    Average Accuracy %: ");
        Serial.println(avgAccuracy);
        Serial.print("    Average % of messages received: ");
        Serial.println(avgCount);
        Serial.print("    Average time to receive these messages (us): ");
        Serial.println(avgTime);
        Serial.print("    Effective Data Rate (kBPS): ");
        Serial.println(dataRate);
        //reset values
        avgAccuracy = 0;
        avgCount = 0;
        avgTime = 0;
        dataRate = 0;
        totalCount = 0;
        toAvgCount =0;
      }
      //Serial.println(checkSum);
      
      //wait one second after printing results and reset timers
      delay(1000);
      count =0;
      checkSum=0;
      time2=millis();
      time1=micros();
    }
    //time1=micros()-time1;
    //Serial.println(time1);
}
