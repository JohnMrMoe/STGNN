# This Exists Now


# Compiler
CXX := g++ --std=c++11
RUN := ./

NAME = stgnn
C_FLG = -I /home/johanom/eigen/
R_FLG =

FILES = main.cpp





cmp:
	$(CXX) $(FILES) $(C_FLG) -o $(NAME)

run:
	$(RUN)$(NAME)
