FILES := ../../diskio/disckiod.c ../../diskio/diskio.c ../record.c ../db_ops.c test.c ../cursor.c ../cursor_bt.c
OUT := test

all:
	gcc -O0 -g $(FILES) -o $(OUT)

clean:
	rm $(OUT) && rm yac.db
