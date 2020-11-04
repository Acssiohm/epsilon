#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_properties_is_number) {
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Binary).isNumber());
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Decimal).isNumber());
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Hexadecimal).isNumber());
  quiz_assert(Decimal::Builder("2",3).isNumber());
  quiz_assert(Float<float>::Builder(1.0f).isNumber());
  quiz_assert(Infinity::Builder(true).isNumber());
  quiz_assert(Undefined::Builder().isNumber());
  quiz_assert(Rational::Builder(2,3).isNumber());
  quiz_assert(!Symbol::Builder('a').isNumber());
  quiz_assert(!Multiplication::Builder(Rational::Builder(1), Rational::Builder(2)).isNumber());
  quiz_assert(!Addition::Builder(Rational::Builder(1), Rational::Builder(2)).isNumber());
}

QUIZ_CASE(poincare_properties_is_random) {
  quiz_assert(Random::Builder().isRandom());
  quiz_assert(Randint::Builder(Rational::Builder(1), Rational::Builder(2)).isRandom());
  quiz_assert(!Symbol::Builder('a').isRandom());
  quiz_assert(!Rational::Builder(2,3).isRandom());
}

QUIZ_CASE(poincare_properties_is_parametered_expression) {
  quiz_assert(Derivative::Builder(Rational::Builder(1), Symbol::Builder('x'), Rational::Builder(2)).isParameteredExpression());
  quiz_assert(Integral::Builder(Rational::Builder(1), Symbol::Builder('x'), Rational::Builder(2), Rational::Builder(2)).isParameteredExpression());
  quiz_assert(Sum::Builder(Rational::Builder(1), Symbol::Builder('n'), Rational::Builder(2), Rational::Builder(2)).isParameteredExpression());
  quiz_assert(Product::Builder(Rational::Builder(1), Symbol::Builder('n'), Rational::Builder(2), Rational::Builder(2)).isParameteredExpression());
  quiz_assert(!Symbol::Builder('a').isParameteredExpression());
  quiz_assert(!Rational::Builder(2,3).isParameteredExpression());
}

void assert_expression_has_property(const char * expression, Context * context, Expression::ExpressionTest test) {
  Expression e = parse_expression(expression, context, false);
  quiz_assert_print_if_failure(e.recursivelyMatches(test, context), expression);
}

void assert_expression_has_not_property(const char * expression, Context * context, Expression::ExpressionTest test) {
  Expression e = parse_expression(expression, context, false);
  quiz_assert_print_if_failure(!e.recursivelyMatches(test, context), expression);
}

QUIZ_CASE(poincare_properties_is_approximate) {
  Shared::GlobalContext context;
  assert_expression_has_property("3.4", &context, Expression::IsApproximate);
  assert_expression_has_property("2.3+1", &context, Expression::IsApproximate);
  assert_expression_has_not_property("a", &context, Expression::IsApproximate);
  assert_reduce("42.3→a");
  assert_expression_has_property("a", &context, Expression::IsApproximate);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
}

QUIZ_CASE(poincare_properties_is_matrix) {
  Shared::GlobalContext context;
  assert_expression_has_property("[[1,2][3,4]]", &context, Expression::IsMatrix);
  assert_expression_has_property("confidence(0.2,3)*2", &context, Expression::IsMatrix);
  assert_expression_has_property("dim([[1,2][3,4]])/3", &context, Expression::IsMatrix);
  assert_expression_has_property("prediction(0.3,10)", &context, Expression::IsMatrix);
  assert_expression_has_property("[[1,2][3,4]]^(-1)", &context, Expression::IsMatrix);
  assert_expression_has_property("inverse([[1,2][3,4]])", &context, Expression::IsMatrix);
  assert_expression_has_property("3*identity(4)", &context, Expression::IsMatrix);
  assert_expression_has_property("transpose([[1,2][3,4]])", &context, Expression::IsMatrix);
  assert_expression_has_property("ref([[1,2][3,4]])", &context, Expression::IsMatrix);
  assert_expression_has_property("rref([[1,2][3,4]])", &context, Expression::IsMatrix);
  assert_expression_has_property("cross([[1][2][3]],[[3][4][5]])", &context, Expression::IsMatrix);
  assert_expression_has_not_property("2*3+1", &context, Expression::IsMatrix);
}

