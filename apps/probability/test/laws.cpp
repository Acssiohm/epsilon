#include <quiz.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include "../law/chi_squared_law.h"
#include "../law/student_law.h"

void assert_cumulative_distributive_function_direct_and_inverse_is(Probability::Law * law, double x, double result) {
  double r = law->cumulativeDistributiveFunctionAtAbscissa(x);
  quiz_assert(!std::isnan(r));
  quiz_assert(!std::isinf(r));
  quiz_assert(std::abs(r-result)/result < FLT_EPSILON);

  r = law->cumulativeDistributiveInverseForProbability(&result);
  quiz_assert(!std::isnan(r));
  quiz_assert(!std::isinf(r));
  quiz_assert(std::abs(r-x) < FLT_EPSILON || std::abs(r-x)/x < FLT_EPSILON);

}

//TODO other laws

QUIZ_CASE(chi_squared_law) {
  // Chi Squared law with 1 degree of freedom
  Probability::ChiSquaredLaw law;
  law.setParameterAtIndex(1.0, 0);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 1.3, 0.7457867763960355222963016785797663033008575439453125);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 2.9874567, 0.91608813616541218127764523160294629633426666259765625);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 4.987, 0.97446155200631878745554104170878417789936065673828125);

  // Chi Squared law with 1.3 degrees of freedom
  law.setParameterAtIndex(1.3, 0);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 1.3, 0.66209429400375563457004091105773113667964935302734375);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 2.9874567, 0.878804860316481750714956433512270450592041015625);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 4.987, 0.96090315938362369507785842870362102985382080078125);

  // Chi Squared law with 5.4 degrees of freedom
  law.setParameterAtIndex(5.4, 0);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 1.3, 0.047059684573231390369851823152202996425330638885498046875);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 2.9874567, 0.250530060451470470983537097708904184401035308837890625);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 4.987, 0.53051693435084168459781039928202517330646514892578125);
}

QUIZ_CASE(student_law) {
  // Student law with 1 degree of freedom
  Probability::StudentLaw law;
  law.setParameterAtIndex(1.0, 0);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, -2.0, 0.1475836176504332741754010762247405259511345238869178945999223128627);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 0.0, 0.5);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 3.4567, 0.9103622230304611040452300585457123816013336181640625);

  // Student law with 1.3 degrees of freedom
  law.setParameterAtIndex(1.3, 0);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, -4.987, 0.041958035438260647687069848643659497611224651336669921875);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 1.3, 0.8111769751474751100062121622613631188869476318359375);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 2.9874567, 0.92143912473359812498330256858025677502155303955078125);

  // Student law with 5.4 degrees of freedom
  law.setParameterAtIndex(5.4, 0);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, -4.987, 0.00167496657737900025118837898929768925881944596767425537109375);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 1.3, 0.876837383157582639370275501278229057788848876953125);
  assert_cumulative_distributive_function_direct_and_inverse_is(&law, 2.9874567, 0.98612148076325445433809591122553683817386627197265625);
}
