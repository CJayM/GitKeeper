TEMPLATE = subdirs

SUBDIRS += \
    git_keeper_app \    
    git_keeper_tests 


git_keeper_tests.depends = git_keeper_app
