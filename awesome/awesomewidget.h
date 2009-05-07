#ifndef AWESOMEWIDGET_H
#define AWESOMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class AwesomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AwesomeWidget(QWidget *parent = 0, const char* name = 0);
    ~AwesomeWidget();

private:
    QVBoxLayout m_layout;
    QLabel m_label;
};

#endif /* AWESOMEWIDGET_H */
