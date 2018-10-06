CXX = g++
CXXFLAGS += -O2 -Wall -Werror -Wextra -std=c++14

BUILDDIR = ./build

SRCS = $(wildcard *.cpp)
EXES = $(patsubst %.cpp,%,$(SRCS))
DEPS = $(patsubst %.cpp,$(BUILDDIR)/%.d,$(SRCS))

.PHONY: all clean

all: $(EXES)

$(EXES) : % : %.cpp
	@mkdir -p $(BUILDDIR)/$(@D)
	$(CXX) $(CXXFLAGS) -MMD -MF $(BUILDDIR)/$(@D)/$(*F).d $< -o $@

clean:
	@$(RM) $(EXES) $(DEPS)
	@rm -rf $(BUILDDIR)/*

-include $(DEPS)
