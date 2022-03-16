// $Id: test_kalman_smoother.cpp 5925 2006-03-14 21:23:49Z tdelaet $
// Copyright (C) 2006 Tinne De Laet <first dot last at mech dot kuleuven dot be>
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

/* Demonstration program for the Bayesian Filtering Library.
   Mobile robot localization with respect to wall with different possibilities for filter
*/


#include <filter/extendedkalmanfilter.h>

#include <model/linearanalyticsystemmodel_gaussianuncertainty.h>
#include <model/linearanalyticmeasurementmodel_gaussianuncertainty.h>

#include <pdf/analyticconditionalgaussian.h>
#include <pdf/analyticconditionalgaussian.h>
#include <pdf/linearanalyticconditionalgaussian.h>

#include "../compare_filters/nonlinearanalyticconditionalgaussianmobile.h"
#include "../mobile_robot.h"

#include <smoother/rauchtungstriebel.h>
#include <smoother/particlesmoother.h>

#include <iostream>
#include <fstream>

// Include file with properties
#include "../examples/mobile_robot_wall_cts.h"

using namespace MatrixWrapper;
using namespace BFL;
using namespace std;

/* The purpose of this program is to construct a filter for the problem
  of localisation of a mobile robot equipped with an ultrasonic sensor.
  In this case the orientation is known, which simplifies the model considerably:
  The system model will become linear.
  The ultrasonic measures the distance to the wall (it can be switched off:
  see mobile_robot_wall_cts.h)

  The necessary SYSTEM MODEL is:

  x_k      = x_{k-1} + v_{k-1} * cos(theta) * delta_t
  y_k      = y_{k-1} + v_{k-1} * sin(theta) * delta_t

  The used MEASUREMENT MODEL:
  measuring the (perpendicular) distance z to the wall y = ax + b

  set WALL_CT = 1/sqrt(pow(a,2) + 1)
  z = WALL_CT * a * x - WALL_CT * y + WALL_CT * b + GAUSSIAN_NOISE
  or Z = H * X_k + J * U_k

  where

  H = [ WALL_CT * a       - WALL_CT      0 ]
  and GAUSSIAN_NOISE = N((WALL_CT * b), SIGMA_MEAS_NOISE)

*/


