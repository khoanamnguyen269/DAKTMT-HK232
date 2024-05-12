#define data_PIN  18
#define latch_PIN 17
#define shift_PIN 16

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
bool matrixLED[8][8][8], mod = 0;
uint8_t z_axis = 0, x_axis = 0, y_axis = 0, line[8], state = 1;
void setup()
{
  // put your setup code here, to run once:
  pinMode(data_PIN, OUTPUT);
  pinMode(latch_PIN, OUTPUT);
  pinMode(shift_PIN, OUTPUT);
  SerialBT.begin("ESP32test");
  Serial.begin(115200);
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        matrixLED[i][j][k] = 0;
      }
    }
  }
  digitalWrite(latch_PIN, LOW);
  shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
  shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
  shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
  shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
  shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
  shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
  shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
  shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
  shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
  digitalWrite(latch_PIN, HIGH);
}
void bluetoothScanning()
{
  uint8_t rec;
  if (SerialBT.available())
  {
    rec = SerialBT.read();
    Serial.println(rec);
    if (rec == 0)
    {
      z_axis = rec;
      x_axis = rec;
      y_axis = rec;
    }
    if (rec >= 1 && rec <= 8)
    {
      z_axis = rec;
    }
    if (rec >= 11 && rec <= 88)
    {
      x_axis = rec / 10;
      y_axis = rec % 10;
    }
    if (rec == 100)
    {
      mod = 1;
      state = 1;
      for (int i = 0; i < 8; i++)
      {
        for (int j = 0; j < 8; j++)
        {
          for (int k = 0; k < 8; k++)
          {
            matrixLED[i][j][k] = 0;
          }
        }
      }
    }
    if (rec == 101)
    {
      mod = 0;
      for (int i = 0; i < 8; i++)
      {
        for (int j = 0; j < 8; j++)
        {
          for (int k = 0; k < 8; k++)
          {
            matrixLED[i][j][k] = 0;
          }
        }
      }
    }
    if (mod == 1 && (rec >= 102 || rec <= 107))
    {
      if (rec == 102) state = 0;
      if (rec == 103) state = 1;
      if (rec == 104) state = 2;
      if (rec == 105) state = 3;
      if (rec == 106) state = 4;
      if (rec == 107) state = 5;
    }
    if (z_axis != 0 && x_axis != 0 && y_axis != 0)
    {
      matrixLED[z_axis - 1][x_axis - 1][y_axis - 1] = !matrixLED[z_axis - 1][x_axis - 1][y_axis - 1];
      Serial.print(z_axis);
      Serial.print("  ");
      Serial.print(x_axis);
      Serial.print("  ");
      Serial.print(y_axis);
      Serial.print("  ");
      Serial.println( matrixLED[z_axis - 1][x_axis - 1][y_axis - 1]);
    }
  }
}
int floorNumber(int a)
{
  int total = 1, floNum;
  a = 8 - a;
  for (int i = 0; i < a - 1; i++)
  {
    total = total * 2;
  }
  floNum = total ^ 255;
  return floNum;
}
void manualMOD()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      //      if ( i == 6 && j == 2) mắt này ngược
      //        line[j] = matrixLED[i][j][0] + matrixLED[i][j][1] * 2 + matrixLED[i][j][2] * 4 + matrixLED[i][j][3] * 8 + (1 - matrixLED[i][j][4]) * 16 + matrixLED[i][j][5] * 32 + matrixLED[i][j][6] * 64 + matrixLED[i][j][7] * 128;
      //      else
      line[j] = matrixLED[i][j][0] + matrixLED[i][j][1] * 2 + matrixLED[i][j][2] * 4 + matrixLED[i][j][3] * 8 + matrixLED[i][j][4] * 16 + matrixLED[i][j][5] * 32 + matrixLED[i][j][6] * 64 + matrixLED[i][j][7] * 128;
    }
    digitalWrite(latch_PIN, LOW);
    shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
    shiftOut(data_PIN, shift_PIN, LSBFIRST, line[7]);
    shiftOut(data_PIN, shift_PIN, LSBFIRST, line[6]);
    shiftOut(data_PIN, shift_PIN, LSBFIRST, line[5]);
    shiftOut(data_PIN, shift_PIN, LSBFIRST, line[4]);
    shiftOut(data_PIN, shift_PIN, LSBFIRST, line[3]);
    shiftOut(data_PIN, shift_PIN, LSBFIRST, line[2]);
    shiftOut(data_PIN, shift_PIN, LSBFIRST, line[1]);
    shiftOut(data_PIN, shift_PIN, LSBFIRST, line[0]);
    digitalWrite(latch_PIN, HIGH);
  }
}
void autoMOD()
{
  static unsigned long pretim = millis();
  if (state == 0)
  {
    for (int i = 0; i < 7; i++)
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7 - i));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      delay(100);
    }
    for (int i = 7; i >= 0; i--)
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7 - i));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      delay(100);
    }
  }
  if (state == 1)
  {
    if (millis() - pretim < 200 || (millis() - pretim > 1200 && millis() - pretim < 1400))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      for (int i = 1; i < 7; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if ((millis() - pretim > 200 && millis() - pretim < 400) || (millis() - pretim > 1000 && millis() - pretim < 1200))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      for (int i = 2; i < 6; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if ((millis() - pretim > 400 && millis() - pretim < 600) || (millis() - pretim > 800 && millis() - pretim < 1000))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      for (int i = 3; i < 5; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if (millis() - pretim > 600 && millis() - pretim < 800)
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if (millis() - pretim > 1400) pretim = millis();
  }
  if (state == 2)
  {
    if (millis() - pretim < 200)
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 200 && millis() - pretim < 400))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 400 && millis() - pretim < 600))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 600 && millis() - pretim < 800))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 800 && millis() - pretim < 1000))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 1000 && millis() - pretim < 1200))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 1200 && millis() - pretim < 1400))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 1400 && millis() - pretim < 1600))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if (millis() - pretim > 1600) pretim = millis();
  }
  if (state == 3)
  {
    if (millis() - pretim < 200)
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00011000);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 200 && millis() - pretim < 400))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00100100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b00111100);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 400 && millis() - pretim < 600))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01000010);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b01111110);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 600 && millis() - pretim < 800))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0b10000001);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
      digitalWrite(latch_PIN, HIGH);
    }
    if (millis() - pretim > 800) pretim = millis();
  }
  if (state == 4)
  {
    if (millis() - pretim < 200)
    {
      for (int i = 0; i < 8; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if ((millis() - pretim > 200 && millis() - pretim < 400) || (millis() - pretim > 2600 && millis() - pretim < 2800))
    {
      for (int i = 0; i < 8; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if ((millis() - pretim > 400 && millis() - pretim < 600) || (millis() - pretim > 2400 && millis() - pretim < 2600))
    {
      for (int i = 0; i < 8; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if ((millis() - pretim > 600 && millis() - pretim < 800) || (millis() - pretim > 2200 && millis() - pretim < 2400))
    {
      for (int i = 0; i < 8; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if ((millis() - pretim > 800 && millis() - pretim < 1000) || (millis() - pretim > 2000 && millis() - pretim < 2200))
    {
      for (int i = 0; i < 8; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if ((millis() - pretim > 1000 && millis() - pretim < 1200) || (millis() - pretim > 1800 && millis() - pretim < 2000))
    {
      for (int i = 0; i < 8; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if ((millis() - pretim > 1200 && millis() - pretim < 1400) || (millis() - pretim > 1600 && millis() - pretim < 1800))
    {
      for (int i = 0; i < 8; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if ((millis() - pretim > 1400 && millis() - pretim < 1600))
    {
      for (int i = 0; i < 8; i++)
      {
        digitalWrite(latch_PIN, LOW);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(i));
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
        shiftOut(data_PIN, shift_PIN, LSBFIRST, 0xff);
        digitalWrite(latch_PIN, HIGH);
      }
    }
    if (millis() - pretim > 2800) pretim = millis();
  }
  if (state == 5)
  {
    bool mapping[8][8];
    static bool done = 0;
    if (done == 0)
    {
      for (int i = 0; i < 8; i++)
      {
        for (int j = 0; j < 8; j++)
        {
          mapping[i][j] = 0;
        }
      }
      for (int i = 0; i < 8; i++)
      {
        int num = rand() % 64;
        mapping[num / 8][num % 8] = 1;
      }
      for (int j = 0; j < 8; j++)
      {
        line[j] = mapping[j][0] + mapping[j][1] * 2 + mapping[j][2] * 4 + mapping[j][3] * 8 + mapping[j][4] * 16 + mapping[j][5] * 32 + mapping[j][6] * 64 + mapping[j][7] * 128;
      }
      done = 1;
    }
    if (millis() - pretim < 100)
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[7]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[6]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[5]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[4]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[3]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[2]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[1]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[0]);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 100 && millis() - pretim < 200))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[7]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[6]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[5]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[4]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[3]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[2]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[1]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[0]);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 200 && millis() - pretim < 300))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[7]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[6]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[5]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[4]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[3]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[2]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[1]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[0]);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 300 && millis() - pretim < 400))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[7]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[6]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[5]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[4]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[3]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[2]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[1]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[0]);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 400 && millis() - pretim < 500))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[7]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[6]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[5]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[4]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[3]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[2]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[1]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[0]);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 500 && millis() - pretim < 600))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[7]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[6]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[5]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[4]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[3]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[2]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[1]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[0]);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 600 && millis() - pretim < 700))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[7]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[6]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[5]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[4]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[3]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[2]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[1]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[0]);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
    }
    if ((millis() - pretim > 700 && millis() - pretim < 800))
    {
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(7));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(6));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(5));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(4));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(3));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(2));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(1));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, 0x00);
      digitalWrite(latch_PIN, HIGH);
      digitalWrite(latch_PIN, LOW);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, floorNumber(0));
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[7]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[6]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[5]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[4]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[3]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[2]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[1]);
      shiftOut(data_PIN, shift_PIN, LSBFIRST, line[0]);
      digitalWrite(latch_PIN, HIGH);
    }
    if (millis() - pretim > 800)
    {
      pretim = millis();
      done = 0;
    }
  }
}
void loop()
{
  bluetoothScanning();
  if (mod == 0)   manualMOD();
  if (mod == 1)   autoMOD();
}
