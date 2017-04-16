NAME=proj4

all: $(NAME).tex
	latex $(NAME).tex
	bibtex $(NAME).aux
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

clean:
	rm $(NAME).aux $(NAME).log $(NAME).pdf $(NAME).dvi $(NAME).ps $(NAME).bbl $(NAME).blg

zip:
	zip xmarus06-fit.zip Makefile $(NAME).tex bibliografia.bib czplain.bst
