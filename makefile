
# png = assets/uncompressed.png
# png = assets/my_zlibFixed.png
# png = assets/PNG_transparency_demonstration.png
png = assets/rgb8.png

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
	zoop/walkUntilLeaf.c \
	zoop/lengthDist.c \
	\
	shrub/tree.c \
	shrub/fixedTree.c \
	shrub/metaTree.c \
	shrub/dynaTree.c \
	shrub/distTree.c \
	\
	sdaubler/convert.c \
	sdaubler/display.c \

links = \
	-lm \
	-lSDL2 \

args =


d:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		\
		$(cfiles) \
		$(links) \
		$(args) \

debug:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		-DDEBUG \
		\
		$(cfiles) \
		$(links) \
		$(args) \

run: a.out
	./a.out $(png)
runlog: a.out
	./a.out $(png) 2> proglog.txt
clean:
	rm a.out
