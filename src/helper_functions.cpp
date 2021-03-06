
/*
* helper_functions.cpp is where all of the universal functions that can be used
* for all N-Dim hydro calculations
*/

#include "helper_functions.h" 
#include "hydro_structs.h"
#include <cmath>
#include <map>
#include <algorithm>
#include <cstdarg>


using namespace std;

// =========================================================================================================
//        HELPER FUNCTIONS FOR COMPUTATION
// =========================================================================================================

// Convert a vector of structs into a struct of vectors for easy post processsing
sr2d::PrimitiveData vecs2struct(const vector<sr2d::Primitive> &p){
    sr2d::PrimitiveData sprims;
    size_t nzones = p.size();
    sprims.rho.reserve(nzones);
    sprims.v1.reserve(nzones);
    sprims.v2.reserve(nzones);
    sprims.p.reserve(nzones);
    for (size_t i = 0; i < nzones; i++)
    {
        sprims.rho.push_back(p[i].rho);
        sprims.v1.push_back(p[i].v1);
        sprims.v2.push_back(p[i].v2);
        sprims.p.push_back(p[i].p);
    }
    
    return sprims;
}

// Find the max element
double findMax(double a, double b, double c ){
    //Find max b/w a & b first
    double inter_max = max(a, b);

    double max_val = max(inter_max, c);

    return max_val;
};

double findMin(double a, double b, double c ){
    //Find max b/w a & b first
    double inter_min = min(a, b);

    double min_val = min(inter_min, c);

    return min_val;
};

