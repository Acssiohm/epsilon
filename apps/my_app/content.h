#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher.h>
#include <apps/shared/settings_message_tree.h>
#include "message_table_cell_with_gauge_with_separator.h"
#include "sub_menu/about_controller.h"
#include "sub_menu/display_mode_controller.h"
#include "sub_menu/exam_mode_controller.h"
#include "sub_menu/localization_controller.h"
#include "sub_menu/preferences_controller.h"

namespace My {

class MainController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  MainController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void viewWillAppear() override;
  TELEMETRY_ID("");
private:
	I18n::Message messageAtIndex(int index);
	
	constexpr k_numberOfGauges = 3
  MessageTableCellWithGauge m_gaugeCells[k_numberOfGauges];
  MessageTableCellWithSwitch m_switch;
  SelectableTableView m_selectableTableView;

};

}

#endif
