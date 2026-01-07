#pragma once

#include <cstdint>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Register {
private:
  uint8_t a;
  uint8_t x;
  uint8_t y;
  uint8_t s;
  uint8_t p;
  uint16_t pc;
  uint8_t flag = 0b00100000;
public:
  void SetPC(uint16_t addr);
  void IncPC();
  uint16_t PC();
  uint8_t A();
  void SetA(uint8_t data);
  uint8_t X();
  void SetX(uint8_t data);
  uint8_t Y();
  void SetY(uint8_t data);
  uint8_t S();
  void SetS(uint8_t data);
  uint8_t P();
  void SetP(uint8_t data);

  void SetCarry();
  void UnsetCarry();
  void SetZero();
  void UnsetZero();
  void SetIRQ();
  void UnsetIRQ();
  void SetDecimal();
  void UnsetDecimal();
  void SetBreak();
  void UnsetBreak();
  void SetOverflow();
  void UnsetOverflow();
  void SetNegative();
  void UnsetNegative();
};

/*
A	8bit	アキュームレータ	汎用演算
X	8bit	インデックスレジスタ	アドレッシング、カウンタなど
Y	8bit	インデックスレジスタ	アドレッシング、カウンタなど
S	8bit	スタックポインタ	スタックの位置を保持
P	8bit	ステータスレジスタ	CPUの各種状態を保持
PC	16bit	プログラムカウンタ	実行している位置を保持

7	N	ネガティブ	Aの7ビット目と同じになります。負数の判定用。
6	V	オーバーフロー	演算がオーバーフローを起こした場合セットされます。
5	R	予約済み	使用できません。常にセットされています。
4	B	ブレークモード	BRK発生時はセットされ、IRQ発生時はクリアされます。
3	D	デシマルモード	セットすると、BCDモードで動作します。(ファミコンでは未実装)
2	I	IRQ禁止	クリアするとIRQが許可され、セットするとIRQが禁止になります。
1	Z	ゼロ	演算結果が0になった場合セットされます。ロード命令でも変化します。
0	C	キャリー	キャリー発生時セットされます。
*/
