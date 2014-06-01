all: stelnet

LIBTERMCAP = -lncurses

#CXXFLAGS:=$(patsubst -O2, -g, $(CXXFLAGS))

# -DAUTHENTICATE
CXXFLAGS += -DUSE_TERMIO -DKLUDGELINEMODE
LIBS += $(LIBTERMCAP)

SRCS = commands.cc main.cc network.cc ring.cc sys_bsd.cc telnet.cc \
	sidekick.cc \
	terminal.cc tn3270.cc utilities.cc genget.cc environ.cc netlink.cc

OBJS = $(patsubst %.cc, %.o, $(SRCS))

stelnet: $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

depend:
	$(CXX) $(CXXFLAGS) -MM $(SRCS) >depend.mk

clean:
	rm -f *.o stelnet

