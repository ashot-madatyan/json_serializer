#ifndef TLV_SERIALIZER_HEADER
#define TLV_SERIALIZER_HEADER

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "json_tlv_value.h"
#include "json_common.h"

using std::vector;
using std::map;

class tlv_data_serializer
{
public:
    using string = std::string;
    
    tlv_data_serializer() = default;
    ~tlv_data_serializer() { if (pf_) fclose(pf_); }

    /**
      Initialize the backing file.
      Open the file for both for binary writing and reading, create if not existing
    */
    int init(const string& fname) 
    {
        if (!(pf_ = fopen(fname.c_str(), "w+b")))   
            return -1;
        return 0;
    }
    
    int dump_map(const map<string, dictionary_value_type>& keys)
    {
        for (auto& [k, v] : keys)
        {
            tlv_value tlkey(k);
            tlv_value tlval(v);

            write_tlv_object(tlkey);
            write_tlv_object(tlval);
        }
        return 0;
    }

    int dump_to_file(const vector<tlv_value>& tlvs)
    {
        for (int i = 0; i < (int)tlvs.size(); ++i)
        {
            write_tlv_object(tlvs[i]);
        }

        return 0;
    }

    template <typename T>
    void write(const T& data)
    {
        if (!pf_)
            throw (runtime_error("No open output file to write"));
        
        size_t rv = fwrite((void*)&data, sizeof(T), 1, pf_);
    }

    template <typename T>
    size_t read(T* data, size_t sz = sizeof(T))
    {
        if (!pf_)
            throw (runtime_error("No open input file to read"));

        size_t rv = fread((void*)data, sz, 1, pf_);
        
#if defined(ADDITIONAL_READ_CHECKS)
        int ifeof, iferr;
        ifeof = feof(pf_);
        iferr = ferror(pf_);

        if (ifeof)
            throw (runtime_error("Attempt to read past the end of file"));
        if (iferr)
            throw (runtime_error("Error reading the backing file"));
#endif
        return rv;
    }

    /**
      Reset the backing file pointer.
      Used for initializing the 'read' operations from the beginning.
    */
    void reset(int* pos = nullptr)
    {
        if (pf_)
        {
            fseek(pf_, 0, SEEK_SET);
        }
        if (pos)
            *pos = (int)ftell(pf_);
    }

private:
    
    void write_tlv_object(const tlv_value& tl)
    {
        tlv_value::size_type sz = tl.size();
        const void* pdata = tl.data();
        tlv_type typ = tl.type();

        // Write: Type-Length-Value sequences
        raw_write_bytes((const void*)&typ, sizeof(std::underlying_type<tlv_type>));
        raw_write_bytes((const void*)&sz, sizeof(tlv_value::size_type));
        raw_write_bytes((const void*)pdata, sz);
    }
    
    void raw_write_bytes(const void* pdata, size_t sz)
    {
        if (!pf_)
            throw (std::runtime_error("No open output file to write"));

        size_t rv = fwrite((void*)pdata, 1, sz, pf_);
    }

private:
    FILE* pf_{nullptr};
};

template <>
void tlv_data_serializer::write<string>(const string& data)
{
    if (!pf_)
        throw (std::runtime_error("No open output file to write"));

    size_t szchar = sizeof(char);
    size_t szstr = data.size();

    size_t rv = fwrite((void*)data.c_str(), sizeof(char), data.size(), pf_);
}

template <> size_t tlv_data_serializer::read<string>(string* data, size_t sz)
{
    if (!pf_)
        throw (std::runtime_error("No open input file to read"));

    if (0 == sz)
        throw (std::runtime_error("Invalid size of string to read"));

    std::unique_ptr<char[]> pbuf(new char[sizeof(char) * (sz + 1)]{ 0 });
    size_t rv = fread((void*)pbuf.get(), sizeof(char), sz, pf_);
    *data = pbuf.get();

#if defined(ADDITIONAL_READ_CHECKS)
    int ifeof, iferr;
    ifeof = feof(pf_);
    iferr = ferror(pf_);

    if (ifeof)
        throw (runtime_error("Attempt to read past the end of file"));
    if (iferr)
        throw (runtime_error("Error reading the backing file"));
#endif
    return 0;
}

#endif // TLV_SERIALIZER_HEADER
