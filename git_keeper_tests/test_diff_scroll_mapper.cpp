#include "test_diff_scroll_mapper.h"

DiffScrollMapper_Test::DiffScrollMapper_Test() {}

DiffScrollMapper_Test::~DiffScrollMapper_Test() {}

void DiffScrollMapper_Test::test_true()
{
    QVERIFY(true);
}

int this_method_make_git_keeper_Test_visible_in_QT_tests_panel()
{
    return QTest::qExec(new DiffScrollMapper_Test());
}

int this_method_make_DiffScrollMapper_Test_visible_in_QT_tests_panel() {}
