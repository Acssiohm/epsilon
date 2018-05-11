#include <escher/expression_layout_field.h>
#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <poincare/expression.h>
#include <poincare/expression_layout_cursor.h>
#include <poincare/src/layout/matrix_layout.h>
#include <assert.h>
#include <string.h>

ExpressionLayoutField::ExpressionLayoutField(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, ExpressionLayoutFieldDelegate * delegate) :
  ScrollableView(parentResponder, &m_contentView, this),
  m_contentView(expressionLayout),
  m_delegate(delegate)
{
}

bool ExpressionLayoutField::isEditing() const {
  return m_contentView.isEditing();
}

void ExpressionLayoutField::setEditing(bool isEditing) {
  m_contentView.setEditing(isEditing);
}

void ExpressionLayoutField::clearLayout() {
  m_contentView.clearLayout();
}

void ExpressionLayoutField::scrollToCursor() {
  // Show the whole cursor
  scrollToContentRect(m_contentView.cursorRect(), true);
  // Show the cursor area around its baseline
  Poincare::ExpressionLayoutCursor * cursor = m_contentView.cursor();
  KDCoordinate cursorBaseline = cursor->baseline();
  KDCoordinate underBaseline = m_contentView.cursorRect().height() - cursorBaseline;
  KDCoordinate minAroundBaseline = min(cursorBaseline, underBaseline);
  minAroundBaseline = min(minAroundBaseline, bounds().height() / 2);
  KDRect balancedRect(m_contentView.cursorRect().x(), m_contentView.cursorRect().y() + cursorBaseline - minAroundBaseline, Poincare::ExpressionLayoutCursor::k_cursorWidth, 2 * minAroundBaseline);
  scrollToContentRect(balancedRect, true);
}

Toolbox * ExpressionLayoutField::toolbox() {
  if (m_delegate) {
    return m_delegate->toolboxForExpressionLayoutField(this);
  }
  return nullptr;
}

bool ExpressionLayoutField::handleEvent(Ion::Events::Event event) {
  bool didHandleEvent = false;
  bool shouldRecomputeLayout = m_contentView.cursor()->showEmptyLayoutIfNeeded();
  bool moveEventChangedLayout = false;
  if (privateHandleMoveEvent(event, &moveEventChangedLayout)) {
    if (!isEditing()) {
      setEditing(true);
    }
    shouldRecomputeLayout = shouldRecomputeLayout || moveEventChangedLayout;
    didHandleEvent = true;
  } else if (privateHandleEvent(event)) {
    shouldRecomputeLayout = true;
    didHandleEvent = true;
  }
  if (didHandleEvent) {
    shouldRecomputeLayout = m_contentView.cursor()->hideEmptyLayoutIfNeeded() || shouldRecomputeLayout;
    if (!shouldRecomputeLayout) {
      m_contentView.cursorPositionChanged();
      scrollToCursor();
    } else {
      reload();
    }
    return true;
  }
  m_contentView.cursor()->hideEmptyLayoutIfNeeded();
  return false;
}

bool ExpressionLayoutField::expressionLayoutFieldShouldFinishEditing(Ion::Events::Event event) {
  return m_delegate->expressionLayoutFieldShouldFinishEditing(this, event);
}

KDSize ExpressionLayoutField::minimalSizeForOptimalDisplay() const {
  KDSize contentViewSize = m_contentView.minimalSizeForOptimalDisplay();
  return KDSize(contentViewSize.width(), contentViewSize.height());
}

bool ExpressionLayoutField::privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout) {
  Poincare::ExpressionLayoutCursor result;
  if (event == Ion::Events::Left) {
    result = m_contentView.cursor()->cursorOnLeft(shouldRecomputeLayout);
  } else if (event == Ion::Events::Right) {
    result =  m_contentView.cursor()->cursorOnRight(shouldRecomputeLayout);
  } else if (event == Ion::Events::Up) {
    result = m_contentView.cursor()->cursorAbove(shouldRecomputeLayout);
  } else if (event == Ion::Events::Down) {
    result = m_contentView.cursor()->cursorUnder(shouldRecomputeLayout);
  } else if (event == Ion::Events::ShiftLeft) {
    *shouldRecomputeLayout = true;
    if (m_contentView.cursor()->pointedExpressionLayout()->removeGreySquaresFromAllMatrixAncestors()) {
      *shouldRecomputeLayout = true;
    }
    result.setPointedExpressionLayout(m_contentView.expressionView()->expressionLayout());
    result.setPosition(Poincare::ExpressionLayoutCursor::Position::Left);
  } else if (event == Ion::Events::ShiftRight) {
    if (m_contentView.cursor()->pointedExpressionLayout()->removeGreySquaresFromAllMatrixAncestors()) {
      *shouldRecomputeLayout = true;
    }
    result.setPointedExpressionLayout(m_contentView.expressionView()->expressionLayout());
    result.setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
  }
  if (result.isDefined()) {
    m_contentView.setCursor(result);
    return true;
  }
  return false;
}

