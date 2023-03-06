from tester import test_cmd

test_cmd("test exit", "exit")
test_cmd("test exit with arg","exit 10")
test_cmd("test exit with multiple arg", "exit 10 15")
test_cmd("exit w/ prev return code", 'echo hi; exit')
test_cmd("arg too big", "exit 09873569487")
test_cmd("arg just big enough", "exit 255")
test_cmd("negative arg", "exit -1")
test_cmd("exit variable", "a = 69; exit $a")
test_cmd("exit if", "if false; then echo oops; else if false; then echo oops if false; then echo oops; else if false; then echo oops too; elif true; echo success!; exit; fi fitoo; elif true; echo success!; exit; fi fi")

if __name__ == "__main__":
    from tester import summary
    summary()
