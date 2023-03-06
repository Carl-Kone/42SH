from tester import test_cmd

test_cmd("test_pdf", "echo \#escaped \"#\"quoted not#first #commented")
test_cmd("1_double_quote", "\"")
test_cmd("2_double_quotes", "\"\"")
test_cmd("unterminated quote", "echo \"the path is clear")
test_cmd("empty", "")
test_cmd("multiple quotes", "echo \"1\" \"2\" \"3\" \"4\" \"5\" \"6\" \"7\" \"8\"")
test_cmd("quote keyword", "if true; \"then\" true; else false; fi")
test_cmd("empty quote before keyword", "if true; \"\" then true; else false; fi")
test_cmd("quote command", "\"ls\"")
test_cmd("test_backslash","echo \"'\"")

if __name__ == "__main__":
    from tester import summary
    summary()
