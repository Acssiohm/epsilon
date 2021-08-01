#ifndef MY_MAIN_CONTROLLER_H
#define MY_MAIN_CONTROLLER_H

#include <escher.h>
#include <apps/i18n.h>


namespace My {

class MyController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  MyController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);
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

	constexpr static int k_numberOfGauges = 3;
  MessageTableCellWithGauge m_gaugeCells[k_numberOfGauges];
  MessageTableCellWithSwitch m_switch;
  SelectableTableView m_selectableTableView;

};

}

#endif
