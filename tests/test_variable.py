from tester import test_cmd

test_cmd("variable assignment 01", "a=b")
test_cmd("variable assignment 02", "a=b''")
test_cmd("variable assignment 03", "a='b'")
test_cmd("variable assignment 04", "a'='b")
test_cmd("variable assignment 05", "'a'=b")
test_cmd("variable assignment 06", "a''=b")
test_cmd("variable assignment 07", "''a=b")
test_cmd("variable assignment 08", "\"a\"=b")
test_cmd("variable assignment 09", "\"a=b\"")
test_cmd("variable assignment 10", "1=b")
test_cmd("not assigned", "echo $a")
test_cmd("not assigned quoted", "echo \"$a\"")
test_cmd("not assigned quoted 2", "echo \"$a\" test")
test_cmd("01", "a=ec; b=ho; c=test; $a$b $c")
test_cmd("02", "variable=aaaaaaaa; echo $var\iable")
test_cmd("02 quoted", "variable=aaaaaaaa; echo \"$var\iable\"")
test_cmd("03", "var1=test1; var2=test2; echo $var1$var2; echo $var1\$var2")
test_cmd("04", "var1=test1; var2=$var1; echo $var2")
test_cmd("05", "echo $a test")
test_cmd("05","param=val; temp=ue; echo '$val$temp")
test_cmd("recursion","temp=temporary; var1=test1$temp; var2=$var1; var3=$var2 ; echo var3")
test_cmd("06","var=print; echo \"$var\" test")
test_cmd("07","var=print; echo $var test")
test_cmd("single quote", "param=value; echo '$param'")
test_cmd("back_slash", "param=value; echo \$param")
test_cmd("10", "echo $12aa")
test_cmd("10", "echo $1 $?")
test_cmd("10", "a=eeeee; echo $a\$test")
test_cmd("04","param=value; echo $param; echo ${param}")

if __name__ == "__main__":
    from tester import summary
    summary()