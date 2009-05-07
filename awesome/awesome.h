#ifndef AWESOME_H
#define AWESOME_H

#include <konqsidebarplugin.h>
#include <QString>
#include <kconfig.h>


class Awesome : public KonqSidebarPlugin
{
    Q_OBJECT

  public:
    Awesome(const KComponentData &inst,QObject *parent,QWidget *widgetParent, QString &desktopName, const char* name=0);
    ~Awesome();

    virtual QWidget *getWidget(){ return widget; }
    virtual void *provides(const QString &) { return 0; }

  protected:
    virtual void handleURL(const KUrl &) {  }
    
    AwesomeWidget *widget;

};

#endif /* AWESOME_H */
