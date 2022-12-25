
#include <Wire.h> 

const int motorR = 0x60;
const int motorL = 0x68;
long Speed;

long SpeedL, SpeedR;


void setup()
{
  delay(1000);
  //Wire1.begin(25, 21);
  Wire.begin(1, 0);

  writeMotorResister(motorR, 0x00, 0x00); //停止
  writeMotorResister(motorL, 0x00, 0x00); //停止

  writeMotorResister(motorR, 0x01, 0x80); //エラー解除
  writeMotorResister(motorL, 0x01, 0x80); //エラー解除

  delay(1000);
}


//モータドライバ I2C制御 motor driver I2C
void writeMotorResister(int motor, byte vset, byte data1){
  int vdata = vset << 2 | data1;
  Wire.beginTransmission(motor);
  Wire.write(0x00);
  Wire.write(vdata);
  Wire.endTransmission(true);
}

void loop()
{

  byte i;

  while(1){
    for(i=0x06;i<=0x3F;i++){
      writeMotorResister(motorR, i, 0x01);
      writeMotorResister(motorL, i, 0x01);
      delay(200);
    }
  
    writeMotorResister(motorR, 0x00, 0x00); //停止
    writeMotorResister(motorL, 0x00, 0x00); //停止
    delay(4000);
    
    for(i=0x06;i<=0x3F;i++){
      writeMotorResister(motorR, i, 0x02);
      writeMotorResister(motorL, i, 0x02);
      delay(200);
    }
  
    writeMotorResister(motorR, 0x00, 0x00); //停止
    writeMotorResister(motorL, 0x00, 0x00); //停止
    delay(4000);
  }

  
}
