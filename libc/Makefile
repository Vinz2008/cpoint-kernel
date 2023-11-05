CPOINT = cpoint

SRCS := $(wildcard *.cpoint)
OBJS = $(patsubst %.cpoint,%.o,$(SRCS))

all: libk.a

libk.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o: %.cpoint
	$(CPOINT) -o $@ $^ -c -no-std

clean:
	rm -f ./*.o ./*.a