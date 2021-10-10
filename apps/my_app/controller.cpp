#include "controller.h"
#include <kandinsky.h>
#include <ion.h>
#include <apps/i18n.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace My {

  MyController::MyController(Responder * parentResponder) : 
  ViewController(parentResponder),
  m_table(this) {
    m_switch_cell.setMessageFont(KDFont::LargeFont);
    for (int i = 0; i < k_numberOfGaugeCells; i++){
      m_gaugeCells[i].setMessageFont(KDFont::LargeFont);
    }
    ( (CustomGaugeView * ) m_gaugeCells[0].accessoryView()  ) -> setColor(Palette::Red);
    ( (CustomGaugeView * ) m_gaugeCells[1].accessoryView()  ) -> setColor(Palette::Green);
    ( (CustomGaugeView * ) m_gaugeCells[2].accessoryView()  ) -> setColor(Palette::Blue);
  }
  View * MyController::view() {
    return &m_table;
  }

  bool MyController::handleEvent(Ion::Events::Event event){
    int rowIndex = selectedRow();
    if ( rowIndex != k_numberOfGaugeCells - 1 && (event == Ion::Events::Left || event == Ion::Events::Right || event == Ion::Events::Minus || event == Ion::Events::Plus)) {
      int gaugeIndex = rowIndex >= k_numberOfGaugeCells ? rowIndex - 1 : rowIndex; 
      float delta = 0.1;
      float direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
      float lvl = ( (GaugeView *) m_gaugeCells[gaugeIndex].accessoryView() ) -> level();
      ( (GaugeView *) m_gaugeCells[gaugeIndex].accessoryView() ) -> setLevel(lvl + direction);
      m_table.reloadCellAtLocation(m_table.selectedColumn(), m_table.selectedRow());
      float r = ( (GaugeView *) m_gaugeCells[0].accessoryView() ) -> level() * (float)0xFF;
      float g = ( (GaugeView *) m_gaugeCells[1].accessoryView() ) -> level() * (float)0xFF;
      float b = ( (GaugeView *) m_gaugeCells[2].accessoryView() ) -> level() * (float)0xFF;
      KDColor clr = KDColor::RGB888(r, g, b);
      Ion::LED::setColor(clr);
      ( (CustomGaugeView * ) m_gaugeCells[3].accessoryView()  ) -> setColor(clr);
      return true;
    }
    if (rowIndex == k_numberOfGaugeCells - 1 && (event == Ion::Events::OK || event == Ion::Events::EXE)){
      bool state = ( (SwitchView *) m_switch_cell.accessoryView()) -> state() ;
      ( (SwitchView *) m_switch_cell.accessoryView()) -> setState( !state );
      m_table.reloadCellAtLocation(m_table.selectedColumn(), m_table.selectedRow());
      return true;
    }
    return false;
  }
  void MyController::willDisplayCellForIndex(HighlightCell * cell, int index) {
    if (index == k_numberOfGaugeCells - 1){
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
    if (type == 0){
      return &m_gaugeCells[index];
    }
    return &m_switch_cell;
  }
  int MyController::reusableCellCount(int type) {
    if (type == 0 ){
      return k_numberOfGaugeCells;
    }
    return 1;
  }
  int MyController::typeAtLocation(int i, int j) {
    if (j < k_numberOfGaugeCells - 1 || j == k_numberOfGaugeCells ){
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
    I18n::Message msgs[] = {I18n::Message::MSG1, I18n::Message::MSG2, I18n::Message::MSG3, I18n::Message::MSG4, I18n::Message::MSG45 };
    return msgs[index];
  }
}
