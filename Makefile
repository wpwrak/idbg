ACCOUNT=werner@almesberger.net
DIR=/home/httpd/almesberger/misc/idbg.git/

.PHONY:		all push

all:
		@echo "make what ?" 2>&1
		@exit 1

push:
		git-update-server-info
		rsync -av -e ssh .git/ $(ACCOUNT):$(DIR)
