#include <poincare/complex_argument.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

int ComplexArgumentNode::numberOfChildren() const { return ComplexArgument::NumberOfChildren(); }

Layout ComplexArgumentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ComplexArgument(this), floatDisplayMode, numberOfSignificantDigits, ComplexArgument::Name());
}

int ComplexArgumentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ComplexArgument::Name());
}

Expression ComplexArgumentNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return ComplexArgument(this).shallowReduce(context, angleUnit, replaceSymbols);
}

template<typename T>
Complex<T> ComplexArgumentNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::arg(c));
}

ComplexArgument::ComplexArgument() : Expression(TreePool::sharedPool()->createTreeNode<ComplexArgumentNode>()) {}

Expression ComplexArgument::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  return *this;
}

}

