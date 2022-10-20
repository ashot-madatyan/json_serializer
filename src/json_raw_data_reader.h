#ifndef RAW_DATA_FILE_READER_HEADER
#define RAW_DATA_FILE_READER_HEADER

#include <stdio.h>
#include <string>
#include <memory>

/**
  *  This is a simple class for reading data strems of json formatted strings per each line.
*/
class raw_data_file_reader
{
public:
    
    using string = std::string;
    raw_data_file_reader() = default;
    ~raw_data_file_reader() { }

    int open(const string& fname) 
    {
        pf_.reset(fopen(fname.c_str(), "rb"), fclose);
        if (!pf_.get())
            return -1;
        return 0;
    }
    
    int read(string& data)
    {
        if (!pf_)
            throw (std::runtime_error("No open input file to read"));
        
        data.clear();
        static const int READ_BUF_SIZE = 1024;
        char buf[READ_BUF_SIZE];
        char* prv = fgets(buf, READ_BUF_SIZE, pf_.get());
        
        if (!prv)
            return -1;
        
        data = buf;
        int k = data.size() - 1;
        while (k >= 0 && (data[k] == '\n' || data[k] == '\r'))
        {
            data[k] = 0;
            --k;
        }

        data.resize(k + 1);
        return (int) data.size();
    }

private:
    std::shared_ptr<FILE> pf_;
};

#endif // RAW_DATA_FILE_READER_HEADER
