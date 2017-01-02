
TARGET=hello
SOURCE=$(TARGET).c
CC=../buildroot/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-gcc

$(TARGET): $(SOURCE)
	@echo "making $(SOURCE)"
	$(CC) -o $(TARGET) $(SOURCE)

install: $(TARGET)
	sudo mount /dev/sdb2 /mnt/sd
	sudo cp $(TARGET) /mnt/sd/home/olivier/
	sync
	sudo umount /dev/sdb2

clean:
	-rm $(TARGET)