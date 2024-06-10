#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <SoftwareSerial.h>
SoftwareSerial mySerial(8,9);
byte statusLed    = 13;
byte sensorInterrupt = 0;
byte sensorPin       = 2;
float calibrationFactor = 4.5;
volatile byte pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;
void setup()
{
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  lcd.init();                      // initialize the lcd
  lcd.init();
  lcd.backlight();
  mySerial.begin(57600);
  Serial.begin(9600);
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}
void loop()
{
  if ((millis() - oldTime) > 1000)   // Only process counters once per second
  {
    detachInterrupt(sensorInterrupt);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    unsigned int frac;
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));
    Serial.print("L/min");
    Serial.print("\t");
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.println("mL");
    Serial.print("\t");       // Print tab space
    Serial.print(totalMilliLitres / 1000);
    Serial.print("L");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BLOOD FLOW RATE");
    lcd.setCursor(0, 1);
    lcd.print(flowRate);
    if (flowRate > 20)
    {
      digitalWrite(7, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(3, HIGH);
      delay(2);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  FLOW RATE IS");
      lcd.setCursor(0, 1);
      lcd.print("      HIGH");
      SendMessage("FLOW RATE IS VERY HIGH, NEED TO CONSULT DOCTOR");
    }
    if (flowRate > 3 && flowRate < 8)
    {
      digitalWrite(7, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);
      digitalWrite(3, LOW);
      delay(2);
    }
    if (flowRate > 8 && flowRate < 12)
    {
      digitalWrite(7, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);
      digitalWrite(3, LOW);
      delay(2);
    }
    if (flowRate > 12 && flowRate < 16)
    {
      digitalWrite(7, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(4, LOW);
      digitalWrite(3, LOW);
      delay(2);
    }
    if (flowRate > 16 && flowRate < 18)
    {
      digitalWrite(7, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(3, LOW);
      delay(2);
    }
    if (flowRate <= 0)
    {
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);
      digitalWrite(3, LOW);
      delay(2);
    }
    pulseCount = 0;
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

void SendMessage(String a)
{
  mySerial.println("AT+CMGF=1");
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+919949403755\"\r");
  delay(1000);
  Serial.println(a);
  mySerial.println(a);
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}
