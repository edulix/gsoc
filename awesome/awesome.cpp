#include <kcomponentdata.h>
#include <qstring.h>
#include <kimageio.h>
#include <klocale.h>
#include <kdemacros.h>

#include "awesomewidget.h"
#include "awesome.h"
#include "awesome.moc"

Awesome::Awesome(const KComponentData &inst,QObject *parent,QWidget *widgetParent, QString &desktopName, const char* name)
    : KonqSidebarPlugin(inst, parent, widgetParent, desktopName, name)
{
    KGlobal::locale()->insertCatalog("konqsidebar_awesome");

    widget = new AwesomeWidget(widgetParent, "awesomewidget");
}


Awesome::~Awesome()
{
    
}



extern "C"
{
    bool add_konqsidebar_awesome(QString* fn, QString* param, QMap<QString,QString> *map)
    {
        Q_UNUSED(param);

        map->insert("Type", "Link");
        map->insert("Icon", "awesome");
        map->insert("Name", "Awesome");
        map->insert("Open", "true");
        map->insert("X-KDE-KonqSidebarModule", "konqsidebar_awesome");
        fn->fromLatin1("awesome%1.desktop");
        return true;
    }
}

extern "C"
{
    void* create_konqsidebar_awesome(const KComponentData &instance, QObject *par, QWidget *widp, QString &desktopname, const char *name)
    {
        KGlobal::locale()->insertCatalog("konqsidebar_awesome");
        return new Awesome(instance, par, widp, desktopname, name);
    }
}
