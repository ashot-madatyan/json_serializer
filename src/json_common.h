#ifndef JSON_COMMON_HEADER
#define JSON_COMMON_HEADER

#include <stdint.h>

using dictionary_value_type = int16_t;

enum class tlv_type : char
{
	TLVT_UNDEFINED = -1,
	TLVT_STRING = 2,
	TLVT_INT8 = 3,
	TLVT_INT16 = 4,
	TLVT_INT32 = 5,
	TLVT_INT64 = 6,
	TLVT_UINT8 = 7,
	TLVT_UINT16 = 8,
	TLVT_UINT32 = 9,
	TLVT_UINT64 = 10,
	TLVT_DOUBLE = 11
};

#endif // JSON_COMMON_HEADER