void assert_expression_is_deep_matrix(const char * expression) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression, &context, false);
  quiz_assert_print_if_failure(e.deepIsMatrix(&context), expression);
}

void assert_expression_is_not_deep_matrix(const char * expression) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression, &context, false);
  quiz_assert_print_if_failure(!e.deepIsMatrix(&context), expression);
}

QUIZ_CASE(poincare_properties_deep_is_matrix) {
  assert_expression_is_not_deep_matrix("diff([[1,2][3,4]],x,2)");
  assert_expression_is_not_deep_matrix("sign([[1,2][3,4]])");
  assert_expression_is_not_deep_matrix("3");
  assert_expression_is_deep_matrix("2*dim(2)");
  assert_expression_is_deep_matrix("log(confidence(0.2,20))");
  assert_expression_is_deep_matrix("confidence(0.2,20)^2");
  assert_expression_is_deep_matrix("cos(confidence(0.2,20))");
}

QUIZ_CASE(poincare_properties_is_infinity) {
  Shared::GlobalContext context;
  assert_expression_has_property("3.4+inf", &context, Expression::IsInfinity);
  assert_expression_has_not_property("2.3+1", &context, Expression::IsInfinity);
  assert_expression_has_not_property("a", &context, Expression::IsInfinity);
  assert_reduce("42.3+inf→a");
  assert_expression_has_property("a", &context, Expression::IsInfinity);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
}

constexpr Poincare::ExpressionNode::Sign Positive = Poincare::ExpressionNode::Sign::Positive;
constexpr Poincare::ExpressionNode::Sign Negative = Poincare::ExpressionNode::Sign::Negative;
constexpr Poincare::ExpressionNode::Sign Unknown = Poincare::ExpressionNode::Sign::Unknown;

void assert_reduced_expression_sign(const char * expression, Poincare::ExpressionNode::Sign sign, Preferences::ComplexFormat complexFormat = Cartesian, Preferences::AngleUnit angleUnit = Radian, Preferences::UnitFormat unitFormat = Metric) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  e = e.reduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::SystemForApproximation));
  quiz_assert_print_if_failure(e.sign(&globalContext) == sign, expression);
}

QUIZ_CASE(poincare_properties_decimal_sign) {
  quiz_assert(Decimal::Builder(-2, 3).sign() == ExpressionNode::Sign::Negative);
  quiz_assert(Decimal::Builder(-2, -3).sign() == ExpressionNode::Sign::Negative);
  quiz_assert(Decimal::Builder(2, -3).sign() == ExpressionNode::Sign::Positive);
  quiz_assert(Decimal::Builder(2, 3).sign() == ExpressionNode::Sign::Positive);
  quiz_assert(Decimal::Builder(0, 1).sign() == ExpressionNode::Sign::Positive);
}

QUIZ_CASE(poincare_properties_based_integer_sign) {
  quiz_assert(BasedInteger::Builder(2, Integer::Base::Binary).sign() == ExpressionNode::Sign::Positive);
  quiz_assert(BasedInteger::Builder(2, Integer::Base::Decimal).sign() == ExpressionNode::Sign::Positive);
  quiz_assert(BasedInteger::Builder(2, Integer::Base::Hexadecimal).sign() == ExpressionNode::Sign::Positive);
}

QUIZ_CASE(poincare_properties_rational_sign) {
  quiz_assert(Rational::Builder(-2).sign() == ExpressionNode::Sign::Negative);
  quiz_assert(Rational::Builder(-2, 3).sign() == ExpressionNode::Sign::Negative);
  quiz_assert(Rational::Builder(2, 3).sign() == ExpressionNode::Sign::Positive);
  quiz_assert(Rational::Builder(0, 3).sign() == ExpressionNode::Sign::Positive);
}

