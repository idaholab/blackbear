/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                       BlackBear                              */
/*                                                              */
/*           (c) 2017 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "SS316ROMPrediction.h"

SS316ROMPrediction::SS316ROMPrediction(){};

void
SS316ROMPrediction::computeROMPredictions(const double & dt,
                                          const double & rhom0,
                                          const double & rhoi0,
                                          const double & vonmises0,
                                          const double & evm0,
                                          const double & temperature0,
                                          double & delta_rhom,
                                          double & delta_rhoi,
                                          double & delta_evm)
{
  double args[number_inputs] = {rhom0, rhoi0, vonmises0, evm0, temperature0};
  double feains[number_outputs] = {rhom0, rhoi0, evm0};
  double rominputs[number_outputs][number_inputs];
  double polyinputs[number_outputs][number_inputs][legendre_degree];
  double xvals[number_outputs][number_rom_coefficients];
  double romouts[number_outputs];
  double feadelta[number_outputs];

  // Call the original ROM prediction methods as defined below
  convert(args, rominputs);
  polymake(rominputs, polyinputs);
  framemake(polyinputs, xvals);
  predict(xvals, betas, romouts);
  unconvert(dt, feains, romouts, feadelta);

  // sort out the values for the outputs back into the reference variables (if can't pass by
  // reference into the outputs array...)
  delta_rhom = feadelta[0];
  delta_rhoi = feadelta[1];
  delta_evm = feadelta[2];

  // std::cout << "Within the ROMPrediction value the calculated delta_rhom is: " << delta_rhom << "\n";
  // std::cout << "  and the ROMPrediction effective strain value is: " << delta_evm << "\n";
}

void
SS316ROMPrediction::computeROMDerivative(const double & vonmises0,
                                         const double & derivative_vonmises0_delp,
                                         const double & derivative_evm_drom,
                                         double & complete_derivative)
{
  const unsigned int trial_stress_index = 2;
  const unsigned int increment_strain_index = 2;

  double dconvert_dvonmises;
  double rominput_vonmises;
  double dpolyinputs[legendre_degree];
  double dxvals[number_rom_coefficients];
  double dromouts;

  // Call the original ROM prediction methods as defined below
  dconvert(vonmises0, trial_stress_index, increment_strain_index, rominput_vonmises, dconvert_dvonmises);
  dpolymake(rominput_vonmises, dpolyinputs);
  dframemake(dpolyinputs, trial_stress_index, dxvals);
  dpredict(dxvals, increment_strain_index, betas, dromouts);
  // NOTE: dunconvert is not required because d(exp(x))/dx = exp(x)
  // instead of recomputing we reuse the calculation from the residual method

  complete_derivative = derivative_vonmises0_delp * dconvert_dvonmises * dromouts * derivative_evm_drom;
}

// after all of the sorting is completed with the parameter setup from the "callable method",
// the original functions are here below:

void
SS316ROMPrediction::convert(double (&arg)[number_inputs], double (&value)[number_outputs][number_inputs])
{
  for (unsigned int i = 0; i < number_outputs; ++i)
  {
    for (unsigned int j = 0; j < number_inputs; ++j)
    {
      if (fnames[i][j] == "exp")
        value[i][j] =
            2.0 * (exp(arg[j] / ffacs[i][j]) - mins[i][j]) / (maxs[i][j] - mins[i][j]) - 1.0;
      else if (fnames[i][j] == "log")
        value[i][j] =
            2.0 * (log(arg[j] + ffacs[i][j]) - mins[i][j]) / (maxs[i][j] - mins[i][j]) - 1.0;
      else
        value[i][j] = 2.0 * (arg[j] - mins[i][j]) / (maxs[i][j] - mins[i][j]) - 1.0;
    }
  }
}

void
SS316ROMPrediction::unconvert(const double & deltat,
                              double (&feains)[number_outputs],
                              double (&romout)[number_outputs],
                              double (&feaout)[number_outputs])
{
  for (unsigned int i = 0; i < number_outputs; ++i)
  {
    if (i == 0)
    {
      const double temp_rhom = exp(romout[i]);
      if (temp_rhom > limit_rho_mobile_increment)
        feaout[i] = limit_rho_mobile_increment - temp_rhom;
      else
        feaout[i] = (limit_rho_mobile_increment * limit_rho_mobile_increment) / temp_rhom - limit_rho_mobile_increment;

      feaout[i] *= feains[i] * deltat;
    }
    else if (i == 1)
    {
      const double temp_rhoi = exp(romout[i]);
      if (temp_rhoi > limit_rho_immobile_increment)
        feaout[i] = temp_rhoi - limit_rho_immobile_increment;
      else
        feaout[i] = limit_rho_immobile_increment -
            (limit_rho_immobile_increment * limit_rho_immobile_increment) / temp_rhoi;

      feaout[i] *= feains[i] * deltat;
    }
    else if (i == 2)
    {
      feaout[i] = exp(romout[2]) * deltat;
    }
  }
}

void
SS316ROMPrediction::dconvert(const double & trial_stress_mpa,
                             const unsigned int & stress_index,
                             const unsigned int & strain_index,
                             double & rominput_vonmises,
                             double & dconvert_dvonmises)
{
  // derivative for input = vonmises stress, output = strain increment
  const double denom = (maxs[strain_index][stress_index] - mins[strain_index][stress_index]) *
                       (trial_stress_mpa + ffacs[strain_index][stress_index]);
  dconvert_dvonmises = 2.0 / denom;

  // original rom input for input = vonmises stress, output = strain increment
  const double nomin =
      log(trial_stress_mpa + ffacs[strain_index][stress_index]) - mins[strain_index][stress_index];
  const double romconvert =
      nomin / (maxs[strain_index][stress_index] - mins[strain_index][stress_index]);
  rominput_vonmises = 2.0 * romconvert - 1.0;
}

void
SS316ROMPrediction::dpolymake(const double &value, double (&dpvalues)[legendre_degree])
{
  for (int degree = 0; degree < legendre_degree; ++degree)
    dpvalues[degree] = dpoly(value, degree);
}

double
SS316ROMPrediction::dpoly(const double & value, int & degree)
{
  if (degree == 0)
    return 0.0;
  else if (degree == 1)
    return 1.0;
  else if (degree == 2)
    return 3.0 * value;
  else if (degree == 3)
    return 7.0 * pow(value, 2) - 1.5;
  else
    terminate();
}

void
SS316ROMPrediction::dframemake(const double (&dpvalues)[legendre_degree],
                    const unsigned int & stress_index,
                    double (&dxvalues)[number_rom_coefficients])
{
  int temp;
  int bases;
  for (unsigned int numcoeffs = 0; numcoeffs < number_rom_coefficients; ++numcoeffs)
  {
    temp = floor(numcoeffs / pow(legendre_degree, stress_index));
    bases = temp % legendre_degree;

    dxvalues[numcoeffs] = 1.0;
    dxvalues[numcoeffs] *= dpvalues[bases];
  }
}

void
SS316ROMPrediction::dpredict(const double (&dxvalues)[number_rom_coefficients],
                  const unsigned int & strain_index,
                  double (&betas)[number_outputs][number_rom_coefficients],
                  double & romout)
{
  romout = 0.0;
  for (unsigned int numcoeffs = 0; numcoeffs < number_rom_coefficients; ++numcoeffs)
    romout += dxvalues[numcoeffs] * betas[strain_index][numcoeffs];
}
