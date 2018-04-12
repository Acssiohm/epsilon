#include "toolbox_helpers.h"
#include <ion/charset.h>
#include <apps/i18n.h>
#include <string.h>
#include <assert.h>

namespace Shared {
namespace ToolboxHelpers {

int CursorIndexInCommandText(const char * text) {
  for (size_t i = 0; i < strlen(text); i++) {
    if (text[i] == '(' || text[i] == '\'') {
      return i + 1;
    }
    if (text[i] == ']') {
      return i;
    }
  }
  return strlen(text);
}

void TextToInsertForCommandMessage(I18n::Message message, char * buffer, int bufferSize) {
  TextToInsertForCommandText(I18n::translate(message), buffer, bufferSize);
}

void TextToInsertForCommandText(const char * command, char * buffer, int bufferSize) {
  int currentNewTextIndex = 0;
  int numberOfOpenParentheses = 0;
  int numberOfOpenBrackets = 0;
  bool insideQuote = false;
  size_t commandLength = strlen(command);
  for (size_t i = 0; i < commandLength; i++) {
    if (command[i] == ')') {
      numberOfOpenParentheses--;
    }
    if (command[i] == ']') {
      numberOfOpenBrackets--;
    }
    if (((numberOfOpenParentheses == 0 && numberOfOpenBrackets == 0)
          || command[i] == ','
          || (numberOfOpenBrackets > 0 && (command[i] == ',' || command[i] == '[' || command[i] == ']')))
        && (!insideQuote || command[i] == '\'')) {
      assert(currentNewTextIndex < bufferSize);
      buffer[currentNewTextIndex++] = command[i];
    }
    if (command[i] == '(') {
      numberOfOpenParentheses++;
    }
    if (command[i] == '[') {
      numberOfOpenBrackets++;
    }
    if (command[i] == '\'') {
      insideQuote = !insideQuote;
    }
  }
  buffer[currentNewTextIndex] = 0;
}

void TextToParseIntoLayoutForCommandMessage(I18n::Message message, char * buffer, int bufferSize) {
  TextToParseIntoLayoutForCommandText(I18n::translate(message), buffer, bufferSize);
}

void TextToParseIntoLayoutForCommandText(const char * command, char * buffer, int bufferSize) {
  TextToInsertForCommandText(command, buffer, bufferSize);
  size_t bufferLength = strlen(buffer);
  for (size_t i = 0; i < bufferLength; i++) {
    if (buffer[i] == '(' || buffer[i] == ',' || (i < bufferLength - 1 && buffer[i] == '[' && buffer[i+1] == ']')) {
      // Shift the buffer to make room for the new char. Use memmove to avoid
      // overwritting.
      memmove(&buffer[i+2], &buffer[i+1], bufferLength - (i+1) + 1);
      bufferLength++;
      i++;
      buffer[i] = Ion::Charset::Empty;
    }
  }
}

}
}
