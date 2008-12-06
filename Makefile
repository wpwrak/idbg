ACCOUNT=werner@almesberger.net
DIR=/home/httpd/almesberger/misc/idbg.git/

.PHONY:		push


push:
		git-update-server-info
		rsync -av -e ssh .git/ $(ACCOUNT):$(DIR)
