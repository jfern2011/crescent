#include "FileIO.h"
#include "util.h"
#include "config.h"
#include "dynamics.h"

#include <windows.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <csignal>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

/*
 **************************************************************
 *
 * Mapping from MATLAB data type to array type. Array types can
 * be obtained implicitly via the data types they carry, e.g.
 * miDOUBLE <=> mxDOUBLE_CLASS because more complex arrays such
 * as cell arrays are not handled
 *
 **************************************************************
 */
static const int mi2mx[16] = {  0,  8,  9, 10,
                               11, 12, 13,  7,
                                0,  6,  0,  0,
                               14, 15,  0,  0 };

// Size of MATLAB data types:
static const int mi_sizeof[16] = { 0, 1, 1, 2,
                                   2, 4, 4, 4,
                                   0, 8, 0, 0,
                                   8, 8, 0, 0 };

namespace FileIO
{
    std::vector<Parameter> params;

    /**
     ******************************************************************
     *
     * @brief Constructs a MatFile object for writing
     *
     * @details Creates a MAT file with the given name with the desired
     *          size. Because of the nature of MAT files, disk space
     *          must be preallocated because data for individual output
     *          variables is contiguous
     *
     * @param[in] name Filename
     * @param[in] size The size of the file, in bytes
     *
     ******************************************************************
     */
    MatFile::MatFile(const std::string& name, int size)
        : _filesize(size), _name(name)
    {
        ///////////////////////////////////////////////////////////////
        //
        // 1. Create the file
        //
        ///////////////////////////////////////////////////////////////
        HANDLE handle =
            CreateFile(name.c_str(),                       // Name of the file
                       GENERIC_WRITE,                      // Open for writing
                       FILE_SHARE_READ | FILE_SHARE_WRITE, // File sharing
                       NULL,                               // Default security
                       CREATE_NEW,                         // New file only
                       FILE_ATTRIBUTE_NORMAL,              // Regular file (?)
                       NULL);                              // ??

        if (handle == INVALID_HANDLE_VALUE)
        {
            std::cout << "error creating file: "
                << name << "(error " << GetLastError() << ")"
                << std::endl;
            std::raise(SIGABRT);
        }

        ///////////////////////////////////////////////////////////////
        //
        // 2. Set the file size
        //
        ///////////////////////////////////////////////////////////////

        if (SetFilePointer(handle, size, NULL, FILE_BEGIN)
            == INVALID_SET_FILE_POINTER)
        {
            std::cout << "allocating disk space: "
                << name << "(error " << GetLastError() << ")"
                << std::endl;
            std::raise(SIGABRT);
        }

        SetEndOfFile(handle);
        CloseHandle (handle);

        ///////////////////////////////////////////////////////////////
        //
        // 3. Open for writing
        //
        ///////////////////////////////////////////////////////////////

        fp = std::fopen( name.c_str(), "wb" );

        if (fp == NULL)
        {
            std::string msg("error opening ");
            msg += name;
            std::perror(msg.c_str());
                std::raise( SIGABRT);
        }

#if defined (DEBUG_IO)
        std::cout << "opened file for writing: "
            << fp << std::endl;
#endif // defined
    }

    /**
     ******************************************************************
     *
     * @brief Copy constructor
     *
     * @details Creates a copy of the given MatFile object. Instead of
     *          having both objects share the same same file stream,
     *          the newly created object will have its own stream with
     *          the same file
     *
     * @param[in] other The MatFile to copy
     *
     ******************************************************************
     */
    MatFile::MatFile(const MatFile& other)
    {
        _name = other._name;
        fp = fopen(_name.c_str(), "wb");

        if (fp == NULL)
        {
            std::string msg("error opening ");
            msg += _name;
            std::perror(msg.c_str());
                          std::raise(SIGABRT);
        }

        _filesize =
            other._filesize;

#if defined (DEBUG_IO)
        std::cout << "opened file for writing: "
            << fp << std::endl;
#endif // defined
    }