QUIZ_CASE(poincare_properties_sign) {
  assert_reduced_expression_sign("abs(-cos(2)+I)", Positive);
  assert_reduced_expression_sign("2.345ᴇ-23", Positive);
  assert_reduced_expression_sign("-2.345ᴇ-23", Negative);
  assert_reduced_expression_sign("2×(-3)×abs(-32)", Negative);
  assert_reduced_expression_sign("2×(-3)×abs(-32)×cos(3)", Unknown);
  assert_reduced_expression_sign("x", Unknown);
  assert_reduced_expression_sign("2^(-abs(3))", Positive);
  assert_reduced_expression_sign("(-2)^4", Positive);
  assert_reduced_expression_sign("(-2)^3", Negative);
  assert_reduced_expression_sign("random()", Positive);
  assert_reduced_expression_sign("42/3", Positive);
  assert_reduced_expression_sign("-23/32", Negative);
  assert_reduced_expression_sign("𝐢", Unknown);
  assert_reduced_expression_sign("-π", Negative);
  assert_reduced_expression_sign("π", Positive);
  assert_reduced_expression_sign("ℯ", Positive);
  assert_reduced_expression_sign("0", Positive);
  assert_reduced_expression_sign("cos(π/2)", Positive);
  assert_reduced_expression_sign("cos(90)", Positive, Cartesian, Degree);
  assert_reduced_expression_sign("√(-1)", Unknown);
  assert_reduced_expression_sign("√(-1)", Unknown, Real);
  assert_reduced_expression_sign("sign(π)", Positive);
  assert_reduced_expression_sign("sign(-π)", Negative);
  assert_reduced_expression_sign("a", Unknown);
  assert_reduce("42→a");
  assert_reduced_expression_sign("a", Positive);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
}

void assert_expression_is_real(const char * expression) {
  Shared::GlobalContext context;
  // isReal can be call only on reduced expressions
  Expression e = parse_expression(expression, &context, false).reduce(ExpressionNode::ReductionContext(&context, Cartesian, Radian, Metric, ExpressionNode::ReductionTarget::SystemForApproximation));
  quiz_assert_print_if_failure(e.isReal(&context), expression);
}

void assert_expression_is_not_real(const char * expression) {
  Shared::GlobalContext context;
  // isReal can be call only on reduced expressions
  Expression e = parse_expression(expression, &context, false).reduce(ExpressionNode::ReductionContext(&context, Cartesian, Radian, Metric, ExpressionNode::ReductionTarget::SystemForApproximation));
  quiz_assert_print_if_failure(!e.isReal(&context), expression);
}

QUIZ_CASE(poincare_properties_is_real) {
  assert_expression_is_real("atan(4)");
  assert_expression_is_not_real("atan(𝐢)");
  assert_expression_is_real("conj(4)");
  assert_expression_is_not_real("conj(𝐢)");
  assert_expression_is_real("sin(4)");
  assert_expression_is_not_real("sin(𝐢)");
  assert_expression_is_real("quo(2,3+a)");
  assert_expression_is_real("sign(2)");
  assert_expression_is_real("abs(2)");
  assert_expression_is_not_real("abs([[1,2]])");
  assert_expression_is_real("ceil(2)");
  assert_expression_is_not_real("ceil([[1,2]])");
  assert_expression_is_not_real("1+2+3+3×𝐢");
  assert_expression_is_real("1+2+3+root(2,3)");
  assert_expression_is_real("1×23×3×root(2,3)");
  assert_expression_is_not_real("1×23×3×root(2,3)×3×𝐢");
  assert_expression_is_not_real("1×23×3×[[1,2]]");
  assert_expression_is_not_real("1×23×3×abs(confidence(cos(5)/25,3))");
  assert_expression_is_real("π");
  assert_expression_is_not_real("unreal");
  assert_expression_is_not_real("undef");
  assert_expression_is_real("2.3");
  assert_expression_is_real("2^3.4");
  assert_expression_is_real("(-2)^(-3)");
  assert_expression_is_not_real("𝐢^3.4");
  assert_expression_is_not_real("2^(3.4𝐢)");
  assert_expression_is_not_real("(-2)^0.4");
}

