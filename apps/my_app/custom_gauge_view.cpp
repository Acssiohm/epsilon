#include "custom_gauge_view.h"

CustomGaugeView::CustomGaugeView(): 
	GaugeView(),
	m_color(Palette::Red)
{	
}

void CustomGaugeView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), m_backgroundColor);
  /* Draw the gauge centered vertically on all the width available */
  KDCoordinate width = bounds().width()-k_indicatorDiameter;
  KDCoordinate height =  bounds().height();
  KDColor gaugeIndicatorWorkingBuffer[GaugeView::k_indicatorDiameter*GaugeView::k_indicatorDiameter];

  ctx->fillRect(KDRect(k_indicatorDiameter/2, (height-k_thickness)/2, width*m_level, k_thickness), color);
  ctx->fillRect(KDRect(k_indicatorDiameter/2+width*m_level, (height-k_thickness)/2, width*(1.0f-m_level), k_thickness), Palette::GrayDark);
  KDRect frame(width*m_level, (height-k_indicatorDiameter)/2, k_indicatorDiameter, k_indicatorDiameter);
  ctx->blendRectWithMask(frame, m_color, (const uint8_t *)gaugeIndicatorMask, gaugeIndicatorWorkingBuffer);
}
void CustomGaugeView::setColor(KDColor color){
	if (m_color != color){
		m_color = color;
		markRectAsDirty(bounds());
	}
}
