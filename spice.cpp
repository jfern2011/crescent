#include "spice.h"
#include "dynamics.h"
#include <util.h>

#include <iostream>
#include <string>
#include <csignal>
#include <cmath>
#include <SpiceUsr.h>

namespace Spice
{
    /**
     ******************************************************************
     *
     * Constructor
     *
     * Creates a SPICE interface object to handle loading/unloading of
     * kernels
     *
     * @param[in] config A simulation configurator produced by parsing
     *                   user input
     *
     ******************************************************************
     */
    Interface::Interface(const Config::Configurator& config)
        : _tpcFile(""), _bspFile(""), _tlsFile("")
    {
        Util::str_v temp;
        if (config.lookup("tpc_file", temp))
        {
            _tpcFile = temp[0];
            if (!Util::dirExists(_tpcFile))
            {
                std::cout << "file does not exist: " << _tpcFile
                    << std::endl;
                std::raise(SIGABRT);
            }
            else
                furnsh_c(_tpcFile.c_str());
        }

        if (config.lookup("tls_file", temp))
        {
            _tlsFile = temp[0];
            if (!Util::dirExists(_tlsFile))
            {
                std::cout << "file does not exist: " << _tlsFile
                    << std::endl;
                std::raise(SIGABRT);
            }
            else
                furnsh_c(_tlsFile.c_str());
        }

        if (config.lookup("bsp_file", temp))
        {
            _bspFile = temp[0];
            if (!Util::dirExists(_bspFile))
            {
                std::cout << "file does not exist: " << _bspFile
                    << std::endl;
                std::raise(SIGABRT);
            }
            else
                furnsh_c(_bspFile.c_str());
        }
        else
        {
            // Since no .bsp file was provided, every rigid body's
            // "spice enabled" flag should be false:
            for (size_t i = 0; i < Dynamics::bodies.size(); i++)
            {
                if (Dynamics::bodies[i].spiceEnabled())
                {
                    std::cout << "neither an ephemeris nor a .bsp file"
                        << " was provided for \""
                        << Dynamics::bodies[i].getName() << "\""
                        << std::endl;
                    std::raise(SIGABRT);
                }
            }
        }

        for (size_t i = 0; i < Dynamics::bodies.size(); i++)
        {
            if (Dynamics::bodies[i].getMass() == 0)
            {
                if (_tpcFile.empty())
                {
                    std::cout << "tpc file needed for "
                        << Dynamics::bodies[i].getName() << std::endl;
                    std::raise(SIGABRT);
                }

                double GM; SpiceInt dim;
                bodvrd_c(Dynamics::bodies[i].getName().c_str(), "GM",
                         1, &dim, &GM);

                // Note: SPICE provides GM in units of (km^3 * kg)/s^2
                Dynamics::bodies[i].setMass(
                                (GM * std::pow(KM2M,3)) / Dynamics::G);
            }
        }
    }

    /**
     ******************************************************************
     *
     * @brief Destructor
     *
     * @details Unloads open SPICE kernels
     *
     ******************************************************************
     */
    Interface::~Interface()
    {
        if (!_tpcFile.empty())
            unload_c(_tpcFile.c_str());
        if (!_bspFile.empty())
            unload_c(_bspFile.c_str());
        if (!_tlsFile.empty())
            unload_c(_tlsFile.c_str());
    }

    /**
     ******************************************************************
     *
     * Obtain the SPICE ephemeris kernel in use
     *
     * @return The name of the ephemeris kernel
     *
     ******************************************************************
     */
    std::string Interface::getBspFile() const
    {
        return _bspFile;
    }

    /**
     ******************************************************************
     *
     * Obtain the SPICE leap seconds kernel in use
     *
     * @return The name of the leap seconds kernel
     *
     ******************************************************************
     */
    std::string Interface::getTlsFile() const
    {
        return _tlsFile;
    }

    /**
     ******************************************************************
     *
     * Obtain the SPICE constants kernel in use
     *
     * @return The name of the constants kernel
     *
     ******************************************************************
     */
    std::string Interface::getTpcFile() const
    {
        return _tpcFile;
    }
}
