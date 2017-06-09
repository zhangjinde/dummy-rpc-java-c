# コンパイル方法

- cmake が必要です


1. javac Server.java
2. cmake .
3. make



# 実行方法

- java Server (サーバ)
- ./client (クライアント)


# 実行のスクリーンショット

- server.png
- client.png


# 定義した通信プロトコル

- サービスを取得する
  1. [client]: スペース区切りの文字列によるコマンドを送信
    - "fetch (サービス名)"
  2. (server) シリアライズしたオブジェクトのバイト列を送信してクローズ


- メソッドコール
  1. [client]: スペース区切りの文字列によるコマンドを送信
    - "call (サービス名) (メソッド名)"
  2. [client]: 4バイト(BE)でその後のバイト列の長さを送信
  3. [client]: シリアライズした第1引数のオブジェクトのバイト列を送信



# 苦労(工夫)した点

- バイト列を受信するときと，シリアライズしてバイト列を生成するとき，最終的なバイト列全体の長さがわからないので，バイト列の断片毎にmallocしリスト構造を作り，最後にconcatするようにした．(blist.h/.c)
- JavaでのClassオブジェクトを表現するようなCのstructの定義．Javaのシリアライズの文法に合わせて再帰構造になるようにした． (descriptor.h/.c)
- クライアントでRPCする際に `task.hello(&task, person);` という記述ができるようにした．つまりスタブ化している． (client.c, task.h/.c)
- Server.javaではサービスの登録時にだけ「Task」が登場し，それ以降は「Task」も「Person」も使われていない． (Reflection)
