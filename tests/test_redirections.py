from tester import test_cmd

# DO NOT WRITE THESE TESTS WITH FILES THAT YOU DON'T WANT TO LOOSE !!! CAUSE YOU WILL LOOSE THEM :(.
test_cmd("output redirection", "echo should not be printed > file.txt",["file.txt"])
test_cmd("output append redirection", "echo should not be printed >> file.txt",["file.txt"])
test_cmd("clobber redirection", "echo should not be printed >| file.txt",["file.txt"])
test_cmd("intput_output redirection", "echo should be printed <> file.txt",["file.txt"])
test_cmd("multiple output redirection", "echo should not be printed > file.txt > file1.txt",["file.txt","file1.txt"])
test_cmd("output redirection then append redirection", "echo should not be printed > file.txt >> file1.txt",["file.txt","file1.txt"])
test_cmd("multiple append redirection", "echo should not be printed >> file.txt >> file1.txt",["file.txt", "file1.txt"])
test_cmd("output then append then clobber redirection", "echo should not be printed > file.txt >> file1.txt >| file2.txt",["file.txt","file1.txt","file2.txt"])
test_cmd("output then append then append redirection", "echo should not be printed > file.txt >> file1.txt >> file2.txt",["file.txt","file1.txt","file2.txt"])
test_cmd("duplicating output redirection", "ls hjfbehkv src/ > file.txt 2>&1",["file.txt"])



# Not Valid
#test_cmd("input redirection", "cat < src/main/utils.c",["src/main/utils.c"])
#test_cmd("duplicating input redirection", "2<&1")
#test_cmd("io_number output redirection", "notACommand 2> file.txt",["file.txt"])
if __name__ == "__main__":
    from tester import summary
    summary()
