// $Id: conditionalUniforMeasPdf1d.cpp TDeLaet $
// Copyright (C) 2007  Tinne De Laet <first dot last at mech dot kuleuven dot be>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
//

#include "conditionalUniformMeasPdf1d.h"
#include <wrappers/rng/rng.h> // Wrapper around several rng libraries

#define MEASMODEL_NUMCONDARGUMENTS 1
#define MEASMODEL_DIMENSION 1

namespace BFL
{
  using namespace MatrixWrapper;

  ConditionalUniformMeasPdf1d::ConditionalUniformMeasPdf1d(const Gaussian& measNoise)
    : ConditionalPdf<ColumnVector,int>(MEASMODEL_DIMENSION,MEASMODEL_NUMCONDARGUMENTS)
  {
    _measNoise = measNoise;
  }


  ConditionalUniformMeasPdf1d::~ConditionalUniformMeasPdf1d(){}

  Probability
  ConditionalUniformMeasPdf1d::ProbabilityGet(const ColumnVector& measurement) const
  {
    // simplified version: the probability of a measurement is just the
    // probability under the additive Gaussian noise. The discrete nature of the
    // underlying state is not taken into account
    int state = ConditionalArgumentGet(0);
    ColumnVector expected_measurement(1);
    // the expected measurement in this simplified 1d example is just two times
    // the position of the 1d mobile robot
    expected_measurement(1) = 2 * state;

    return _measNoise.ProbabilityGet(expected_measurement-measurement);
  }

}//namespace BFL

