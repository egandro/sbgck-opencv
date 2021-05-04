all:: build

build: svgassets

svgassets:
	mogrify -trim  -background transparent -path . -format png svg/*.svg
	rm -f ./*_all.png
