#include <iostream>
#include <cmath>
#include <vector>

#include <Vector.h>
#include <Matrix.h>

#include "dynamics.h"
#include "spice.h"

static const double _ICRF_TO_J2000[9] =
{
	 0.999999999999994, -0.000000070782797,  0.000000080561489,
	 0.000000070782795,  0.999999999999997,  0.000000033060415,
	-0.000000080561492, -0.000000033060409,  0.999999999999996,
};

namespace Dynamics
{
    /** A list of all rigid bodies considered in this
     *  simulation
     */
    std::vector<RigidBody> bodies;

    /** A pointer to a Configurator object, used for
     *  looking up the central body when solving the
     *  N-body problem:
     */
    Config::Configurator* config;

    /** Universal constant of gravitation, m^3/kg/s^2
     */
    const double G = 0.0000000000667384;

    /** ICRF to J2000 DCM
     */
    LinAlg::Matrix ICRF_TO_J2000(3,3,_ICRF_TO_J2000 );

    /** The current simulation time (in seconds)
     */
    double simulationTime = 0;

    /**
     ******************************************************************
     *
     * Constructor
     *
     * @param[in] name The name of the rigid body
     * @param[in] mass The rigid body's mass
     * @param[in] mdot The rigid body's time rate of change of mass
     *
     ******************************************************************
     */
    RigidBody::RigidBody(const std::string& name, double mass,
                         double mdot)
        : _name(name),      // Name of this body
          _mass(mass),      // Mass (kg)
          _mdot(mdot),      // Body's mass rate of change (kg/sec)
          _state(6,0.0),    // Orbital position/velocity (m/s)
          _stateMask(0),    // Bit mask for initialized states
          _uspice(false),   // Obtain ephemeris from SPICE
          _isCenter(false), // Is true if this is the central body
          _accel(3,0.0)     // Acceleration vector
    {}

    /**
     ******************************************************************
     *
     * Destructor
     *
     ******************************************************************
     */
    RigidBody::~RigidBody() {};

    /**
     ******************************************************************
     *
     * Assignment operator
     *
     * @param[in] rhs The body to assign this one to
     *
     * @return A reference to *this
     *
     ******************************************************************
     */
    RigidBody& RigidBody::operator=(const RigidBody& rhs)
    {
        if (this == &rhs) return *this;

        _name = rhs.getName();
        _mass = rhs.getMass();
        _mdot = rhs.getMdot();
        _state = rhs.getECIstateJ2000();
        _stateMask = rhs.getStateMask();
        _uspice = rhs.spiceEnabled();
        _isCenter = rhs.isCentralBody();

        return *this;
    }

    /**
     ******************************************************************
     *
     * Retrieve the acceleration of this rigid body
     *
     * @return The 3x1 acceleration vector in m/s^2
     *
     ******************************************************************
     */
    LinAlg::Vector RigidBody::getAccel(void) const
    {
        return _accel;
    }

    /**
     ******************************************************************
     *
     * Retrieve the state vector
     *
     * @return The state vector
     *
     ******************************************************************
     */
    const LinAlg::Vector& RigidBody::getECIstateJ2000() const
    {
        return _state;
    }

    /**
     ******************************************************************
     *
     * Retrieve the body's mass
     *
     * @return The body's mass in kg
     *
     ******************************************************************
     */
    double RigidBody::getMass() const
    {
        return _mass;
    }

    /**
     ******************************************************************
     *
     * Retrieve the mass rate of change with respect to time
     *
     * @return The mass rate in kg/sec
     *
     ******************************************************************
     */
    double RigidBody::getMdot() const
    {
        return _mdot;
    }

    /**
     ******************************************************************
     *
     * Retrieve the name of this rigid body
     *
     * @return The body's name
     *
     ******************************************************************
     */
    std::string RigidBody::getName() const
    {
        return _name;
    }

    /**
     ******************************************************************
     *
     * Retrieve a bit mask indicating which states were initialized
     * for this body. This is used to determine if the input
     * configuration file only partially specifies the state vector
     * of a rigid body. A value of 0x3F means all states were
     * initialized
     *
     * @return A bit mask indicating which states have been initialized
     *         for this body
     *
     ******************************************************************
     */
    int RigidBody::getStateMask() const
    {
        return _stateMask;
    }

    /**
     ******************************************************************
     *
     * Indicates if this body is the central body in the system, i.e.
     * if positions and velocities of all bodies within the system are
     * computed relative to this one
     *
     * @return True if this is the central body, false otherwise
     *
     ******************************************************************
     */
    bool RigidBody::isCentralBody() const
    {
        return _isCenter;
    }

    /**
     ******************************************************************
     *
     * Print the members of this object
     *
     ******************************************************************
     */
    void RigidBody::print() const
    {
        std::cout << "name:           " << _name     << "\n";
        std::cout << "mass (kg):      " << _mass     << "\n";
        std::cout << "mdot (kg/s):    " << _mdot     << "\n";
        std::cout << "using SPICE:    " << _uspice   << "\n";
        std::cout << "central body:   " <<
                        (_isCenter ? "yes" : "no")   << "\n";
        std::cout << "state[0] (m):   " << _state(0) << "\n";
        std::cout << "state[1] (m):   " << _state(1) << "\n";
        std::cout << "state[2] (m):   " << _state(2) << "\n";
        std::cout << "state[3] (m/s): " << _state(3) << "\n";
        std::cout << "state[4] (m/s): " << _state(4) << "\n";
        std::cout << "state[5] (m/s): " << _state(5) << std::endl;
    }

