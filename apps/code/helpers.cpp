#include "helpers.h"
#include <escher/clipboard.h>
#include <ion/events.h>
#include <layout_events.h>
#include <string.h>

using namespace Escher;
namespace Code {
namespace Helpers {

const char * PythonTextForEvent(Ion::Events::Event event) {
  if (event == Ion::Events::XNT) {
    return "x";
  }
  if (event.text() == nullptr) {
    return nullptr;
  }
  for (size_t i=0; i<NumberOfPythonTextPairs; i++) {
    UTF8Helper::TextPair pair = PythonTextPairs[i];
    char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
    Ion::Events::copyText(static_cast<uint8_t>(event), buffer, Ion::Events::EventData::k_maxDataSize);
    if (strcmp(buffer, pair.firstString()) == 0) {
      return pair.secondString();
    }
  }
  return nullptr;
}

}
}
