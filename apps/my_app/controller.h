#ifndef MY_MAIN_CONTROLLER_H
#define MY_MAIN_CONTROLLER_H

#include <escher.h>
#include <apps/i18n.h>
// #include "custom_gauge_view.h"
#include "message_table_cell_with_custom_gauge.h"

namespace My {

class MyController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
	
public:
	MyController(Responder * parentResponder);
	const char * title() { return I18n::translate(I18n::Message::MSG5); }
	View * view() override;
	// KDCoordinate cellWidth() override;
	bool handleEvent(Ion::Events::Event event) override;
	void willDisplayCellForIndex(HighlightCell * cell, int index) override;
	int numberOfRows() const override; 
	KDCoordinate rowHeight(int j) override;
	KDCoordinate cumulatedHeightFromIndex(int j) override;
	int indexFromCumulatedHeight(KDCoordinate offsetY) override;
	HighlightCell * reusableCell(int index, int type) override;
	int reusableCellCount(int type) override;
	int typeAtLocation(int i, int j) override;
	void didBecomeFirstResponder() override; // ?
	KDColor gaugesSelectedColor();
	void updateGaugeLevel(int index, Ion::Events::Event event);
	int gaugesSelectedPeriod();
	float gaugesSelectedDutyCycle();
private:
	I18n::Message messageAtIndex(int index);

	constexpr static int k_numberOfGaugeCells = 3+2;
	MessageTableCellWithCustomGauge m_gaugeCells[k_numberOfGaugeCells];
	MessageTableCellWithSwitch m_switch_cell;
	SelectableTableView m_table;
};

}

#endif
