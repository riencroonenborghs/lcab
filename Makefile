BUILD_DIR = build
SRCS = $(wildcard src/*.c)
TEST_SRCS = $(wildcard tests/*.c)
CMAKELISTS = CMakeLists.txt src/CMakeLists.txt tests/CMakeLists.txt

.PHONY: clean watch

default: test

$(BUILD_DIR)/rules.ninja: $(BUILD_DIR) $(SRCS) $(TEST_SRCS) $(CMAKELISTS)
	cmake -H. -B$(BUILD_DIR) -GNinja

$(BUILD_DIR)/src/lcab: $(BUILD_DIR)/rules.ninja $(SRCS)
	cmake --build $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) Testing

ninja: $(BUILD_DIR)/src/lcab

test: $(BUILD_DIR)/src/lcab
	-cmake --build build --target test
	cat $(BUILD_DIR)/tests/*.tap

$(BUILD_DIR):
	mkdir $@
