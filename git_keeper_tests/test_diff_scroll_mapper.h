#include <QtTest>

#include "diff.h"

class DiffScrollMapper_Test : public QObject
{
    Q_OBJECT

  public:
      DiffScrollMapper_Test();
      ~DiffScrollMapper_Test();

  private:
  private slots:
      void test_mapped_pos();
};

int this_method_make_DiffScrollMapper_Test_visible_in_QT_tests_panel();
