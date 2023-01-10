// モーターとToFセンサー、フォトリフレクタ、ホールセンサーのプログラムを組み合わせたもの。

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
const int frontMotor = 0x62; // 前輪用モーター
const int backMotorL = 0x60; // 後輪用モーター(左)
const int backMotorR = 0x68; // 後輪用モーター(右)
#define ADDRESS 0x52
byte car_speed; // モーターの回転速度を制御するための変数
const double backMotorL_Kp = 0.2; // 比例制御のための定数
const double backMotorR_Kp = -0.2; // 比例制御のための定数

// 定数:ToFセンサー関係
uint16_t distance_ToF;
uint16_t distance_tmp_ToF;
uint8_t data_cnt_ToF;

// 定数:フォトリフレクタ関係
int distance_PHOTO;
#define PHOTO_SENSOR 4 // フォトリフレクタ用のピン番号
#define LED 5 // LED用のピン番号 // <開発の最終段階で削除する>

// 定数:ホールセンサー関係
#define RIGHT_HALL_SENSOR 0 // ホールセンサー用のピン番号
#define LEFT_HALL_SENSOR 1 // ホールセンサー用のピン番号
#define LED_PIN_HALL 6 // <開発の最終段階で削除する>
int value_Left_Hall;
int value_Right_Hall;

void setup() {
  delay(1000);
  Wire.begin(8, 10); // SDA:G8, SCL:G10
  stopDrive(); // 停止
  writeMotorResister(frontMotor, 0x01, 0x80); // エラー解除
  writeMotorResister(backMotorL, 0x01, 0x80); // エラー解除
  writeMotorResister(backMotorR, 0x01, 0x80); // エラー解除
  delay(1000);
  pinMode(PHOTO_SENSOR, INPUT); // PHOTO_SENSOR番のピンを入力に設定
  pinMode(LED, OUTPUT); // LED番のピンを出力に設定
  pinMode(LEFT_HALL_SENSOR, INPUT); // LEFT_HALL_SENSOR番のピンを入力に設定
  pinMode(RIGHT_HALL_SENSOR, INPUT); // RIGHT_HALL_SENSOR番のピンを入力に設定
  pinMode(LED_PIN_HALL, OUTPUT); // <開発の最終段階で削除する>
  Serial.begin(115200); // シリアル通信の設定
  delay(1000);
}

void loop() {
  readDistance(0xD3); // 距離の読み込み(ToFセンサー)
  Serial.print("distance_ToF = ");
  Serial.print(distance_ToF); // ToFセンサーの取得値をシリアルモニタに出力
  Serial.println(" mm");
  
  distance_PHOTO = analogRead(PHOTO_SENSOR); // フォトリフレクタの取得値を読み込み、変数distance_PHOTOに代入
  Serial.print("Photo Sensor:");
  Serial.println(distance_PHOTO); // フォトリフレクタの取得値をシリアルモニタに出力
  
  startDrive(); // 運転を開始
  duringDriveCar(); // 運転中の処理を呼び出す
  delay(45);
}

// 車の運転中の処理
void duringDriveCar() {
  if(distance_ToF <= 50) {
    // 車同士が原因で停止する場合
    // 停止中の処理
    digitalWrite(LED, LOW); // LED消灯 // 開発の最終段階で消す
    stopDrive(); // 停止
    delay(3000); // ここは車が半周するまでの時間に設定しておく。
  } else if(distance_ToF <= 200 || distance_PHOTO > 1000) {
    // 人間が原因で停止する場合
    // 停止中の処理
    digitalWrite(LED, LOW); // LED消灯 // 開発の最終段階で消す
    stopDrive(); // 停止

    // もう一方の車も止める(今後その処理を追記する)

    delay(1000);
  } else {
    // 走行中の処理
    digitalWrite(LED, HIGH); // LED点灯 // 開発の最終段階で消す
    startDrive(); // 運転を開始
    value_Left_Hall = analogRead(LEFT_HALL_SENSOR); // ホールセンサーの取得値を読み込み、変数value_Left_Hallに代入
    value_Right_Hall = analogRead(RIGHT_HALL_SENSOR); // ホールセンサーの取得値を読み込み、変数value_Right_Hallに代入
    Serial.print("value_Left_Hall = ");
    Serial.print(value_Left_Hall); // ホールセンサーの取得値をシリアルモニタに出力
    Serial.print("\t value_Right_Hall = ");
    Serial.println(value_Right_Hall); // ホールセンサーの取得値をシリアルモニタに出力
    int value_determine_RL = value_Right_Hall - value_Left_Hall;
    Serial.println(value_determine_RL); // ホールセンサーの取得値をシリアルモニタに出力
    if(value_determine_RL > 0) {
      // 右側のホールセンサーに磁石があるとき
      value_determine_RL = (int)(backMotorR_Kp * (value_determine_RL - 50) + 63);
      value_determine_RL = max(6, min(63, value_determine_RL));
      digitalWrite(LED_PIN_HALL, HIGH); // <開発の最終段階で削除する>
      writeMotorResister(backMotorR, value_determine_RL, 0x01);
      writeMotorResister(backMotorL, 63, 0x01);
      Serial.print("Speed : ");
      Serial.println(value_determine_RL);
    } else {
      // 左側のホールセンサーに磁石があるとき
      value_determine_RL = (int)(backMotorL_Kp * (value_determine_RL - 50) + 63);
      value_determine_RL = max(6, min(63, value_determine_RL));
      digitalWrite(LED_PIN_HALL, LOW); // <開発の最終段階で削除する>
      writeMotorResister(backMotorR, 63, 0x01);
      writeMotorResister(backMotorL, value_determine_RL, 0x01);
      Serial.print("Speed : ");
      Serial.println(value_determine_RL);
    }
    delay(10);
  }
}

// 車の運転を停止する
void stopDrive() {
  writeMotorResister(frontMotor, 0x00, 0x00); // 停止
  writeMotorResister(backMotorL, 0x00, 0x00); // 停止
  writeMotorResister(backMotorR, 0x00, 0x00); // 停止
}

// 車の運転を開始する
void startDrive() {
  car_speed = 63;
  writeMotorResister(frontMotor, 0x10, 0x01);
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

// 距離の読み込み(ToFセンサー)
int readDistance(byte reg) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(ADDRESS, 2);
  data_cnt_ToF = 0;
  distance_ToF = 0;
  distance_tmp_ToF = 0;
  while(Wire.available()) {
    distance_tmp_ToF = Wire.read();
    distance_ToF = (distance_ToF << (data_cnt_ToF * 8)) | distance_tmp_ToF;
    data_cnt_ToF++;
  }
  return distance_ToF;
}
