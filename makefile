CC = gcc
TARGET = watershed


$(TARGET): watershed.c 
	$(CC) -o watershed watershed.c pointcloud.c utils.c bmp.c -DUSEPNG `libpng-config --cflags` `libpng-config --ldflags` -lz -lm -g

partb: pointcloud.c 
	$(CC) -o partb pointcloud.c utils.c bmp.c -DUSEPNG `libpng-config --cflags` `libpng-config --ldflags` -lz -lm

parta: parta.c
	$(CC) -o parta parta.c 

clean:
	rm -rf $(TARGET) *.gif