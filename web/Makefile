ACCOUNT=werner@almesberger.net
DIR=/home/httpd/almesberger/misc/idbg/

FILES=index.html idbg-coexistence.jpg idbg-connected.jpg idbg-ipl.jpg \
      idbg-soldered.jpg idbg-top.jpg idbg-wired.jpg idbg-wires.jpg

.PHONY:		all upload

all:
		@echo "make what ?" 2>&1
		@exit 1

upload:
		rsync -av -e ssh $(FILES) $(ACCOUNT):$(DIR)