void assert_reduced_expression_polynomial_degree(const char * expression, int degree, const char * symbolName = "x", Preferences::ComplexFormat complexFormat = Cartesian, Preferences::AngleUnit angleUnit = Radian, Preferences::UnitFormat unitFormat = Metric) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  Expression result = e.reduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, SystemForApproximation));

  quiz_assert_print_if_failure(result.polynomialDegree(&globalContext, symbolName) == degree, expression);
}

QUIZ_CASE(poincare_properties_polynomial_degree) {
  assert_reduced_expression_polynomial_degree("x+1", 1);
  assert_reduced_expression_polynomial_degree("cos(2)+1", 0);
  assert_reduced_expression_polynomial_degree("confidence(0.2,10)+1", -1);
  assert_reduced_expression_polynomial_degree("diff(3×x+x,x,2)", 0);
  assert_reduced_expression_polynomial_degree("diff(3×x+x,x,x)", 0);
  assert_reduced_expression_polynomial_degree("diff(3×x+x,x,x)", 0, "a");
  assert_reduced_expression_polynomial_degree("(3×x+2)/3", 1);
  assert_reduced_expression_polynomial_degree("(3×x+2)/x", -1);
  assert_reduced_expression_polynomial_degree("int(2×x,x, 0, 1)", -1);
  assert_reduced_expression_polynomial_degree("int(2×x,x, 0, 1)", 0, "a");
  assert_reduced_expression_polynomial_degree("[[1,2][3,4]]", -1);
  assert_reduced_expression_polynomial_degree("(x^2+2)×(x+1)", 3);
  assert_reduced_expression_polynomial_degree("-(x+1)", 1);
  assert_reduced_expression_polynomial_degree("(x^2+2)^(3)", 6);
  assert_reduced_expression_polynomial_degree("prediction(0.2,10)+1", -1);
  assert_reduced_expression_polynomial_degree("2-x-x^3", 3);
  assert_reduced_expression_polynomial_degree("π×x", 1);
  assert_reduced_expression_polynomial_degree("√(-1)×x", -1, "x", Real);
  // f: x→x^2+πx+1
  assert_reduce("1+π×x+x^2→f(x)");
  assert_reduced_expression_polynomial_degree("f(x)", 2);
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}

void assert_reduced_expression_has_characteristic_range(Expression e, float range, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree, Preferences::UnitFormat unitFormat = Metric) {
  Shared::GlobalContext globalContext;
  e = e.reduce(ExpressionNode::ReductionContext(&globalContext, Preferences::ComplexFormat::Cartesian, angleUnit, unitFormat, ExpressionNode::ReductionTarget::SystemForApproximation));
  if (std::isnan(range)) {
    quiz_assert(std::isnan(e.characteristicXRange(&globalContext, angleUnit)));
  } else {
    quiz_assert(std::fabs(e.characteristicXRange(&globalContext, angleUnit) - range) < 0.0000001f);
  }
}

