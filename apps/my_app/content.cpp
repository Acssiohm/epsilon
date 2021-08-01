#include "content.h"

#include <apps/i18n.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace My {


MyController::MyController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  ViewController(parentResponder),
  m_switch(I18n::Message::Default, KDFont::LargeFont),
  m_selectableTableView(this)
{
  for ( int i = 0 ; i < k_numberOfGauges; i++){
    m_gaugeCells[i] = MessageTableCellWithGauge(I18n::Message::Default, KDFont::LargeFont);
  }
}

View * MyController::view() {
  return &m_selectableTableView;
}

void MyController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool MyController::handleEvent(Ion::Events::Event event) {
  int rowIndex = selectedRow();

  if (rowIndex < k_numberOfGauges
   && (event == Ion::Events::Left || event == Ion::Events::Right || event == Ion::Events::Minus || event == Ion::Events::Plus)) {
    float delta = 0.1;
    float direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
    float lvl =  (&m_gaugeCells[rowIndex]) -> accessoryView() -> level();
    (&m_gaugeCells[rowIndex]) -> accessoryView()-> setLevel(lvl + direction);
    m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
    return true;
  }

  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (rowIndex == k_numberOfGauges) {
      bool state = (&m_switch)->accessoryView()->state();
      m_switch->accessoryView()->setState(!state);
      
    }

    return true;
  }

  return false;
}

int MyController::numberOfRows() const {
  return k_numberOfGauges + 1;
};

KDCoordinate MyController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

KDCoordinate MyController::cumulatedHeightFromIndex(int j) {
  KDCoordinate height = j * rowHeight(0);
  return height;
}

int MyController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return offsetY/rowHeight(0);
}

HighlightCell * MyController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == 0) {
    return &m_gaugeCells[index];
  }
  return &m_switch;
  
}

int MyController::reusableCellCount(int type) {
  if (type == 0) {
    return k_numberOfGauges;
  }
  return 1;
}

int MyController::typeAtLocation(int i, int j) {
  if (j <= k_numberOfGauges){
    return 0;
  }
  return 1;
}

void MyController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index <= k_numberOfGauges) {
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    myGaugeCell->setMessage(messageAtIndex(index));

    return;
  }
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(messageAtIndex(index));
  MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
  SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
  mySwitch->setState(0);
  return;

}

void MyController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.reloadData();
}

I18n::Message messageAtIndex(int index){
  I18n::Message m[] = {I18n::Message::MSG1, I18n::Message::MSG2, I18n::Message::MSG3, I18n::Message::MSG4}
  return m[index]
}

}
