#ifndef CUSTOM_GAUGE_VIEW
#define CUSTOM_GAUGE_VIEW

#include "../../escher/src/gauge_view.cpp"

class CustomGaugeView : public GaugeView {
public:
	CustomGaugeView();
	void drawRect(KDContext * ctx, KDRect rect) const override;
	void setColor(KDColor color);
protected:
	KDColor m_color;
};




#endif