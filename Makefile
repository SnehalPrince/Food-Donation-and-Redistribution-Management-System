CXX ?= g++
STD ?= c++11
CXXFLAGS ?= -std=$(STD) -Wall -Wextra -pedantic
BUILD_DIR ?= build
OUT ?= foodbank

ifeq ($(OS),Windows_NT)
  EXE = .exe
  MKDIR = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
  RMDIR = if exist "$(subst /,\,$(1))" rmdir /s /q "$(subst /,\,$(1))"
  RM = if exist "$(subst /,\,$(1))" del /f /q "$(subst /,\,$(1))"
else
  EXE =
  MKDIR = mkdir -p "$(1)"
  RMDIR = rm -rf "$(1)"
  RM = rm -f "$(1)"
endif

SRCS = Date.cpp InputHelper.cpp Person.cpp Donor.cpp Recipient.cpp \
       FoodItem.cpp CookedFood.cpp PackagedFood.cpp Donation.cpp \
       RecipientRequest.cpp Delivery.cpp FoodBank.cpp ReportGenerator.cpp \
       FileManager.cpp main.cpp

OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

HEADERS = Date.h InputHelper.h Person.h Donor.h Recipient.h \
          FoodItem.h CookedFood.h PackagedFood.h Donation.h \
          RecipientRequest.h Delivery.h FoodBank.h ReportGenerator.h \
          FileManager.h

.PHONY: all clean test test-people test-food test-requests test-files test-foodbank test-reports demo check-std asan check-headers

all: $(BUILD_DIR)/$(OUT)$(EXE)

$(BUILD_DIR):
	@$(call MKDIR,$(BUILD_DIR))

$(BUILD_DIR)/%.o: %.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/$(OUT)$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

check-headers: | $(BUILD_DIR)
	@echo Checking that all headers compile independently...
	@for %%H in ($(HEADERS)) do @( \
		$(CXX) $(CXXFLAGS) -x c++ -c %%H -o $(BUILD_DIR)\temp_header.o || exit /b 1 \
	)
	@echo All headers compile independently with zero warnings!

# Module test rules
TEST_PEOPLE_OBJS = $(BUILD_DIR)/Person.o $(BUILD_DIR)/Donor.o $(BUILD_DIR)/Recipient.o
test-people: $(BUILD_DIR) $(TEST_PEOPLE_OBJS)
	$(CXX) $(CXXFLAGS) tests/test_people.cpp $(TEST_PEOPLE_OBJS) -o $(BUILD_DIR)/test_people$(EXE)
	$(BUILD_DIR)/test_people$(EXE)

TEST_FOOD_OBJS = $(BUILD_DIR)/Date.o $(BUILD_DIR)/FoodItem.o $(BUILD_DIR)/CookedFood.o $(BUILD_DIR)/PackagedFood.o $(BUILD_DIR)/Donation.o
test-food: $(BUILD_DIR) $(TEST_FOOD_OBJS)
	$(CXX) $(CXXFLAGS) tests/test_food.cpp $(TEST_FOOD_OBJS) -o $(BUILD_DIR)/test_food$(EXE)
	$(BUILD_DIR)/test_food$(EXE)

TEST_REQ_OBJS = $(BUILD_DIR)/Date.o $(BUILD_DIR)/RecipientRequest.o $(BUILD_DIR)/Delivery.o
test-requests: $(BUILD_DIR) $(TEST_REQ_OBJS)
	$(CXX) $(CXXFLAGS) tests/test_requests.cpp $(TEST_REQ_OBJS) -o $(BUILD_DIR)/test_requests$(EXE)
	$(BUILD_DIR)/test_requests$(EXE)

TEST_FILES_OBJS = $(filter-out $(BUILD_DIR)/main.o $(BUILD_DIR)/InputHelper.o,$(OBJS))
test-files: $(BUILD_DIR) $(TEST_FILES_OBJS)
	$(CXX) $(CXXFLAGS) tests/test_files.cpp $(TEST_FILES_OBJS) -o $(BUILD_DIR)/test_files$(EXE)
	$(BUILD_DIR)/test_files$(EXE)

TEST_FB_OBJS = $(filter-out $(BUILD_DIR)/main.o $(BUILD_DIR)/InputHelper.o,$(OBJS))
test-foodbank: $(BUILD_DIR) $(TEST_FB_OBJS)
	$(CXX) $(CXXFLAGS) tests/test_foodbank.cpp $(TEST_FB_OBJS) -o $(BUILD_DIR)/test_foodbank$(EXE)
	$(BUILD_DIR)/test_foodbank$(EXE)

TEST_REP_OBJS = $(filter-out $(BUILD_DIR)/main.o $(BUILD_DIR)/InputHelper.o,$(OBJS))
test-reports: $(BUILD_DIR) $(TEST_REP_OBJS)
	$(CXX) $(CXXFLAGS) tests/test_reports.cpp $(TEST_REP_OBJS) -o $(BUILD_DIR)/test_reports$(EXE)
	$(BUILD_DIR)/test_reports$(EXE)

test: test-people test-food test-requests test-files test-foodbank test-reports
	@echo ========================================
	@echo ALL UNIT AND MODULE TESTS PASSED
	@echo ========================================

check-std:
	@echo Checking C++11 compliance...
	$(MAKE) clean
	$(MAKE) STD=c++11 all test
	@echo Checking C++17 compliance...
	$(MAKE) clean
	$(MAKE) STD=c++17 all test
	@echo Zero warnings under both -std=c++11 and -std=c++17!

demo: all
	@echo Running Viva Demo Scenario (Plan.md section 18)...
	$(BUILD_DIR)/$(OUT)$(EXE) --today 2026-10-05 --data-dir tests/temp_demo_data < tests/demo_input.txt

asan:
	@echo Building with Address and Undefined sanitizers...
	$(CXX) -std=$(STD) -fsanitize=address,undefined -Wall -Wextra -pedantic $(SRCS) -o $(BUILD_DIR)/$(OUT)_asan$(EXE)

clean:
	@$(call RMDIR,$(BUILD_DIR))
	@$(call RMDIR,tests/temp_demo_data)
	@$(call RM,$(OUT)$(EXE))
