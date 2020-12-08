#include "FiveInRowSolver.h"

//Bind all your methods used in this class
void FiveInRowSolver::_bind_methods() {
	ClassDB::bind_method(D_METHOD("SetBoardSize", "size"), &FiveInRowSolver::SetBoardSize);
	ClassDB::bind_method(D_METHOD("FillBoard", "player_type"), &FiveInRowSolver::FillBoard);
	ClassDB::bind_method(D_METHOD("GetElementBoard", "row", "column"), &FiveInRowSolver::GetElementBoard);
	ClassDB::bind_method(D_METHOD("SetElementBoard", "row", "column", "player_type"), &FiveInRowSolver::SetElementBoard);
	ClassDB::bind_method(D_METHOD("WhoWin", "block_size"), &FiveInRowSolver::WhoWin);
	ClassDB::bind_method(D_METHOD("NextAIMove", "hard"), &FiveInRowSolver::NextAIMove);
}

int FiveInRowSolver::GetWeight(std::vector<PlayerType> block) {
	int empty = std::count(block.begin(), block.end(), PlayerType::NONE);
	int ai = std::count(block.begin(), block.end(), PlayerType::AI);
	int human = std::count(block.begin(), block.end(), PlayerType::HUMAN);
	if (!empty)
		return 0;
	if (
			ai == 3 && human == 1 ||
			ai == 2 && human == 1 ||
			ai == 2 && human == 2 ||
			ai == 1 && human == 1 ||
			ai == 1 && human == 2 ||
			ai == 1 && human == 3 ||
			ai == 0 && human == 0)
		return 1;
	if (ai == 0 && human == 1)
		return 2;
	if (ai == 1 && human == 0)
		return 3;
	if (ai == 0 && human == 2)
		return 8;
	if (ai == 2 && human == 0)
		return 10;
	if (ai == 0 && human == 3)
		return 80;
	if (ai == 3 && human == 0)
		return 90;
	if (ai == 0 && human == 4)
		return 800;
	if (ai == 4 && human == 0)
		return 900;
}

void FiveInRowSolver::_init() {
}

void FiveInRowSolver::SetBoardSize(size_t size) {
	this->board = CycleMatrix<PlayerType>{ size };
}

void FiveInRowSolver::FillBoard(String player_type) {
	if (player_type == "NONE")
		for (auto &el : this->board)
			el = PlayerType::NONE;
	else if (player_type == "HUMAN")
		for (auto &el : this->board)
			el = PlayerType::HUMAN;
	else if (player_type == "AI")
		for (auto &el : this->board)
			el = PlayerType::AI;
	else
		ERR_PRINT("This string is not player type! Use AI, HUMAN or NONE.");
}

String FiveInRowSolver::GetElementBoard(size_t row, size_t column) const {
	PlayerType element = this->board.Get(row, column);
	switch (element)
	{
		case PlayerType::NONE:
			return "NONE";
			break;
		case PlayerType::HUMAN:
			return "HUMAN";
			break;
		case PlayerType::AI:
			return "AI";
			break;
	}
}

void FiveInRowSolver::SetElementBoard(size_t row, size_t column, String player_type) {
	if (player_type == "NONE")
		this->board.Get(row, column) = PlayerType::NONE;
	else if (player_type == "HUMAN")
		this->board.Get(row, column) = PlayerType::HUMAN;
	else if (player_type == "AI")
		this->board.Get(row, column) = PlayerType::AI;
	else
		ERR_PRINT("This string is not player type! Use AI, HUMAN or NONE.");
}

String FiveInRowSolver::WhoWin(size_t block_size) {
	bool has_none = false;
	this->board.SetBlockSize(block_size);
	for (auto block = this->board.block_begin(); block != this->board.block_end(); ++block)
	{
		if (!has_none && std::any_of((*block).begin(), (*block).end(), [](PlayerType type) { return type == PlayerType::NONE; }))
			has_none = true;
		if (std::all_of((*block).begin(), (*block).end(), [](PlayerType type) { return type == PlayerType::AI; }))
			return "AI";
		if (std::all_of((*block).begin(), (*block).end(), [](PlayerType type) { return type == PlayerType::HUMAN; }))
			return "HUMAN";
	}
	return has_none ? "NONE" : "DRAW";
}

