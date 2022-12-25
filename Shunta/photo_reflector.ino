// M5Stamp-C3のプログラム
// このプログラムの構造を保ったままモーターや距離センサー用にアレンジできる。
// 今回はフォトリフレクタ用にアレンジした。これは、モノの検知に用いるものである。
#define PHOTO_SENSOR 4 // フォトリフレクタ用のピン番号
#define LED 5 // LED用のピン番号

void setup() {
  pinMode(PHOTO_SENSOR, INPUT);  // PHOTO_SENSOR番のピンを入力に設定
  pinMode(LED, OUTPUT); // LED番のピンを出力に設定
  Serial.begin(115200); // シリアル通信の設定
}

void loop() {
  int dish_object_distance = analogRead(PHOTO_SENSOR); // フォトリフレクタの取得値を読み込み、変数dish_object_distanceに代入
  Serial.println(dish_object_distance); // フォトリフレクタの取得値をシリアルモニタに出力

  // ここから、皿にモノが置かれたのか検知する処理を記述
  if (dish_object_distance > 1000){
    digitalWrite(LED, LOW);
  } else {
    digitalWrite(LED, HIGH);
  }
  delay(500); // 500ミリ秒待機
}