QUIZ_CASE(poincare_properties_characteristic_range) {
  // cos(x), degree
  assert_reduced_expression_has_characteristic_range(Cosine::Builder(Symbol::Builder(UCodePointUnknown)), 360.0f);
  // cos(-x), degree
  assert_reduced_expression_has_characteristic_range(Cosine::Builder(Opposite::Builder(Symbol::Builder(UCodePointUnknown))), 360.0f);
  // cos(x), radian
  assert_reduced_expression_has_characteristic_range(Cosine::Builder(Symbol::Builder(UCodePointUnknown)), 2.0f*M_PI, Preferences::AngleUnit::Radian);
  // cos(-x), radian
  assert_reduced_expression_has_characteristic_range(Cosine::Builder(Opposite::Builder(Symbol::Builder(UCodePointUnknown))), 2.0f*M_PI, Preferences::AngleUnit::Radian);
  // sin(9x+10), degree
  assert_reduced_expression_has_characteristic_range(Sine::Builder(Addition::Builder(Multiplication::Builder(Rational::Builder(9),Symbol::Builder(UCodePointUnknown)),Rational::Builder(10))), 40.0f);
  // sin(9x+10)+cos(x/2), degree
  assert_reduced_expression_has_characteristic_range(Addition::Builder(Sine::Builder(Addition::Builder(Multiplication::Builder(Rational::Builder(9),Symbol::Builder(UCodePointUnknown)),Rational::Builder(10))),Cosine::Builder(Division::Builder(Symbol::Builder(UCodePointUnknown),Rational::Builder(2)))), 720.0f);
  // sin(9x+10)+cos(x/2), radian
  assert_reduced_expression_has_characteristic_range(Addition::Builder(Sine::Builder(Addition::Builder(Multiplication::Builder(Rational::Builder(9),Symbol::Builder(UCodePointUnknown)),Rational::Builder(10))),Cosine::Builder(Division::Builder(Symbol::Builder(UCodePointUnknown),Rational::Builder(2)))), 4.0f*M_PI, Preferences::AngleUnit::Radian);
  // x, degree
  assert_reduced_expression_has_characteristic_range(Symbol::Builder(UCodePointUnknown), NAN);
  // cos(3)+2, degree
  assert_reduced_expression_has_characteristic_range(Addition::Builder(Cosine::Builder(Rational::Builder(3)),Rational::Builder(2)), 0.0f);
  // log(cos(40x), degree
  assert_reduced_expression_has_characteristic_range(CommonLogarithm::Builder(Cosine::Builder(Multiplication::Builder(Rational::Builder(40),Symbol::Builder(UCodePointUnknown)))), 9.0f);
  // cos(cos(x)), degree
  assert_reduced_expression_has_characteristic_range(Cosine::Builder((Expression)Cosine::Builder(Symbol::Builder(UCodePointUnknown))), 360.0f);
  // f(x) with f : x --> cos(x), degree
  assert_reduce("cos(x)→f(x)");
  assert_reduced_expression_has_characteristic_range(Function::Builder("f",1,Symbol::Builder(UCodePointUnknown)), 360.0f);
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}

void assert_expression_has_variables(const char * expression, const char * variables[], int trueNumberOfVariables) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  constexpr static int k_maxVariableSize = Poincare::SymbolAbstract::k_maxNameSize;
  char variableBuffer[Expression::k_maxNumberOfVariables][k_maxVariableSize] = {{0}};
  int numberOfVariables = e.getVariables(&globalContext, [](const char * symbol, Poincare::Context * context) { return true; }, (char *)variableBuffer, k_maxVariableSize);
  quiz_assert_print_if_failure(trueNumberOfVariables == numberOfVariables, expression);
  if (numberOfVariables < 0) {
    // Too many variables
    return;
  }
  int index = 0;
  while (index < Expression::k_maxNumberOfVariables && (variableBuffer[index][0] != 0 || variables[index][0] != 0)) {
    quiz_assert_print_if_failure(strcmp(variableBuffer[index], variables[index]) == 0, expression);
    index++;
  }
}

QUIZ_CASE(poincare_properties_get_variables) {
  const char * variableBuffer1[] = {"x","y",""};
  assert_expression_has_variables("x+y", variableBuffer1, 2);
  const char * variableBuffer2[] = {"x","y","z","t",""};
  assert_expression_has_variables("x+y+z+2×t", variableBuffer2, 4);
  const char * variableBuffer3[] = {"a","x","y","k","A", ""};
  assert_expression_has_variables("a+x^2+2×y+k!×A", variableBuffer3, 5);
  const char * variableBuffer4[] = {"BABA","abab", ""};
  assert_expression_has_variables("BABA+abab", variableBuffer4, 2);
  const char * variableBuffer5[] = {"BBBBBB", ""};
  assert_expression_has_variables("BBBBBB", variableBuffer5, 1);
  const char * variableBuffer6[] = {""};
  assert_expression_has_variables("a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+aa+bb+cc+dd+ee+ff+gg+hh+ii+jj+kk+ll+mm+nn+oo", variableBuffer6, -1);
  assert_expression_has_variables("a+b+c+d+e+f+g", variableBuffer6, -1);
  // f: x→1+πx+x^2+toto
  assert_reduce("1+π×x+x^2+toto→f(x)");
  const char * variableBuffer7[] = {"tata","toto", ""};
  assert_expression_has_variables("f(tata)", variableBuffer7, 2);
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

  const char * variableBuffer8[] = {"y", ""};
  assert_expression_has_variables("diff(3x,x,0)y-2", variableBuffer8, 1);
  const char * variableBuffer9[] = {"a", "b", "c", "d", "e", "f"};
  assert_expression_has_variables("a+b+c+d+e+f", variableBuffer9, 6);
}

