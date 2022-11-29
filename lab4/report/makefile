all:
	pandoc \
	-tpdf \
	--pdf-engine=xelatex \
	--defaults='build_config.yaml' \
	--metadata date="`date +%d/%m/%Y`" \
	-o output.pdf \

latex:
	pandoc \
	-tlatex \
	--defaults='build_config.yaml' \
	--metadata date="`date +%d/%m/%Y`" \
	-s \
	-o output.tex \
