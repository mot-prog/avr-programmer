CC = gcc
CFLAGS = -Wall -Wextra

LDFLAGS = -lusb-1.0

TARGETS = main_pc isp_ids flash_read flash_rw

all: $(TARGETS)

main_pc: main_pc.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

isp_ids: isp_ids.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGETS)


#TODO : fonction isp_ids.c