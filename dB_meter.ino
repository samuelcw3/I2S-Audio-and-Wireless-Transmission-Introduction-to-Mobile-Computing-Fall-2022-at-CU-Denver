

#include <I2S.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//timing variables
unsigned long time1,time2;
//value for average
long avgMinMax;
int avgCount;
//nrf24l01 max packet size is 32 bytes, so create an array of 16 bit values for the test
struct data{
  int16_t readValue[16];
};

void setup() {
  //start i2c display
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Mode: dB Meter"));
  display.display();
  
  //start serial port for debugging
  Serial.begin(115200);
  //while (!Serial) {
   // ; // wait for serial port to connect. Needed for native USB port only
  //}

  //i2s @ 16khZ SAMPLING with 16 bits per sample
  if (!I2S.begin(I2S_PHILIPS_MODE, 16000, 16)) {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }
  time1 = millis();
}





void loop() {
  time2 =micros();
  data values;
  //collect 1 dataframe worth of audio samples
  for (int i=0; i<16; i++) {
    int sample = 0; 
    while ((sample == 0) || (sample == -1) ) {
      sample = I2S.read();
    }
    values.readValue[i] = sample;
  }
  // get the average of the df
  float avg = 0;
  for (int i=0; i<16; i++) {
    avg = avg+values.readValue[i];
  }
  avg =avg/16;

  //normalize the output by subtracting the mean from samples
  for (int i=0; i<16; i++) {
    values.readValue[i] = values.readValue[i]-avg;
  }

  // find the max pkpk value in the df
  int maxSample, minSample;
  minSample = 100000;
  maxSample = -100000;
  for (int i=0; i<16; i++) {
    maxSample = max(maxSample, values.readValue[i]);
    minSample = min(minSample, values.readValue[i]);
  }
  long minMax = maxSample-minSample;
  time2=micros()-time2;
  //Serial.println(time2);
  //take the avg of a couple frames to even out the noise
  if (avgCount<10){
    avgMinMax = avgMinMax+minMax;
    avgCount = avgCount+1;
  }
  else{
    avgMinMax = avgMinMax/10;
    Serial.println(avgMinMax);
    //convert to dB, initially calibrated at minMax val of 3 = 35dBa
    float dB;
    //check if minMax>0 to ignore erronious readings
    if(minMax>0){
      dB = 20*log10(minMax/3)+35;
    }
    //only update display every 200ms so it's easier to read
    if (millis()-time1>200 ){
      display.clearDisplay();
      display.setCursor(0,0);             
      display.println(F("Mode: dB Meter"));
      display.setCursor(0,10);             
      display.setTextSize(1);             
      display.setTextColor(SSD1306_WHITE);        
      display.print(dB);
      display.print(" dB");
      display.display();
      time1 = millis();
    }
    avgCount=0;
    avgMinMax=0;
  }
  
}
