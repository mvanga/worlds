include config.mk
include env.mk

OUT=smmo

all: $(OUT)

$(OUT): $(obj-y)
	$(CC) $(CFLAGS) $(obj-y) -o $(OUT) $(LDFLAGS)

clean:
	rm -rf $(obj-y) $(OUT)
