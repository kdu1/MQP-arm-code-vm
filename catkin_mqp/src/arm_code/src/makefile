all: arm_code libs 


libs: libhidapi-hidraw.so

CC       ?= gcc
CFLAGS   ?= -Wall -g -fpic

LDFLAGS  ?= -Wall -g

COBJS     = ../hidapi/linux/hid.o
OBJS      = $(COBJS)
LIBS_UDEV = `pkg-config libudev --libs` -lrt
LIBS      = $(LIBS_UDEV)
INCLUDES ?= -I../hidapi/hidapi `pkg-config libusb-1.0 --cflags`




arm_code: Robot.o SimpleComsDevice.o FloatPacketType.o ik3001.o cubic_traj.o rt_nonfinite.o $(COBJS)
#	gcc -g -Wall Robot.o SimpleComsDevice.o FloatPacketType.o ik3001.o cubic_traj.o rt_nonfinite.o -lstdc++ -lm -o arm_code 
	$(CC) $(LDFLAGS) $^ $(LIBS_UDEV) -lstdc++ -lpthread -lm -o $@


# Console Test Program
#hidtest-hidraw: $(COBJS)
#	$(CC) $(LDFLAGS) $^ $(LIBS_UDEV) -o $@

# Shared Libs
libhidapi-hidraw.so: $(COBJS)
	$(CC) $(LDFLAGS) $(LIBS_UDEV) -shared -fpic -Wl,-soname,$@.0 $^ -o $@

# Objects
$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@
#hid.o: ../hidapi/hidapi/hidapi.h ../hidapi/linux/hid.c
#	gcc -g -Wall -c $(INCLUDES) ../hidapi/linux/hid.c


FloatPacketType.o: FloatPacketType.h FloatPacketType.cpp
	gcc -g -Wall -c FloatPacketType.cpp

ik3001.o: ik3001.h rt_defines.h rt_nonfinite.h ik3001.cpp
	gcc -g -Wall -c ik3001.cpp

cubic_traj.o: cubic_traj.h rt_nonfinite.h cubic_traj.cpp
	gcc -g -Wall -c cubic_traj.cpp

Robot.o: Robot.h ik3001.h cubic_traj.h SimpleComsDevice.h Robot.cpp
	gcc -g -Wall -c Robot.cpp

SimpleComsDevice.o: SimpleComsDevice.h FloatPacketType.h ../hidapi/hidapi/hidapi.h SimpleComsDevice.cpp
	gcc -g -Wall -c SimpleComsDevice.cpp

rt_nonfinite.o: rt_nonfinite.h rt_nonfinite.cpp
	gcc -g -Wall -c rt_nonfinite.cpp

clean:
	rm -f Robot.o SimpleComsDevice.o FloatPacketType.o ik3001.o cubic_traj.o rt_nonfinite.o $(COBJS) arm_code

all: arm_code