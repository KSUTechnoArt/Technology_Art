// 定数:ホールセンサー関係
#define LEFT_HALL_SENSOR 4 // ホールセンサー用のピン番号
#define RIGHT_HALL_SENSOR 1 // ホールセンサー用のピン番号
int distance_Left_Hall;
int distance_Right_Hall;

void setup() {
  pinMode(LEFT_HALL_SENSOR, INPUT); // LEFT_HALL_SENSOR番のピンを入力に設定
  pinMode(RIGHT_HALL_SENSOR, INPUT); // RIGHT_HALL_SENSOR番のピンを入力に設定
  Serial.begin(115200); // シリアル通信の設定
}

void loop() {
  distance_Left_Hall = analogRead(LEFT_HALL_SENSOR); // ホールセンサーの取得値を読み込み、変数distance_Left_Hallに代入
  distance_Right_Hall = analogRead(RIGHT_HALL_SENSOR); // ホールセンサーの取得値を読み込み、変数distance_Right_Hallに代入
  Serial.print("distance_Left_Hall = ");
  Serial.print(distance_Left_Hall); // ホールセンサーの取得値をシリアルモニタに出力
  Serial.print("\t distance_Right_Hall = ");
  Serial.println(distance_Right_Hall); // ホールセンサーの取得値をシリアルモニタに出力
  delay(100);
}
