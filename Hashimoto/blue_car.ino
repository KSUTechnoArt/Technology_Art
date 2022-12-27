// モーターとToFセンサーとフォトリフレクタのプログラムを組み合わせたもの。
// 「車から5cm以内に障害物がある」または「お皿にモノが置かれていない」ならば、0.5秒間停止するプログラムである。

//Sharp MTOF171000C0 I2C
//Due MTOF
//3.3 VDD
//GND GND
//18  TX1
//19  RX1
//20  SDA
//21  SCL

#include <Wire.h>
// 定数:モーター関係
const int motorR = 0x60;
const int motorL = 0x68;
long Speed;
long SpeedL, SpeedR;
#define ADDRESS 0x52

// 定数:ToFセンサー関係
uint16_t distance;
uint16_t distance_tmp;
uint8_t data_cnt;

// 定数:フォトリフレクタ関係
int dish_object_distance;
#define PHOTO_SENSOR 4 // フォトリフレクタ用のピン番号
#define LED 5 // LED用のピン番号 // <開発の最終段階で削除する>

void setup() {
  delay(1000);
  //Wire1.begin(25, 21);
  Wire.begin(1, 0);  // SDA:G1, SCL:G0
  writeMotorResister(motorR, 0x00, 0x00); // 停止
  writeMotorResister(motorL, 0x00, 0x00); // 停止
  writeMotorResister(motorR, 0x01, 0x80); // エラー解除
  writeMotorResister(motorL, 0x01, 0x80); // エラー解除
  delay(1000);
  pinMode(PHOTO_SENSOR, INPUT);  // PHOTO_SENSOR番のピンを入力に設定
  pinMode(LED, OUTPUT); // LED番のピンを出力に設定
  Serial.begin(115200); // シリアル通信の設定}
  delay(1000);
}

void loop() {
  readDistance(0xD3); // 距離の読み込み(ToFセンサー)
  Serial.print("distance = ");
  Serial.print(distance); // ToFセンサーの取得値をシリアルモニタに出力
  Serial.println(" mm");

  dish_object_distance = analogRead(PHOTO_SENSOR); // フォトリフレクタの取得値を読み込み、変数dish_object_distanceに代入
  Serial.print("Photo Sensor:");
  Serial.println(dish_object_distance); // フォトリフレクタの取得値をシリアルモニタに出力
  startDrive(); // 運転を開始
  duringDriveCar(); // 運転中の処理を呼び出す
  delay(45);
}

// 車の運転中の処理
void duringDriveCar() {
  if(distance <= 50 || dish_object_distance > 1000) {
    // 車が停止しているときの処理
    digitalWrite(LED, LOW); // LED消灯 // 開発の最終段階で消す
    writeMotorResister(motorR, 0x00, 0x00); // 停止
    writeMotorResister(motorL, 0x00, 0x00); // 停止
    delay(500);
  } else {
    // 車が走行しているときの処理 // 開発の最終段階で消す
    digitalWrite(LED, HIGH); // LED点灯
    startDrive(); // 運転を開始
  }  
}

// 車の運転を開始する
void startDrive() {
  byte car_speed;
  for(car_speed = 0x09; car_speed <= 0x10; car_speed++) {
    if(car_speed == 0x1E) {
      car_speed--;
    }
    writeMotorResister(motorR, car_speed, 0x02);
    writeMotorResister(motorL, car_speed, 0x02);
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

// 距離の読み込み(ToFセンサー)
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
    distance = (distance << (data_cnt * 8)) | distance_tmp;
    data_cnt++;
  }
  return distance;
}
