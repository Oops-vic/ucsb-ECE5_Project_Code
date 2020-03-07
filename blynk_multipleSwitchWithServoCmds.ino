#define BLYNK_USE_DIRECT_CONNECT
#define BLYNK_PRINT Serial

#include <BlynkSimpleSerialBLE.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <SPI.h>

char auth[] = "B3PQzPq4wDHWaeukUqdJ5xx7GtTe8wy8";
/*************BLE Settings********************/
#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4    
#define BLUEFRUIT_VERBOSE_MODE         true
#define BLYNK_RED       "#D3435C"
#define BLYNK_GREEN     "#23C48E"
/***************Switch Settings***************/
#define MAXNUM 4
#define onAngle 180                 //Change to the on angle (0-180)
#define offAngle 0                  //Change to the off angle (0-180)

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

unsigned short int switchStatus[MAXNUM], switchLen = 0;
bool changeVar[MAXNUM], numIndicator = false;
const short int dataPin = 9; 
const short int clockPin = 5; 
const short int latchPin = 6;

void error(const);                            //Give out error messages
void initialButtonColor();                    //Initialization process
bool b_sum(bool* b_array, short int len);     //Determine if switch state is changed
void onServoShift();                          //On commands for all attachments
void offServoShift();                         //Off commands for all attachments
int power(int, int);                          //Calculate the power with return value int

BLYNK_WRITE(V0){     //Set the number of switches (4 max)
  if (switchLen != param.asInt() - 1){
    numIndicator = true;
    switchLen = param.asInt() - 1;
  }
}
BLYNK_WRITE(V1){     //State of switch 1
  if (switchStatus[0] != param.asInt()){
    changeVar[0] = true;
    switchStatus[0] = param.asInt();
  }
}
BLYNK_WRITE(V2){     //State of switch 2
  if (switchStatus[1] != param.asInt()){
    changeVar[1] = true;
    switchStatus[1] = param.asInt();
  }
}
BLYNK_WRITE(V3){     //State of switch 3
  if (switchStatus[2] != param.asInt()){
    changeVar[2] = true;
    switchStatus[2] = param.asInt();
  }
}
BLYNK_WRITE(V4){     //State of switch 4
  if (switchStatus[3] != param.asInt()){
    changeVar[3] = true;
    switchStatus[3] = param.asInt();
  }
}

void setup() { 
/*********************************Setup part&Initialization***************************************/  
  while(!Serial){
    delay(500);
  }
  Serial.begin(9600);
  delay(100);
  if (!ble.begin(BLUEFRUIT_VERBOSE_MODE)){
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
  }
  ble.waitForOK();
  ble.setMode(BLUEFRUIT_MODE_DATA);
  Serial.println("Waiting for connections...");
  Blynk.config(ble,auth);
  while (!Blynk.connect()){
    delay(10);
  }
  initialButtonColor();
  pinMode(latchPin, OUTPUT); 
  pinMode(dataPin, OUTPUT); 
  pinMode(clockPin, OUTPUT);
}

void loop() {                         //switchStatus contains elements of on/off. 1--on, 2--off
  Blynk.run();
  if (!b_sum(changeVar, MAXNUM) && !numIndicator){
    delay(10);
    return;
  }
  for (int i = 1; i <= switchLen; i++){
    Blynk.setProperty(i, "color", BLYNK_GREEN);
  }
  for (int i = switchLen + 1; i <= MAXNUM; i++){
    Blynk.setProperty(i, "color", BLYNK_RED);
    Blynk.virtualWrite(i,2);
    Blynk.syncVirtual(i);
    switchStatus[i - 1] = 2;
  }
/****Control servo motor with instructions of switchStatus[]****/
  for (int i = 0; i < switchLen; i++){
    Serial.println(switchStatus[i]);
  }
  Serial.println("***********");
  Serial.println(switchLen);
  Serial.println("/*********/");
  onServoShift();
  offServoShift();
/*****************Re-initialize bool indicators*****************/
  numIndicator = false;
  for (int i = 0; i < MAXNUM; i++){
    changeVar[i] = false;
  }
}

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void initialButtonColor(){
  for (int i = 1; i <= MAXNUM; i++){
    Blynk.setProperty(i, "color", BLYNK_RED);
    Blynk.virtualWrite(i, 2);
    Blynk.syncVirtual(i);
  }
  for (int i = 0; i < MAXNUM; i++){
    changeVar[i] = false;
    switchStatus[i] = 2;
  }
}

bool b_sum(bool* b_array, int len){
  bool result = 0;
  for (int i = 0; i < len; i++){
    result += b_array[i];
  }
  return result;
}

int power(int num, int p){
    if (p == 0) return num;
    return num * power(num, p - 1);
}

void onServoShift(){
  int binaryCmd = 0;
  for (int i = 0; i < switchLen; i++){
      if (switchStatus[i] == 1 && changeVar[i]) binaryCmd += power(2, i);
  }
  for (int i = offAngle; i <= onAngle; i++) { 
    digitalWrite(latchPin ,LOW); 
    shiftOut(dataPin ,clockPin ,MSBFIRST, binaryCmd); 
    digitalWrite(latchPin ,HIGH); 
    delayMicroseconds(map(i, 0, 180, 400, 2500));
    digitalWrite(latchPin ,LOW); 
    shiftOut(dataPin ,clockPin ,MSBFIRST, 0b00000000); 
    digitalWrite(latchPin ,HIGH);
    delayMicroseconds(20000 - map(i, 0, 180, 400, 2500));
    delay(10);     
  }
}

void offServoShift(){
  int binaryCmd = 0;
  for (int i = 0; i < switchLen; i++){
      if (switchStatus[i] == 2 && changeVar[i]) binaryCmd += power(2, i);
  }
  for (int i = onAngle; i >= offAngle; i--) { 
    digitalWrite(latchPin ,LOW); 
    shiftOut(dataPin ,clockPin ,MSBFIRST, binaryCmd); 
    digitalWrite(latchPin ,HIGH); 
    delayMicroseconds(map(i, 0, 180, 400, 2500));
    digitalWrite(latchPin ,LOW); 
    shiftOut(dataPin ,clockPin ,MSBFIRST, 0b00000000); 
    digitalWrite(latchPin ,HIGH);
    delayMicroseconds(20000 - map(i, 0, 180, 400, 2500));
    delay(10);     
  }
}
