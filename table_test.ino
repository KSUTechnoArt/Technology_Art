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
const double backMotorL_Kp = 2.0; // 比例制御のための定数
const double backMotorR_Kp = -2.0; // 比例制御のための定数

// 定数:ToFセンサー関係
uint16_t distance_ToF;
uint16_t distance_tmp_ToF;
uint8_t data_cnt_ToF;
int countToF = 0; // ToFセンサーが何回反応したかカウントする(ホコリなどで誤停止する問題を解決するため)

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
int times_no_reaction_Hall = 0; // ホールセンサーが磁石に反応しなかった時間

void setup() {
  Wire.begin(8, 10); // SDA:G8, SCL:G10
  stopDrive(500); // 停止
  writeMotorResister(frontMotor, 0x01, 0x80); // エラー解除
  writeMotorResister(backMotorL, 0x01, 0x80); // エラー解除
  writeMotorResister(backMotorR, 0x01, 0x80); // エラー解除
  pinMode(PHOTO_SENSOR, INPUT); // PHOTO_SENSOR番のピンを入力に設定
  pinMode(LED, OUTPUT); // LED番のピンを出力に設定
  pinMode(LEFT_HALL_SENSOR, INPUT); // LEFT_HALL_SENSOR番のピンを入力に設定
  pinMode(RIGHT_HALL_SENSOR, INPUT); // RIGHT_HALL_SENSOR番のピンを入力に設定
  pinMode(LED_PIN_HALL, OUTPUT); // <開発の最終段階で削除する>
  Serial.begin(115200); // シリアル通信の設定
  delay(500);
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
  delay(10);
}


// 車の運転中の処理
void duringDriveCar() {
  if(distance_ToF <= 50 || distance_PHOTO > 2500) {
    // 停止中の処理(相手が手の場合)
    countToF++;
    if(countToF > 10) {
      digitalWrite(LED, LOW); // LED消灯 // <開発の最終段階で削除する>
      stopDrive(3000); // 停止(ここはモノを取るのに十分な時間だけ止まるよう設定しておく)
      countToF = 10;
    }
  } else if(distance_ToF <= 80) {
    // 停止中の処理(相手が車の場合)
    countToF++;
    if(countToF > 10) {
      digitalWrite(LED, LOW); // LED消灯 // <開発の最終段階で削除する>
      stopDrive(10000); // 停止(ここは車が半周するまでの時間に設定しておく)
      countToF = 10;
    }
  } else {
    // 走行中の処理
    countToF = 0;
    digitalWrite(LED, HIGH); // LED点灯 // <開発の最終段階で削除する>
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
      if(value_determine_RL < 45) {
      writeMotorResister(frontMotor, 0x18, 0x02);
      }
      writeMotorResister(backMotorL, value_determine_RL, 0x01);
      writeMotorResister(backMotorR, 63, 0x02);
      Serial.print("Speed : ");
      Serial.println(value_determine_RL);
    } else {
      // 左側のホールセンサーに磁石があるとき
      value_determine_RL = (int)(backMotorL_Kp * (value_determine_RL - 50) + 63);
      value_determine_RL = max(6, min(63, value_determine_RL));
      digitalWrite(LED_PIN_HALL, LOW); // <開発の最終段階で削除する>
      if(value_determine_RL < 45) {
      writeMotorResister(frontMotor, 0x18, 0x01);
      }
      writeMotorResister(backMotorL, 63, 0x01);
      writeMotorResister(backMotorR, value_determine_RL, 0x02);
      Serial.print("Speed : ");
      Serial.println(value_determine_RL);
    }

    // テーブルからの落下防止
    if(times_no_reaction_Hall >= 3000) {
      while(true) {
        stopDrive(1000); // 落下しそうになったら永遠に動かさないようにする。
      }
    } else if(30 < value_determine_RL && value_determine_RL < 80) {
      times_no_reaction_Hall += 10; // 磁石が反応しなかった時間を加える。
    } else {
      times_no_reaction_Hall = 0; // 磁石が反応しなかった時間をリセットする。
    }
  }
}

// 車の運転を開始する
void startDrive() {
  car_speed = 63;
  //writeMotorResister(frontMotor, 0x10, 0x01);
  writeMotorResister(backMotorL, car_speed, 0x01);
  writeMotorResister(backMotorR, car_speed, 0x01);
}

// 車の運転を停止する
void stopDrive(int time) {
  writeMotorResister(frontMotor, 0x00, 0x00); // 停止
  writeMotorResister(backMotorL, 0x00, 0x00); // 停止
  writeMotorResister(backMotorR, 0x00, 0x00); // 停止
  delay(time);
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
