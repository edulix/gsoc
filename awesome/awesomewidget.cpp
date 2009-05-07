#include "awesomewidget.h"

AwesomeWidget::AwesomeWidget(QWidget *parent, const char *name)
  : QWidget(parent), m_layout(this), m_label(name)
{
    m_layout.addWidget(&m_label);
}

AwesomeWidget::~AwesomeWidget()
{
    
}
