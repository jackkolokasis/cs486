TARGET=hw2_$(USER)
TEX=pdflatex
TEXFILES=$(shell ls *.tex)
CARVGIT=carvgit

.PHONY: all view commit update status lastdiff2 lastdiff aspell clean 
		distclean

all: $(TARGET).pdf

$(TARGET).pdf: $(TEXFILES)
	$(TEX) -synctex=1 $(TARGET)
	$(TEX) $(TARGET)
	$(TEX) $(TARGET)

view:
	@(evince $(TARGET).pdf 2> /dev/null & echo $$! > evince.pid)

commit: 
	-git status
ifeq ($(m),)
	-git commit -a -m "Commit all changes"
else
	git commit -a -m "$m"
endif
	git push

up: update

update:
	-git status
	git pull

status:
	git status

lastdiff2:
	-git diff --name-status HEAD HEAD~2

lastdiff:
	-git diff --name-status HEAD HEAD~1

aspell:
	aspell --lang=en --mode=tex check $f

cc: clean commit

clean:
	\rm -f *.aux *.log *.toc *.bbl *.blg *.lof evince.pid

distclean:
	rm -rf $(TARGET).pdf
