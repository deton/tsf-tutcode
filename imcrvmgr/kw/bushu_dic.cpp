/* $Id: bushu_dic.c,v 1.6 2006/03/25 10:22:17 yuse Exp $ */

/* comp.c from tserv-0.2 <http://www.tcp-ip.or.jp/~tagawa/archive/>.
 * modified for 漢直Win.
 */

/*
 *                    Copyright (C) Shingo NISHIOKA, 1992
 *                       nishioka@sanken.osaka-u.ac.jp
 *            Everyone is permitted to do anything on this program
 *         including copying, transplanting, debugging, and modifying.
 */

/*
  このプログラムの原型は, tc.el から作られました.  tc.el の Copyright
  は以下の通りです.
;;
;; T-Code frontend for Nemacs.
;; Author : Yasushi Saito (yasushi@is.s.u-tokyo.ac.jp)
;;          Kaoru Maeda   (kaoru@is.s.u-tokyo.ac.jp)
;;
;; Department of Information Science
;; Faculty of Science, University of Tokyo
;; 7-3-1 Hongo, Bunkyo-ku, 113 Tokyo, Japan
;;
;; Copyright (C) 1989, 1990, 1991 Yasushi Saito and Kaoru Maeda.
;;

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License version 1
;; as published by the Free Software Foundation.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "bushu_dic.h"
#ifndef IMCRV
#include "debug.h"
#endif

#ifndef IMCRV
#define MOJI_BUSHU_NL B2MOJI('N', 'L')
//#define MOJI_BUSHU_NL 0
#endif

BushuDic::BushuDic() {
#ifndef IMCRV
    nent = 0;
#endif
}

BushuDic::~BushuDic() {
#ifndef IMCRV
    for (int i = 0; i < nent; i++) { delete(ent[i]); }
#endif
}

#ifndef IMCRV
void BushuDic::readFile(ifstream *is) {
    // 作業領域
    char buffer[2048];
    //int line = 1;

    for (nent = 0; !(is->eof()) && nent < BUSHUDIC_MAXENT; // XXX
         nent++) {
        // get one line
        is->getline(buffer, sizeof(buffer));
        if (*buffer == 0) { break; } // XXX; ?
        char *s = buffer;
        // "CAB" → C := A + B
        MOJI c = str2moji(s, &s);
        MOJI a = str2moji(s, &s);
        MOJI b = str2moji(s, &s);
        // "BC" → B ≡ C
        // ↑間違えた……正しくは
        // "CB" → B ≡ C
#ifdef _MSC_VER //<OKA>
        if (b == MOJI('\r') || b == MOJI('\n') || b == 0) { // XXX
#else
        if (b == MOJI(0, '\r') || b == MOJI(0, '\n') || b == 0) { // XXX
#endif          //</OKA>
            b = c;
            c = a;
            a = MOJI_BUSHU_NL;
        }
        ent[nent] = new BushuEnt;
        ent[nent]->a = a;
        ent[nent]->b = b;
        ent[nent]->c = c;
    }
}
#endif // !IMCRV

// kanji を 2 つの部品に分解する
// 成功したら 1 を、失敗したら 0 を返す
int BushuDic::decompose(MOJI kanji, MOJI &c1, MOJI &c2) {
#ifndef IMCRV
    for (int i = 0; i < nent; i++) {
        if (ent[i]->c == kanji) {
            c1 = ent[i]->a;
            c2 = ent[i]->b;
#else
    for (int i = 0; ent[i].c != L'\0'; i++) {
        if (ent[i].c == kanji) {
            c1 = ent[i].a;
            c2 = ent[i].b;
#endif
            return 1;
        }
    }
    // not found
    //c1 = c2 = 0;
    c1 = c2 = MOJI_BUSHU_NL;
    return 0;
}

// c と等価な文字 eqc を返す。特になければ c 自身を返す。
// 等価であるとは c := NL + eqc と定義されていることである。
MOJI BushuDic::alternative(MOJI c) {
    MOJI a, b;

    if (decompose(c, a, b) != 0 && a == MOJI_BUSHU_NL) {
        return b;
    }
    return c;
}

// a と b を直接組み合わせてできる外字を探す。
// 見つからなかった場合は 0 を返す。XXX
MOJI BushuDic::lookSub(MOJI a, MOJI b) {
#ifndef IMCRV
    for (int i = 0; i < nent; i++) {
        if (ent[i]->a == a && ent[i]->b == b) {
            // ここで a と b を入れ換えて合成可能な
            // ent[i]->a == b && ent[i]->b == a
            // という状況は考えない
            return ent[i]->c;
#else
    for (int i = 0; ent[i].c != L'\0'; i++) {
        if (ent[i].a == a && ent[i].b == b) {
            // ここで a と b を入れ換えて合成可能な
            // ent[i].a == b && ent[i].b == a
            // という状況は考えない
            return ent[i].c;
#endif
        }
    }
    return 0;
}

// a と b を組み合わせてできる外字を探す。
// 見つからなかった場合は 0 を返す。XXX
//
// 以下のコードは、
// 岡 俊行 (OKA Toshiyuki) <oka@nova.co.jp>
// さんによるアルゴリズムを用いたもの。
MOJI BushuDic::look(MOJI ca, MOJI cb) {
    return look(ca, cb, TC_BUSHU_ALGO_OKA);
}

MOJI BushuDic::look(MOJI ca, MOJI cb, int bushu_algo) {
    // このルーチンでは、合成後の文字が元の文字に戻らないよう
    // 注意しなければならない
#define RETURN(x) if (x != ca && x != cb) return x

    // 全角文字で半角に直せる文字は半角にしておく
    // XXX (そのうち書く予定)
    // ……と思ったが、bushu.rev に
    // > 3３
    // のような等価定義を書いて対処してもらうことにした。

    // 等価な部品を用意しておく
    MOJI a = alternative(ca);
    MOJI b = alternative(cb);
    // さらに、分解しておく
    MOJI a1, a2, b1, b2;
    decompose(a, a1, a2);       // a := a1 + a2
    decompose(b, b1, b2);       // b := b1 + b2

    // NL は避ける
    if (a1 == MOJI_BUSHU_NL) { a1 = 0; }
    if (a2 == MOJI_BUSHU_NL) { a2 = 0; }
    if (b1 == MOJI_BUSHU_NL) { b1 = 0; }
    if (b2 == MOJI_BUSHU_NL) { b2 = 0; }

    MOJI a11, a12, a21, a22;

    for (int i = 0; i < 2; i++) {
        MOJI r;
        // まず、足し算
        if ((r = lookSub(ca, cb)) != 0) RETURN(r);

        // 等価文字同士で足し算
        if ((r = lookSub(a, b)) != 0) RETURN(r);
#define lookSub_L2R(x, y, z) ((r = lookSub(x, y)) != 0 && (r = lookSub(r, z)) != 0)
#define lookSub_R2L(x, y, z) ((r = lookSub(y, z)) != 0 && (r = lookSub(x, r)) != 0)
        // YAMANOBE algorithm
        if (bushu_algo == TC_BUSHU_ALGO_YAMANOBE) {
            if (a1 && a2) {
                if (lookSub_L2R(a1, b, a2)) RETURN(r);
                if (lookSub_R2L(a1, a2, b)) RETURN(r);
            }
            if (b1 && b2) {
                if (lookSub_L2R(a, b1, b2)) RETURN(r);
                if (lookSub_R2L(b1, a, b2)) RETURN(r);
            }
        }

        // 次は、引き算
        if (a2 && a1 && a2 == b) RETURN(a1);
        if (a1 && a2 && a1 == b) RETURN(a2);
        // YAMANOBE algorithm
        if (bushu_algo == TC_BUSHU_ALGO_YAMANOBE) {
            a11 = a12 = a21 = a22 = 0;
            if (a1) decompose(a1, a11, a12);
            if (a2) decompose(a2, a21, a22);
            if (a11 == MOJI_BUSHU_NL) { a11 = 0; }
            if (a12 == MOJI_BUSHU_NL) { a12 = 0; }
            if (a21 == MOJI_BUSHU_NL) { a21 = 0; }
            if (a22 == MOJI_BUSHU_NL) { a22 = 0; }

            if (a2 && a11 && a12) {
                if (a12 == b && (r = lookSub(a11, a2)) != 0) RETURN(r);
                if (a11 == b && (r = lookSub(a12, a2)) != 0) RETURN(r);
            }
            if (a1 && a21 && a22) {
                if (a22 == b && (r = lookSub(a1, a21)) != 0) RETURN(r);
                if (a21 == b && (r = lookSub(a1, a22)) != 0) RETURN(r);
            }
        }

        // 一方が部品による足し算
        if (a && b1 && (r = lookSub(a, b1)) != 0) RETURN(r);
        if (a && b2 && (r = lookSub(a, b2)) != 0) RETURN(r);
        if (a1 && b && (r = lookSub(a1, b)) != 0) RETURN(r);
        if (a2 && b && (r = lookSub(a2, b)) != 0) RETURN(r);
        // YAMANOBE algorithm
        if (bushu_algo == TC_BUSHU_ALGO_YAMANOBE) {
            if (a1 && a2 && b1) {
                if (lookSub_L2R(a1, b1, a2)) RETURN(r);
                if (lookSub_R2L(a1, a2, b1)) RETURN(r);
            }
            if (a1 && a2 && b2) {
                if (lookSub_L2R(a1, b2, a2)) RETURN(r);
                if (lookSub_R2L(a1, a2, b2)) RETURN(r);
            }
            if (a1 && b1 && b2) {
                if (lookSub_L2R(a1, b1, b2)) RETURN(r);
                if (lookSub_R2L(b1, a1, b2)) RETURN(r);
            }
            if (a2 && b1 && b2) {
                if (lookSub_L2R(a2, b1, b2)) RETURN(r);
                if (lookSub_R2L(b1, a2, b2)) RETURN(r);
            }
        }

        // 両方が部品による足し算
        if (a1 && b1 && (r = lookSub(a1, b1)) != 0) RETURN(r);
        if (a1 && b2 && (r = lookSub(a1, b2)) != 0) RETURN(r);
        if (a2 && b1 && (r = lookSub(a2, b1)) != 0) RETURN(r);
        if (a2 && b2 && (r = lookSub(a2, b2)) != 0) RETURN(r);

        // 部品による引き算
        if (a2 && b1 && a2 == b1) RETURN(a1);
        if (a2 && b2 && a2 == b2) RETURN(a1);
        if (a1 && b1 && a1 == b1) RETURN(a2);
        if (a1 && b2 && a1 == b2) RETURN(a2);
        if (bushu_algo == TC_BUSHU_ALGO_YAMANOBE) {
            if (a2 && a11 && a12) {
                if ((a12 == b1 || a12 == b2) && (r = lookSub(a11, a2)) != 0) RETURN(r);
                if ((a11 == b1 || a11 == b2) && (r = lookSub(a12, a2)) != 0) RETURN(r);
            }
            if (a1 && a21 && a22) {
                if ((a22 == b1 || a22 == b2) && (r = lookSub(a1, a21)) != 0) RETURN(r);
                if ((a21 == b1 || a21 == b2) && (r = lookSub(a1, a22)) != 0) RETURN(r);
            }
        }

        // 文字の順序を逆にしてみる
        MOJI t = ca; ca = cb, cb = t;
        t = a, a = b, b = t;
        t = a1, a1 = b1, b1 = t;
        t = a2, a2 = b2, b2 = t;
    }
    return 0;
}
#undef RETURN
#undef MOJI_BUSHU_NL