// Sound Speed Function
double calc_sound_speed(float gamma, double rho, double pressure){
    double c = sqrt(gamma*pressure/rho);
    return c;

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

// Roll a single vector index in y-direction of lattice
double roll(vector<vector<double>>  &v, unsigned int xpos, unsigned int ypos) {
   return v[ypos % v.size()][xpos % v[0].size()];
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

void compute_vertices(vector<double> &cz, 
                      vector<double> &xv, 
                      int lx, 
                      bool linspace){
    xv[0]  = cz[0];
    xv[lx] = cz[lx-1];
    for (size_t i = 1; i < lx; i++)
    {
        xv[i] = linspace ? 0.5 * (cz[i] + cz[i-1]) : sqrt(cz[i] * cz[i-1]);
    };
}

std::map<std::string, simbi::Geometry> geometry;
void config_system() {
    geometry["cartesian"] = simbi::Geometry::CARTESIAN;
    geometry["spherical"] = simbi::Geometry::SPHERICAL;
}


void config_ghosts2D(
    std::vector<hydro2d::Conserved> &u_state, 
    int x1grid_size, 
    int x2grid_size, 
    bool first_order)
{

    if (first_order){
        for (int jj = 0; jj < x2grid_size; jj++){
            for (int ii = 0; ii < x1grid_size; ii++){
                if (jj < 1){
                    u_state[ii + x1grid_size * jj].rho    =   u_state[ii + x1grid_size].rho;
                    u_state[ii + x1grid_size * jj].m1     =   u_state[ii + x1grid_size].m1;
                    u_state[ii + x1grid_size * jj].m2     = - u_state[ii + x1grid_size].m2;
                    u_state[ii + x1grid_size * jj].e_dens =   u_state[ii + x1grid_size].e_dens;
                    
                } else if (jj > x2grid_size - 2) {
                    u_state[ii + x1grid_size * jj].rho     =   u_state[(x2grid_size - 2) * x1grid_size + ii].rho;
                    u_state[ii + x1grid_size * jj].m1      =   u_state[(x2grid_size - 2) * x1grid_size + ii].m1;
                    u_state[ii + x1grid_size * jj].m2      = - u_state[(x2grid_size - 2) * x1grid_size + ii].m2;
                    u_state[ii + x1grid_size * jj].e_dens  =   u_state[(x2grid_size - 2) * x1grid_size + ii].e_dens;

                } else {
                    u_state[jj * x1grid_size].rho         =   u_state[jj * x1grid_size + 1].rho;
                    u_state[jj * x1grid_size + 0].m1      = - u_state[jj * x1grid_size + 1].m1;
                    u_state[jj * x1grid_size + 0].m2      =   u_state[jj * x1grid_size + 1].m2;
                    u_state[jj * x1grid_size + 0].e_dens  =   u_state[jj * x1grid_size + 1].e_dens;

                    u_state[jj * x1grid_size + x1grid_size - 1].rho    =  u_state[jj*x1grid_size + x1grid_size - 2].rho;
                    u_state[jj * x1grid_size + x1grid_size - 1].m1     =  u_state[jj * x1grid_size + x1grid_size - 2].m1;
                    u_state[jj * x1grid_size + x1grid_size - 1].m2     =  u_state[jj * x1grid_size + x1grid_size - 2].m2;
                    u_state[jj * x1grid_size + x1grid_size - 1].e_dens =  u_state[jj * x1grid_size + x1grid_size - 2].e_dens;
                }
            }
        }

    } else {
        for (int jj = 0; jj < x2grid_size; jj++){

            // Fix the ghost zones at the radial boundaries
            u_state[jj * x1grid_size +  0].rho               = u_state[jj * x1grid_size +  3].rho;
            u_state[jj * x1grid_size +  1].rho               = u_state[jj * x1grid_size +  2].rho;
            u_state[jj * x1grid_size +  x1grid_size - 1].rho = u_state[jj * x1grid_size +  x1grid_size - 3].rho;
            u_state[jj * x1grid_size +  x1grid_size - 2].rho = u_state[jj * x1grid_size +  x1grid_size - 3].rho;

            u_state[jj * x1grid_size + 0].m1               = - u_state[jj * x1grid_size + 3].m1;
            u_state[jj * x1grid_size + 1].m1               = - u_state[jj * x1grid_size + 2].m1;
            u_state[jj * x1grid_size + x1grid_size - 1].m1 =   u_state[jj * x1grid_size + x1grid_size - 3].m1;
            u_state[jj * x1grid_size + x1grid_size - 2].m1 =   u_state[jj * x1grid_size + x1grid_size - 3].m1;

            u_state[jj * x1grid_size + 0].m2               = u_state[jj * x1grid_size + 3].m2;
            u_state[jj * x1grid_size + 1].m2               = u_state[jj * x1grid_size + 2].m2;
            u_state[jj * x1grid_size + x1grid_size - 1].m2 = u_state[jj * x1grid_size + x1grid_size - 3].m2;
            u_state[jj * x1grid_size + x1grid_size - 2].m2 = u_state[jj * x1grid_size + x1grid_size - 3].m2;

            u_state[jj * x1grid_size + 0].e_dens                = u_state[jj * x1grid_size + 3].e_dens;
            u_state[jj * x1grid_size + 1].e_dens                = u_state[jj * x1grid_size + 2].e_dens;
            u_state[jj * x1grid_size + x1grid_size - 1].e_dens  = u_state[jj * x1grid_size + x1grid_size - 3].e_dens;
            u_state[jj * x1grid_size + x1grid_size - 2].e_dens  = u_state[jj * x1grid_size + x1grid_size - 3].e_dens;

            // Fix the ghost zones at the angular boundaries
            if (jj < 2){
                for (int ii = 0; ii < x1grid_size; ii++){
                     if (jj == 0){
                        u_state[jj * x1grid_size + ii].rho    =   u_state[3 * x1grid_size + ii].rho;
                        u_state[jj * x1grid_size + ii].m1     =   u_state[3 * x1grid_size + ii].m1;
                        u_state[jj * x1grid_size + ii].m2     = - u_state[3 * x1grid_size + ii].m2;
                        u_state[jj * x1grid_size + ii].e_dens =   u_state[3 * x1grid_size + ii].e_dens;
                    } else {
                        u_state[jj * x1grid_size + ii].rho     =   u_state[2 * x1grid_size + ii].rho;
                        u_state[jj * x1grid_size + ii].m1      =   u_state[2 * x1grid_size + ii].m1;
                        u_state[jj * x1grid_size + ii].m2      = - u_state[2 * x1grid_size + ii].m2;
                        u_state[jj * x1grid_size + ii].e_dens  =   u_state[2 * x1grid_size + ii].e_dens;
                    }
                }
            } else if (jj > x2grid_size - 3) {
                for (int ii = 0; ii < x1grid_size; ii++){
                    if (jj == x2grid_size - 1){
                        u_state[jj * x1grid_size + ii].rho    =   u_state[(x2grid_size - 4) * x1grid_size + ii].rho;
                        u_state[jj * x1grid_size + ii].m1     =   u_state[(x2grid_size - 4) * x1grid_size + ii].m1;
                        u_state[jj * x1grid_size + ii].m2     = - u_state[(x2grid_size - 4) * x1grid_size + ii].m2;
                        u_state[jj * x1grid_size + ii].e_dens =   u_state[(x2grid_size - 4) * x1grid_size + ii].e_dens;
                    } else {
                        u_state[jj * x1grid_size + ii].rho    =   u_state[(x2grid_size - 3) * x1grid_size + ii].rho;
                        u_state[jj * x1grid_size + ii].m1     =   u_state[(x2grid_size - 3) * x1grid_size + ii].m1;
                        u_state[jj * x1grid_size + ii].m2     = - u_state[(x2grid_size - 3) * x1grid_size + ii].m2;
                        u_state[jj * x1grid_size + ii].e_dens =   u_state[(x2grid_size - 3) * x1grid_size + ii].e_dens;
                    }
                }
            }
            
        }

    }
};

void config_ghosts2D(
    std::vector<sr2d::Conserved> &u_state, 
    int x1grid_size, 
    int x2grid_size, 
    bool first_order,
    std::string kind){

    if (first_order){
        for (int jj = 0; jj < x2grid_size; jj++){
            for (int ii = 0; ii < x1grid_size; ii++){
                if (jj < 1){
                    u_state[ii + x1grid_size * jj].D   =   u_state[ii + x1grid_size].D;
                    u_state[ii + x1grid_size * jj].S1  =   u_state[ii + x1grid_size].S1;
                    u_state[ii + x1grid_size * jj].S2  = - u_state[ii + x1grid_size].S2;
                    u_state[ii + x1grid_size * jj].tau =   u_state[ii + x1grid_size].tau;
                    
                } else if (jj > x2grid_size - 2) {
                    u_state[ii + x1grid_size * jj].D    =   u_state[(x2grid_size - 2) * x1grid_size + ii].D;
                    u_state[ii + x1grid_size * jj].S1   =   u_state[(x2grid_size - 2) * x1grid_size + ii].S1;
                    u_state[ii + x1grid_size * jj].S2   = - u_state[(x2grid_size - 2) * x1grid_size + ii].S2;
                    u_state[ii + x1grid_size * jj].tau  =   u_state[(x2grid_size - 2) * x1grid_size + ii].tau;

                } else {
                    u_state[jj * x1grid_size].D    = u_state[jj * x1grid_size + 1].D;
                    u_state[jj * x1grid_size + x1grid_size - 1].D = u_state[jj*x1grid_size + x1grid_size - 2].D;

                    u_state[jj * x1grid_size + 0].S1               = - u_state[jj * x1grid_size + 1].S1;
                    u_state[jj * x1grid_size + x1grid_size - 1].S1 =   u_state[jj * x1grid_size + x1grid_size - 2].S1;

                    u_state[jj * x1grid_size + 0].S2                = u_state[jj * x1grid_size + 1].S2;
                    u_state[jj * x1grid_size + x1grid_size - 1].S2  = u_state[jj * x1grid_size + x1grid_size - 2].S2;

                    u_state[jj * x1grid_size + 0].tau               = u_state[jj * x1grid_size + 1].tau;
                    u_state[jj * x1grid_size + x1grid_size - 1].tau = u_state[jj * x1grid_size + x1grid_size - 2].tau;
                }
            }
        }

    } else {
        for (int jj = 0; jj < x2grid_size; jj++){

            // Fix the ghost zones at the radial boundaries
            u_state[jj * x1grid_size +  0].D               = u_state[jj * x1grid_size +  3].D;
            u_state[jj * x1grid_size +  1].D               = u_state[jj * x1grid_size +  2].D;
            u_state[jj * x1grid_size +  x1grid_size - 1].D = u_state[jj * x1grid_size +  x1grid_size - 3].D;
            u_state[jj * x1grid_size +  x1grid_size - 2].D = u_state[jj * x1grid_size +  x1grid_size - 3].D;

            u_state[jj * x1grid_size + 0].S1               = - u_state[jj * x1grid_size + 3].S1;
            u_state[jj * x1grid_size + 1].S1               = - u_state[jj * x1grid_size + 2].S1;
            u_state[jj * x1grid_size + x1grid_size - 1].S1 =   u_state[jj * x1grid_size + x1grid_size - 3].S1;
            u_state[jj * x1grid_size + x1grid_size - 2].S1 =   u_state[jj * x1grid_size + x1grid_size - 3].S1;

            u_state[jj * x1grid_size + 0].S2               = u_state[jj * x1grid_size + 3].S2;
            u_state[jj * x1grid_size + 1].S2               = u_state[jj * x1grid_size + 2].S2;
            u_state[jj * x1grid_size + x1grid_size - 1].S2 = u_state[jj * x1grid_size + x1grid_size - 3].S2;
            u_state[jj * x1grid_size + x1grid_size - 2].S2 = u_state[jj * x1grid_size + x1grid_size - 3].S2;

            u_state[jj * x1grid_size + 0].tau               = u_state[jj * x1grid_size + 3].tau;
            u_state[jj * x1grid_size + 1].tau               = u_state[jj * x1grid_size + 2].tau;
            u_state[jj * x1grid_size + x1grid_size - 1].tau  = u_state[jj * x1grid_size + x1grid_size - 3].tau;
            u_state[jj * x1grid_size + x1grid_size - 2].tau  = u_state[jj * x1grid_size + x1grid_size - 3].tau;

            // Fix the ghost zones at the angular boundaries
            if (jj < 2){
                for (int ii = 0; ii < x1grid_size; ii++){
                     if (jj == 0){
                        u_state[jj * x1grid_size + ii].D   =   u_state[3 * x1grid_size + ii].D;
                        u_state[jj * x1grid_size + ii].S1  =   u_state[3 * x1grid_size + ii].S1;
                        u_state[jj * x1grid_size + ii].S2  = - u_state[3 * x1grid_size + ii].S2;
                        u_state[jj * x1grid_size + ii].tau =   u_state[3 * x1grid_size + ii].tau;
                    } else {
                        u_state[jj * x1grid_size + ii].D    =   u_state[2 * x1grid_size + ii].D;
                        u_state[jj * x1grid_size + ii].S1   =   u_state[2 * x1grid_size + ii].S1;
                        u_state[jj * x1grid_size + ii].S2   = - u_state[2 * x1grid_size + ii].S2;
                        u_state[jj * x1grid_size + ii].tau  =   u_state[2 * x1grid_size + ii].tau;
                    }
                }
            } else if (jj > x2grid_size - 3) {
                for (int ii = 0; ii < x1grid_size; ii++){
                    if (jj == x2grid_size - 1){
                        u_state[jj * x1grid_size + ii].D   =   u_state[(x2grid_size - 4) * x1grid_size + ii].D;
                        u_state[jj * x1grid_size + ii].S1  =   u_state[(x2grid_size - 4) * x1grid_size + ii].S1;
                        u_state[jj * x1grid_size + ii].S2  = - u_state[(x2grid_size - 4) * x1grid_size + ii].S2;
                        u_state[jj * x1grid_size + ii].tau =   u_state[(x2grid_size - 4) * x1grid_size + ii].tau;
                    } else {
                        u_state[jj * x1grid_size + ii].D   =   u_state[(x2grid_size - 3) * x1grid_size + ii].D;
                        u_state[jj * x1grid_size + ii].S1  =   u_state[(x2grid_size - 3) * x1grid_size + ii].S1;
                        u_state[jj * x1grid_size + ii].S2  = - u_state[(x2grid_size - 3) * x1grid_size + ii].S2;
                        u_state[jj * x1grid_size + ii].tau =   u_state[(x2grid_size - 3) * x1grid_size + ii].tau;
                    }
                }
            }
            
        }

    }
};


//====================================================================================================
//                                  WRITE DATA TO FILE
//====================================================================================================
void toWritePrim(sr1d::PrimitiveArray *from, PrimData *to)
{
    to->rho  = from->rho;
    to->v    = from->v;
    to->p    = from->p;

}

void toWritePrim(sr2d::PrimitiveData *from, PrimData *to)
{
    to->rho  = from->rho;
    to->v1   = from->v1;
    to->v2   = from->v2;
    to->p    = from->p;
}

string create_step_str(double t_interval, string &tnow){

    // Convert the time interval into an int with 2 decimal displacements
    int t_interval_int = round( 1.e3 * t_interval );
    int a, b;

    string s = to_string(t_interval_int);

    // Pad the file string if size less than tnow_size
    if (s.size() < tnow.size()) {

        int num_zeros = tnow.size() - s.size();
        string pad_zeros = string(num_zeros, '0');
        s.insert(0, pad_zeros);

    }

    for (int i = 0; i < 7; i++){
        a = tnow[i] - '0';
        b = s[i] - '0';
        s[i] = a + b + '0';
    }

    return s;


}
void write_hdf5(string data_directory, string filename, PrimData prims, DataWriteMembers setup, int dim = 2)
{
    string filePath = data_directory;
    cout << "\n" <<  "Writing File...: " << filePath + filename << endl;
    h5::fd_t fd = h5::create(filePath + filename, H5F_ACC_TRUNC, h5::default_fcpl,
                    h5::libver_bounds({H5F_LIBVER_V18, H5F_LIBVER_V18}) );

    // Generate Dataset Object to Write Attributes
    vector<int> sim_data(10, 0);
    auto ds = h5::write(fd, "sim_info", sim_data);
    
    switch (dim)
    {
    case 2:
        // Write the Primitives 
        h5::write(fd,"rho", prims.rho);
        h5::write(fd,"v1",  prims.v1);
        h5::write(fd,"v2",  prims.v2);
        h5::write(fd,"p",   prims.p);

        // Write Datset Attributes
        ds["current_time"]   = setup.t;
        ds["time_step"]      = setup.dt;
        ds["NX"]             = setup.NX;
        ds["NY"]             = setup.NY;
        ds["xmax"]           = setup.xmax;
        ds["xmin"]           = setup.xmin;
        ds["ymax"]           = setup.ymax;
        ds["ymin"]           = setup.ymin;
        ds["xactive_zones"]  = setup.xactive_zones;
        ds["yactive_zones"]  = setup.yactive_zones;
        break;
    
    case 1:
        // Write the Primitives 
        h5::write(fd,"rho", prims.rho);
        h5::write(fd,"v",   prims.v);
        h5::write(fd,"p",   prims.p);


        // Write Datset Attributes
        ds["current_time"]   = setup.t;
        ds["time_step"]      = setup.dt;
        ds["Nx"]             = setup.NX;
        ds["xmax"]           = setup.xmax;
        ds["xmin"]           = setup.xmin;
        ds["xactive_zones"]  = setup.xactive_zones;
        break;
    }
    
}


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


//------------------------------------------------------------------------------------------------------------
//  SPECIFIC ENTHALPY CALCULATIONS
//------------------------------------------------------------------------------------------------------------
double calc_enthalpy(float gamma, double rho, double pressure){
        return 1 + gamma*pressure/(rho*(gamma - 1));
};

double epsilon_rel(double pressure, double D, double tau, double lorentz_gamma){
    return ( tau + D*(1. - lorentz_gamma) + (1. - lorentz_gamma*lorentz_gamma)*pressure )/(D*lorentz_gamma);
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

double calc_intermed_wave(double energy_density, double momentum_density, 
                            double flux_momentum_density, 
                            double flux_energy_density)
{
    double a = flux_energy_density;
    double b = - (energy_density + flux_momentum_density);
    double c = momentum_density;
    double disc = sqrt( b*b - 4*a*c);
    double quad = -0.5*(b + sgn(b)*disc);
    return c/quad;
}


double calc_intermed_pressure(double a,double aStar, double energy, double norm_mom, double u, double p){

    double e, f, g;
    e = (a*energy - norm_mom)*aStar;
    f = norm_mom*(a - u) - p;
    g = 1 + a*aStar;

    return (e - f)/g;
}
//------------------------------------------------------------------------------------------------------------
//  LORENTZ FACTOR CALCULATION
//------------------------------------------------------------------------------------------------------------



double calc_rel_sound_speed(double pressure, double D, double tau, double lorentz_gamma, float gamma){
    double epsilon = epsilon_rel(pressure, D, tau, lorentz_gamma);

    return sqrt((gamma - 1)*gamma*epsilon/(1 + gamma*epsilon));
}
//------------------------------------------------------------------------------------------------------------
//  F-FUNCTION FOR ROOT FINDING: F(P)
//------------------------------------------------------------------------------------------------------------
double pressure_func(double pressure, double D, double tau, double lorentz_gamma, float gamma, double S){

    double v       = S / (tau + pressure + D);
    double W_s     = 1.0 / sqrt(1.0 - v * v);
    double rho     = D / W_s; 
    double epsilon = ( tau + D*(1. - W_s) + (1. - W_s*W_s)*pressure )/(D * W_s);

    return (gamma - 1.)*rho*epsilon - pressure;
}

double dfdp(double pressure, double D, double tau, double lorentz_gamma, float gamma, double S){

    double v       = S/(tau + D + pressure);
    double W_s     = 1.0 / sqrt(1.0 - v*v);
    double rho     = D / W_s; 
    double h       = 1 + pressure*gamma/(rho*(gamma - 1.));
    double c2      = gamma*pressure/(h*rho);
    

    return v*v*c2 - 1.;
}
