/*
  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Authors: Ethan Coon (ecoon@lanl.gov)
*/

//! Wraps a PDE_Accumulation to be an Evaluator.

/*!

Lots of options here, document me!  
  
*/

#ifndef STATE_EVALUATOR_PDE_ACCUMULATION_HH_
#define STATE_EVALUATOR_PDE_ACCUMULATION_HH_

#include "Teuchos_ParameterList.hpp"

#include "Evaluator_Factory.hh"
#include "State.hh"
#include "EvaluatorAlgebraic.hh"

namespace Amanzi {

class Evaluator_PDE_Accumulation : public EvaluatorAlgebraic<CompositeVector,CompositeVectorSpace> {
 public:
  Evaluator_PDE_Accumulation(Teuchos::ParameterList &plist);

  Evaluator_PDE_Accumulation(const Evaluator_PDE_Accumulation& other) = default;
  virtual Teuchos::RCP<Evaluator> Clone() const override {
    return Teuchos::rcp(new Evaluator_PDE_Accumulation(*this));
  };

  virtual void EnsureCompatibility(State &S) override;
  
 protected:
  virtual void Evaluate_(const State &S, CompositeVector &result) override;
  virtual void EvaluatePartialDerivative_(const State &S,
          const Key &wrt_key, const Key &wrt_tag, CompositeVector &result) override;

 protected:
  Key conserved_key_;
  Key cv_key_;
  Key tag_old_, tag_new_;

 private:
  static Utils::RegisteredFactory<Evaluator, Evaluator_PDE_Accumulation> fac_;
  
};

}  // namespace Amanzi

#endif