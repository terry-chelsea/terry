bm : test_bloom_filter.c HashFunc.h HashFunc.c test_hash.c
	gcc -o test_hash test_hash.c HashFunc.c -g
	gcc -o test_bm test_bloom_filter.c HashFunc.c -g -lm

kmp : kmp_search.c
	gcc -o kmp_search kmp_search.c -g  -DKMP