int main(int argc, char** argv)
{
  cerr << "==================================================" << endl
       << "Test of different smooters" << endl
       << "Mobile robot localisation example" << endl
       << "==================================================" << endl;

  /***********************
   * PREPARE FILESTREAMS *
   **********************/
  ofstream fout_time, fout_E, fout_cov, fout_meas, fout_states, fout_particles, fout_numparticles, fout_E_smooth, fout_cov_smooth, fout_time_smooth;

  fout_time.open("time.out");
  fout_E.open("E.out");
  fout_cov.open("cov.out");
  fout_meas.open("meas.out");
  fout_states.open("states.out");
  fout_E_smooth.open("Esmooth.out");
  fout_cov_smooth.open("covsmooth.out");
  fout_time_smooth.open("timesmooth.out");

  /****************************
   * NonLinear system model      *
   ***************************/

  // create gaussian
  ColumnVector sys_noise_Mu(STATE_SIZE);
  sys_noise_Mu = 0.0;
  sys_noise_Mu(1) = MU_SYSTEM_NOISE_X;
  sys_noise_Mu(2) = MU_SYSTEM_NOISE_Y;
  sys_noise_Mu(3) = MU_SYSTEM_NOISE_THETA;

  SymmetricMatrix sys_noise_Cov(STATE_SIZE);
  sys_noise_Cov = 0.0;
  sys_noise_Cov(1,1) = SIGMA_SYSTEM_NOISE_X;
  sys_noise_Cov(1,2) = 0.0;
  sys_noise_Cov(1,3) = 0.0;
  sys_noise_Cov(2,1) = 0.0;
  sys_noise_Cov(2,2) = SIGMA_SYSTEM_NOISE_Y;
  sys_noise_Cov(2,3) = 0.0;
  sys_noise_Cov(3,1) = 0.0;
  sys_noise_Cov(3,2) = 0.0;
  sys_noise_Cov(3,3) = SIGMA_SYSTEM_NOISE_THETA;

  Gaussian system_Uncertainty(sys_noise_Mu, sys_noise_Cov);

  // create the model
  NonLinearAnalyticConditionalGaussianMobile sys_pdf(system_Uncertainty);
  AnalyticSystemModelGaussianUncertainty sys_model(&sys_pdf);

  /*********************************
   * Initialise measurement model *
   ********************************/
  // Fill up H
  double wall_ct = 2/(sqrt(pow(RICO_WALL,2.0) + 1));
  Matrix H(MEAS_SIZE,STATE_SIZE);
  H = 0.0;
  H(1,1) = wall_ct * RICO_WALL;
  H(1,2) = 0 - wall_ct;

   // Construct the measurement noise (a scalar in this case)
  ColumnVector MeasNoise_Mu(MEAS_SIZE);
  SymmetricMatrix MeasNoise_Cov(MEAS_SIZE);
  MeasNoise_Mu(1) = MU_MEAS_NOISE;
  MeasNoise_Cov(1,1) = SIGMA_MEAS_NOISE;

  Gaussian measurement_Uncertainty(MeasNoise_Mu,MeasNoise_Cov);

  // create the model
  LinearAnalyticConditionalGaussian meas_pdf(H, measurement_Uncertainty);
  LinearAnalyticMeasurementModelGaussianUncertainty meas_model(&meas_pdf);


  /****************************
   * Linear prior DENSITY     *
   ***************************/
  // Continuous Gaussian prior (for Kalman filters)
  ColumnVector prior_mu(STATE_SIZE);
  SymmetricMatrix prior_sigma(STATE_SIZE);
  prior_mu(1) = PRIOR_MU_X;
  prior_mu(2) = PRIOR_MU_Y;
  prior_sigma = 0.0;
  prior_sigma(1,1) = PRIOR_COV_X;
  prior_sigma(2,2) = PRIOR_COV_Y;
  prior_sigma(3,3) = PRIOR_COV_THETA;
  Gaussian prior_cont(prior_mu,prior_sigma);


   cout<< "Prior initialised"<< "" << endl;
   cout << "Prior Mean = " << endl << prior_cont.ExpectedValueGet() << endl
        << "Covariance = " << endl << prior_cont.CovarianceGet() << endl;

  /******************************
   * Construction of the Filter *
   ******************************/
  ExtendedKalmanFilter filter(&prior_cont);



  /***************************
   * initialise MOBILE ROBOT *
   **************************/
  // Model of mobile robot in world with one wall
  // The model is used to simultate the distance measurements.
  MobileRobot mobile_robot;
  ColumnVector input(2);
  input(1) = 0.1;
  input(2) = 0.0;

  /***************************
   * vector in which all posteriors will be stored*
   **************************/
  vector<Gaussian> posteriors(NUM_TIME_STEPS);
  vector<Gaussian>::iterator posteriors_it  = posteriors.begin();

  /*******************
   * ESTIMATION LOOP *
   *******************/
  cout << "MAIN: Starting estimation" << endl;
  unsigned int time_step;
  for (time_step = 0; time_step < NUM_TIME_STEPS; time_step++)
    {
      // write date in files
      fout_time << time_step << ";" << endl;
      fout_meas << mobile_robot.Measure()(1) << ";" << endl;
      fout_states << mobile_robot.GetState()(1) << "," << mobile_robot.GetState()(2) << ","
                  << mobile_robot.GetState()(3) << ";" << endl;

      // write posterior to file
      Gaussian * posterior = (Gaussian*)(filter.PostGet());
      fout_E << posterior->ExpectedValueGet()(1) << "," << posterior->ExpectedValueGet()(2)<< ","
             << posterior->ExpectedValueGet()(3) << ";"  << endl;
      fout_cov << posterior->CovarianceGet()(1,1) << "," << posterior->CovarianceGet()(1,2) << ","
               << posterior->CovarianceGet()(1,3) << "," << posterior->CovarianceGet()(2,2) << ","
               << posterior->CovarianceGet()(2,3) << "," << posterior->CovarianceGet()(3,3) << ";" << endl;

      // DO ONE STEP WITH MOBILE ROBOT
      mobile_robot.Move(input);

      if ((time_step+1)%10 == 0){
        // DO ONE MEASUREMENT
        ColumnVector measurement = mobile_robot.Measure();

        // UPDATE FILTER
        filter.Update(&sys_model,input,&meas_model,measurement);
      }
      else{
        filter.Update(&sys_model,input);
      }

      //Pdf<ColumnVector> * posterior = filter.PostGet();
      // make copy of posterior
      *posteriors_it = *posterior;

      posteriors_it++;
    } // estimation loop


      Pdf<ColumnVector> * posterior = filter.PostGet();
      // write data in files
      fout_time << time_step << ";" << endl;
      fout_meas << mobile_robot.Measure()(1) << ";" << endl;
      fout_states << mobile_robot.GetState()(1) << "," << mobile_robot.GetState()(2) << ","
                  << mobile_robot.GetState()(3) << ";" << endl;

      // write posterior to file
      fout_E << posterior->ExpectedValueGet()(1) << "," << posterior->ExpectedValueGet()(2)<< ","
             << posterior->ExpectedValueGet()(3) << ";"  << endl;
      fout_cov << posterior->CovarianceGet()(1,1) << "," << posterior->CovarianceGet()(1,2) << ","
               << posterior->CovarianceGet()(1,3) << "," << posterior->CovarianceGet()(2,2) << ","
               << posterior->CovarianceGet()(2,3) << "," << posterior->CovarianceGet()(3,3) << ";" << endl;

  cout << "After " << time_step+1 << " timesteps " << endl;
  cout << " Posterior Mean = " << endl << posterior->ExpectedValueGet() << endl
       << " Covariance = " << endl << posterior->CovarianceGet() << "" << endl;


  cout << "======================================================" << endl
       << "End of the filter for mobile robot localisation" << endl
       << "======================================================"
       << endl;


  /***************************************
   * Construction of the Backward Filter *
   **************************************/
RauchTungStriebel backwardfilter((Gaussian*)posterior);

  fout_time_smooth << time_step << ";" << endl;
  // write posterior to file
  fout_E_smooth << posterior->ExpectedValueGet()(1) << "," << posterior->ExpectedValueGet()(2)<< ","
         << posterior->ExpectedValueGet()(3) << ";"  << endl;
  fout_cov_smooth << posterior->CovarianceGet()(1,1) << "," << posterior->CovarianceGet()(1,2) << ","
           << posterior->CovarianceGet()(1,3) << "," << posterior->CovarianceGet()(2,2) << ","
           << posterior->CovarianceGet()(2,3) << "," << posterior->CovarianceGet()(3,3) << ";" << endl;

  /*******************
   * ESTIMATION LOOP *
   *******************/
  cout << "======================================================" << endl
       << "Start of the smoother for mobile robot localisation" << endl
       << "======================================================"
       << endl;
  for (time_step = NUM_TIME_STEPS-1; time_step+1 > 0 ; time_step--)
    {
      posteriors_it--;
      // UPDATE  BACKWARDFILTER
      Gaussian filtered(posteriors_it->ExpectedValueGet(),posteriors_it->CovarianceGet());
      backwardfilter.Update(&sys_model,input, &filtered);
      Pdf<ColumnVector> * posterior = backwardfilter.PostGet();

      fout_time_smooth << time_step << ";" << endl;
      // write posterior to file
      fout_E_smooth << posterior->ExpectedValueGet()(1) << "," << posterior->ExpectedValueGet()(2)<< ","
             << posterior->ExpectedValueGet()(3) << ";"  << endl;
      fout_cov_smooth << posterior->CovarianceGet()(1,1) << "," << posterior->CovarianceGet()(1,2) << ","
               << posterior->CovarianceGet()(1,3) << "," << posterior->CovarianceGet()(2,2) << ","
               << posterior->CovarianceGet()(2,3) << "," << posterior->CovarianceGet()(3,3) << ";" << endl;

      // make copy of posterior
      posteriors_it->ExpectedValueSet(posterior->ExpectedValueGet());
      posteriors_it->CovarianceSet(posterior->CovarianceGet());

    } // estimation loop

  cout << "After Smoothing first timestep " << endl;
  cout << " Posterior Mean = " << endl << posteriors_it->ExpectedValueGet() << endl
       << " Covariance = " << endl << posteriors_it->CovarianceGet() << "" << endl;



  cout << "======================================================" << endl
       << "End of the Kalman smoother for mobile robot localisation" << endl
       << "======================================================"
       << endl;
  return 0;

  /****************************
   * CLOSE FILESTREAMS
   ***************************/
  fout_time.close();
  fout_E.close();
  fout_cov.close();
  fout_meas.close();
  fout_states.close();
  fout_time_smooth.close();
  fout_E_smooth.close();
  fout_cov_smooth.close();
}