    /**
     ******************************************************************
     *
     * Assign a new acceleration to this rigid body
     *
     * @param[in] accel The new acceleration vector
     *
     ******************************************************************
     */
    void RigidBody::setAccel(const LinAlg::Vector& accel)
    {
        _accel = accel;
    }

    /**
     ******************************************************************
     *
     * Flag this body as the central body,  meaning that positions and
     * velocities of all other rigid bodies in the system are computed
     * relative to this one
     *
     * @param[in] value True to make this the central body, false
     *                  otherwise
     *
     ******************************************************************
     */
    void RigidBody::setCentralBody( bool value )
    {
        _isCenter = value;
    }

    /**
     ******************************************************************
     *
     * Set the state vector (1)
     *
     * The state consists of six independent quantities, namely the
     * three components of position and velocity, respectively
     *
     * @param[in] state The state vector as a 6x1 LinAlg::Vector
     *
     ******************************************************************
     */
    void RigidBody::setECIstateJ2000(const LinAlg::Vector& state)
    {
        _state = state;
    }

    /**
     ******************************************************************
     *
     * Set the state vector (2)
     *
     * The state consists of six independent quantities, namely the
     * three components of position and velocity, respectively
     *
     * @param[in] state The state vector as an array of doubles
     *
     ******************************************************************
     */
    void RigidBody::setECIstateJ2000(const double* state)
    {
        _state = LinAlg::Vector(6,state);
    }

    /**
     ******************************************************************
     *
     * Set the state vector (3)
     *
     * The state consists of six independent quantities, namely the
     * three components of position and velocity, respectively
     *
     * @param[in] index Index of the element in the state vector to
     *                  update
     * @param[in] value The value to prescribe this element
     *
     ******************************************************************
     */
    void RigidBody::setECIstateJ2000(int index, double value)
    {
        _state(index) = value;
    }

    /**
     ******************************************************************
     *
     * Set the mass of this body
     *
     * @param[in] mass The mass in kg
     *
     ******************************************************************
     */
    void RigidBody::setMass(double mass)
    {
        _mass = mass;
    }

    /**
     ******************************************************************
     *
     * Set the rate of change of mass for this body
     *
     * @param[in] mdot The mass rate in kg/sec
     *
     ******************************************************************
     */
    void RigidBody::setMdot(double mdot)
    {
        _mdot = mdot;
    }

    /**
     ******************************************************************
     *
     * Enable or disable SPICE updates for this body. If updates are
     * enabled, the propagator will periodically reset the ephemeris
     * obtained from SPICE
     *
     * @param[in] option True to enable, false otherwise
     *
     ******************************************************************
     */
    void RigidBody::setSpiceEnable(bool option)
    {
        _uspice = option;
    }

    /**
     ******************************************************************
     *
     * Each time the input file provides a new state element for this
     * body, the corresponding bit is set in an internal bit mask,
     * which ensures the state is fully initialized before attempting
     * to propagate it
     *
     * @param[in] index  The index [0,5] of the state that was set in
     *                   the input configuration file
     *
     ******************************************************************
     */
    void RigidBody::setStateMaskBit( int index )
    {
        _stateMask |= 1 << index;
    }

    /**
     ******************************************************************
     *
     * Check if SPICE updates are enabled for this body
     *
     * @return True if SPICE is enabled
     *
     ******************************************************************
     */
    bool RigidBody::spiceEnabled() const
    {
        return _uspice;
    }

    /**
     ******************************************************************
     *
     * Compute derivatives of the state vector of all rigid bodies of
     * the system. This function is passed to integrate()
     *
     * @param[in]  time    The current simulation time (not used if
     *                     dynamics are time invariant)
     * @param[in]  stateIn The initial state vectors of all bodies
     * @param[out] dxdt    Velocities and accelerations of all rigid
     *                     bodies
     *
     ******************************************************************
     */
    void compute_dxdt(double time, const LinAlg::Vector& stateIn,
                      LinAlg::Vector& dxdt)
    {
        std::vector<LinAlg::Vector> F_ext;

        // Compute the external forces on each rigid
        // body:
        for (size_t i = 0; i < bodies.size(); i++)
            F_ext.push_back(LinAlg::Vector(3,0.0));

        nBodyEOM(F_ext);
        dxdt = LinAlg::Vector(6*bodies.size(),0.0);

        LinAlg::Vector velocity, accel;

        for (size_t i = 0; i < bodies.size(); i++)
        {
            accel    = bodies[i].getAccel();
            velocity =
                bodies[i].getECIstateJ2000().sub( 3, 3 );

            // Note: May add angular rates and accelerations to this
            //       as well
            dxdt(6*i+3) = accel(0);
            dxdt(6*i+4) = accel(1);
            dxdt(6*i+5) = accel(2);

            dxdt(6*i+0) = velocity(0);
            dxdt(6*i+1) = velocity(1);
            dxdt(6*i+2) = velocity(2);
        }
    }

