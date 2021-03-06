// Copyright (C) 2014  Victor Fragoso <vfragoso@cs.ucsb.edu>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//
//     * Neither the name of the University of California, Santa Barbara nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL VICTOR FRAGOSO BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef STATX_DISTRIBUTIONS_EVD_GEV_MLE_H_
#define STATX_DISTRIBUTIONS_EVD_GEV_MLE_H_

#include <optimo/core/objects_ls.h>
#include <vector>

namespace vstatx {
namespace distributions {
namespace evd {

using Eigen::Matrix;
using Eigen::Dynamic;
using std::vector;

// MLE estimation for GEV distribution
bool gevfit_mle(const vector<double>& data,
                double* mu,
                double* sigma,
                double* xi);

// Helper Objects for solving the MLE problem
// MLE objective function for the generalized extreme value distributiono (GEV)
// The distribution has three parameters:
// -\inf < mu < \inf : location
// 0 < sigma : scale
// -\inf < xi < \inf: shape
// When xi = 0 => the GEV becomes the Gumbel family of distributions
// We implement the negative log-likelihood function.
// The problem we try to solve then is:
//        minimize -l(z; mu, sigma, xi)
//
// The minimizer is the vector holding our ML estimates for the GEV distribution
// given the data.
// There are two log-likelihood functions that need to be optimized. The
// selection of which function to optimize depends on the xi. If xi = 0, then
// we solve for Gumbel family of distributions. Otherwise, use the general
// formulation of the log-likelihood.

// General MLE problem for xi != 0.
class GEVMLEObjective : public optimo::ObjectiveFunctorLS<double> {
 public:
  // Constructor
  explicit GEVMLEObjective(const vector<double>& data,
                           const double alpha = 100.0,
                           const double beta = 100.0,
                           const double lambda = 50.0) :
      data_(data), alpha_(alpha), beta_(beta), lambda_(lambda) { }

  // Destructor
  virtual ~GEVMLEObjective(void) { }

  // Impl
  virtual double operator()(const Matrix<double, Dynamic, 1>& x) const;
 protected:
  const vector<double>& data_;
  const double alpha_;  // Penalty for constraint 0 < sigma
  const double beta_;  // Penalty for constraint 1 + xi*(z - mu)/sigma > 0
  // Control of threshold over the argument of log-barrier
  const double lambda_;  
};

// The Hessian functor (Dummy object, as BFGS is used to solve the problem)
class GEVMLEHessianFunctor : public optimo::HessianFunctorLS<double> {
 public:
  virtual void
  operator()(const Matrix<double, Dynamic, 1>& x,
             Matrix<double, Dynamic, Dynamic>* h) const;
};

// The gradient functor
class GEVMLEGradientFunctor : public optimo::GradientFunctorLS<double> {
 public:
  // Constructor
  explicit GEVMLEGradientFunctor(const vector<double>& data,
                                 const double alpha = 100.0,
                                 const double beta = 100.0,
                                 const double lambda = 50.0) :
      data_(data), alpha_(alpha), beta_(beta), lambda_(lambda) { }

  // Destructor
  virtual ~GEVMLEGradientFunctor(void) { }

  // Impl
  virtual void
  operator()(const Matrix<double, Dynamic, 1>& x,
             Matrix<double, Dynamic, 1>* g) const;

 protected:
  const vector<double>& data_;
  const double alpha_;  // Penalty for constraint 0 < sigma
  const double beta_;  // Penalty for constraint 1 + xi*(z - mu)/sigma > 0
  const double lambda_;  // Control of threshold over the argument of log-barrier
};
}  // evd
}  // distributions
}  // statx
#endif  // STATX_DISTRIBUTIONS_EVD_GEV_MLE_H_
