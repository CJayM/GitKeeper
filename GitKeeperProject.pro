TEMPLATE = subdirs

SUBDIRS += \
    git_keeper \
    git_keeper_app \    
    git_keeper_tests 


git_keeper_tests.depends = git_keeper
git_keeper_app.depends = git_keeper
