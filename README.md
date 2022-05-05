# 15618-Final-Project

# Proposal
## Summary
This project is a chess engine. The main idea is to run minimax on the tree of possible moves in a game to select the best move for a side. To improve performance, the tree search will be parallelized using OpenMP.

## Background
Chess is a board game. A conservative lower-bound of the game-tree complexity for chess is 10<sup>120</sup>. Even with the elimination of terrible, game losing moves, the bound is around 10<sup>40</sup>. This is obviously too large to completely solve with current technology, so chess engines use evaluation functions that usually consider top-level human chess principles to evaluate a position some moves ahead of the current position. I say usually because some recent advancements in deep reinforcement learning have created engines that challenge these principles.

My aim with this project is not to create a better chess engine than Google, by myself in 6 weeks. Rather, I would like to create a competent engine using a simple minimax search on the game tree. On its own, this algorithm does not perform particularly well because all the possible moves in each position limit the depth of search. My hope is that parallelizing the tree search will allow it to reach sufficient depth to compete against somewhat competent chess players.

## Challenge
This is a difficult problem because the search space in the move tree for a chess game is massive (current "solved position" databases only have positions up to 8 remaining pieces). An efficient parallel search of the game tree should improve the performance of the engine in a time-constrained setting.

## Resources
I plan on using the Gates Gluster for development and evaluation.
https://github.com/billforsternz/thc-chess-library is a general purpose chess library that implements the basic rules of chess and provides an api for a game. This will allow me to focus on the searching and position evaluation aspects of the game without having to spend tons of time just implementing the rules of chess.

## Goals and Deliverables
75% target - An OpenMP chess engine that can beat people who are not chess players (<1000 elo on chess.com or so). This would happen if I am not able to iterate on my parallelization strategies for the engine to reach an acceptable tree depth in a reasonable amount of time (~5 seconds per move is my target).

100% target - An OpenMP chess engine that can beat an average chess player (~1400 elo). I think that this is a reasonable target strength for a simple minimax tree search. The state-of-the-art engines are much more complicated, incorporating deep RL models in balanced positions, optimized bit-level board representation, solved endgame database lookups, as well as already incorporating multicore computations. I think that this is a bit out of reach for a 5-6 week project.

125% target - An OpenMP chess engine that can beat an average chess player (~1400 elo), and a comparable MPI chess engine to compare against.

## Schedule
  Week 1 -
  Install and familiarize myself with the thc-chess-library, and be able to interface a game with an engine. Begin initial sequential minimax tree search.
  
  Week 2 -
  Finish sequential minimax tree search, probably add alpha/beta pruning. Begin parallelizing the implementation with OpenMP.
  
  Week 3 -
  Finish parallel implementation, and evaluate performance. Try to identify bottlenecks for further improvement.
  
  Week 4 -
  Implement a solution that address bottlenecks and evaluate performance.
  
  Week 5 -
  Time permitting, address bottlenecks from the previous solution, try a solution using MPI and compare to the OpenMP solution, prehaps incorporate endgame tablebase lookups for better endgame performance.

# Milestone Report
So far, I have implemented a sequential (not parallel) chess AI that does a minimax tree search with alpha-beta pruning. It uses a simple leaf evaluation function that only considers the number of pieces on the board. This AI can search at depth 5 with under 5 seconds per move and depth 6 with under 30 seconds per move. It plays relatively well in the middle and end game, but plays poorly in the opening because the search depth is too shallow for any material differences to be seen. I believe that I will be able to reach my 100% target from the project proposal, but probably not the 125% target. I would like to focus more on a good OpenMP parallelization than trying to additionally make an MPI implementation to compare against. For the poster session I plan to show graphs of the speedup from the parallelization, and possibly a demo where someone can play against the AI.

My remaining goals before the final presentation are -
* Improve the leaf evaluation function to incorporate piece mobility for better solution quality when pieces can not be easily taken. This is a simple count of the available moves the AI will have after playing a move. This is costly but can be parallelized.
* Parallelize the minimax algorithm using OpenMP, identify performance bottlenecks, and attempt to optimize.
* Create an evaluation environment for the AI. I'm thinking that I'll have a few different fixed board positions and measure the time taken to compute the next move at various depths. An interesting, but less qualitative measure, could be having the AI play against players of various skill levels and recording the results.


# Final Presentation Video
https://www.youtube.com/watch?v=QAK_cr8-kR4
