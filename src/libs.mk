# Any copyright of this file is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# Defines rules for rebuilding the YYY libs.

# yyymonitor build
MONDIR=monitor
MONLIB=$(MONDIR)\lib\$(PX)yyymonitor.$(SX)

# yyynetwork build
NETDIR=network
NETLIB=$(NETDIR)\lib\$(PX)yyynetwork.$(SX)

# yyyirc build
IRCDIR=irc
IRCLIB=$(IRCDIR)\lib\$(PX)yyyirc.$(SX)

# yyychat build
CHATDIR=chat
CHATLIB=$(CHATDIR)\lib\$(PX)yyychat.$(SX)

# yyythread build
THRDIR=thread
THRLIB=$(THRDIR)\lib\$(PX)yyythread.$(SX)

# Kind of a brute force solution, but it works in wmake, bmake, and gmake.
$(MONLIB):
	cd $(MONDIR) && $(MAKE) -f makefile.$(MK)

$(NETLIB): $(NETDIR)/yyy_network.c $(NETDIR)/yyy_network.h $(NETDIR)/yyy_network_common.c $(NETDIR)/yyy_network_common.c
	cd $(NETDIR) && $(MAKE) -f makefile.$(MK)

$(IRCLIB):
	cd $(IRCDIR) && $(MAKE) -f makefile.$(MK)

$(CHATLIB):
	cd $(CHATDIR) && $(MAKE) -f makefile.$(MK)

$(THRLIB):
	cd $(THRDIR) && $(MAKE) -f makefile.$(MK)

libclean:
	cd $(MONDIR) && $(MAKE) -f makefile.$(MK) clean
	cd $(NETDIR) && $(MAKE) -f makefile.$(MK) clean
	cd $(IRCDIR) && $(MAKE) -f makefile.$(MK) clean
	cd $(CHATLIB) && $(MAKE) -f makefile.$(MK) clean
	cd $(THRDIR) && $(MAKE) -f makefile.$(MK) clean

YYYLIBS= $(MONLIB) $(NETLIB) $(IRCLIB) $(CHATLIB) $(THRLIB)
