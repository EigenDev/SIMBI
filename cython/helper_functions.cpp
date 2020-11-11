
/*
* helper_functions.cpp is where all of the universal functions that can be used
* for all N-Dim hydro calculations
*/

#include "helper_functions.h" 
#include <cmath>
#include <map>
#include <algorithm>
#include <cstdarg>

using namespace std;

// =========================================================================================================
//        HELPER FUNCTIONS FOR COMPUTATION
// =========================================================================================================


// Find the max element
double findMax(double a, double b, double c ){
    //Find max b/w a & b first
    double inter_max = max(a, b);

    double max_val = max(inter_max, c);

    return max_val;
};

// Sound Speed Function
double calc_sound_speed(float gamma, double rho, double pressure){
    double c = sqrt(gamma*pressure/rho);
    return c;

};

// Get the Sign of a Number
int sign(double x){
    if (x/abs(x) == 1) { 
        return 1;
    } else if (x/abs(x) == -1){
        return - 1;
    } else {
        return 0;
    }
};

// The Minmod slope delimiter
double minmod(double x, double y, double z){
    // The intermediate minimum
    double intermin = min(abs(x), abs(y));
    double min_val = min(intermin, abs(z)); 

    return 0.25*abs(sign(x) + sign(y))*(sign(x) + sign(z))*min_val;

};

// Roll a vector for use with periodic boundary conditions
vector<double> rollVector(const vector<double>& v, unsigned int n){
    auto b = v.begin() + (n % v.size());
    vector<double> ret(b, v.end());
    ret.insert(ret.end(), v.begin(), b);
    return ret;
};

// Roll a single vector index
double roll(vector<double>  &v, unsigned int n) {
   return v[n % v.size()];
};

vector<vector<double> > transpose(vector<vector<double> > &mat){

    vector<vector<double> > trans_vec(mat[0].size(), vector<double>());

    int y_size = mat.size();
    int x_size = mat[0].size(); 

    for (int i = 0; i < x_size; i++)
    {
        for (int j = 0; j < y_size; j++)
        {
            if (trans_vec[j].size() != mat.size()){

                trans_vec[j].resize(mat.size());

            }
            
            trans_vec[j][i] = mat[i][j];
        }
    }

    return trans_vec;
};

// void config_ghosts1D(vector<vector<double> >&, int grid_size);

void config_ghosts1D(vector<vector<double> > &u_state, int grid_size, bool first_order){
    if (first_order){
        u_state[0][0] = u_state[0][1];
        u_state[0][grid_size - 1] = u_state[0][grid_size - 2];

        u_state[1][0] = u_state[1][1];
        u_state[1][grid_size - 1] = u_state[1][grid_size - 2];

        u_state[2][0] = u_state[2][1];
        u_state[2][grid_size - 1] = u_state[2][grid_size - 2];
    } else {
        u_state[0][0] = u_state[0][2];
        u_state[0][1] = u_state[0][2];
        u_state[0][grid_size - 1] = u_state[0][grid_size - 3];
        u_state[0][grid_size - 2] = u_state[0][grid_size - 3];

        u_state[1][0] = u_state[1][2];
        u_state[1][1] = u_state[1][2];
        u_state[1][grid_size - 1] = u_state[1][grid_size - 3];
        u_state[1][grid_size - 2] = u_state[1][grid_size - 3];

        u_state[2][0] = u_state[2][2];
        u_state[2][1] = u_state[2][2];
        u_state[2][grid_size - 1] = u_state[2][grid_size - 3];
        u_state[2][grid_size - 2] = u_state[2][grid_size - 3];
    }
};

