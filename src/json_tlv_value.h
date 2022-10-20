#ifndef JSON_TLV_VALUE_HEADER
#define JSON_TLV_VALUE_HEADER

#include <utility>
#include <exception>
#include <stdexcept>

#include <string>
#include <type_traits>
#include <stdint.h>
#include <cstring>
#include "json_common.h"

using namespace std;

class tlv_value
{
public:

	using string = std::string;
	using tlv_enum_type = std::underlying_type_t < tlv_type>;
	using size_type = uint32_t;

	tlv_value()
	{}

	template<typename T>
	tlv_value(const T& value) 
	{		
		if (is_same<T, uint8_t>())
		{
			init((void*)&value, sizeof(uint8_t), tlv_type::TLVT_UINT8);
		}
		else if (is_same<T, uint16_t>())
		{
			init((void*)&value, sizeof(uint16_t), tlv_type::TLVT_UINT16);
		}
		else if (is_same<T, uint32_t>())
		{
			init((void*)&value, sizeof(uint32_t), tlv_type::TLVT_UINT32);
		}
		else if (is_same<T, uint64_t>())
		{
			init((void*)&value, sizeof(uint64_t), tlv_type::TLVT_UINT64);
		}
		else if (is_same<T, int8_t>())
		{
			init((void*)&value, sizeof(int8_t), tlv_type::TLVT_INT8);
		}
		else if (is_same<T, int16_t>())
		{
			init((void*)&value, sizeof(int16_t), tlv_type::TLVT_INT16);
		}
		else if (is_same<T, int32_t>())
		{
			init((void*)&value, sizeof(int32_t), tlv_type::TLVT_INT32);
		}
		else if (is_same<T, int64_t>())
		{
			init((void*)&value, sizeof(int64_t), tlv_type::TLVT_INT64);
		}
		else if (is_same<T, float>())
		{
			init((void*)&value, sizeof(double), tlv_type::TLVT_DOUBLE);
		}
		else if (is_same<T, double>())
		{
			init((void*)&value, sizeof(double), tlv_type::TLVT_DOUBLE);
		}
		else if (is_same<T, std::string>())
		{
			const string& sref = (const string&)value;
			size_t sz = sref.size() + 1;
			init((void*)sref.c_str(), sz, tlv_type::TLVT_STRING);
		}
		else if (is_same<T, const char*>())
		{
			const char* pstr = (const char*)&value;
			size_t sz = strlen(pstr) + 1;
			init((void*)pstr, sz, tlv_type::TLVT_STRING);
		}
		else
		{
			throw(runtime_error("Unsopported data type or qualifier"));
		}
	}
	
	tlv_value(const tlv_value& rhs)
	{
		if (&rhs == this)
			return;

		this->operator=(rhs);
	}

	tlv_value& operator=(const tlv_value& rhs)
	{
		if (&rhs == this)
			return *this;

		if (0 == rhs.size() && size_)
		{
			delete[] (char*) pdata_;
			pdata_ = nullptr;
			size_ = 0;
			type_ = rhs.type();
			return *this;
		}

		if (pdata_)
		{
			delete[] (char*) pdata_;
			pdata_ = nullptr;
		}

		init(rhs.data(), rhs.size(), rhs.type());
		return *this;
	}

	~tlv_value() 
	{ 
		if (pdata_) 
		{
			delete[] (char*)pdata_;
		}
	}

	size_type size() const
	{
		return size_;
	}

	tlv_type type() const
	{
		return type_;
	}

	const void* data() const
	{
		return pdata_;
	}

	template <typename T>
	T get_value_as() const
	{
		if (0 == size_)
			throw(runtime_error("Object not initialized"));
		
		T rv;
		rv = *(T*)(pdata_);
		return rv;
	}


	string as_string_rep() const
	{
		string rv;
		char buffer[512]{0};

		// Format: "type: " <type> <type enum name> "value: " <value>

		switch (type_)
		{
		case tlv_type::TLVT_INT8:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %-10d", type_, "int8_t", size_, *(int8_t*)pdata_);
			break;
		
		case tlv_type::TLVT_INT16:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %-10d", type_, "int16_t", size_, *(int16_t*)pdata_);
			break;

		case tlv_type::TLVT_INT32:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %-10d", type_, "int32_t", size_, *(int32_t*)pdata_);
			break;
		case tlv_type::TLVT_INT64:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %-10lld", type_, "int64_t", size_, *(int64_t*)pdata_);
			break;

		case tlv_type::TLVT_UINT8:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %-10u", type_, "uint8_t", size_, *(uint8_t*)pdata_);
			break;

		case tlv_type::TLVT_UINT16:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %-10u", type_, "uint16_t", size_, *(uint16_t*)pdata_);
			break;

		case tlv_type::TLVT_UINT32:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %-10u", type_, "uint32_t", size_, *(uint32_t*)pdata_);
			break;
		case tlv_type::TLVT_UINT64:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %-10llu", type_, "uint64_t", size_, *(uint64_t*)pdata_);
			break;

		case tlv_type::TLVT_DOUBLE:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %f", type_, "double", size_, *(double*)pdata_);
			break;
		case tlv_type::TLVT_STRING:
			sprintf(buffer, "type: %-4d %-10s size: %-4u value: %-20s", type_, "string", size_, (char*)pdata_);
			break;
		default:
			rv = "- - - - - - ";
		}
		
		rv = buffer;
		return rv;
	}

private:
	
	void init(const void* psrc, size_t size, tlv_type type)
	{
		pdata_ = (void*) new char[size];
		type_ = type;
		size_ = size;
		memcpy(pdata_, psrc, size_);
	}

private:
	void*			pdata_{nullptr};
	size_type		size_{0};
	tlv_type		type_{tlv_type::TLVT_UNDEFINED};
};


template <>
string tlv_value::get_value_as<string>() const
{
	if (0 == size_)
		throw(std::runtime_error("Object not initialized"));

	string rv((char*)pdata_);
	return rv;
}

#endif // JSON_TLV_VALUE_HEADER
