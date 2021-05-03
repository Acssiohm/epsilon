#include <quiz.h>
#include "../clipboard.h"
#include <string.h>
#include <poincare/expression.h>

using namespace Escher;
using namespace Poincare;

namespace Code {

void assert_clipboard_enters_and_exits_python(const char * string, const char * stringResult) {
  Clipboard * clipboard = Clipboard::sharedClipboard();
  clipboard->store(string);
  clipboard->enterPython();
  quiz_assert(strcmp(clipboard->storedText(), stringResult) == 0);
  clipboard->exitPython();
  quiz_assert(strcmp(clipboard->storedText(), string) == 0);
}

QUIZ_CASE(code_clipboard_enters_and_exits_python) {
  assert_clipboard_enters_and_exits_python("4×4", "4*4");
  assert_clipboard_enters_and_exits_python("ℯ^\u00121+2\u0013", "exp\u00121+2\u0013");
  assert_clipboard_enters_and_exits_python("ℯ^(ln(4))", "exp(log(4))");
  assert_clipboard_enters_and_exits_python("ln(log(ln(π)))^𝐢", "log(log10(log(pi)))**1j");
  assert_clipboard_enters_and_exits_python("√(1ᴇ10)", "sqrt(1e10)");
  assert_clipboard_enters_and_exits_python("1×𝐢^2", "1*1j**2");
  assert_clipboard_enters_and_exits_python("12^(1/4)×(π/6)×(12×π)^(1/4)", "12**(1/4)*(pi/6)*(12*pi)**(1/4)");
  /* This fails, as 1+e^x -> 1+expx, but should never occur : ℯ^ should always
   * have its exponent in parentheses.
  assert_clipboard_enters_and_exits_python("1+ℯ^x", "1+exp(x)");
   */
  Expression e = Expression::Parse("1+ℯ^x", nullptr);
  char buffer[32];
  e.serialize(buffer, sizeof(buffer));
  assert_clipboard_enters_and_exits_python(buffer, "1+exp\u0012x\u0013");
}

}