    /**
     ******************************************************************
     *
     * Destructor
     *
     ******************************************************************
     */
    MatFile::~MatFile()
    {
#if defined (DEBUG_IO)
        std::cout << "closing file: " << fp << std::endl;
#endif // defined

        if (fp)
        {
            std::fclose(fp); fp = NULL;
        }
    }

    /**
     ******************************************************************
     *
     * Wrapper to the C standard fseek() function
     *
     * @param[in] offset Number of bytes to offset from "origin"
     * @param[in] origin Position used as a reference for "offset"
     *
     * @return True on success
     *
     ******************************************************************
     */
    bool MatFile::fseek(int offset, int origin)
    {
        if ( std::fseek(fp, offset, origin))
        {
            std::perror(NULL); return false;
        }

        return true;
    }

    /**
     ******************************************************************
     *
     * Retrieve the stream position indicator (number of bytes from
     * beginning of file)
     *
     * @return The stream position
     *
     ******************************************************************
     */
    int MatFile::ftell(void) const
    {
        return std::ftell(fp);
    }

    /**
     ******************************************************************
     *
     * Write to the file
     *
     * @param[in] src    The memory address to write from
     * @param[in] size   The size, in bytes, of each element to write
     * @param[in] numel  The number of elements to write
     * @param[in] offset The offset from the beginning of file from
     *                   which to start writing. If this is less
     *                   than zero, it is not used, i.e. the file
     *                   is written to starting at the current stream
     *                   position
     *
     * @return The number of elements written
     *
     ******************************************************************
     */
    int MatFile::write(const void* src, int size, int numel,
                       int offset)
    {
        if (offset >= 0)
        {
            if (std::fseek(fp, offset, SEEK_SET))
            {
#if defined (DEBUG_IO)
                std::cout << "error writing to: "
                    << fp << std::endl;
#endif // defined
                std::perror("error using fseek()");
                return 0;
            }
        }

#if defined (DEBUG_IO)
        std::cout << "writing to file: " << fp << std::endl;
#endif // defined

        int _numel =
            std::fwrite(src, size, numel, fp);

        if (_numel != numel)
        {
            std::cout << "warning: wrote " << _numel
                << " of " << numel
                        << " elements "<< std::endl;
            std::perror(NULL);
        }

        return _numel;
    }

    /**
     ******************************************************************
     *
     * Assign output parameters their location in memory and their
     * MATLAB type
     *
     * @param[out] var2type Mapping from variable name to MATLAB
     *                      type
     * @param[out] var2addr Mapping from variable name to address
     *
     ******************************************************************
     */
    void assignTypeAddr(str_int_map& var2type, str_mem_map& var2addr)
    {
        var2addr["time"] = &Dynamics::simulationTime;
        var2type["time"] = miDOUBLE;

        std::string varName;

        for ( size_t i = 0; i < Dynamics::bodies.size(); i++ )
        {
            /* J2000 position vector */
            varName = Dynamics::bodies[i].getName() + "_pos_j2000";
            var2addr[varName] = &Dynamics::bodies[i]._state(0);
            var2type[varName] = miDOUBLE;

            /* J2000 velocity vector */
            varName = Dynamics::bodies[i].getName() + "_vel_j2000";
            var2addr[varName] = &Dynamics::bodies[i]._state(3);
            var2type[varName] = miDOUBLE;
        }
    }

    /**
     ******************************************************************
     *
     * Confirm file deletion. If the user elects not to delete, this
     * will terminate the program
     *
     * @param[in] file The file to delete
     *
     * @return True if the file was deleted, or if the file does not
     *         exist
     *
     ******************************************************************
     */
    bool checkDeleteFile( const std::string& file )
    {
        char resp;
        if (Util::dirExists(file))
        {
            do
            {
                std::cout << file << " exists. Overwrite (Y/N)? ";
                std::cin >> resp;
                if (resp == 'N' || resp == 'n')
                {
                    return false;
                }
                if (resp == 'Y' || resp == 'y')
                {
                    if (!FileIO::deleteFile(file, 2000))
                    {
                        std::cout << "delete failed: " << file
                            << std::endl;
                        std::raise(SIGABRT);
                    }
                    break;
                }
            } while (1);
        }

        return true;
    }

