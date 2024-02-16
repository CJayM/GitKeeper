#include "test_diff_scroll_mapper.h"

#include <QtTest/QtTest>

int main(int argc, char* argv[])
{
    int result = 0;

    auto RUN_TEST = [&result, argc, argv](QObject* tst) {
        result |= QTest::qExec(tst, argc, argv);
        delete tst;
    };

    RUN_TEST(new DiffScrollMapper_Test());

    return result;
}
