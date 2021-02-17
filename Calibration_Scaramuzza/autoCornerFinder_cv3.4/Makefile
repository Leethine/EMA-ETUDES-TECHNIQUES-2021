CV_INC_PATH=/home/lizian/.local/usr/opencv3.4.12/usr/local/include
CV_LINK_PATH=/home/lizian/.local/usr/opencv3.4.12/usr/local/lib64

DEFINCS = -I/usr/include -I$(CV_INC_PATH)
DEFLIBS = -L$(CV_LINK_PATH) -L/usr/lib64

LINKLIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio

CPP  = g++
CC   = gcc
OBJ  = main.o cvcalibinit3.o 
LINKOBJ  = main.o cvcalibinit3.o 
BIN  = FindCorners
RM = rm -f


all: $(BIN)

clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
#	$(CPP) $(LINKOBJ) -g -o $(BIN) $(LIBS) $(DEFLIBS) $(LINKLIBS)
	$(CPP) $(LINKOBJ) -g -o $(BIN) $(DEFLIBS) $(LINKLIBS)
main.o: main.cpp
#	$(CPP) -c -g main.cpp -o main.o $(INCS) $(DEFINCS)
	$(CPP) -c -g main.cpp -o main.o $(DEFINCS)
cvcalibinit3.o: cvcalibinit3.cpp
#	$(CPP) -c -g cvcalibinit3.cpp -o cvcalibinit3.o  $(INCS) $(DEFINCS)
	$(CPP) -c -g cvcalibinit3.cpp -o cvcalibinit3.o $(DEFINCS)
