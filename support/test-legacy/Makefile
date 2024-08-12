.PHONY: all clean \
	_meta \
	filesystem fstream

all: _meta filesystem fstream

_meta:
	$(MAKE) -C _meta

filesystem: _meta
	$(MAKE) -C filesystem
	$(MAKE) -C filesystem test

fstream:
	$(MAKE) -C fstream
	$(MAKE) -C fstream test

clean:
	$(MAKE) -C _meta clean
	$(MAKE) -C filesystem clean
	$(MAKE) -C fstream clean
