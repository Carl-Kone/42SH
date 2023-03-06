from tester import test_cmd

test_cmd("usual export", "export a=b; echo $a")
test_cmd("export no =", "export a; echo $a")
test_cmd("export multiple args", "export theo=ouais ouais; echo $theo")
test_cmd("export recursive", "export a=b; export c=$a; echo $c")
test_cmd("export space", "export a = b; echo $a")
test_cmd("export tricky", "export a=b =; echo $a")
test_cmd("export weird", "export a==b; echo $a")
test_cmd("export multiple", "export a=omg b=so c=many d=variables; echo $a $b $c $d")
test_cmd("export multiple and fail", "export a=b = = c=d; echo $? $a $c")

if __name__ == "__main__":
    from tester import summary
    summary()