Vector2 FiveInRowSolver::NextAIMove(bool hard) {
	CycleMatrix<int> matrix_weights(this->board.GetSize());
	this->board.SetBlockSize(5);
	matrix_weights.SetBlockSize(5);
	auto block_board = this->board.block_begin();
	auto block_weights = matrix_weights.block_begin();
	for (; block_board != this->board.block_end(); ++block_board, ++block_weights) {
		int weight = GetWeight(*block_board);
		for (size_t i = 0; i < (*block_board).size(); i++) {
			if ((*block_board)[i] == PlayerType::NONE)
				(*block_weights)[i] += weight;
		}
		block_weights.SetMatrixFromBlock();
	}
	if (hard) {
		matrix_weights.SetBlockSize(8);
		this->board.SetBlockSize(8);
		block_board = this->board.block_begin();
		block_weights = matrix_weights.block_begin();
		for (; block_board != this->board.block_end(); ++block_board, ++block_weights) {
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 2 &&
					(*block_board)[3] == PlayerType::AI &&
					(*block_board)[4] == PlayerType::AI) {
				(*block_weights)[2] += 70;
				(*block_weights)[5] += 70;
			}
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 2 &&
					(*block_board)[3] == PlayerType::HUMAN &&
					(*block_board)[4] == PlayerType::HUMAN) {
				(*block_weights)[2] += 60;
				(*block_weights)[5] += 60;
			}
			block_weights.SetMatrixFromBlock();
		}
		matrix_weights.SetBlockSize(7);
		this->board.SetBlockSize(7);
		block_board = this->board.block_begin();
		block_weights = matrix_weights.block_begin();
		for (; block_board != this->board.block_end(); ++block_board, ++block_weights) {
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 3 &&
					(*block_board)[2] == PlayerType::AI &&
					(*block_board)[3] == PlayerType::AI &&
					(*block_board)[4] == PlayerType::AI) {
				(*block_weights)[1] += 200;
				(*block_weights)[5] += 200;
			}
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 3 &&
					(*block_board)[2] == PlayerType::HUMAN &&
					(*block_board)[3] == PlayerType::HUMAN &&
					(*block_board)[4] == PlayerType::HUMAN) {
				(*block_weights)[1] += 150;
				(*block_weights)[5] += 150;
			}
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 2 &&
					(*block_board)[2] == PlayerType::AI &&
					(*block_board)[4] == PlayerType::AI) {
				(*block_weights)[3] += 70;
			}
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 2 &&
					(*block_board)[2] == PlayerType::HUMAN &&
					(*block_board)[4] == PlayerType::HUMAN) {
				(*block_weights)[3] += 60;
			}
			block_weights.SetMatrixFromBlock();
		}
		matrix_weights.SetBlockSize(6);
		this->board.SetBlockSize(6);
		block_board = this->board.block_begin();
		block_weights = matrix_weights.block_begin();
		for (; block_board != this->board.block_end(); ++block_board, ++block_weights) {
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 3 &&
					(*block_board)[1] == PlayerType::AI &&
					(*block_board)[2] == PlayerType::AI &&
					(*block_board)[4] == PlayerType::AI) {
				(*block_weights)[3] += 200;
			}
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 3 &&
					(*block_board)[1] == PlayerType::AI &&
					(*block_board)[3] == PlayerType::AI &&
					(*block_board)[4] == PlayerType::AI) {
				(*block_weights)[2] += 200;
			}
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 3 &&
					(*block_board)[1] == PlayerType::HUMAN &&
					(*block_board)[2] == PlayerType::HUMAN &&
					(*block_board)[4] == PlayerType::HUMAN) {
				(*block_weights)[3] += 150;
			}
			if (std::count((*block_board).begin(), (*block_board).end(), PlayerType::AI) == 0 &&
					std::count((*block_board).begin(), (*block_board).end(), PlayerType::HUMAN) == 3 &&
					(*block_board)[1] == PlayerType::HUMAN &&
					(*block_board)[3] == PlayerType::HUMAN &&
					(*block_board)[4] == PlayerType::HUMAN) {
				(*block_weights)[2] += 150;
			}
			block_weights.SetMatrixFromBlock();
		}
	}
	auto indexes = std::max_element(matrix_weights.begin(), matrix_weights.end()).GetIndexes();
	return Vector2(indexes.first, indexes.second);
}
