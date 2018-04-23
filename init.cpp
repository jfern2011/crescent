#include "dynamics.h"
#include "spice.h"
#include "config.h"
#include "time.h"
#include <Util.h>

#include <iostream>
#include <string>
#include <csignal>

bool initialize( const Time::Epoch& epoch, const Spice::Interface& spice )
{
    SpiceBoolean found;
    SpiceInt code;

    // Make sure the .bsp file provided has ephemeris data for each
    // body SPICE is enabled for:
    for (size_t i = 0; i < Dynamics::bodies.size(); i++)
    {
        if (Dynamics::bodies[i].spiceEnabled())
        {
            Spice::bods2c(Dynamics::bodies[i].getName().c_str(),
                          &code, &found);
            if (!found)
            {
                std::cout << "ephemeris data is not available for \""
                    << Dynamics::bodies[i].getName()
                    << "\" in "
                    << spice.getBspFile() << std::endl;
                return false;
            }

            // Set up initial state vectors for all rigid bodies
            // whose ephemeris was not provided in the input
            // configuration and which are not the central body:
            Dynamics::updateEphemerides(epoch, 0);
        }
    }

    return true;
}
