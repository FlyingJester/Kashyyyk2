
mkgendir:
	mkdir -p $(GENDIR)

mkobjdir:
	mkdir -p $(OBJDIR)

mklibdir:
	mkdir -p $(LIBDIR)

.PHONY: mkgendir mkobjdir clean
