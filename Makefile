TARGET := comment-inserter
WARNINGS := -Wall -Wextra -pedantic -Wno-unused-parameter -fvisibility=hidden
CXX := clang++

CLANG_LIBS := \
	-lclangFrontendTool \
	-lclangRewriteFrontend \
	-lclangDynamicASTMatchers \
	-lclangTooling \
	-lclangFrontend \
	-lclangToolingCore \
	-lclangASTMatchers \
	-lclangParse \
	-lclangDriver \
	-lclangSerialization \
	-lclangRewrite \
	-lclangSema \
	-lclangEdit \
	-lclangAnalysis \
	-lclangAST \
	-lclangLex \
	-lclangBasic

LIBS := $(CLANG_LIBS) `llvm-config --cxxflags --ldflags --libs --system-libs`

all: comment-inserter

clean:
	rm $(TARGET) 

comment-inserter: $(TARGET).cpp
	$(CXX) $(WARNINGS) $(TARGET).cpp $(LIBS) -o $(TARGET)
