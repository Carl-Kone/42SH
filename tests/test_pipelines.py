from tester import test_cmd

#Valid
test_cmd("pipeline 1", "echo a | echo b")
test_cmd("pipeline 2", "echo a | \n\necho b")
test_cmd("pipeline 3", "ls -l | grep \"^d\"")
test_cmd("pipeline 4", "echo \"Hello World\" | rev")
test_cmd("pipeline 3 pipelines bis", "cat file.txt | tr \"a-z\" \"A-Z\" | tee newfile.txt")

# Not Valid
test_cmd("pipeline newline wrong place", "echo a | echo b\n \n\n | echo c")

if __name__ == "__main__":
    from tester import summary
    summary()
