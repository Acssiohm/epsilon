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
    ( (SwitchView *) m_switch_cell.accessoryView()) -> setState( false );
    for (int i = 0; i < k_numberOfGaugeCells; i++){
      m_gaugeCells[i].setMessageFont(KDFont::LargeFont);
    }
    KDColor currentColor = Ion::LED::getColor();
    ( (CustomGaugeView * ) m_gaugeCells[0].accessoryView()  ) -> setColor(Palette::Red);
    ( (GaugeView * ) m_gaugeCells[0].accessoryView()  ) -> setLevel((float)currentColor.red() / 0xFF);
    ( (CustomGaugeView * ) m_gaugeCells[1].accessoryView()  ) -> setColor(Palette::Green);
    ( (GaugeView * ) m_gaugeCells[1].accessoryView()  ) -> setLevel((float)currentColor.green() / 0xFF);
    ( (CustomGaugeView * ) m_gaugeCells[2].accessoryView()  ) -> setColor(Palette::Blue);
    ( (GaugeView * ) m_gaugeCells[2].accessoryView()  ) -> setLevel((float)currentColor.blue() / 0xFF);

  }

  View * MyController::view() {
    return &m_table;
  }

  KDColor MyController::gaugesSelectedColor(){
    uint8_t r = ( (GaugeView *) m_gaugeCells[0].accessoryView() ) -> level() * 0xFF;
    uint8_t g = ( (GaugeView *) m_gaugeCells[1].accessoryView() ) -> level() * 0xFF;
    uint8_t b = ( (GaugeView *) m_gaugeCells[2].accessoryView() ) -> level() * 0xFF;
    return KDColor::RGB888(r, g, b);    
  }
  int MyController::gaugesSelectedPeriod(){
     return ( (GaugeView *) m_gaugeCells[3].accessoryView() ) -> level() * 1999 + 1;
  }
  float MyController::gaugesSelectedDutyCycle(){
     return ( (GaugeView *) m_gaugeCells[4].accessoryView() ) -> level();
  }
  void MyController::updateGaugeLevel(int index, Ion::Events::Event event){
    float delta = (event == Ion::Events::Right || event == Ion::Events::Left) ? 0.1 : 0.02;
    float direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
    float lvl = ( (GaugeView *) m_gaugeCells[index].accessoryView() ) -> level();
    ( (GaugeView *) m_gaugeCells[index].accessoryView() ) -> setLevel(lvl + direction);
    m_table.reloadCellAtLocation(m_table.selectedColumn(), m_table.selectedRow());
  }
  void MyController::setLEDBlinking(bool blink){
    if (blink){
        Ion::LED::setBlinking(gaugesSelectedPeriod(), gaugesSelectedDutyCycle());
      }else {
        Ion::LED::setColor( Ion::LED::getColor());
      }
  }
  bool MyController::handleEvent(Ion::Events::Event event){
    int rowIndex = selectedRow();
    if ( rowIndex != k_indexOfSwitchCell && (event == Ion::Events::Left || event == Ion::Events::Right || event == Ion::Events::Minus || event == Ion::Events::Plus)) {
      
      int gaugeIndex = rowIndex >= k_numberOfGaugeCells - 1 ? rowIndex - 1 : rowIndex; 
     
      updateGaugeLevel(gaugeIndex, event);
      
      KDColor clr = gaugesSelectedColor();
      Ion::LED::setColor(clr);
      ( (CustomGaugeView * ) m_gaugeCells[3].accessoryView()  ) -> setColor(clr);
      if ( ( (SwitchView *) m_switch_cell.accessoryView()) -> state() ){
        setLEDBlinking(true);
      }
      return true;
    }
    if (rowIndex == k_indexOfSwitchCell && (event == Ion::Events::OK || event == Ion::Events::EXE)){
      bool state = ( (SwitchView *) m_switch_cell.accessoryView()) -> state() ;
      ( (SwitchView *) m_switch_cell.accessoryView()) -> setState( !state );
      setLEDBlinking(!state);
      m_table.reloadCellAtLocation(m_table.selectedColumn(), m_table.selectedRow());
      return true;
    }
    return false;
  }

  void MyController::willDisplayCellForIndex(HighlightCell * cell, int index) {
    if (index == k_numberOfGaugeCells - 2){
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
    if (j < k_numberOfGaugeCells - 2 || j >= k_numberOfGaugeCells - 1  ){
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
    I18n::Message msgs[] = {I18n::Message::MSG1, I18n::Message::MSG2, I18n::Message::MSG3, I18n::Message::MSG4, I18n::Message::MSG45 , I18n::Message::MSG46 };
    return msgs[index];
  }
}
