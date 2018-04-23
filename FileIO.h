#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <string>
#include <map>
#include <cstdio>

#include "config.h"

#define MAX_FILE_SIZE 1000000000

// MATLAB Data Types:
#define miINT8    1
#define miUINT8   2
#define miINT16   3
#define miUINT16  4
#define miINT32   5
#define miUINT32  6
#define miSINGLE  7
#define miDOUBLE  9
#define miINT64  12
#define miUINT64 13
#define miMATRIX 14

// Following sizes are in bytes:

#define MAT_HEADER_SIZE    128
#define MIMATRIX_TAG_SIZE    8
#define ARRAY_FLAGS_SIZE    16
#define DIM_ARRAY_SIZE      16
#define ARRAY_NAME_TAG_SIZE  8
#define RE_TAG_SIZE          8

namespace FileIO
{
    typedef std::map<std::string,int>   str_int_map;
    typedef unsigned long long u64;
    typedef std::map<std::string,void*> str_mem_map;

    typedef struct
    {
        void* address;
        int origin;
        int miType;
        int dim;
        int matsize;
        int fileNo;
        std::string name;
    } Parameter;

    class MatFile
    {
    public:
        MatFile(const std::string& name, int size);
        MatFile(const MatFile& other);
        ~MatFile();
        bool fseek(int offset, int origin);
        int ftell(void) const;
        int write(const void* src, int size, int numel, int offset);

        std::FILE* fp;

    private:
        int _filesize;
        //std::FILE* fp;
        std::string _name;
    };

    void assignTypeAddr(str_int_map& var2type, str_mem_map& var2addr);
    bool checkDeleteFile( const std::string& file );
    bool deleteFile(const std::string& name, int timeout);
    void printOutputs(void);
    bool readVariables( const Config::Configurator& config,
                        const str_int_map& var2type,
                        str_int_map& var2dim );
    bool setupOutputFiles( const Config::Configurator& config,
                           std::vector<MatFile>& files );
    void updateOuputFile( int step, std::vector<MatFile>& files );
    bool writeMetaData( const Config::Configurator& config,
                        const str_mem_map& var2addr,
                        std::vector<MatFile>& files );
}

// Code that gets used a few times:
#define writeToFile()                                 \
{                                                     \
    numel = 0;                                        \
    for (int n = 0; n < dword; n++)                   \
        numel += files[i].write(&_4bytes[n],          \
                                sizeof(int),          \
                                1, -1);               \
    if (numel != dword)                               \
    {                                                 \
        std::cout << "error writing to output file "  \
            << i << std::endl;                        \
        std::raise(SIGABRT);                          \
    }                                                 \
}

#endif // __FILEIO_H__
