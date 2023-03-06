from tester import test_cmd

# Is Valid

# Variables

test_cmd("test unset variable no -", "a=b; echo $a; unset a; echo $?; echo $a")
test_cmd("test unset variable with -v", "a=b; echo $a; unset -v a; echo $?; echo $a")
test_cmd("test unset variable with -f", "a=b; echo $a; unset -f a; echo $?; echo $a")

test_cmd("test unset variable with 2 args", "a=b; b=c; echo $a; echo $b; unset a b; echo $?; echo $a; echo $b")
test_cmd("test unset variable with 2 args -v", "a=b; b=c; echo $a; echo $b; unset -v a b; echo $?; echo $a; echo $b")
test_cmd("test unset variable with 2 args -f", "a=b; b=c; echo $a; echo $b; unset -f a b; echo $?; echo $a; echo $b")

# Functions

test_cmd("test unset function no -", "foo() { echo foo; }; foo; unset foo; echo $?; foo;")
test_cmd("test unset function with -v", "foo() { echo foo; }; foo; unset -v foo; echo $?; foo;")
test_cmd("test unset function with -f", "foo() { echo foo; }; foo; unset -f foo; echo $?; foo;")

test_cmd("test unset function with 2 args", "foo() { echo foo; }; bar() { echo bar; }; foo; bar; unset foo bar; echo $?; foo; bar;")
test_cmd("test unset function with 2 args -v", "foo() { echo foo; }; bar() { echo bar; }; foo; bar; unset -v foo bar; echo $?; foo; bar;")
test_cmd("test unset function with 2 args -f", "foo() { echo foo; }; bar() { echo bar; }; foo; bar; unset -f foo bar; echo $?; foo; bar;")

# Is Not Valid

test_cmd("test unset variable error -vf", "a=b; echo $a; unset -vf a; echo $?; echo $a")
test_cmd("test unset variable error -s", "a=b; echo $a; unset -s a; echo $?; echo $a")

if __name__ == "__main__":
    from tester import summary
    summary()
