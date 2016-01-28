WHERE=${HOME}/cs350-b/
WHAT=${WHERE}/tiger/Debug/tiger
TWHERE="$WHERE/Examples/Tiger-tests/davew $WHERE/Examples/Tiger-tests/others "

echo "Running $WHAT on all tests in $TWHERE"
$WHERE/Examples/Tiger-tests/Run-tests.sh `echo "$TWHERE" | sed -e 's/ /\/*.tig /g'`

