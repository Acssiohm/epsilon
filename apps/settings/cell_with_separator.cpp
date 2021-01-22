#include "cell_with_separator.h"
#include <escher/metric.h>

using namespace Escher;

namespace Settings {

void CellWithSeparator::setHighlighted(bool highlight) {
  cell()->setHighlighted(highlight);
  HighlightCell::setHighlighted(highlight);
}

void CellWithSeparator::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, Metric::CellSeparatorThickness, bounds().width(), k_margin), Palette::WallScreen);
}

int CellWithSeparator::numberOfSubviews() const {
  return 1;
}

View * CellWithSeparator::subviewAtIndex(int index) {
  assert(index == 0);
  return cell();
}

void CellWithSeparator::layoutSubviews(bool force) {
  cell()->setFrame(KDRect(0, k_margin, bounds().width(), bounds().height()-k_margin), force);
}

KDSize CellWithSeparator::minimalSizeForOptimalDisplay() const {
  // TODO Hugo : cell() isn't const, find a workaround.
  // KDSize cellSize = cell()->minimalSizeForOptimalDisplay();
  // return KDSize(cellSize.width(), k_margin + cellSize.height());
  return KDSize(0, k_margin);
}

}
