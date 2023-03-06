from tester import test_cmd

test_cmd("test for + echo + redir + pipe", "for i in 1 2 3; do echo printing >&2; echo to grep; sleep 1; done | grep to;")
test_cmd("test command block + exit", "{ echo a; exit 2; echo b; }")

test_cmd("test function + if + echo -ne", "foo () { if false; then exit 2; else echo -ne //////hello////; fi  }")

if __name__ == "__main__":
    from tester import summary
    summary()
