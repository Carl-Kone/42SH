true
echo $?
echo "Hello World"
echo -n "Hello World"
false
echo $?
true && echo "Command succeeded"
false && echo "Command succeeded"
false || echo "Command failed"
true || echo "Command failed"

