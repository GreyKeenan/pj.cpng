
png = assets/uncompressed.png

cfiles = \
	main.c \
	pix_as_imt.c \
	\
	gunc/log.c \
	gunc/files.c \
	gunc/byteBalloon.c \
	gunc/sequence.c \
	gunc/bitStream.c \
	gunc/byteSeq64.c \
	gunc/ascii.c \
	\
	whine/stripng.c \
	whine/reads.c \
	whine/chunk.c \
	whine/nofilter.c \
	whine/image.c \
	whine/pixie.c \
	\
	zoop/decompress.c \
	zoop/alderman.c \
	zoop/deflate.c \
	\
	sdaubler/convert.c \
	sdaubler/display.c \


d:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		\
		$(cfiles) \
		-lSDL2 \

debug:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		-DDEBUG \
		\
		$(cfiles) \
		-lSDL2 \

run: a.out
	./a.out $(png)
runlog: a.out
	./a.out $(png) 2> proglog.txt
clean:
	rm a.out
