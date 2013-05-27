flags = -O0 -g -Wall -Werror -std=c99

bm : test_bloom_filter.c HashFunc.h HashFunc.c test_hash.c
	gcc -o test_hash test_hash.c HashFunc.c ${flags}
	gcc -o test_bm test_bloom_filter.c HashFunc.c ${flags} -lm

kmp : search_test.c Search.c Search.h
	gcc -o search_test search_test.c Search.c ${flags}

rotate : rotate_string.c
	gcc -o rotate rotate_string.c ${flags}

diff : find_diff_number.c
	gcc -o find_diff find_diff_number.c ${flags}
