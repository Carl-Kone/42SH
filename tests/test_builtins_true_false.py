from tester import test_cmd

test_cmd("false", "false")
test_cmd("false args", "false test echo pain")
test_cmd("'false'", "'false'")
test_cmd("true", "true")
test_cmd("'true'", "'true'")
test_cmd("true args", "true test echo pain")
test_cmd("true false", "true false")
test_cmd("false true", "false true")
test_cmd("false newline", "false \n ")

if __name__ == "__main__":
    from tester import summary
    summary()