bool ExpressionLayoutField::privateHandleEvent(Ion::Events::Event event) {
  if (m_delegate && m_delegate->expressionLayoutFieldDidReceiveEvent(this, event)) {
    return true;
  }
  if (Responder::handleEvent(event)) {
    /* The only event Responder handles is 'Toolbox' displaying. In that case,
     * the ExpressionLayoutField is forced into editing mode. */
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && expressionLayoutFieldShouldFinishEditing(event)) {
    setEditing(false);
    int bufferSize = TextField::maxBufferSize();
    char buffer[bufferSize];
    m_contentView.expressionView()->expressionLayout()->writeTextInBuffer(buffer, bufferSize);
    if (m_delegate->expressionLayoutFieldDidFinishEditing(this, buffer, event)) {
      clearLayout();
    }
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    setEditing(true);
    m_contentView.cursor()->setPointedExpressionLayout(m_contentView.expressionView()->expressionLayout());
    m_contentView.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
    return true;
  }
  if (event == Ion::Events::Back && isEditing()) {
    clearLayout();
    setEditing(false);
    m_delegate->expressionLayoutFieldDidAbortEditing(this);
    return true;
  }
  if (event == Ion::Events::Division) {
    if (!isEditing()) {
      setEditing(true);
    }
    m_contentView.cursor()->addFractionLayoutAndCollapseSiblings();
    return true;
  }
  if (event == Ion::Events::Exp) {
    if (!isEditing()) {
      setEditing(true);
    }
    m_contentView.cursor()->addEmptyExponentialLayout();
    return true;
  }
  if (event == Ion::Events::Power) {
    if (!isEditing()) {
      setEditing(true);
    }
    m_contentView.cursor()->addEmptyPowerLayout();
    return true;
  }
  if (event == Ion::Events::Sqrt) {
    if (!isEditing()) {
      setEditing(true);
    }
    m_contentView.cursor()->addEmptySquareRootLayout();
    return true;
  }
  if (event == Ion::Events::Square) {
    if (!isEditing()) {
      setEditing(true);
    }
    m_contentView.cursor()->addEmptySquarePowerLayout();
    return true;
  }
  if (event == Ion::Events::EE) {
    if (!isEditing()) {
      setEditing(true);
    }
    m_contentView.cursor()->addEmptyTenPowerLayout();
    return true;
  }
  if (event.hasText()) {
    if (!isEditing()) {
      setEditing(true);
    }
    const char * textToInsert = event.text();
    if (textToInsert[1] == 0 && (textToInsert[0] == '[' || textToInsert[0] == ']')) {
      m_contentView.cursor()->addEmptyMatrixLayout();
      return true;
    }
    m_contentView.cursor()->insertText(textToInsert);
    return true;
  }
  if (event == Ion::Events::Backspace) {
    if (!isEditing()) {
      setEditing(true);
    }
    m_contentView.cursor()->performBackspace();
    return true;
  }
  if (event == Ion::Events::Paste) {
    if (!isEditing()) {
      setEditing(true);
    }
    insertLayoutFromTextAtCursor(Clipboard::sharedClipboard()->storedText());
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    clearLayout();
    return true;
  }
  return false;
}

