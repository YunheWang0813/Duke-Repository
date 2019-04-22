#! /bin/bash

HOST=vcm-7703
NUM=1000

for ((i = 0; i < $NUM; i++))
do
	./client4 $HOST &
done

#wait
