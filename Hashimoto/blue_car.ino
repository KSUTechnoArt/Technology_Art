//Sharp MTOF171000C0 I2C
//Due MTOF
//3.3 VDD
//GND GND
//18  TX1
//19  RX1
//20  SDA
//21  SCL

#include <Wire.h>
const int motorR = 0x60;
const int motorL = 0x68;
long Speed;
long SpeedL, SpeedR;
#define ADDRESS 0x52
uint16_t distance;
uint16_t distance_tmp;
uint8_t data_cnt;

void setup() {
  delay(1000);
  //Wire1.begin(25, 21);
  Wire.begin(1, 0);  //SDA:G1, SCL:G0
  writeMotorResister(motorR, 0x00, 0x00); //停止
  writeMotorResister(motorL, 0x00, 0x00); //停止
  writeMotorResister(motorR, 0x01, 0x80); //エラー解除
  writeMotorResister(motorL, 0x01, 0x80); //エラー解除
  delay(1000);
  Serial.begin(115200);
  Serial.println("MTOF171000C0_I2C");
  //I2Cの初期化
  delay(1000);
}

void loop() {
  while(1) {
    Serial.print("distance = ");
    Wire.beginTransmission(ADDRESS);
    Wire.write(0xD3);
    Wire.endTransmission(false);
    Wire.requestFrom(ADDRESS, 2);
    data_cnt = 0;
    distance = 0;
    distance_tmp = 0;
    while(Wire.available()) {
      distance_tmp = Wire.read();
      distance = (distance << (data_cnt*8)) | distance_tmp;
      data_cnt++;
    }
    Serial.print(distance);
    Serial.println(" mm");
    byte carSpeed;
    for (carSpeed = 0x09; carSpeed <= 0x10; carSpeed++) {
      if (carSpeed == 0x1E) {
        carSpeed--;
      }
      writeMotorResister(motorR, carSpeed, 0x02);
      writeMotorResister(motorL, carSpeed, 0x02);
      //delay(200);
    }
    //for(carSpeed = 0x06; carSpeed <= 0x3F; carSpeed++){
     // writeMotorResister(motorR, carSpeed, 0x02);
     // writeMotorResister(motorL, carSpeed, 0x02);
      //delay(200);
    //}
    if(distance <= 50){
        writeMotorResister(motorR, 0x00, 0x00); //停止
        writeMotorResister(motorL, 0x00, 0x00); //停止
        delay(500);
    }
  }
  delay(45);
}

//モータドライバ I2C制御 motor driver I2C
void writeMotorResister(int motor, byte vset, byte data1){
  int vdata = vset << 2 | data1;
  Wire.beginTransmission(motor);
  Wire.write(0x00);
  Wire.write(vdata);
  Wire.endTransmission(true);
}

int readDistance(byte reg) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(ADDRESS, 2);
  data_cnt = 0;
  distance = 0;
  distance_tmp = 0;
  while(Wire.available()) {
    distance_tmp = Wire.read();
    distance = (distance << (data_cnt*8)) | distance_tmp;
    data_cnt++;
  }
  return distance;
}
