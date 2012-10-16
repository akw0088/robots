
CC = g++
CFLAGS =  -DaUNIX \
	 -I./src \
	 -I./acroname_arm/aInclude \
	 -I./acroname_arm/aSource \
	 -I./acroname_arm/aSystem
LFLAGS = -Wl,-rpath,.
GARCIA_SRC = ./acroname_arm/aSource
SRC = ./src
BIN = ./bin
LIBS = -lpthread -lcrypt -lssl\
       -L./acroname_arm/aBinary -l aIO -l aMath -l aStem -l aUtil

OBJ = obj

.PHONY : app
app : $(OBJ) $(BIN)/i2c_compass $(BIN)/iwlist_fake $(BIN)/iwlist_drive $(BIN)/iwlist_laser $(BIN)/iwlist_listen $(BIN)/iwlist_match $(BIN)/iwlist_meet $(BIN)/laser_parse $(BIN)/create_database $(BIN)/subdivide_database $(BIN)/create_graph $(BIN)/create_heatmap

$(OBJ) : 
	mkdir $(OBJ)

$(BIN)/iwlist_drive : $(OBJ)/iwlist_drive.o $(OBJ)/graph.o $(OBJ)/heap.o $(OBJ)/iwlist_common.o $(OBJ)/acpGarcia.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/iwlist_laser : $(OBJ)/iwlist_laser.o $(OBJ)/iwlist_common.o $(OBJ)/acpLaser.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/iwlist_listen : $(OBJ)/iwlist_listen.o $(OBJ)/iwlist_common.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/iwlist_match : $(OBJ)/iwlist_match.o $(OBJ)/iwlist_common.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/iwlist_meet : $(OBJ)/iwlist_meet.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/laser_parse : $(OBJ)/laser_parse.o $(OBJ)/iwlist_common.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/create_database : $(OBJ)/create_database.o $(OBJ)/iwlist_common.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/subdivide_database : $(OBJ)/subdivide_database.o $(OBJ)/iwlist_common.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/create_graph : $(OBJ)/create_graph.o $(OBJ)/iwlist_common.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/create_heatmap : $(OBJ)/create_heatmap.o $(OBJ)/iwlist_common.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/iwlist_fake : $(OBJ)/iwlist_fake.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@
$(BIN)/i2c_compass : $(OBJ)/i2c_compass.o
	$(CC) -g $(CFLAGS) $(LFLAGS) $^ $(LIBS) -o $@


$(OBJ)/create_database.o : $(SRC)/create_database.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/create_graph.o : $(SRC)/create_graph.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/create_heatmap.o : $(SRC)/create_heatmap.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/graph.o : $(SRC)/graph.cpp
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/heap.o : $(SRC)/heap.cpp
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/iwlist_common.o : $(SRC)/iwlist_common.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/iwlist_drive.o : $(SRC)/iwlist_drive.cpp 
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/iwlist_laser.o : $(SRC)/iwlist_laser.cpp 
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/iwlist_listen.o : $(SRC)/iwlist_listen.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/iwlist_match.o : $(SRC)/iwlist_match.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/iwlist_meet.o : $(SRC)/iwlist_meet.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/laser_parse.o : $(SRC)/laser_parse.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/subdivide_database.o : $(SRC)/subdivide_database.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/iwlist_fake.o : $(SRC)/iwlist_fake.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/i2c_compass.o : $(SRC)/i2c_compass.c
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/acpGarcia.o : $(GARCIA_SRC)/acpGarcia.cpp $(GARCIA_SRC)/acpGarcia.h
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJ)/acpLaser.o : $(GARCIA_SRC)/acpLaser.cpp $(GARCIA_SRC)/acpLaser.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY : clean
clean :
	rm -rf $(OBJ)
