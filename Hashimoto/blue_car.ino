// モーターとサーボモータ、ToFセンサー、フォトリフレクタ、ホールセンサーのプログラムを組み合わせたもの。

// このプログラムの動作(仕様)
// 「車から20cm以内に障害物がある」または「お皿にモノが置かれていない」ならば、1秒間停止する。
// 「車から5cm以内に障害物がある」ならば、3秒間停止する。
// 左側のホールセンサーに磁石があれば、LEDは点灯状態になり、右側のホールセンサーに磁石があれば、LEDは消灯状態になる。
// サーボモータは、左側のホールセンサーに磁石があれば、90度の方向へ回転し、右側のホールセンサーに磁石があれば、-90度の方向へ回転し、平常時は0度の方向へ回転する。

//Sharp MTOF171000C0 I2C
//Due MTOF
//3.3 VDD
//GND GND
//18  TX1
//19  RX1
//20  SDA
//21  SCL

/*
<サーボモーターについて>
PWMを利用して回転量を調整する。
HIGHとなる時間の範囲は0.5msから2.4msの間。

【参考用】
0.5ms  -> -90度
1.45ms -> 0度
2.4ms  -> 90度

なぜタイマー割込みなのか？ -> Servoライブラリが使えないため。
*/


#include <Wire.h>

// 定数:モーター関係
const int frontMotor = 0x68; // 前輪用モーター
const int backMotor = 0x60;  // 後輪用モーター
#define ADDRESS 0x52

// 定数：サーボモータ関係
#define SERVO_PIN 18 // サーボモータ(前輪モーター)のピン番号
hw_timer_t * timer = NULL; // timer 初期化
volatile int length_pwm_time_servo = 500; // サーボモータのPWM用変数。volatileは、すぐに値が変わるような変数の型として用いる。

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

//タイマー割込みサービスルーチン
void IRAM_ATTR call_Servo() {
  digitalWrite(SERVO_PIN, HIGH);
  delayMicroseconds(length_pwm_time_servo);
  digitalWrite(SERVO_PIN, LOW);
}

void setup() {
  delay(1000);
  Wire.begin(8, 10); // SDA:G8, SCL:G10
  stopDrive(); // 停止
  writeMotorResister(frontMotor, 0x01, 0x80); // エラー解除
  writeMotorResister(backMotor, 0x01, 0x80); // エラー解除
  delay(1000);
  pinMode(PHOTO_SENSOR, INPUT); // PHOTO_SENSOR番のピンを入力に設定
  pinMode(LED, OUTPUT); // LED番のピンを出力に設定
  pinMode(LEFT_HALL_SENSOR, INPUT); // LEFT_HALL_SENSOR番のピンを入力に設定
  pinMode(RIGHT_HALL_SENSOR, INPUT); // RIGHT_HALL_SENSOR番のピンを入力に設定
  pinMode(LED_PIN_HALL, OUTPUT); // <開発の最終段階で削除する>
  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW);
  timer = timerBegin(0, 80, true); // timer=1us
  timerAttachInterrupt(timer, &call_Servo, true); // タイマー割込みに使う関数を登録
  timerAlarmWrite(timer, 20000, true); // タイマー割込み時間の設定us単位 20ms
  timerAlarmEnable(timer);
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
    if(value_Left_Hall < 2000 || value_Left_Hall > 4000) {
      // 左側のホールセンサーに磁石があるとき
      digitalWrite(LED_PIN_HALL, HIGH); // <開発の最終段階で削除する>
      length_pwm_time_servo = 2400; // 90度
      delay(500);
    } else if (value_Right_Hall < 2000 || value_Right_Hall > 4000){
      // 右側のホールセンサーに磁石があるとき
      digitalWrite(LED_PIN_HALL, LOW); // <開発の最終段階で削除する>
      length_pwm_time_servo = 500; // -90度
      delay(500);
    } else {
      // 磁石がないとき
      length_pwm_time_servo = 1450; // 0度
      delay(500);
    }
  }
}

// 車の運転を停止する
void stopDrive() {
  writeMotorResister(frontMotor, 0x00, 0x00); // 停止
  writeMotorResister(backMotor, 0x00, 0x00); // 停止
}

// 車の運転を開始する
void startDrive() {
  byte car_speed;
  for(car_speed = 0x09; car_speed <= 0x10; car_speed++) {
    if(car_speed == 0x1E) {
      car_speed--;
    }
    writeMotorResister(frontMotor, car_speed, 0x02);
    writeMotorResister(backMotor, car_speed, 0x02);
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
