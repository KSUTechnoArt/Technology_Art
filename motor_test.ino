#include <Wire.h>

// 定数:モーター関係
const int frontMotor = 0x62; // 前輪用モーター
const int backMotorL = 0x60; // 後輪用モーター(左)
const int backMotorR = 0x68; // 後輪用モーター(右)
#define ADDRESS 0x52
byte car_speed; // モーターの回転速度を制御するための変数

void setup() {
  delay(1000);
  Wire.begin(8, 10); // SDA:G8, SCL:G10
  stopDrive(); // 停止
  writeMotorResister(frontMotor, 0x01, 0x80); // エラー解除
  writeMotorResister(backMotorL, 0x01, 0x80); // エラー解除
  writeMotorResister(backMotorR, 0x01, 0x80); // エラー解除
  delay(1000);
  Serial.begin(115200); // シリアル通信の設定
}

void loop() {
  duringDriveCar(); // 運転中の処理を呼び出す
  delay(45);
}

// 車の運転中の処理
void duringDriveCar() {
  // 走行中の処理
  writeMotorResister(frontMotor, 0x3f, 0x01);
  delay(1000);
  stopDrive();
  delay(1000);
  writeMotorResister(backMotorR, 0x3f, 0x01);
  delay(1000);
  stopDrive();
  delay(1000);
  writeMotorResister(backMotorL, 0x3f, 0x01);
  delay(1000);
  stopDrive();
  delay(1000);
}

// 車の運転を停止する
void stopDrive() {
  writeMotorResister(frontMotor, 0x00, 0x00); // 停止
  writeMotorResister(backMotorL, 0x00, 0x00); // 停止
  writeMotorResister(backMotorR, 0x00, 0x00); // 停止
}

// 車の運転を開始する
void startDrive() {
  car_speed = 0x3f;
  writeMotorResister(frontMotor, car_speed, 0x01);
  writeMotorResister(backMotorL, car_speed, 0x01);
  writeMotorResister(backMotorR, car_speed, 0x01);
}

// モータドライバ I2C制御 motor driver I2C
void writeMotorResister(int motor, byte vset, byte data1) {
  int vdata = vset << 2 | data1;
  Wire.beginTransmission(motor);
  Wire.write(0x00);
  Wire.write(vdata);
  Wire.endTransmission(true);
}