    /**
     ******************************************************************
     *
     * Delete a regular file. If the file is not found it is treated
     * as though it was successfully deleted
     *
     * @param[in] name    The name of the file to be deleted
     * @param[in] timeout The maximum number of milliseconds to wait
     *                    for deletion
     *
     * @return True if the file is deleted before the timeout period
     *
     ******************************************************************
     */
    bool deleteFile(const std::string& name, int timeout)
    {
        int time = 0;

        if (Util::dirExists(name))
        {
            if (std::remove(name.c_str()))
                return false;
        }
        else
            return true;

        // Wait at most timeout ms for deletion
        // to occur:
        while (Util::dirExists(name))
        {
            Sleep(200); time += 200;
            if (timeout <= time)
                        return false;
        }

        return true;
    }

    /**
     ******************************************************************
     *
     * Prints all output variables
     *
     ******************************************************************
     */
    void printOutputs()
    {
        for (size_t i = 0; i < params.size(); i++)
        {
            std::cout << "name:    " << params[i].name << "\n";
            std::cout << "address: " << params[i].address << "\n";
            std::cout << "origin:  " << params[i].origin << "\n";
            std::cout << "miType:  " << params[i].miType << "\n";
            std::cout << "dim:     " << params[i].dim << "\n";
            std::cout << "matsize: " << params[i].matsize << "\n";
            std::cout << "fileNo:  " << params[i].fileNo << "\n"
                << std::endl;
        }
    }

    /**
     ******************************************************************
     *
     * Read the variables input file. An error occurs if a variable is
     * requested that is not an output
     *
     * @param[in] config   A Configurator that stores user options
     * @param[in] var2type Mapping from variable name to MATLAB
     *                     type
     * @param[out] var2dim Mapping from variable name to dimension
     *
     ******************************************************************
     */
    bool readVariables( const Config::Configurator& config,
                        const str_int_map& var2type,
                        str_int_map& var2dim )
    {
        Util::str_v args;
        if (!config.lookup("variables", args))
        {
            std::cout << "error: variables file does not exist"
                << std::endl;
            return false;
        }

        std::ifstream infile(args[0].c_str());

        if (!infile.is_open())
        {
            std::cout << "unable to open: " << args[0]
                << std::endl;
            return false;
        }

        std::string line, varName;
        int dim, lineNo = 0;

        while (std::getline(infile, line))
        {
            lineNo++;

            line = Util::trim(Util::removeComment(line));
            std::istringstream iss(line);
            iss.peek();
            if (iss.eof()) continue;

            if (!(iss >> varName >> dim))
            {
                std::cout << "error parsing " << args[0]
                    << " at line "
                    << lineNo << std::endl;
                return false;
            }

            if (var2dim.find(varName) != var2dim.end())
            {
                std::cout << "error: variable \""
                    << varName << "\" is listed more than once"
                    << std::endl;
                return false;
            }

            if (var2type.find(varName) == var2type.end())
            {
                std::cout << "error: \"" << varName
                    << "\" is not available for output"
                    << std::endl;
                return false;
            }

            var2dim[varName] = dim;
        }

        return true;
    }

