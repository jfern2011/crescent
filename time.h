#ifndef __TIME_H__
#define __TIME_H__

#define SECONDS_PER_DAY 86400

namespace Time
{
    class Epoch
    {
    public:
        Epoch();
        Epoch(const std::string& datestr);
        ~Epoch();
        double getJulianDate( double sec ) const;
        bool setDate(const std::string& datestr);
    private:
        double _jd;
    };
}

#endif // __TIME_H__
