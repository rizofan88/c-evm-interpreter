CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

TARGET = build/evm

APP_SRC = src/main.c

CORE_SRC = src/utils.c \
           src/disasm.c \
           src/opcode.c \
           src/evm.c \
           src/stack.c \
           src/operations.c \
           src/math/add_sub.c \
           src/math/mul_div_mod.c \
           src/math/shift.c

SRC = $(APP_SRC) $(CORE_SRC)
OBJ = $(SRC:src/%.c=build/%.o)

CORE_OBJ = $(CORE_SRC:src/%.c=build/%.o)

UNIT_TEST_SRC = $(wildcard test/unit/test_*.c)
UNIT_TEST_TARGETS = $(patsubst test/unit/%.c,build/test/unit/%,$(UNIT_TEST_SRC))

INTEGRATION_TEST_SRC = $(wildcard test/integration/test_*.c)
INTEGRATION_TEST_TARGETS = $(patsubst test/integration/%.c,build/test/integration/%,$(INTEGRATION_TEST_SRC))

TEST_TARGETS = $(UNIT_TEST_TARGETS) $(INTEGRATION_TEST_TARGETS)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/test/unit/%: test/unit/%.c $(CORE_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Itest $< $(CORE_OBJ) -o $@

build/test/integration/%: test/integration/%.c $(CORE_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Itest $< $(CORE_OBJ) -o $@

clean:
	rm -rf build

demo: $(TARGET)
	./$(TARGET) demo

run:
	@echo "Usage:"
	@echo "  make disasm HEX=600a600506"
	@echo "  make exec HEX=600a600506"
	@echo "  make demo"
	@echo ""
	@echo "Tests:"
	@echo "  make test"
	@echo "  make test-unit"
	@echo "  make test-integration"

disasm: $(TARGET)
	./$(TARGET) disasm $(HEX)

exec: $(TARGET)
	./$(TARGET) run $(HEX)

test: $(TEST_TARGETS)
	@for test in $(TEST_TARGETS); do \
		printf "\nRunning %s\n\n" "$$test"; \
		./$$test || exit 1; \
	done

test-unit: $(UNIT_TEST_TARGETS)
	@for test in $(UNIT_TEST_TARGETS); do \
		printf "\nRunning %s\n\n" "$$test"; \
		./$$test || exit 1; \
	done

test-integration: $(INTEGRATION_TEST_TARGETS)
	@for test in $(INTEGRATION_TEST_TARGETS); do \
		printf "\nRunning %s\n\n" "$$test"; \
		./$$test || exit 1; \
	done

.PHONY: all clean run demo disasm exec test test-unit test-integration
