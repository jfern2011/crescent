#include <iostream>
#include <string>

#include <Vector.h>
#include <Util.h>
#include "dynamics.h"
#include "spice.h"
#include "time.h"
#include "config.h"
#include "init.h"
#include "FileIO.h"

#include <cstdio>

/**
 ******************************************************************
 *
 * Show progress towards completion. This updates at intervals of
 * 0.01% completion
 *
 * @param[in] step        Current time step
 * @param[in] total_steps The total number of discrete time steps
 *
 ******************************************************************
 */
void showProgress( int step, int total_steps )
{
    static double last_pc = 0;
	double pc = step*100.0/total_steps;

	if ( 0.01 <= pc - last_pc )
    {
        std::printf("\rRunning...%6.2f%%", pc);
        last_pc = pc;
        std::fflush(stdout);
    }
}

/**
 ******************************************************************
 *
 * Run the simulation
 *
 * @param[in] config A Configurator created by reading the input
 *                   config file
 * @param[in] files  A list of MAT files that will be written to
 *                   as output
 *
 ******************************************************************
 */
void runSim( const Config::Configurator& config,
            std::vector<FileIO::MatFile>& files )
{
    // 1. Set initial state vectors for all rigid bodies:
    //
    LinAlg::Vector x0(6 * Dynamics::bodies.size(),
                      0.0);
    LinAlg::Vector xf(6 * Dynamics::bodies.size(),
                      0.0);

    for (size_t i= 0; i < Dynamics::bodies.size(); i++)
    {
        LinAlg::Vector state =
                Dynamics::bodies[i].getECIstateJ2000();

        for (int j = 0; j < 6; j++)
            x0(6*i + j) = state(j);
    }

    // 2. Save initial values:
    FileIO::updateOuputFile(0, files);

    // 3. Run:
    //
    for (int current_step = 1;
         current_step < config.getNumOfTimeSteps();
         current_step++)
    {
        Dynamics::simulationTime =
            Util::integrate(&Dynamics::compute_dxdt, Dynamics::simulationTime,
                            current_step * config.getDeltaT(),
                            x0, xf );

        Dynamics::updateDynamics(xf);
        FileIO::updateOuputFile(current_step, files);

        x0 = xf;
        showProgress(current_step,
                     config.getNumOfTimeSteps());
    }
}

int main()
{
    std::string configFile = "C:\\Users\\Jason\\Desktop\\Coding\\dynamics2\\dynamics2\\config";
    if (!Util::dirExists(configFile))
    {
        std::cout << configFile << " does not exist"
            << std::endl;
        return 0;
    }


    // Begin:


    Config::Configurator conf(configFile);

    // Have Dynamics use this Configurator:
    Dynamics::config = &conf;

    // Create a SPICE Interface object:
    Spice::Interface spiceObj(conf);

    // Set the Epoch (T = 0) for this run:
    Util::str_v args;
    conf.lookup("utc_epoch", args);
    Time::Epoch epoch( args[ 0 ] );

    // Execute initialization routines:
    initialize(epoch, spiceObj);

    // Print out the initialization summary:
    conf.print(epoch);

    // Create output files:
    std::vector<FileIO::MatFile> matFiles;
    if (!FileIO::setupOutputFiles(conf,
                                matFiles))
        return 0;

    std::cout << "Number of output files: "
        << matFiles.size() << std::endl;

    runSim(conf, matFiles);

    return 0;
}
