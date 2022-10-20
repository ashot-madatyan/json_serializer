/***********************************************************************************************************

    The task is to read data in JSON format and using dictionary encoding write a compressed stream to
    disk.
    
    The code should compile with gcc on debian linux.
    You may use the following:
        • boost
        • some JSON parser library
        • cmake or simple make
    
    The input will contain records in JSON format. Each record is separated by a new line (LF). The
    input will only contain simple types, complex and nested data structures will not be present (i.e.
    map and array).
    Input example:
        { "key1":"value", "key2":42, "key3":TRUE}
        {"sadsf":"dsewtew", "dsre":3221, "sdfds":"dsfewew"}
    Basically a record is a list of KVPs (key-value pairs).
    
    Once the input is read, the program should encode the input so that the keys are put into a
    dictionary. The dictionary maps the keys (strings) to an integer. The key strings are then replaced
    with a number.
    The dictionary would be as follows for the first record:
    "key1":1, "key2":2, "key3":3
    This record is then represented as:
    { 1:"value", 2:42, 3:TRUE}
    
    Types supported by JSON must be handled and preserved (i.e. integer, string, boolean). The output
    should be dumped to a file in a binary form using TLV encoding instead of the above text-based
    representation. This binary TLV encoding is more compact, e.g. a boolean value can be encoded in
    two bytes (or even less).
    At the end of the process the dictionary must be also written to a file using the same encoding
    method for the string-integer pairs.
    
    The input file can be arbitrary large, make sure to use streaming instead of reading everything into
    memory. You can assume that the key space (number of distinct key strings) is small and regular inmemory
    structures (i.e. hashtable) can be used for the dictionary.
    
    While completing this task please try to show your skills (including C++11). Clear, readable and
    documented code is preferred. Avoid writing obfuscated code, speed should be a secondary
    objective. Feel free to add unit tests.
    Note: This simple assignment shows a very basic algorithm which might be useful to you in the
    project later but in no way can it be used by us as is.

***********************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "json_data_processor.h"
//#include "json_tlv_serializer.h"
//#include "json_raw_data_reader.h"
//#include "json_tlv_value.h"

using namespace std;

int tlv_test_not_init()
{
    try
    {
        tlv_data_serializer ds;
        uint64_t dat;
        size_t sz;
        sz = ds.read(&dat);
    }
    catch (const runtime_error& e)
    {
        printf("Exception: %s\n", e.what());
        return 0;
    }
    
    return -1;
}

/*
    First write a series of different types of data, then reset the backing file read position
    and try to read the same data  back and compare to the original ones.
*/
int tlv_test_write_read()
{
    try
    {
        string str = "ashot_madatyan";
        int kk = sizeof(str);
        
        tlv_data_serializer ds;
        uint8_t dat1 = 0x12, dat11;
        uint16_t dat2 = 0x1234, dat22;
        uint32_t dat3 = 0x12345678, dat33;
        uint64_t dat4 = 0x1234567812345678, dat44;
        
        ds.init("out.dat");
        ds.write(dat1);
        ds.write(dat2);
        ds.write(dat3);
        ds.write(dat4);

        ds.reset();

        ds.read(&dat11);
        ds.read(&dat22);
        ds.read(&dat33);
        ds.read(&dat44);

        if (!(dat1 == dat11 && dat2 == dat22 && dat3 == dat33))
            throw(runtime_error("'tlv_test_write_read' test failed"));
                
    }
    catch (const runtime_error& e)
    {
        printf("Exception: %s\n", e.what());
        return -1;
    }

    return 0;
}


int tlv_test_write_read_string()
{
    /*
       First write a string data, then reset the backing file read position
       and try to read the same data  back and compare to the original ones.
    */
    try
    {
        string str = "ashot_madatyan";
        string chk;

        int kk = sizeof(str);

        tlv_data_serializer ds;
        ds.init("out.dat");
        ds.write(str);
        
        int pos = -1;
        ds.reset(&pos);
        
        ds.read(&chk, str.size());

        if (!(str == chk))
            throw(runtime_error("'tlv_test_write_read_string' test failed"));

        //printf("read string: '%s'\n", chk.c_str());
    }
    catch (const runtime_error& e)
    {
        printf("Exception: %s\n", e.what());
        return -1;
    }

    return 0;
}

void test_raw_data_reader()
{
    raw_data_file_reader rdr;

    if (rdr.open("D:/test_projects/interview/nxlog/test_inp_file.txt"))
    {
        printf("failed to read the input file 'raw_json_data.txt'\n");
        return;
    }

    string s;
    int rv;
    while ((rv = rdr.read(s)) != -1)
    {
        if (rv > 0)
            printf("%s\n", s.c_str());
    }
}

void test_data_processor(const string& infile, const string& outfile)
{
    json_data_processor dp;
    dp.process(infile, outfile);
}

void test_tlv_value()
{
    //const std::string s("kuku");
    const char* s("kuku");
    //std::string s("kuku");
    tlv_value t1((uint8_t) 12);
    //tlv_value t2((uint16_t) 0x1234);
    //tlv_value tstr("madatyan");
    //tlv_value tstr(string("madatyan"));
    
    tlv_value tstr(s);
    tlv_value tt = tstr;
    tstr = tstr;


    // tlv_value k = "kuku"; // Unsupported DT

    //string ss = string((char*)tstr.data());
}

void usage(int argc, const char* argv[])
{
    printf("Usage: %s <input file> <output file>\n", argv[0]);
    exit(-1);
}

int main(int argc, const char* argv[])
{
    if (argc < 3)
        usage(argc, argv);

    try
    {
        //tlv_test_not_init();
        //tlv_test_write_read();
        //tlv_test_write_read_string();
        //test_raw_data_reader();
        test_data_processor(argv[1], argv[2]);

        //test_tlv_value();

        return 0;
    }
    catch (const runtime_error& e)
    {
        printf("Exception: %s\n", e.what());
        return -1;
    }
}
