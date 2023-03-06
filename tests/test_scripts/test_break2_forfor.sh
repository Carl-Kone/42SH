for i in 1 2 3 4 5; do
    echo this is $i
    for a in 5 6 7 break 9; do
        echo this is $a with $i
        $a 2> /dev/null
    done
done