void config_ghosts2D(vector<vector<vector<double> > > &u_state, int x1grid_size, int x2grid_size, bool first_order){

    if (first_order){
        for (int jj = 0; jj < x2grid_size; jj++){
            for (int ii = 0; ii < x1grid_size; ii++){
                if (jj < 1){
                    u_state[0][jj][ii] =   u_state[0][1][ii];
                    u_state[1][jj][ii] =   u_state[1][1][ii];
                    u_state[2][jj][ii] = - u_state[2][1][ii];
                    u_state[3][jj][ii] =   u_state[3][1][ii];
                    
                } else if (jj > x2grid_size - 2) {
                    u_state[0][jj][ii] =   u_state[0][x2grid_size - 2][ii];
                    u_state[1][jj][ii] =   u_state[1][x2grid_size - 2][ii];
                    u_state[2][jj][ii] = - u_state[2][x2grid_size - 2][ii];
                    u_state[3][jj][ii] =   u_state[3][x2grid_size - 2][ii];

                } else {
                    u_state[0][jj][0] = u_state[0][jj][1];
                    u_state[0][jj][x1grid_size - 1] = u_state[0][jj][x1grid_size - 2];

                    u_state[1][jj][0] = - u_state[1][jj][1];
                    u_state[1][jj][x1grid_size - 1] = u_state[1][jj][x1grid_size - 2];

                    u_state[2][jj][0] = u_state[2][jj][1];
                    u_state[2][jj][x1grid_size - 1] = u_state[2][jj][x1grid_size - 2];

                    u_state[3][jj][0] = u_state[3][jj][1];
                    u_state[3][jj][x1grid_size - 1] = u_state[3][jj][x1grid_size - 2];
                }
            }
        }

    } else {
        int i_shift;
        for (int jj = 0; jj < x2grid_size; jj++){

            // Fix the ghost zones at the radial boundaries
            u_state[0][jj][0] = u_state[0][jj][3];
            u_state[0][jj][1] = u_state[0][jj][2];
            u_state[0][jj][x1grid_size - 1] = u_state[0][jj][x1grid_size - 3];
            u_state[0][jj][x1grid_size - 2] = u_state[0][jj][x1grid_size - 3];

            u_state[1][jj][0] = - u_state[1][jj][3];
            u_state[1][jj][1] = - u_state[1][jj][2];
            u_state[1][jj][x1grid_size - 1] = u_state[1][jj][x1grid_size - 3];
            u_state[1][jj][x1grid_size - 2] = u_state[1][jj][x1grid_size - 3];

            u_state[2][jj][0] = u_state[2][jj][3];
            u_state[2][jj][1] = u_state[2][jj][2];
            u_state[2][jj][x1grid_size - 1] = u_state[2][jj][x1grid_size - 3];
            u_state[2][jj][x1grid_size - 2] = u_state[2][jj][x1grid_size - 3];

            u_state[3][jj][0] = u_state[3][jj][3];
            u_state[3][jj][1] = u_state[3][jj][2];
            u_state[3][jj][x1grid_size - 1] = u_state[3][jj][x1grid_size - 3];
            u_state[3][jj][x1grid_size - 2] = u_state[3][jj][x1grid_size - 3];

            // Fix the ghost zones at the angular boundaries
            for (int ii = 0; ii < x1grid_size; ii++){
                if (jj < 2){
                    if (jj == 0){
                        u_state[0][jj][ii] =   u_state[0][3][ii];
                        u_state[1][jj][ii] =   u_state[1][3][ii];
                        u_state[2][jj][ii] = - u_state[2][3][ii];
                        u_state[3][jj][ii] =   u_state[3][3][ii];
                    } else {
                        u_state[0][jj][ii] =   u_state[0][2][ii];
                        u_state[1][jj][ii] =   u_state[1][2][ii];
                        u_state[2][jj][ii] = - u_state[2][2][ii];
                        u_state[3][jj][ii] =   u_state[3][2][ii];
                    }
                    
                } else if (jj > x2grid_size - 3) {
                    if (jj == x2grid_size - 1){
                        u_state[0][jj][ii] =   u_state[0][x2grid_size - 4][ii];
                        u_state[1][jj][ii] =   u_state[1][x2grid_size - 4][ii];
                        u_state[2][jj][ii] = - u_state[2][x2grid_size - 4][ii];
                        u_state[3][jj][ii] =   u_state[3][x2grid_size - 4][ii];
                    } else {
                        u_state[0][jj][ii] =   u_state[0][x2grid_size - 3][ii];
                        u_state[1][jj][ii] =   u_state[1][x2grid_size - 3][ii];
                        u_state[2][jj][ii] = - u_state[2][x2grid_size - 3][ii];
                        u_state[3][jj][ii] =   u_state[3][x2grid_size - 3][ii];
                    }

                }
            }
        }

    }
};


//=======================================================================================================
//                                      NEWTONIAN HYDRO
//=======================================================================================================

//----------------------------------------------------------------------------------------------------------
//  PRESSURE CALCULATIONS
//---------------------------------------------------------------------------------------------------------

double calc_pressure(float gamma, double rho, double energy, double v){
    double pressure = (gamma - 1.)*(energy - 0.5*rho*v*v);
    return pressure;
};

