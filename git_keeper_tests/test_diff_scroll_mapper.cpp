#include "test_diff_scroll_mapper.h"
#include "domain/project.h"

DiffScrollMapper_Test::DiffScrollMapper_Test() {}

DiffScrollMapper_Test::~DiffScrollMapper_Test() {}

void DiffScrollMapper_Test::test_mapped_pos()
{
    const auto REPLACE = DiffOperationType::REPLACE;

    Project diffs("");
    diffs.append(DiffOperation{{10, 1, REPLACE}, {10, 1, REPLACE}});
    diffs.append(DiffOperation{{20, 1, REPLACE}, {30, 1, REPLACE}});
    diffs.append(DiffOperation{{32, 1, REPLACE}, {48, 1, REPLACE}});
    diffs.append(DiffOperation{{64, 1, REPLACE}, {64, 1, REPLACE}});

    QCOMPARE(diffs.getMappedLeftPos(0), 0);
    QCOMPARE(diffs.getMappedLeftPos(1), 1);
    QCOMPARE(diffs.getMappedLeftPos(5), 5);
    QCOMPARE(diffs.getMappedLeftPos(10), 10);
    QCOMPARE(diffs.getMappedLeftPos(20), 15);
    QCOMPARE(diffs.getMappedLeftPos(30), 20);
    QCOMPARE(diffs.getMappedLeftPos(39), 26);
    QCOMPARE(diffs.getMappedLeftPos(48), 32);
    QCOMPARE(diffs.getMappedLeftPos(56), 48);
    QCOMPARE(diffs.getMappedLeftPos(64), 64);
}

int this_method_make_git_keeper_Test_visible_in_QT_tests_panel()
{
    return QTest::qExec(new DiffScrollMapper_Test());
}

int this_method_make_DiffScrollMapper_Test_visible_in_QT_tests_panel() {}
