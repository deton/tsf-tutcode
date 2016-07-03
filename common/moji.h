
//! サロゲートペア等を考慮して、count数前方の文字にidxを進める
size_t _ForwardMoji(const std::wstring &s, size_t idx, size_t count, BOOL ucs4 = FALSE);

//! サロゲートペア等を考慮して、count数後方の文字にidxを戻す
size_t _BackwardMoji(const std::wstring &s, size_t idx, size_t count);

//! サロゲートペア等を考慮してidx位置の1文字をコピー
size_t _Copy1Moji(const std::wstring &s, size_t idx, std::wstring *target);

//! サロゲートペア等を考慮してidx位置の1文字を取得
std::wstring _Get1Moji(const std::wstring &s, size_t idx);

//! サロゲートペア等を1文字とみなして、文字数を返す
size_t _CountMoji(const std::wstring &s);

//! サロゲートペアを1文字とみなして、UCS4での文字数を返す
size_t _CountMojiInUcs4(const std::wstring &s);
