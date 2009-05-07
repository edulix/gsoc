#include "bookmarks.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>

static const char description[] =
    I18N_NOOP("A KDE 4 Application");

static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("bookmarks", 0, ki18n("bookmarks"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2007 Eduardo Robles Elvira"), KLocalizedString(), 0, "edulix@gmail.com");
    about.addAuthor( ki18n("Eduardo Robles Elvira"), KLocalizedString(), "edulix@gmail.com" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[URL]", ki18n( "Document to open" ));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    Bookmarks *widget = new Bookmarks;

    // see if we are starting with session management
    if (app.isSessionRestored())
    {
        RESTORE(Bookmarks);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->count() == 0)
        {
            //bookmarks *widget = new bookmarks;
            widget->show();
        }
        else
        {
            int i = 0;
            for (; i < args->count(); i++)
            {
                //bookmarks *widget = new bookmarks;
                widget->show();
            }
        }
        args->clear();
    }

    return app.exec();
}
