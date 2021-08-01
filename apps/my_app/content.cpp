#include "content.h"

#include <apps/i18n.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace My {
  MyController::MyController(Responder * parentResponder) : 
  ViewController(parentResponder),
  m_table(this) {
    for (int i = 0; i < k_numberOfGaugeCells; i++){
      m_gaugeCells[i].setMessageFont(KDFont::LargeFont);
    }
  }
  View * MyController::view() {
    return &m_table;
  }
  bool MyController::handleEvent(Ion::Events::Event event){
    int rowIndex = selectedRow();
    if (rowIndex < k_numberOfGaugeCells && (event == Ion::Events::Left || event == Ion::Events::Right || event == Ion::Events::Minus || event == Ion::Events::Plus)) {
      float delta = 0.1;
      float direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
      float lvl = ( (GaugeView *) m_gaugeCells[rowIndex].accessoryView() ) -> level();
      ( (GaugeView *) m_gaugeCells[rowIndex].accessoryView() ) -> setLevel(lvl + direction);
      m_table.reloadCellAtLocation(m_table.selectedColumn(), m_table.selectedRow());
      return true;
    }
    if (rowIndex == k_numberOfGaugeCells && (event == Ion::Events::OK || event == Ion::Events::EXE)){
      bool state = ( (SwitchView *) m_switch_cell.accessoryView()) -> state()
      ( (SwitchView *) m_switch_cell.accessoryView()) -> setState( !state );
    }
    return false;
  }
  void MyController::willDisplayCellForIndex(HighlightCell * cell, int index) {
    if (index == k_numberOfGaugeCells){
      MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
      mySwitchCell -> setMessage(messageAtIndex(index));  
      return ;
    }
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    myGaugeCell->setMessage(messageAtIndex(index));
  }
  int MyController::numberOfRows() const {
    return k_numberOfGaugeCells + 1;
  }
  KDCoordinate MyController::rowHeight(int j) {
    return Metric::ParameterCellHeight;
  }
  KDCoordinate MyController::cumulatedHeightFromIndex(int j) {
    return j * rowHeight(0);
  }
  int MyController::indexFromCumulatedHeight(KDCoordinate offsetY) {
    return offsetY / rowHeight(0);
  }
  HighlightCell * MyController::reusableCell(int index, int type) {
    return &m_gaugeCells[index];
  }
  int MyController::reusableCellCount(int type) {
    if (type == 0 ){
      return k_numberOfGaugeCells;
    }
    return 1;
  }
  int MyController::typeAtLocation(int i, int j) {
    if (j < k_numberOfGaugeCells){
      return 0;
    }
    return 1;
  }
  void MyController::didBecomeFirstResponder() {
    if (selectedRow() < 0) {
      selectCellAtLocation(0, 0);
    }
    Container::activeApp()->setFirstResponder(&m_table);
  }

  I18n::Message MyController::messageAtIndex(int index){
    I18n::Message msgs[4] = {I18n::Message::MSG1, I18n::Message::MSG2, I18n::Message::MSG3, I18n::Message::MSG4};
    return msgs[index];
  }
}
