#!/bin/sh

for i in $(seq 1 $1); do
	echo $i
	./spi_p $2 $3
	sleep 0.1
done

# EOF
