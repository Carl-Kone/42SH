from tester import test_cmd

# Is Valid

test_cmd("1_command", "echo lets gooooooooooooooooooooooooooo")
test_cmd("2_commands", "echo a; true")
test_cmd("3_commands", "echo a; echo b; echo c")
test_cmd("10_commands", "echo a; true; echo b; false; echo c; true; echo d; false; echo e; true;")
test_cmd("true a", "true a")
test_cmd("semi_col_in_arg", "echo 'semi;colon'")
test_cmd("no_spaces", "echo '42 sh';true")
test_cmd("spaces","        echo        '42 sh'             ;          false         ")

# Is Not Valid

test_cmd("2_semi_col", "echo a;; echo b")
test_cmd("only_semi_col", ";")

if __name__ == "__main__":
    from tester import summary
    summary()
