#include "dynamics.h"
#include "time.h"
#include "config.h"
#include "spice.h"
#include <util.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <csignal>
#include <cstdlib>

namespace Config
{
    /**
     ******************************************************************
     *
     * Constructor
     *
     * Parses the input configuration file and populates the list of
     * rigid bodies housed in Dynamics and assigns them the initial
     * values given in the config file. Any required information
     * missing from the config, such as mass or ephemeris, is looked
     * up later via SPICE
     *
     * @param[in] configFile Full path to the input config file
     *
     ******************************************************************
     */
    Configurator::Configurator(std::string& configFile)
    {
        std::ifstream infile(configFile.c_str());

        if (!infile.is_open())
        {
            std::cout << "unable to open: " << configFile
                << std::endl;
            std::raise(SIGABRT);
        }

        std::string line, option, arg;
        int lineNo = 0;

        while (std::getline(infile, line))
        {
            lineNo++;

            line = Util::trim(Util::removeComment(line));
            std::istringstream iss(line);
            iss.peek();
            if (iss.eof()) continue;

            if (!(iss >> option))
            {
                std::cout << "error reading " << configFile
                    << " at line "
                    << lineNo << std::endl;
                std::raise(SIGABRT);
            }

            if (option == "utc_epoch")
            {
                std::string epoch =
                    line.substr(option.size(),std::string::npos);
                epoch = Util::trim(epoch);
                if (epoch.size() == 0)
                {
                    std::cout << "error parsing epoch"
                        << std::endl;
                    std::raise(SIGABRT);
                }
                config[option].push_back(epoch);
            }
            else if (option == "body")
            {
                if (!(iss >> arg))
                {
                    std::cout << "error reading " << configFile
                        << " at line "
                        << lineNo << std::endl;
                    std::raise(SIGABRT);
                }

                double mass, mdot;
                int index = Dynamics::lookupRigidBody(arg);
                if (index < 0)
                {
                    Dynamics::RigidBody temp(arg,0,0);
                    Dynamics::bodies.push_back(temp);
                    index = Dynamics::bodies.size()-1;
                }

                if (iss >> mass)
                    Dynamics::bodies[index].setMass(mass);
                if (iss >> mdot)
                    Dynamics::bodies[index].setMdot(mdot);

            }
            else if (line.find('[',0) != std::string::npos)
            {
                size_t start = line.find('[',0);
                size_t stop  = line.find(']',0);

                if (stop == std::string::npos ||
                    stop <= start+1)
                {
                    std::cout << "error reading " << configFile
                        << " at line "
                        << lineNo << std::endl;
                    std::raise(SIGABRT);
                }

                std::string ind_str =
                        Util::trim(line.substr(start+1,stop-start-1));
                int index =
                    std::strtol(ind_str.c_str(), NULL, 10);

                double value;
                if ( index < 0 || index > 5 || !(iss >> value))
                {
                    std::cout << "error reading " << configFile
                        << " at line "
                        << lineNo << std::endl;
                    std::raise(SIGABRT);
                }

                std::string name = Util::trim(line.substr(0, start));
                int body =
                    Dynamics::lookupRigidBody(name);

                if (body < 0)
                {
                    Dynamics::RigidBody temp(name,0,0);
                    Dynamics::bodies.push_back(temp);
                    body = Dynamics::bodies.size() - 1;
                }

                Dynamics::bodies[body].setECIstateJ2000(index, value);
                Dynamics::bodies[body].setStateMaskBit(index);
            }
            else
            {
                while (iss >> arg)
                    config[option].push_back(arg);
            }
        }

        opt_arg_map::const_iterator it = config.find("spice_dt");
        if (it != config.end())
        {
            spiceDT = std::strtod(it->second[0].c_str(), NULL);
            if (spiceDT < 0)
            {
                std::cout << "error: option \"spice_dt\" = " << spiceDT
                    << std::endl;
                std::raise(SIGABRT);
            }
        }

        it = config.find("delta_t");
        if (it == config.end())
        {
            std::cout << "time step \"delta_t\" must be specified"
                << std::endl;
            std::raise(SIGABRT);
        }
        else
        {
            deltaT = std::strtod(it->second[0].c_str(), NULL);
            if (deltaT < 0)
            {
                std::cout << "error: option \"delta_t\" is " << deltaT
                    << std::endl;
                std::raise(SIGABRT);
            }
        }

        it = config.find("duration");
        if (it == config.end())
        {
            std::cout << "option \"duration\" must be specified"
                << std::endl;
            std::raise(SIGABRT);
        }
        else
        {
            duration = std::strtod(it->second[0].c_str(), NULL);
            if (duration < 0)
            {
                std::cout << "error: option \"duration\" is "
                    << duration << std::endl;
                std::raise(SIGABRT);
            }
        }

        // Compute the total number of data samples:
        totalsamples = duration / deltaT + 1;

        it = config.find("central_body");
        if (it == config.end())
        {
            std::cout << "option \"central_body\" must be specified"
                << std::endl;
            std::raise(SIGABRT);
        }
        else
        {
            central_body.str_val = it->second[0];
            central_body.int_val =
                            Dynamics::lookupRigidBody(it->second[0]);
            if (central_body.int_val < 0)
            {
                std::cout << "central body \""
                    << central_body.str_val
                    << "\" is undefined" << std::endl;
                std::raise(SIGABRT);
            }
        }

        it = config.find("utc_epoch");
        if (it == config.end())
        {
            std::cout << "option \"utc_epoch\" must be specified"
                << std::endl;
            std::raise(SIGABRT);
        }

        //  For each rigid body whose ephemeris was not specified, get
        //  that data from SPICE:
        for (size_t i = 0;
             i < Dynamics::bodies.size(); i++)
        {
            if (Dynamics::bodies[i].getName() == central_body.str_val)
                Dynamics::bodies[i].setCentralBody(true);

            switch ( Dynamics::bodies[i].getStateMask() )
            {
            case 0x00:
                Dynamics::bodies[i].setSpiceEnable(true);
                break;
            case 0x3F:
                Dynamics::bodies[i].setSpiceEnable(false);
                break;
            default:
                std::cout << "incomplete ephemeris provided for "
                    << Dynamics::bodies[i].getName()
                    << std::endl;
                std::raise(SIGABRT);
            }
        }
    }

