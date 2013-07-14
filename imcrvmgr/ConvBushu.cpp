
#include "imcrvmgr.h"
#include "kw/bushu_dic.h"

static BushuDic bushudic;

WCHAR ConvBushu(const std::wstring &bushu1, const std::wstring &bushu2)
{
	return bushudic.look(bushu1[0], bushu2[0], TC_BUSHU_ALGO_YAMANOBE);
}
