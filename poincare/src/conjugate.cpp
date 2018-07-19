#include <poincare/conjugate.h>
#include <poincare/complex.h>
#include <poincare/simplification_engine.h>
#include <poincare/conjugate_layout_node.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

Expression::Type Conjugate::type() const {
  return Type::Conjugate;
}

Expression * Conjugate::clone() const {
  Conjugate * a = new Conjugate(m_operands, true);
  return a;
}

LayoutRef Conjugate::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return ConjugateLayoutRef(operand(0)->createLayout(floatDisplayMode, complexFormat));
}

Expression * Conjugate::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
#if MATRIX_EXACT_REDUCING
  if (op->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  if (op->type() == Type::Rational) {
    return replaceWith(op, true);
  }
  return this;
}

template<typename T>
Complex<T> Conjugate::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return c.conjugate();
}

}

