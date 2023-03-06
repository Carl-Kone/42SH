from tester import test_cmd

test_cmd("faulty loop","for i in 1 2 3 4 5; do if $i == 2; then echo the number is two; fi done")
test_cmd("for_loop simple", "for i in 1 2 3 4 5; do echo $i; done")
test_cmd("for loop nothing", "for word; do echo $word; done")
test_cmd("for loop newlines", "for w in one \n two \n \n \n three \n how; do echo $w; done")
