#ifndef __DYNAMICS_H__
#define __DYNAMICS_H__

#include "config.h"
#include "FileIO.h"

#include <string>
#include <vector>
#include <Matrix.h>
#include <Vector.h>

#define KM2M   1000.0

namespace Dynamics
{
    class RigidBody
    {
    public:
        RigidBody(const std::string& name, double mass,
                  double mdot);
        ~RigidBody();
        RigidBody& operator=(const RigidBody& rhs);

        LinAlg::Vector getAccel(void) const;
        const LinAlg::Vector& getECIstateJ2000(void) const;
        double getMass(void) const;
        double getMdot(void) const;
        std::string getName(void) const;
        int getStateMask(void) const;

        bool isCentralBody(void) const;
        void print(void) const;

        void setAccel(const LinAlg::Vector& accel);
        void setCentralBody(bool value);
        void setECIstateJ2000(const LinAlg::Vector& state);
        void setECIstateJ2000(const double* state);
        void setECIstateJ2000(int index, double value);
        void setMass(double mass);
        void setMdot(double mdot);
        void setSpiceEnable(bool option);
        void setStateMaskBit(int index );

        bool spiceEnabled(void) const;

        friend
        void FileIO::assignTypeAddr(FileIO::str_int_map& var2type,
                                    FileIO::str_mem_map& var2addr);

    private:
        std::string _name;
        double _mass;
        double _mdot;
        LinAlg::Vector _state;
        int _stateMask;
        bool _uspice;
        bool _isCenter;
        LinAlg::Vector _accel;
    };

    extern std::vector<RigidBody> bodies;
    extern Config::Configurator * config;
    extern const double G;
    extern LinAlg::Matrix  ICRF_TO_J2000;
    extern double simulationTime;

    void compute_dxdt(double time, const LinAlg::Vector& stateIn,
                      LinAlg::Vector& dxdt);
    int lookupRigidBody(const std::string& name);
    void nBodyEOM( const std::vector<LinAlg::Vector>& F_ext );
    void updateDynamics(const LinAlg::Vector& x);
    void updateEphemerides( const Time::Epoch& epoch, double simTime );
}

#endif // __DYNAMICS_H__
