#ifndef BUSHU_H
#define BUSHU_H
// -------------------------------------------------------------------

#ifndef IMCRV
#include <iostream>
#include <fstream>
#endif
#include <windows.h>

using namespace std;

#ifndef IMCRV
#include "moji.h"
#include "tc.h"                 // TC_BUSHU_ALGO_{OKA,YAMANOBE}
#else
// from moji.h
typedef WCHAR MOJI;
// from bushu_dic.cpp for bushuent.cpp
#define MOJI_BUSHU_NL L'\n'
// from tc.h
#define TC_BUSHU_ALGO_OKA      1 // 岡アルゴリズム (tc.el)
#define TC_BUSHU_ALGO_YAMANOBE 2 // 山辺アルゴリズム [tcode-ml:2652]
#endif

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
#ifndef IMCRV
    BushuEnt *ent[BUSHUDIC_MAXENT];
    int nent;
#else
    static const BushuEnt ent[];
#endif

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
