#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CUSTOM_GAUGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CUSTOM_GAUGE_H

#include <escher/message_table_cell.h>
#include "custom_gauge_view.h"

class MessageTableCellWithCustomGauge : public MessageTableCell {
public:
  MessageTableCellWithCustomGauge(I18n::Message message = (I18n::Message)0, const KDFont * font = KDFont::SmallFont);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
private:
  CustomGaugeView m_accessoryView;
};

#endif