    /**
     ******************************************************************
     *
     * Set up the output file(s). If a single file is determined not to
     * be large enough for all data without exceeding the file size
     * limit then multiple files may be created, each not exceeding the
     * maximum file size
     *
     * @param[in]  config A Configurator that stores user options
     * @param[out] files  A vector of MAT files that were created
     *
     ******************************************************************
     */
    bool setupOutputFiles( const Config::Configurator& config,
                           std::vector<MatFile>& files )
    {
        str_int_map var2dim, var2type;
        str_mem_map var2addr;

        assignTypeAddr(var2type, var2addr);

        // Read the variables file:
        if (!readVariables(config, var2type, var2dim))
            return false;

        Parameter temp;

        // Initial default values:
        temp.address = NULL;
        temp.origin  = -1;
        temp.fileNo  = -1;
        temp.matsize = -1;

        for (str_int_map::iterator it = var2dim.begin();
             it != var2dim.end(); ++it)
        {
            temp.dim    = it->second;
            temp.name   = it->first;
            temp.miType = var2type[temp.name];

            params.push_back(temp);
        }

        // Determine the number of bytes required per
        // parameter
        int bytes, cumbytes = MAT_HEADER_SIZE;
        char outFile[32];
        int rem, datalen, nFiles = 0;

        for ( size_t i = 0; i < params.size(); i++ )
        {
            bytes = MIMATRIX_TAG_SIZE   +
                    ARRAY_FLAGS_SIZE    +
                    DIM_ARRAY_SIZE      +
                    ARRAY_NAME_TAG_SIZE +
                    params[i].name.size();

            // Ensure the # of bytes required for the name aligns on
            // an 8-byte boundary:
            rem = params[i].name.size() % 8;
            if (rem)
                bytes += (8 - rem);

            bytes += RE_TAG_SIZE;

            datalen = config.getNumOfTimeSteps()
                      * params[i].dim
                      * mi_sizeof[params[i].miType];

            bytes += datalen;

            // Ensure the # of bytes required for the data aligns on
            // an 8-byte boundary:
            rem = datalen % 8;
            if (rem) bytes += (8 - rem);

            cumbytes += bytes;

            params[i].matsize = bytes;
            params[i].fileNo = nFiles;

            if ( cumbytes > MAX_FILE_SIZE
                && (i+1) < params.size() )
            {
                sprintf( outFile, "simout_%d.mat", nFiles );
                if (!checkDeleteFile(outFile))
                    return false;
                files.push_back(MatFile(outFile, cumbytes));
                nFiles++;
                cumbytes =
                    MAT_HEADER_SIZE;
            }
        }

        // Create last file:
        sprintf( outFile , "simout_%d.mat",
                nFiles);
        if (!checkDeleteFile(outFile))
            return false;
        files.push_back(MatFile(outFile,
                                cumbytes));

        if (!writeMetaData(config,
                           var2addr, files))
            return false;

        return true;
    }

    /**
     ******************************************************************
     *
     * Update the data file(s) with current values of all outputs
     *
     * @param[in] step  Current time step
     * @param[in] files List of MAT files that will be written to
     *
     ******************************************************************
     */
    void updateOuputFile( int step, std::vector<MatFile>& files )
    {
        register int offset;

        for (size_t i = 0; i < params.size(); i++)
        {
            offset = params[i].origin +
                step * mi_sizeof[params[i].miType] * params[i].dim;

            files[params[i].fileNo].write(params[i].address,
                                          mi_sizeof[params[i].miType],
                                          params[i].dim,
                                          offset);
        }
    }