void assert_reduced_expression_has_polynomial_coefficient(const char * expression, const char * symbolName, const char ** coefficients, Preferences::ComplexFormat complexFormat = Cartesian, Preferences::AngleUnit angleUnit = Radian, Preferences::UnitFormat unitFormat = Metric, ExpressionNode::SymbolicComputation symbolicComputation = ReplaceAllDefinedSymbolsWithDefinition) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  e = e.reduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, SystemForAnalysis, symbolicComputation));
  Expression coefficientBuffer[Poincare::Expression::k_maxNumberOfPolynomialCoefficients];
  int d = e.getPolynomialReducedCoefficients(symbolName, coefficientBuffer, &globalContext, complexFormat, Radian, unitFormat, symbolicComputation);
  for (int i = 0; i <= d; i++) {
    Expression f = parse_expression(coefficients[i], &globalContext, false);
    coefficientBuffer[i] = coefficientBuffer[i].reduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, SystemForAnalysis, symbolicComputation));
    f = f.reduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, SystemForAnalysis, symbolicComputation));
    quiz_assert_print_if_failure(coefficientBuffer[i].isIdenticalTo(f), expression);
  }
  quiz_assert_print_if_failure(coefficients[d+1] == 0, expression);
}

QUIZ_CASE(poincare_properties_get_polynomial_coefficients) {
  const char * coefficient0[] = {"2", "1", "1", 0};
  assert_reduced_expression_has_polynomial_coefficient("x^2+x+2", "x", coefficient0);
  const char * coefficient1[] = {"12+(-6)×π", "12", "3", 0}; //3×x^2+12×x-6×π+12
  assert_reduced_expression_has_polynomial_coefficient("3×(x+2)^2-6×π", "x", coefficient1);
  // TODO: decomment when enable 3-degree polynomes
  //const char * coefficient2[] = {"2+32×x", "2", "6", "2", 0}; //2×n^3+6×n^2+2×n+2+32×x
  //assert_reduced_expression_has_polynomial_coefficient("2×(n+1)^3-4n+32×x", "n", coefficient2);
  const char * coefficient3[] = {"1", "-π", "1", 0}; //x^2-π×x+1
  assert_reduced_expression_has_polynomial_coefficient("x^2-π×x+1", "x", coefficient3);

  // f: x→x^2+Px+1
  assert_reduce("1+π×x+x^2→f(x)");
  const char * coefficient4[] = {"1", "π", "1", 0}; //x^2+π×x+1
  assert_reduced_expression_has_polynomial_coefficient("f(x)", "x", coefficient4);
  const char * coefficient5[] = {"0", "𝐢", 0}; //√(-1)x
  assert_reduced_expression_has_polynomial_coefficient("√(-1)x", "x", coefficient5);
  const char * coefficient6[] = {0}; //√(-1)x
  assert_reduced_expression_has_polynomial_coefficient("√(-1)x", "x", coefficient6, Real);

  // 3 -> x
  assert_reduce("3→x");
  const char * coefficient7[] = {"4", 0};
  assert_reduced_expression_has_polynomial_coefficient("x+1", "x", coefficient7 );
  const char * coefficient8[] = {"2", "1", 0};
  assert_reduced_expression_has_polynomial_coefficient("x+2", "x", coefficient8, Real, Radian, Metric, DoNotReplaceAnySymbol);
  assert_reduced_expression_has_polynomial_coefficient("x+2", "x", coefficient8, Real, Radian, Metric, ReplaceDefinedFunctionsWithDefinitions);
  assert_reduced_expression_has_polynomial_coefficient("f(x)", "x", coefficient4, Cartesian, Radian, Metric, ReplaceDefinedFunctionsWithDefinitions);

  // Clear the storage
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}

