all:
	gcc portfolio.c rbtree.c -o portfolio -Wall
	./portfolio
full:
	wget https://datasets.imdbws.com/title.basics.tsv.gz
	gunzip -k title.basics.tsv.gz
	grep "movie" title.basics.tsv > movie_records
remove:
	-rm portfolio
	-rm movie_records
	-rm title.basics.tsv
	-rm title.basics.tsv.gz