    /**
     ******************************************************************
     *
     * Search through the list of rigid bodies by name (case-sensitive)
     *
     * @param[in] name The name of the rigid body
     *
     * @return An index into the list of rigid bodies where a match was
     *         found, -1 otherwise
     *
     ******************************************************************
     */
    int lookupRigidBody(const std::string& name)
    {
        for (size_t i = 0; i < bodies.size(); i++)
        {
            if (bodies[i].getName() == name)
                return i;
        }

        return -1;
    }

    /**
     ******************************************************************
     *
     * Compute the acceleration for all rigid bodies in the system and
     * update each body's "accel" vector
     *
     * @param[in] F_ext External forces acting on individual bodies in
     *                  the system
     *
     ******************************************************************
     */
    void nBodyEOM( const std::vector<LinAlg::Vector>& F_ext )
    {
        double m_i, m_j, m_N, mag_ri, mag_rj, mag_rji;
        LinAlg::Vector accel;
        LinAlg::Vector r_i, r_j, r_ji, pos_N;

        size_t centralBody= config->getCentralBody().int_val;

        m_N = bodies[centralBody].getMass();
        pos_N =
             bodies[centralBody].getECIstateJ2000().sub(0,3);


        for ( size_t i = 0; i < bodies.size(); i++ )
        {
            if (i == centralBody) continue;

            // Compute accelerations due to the central body
            // and external forces
            r_i =
                bodies[i].getECIstateJ2000().sub(0,3)-pos_N;
            mag_ri = r_i.norm();
            m_i = bodies[i].getMass();

            accel = (-G*(m_N + m_i)/pow(mag_ri,3))*r_i
                    + F_ext[i]/m_i
                    - F_ext[centralBody]/m_N;

            // Compute perturbations due to the remaining
            // bodies:
            for ( size_t j = 0; j < bodies.size(); j++ )
            {
                if (j == centralBody || i == j) continue;

                r_j =
                    bodies[j].getECIstateJ2000().sub(0,3) - pos_N;

                mag_rj = r_j.norm();
                r_ji = r_i - r_j;
                mag_rji = r_ji.norm();
                m_j = bodies[j].getMass();
                accel -=
                    G*m_j*(r_ji/pow(mag_rji,3)+r_j/pow(mag_rj,3));
            }

            bodies[i].setAccel(accel);
        }
    }

    /**
     ******************************************************************
     *
     * Update dynamics quantities for all rigid bodies with the given
     * state vector
     *
     * @param[in] x A new state vector of size 6*n for n rigid bodies
     *              (6 states per body)
     *
     ******************************************************************
     */
    void updateDynamics( const LinAlg::Vector& x )
    {
        for (size_t i = 0;
             i < bodies.size(); i++)
        {
            for (int j = 0; j < 6; j++)
                bodies[i].setECIstateJ2000( j, x(6*i+j) );
        }
    }

    /**
     ******************************************************************
     *
     * Update the ephemeris of each rigid body for which SPICE has been
     * enabled
     *
     * @param[in] epoch   The epoch corresponding to the simulation
     *                    start time (T0)
     * @param[in] simTime The current simulation time (sec)
     * @param[in] conf    The Configurator object used to configure the
     *                    simulation
     *
     ******************************************************************
     */
    void updateEphemerides( const Time::Epoch& epoch, double simTime )
    {
        const std::string& center = config->getCentralBody().str_val;
        double lt, tdb, state[6];
        double jd = epoch.getJulianDate(0) + simTime/SECONDS_PER_DAY;

        tdb = Spice::unitim(jd, "JDTDT", "TDB");

        LinAlg::Vector position(3,0.0);
        LinAlg::Vector velocity(3,0.0);

        for (size_t i = 0; i < bodies.size(); i++)
        {
            if (!bodies[i].spiceEnabled() || bodies[i].isCentralBody())
                continue;

            Spice::spkezr(bodies[i].getName().c_str(),

                          tdb,             /* Barycentric dynamical time  */
                          "j2000",         /* J2000 frame */
                          "LT+S",          /* Light-Time + Stellar
                                              Abberation correction */
                           center.c_str(), /* Observer */
                           state,          /* Cartesian position/velocity */
                           &lt);           /* One-way light time */

            for (int j = 0; j < 3; j++)
            {
                position(j) = state[j]   * KM2M;
                velocity(j) = state[j+3] * KM2M;
            }

            /*
             * SPICE quantities are in ICRF coordinates; rotate
             * to J2000:
             */
            position = ICRF_TO_J2000 * position;
            velocity = ICRF_TO_J2000 * velocity;

            for (int j = 0; j < 3; j++)
            {
                bodies[i].setECIstateJ2000(j,   position(j));
                bodies[i].setECIstateJ2000(j+3, velocity(j));
            }
        }
    }
}
