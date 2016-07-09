
#define MOJIMB_NONE 0
#define MOJIMB_JIS_COMBINING_CHAR 2
#define MOJIMB_IVS 4
#define MOJIMB_FULL (MOJIMB_JIS_COMBINING_CHAR|MOJIMB_IVS)

//! サロゲートペア等を考慮して、count数前方の文字にidxを進める
size_t ForwardMoji(const std::wstring &s, size_t idx, size_t count, UINT mb = MOJIMB_FULL);

//! サロゲートペア等を考慮して、count数後方の文字にidxを戻す
size_t BackwardMoji(const std::wstring &s, size_t idx, size_t count);

//! サロゲートペア等を考慮してidx位置の1文字をコピー
size_t Copy1Moji(const std::wstring &s, size_t idx, std::wstring *target);

//! サロゲートペア等を考慮してidx位置の1文字を取得
std::wstring Get1Moji(const std::wstring &s, size_t idx);

//! サロゲートペア等を1文字とみなして、文字数を返す
size_t CountMoji(const std::wstring &s, UINT mb = MOJIMB_FULL);