    /**
     ******************************************************************
     *
     * Destructor
     *
     ******************************************************************
     */
    Configurator::~Configurator()
    {}

    /**
     ******************************************************************
     *
     * Get the central body. Positions and velocities of all bodies in
     * the system are computed relative to this
     *
     * @return The central body, given as a name and integer pair; the
     *         index provides the location of this body in the list of
     *         rigid bodies
     *
     ******************************************************************
     */
    const str_int& Configurator::getCentralBody() const
    {
        return central_body;
    }

    /**
     ******************************************************************
     *
     * Get the simulation output rate (seconds)
     *
     * @return The output sample period
     *
     ******************************************************************
     */
    double Configurator::getDeltaT() const
    {
        return deltaT;
    }

    /**
     ******************************************************************
     *
     * Get the simulation duration (seconds)
     *
     * @return The simulation duration
     *
     ******************************************************************
     */
    double Configurator::getDuration() const
    {
        return duration;
    }

    /**
     ******************************************************************
     *
     * Retrieve the total number of time steps for the simulation,
     * computed as (duration / deltaT) + 1
     *
     * @return The total number of time steps
     *
     ******************************************************************
     */
    int Configurator::getNumOfTimeSteps() const
    {
        return totalsamples;
    }

    /**
     ******************************************************************
     *
     * Get the time interval (in seconds) between consecutive SPICE
     * updates
     *
     * @return The SPICE update delta-T
     *
     ******************************************************************
     */
    double Configurator::getSpiceDT() const
    {
        return spiceDT;
    }

    /**
     ******************************************************************
     *
     * @brief Look up a configuration option
     *
     * Configuration options are stored as <key, list> pairs. A key of
     * "duration" will return a single-element list containing the
     * simulation duration. An option is supported if can be specified
     * multiple times, e.g. "body"
     *
     * @param[in]  option The config option to look up
     * @param[out] args   A list of arguments provided with this option
     *
     * @return True on success
     *
     ******************************************************************
     */
    bool Configurator::lookup(const std::string& option,
                              Util::str_v& args) const
    {
        opt_arg_map::const_iterator it = config.find(option);
        if (it == config.end())
            return false;

        args = it->second;
        return true;
    }

    /**
     ******************************************************************
     *
     * Prints configuration information and initial conditions at
     * simulation start time
     *
     ******************************************************************
     */
    void Configurator::print(const Time::Epoch& epoch) const
    {
        Util::str_v args;
        char utc_str[128], format[128], heading[1024];
        char tempstr[128];
        double et, jd = epoch.getJulianDate(duration);

        et = Spice::unitim( jd, "JDTDT", "ET" );
        Spice::et2utc(et, "C", 2, 128, utc_str);
        lookup("utc_epoch", args);
        std::cout << "\nEpoch:      " << args[0] << "\n";
        std::cout <<   "End date:   " << utc_str << "\n";
        lookup("central_body", args);
        std::cout << "Central body: " << args[0] << "\n";

        std::vector<int> col_widths;
        for (size_t i = 0;
             i < Dynamics::bodies.size(); i++)
        {
            size_t sz = Dynamics::bodies[i].getName().size();
            size_t col_width = 0;
            if (sz > col_width)
                col_width = sz;

            LinAlg::Vector state =
                Dynamics::bodies[i].getECIstateJ2000();
            for (int j = 0; j < 6; j++)
            {
                std::sprintf(format, "%0.6f", state(j));
                if (std::strlen(format) > col_width)
                        col_width = std::strlen(format);
            }
            col_widths.push_back(col_width+2);
        }

        *heading = '\0';
        for ( size_t i = 0; i < Dynamics::bodies.size(); i++ )
        {
            std::sprintf(format, "%%%ds", col_widths[i]);
            std::sprintf(tempstr, format,
                  Dynamics::bodies[i].getName().c_str());
            std::strcat(heading, tempstr);
        }

        std::cout << heading << "\n";
        for (int j = 0; j < 6; j++)
        {
            *heading = '\0';
            for (size_t i = 0; i < Dynamics::bodies.size(); i++)
            {
                LinAlg::Vector state =
                        Dynamics::bodies[i].getECIstateJ2000();
                std::sprintf(format, "%%%d.6f", col_widths[i]);
                std::sprintf(tempstr, format, state(j));
                                    std::strcat(heading, tempstr);
            }
            std::cout << heading << "\n";
        }

        std::cout << std::endl;
    }

    /**
     ******************************************************************
     *
     * Prints out a mapping from configuration option to arguments;
     * essentially this provides the input configuration, excluding
     * multiply-defined options (e.g. "body")
     *
     ******************************************************************
     */
    void Configurator::printMap() const
    {
        for (opt_arg_map::const_iterator it = config.begin();
             it != config.end(); ++it)
        {
            std::cout << it->first << ": ";
            for (size_t i = 0;
                 i < it->second.size(); i++)
            {
                std::cout << "\"" << it->second[i] << "\""
                    << " ";
            }
            std::cout << std::endl;
        }
    }
}
