# Chess Engine with AlphaZero-inspired AI

This project is a chess engine developed from scratch as part of a research honours project and personal endeavor. It combines traditional chess programming techniques with modern machine learning approaches inspired by DeepMind's AlphaZero.

## Key Features

- Custom-built chess engine implemented in C++
- AI model trained using self-play, following the AlphaZero methodology
- Neural network implemented in Python using PyTorch
- Seamless integration of the trained model into the C++ engine

## Project Structure

The project consists of several key components:

- `src/chess.cpp`: Core chess logic and game state management
- `src/moveGenerator.cpp`: Legal move generation for all piece types
- `src/resNet.cpp`: Integration of the trained neural network model
- `src/board.cpp`: Board representation and FEN parsing

## AI Model

The AI model is based on a residual neural network (ResNet) architecture, trained using reinforcement learning through self-play. The model was initially developed and trained in Python using PyTorch, then exported and integrated into the C++ engine for efficient inference during gameplay.

## Future Work

This project is ongoing, with plans to further improve the AI's performance, optimize the engine's speed, and potentially add a user interface for human vs. AI play.

## Acknowledgements

This project draws inspiration from the groundbreaking work done by DeepMind on AlphaZero, adapting their approach to create a powerful chess AI from first principles.
