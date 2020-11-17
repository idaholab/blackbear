/******************************************************************************/
/*                         DO NOT MODIFY THIS HEADER                          */
/*                                                                            */
/*                Copyright 2015 Battelle Energy Alliance, LLC                */
/*             Copyright (2017) Los Alamos National Security, LLC             */
/*                            ALL RIGHTS RESERVED                             */
/*                                                                            */
/* This material was produced under U.S. Government contract                  */
/* DE-AC52-06NA25396 and DE-AC07-05ID14517 for Los Alamos National            */
/* Laboratory, which is operated by Los Alamos National Security, LLC and     */
/* Idaho National Laboratory, which is operated by Battelle Energy Alliance,  */
/* LLC for the U.S. Department of Energy. The Government is granted for       */
/* itself and others acting on its behalf a paid-up, nonexclusive,            */
/* irrevocable worldwide license in this material to reproduce, prepare       */
/* derivative works, and perform publicly and display publicly. Beginning     */
/* five (5) years after May 17, 2017, subject to additional five-year         */
/* worldwide renewals, the Government is granted for itself and others acting */
/* on its behalf a paid-up, nonexclusive, irrevocable worldwide license in    */
/* this material to reproduce, prepare derivative works, distribute copies to */
/* the public, perform publicly and display publicly, and to permit others to */
/* do so. NEITHER THE UNITED STATES NOR THE UNITED STATES DEPARTMENT OF       */
/* ENERGY, NOR LOS ALAMOS NATIONAL SECURITY, LLC, NOR BATTELLE ENERGY         */
/* ALLIANCE, LLC, NOR ANY OF THEIR CONTRACTORS, NOR ANY OF THEIR EMPLOYEES,   */
/* MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LEGAL LIABILITY OR  */
/* RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR USEFULNESS OF ANY        */
/* INFORMATION, APPARATUS, PRODUCT, OR PROCESS DISCLOSED, OR REPRESENTS THAT  */
/* ITS USE WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS.                         */
/******************************************************************************/

#pragma once

#include "ADLAROMANCEStressUpdateBase.h"

class P91LAROMANCEStressUpdate : public ADLAROMANCEStressUpdateBase
{
public:
  static InputParameters validParams();

  P91LAROMANCEStressUpdate(const InputParameters & parameters);

protected:
  virtual std::vector<std::vector<std::vector<ROMInputTransform>>> getTransform() override;
  virtual std::vector<std::vector<std::vector<Real>>> getTransformCoefs() override;
  virtual std::vector<std::vector<std::vector<Real>>> getNormalizationLimits() override;
  virtual std::vector<std::vector<std::vector<Real>>> getInputLimits() override;
  virtual std::vector<std::vector<std::vector<Real>>> getCoefs() override;
  virtual std::vector<unsigned int> getTilings() override;
};
