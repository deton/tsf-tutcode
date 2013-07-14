#ifndef BUSHU_H
#define BUSHU_H
// -------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

#include "moji.h"
#include "tc.h"                 // TC_BUSHU_ALGO_{OKA,YAMANOBE}

// 部首合成辞書 bushu.rev の中の行の意味:
// * "CAB" → C := A + B (例: "茜サ西")
// * "BC"  → B ≡ C     (例: "院ア")
// * ↑間違えた……正しくは
// * "CB" → B ≡ C      (例: "院ア")
struct BushuEnt {
    MOJI a;
    MOJI b;
    MOJI c;
};

#define BUSHUDIC_MAXENT 16000

class BushuDic {
public:
    BushuEnt *ent[BUSHUDIC_MAXENT];
    int nent;

    BushuDic();
    ~BushuDic();

    void readFile(ifstream *);
    MOJI look(MOJI, MOJI);
    MOJI look(MOJI, MOJI, int);
private:
    int decompose(MOJI, MOJI &, MOJI &);
    MOJI alternative(MOJI);
    MOJI lookSub(MOJI, MOJI);
};

// -------------------------------------------------------------------
#endif // BUSHU_H