void ExpressionLayoutField::insertLayoutAtCursor(Poincare::ExpressionLayout * layout, Poincare::ExpressionLayout * pointedLayout) {
  if (layout == nullptr) {
    return;
  }
  m_contentView.cursor()->showEmptyLayoutIfNeeded();
  if (layout->isMatrix() && pointedLayout && pointedLayout->hasAncestor(layout)) {
    static_cast<Poincare::MatrixLayout *>(layout)->addGreySquares();
  }
  bool layoutWillBeMerged = layout->isHorizontal();
  m_contentView.cursor()->addLayoutAndMoveCursor(layout);
  if (pointedLayout != nullptr && (pointedLayout != layout || !layoutWillBeMerged)) {
    m_contentView.cursor()->setPointedExpressionLayout(pointedLayout);
    m_contentView.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
  } else if (!layoutWillBeMerged) {
    m_contentView.cursor()->setPointedExpressionLayout(layout->layoutToPointWhenInserting());
    m_contentView.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
  }
  m_contentView.cursor()->hideEmptyLayoutIfNeeded();
  reload();
}

void ExpressionLayoutField::insertLayoutFromTextAtCursor(const char * text) {
  m_contentView.cursor()->showEmptyLayoutIfNeeded();
  Poincare::Expression * expression = Poincare::Expression::parse(text);
  if (expression != nullptr) {
    Poincare::ExpressionLayout * layout = expression->createLayout();
    delete expression;
    insertLayoutAtCursor(layout, layout);
  } else {
    m_contentView.cursor()->insertText(text);
  }
  m_contentView.cursor()->hideEmptyLayoutIfNeeded();
  reload();
}

void ExpressionLayoutField::reload() {
  KDSize previousSize = minimalSizeForOptimalDisplay();
  m_contentView.expressionView()->expressionLayout()->invalidAllSizesPositionsAndBaselines();
  KDSize newSize = minimalSizeForOptimalDisplay();
  if (m_delegate && previousSize.height() != newSize.height()) {
    m_delegate->expressionLayoutFieldDidChangeSize(this);
  }
  m_contentView.cursorPositionChanged();
  scrollToCursor();
  markRectAsDirty(bounds());
}

bool ExpressionLayoutField::hasText() const {
  return m_contentView.expressionView()->expressionLayout()->hasText();
}

void ExpressionLayoutField::writeTextInBuffer(char * buffer, int bufferLength) {
  m_contentView.expressionView()->expressionLayout()->writeTextInBuffer(buffer, bufferLength);
}

bool ExpressionLayoutField::handleEventWithText(const char * text, bool indentation) {
  size_t textLength = strlen(text) + 1;
  size_t textToInsertMaxLength = 2*textLength;
  char textToInsert[textToInsertMaxLength];

  size_t textToInsertIndex = 0;
  // Add empty chars where arguments are needed
  for (size_t i = textToInsertIndex; i < textLength; i++) {
    textToInsert[textToInsertIndex++] = text[i];
    if (((text[i] == '(' || text[i] == '[')
          && text[i+1] == ',')
        || (text[i] == ','
          && (text[i+1] == ')' || text[i+1] == ']')))
    {
      textToInsert[textToInsertIndex++] = Ion::Charset::Empty;
    }
  }

  Poincare::Expression * resultExpression = Poincare::Expression::parse(textToInsert);
  if (resultExpression == nullptr) {
    return false;
  }
  Poincare::ExpressionLayout * resultLayout = resultExpression->createLayout();
  delete resultExpression;
  // Find the pointed layout.
  Poincare::ExpressionLayout * pointedLayout = nullptr;
  if (resultLayout->isHorizontal()) {
    /* If the layout is horizontal, pick the first open parenthesis. For now,
     * all horizontal layouts in MathToolbox have parentheses. */
    for (int i = 0; i < resultLayout->numberOfChildren(); i++) {
      if (resultLayout->editableChild(i)->isLeftParenthesis()) {
        pointedLayout = resultLayout->editableChild(i);
        break;
      }
    }
  }
  /* Insert the layout. If pointedLayout is nullptr, the cursor will be on the
   * right of the inserted layout. */
  insertLayoutAtCursor(resultLayout, pointedLayout);
  return true;
}

Poincare::ExpressionLayout * ExpressionLayoutField::expressionLayout() {
  return m_contentView.expressionView()->expressionLayout();
}

char ExpressionLayoutField::XNTChar() {
  return m_contentView.cursor()->pointedExpressionLayout()->XNTChar();
}

void ExpressionLayoutField::setBackgroundColor(KDColor c) {
  ScrollableView::setBackgroundColor(c);
  m_contentView.setBackgroundColor(c);
}
