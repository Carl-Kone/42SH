from tester import test_cmd

# Is Valid
test_cmd("comment simple 2", "echo qq  #test")
test_cmd("comment simple 3", "echo #test")
test_cmd("comment simple", "#echo test")
test_cmd("comment hard_test", "echo \#escaped '#'quoted not#first #commented")

# Is Not Valid

if __name__ == "__main__":
    from tester import summary
    summary()