void assert_reduced_expression_unit_is(const char * expression, const char * unit) {
  Shared::GlobalContext globalContext;
  ExpressionNode::ReductionContext redContext(&globalContext, Real, Degree, Metric, SystemForApproximation);
  Expression e = parse_expression(expression, &globalContext, false);
  e = e.reduce(redContext);
  Expression u1;
  e = e.removeUnit(&u1);
  Expression e2 = parse_expression(unit, &globalContext, false);
  Expression u2;
  e2 = e2.reduce(redContext);
  e2.removeUnit(&u2);
  quiz_assert_print_if_failure(u1.isUninitialized() == u2.isUninitialized() && (u1.isUninitialized() || u1.isIdenticalTo(u2)), expression);
}

QUIZ_CASE(poincare_properties_remove_unit) {
  assert_reduced_expression_unit_is("_km", "_m");
  assert_reduced_expression_unit_is("_min/_km", "_m^(-1)×_s");
  assert_reduced_expression_unit_is("_km^3", "_m^3");
  assert_reduced_expression_unit_is("1_m+_km", "_m");
  assert_reduced_expression_unit_is("_L^2×3×_s", "_m^6×_s");
}

void assert_additional_results_compute_to(const char * expression, const char * * results, int length) {
  Shared::GlobalContext globalContext;
  constexpr int maxNumberOfResults = 5;
  assert(length <= maxNumberOfResults);
  Expression additional[maxNumberOfResults];
  ExpressionNode::ReductionContext reductionContext = ExpressionNode::ReductionContext(&globalContext, Cartesian, Degree, Metric, User, ReplaceAllSymbolsWithUndefined, DefaultUnitConversion);
  Expression e = parse_expression(expression, &globalContext, false).reduce(reductionContext);
  Expression units;
  e = e.removeUnit(&units);
  double value = e.approximateToScalar<double>(&globalContext, Cartesian, Degree);

  if (!Unit::ShouldDisplayAdditionalOutputs(value, units)) {
    quiz_assert(length == 0);
    return;
  }
  const int numberOfResults = Unit::SetAdditionalExpressions(units, value, additional, maxNumberOfResults, reductionContext);

  quiz_assert(numberOfResults == length);
  for (int i = 0; i < length; i++) {
    assert_expression_serialize_to(additional[i], results[i], Preferences::PrintFloatMode::Decimal);
  }
}

QUIZ_CASE(poincare_expression_additional_results) {
  // Time
  assert_additional_results_compute_to("3×_s", nullptr, 0);
  const char * array1[1] = {"1×_min+1×_s"};
  assert_additional_results_compute_to("61×_s", array1, 1);
  const char * array2[1] = {"1×_day+10×_h+17×_min+36×_s"};
  assert_additional_results_compute_to("123456×_s", array2, 1);
  const char * array3[1] = {"7×_day"};
  assert_additional_results_compute_to("1×_week", array3, 1);

  // Distance
  const char * array4[1] = {"19×_mi+853×_yd+1×_ft+7×_in"};
  assert_additional_results_compute_to("1234567×_in", array4, 1);
  const char * array5[1] = {"1×_yd+7.700787×_in"};
  assert_additional_results_compute_to("1.11×_m", array5, 1);

  // Masses
  const char * array6[1] = {"1×_shtn+240×_lb"};
  assert_additional_results_compute_to("1×_lgtn", array6, 1);
  const char * array7[1] = {"2×_lb+3.273962×_oz"};
  assert_additional_results_compute_to("1×_kg", array7, 1);

  // Energy
  const char * array8[2] = {"1×_kW×_h", "2.246943ᴇ13×_TeV"};
  assert_additional_results_compute_to("3.6×_MN_m", array8, 2);

  // Volume
  const char * array9[2] = {"1000×_L", "264×_gal+2×_cup+6.022702×_floz"};
  assert_additional_results_compute_to("1×_m^3", array9, 2);
  const char * array10[2] = {"182.5426×_L", "48×_gal+3×_cup+4.5×_floz"};
  assert_additional_results_compute_to("12345×_tbsp", array10, 2);

  // Speed
  const char * array11[2] = {"3.6×_km×_h^\x12-1\x13", "2.236936×_mi×_h^\x12-1\x13"};
  assert_additional_results_compute_to("1×_m/_s", array11, 2);
}