    /**
     ******************************************************************
     *
     * Write the meta data for each parameter to the MAT file that it
     * belongs to. This function is called by setupOutputFiles()
     *
     * @param[in]  config A Configurator containing user options
     * @param[out] files  A vector of MAT files to be written to
     *
     ******************************************************************
     */
    bool writeMetaData( const Config::Configurator& config,
                        const str_mem_map& var2addr,
                        std::vector<MatFile>& files )
    {
        char header[124], arrayFlags[4];
        int _4bytes[16], dataBytes;
        int _8bytes, rem, numel, dword;

        const short version = 0x0100;
        const short endian  =
            (((short)'M') << 8) | 'I';

        for (size_t i = 0; i < files.size(); i++)
        {
            // Write the MAT file header:
            std::memset(header, 0, 124);

            sprintf(header, "Simulation Data File %d of %d\n",
                    i+1, files.size());
            strcat(header, "Format: MATLAB 5.0 MAT file  \n");

            numel  = files[i].write(header,
                                    sizeof(char), 124, -1);
            numel += files[i].write(&version,
                                    sizeof(short),  1, -1);
            numel += files[i].write(&endian,
                                    sizeof(short),  1, -1);

            if ( numel != 126 )
            {
                std::cout <<  "error writing to output file "
                    << i << std::endl;
                return false;
            }

            for (size_t j = 0; j < params.size(); j++)
            {
                // If this parameter belongs to this file, write its
                // metadata:
                if (params[j].fileNo == (int)i)
                {
                    // Write the Matrix tag (size excludes
                    // the tag itself):
                    dword = 0;
                    _4bytes[dword++] = miMATRIX;
                    _4bytes[dword++] = params[j].matsize-8;

                    // Write Array flags:
                    _4bytes[dword++] = miUINT32;
                    _4bytes[dword++] = 8;

                    // Write these items now since the Array Flags
                    // are written byte-wise
                    writeToFile();

                    memset(arrayFlags, 0, 4);
                    arrayFlags[0] = mi2mx[params[j].miType];

                    numel = 0;
                    for (int n = 0; n < 4; n++)
                    {
                        numel +=
                            files[i].write(&arrayFlags[n],
                                           sizeof(char), 1, -1);
                    }

                    if (numel != 4)
                    {
                        std::cout <<  "error writing to output file "
                            << i << std::endl;
                        return false;
                    }

                    dword = 0;
                    _4bytes[dword++] = 0;

                    // Write Dimensions Array subelement:
                    _4bytes[dword++] = miINT32;
                    _4bytes[dword++] = 8;
                    _4bytes[dword++] = params[j].dim;
                    _4bytes[dword++] =
                            config.getNumOfTimeSteps();

                    // Write the Array Name:
                    _4bytes[dword++] = miUINT8;
                    _4bytes[dword++] =
                                 params[j].name.size();

                    // Write these items now since the variable name
                    // is written byte-wise
                    writeToFile();

                    if (files[i].write(params[j].name.c_str(),
                                       sizeof(char),
                                       params[j].name.size() ,
                                       -1)
                        != (int)params[j].name.size())
                    {
                        std::cout << "error writing to output file "
                            << i << std::endl;
                        return false;
                    }

                    rem = params[j].name.size() % 8;
                    if (rem)
                    {
                        // Add padding to the name to make sure it aligns
                        // on an 8-byte boundary:
                        memset(&_8bytes, 0, 8);
                        if (files[i].write(&_8bytes,
                                           sizeof(char), 8-rem, -1)
                            != 8-rem)
                        {
                            std::cout  <<  "error writing to output file "
                                << i << std::endl;
                            return false;
                        }
                    }

                    //  Write the Real Part Subelement (tag):
                    dword = 0;
                    _4bytes[dword++] = params[j].miType;

                    dataBytes = mi_sizeof[ params[j].miType]
                                * params[j].dim
                                * config.getNumOfTimeSteps();

                    _4bytes[dword++] = dataBytes;

                    // Write the remaining items:
                    writeToFile();

                    params[j].origin  = files[i].ftell();
                    params[j].address =
                        var2addr.find(params[j].name)->second;

                    // Make sure the next parameter starts at an 8-byte
                    // boundary:
                    rem = dataBytes % 8;
                    if (rem)
                        dataBytes += rem;

                    if (!files[i].fseek(dataBytes, SEEK_CUR))
                    {
                        std::cout  <<  "error using fseek() "
                            << std::endl;
                        return false;
                    }
                }
            } // Iterate through parameters
        } // Iterate through files

        return true;
    }
}
