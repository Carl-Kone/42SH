for i in 1 2 3 4 continue 5 6; do
    $i 2> /dev/null
    echo $i
done
