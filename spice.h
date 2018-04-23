#ifndef __SPICE_H__
#define __SPICE_H__

#include "config.h"

#include <string>
#include <SpiceUsr.h>

namespace Spice
{
    class Interface
    {
    public:
        Interface(const Config::Configurator& config);
        ~Interface();
        std::string getBspFile(void) const;
        std::string getTlsFile(void) const;
        std::string getTpcFile(void) const;
    private:
        std::string _tpcFile;
        std::string _bspFile;
        std::string _tlsFile;
    };

    /**
     ******************************************************************
     *
     * Wrapper for the SPICE function bods2c_c(). See the documentation
     * for details
     *
     * @param[in]  name  The name of the body to obtain an ID code for
     * @param[out] code  A unique ID code corresponding to "name"
     * @param[out] found A flag indicating whether or not the name was
     *                   successfully ID'd
     *
     ******************************************************************
     */
    inline void bods2c(ConstSpiceChar* name, SpiceInt* code,
                       SpiceBoolean* found)
    {
        bods2c_c(name, code, found);
    }

    /**
     ******************************************************************
     *
     * Wrapper for the SPICE function et2utc_c(). See the documentation
     * for details
     *
     * @param[in]  et      Epoch given as ephemeris time
     * @param[in]  format  Desired output format
     * @param[in]  prec    Output precision
     * @param[in]  lenout  Maximum output string length
     * @param[out] utcstr  Output UTC date string
     *
     ******************************************************************
     */
    inline void et2utc(SpiceDouble et, ConstSpiceChar* format,
                       SpiceInt prec, SpiceInt lenout,
                       SpiceChar* utcstr)
    {
        et2utc_c(et, format, prec, lenout, utcstr);
    }

    /**
     ******************************************************************
     *
     * Wrapper for the SPICE function spkezr_c(). See the documentation
     * for details
     *
     * @param[in]  targ   Target body name, or its equivalent ID code
     * @param[in]  et     Observer epoch in ephemeris time
     * @param[in]  ref    Reference frame of the output state vector
     * @param[in]  abcorr Type of correction to apply
     * @param[in]  obs    Observing body name
     * @param[out] starg  Target state vector
     * @param[out] lt     One-way light time between the observer and
     *                    target
     *
     ******************************************************************
     */
    inline void spkezr(ConstSpiceChar* targ, SpiceDouble et,
                       ConstSpiceChar* ref, ConstSpiceChar *abcorr,
                       ConstSpiceChar* obs, SpiceDouble starg[6],
                       SpiceDouble* lt)
    {
        spkezr_c(targ, et, ref, abcorr, obs, starg, lt);
    }

    /**
     ******************************************************************
     *
     * Wrapper for the SPICE function str2et_c(). See the documentation
     * for details
     *
     * @param[in]  str An input date string. By default, SPICE may take
     *                 this as UTC time on a 24-hour clock
     * @param[out] et  Output ephemeris time
     *
     ******************************************************************
     */
    inline void str2et(ConstSpiceChar* str, SpiceDouble* et)
    {
        str2et_c(str, et);
    }

    /**
     ******************************************************************
     *
     * Wrapper for the SPICE function unitim_c(). See the documentation
     * for details
     *
     * @param[in] epoch  The epoch to convert
     * @param[in] insys  Input time scale
     * @param[in] outsys Desired time scale
     *
     * @return The epoch converted from the input to desired time scale
     *
     ******************************************************************
     */
    inline double unitim(SpiceDouble epoch, ConstSpiceChar* insys,
                         ConstSpiceChar* outsys)
    {
        return unitim_c(epoch, insys, outsys);
    }
}

#endif // __SPICE_H__
