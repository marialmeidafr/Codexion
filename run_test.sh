#!/bin/bash
for i in $(seq 1 10)
do
    echo "=== Tentativa $i ==="
    ./codexion 5 3000 200 200 200 10 400 fifo > out.log 2> debug.log
    if grep -q "burned out" out.log; then
        echo "Reproduziu na tentativa $i!"
        break
    fi
done
