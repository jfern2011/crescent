#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <vector>
#include <map>
#include <string>

#include <util.h>
#include "time.h"

namespace Config
{
    typedef std::map<std::string,Util::str_v> opt_arg_map;
    typedef struct
    {
        std::string str_val;
        int int_val;
    } str_int;

    class Configurator
    {
    public:
        Configurator(std::string& configFile);
        ~Configurator();
        const str_int& getCentralBody(void) const;
        double getDeltaT(void) const;
        double getDuration(void) const;
        int getNumOfTimeSteps(void) const;
        double getSpiceDT(void) const;
        bool lookup(const std::string& option, Util::str_v& args) const;
        void print(const Time::Epoch& epoch) const;
        void printMap() const;

    private:
        opt_arg_map config;
        double spiceDT;
        double deltaT;
        double duration;
        str_int central_body;
        int totalsamples;
    };
}

#endif // __CONFIG_H__
