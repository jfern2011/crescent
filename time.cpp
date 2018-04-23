#include "spice.h"
#include "time.h"

#include <string>

namespace Time
{
    /**
     ******************************************************************
     *
     * Default constructor
     *
     ******************************************************************
     */
    Epoch::Epoch()
        :_jd(0)
    {}

    /**
     ******************************************************************
     *
     * Constructor
     *
     * Creates an Epoch set to the specified date. See setDate() for
     * additional info
     *
     * @param[in] datestr A string representing the epoch
     *
     ******************************************************************
     */
     Epoch::Epoch(const std::string& datestr)
     {
         setDate(datestr);
     }

    /**
     ******************************************************************
     *
     * Destructor
     *
     ******************************************************************
     */
    Epoch::~Epoch()
    {}

    /**
	 ******************************************************************
	 *
	 * Retrieve the Julian date
	 *
	 * @param sec  The number of seconds since the reference date
	 *
	 * @return the Julian date
	 *
	 ******************************************************************
	 */
	double Epoch::getJulianDate(double sec) const
	{
		return _jd + sec / SECONDS_PER_DAY;
	}

    /**
     ******************************************************************
     *
     * Set the reference date
	 *
	 * Parses the input date string and sets the internal Julian date
	 * and current year. The set of allowable formats is limited by
	 * SPICE's str2et_c() function, which accepts virtually all common
	 * calendar representations
     *
     * @param[in] datestr A string representing the epoch
     *
	 * @return true upon success
	 *
	 ******************************************************************
     */
	bool Epoch::setDate(const std::string& datestr)
	{
		double et;

		Spice::str2et(datestr.c_str(), &et);
		_jd = Spice::unitim( et, "ET", "JDTDT" );

		return true;
	}
}
