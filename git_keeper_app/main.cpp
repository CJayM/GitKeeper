#include "app_palette.h"
#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
  QApplication::setStyle(QLatin1String("fusion"));
  QPixmap pixmap(":/resources/splash.png");
  QSplashScreen splash(pixmap);

  git_file_register_metatypes();

  splash.show();

  a.processEvents();
  MainWindow w;
  w.show();
  splash.finish(&w);
  return a.exec();
}
