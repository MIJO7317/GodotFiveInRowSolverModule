#ifndef FIVEINROWSOLVER_H
#define FIVEINROWSOLVER_H

#include "core/reference.h"
#include "CycleMatrix.h"
#include <string>

enum class PlayerType {
	NONE,
	HUMAN,
	AI
};

class FiveInRowSolver : public Reference {
	GDCLASS(TicTacToeSolver, Reference);

protected:
	static void _bind_methods();

private:
	int GetWeight(std::vector<PlayerType> block);

public:
	void _init();

	void SetBoardSize(size_t size);

	void FillBoard(String player_type);

	String GetElementBoard(size_t row, size_t column) const;

	void SetElementBoard(size_t row, size_t column, String player_type);

	String WhoWin(size_t block_size);

	Vector2 NextAIMove(bool hard);

private:
	CycleMatrix<PlayerType> board;
	
};

#endif
