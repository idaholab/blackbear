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

#include "ROMBase.h"

// looks like the constructor goes here
ROMBase::ROMBase(/*unsigned int number_inputs,
                 unsigned int number_outputs,
                 unsigned int legendre_degree,
                 unsigned int number_rom_coefficients*/){};

void
ROMBase::polymake(double (&value)[number_outputs][number_inputs],
                  double (&pvalues)[number_outputs][number_inputs][legendre_degree])
{
  for (unsigned int outvar = 0; outvar < number_outputs; ++outvar)
  {
    for (unsigned int invar = 0; invar < number_inputs; ++invar)
    {
      for (int degree = 0; degree < legendre_degree; ++degree)
        pvalues[outvar][invar][degree] = poly(value[outvar][invar], degree);
    }
  }
}

double
ROMBase::poly(double & value, int & degree)
{
  if (degree == 0)
    return 1.0;
  else if (degree == 1)
    return 1.0 * value;
  else if (degree == 2)
    return 1.5 * pow(value, 2) - 0.5;
  else if (degree == 3)
    return 2.5 * pow(value, 3) - 1.5 * value;
  else
    terminate();
}

void
ROMBase::framemake(double (&pvalues)[number_outputs][number_inputs][legendre_degree],
                   double (&xvalues)[number_outputs][number_rom_coefficients])
{
  int bases[number_inputs];
  int temp;
  for (unsigned int outvar = 0; outvar < number_outputs; ++outvar)
  {
    for (unsigned int numcoeffs = 0; numcoeffs < number_rom_coefficients; ++numcoeffs)
    {
      for (unsigned int invar = 0; invar < number_inputs; ++invar)
      {
        temp = floor(numcoeffs / pow(legendre_degree, invar));
        bases[invar] = temp % legendre_degree;
      }

      xvalues[outvar][numcoeffs] = 1.0;
      // Multiply the coefficients within the material specific class
      for (unsigned int invar = 0; invar < number_inputs; ++invar)
        xvalues[outvar][numcoeffs] *= pvalues[outvar][invar][bases[invar]];

    }
  }
}

void
ROMBase::predict(double (&xvalues)[number_outputs][number_rom_coefficients],
                 double (&betas)[number_outputs][number_rom_coefficients],
                 double (&romout)[number_outputs])
{
  for (unsigned int outvar = 0; outvar < number_outputs; ++outvar)
  {
    romout[outvar] = 0.0;
    for (unsigned int numcoeffs = 0; numcoeffs < number_rom_coefficients; ++numcoeffs)
      romout[outvar] += xvalues[outvar][numcoeffs] * betas[outvar][numcoeffs];
  }
}
