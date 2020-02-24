#define BLYNK_USE_DIRECT_CONNECT
#define BLYNK_PRINT Serial

#include <BlynkSimpleSerialBLE.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <SPI.h>

char auth[] = "B3PQzPq4wDHWaeukUqdJ5xx7GtTe8wy8";

#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4    
#define BLUEFRUIT_VERBOSE_MODE         true
#define BLYNK_RED       "#D3435C"
#define BLYNK_DEFAULT_COLOR "#787878"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}
unsigned int switchStatus[2] = {2,2};
bool changeVar1 = false, changeVar2 = false;

BLYNK_WRITE(V1){
  int tempCmp = switchStatus[0];
  switchStatus[0] = param.asInt();
  if (switchStatus[0] != tempCmp) changeVar1 = true;
  else return;
}
BLYNK_WRITE(V2){
  int tempCmp = switchStatus[1];
  switchStatus[1] = param.asInt();
  if (switchStatus[1] != tempCmp) changeVar2 = true;
  else return;
}
void initialButtonColor(){
  Blynk.virtualWrite(V1, switchStatus[0]);
  Blynk.virtualWrite(V2, switchStatus[1]);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
}

void setup() { 
/******************************************Setup part*********************************************/  
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
  
/*****************************************Initialization*******************************************/
  initialButtonColor();
}

void loop() {                         //switchStatus contains elements of on/off. 1--on, 2--off
  Blynk.run();
  if (!changeVar1 && !changeVar2){
    delay(10);
    return;
  }
  for (int i = 0; i < 2; i++){
    Serial.println(switchStatus[i]);
  }
  Serial.println("***********");
  changeVar1 = false;
  changeVar2 = false;
  //Add code to control servo motor with instructions of switchStatus[]
}
