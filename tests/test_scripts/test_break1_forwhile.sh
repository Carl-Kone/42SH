for i in should show this one too break not this though; do
    while true; do
        echo curr arg in WHILE is $i
        $i 2> /dev/null
        echo breaking!
        break
    done
    echo curr arg in FOR is $i
done
