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

#pragma once

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>

using namespace std;
#define number_inputs 5
#define number_outputs 3
#define legendre_degree 4
#define number_rom_coefficients 1024

class ROMBase
{
public:
  /// Constructor for the class
  ROMBase(/*const unsigned int number_inputs = 0 ,
          unsigned int number_outputs = 0,
          unsigned int legendre_degree = 0,
          unsigned int number_rom_coefficients = 0*/);

  ///@{Matrice size parameters for the specific ROM calculation
  // unsigned int number_inputs = 6;
  // unsigned int number_outputs = 6;
  // unsigned int legendre_degree = 3;
  // unsigned int number_rom_coefficients = 729;
  ///@}

protected:
  /**
   * Assemble the array of Legendre polynomials to be multiplied by the ROM
   * coefficients
   */
  void polymake(double (&value)[number_outputs][number_inputs],
                double (&pvalues)[number_outputs][number_inputs][legendre_degree]);

  /**
   * Calculate the zeroth, first, and second order Legendre polynomials for each
   * input variable
   */
  double poly(double & val, int & d);

  /**
   * Arranges the calculated Legendre polynomials into the order expected by the
   * ROM coefficients
   */
  void framemake(double (&pvalues)[number_outputs][number_inputs][legendre_degree],
                 double (&xvalues)[number_outputs][number_rom_coefficients]);

  /**
   * Multiplies the Legendre polynomials by the ROM coefficients to compute the
   * the predicted output values
   */
  void predict(double (&xvalues)[number_outputs][number_rom_coefficients],
               double (&beta)[number_outputs][number_rom_coefficients],
               double (&romout)[number_outputs]);
};
