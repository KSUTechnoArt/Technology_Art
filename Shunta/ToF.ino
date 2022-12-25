// M5Stamp-C3のプログラム
// このプログラムの構造を保ったままモーターや距離センサー用にアレンジできる。
// 今回はフォトリフレクタ用にアレンジした。これは、モノの検知に用いるものである。
#define ToF 4 // フォトリフレクタ
#define LED 5 // LED

void setup() {
  pinMode(ToF, INPUT);  // ToF番ピンを入力に設定
  pinMode(LED, OUTPUT); // LED番ピンを出力に設定
  Serial.begin(115200); // シリアル通信の設定
}

void loop() {
  int data = analogRead(ToF); // フォトリフレクタの取得値を読み込む
  Serial.println(data);       // シリアルモニタに取得値を出力
  // ここから、皿にモノが置かれたのか検知する処理を記述
  if (data > 1000){
    digitalWrite(LED, LOW);
  } else {
    digitalWrite(LED, HIGH);
  }
  delay(500); // 500ミリ秒待機
}
