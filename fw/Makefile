DIRS=boot idbg


.PHONY:		all depend clean spotless

all:
		for d in $(DIRS); do make -C $$d all || exit 1; done

depend:
		for d in $(DIRS); do make -C $$d depend || exit 1; done

clean:
		for d in $(DIRS); do make -C $$d clean || exit 1; done

spotless:
		for d in $(DIRS); do make -C $$d spotless || exit 1; done
