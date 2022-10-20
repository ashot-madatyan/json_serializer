#ifndef JSON_DATA_PROCESSOR_HEADER
#define JSON_DATA_PROCESSOR_HEADER

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "json.hpp"
#include "json_raw_data_reader.h"
#include "json_tlv_serializer.h"
#include "json_tlv_value.h"
#include "json_common.h"

#include <stdint.h>

using std::vector;
using std::map;

/**
  * This class is the core processing logic of the application.
  * It reads the input file stream line-by-line using the data reader logic and dispatches the 
  * further pocessing of the data to the Nlohman's header-only json library (json.hpp). 
  * Once the JSON procesing is done, this logic transforms and makes any required modifications
  * to the data, thus preparing for storing.
  * In the final stage of the proecessing this class uses the tlv_serializer to store the processed data
  * in the given output file.
*/
class json_data_processor
{
public:
    
    using string = std::string;
    using json = nlohmann::json;
    
    json_data_processor() = default;
    ~json_data_processor() { }

    int process(const string& input_file_name, const string& output_file_name)
    {
        raw_data_file_reader rdr;
        if (-1 == rdr.open(input_file_name))
        {
            printf("Failed to open the input file: '%s'\n", input_file_name.c_str());
            return -1;
        }

        tlv_data_serializer ds;
        if (-1 == ds.init(output_file_name))
        {
            printf("Failed to open the output file: '%s'\n", output_file_name.c_str());
            return -1;
        }

        string line;
        int rv;
        while ((rv = rdr.read(line)) != -1)
        {
            if (0 == rv)
                continue;

            json jst;
            //nlohmann::ordered_json jst;

            printf("----------------------------------------------------------------------------------------------------\n");

            if (-1 == parse_json_line(line, jst))
            {
                printf("Failed to process line '%s'\n", line.c_str());
                continue;
            }
            
            vector<tlv_value> tlv_vals;

            for (auto it = jst.begin(); it != jst.end(); ++it)
            {
                string stype, skey;
                tlv_value tlv;
                process_value(it, tlv, stype, skey);
                
                int32_t mapped_key = get_mapped_key(skey);
                tlv_value val_key(mapped_key);

                tlv_vals.push_back(val_key);
                tlv_vals.push_back(tlv);

                printf("%-60s key: %-12s mapped key: %d\n", 
                        tlv.as_string_rep().c_str()
                        , skey.c_str()
                        , mapped_key
                        );
#if 0
                if (tlv.type() == tlv_type::TLVT_STRING)
                {
                    string s = tlv.get_value_as<string>();
                    printf("type: %-5d %-12s key: %-8s value: %-20s mapped_key: %d\n", 
                                    tlv.type(), 
                                    stype.c_str(), 
                                    skey.c_str(),
                                    s.c_str(),
                                    mapped_key
                                    );
                }
                else
                {
                    uint64_t val = tlv.get_value_as<uint8_t>();
                    printf("type: %-5d %-12s key: %-8s value: %-20llu mapped_key: %d\n", 
                                    tlv.type(), 
                                    stype.c_str(), 
                                    skey.c_str(),
                                    val,
                                    mapped_key
                                    );

                }
#endif           //continue;
            }

            ds.dump_to_file(tlv_vals);
        }
        
        // Write the key mapping to the out put file
        ds.dump_map(map_keys_);

        return 0;
    }
    
private:

    void process_value(const json::iterator& js, tlv_value& tval, string& stype, string& skey)
    {
        skey = js.key();

        if (js->is_number_integer())
        {
            if (js.value() >= 0)
            {
                if (js.value() >= std::numeric_limits<uint8_t>::min() && js.value() <= std::numeric_limits<uint8_t>::max())
                {
                    stype = "uint8_t";
                    tval = tlv_value((uint8_t)js.value());
                }
                else if (js.value() >= std::numeric_limits<uint16_t>::min() && js.value() <= std::numeric_limits<uint16_t>::max())
                {
                    stype = "uint16_t";
                    tval = tlv_value((uint16_t)js.value());
                }
                else if (js.value() >= std::numeric_limits<uint32_t>::min() && js.value() <= std::numeric_limits<uint32_t>::max())
                {
                    stype = "uint32_t";

                    tval = tlv_value((uint32_t)js.value());
                }
                else if (js.value() >= std::numeric_limits<uint64_t>::min() && js.value() <= std::numeric_limits<uint64_t>::max())
                {
                    stype = "uint64_t";

                    tval = tlv_value((uint64_t)js.value());
                }
            }
            else
            {
                if (js.value() >= std::numeric_limits<int8_t>::min())
                {
                    stype = "int8_t";
                    tval = tlv_value((int8_t)js.value());
                }
                else if (js.value() >= std::numeric_limits<int16_t>::min())
                {
                    stype = "int16_t";
                    tval = tlv_value((int16_t)js.value());
                }
                else if (js.value() >= std::numeric_limits<int32_t>::min())
                {
                    stype = "int32_t";
                    tval = tlv_value((int32_t)js.value());
                }
                if (js.value() >= std::numeric_limits<int64_t>::min())
                {
                    stype = "int64_t";
                    tval = tlv_value((int64_t)js.value());
                }
            }
        }
        else if (js->is_number_float())
        {
            stype = "float";
            tval = tlv_value((double)js.value());
        }
        else if (js->is_string())
        {
            stype = "string";
            tval = tlv_value(string(js.value()));
        }
        else if (js->is_boolean())
        {
            stype = "boolean";
            tval = tlv_value(uint8_t(js.value()));
        }
        else
        {
            throw(runtime_error("Unsupported data type in json encountered"));
        }
    }

    int parse_json_line(const string& line, json& jst)
    {
        try
        {
            jst = json::parse(line);
        }
        catch (const runtime_error& e)
        {
            printf("Exception: %s\n", e.what());
            return -1;
        }

        //printf("successfully processed: '%s'\n", line.c_str());
        return 0;
    }

    // Not a thread-safe implementation.
    dictionary_value_type get_mapped_key(const string& skey)
    {
        static dictionary_value_type nxt_key = 0;
        
        if (map_keys_.find(skey) != map_keys_.end())
            return map_keys_[skey];

        ++nxt_key;
        map_keys_[skey] = nxt_key;
        return nxt_key;
    }

private:

    map<string, dictionary_value_type> map_keys_;
};

#endif // JSON_DATA_PROCESSOR_HEADER