//------------------------------------------------------------------------------------------------------------
//  ENERGY CALCULATIONS
//------------------------------------------------------------------------------------------------------------

double calc_energy(float gamma, double rho, double pressure, double v){
        return pressure/(gamma-1.) + 0.5*rho*v*v;
};


//=======================================================================================================
//                                      RELATIVISITC HYDRO
//=======================================================================================================
int kronecker(int i, int j){
    if (i == j){
        return 1;
    } else{
        return 0;
    }
}

//---------------------------------------------------------------------------------------------------------
//  DERIVATIVE ALGORITHMS 
//---------------------------------------------------------------------------------------------------------
double central_difference(double x, double (*f)(double), double h){
    /**
     * Calculate the derivative of a function to 6th order
     * accuracy
     */

    double m1, m2, m3;

    m1 = ( (*f)(x + h/2) - (*f)(x - h/2))/h;

    m2 = ( (*f)(x + h) - (*f)(x - h))/(2*h);

    m3 = ( (*f)(x + 3*h/2) - (*f)(x - 3*h/2))/(3*h);
}


//------------------------------------------------------------------------------------------------------------
//  SPECIFIC ENTHALPY CALCULATIONS
//------------------------------------------------------------------------------------------------------------
double calc_enthalpy(float gamma, double rho, double pressure){
        return 1 + gamma*pressure/(rho*(gamma - 1));
};

double epsilon_rel(double pressure, double D, double tau, double lorentz_gamma){
    return ( tau + D*(1 - lorentz_gamma) + (1- lorentz_gamma*lorentz_gamma)*pressure )/(D*lorentz_gamma);
}

double rho_rel(double D, double lorentz_gamma, double root_g){
    return D/(lorentz_gamma*root_g);
}
//------------------------------------------------------------------------------------------------------------
//  VELOCITY CALCULATION
//------------------------------------------------------------------------------------------------------------
double calc_velocity(double s, double tau, double pressure, double D, double root_g){
    // Compute the 3-velocity given relaitivistic quanrities
    return s/(tau + root_g*pressure + D);
}

//------------------------------------------------------------------------------------------------------------
//  LORENTZ FACTOR CALCULATION
//------------------------------------------------------------------------------------------------------------
double calc_lorentz_gamma(double v){
    return 1/sqrt(1 - v*v);
}

vector<double> calc_lorentz_gamma(vector<double> &v){
    int vsize = v.size();
    vector<double> W(vsize); 

    for (int ii=0; ii < vsize; ii++){
        W[ii] = 1/sqrt(1 - v[ii]*v[ii]);
    }

    return W;
}

vector<vector<double> > calc_lorentz_gamma(vector<vector<double> > &v1, vector<vector<double> > &v2){
    int xgrid_size = v1[0].size();
    int ygrid_size = v1.size();
    double vtot;
    vector<vector<double> > W(ygrid_size, vector<double> (xgrid_size, 0)); 

    for (int jj=0; jj < ygrid_size; jj++){
        for (int ii=0; ii < xgrid_size; ii++){
            vtot = sqrt(v1[jj][ii]*v1[jj][ii] + v2[jj][ii]*v2[jj][ii]);

            W[jj][ii] = 1/sqrt(1 - vtot*vtot);
        }
    }
    
    return W;
}


double calc_rel_sound_speed(double pressure, double D, double tau, double lorentz_gamma, float gamma){
    double epsilon = epsilon_rel(pressure, D, tau, lorentz_gamma);

    return sqrt((gamma - 1)*gamma*epsilon/(1 + gamma*epsilon));
}
//------------------------------------------------------------------------------------------------------------
//  F-FUNCTION FOR ROOT FINDING: F(P)
//------------------------------------------------------------------------------------------------------------
double pressure_func(double pressure, double D, double tau, double lorentz_gamma, float gamma, double S){

    double rho = rho_rel(D, lorentz_gamma, 1);
    double epsilon = epsilon_rel(pressure, D, tau, lorentz_gamma);

    return (gamma - 1)*rho*epsilon - pressure;
}

double dfdp(double pressure, double D, double tau, double lorentz_gamma, float gamma, double S){

    double cs = calc_rel_sound_speed(pressure, D, tau, lorentz_gamma, gamma);
    double v = S/(tau + D + pressure);

    return v*v*cs*cs - 1.;
}