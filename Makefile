all: cachetest16 cachetest64 cachetest1024 cachetest4096 cachetest16384

clean:
	rm cachetest16 cachetest64 cachetest1024 cachetest4096 cachetest16384

cachetest16: cachetest.c
	gcc -Wall -march=native -std=gnu99 -O3 -DBLOCK_EXP=0 -o cachetest16 cachetest.c

cachetest64: cachetest.c
	gcc -Wall -march=native -std=gnu99 -O3 -DBLOCK_EXP=2 -o cachetest64 cachetest.c

cachetest1024: cachetest.c
	gcc -Wall -march=native -std=gnu99 -O3 -DBLOCK_EXP=6 -o cachetest1024 cachetest.c

cachetest4096: cachetest.c
	gcc -Wall -march=native -std=gnu99 -O3 -DBLOCK_EXP=8 -o cachetest4096 cachetest.c

cachetest16384: cachetest.c
	gcc -Wall -march=native -std=gnu99 -O3 -DBLOCK_EXP=10 -o cachetest16384 cachetest.c
