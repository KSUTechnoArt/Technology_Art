#include <Wire.h> 

const int motorR = 0x60;
const int motorL = 0x68;
long Speed;
long SpeedL, SpeedR;

void setup() {
  delay(1000);
  //Wire1.begin(25, 21);
  Wire.begin(1, 0);

  writeMotorResister(motorR, 0x00, 0x00); // 停止
  writeMotorResister(motorL, 0x00, 0x00); // 停止

  writeMotorResister(motorR, 0x01, 0x80); // エラー解除
  writeMotorResister(motorL, 0x01, 0x80); // エラー解除

  delay(1000);
}

void loop() {
  byte car_speed;
  while(1) {
    for(car_speed = 0x06; car_speed <= 0x3F; car_speed++) {
      writeMotorResister(motorR, car_speed, 0x01);
      writeMotorResister(motorL, car_speed, 0x01);
      delay(200);
    }
    
    writeMotorResister(motorR, 0x00, 0x00); // 停止
    writeMotorResister(motorL, 0x00, 0x00); // 停止
    delay(4000);
    
    for(car_speed = 0x06; car_speed <= 0x3F; car_speed++) {
      writeMotorResister(motorR, car_speed, 0x02);
      writeMotorResister(motorL, car_speed, 0x02);
      delay(200);
    }

    writeMotorResister(motorR, 0x00, 0x00); // 停止
    writeMotorResister(motorL, 0x00, 0x00); // 停止
    delay(4000);
  }
}

// モータドライバ I2C制御 motor driver I2C
void writeMotorResister(int motor, byte vset, byte data1) {
  int vdata = vset << 2 | data1;
  Wire.beginTransmission(motor);
  Wire.write(0x00);
  Wire.write(vdata);
  Wire.endTransmission(true);
}
