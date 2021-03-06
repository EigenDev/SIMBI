/* 
* Interface between python construction of the 1D SR state 
* and cpp. This is where the heavy lifting will occur when 
* computing the HLL derivative of the state vector
* given the state itself.
*/

#include <vector>
#include <string>
#include "hydro_structs.h"

#ifndef SRHD_1D_H
#define SRHD_1D_H

namespace simbi
{
    class SRHD
    {
    public:
        double gamma, CFL;
        std::string coord_system;
        std::vector<double> r;
        std::vector<std::vector<double>> state;

        SRHD();
        SRHD(std::vector<std::vector<double>> state, double gamma, double CFL,
             std::vector<double> r, std::string coord_system);
        ~SRHD();

        sr1d::ConservedArray cons_state;
        sr1d::PrimitiveArray prims;

        int Nx, n, pgrid_size, idx_shift;
        float tend, dt;
        double theta, engine_duration, t, decay_constant;
        bool first_order, periodic, linspace, hllc;

        std::vector<double> lorentz_gamma, sourceD, sourceS, source0, pressure_guess;

        sr1d::PrimitiveArray cons2prim1D(const sr1d::ConservedArray &cons_state, std::vector<double> &lorentz_gamma);

        sr1d::Eigenvals calc_eigenvals(const sr1d::Primitive &prims_l, const sr1d::Primitive &prims_r);

        double adapt_dt(sr1d::PrimitiveArray &prims);

        sr1d::Conserved calc_state(double rho, double v, double pressure);

        sr1d::Conserved calc_hll_state(
            const sr1d::Conserved &left_state,
            const sr1d::Conserved &right_state,
            const sr1d::Conserved &left_flux,
            const sr1d::Conserved &right_flux,
            const sr1d::Primitive &left_prims,
            const sr1d::Primitive &right_prims);

        sr1d::Conserved calc_intermed_state(const sr1d::Primitive &prims,
                                            const sr1d::Conserved &state,
                                            const double a,
                                            const double aStar,
                                            const double pStar);

        sr1d::Conserved calc_flux(double rho, double v, double pressure);

        sr1d::Conserved calc_hll_flux(const sr1d::Primitive &left_prims,
                                      const sr1d::Primitive &right_prims,
                                      const sr1d::Conserved &left_state,
                                      const sr1d::Conserved &right_state,
                                      const sr1d::Conserved &left_flux,
                                      const sr1d::Conserved &right_flux);

        sr1d::Conserved calc_hllc_flux(const sr1d::Conserved &left_state,
                                       const sr1d::Conserved &right_state,
                                       const sr1d::Conserved &left_flux,
                                       const sr1d::Conserved &right_flux,
                                       const sr1d::Primitive &left_prims,
                                       const sr1d::Primitive &right_prims);

        sr1d::ConservedArray u_dot1D(sr1d::ConservedArray &u_state);

        std::vector<std::vector<double>> simulate1D(std::vector<double> &lorentz_gamma,
                                                    std::vector<std::vector<double>> &sources,
                                                    float tstart,
                                                    float tend,
                                                    float dt,
                                                    double theta,
                                                    double engine_duraction,
                                                    double chkpt_interval,
                                                    std::string data_directory,
                                                    bool first_order, bool periodic, bool linspace, bool hllc);
    };
}

#endif