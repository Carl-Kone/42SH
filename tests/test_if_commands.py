from tester import test_cmd

#Is Valid

test_cmd("good_if_1", "if true; then true; else false; fi")
test_cmd("good_if_2", "if false; then echo false; elif true; then echo true; else echo false; fi")
test_cmd("good_if_3_without_echo", "if false; then false; elif true; then true; else false; fi")
test_cmd("missing_fi", "if true; then true; else false;")
test_cmd("missing_semi_col", "if true then true; else false; fi")
test_cmd("good_if_4_pdf1", "if false; true; then \n echo a \n echo b; echo c; \n fi")
test_cmd("good_if_5_pdf2", "if false \n true \n then \n echo a \n echo b; echo c \n fi")
test_cmd("good_if_6_newline", "if true \n then \n echo a \n fi")
test_cmd("good_if_7_big_test", "if false; then echo false; elif true; then echo true; else echo false; fi")

#Is Not Valid

test_cmd("missing_fi", "if true; then true; else false;")
test_cmd("missing_semi_col", "if true then true; else false; fi")
test_cmd("missing_then", "if true true; else false; fi")
test_cmd("else then", "if true then true; else then false; fi")

if __name__ == "__main__":
    from tester import summary
    summary()
