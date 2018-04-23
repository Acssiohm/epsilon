#ifndef ESCHER_EXPRESSION_VIEW_WITH_CURSOR_H
#define ESCHER_EXPRESSION_VIEW_WITH_CURSOR_H

#include <escher/expression_view.h>
#include <escher/text_cursor_view.h>
#include <kandinsky/point.h>
#include <poincare/expression_layout.h>
#include <poincare/expression_layout_cursor.h>

class ExpressionViewWithCursor : public View {
public:
  ExpressionViewWithCursor(Poincare::ExpressionLayout * expressionLayout);
  bool isEditing() const { return m_isEditing; }
  void setEditing(bool isEditing);
  void setCursor(Poincare::ExpressionLayoutCursor cursor) { m_cursor = cursor; }
  void cursorPositionChanged();
  KDRect cursorRect();
  Poincare::ExpressionLayoutCursor * cursor() { return &m_cursor; }
  ExpressionView * expressionView() { return &m_expressionView; }
  /* View */
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  enum class Position {
    Top,
    Bottom
  };
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void layoutCursorSubview();
  Poincare::ExpressionLayoutCursor m_cursor;
  ExpressionView m_expressionView;
  TextCursorView m_cursorView;
  bool m_isEditing;
};

#endif
