from tester import test_cmd

test_cmd("simple cd", "cd src; ls;echo $PWD; echo $OLDPWD")
test_cmd("tricky cd back", "cd src/; cd; cd -;echo $PWD; echo $OLDPWD")
test_cmd("cd not existant", "cd .; cd doesntexist/; ls;echo $PWD; echo $OLDPWD")
test_cmd("cd file", "cd .; cd 42sh; ls;echo $PWD; echo $OLDPWD")
test_cmd("cd into cd", "cd tests; cd test_scripts; ls;echo $PWD; echo $OLDPWD")
test_cmd("cd multiple args", "cd .; cd src/ lexer/; ls; echo $PWD; echo $OLDPWD")
test_cmd("cd no arg", "cd; ls")
test_cmd("cd relative parent", "cd ../; ls; echo $PWD; echo $OLDPWD")
test_cmd("cd relative parent 2", "cd ../..; ls; echo $PWD; echo $OLDPWD")
test_cmd("cd root", "cd /; ls; echo $PWD; echo $OLDPWD")
test_cmd("cd multiple", "cd tests/test_scripts/; ls; echo $PWD; echo $OLDPWD")
test_cmd("cd home", "cd ~; ls; echo $PWD; echo $OLDPWD")
test_cmd("cd dir space", "cd 'tests/dir space; ls; echo $PWD; echo $OLDPWD")
test_cmd("cd .", "cd .; echo $PWD; echo $OLDPWD")
test_cmd("cd canon", "cd .;cd src/interpreter/../lexer/../builtins; ls; echo $PWD; echo $OLDPWD")


if __name__ == "__main__":
    from tester import summary
    summary()
