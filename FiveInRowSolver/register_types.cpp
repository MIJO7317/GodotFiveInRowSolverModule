#include "register_types.h"
#include "core/class_db.h"
#include "FiveInRowSolver.h"

void register_tictactoe_solver_types() {
	ClassDB::register_class<FiveInRowSolver>();
}

void unregister_tictactoe_solver_types() {
}
