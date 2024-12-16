
# png = assets/uncompressed.png
# png = assets/my_zlibFixed.png
# png = assets/PNG_transparency_demonstration.png
# png = assets/rgb8.png
# png = assets/blog-90-funny-jokes-to-share-with-coworkers.png
# png = assets/spiderman.png
# png = assets/attesa-prelievo.png
png = assets/my48x32.png

cfiles = \
	main.c \
	\
	gunc/log.c \
	gunc/files.c \
	gunc/bitStream.c \
	gunc/ascii.c \
	gunc/byteUence64.c \
	gunc/byteBalloon64.c \
	gunc/byteArr64.c \
	\
	pork/createPixie.c \
	\
	whine/stripng.c \
	whine/reads.c \
	whine/chunk.c \
	whine/thicken.c \
	whine/imHeader.c \
	whine/pixie.c \
	whine/easel.c \
	whine/canvas.c \
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

links = \
	-lm \
	-lSDL2 \

flags = \
	-std=c99 \
	-Wpedantic \
	-Wall \
	-Wextra \

src = src/
out = a.out
log = proglog.txt

args =


d:
	cd $(src) && \
	gcc $(flags) \
		-o ../$(out) \
		-iquote . \
		\
		$(args) \
		\
		$(cfiles) \
		$(links) \

debug:
	cd $(src) && \
	gcc $(flags) \
		-o ../$(out) \
		-iquote . \
		\
		-DDEBUG \
		$(args) \
		\
		$(cfiles) \
		$(links) \

run: a.out
	./$(out) $(png)
runlog: a.out
	./$(out) $(png) 2> $(log)
clean:
	rm $(out)
	rm $(log)
