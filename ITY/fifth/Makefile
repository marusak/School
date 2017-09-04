NAME=proj5

all: $(NAME).tex
	latex $(NAME).tex
	pdflatex $(NAME).tex

clean:
	rm $(NAME).aux $(NAME).log $(NAME).pdf $(NAME).dvi $(NAME).nav $(NAME).out $(NAME).snm $(NAME).toc $(NAME).vrb

zip:
	zip xmarus06-fit.zip Makefile $(NAME).tex
