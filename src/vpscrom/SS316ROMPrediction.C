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

// looks like the constructor comes next here, if I use it...
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
}

void
SS316ROMPrediction::computeROMDerivative(const double & dt,
                                         const double & rhom0,
                                         const double & rhoi0,
                                         const double & vonmises0,
                                         const double & evm0,
                                         const double & temperature0,
                                         double & delta_evm)
{
  double args[number_inputs] = {rhom0, rhoi0, vonmises0, evm0, temperature0};
  double rominputs[number_outputs][number_inputs];
  double dpolyinputs[number_outputs][number_inputs][legendre_degree];
  double dxvals[number_outputs][number_rom_coefficients];
  double dromouts[number_outputs];
  double dfeadelta[number_outputs];

  // Call the original ROM prediction methods as defined below
  convert(args, rominputs);
  dpolymake(rominputs, dpolyinputs);
  framemake(dpolyinputs, dxvals);
  predict(dxvals, betas, dromouts);
  dunconvert(dt, dromouts, dfeadelta);

  // sort out the values for the outputs back into the reference variables
  delta_evm = dfeadelta[2];
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
        feaout[i] = (1.0 * limit_rho_mobile_increment * limit_rho_mobile_increment) / temp_rhom +
                    limit_rho_mobile_increment;

      feaout[i] *= feains[i] * deltat;
    }
    else if (i == 1)
    {
      const double temp_rhoi = exp(romout[i]);
      if (temp_rhoi > limit_rho_immobile_increment)
        feaout[i] = temp_rhoi - limit_rho_immobile_increment;
      else
        feaout[i] =
            (-1.0 * limit_rho_immobile_increment * limit_rho_immobile_increment) / temp_rhoi +
            limit_rho_immobile_increment;

      feaout[i] *= feains[i] * deltat;
    }
    else if (i == 2)
    {
      feaout[i] = exp(romout[2]) * deltat;
    }
  }
}

void
SS316ROMPrediction::dpolymake(double (&value)[number_outputs][number_inputs],
                              double (&pvalues)[number_outputs][number_inputs][legendre_degree])
{
  for (unsigned int outvar = 0; outvar < number_outputs; ++outvar)
  {
    for (unsigned int invar = 0; invar < number_inputs; ++invar)
    {
      for (int degree = 0; degree < legendre_degree; ++degree)
        if (invar == 3) // case for the vonmises stress input
        {
          // Also need to include the shape function derivative
          double d_shapefunction = 1.0;
          if (fnames[outvar][invar] == "exp")
            d_shapefunction =
                2.0 * (exp(value[outvar][invar] / ffacs[outvar][invar]) / ffacs[outvar][invar]);
          else if (fnames[outvar][invar] == "log")
            d_shapefunction = 1.0 / (value[outvar][invar] / ffacs[outvar][invar]);

          d_shapefunction *= 2.0 / (maxs[outvar][invar] - mins[outvar][invar]);

          pvalues[outvar][invar][degree] = dpoly(value[outvar][invar], degree);
          pvalues[outvar][invar][degree] *= d_shapefunction;
        }
        else
          pvalues[outvar][invar][degree] = poly(value[outvar][invar], degree);
    }
  }
}

double
SS316ROMPrediction::dpoly(double & value, int & degree)
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
SS316ROMPrediction::dunconvert(const double & deltat,
                               double (&romout)[number_outputs],
                               double (&feaout)[number_outputs])
{
  for (unsigned int i = 0; i < number_outputs; ++i)
  {
    if (i == 2)
    {
      feaout[i] =
          exp(romout[2]) * deltat; // not sure about the stress multiplier in this application
    }
  }
}
