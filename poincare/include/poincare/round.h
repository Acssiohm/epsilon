#ifndef POINCARE_ROUND_H
#define POINCARE_ROUND_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare {

class RoundNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(RoundNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Round";
  }
#endif

  // Properties
  Type type() const override { return Type::Round; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Round final : public Expression {
public:
  Round();
  Round(const RoundNode * n) : Expression(n) {}
  Round(Expression operand0, Expression operand1) : Round() {
    replaceChildAtIndexInPlace(0, operand0);
    replaceChildAtIndexInPlace(1, operand1);
  }
  static const char * Name() { return "round"; }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
};

}

#endif
