#include "message_table_cell_with_custom_gauge.h"
#include <escher/palette.h>

MessageTableCellWithCustomGauge::MessageTableCellWithCustomGauge(I18n::Message message, const KDFont * font) :
  MessageTableCell(message, font),
  m_accessoryView()
{
}

View * MessageTableCellWithCustomGauge::accessoryView() const {
  return (View *)&m_accessoryView;
}

void MessageTableCellWithCustomGauge::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}
