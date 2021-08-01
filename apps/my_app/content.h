#ifndef MY_MAIN_CONTROLLER_H
#define MY_MAIN_CONTROLLER_H

#include <escher.h>
#include <apps/i18n.h>


namespace My {

class MyController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
	
	public:
	MyController(Responder * parentResponder);
	
	View * view() override;
	// KDCoordinate cellWidth() override;
	bool handleEvent(Ion::Events::Event event) override;
	void willDisplayCellForIndex(HighlightCell * cell, int index) override;
	int numberOfRows() override; 
	KDCoordinate rowHeight(int j) override;
	KDCoordinate cumulatedHeightFromIndex(int j) override;
	int indexFromCumulatedHeight(KDCoordinate offsetY) override;
	HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void didBecomeFirstResponder() override; // ?

private:
	I18n::Message messageAtIndex(int index);

	constexpr static int k_numberOfGaugeCells = 3;
	MessageTableCellWithGauge gaugeCells[k_numberOfGaugeCells];
	SelectableTableView m_table;
};

}

#endif
