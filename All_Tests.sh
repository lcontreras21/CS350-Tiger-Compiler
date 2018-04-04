#!/bin/sh
WHERE=${HOME}/cs350/
WHAT=${WHERE}/tiger/Debug/tiger
export TIGER_TEST_THERE=${WHERE}/A-folder-of-examples/Tiger-tests

for d in davew others
do
    cd $TIGER_TEST_THERE/$d
    if test "${#}" -eq 0
    then
	TESTS=*.tig
    else
	TESTS=${*}*.tig
    fi

    $WHERE/A-folder-of-examples/Tiger-tests/Run-tests.sh ${TESTS}
done


