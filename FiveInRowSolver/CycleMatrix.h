#pragma once
#include <vector>
#include <algorithm>
#include <utility>

template <typename T>
class CycleMatrix
{
private:
	//Класс итератора:
	class Iterator : public std::iterator<std::bidirectional_iterator_tag, T>
	{
		friend class CycleMatrix;
		std::vector<std::vector<T>>* matrix_ptr; //Указатель на матрицу
		int row;
		int column;
	private:
		//Приватный конструктор:
		Iterator(std::vector<std::vector<T>>* matrix_ptr, int row, int column) : matrix_ptr(matrix_ptr), row(row), column(column) {}
	public:
		//Копирующий конструктор:
		Iterator(const Iterator& other) : matrix_ptr(other.matrix_ptr), row(other.row), column(other.column) {}
		//Операторы:
		Iterator& operator=(const Iterator& other);
		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const;
		T& operator*();
		Iterator& operator++();//Префикс
		Iterator& operator--();//Префикс
		Iterator operator++(int);//Постфикс
		Iterator operator--(int);//Постфикс
		//Методы
		std::pair<size_t, size_t> GetIndexes() const;
	};
	class BlockIterator
	{
		enum class Move
		{
			Horizontal,
			Vertical,
			MainDiagonal,
			SideDiagonal,
			End
		};
		friend class CycleMatrix;
		std::vector<std::vector<T>>* matrix_ptr; //Указатель на матрицу
		std::vector<T> block;
		int row;
		int column;
		Move move;
	private:
		size_t ToMatrixIndex(int index)
		{
			return index >= 0 ? index % int(matrix_ptr->size()) : matrix_ptr->size() + index % int(matrix_ptr->size());
		}
		std::pair<size_t, size_t> NextIndexes(int row, int column)
		{
			switch (this->move)
			{
			case Move::Horizontal:
				return { ToMatrixIndex(row), ToMatrixIndex(++column) };
			case Move::Vertical:
				return { ToMatrixIndex(++row), ToMatrixIndex(column) };
			case Move::MainDiagonal:
				return { ToMatrixIndex(++row), ToMatrixIndex(++column) };
			case Move::SideDiagonal:
				return { ToMatrixIndex(++row), ToMatrixIndex(--column) };
			case Move::End:
				throw std::exception("Can't find next indexes of End move");
			}
		}
		void SetBlock(int row, int column)
		{
			std::pair<size_t, size_t> indexes = { row, column };
			for (size_t i = 0; i < block.size(); i++)
				block[i] = (*matrix_ptr)[indexes.first][indexes.second],
				indexes = NextIndexes(indexes.first, indexes.second);
		}
		//Приватный конструктор:
		BlockIterator(std::vector<std::vector<T>>* matrix_ptr, int row, int column, Move move, size_t block_size) : matrix_ptr(matrix_ptr), row(row), column(column), move(move)
		{
			block.resize(block_size);
			if (matrix_ptr != nullptr)
				SetBlock(row, column);
		}
	public:
		void SetMatrixFromBlock()
		{
			std::pair<size_t, size_t> indexes = { this->row, this->column };
			for (size_t i = 0; i < block.size(); i++)
				(*matrix_ptr)[indexes.first][indexes.second] = block[i],
				indexes = NextIndexes(indexes.first, indexes.second);
		}
		//Копирующий конструктор:
		BlockIterator(const BlockIterator& other) : matrix_ptr(other.matrix_ptr), row(other.row), column(other.column), block(other.block), move(other.move) {}
		//Операторы:
		BlockIterator& operator=(const BlockIterator& other);
		bool operator==(const BlockIterator& other) const;
		bool operator!=(const BlockIterator& other) const;
		std::vector<T>& operator*();
		BlockIterator& operator++();//Префикс
		Iterator& operator--();//Префикс
		Iterator operator++(int);//Постфикс
		Iterator operator--(int);//Постфикс
		//Методы
		std::pair<size_t, size_t> GetBlockPosition() const;
	};
public:
	using iterator = Iterator;
	using block_iterator = BlockIterator;
	CycleMatrix();
	CycleMatrix(size_t size);
	T& Get(size_t row, size_t column);
	T Get(size_t row, size_t column) const;
	size_t GetSize() const;
	void SetBlockSize(size_t block_size);
	size_t GetBlockSize() const;
	iterator begin();
	iterator end();
	block_iterator block_begin();
	block_iterator block_end();
private:
	std::vector<std::vector<T>> matrix;
	size_t size;
	size_t block_size;
};

template<typename T>
CycleMatrix<T>::CycleMatrix() : size(0), block_size(1)
{
	matrix = { {} };
}

template<typename T>
CycleMatrix<T>::CycleMatrix(size_t size)
{
	this->size = size;
	this->block_size = 1;
	for (size_t i = 0; i < this->size; i++)
	{
		this->matrix.emplace_back(this->size);
	}
}

template<typename T>
T& CycleMatrix<T>::Get(size_t row, size_t column)
{
	return this->matrix[row][column];
}

template<typename T>
T CycleMatrix<T>::Get(size_t row, size_t column) const
{
	return this->matrix[row][column];
}

template<typename T>
size_t CycleMatrix<T>::GetSize() const
{
	return this->size;
}

template<typename T>
void CycleMatrix<T>::SetBlockSize(size_t block_size)
{
	if (block_size > this->size)
		throw std::exception("Incorrect block size");
	this->block_size = block_size;
}

template<typename T>
size_t CycleMatrix<T>::GetBlockSize() const
{
	return this->block_size;
}

template<typename T>
typename CycleMatrix<T>::iterator CycleMatrix<T>::begin()
{
	return iterator(&this->matrix, 0, 0);
}

template<typename T>
typename CycleMatrix<T>::iterator CycleMatrix<T>::end()
{
	return iterator(nullptr, this->size, 0);
}

template<typename T>
typename CycleMatrix<T>::block_iterator CycleMatrix<T>::block_begin()
{
	return block_iterator(&this->matrix, 0, 0, CycleMatrix<T>::block_iterator::Move::Horizontal, this->block_size);
}

template<typename T>
typename CycleMatrix<T>::block_iterator CycleMatrix<T>::block_end()
{
	return block_iterator(nullptr, 0, 0, CycleMatrix<T>::block_iterator::Move::End, this->block_size);
}

template<typename T>
typename CycleMatrix<T>::Iterator& CycleMatrix<T>::Iterator::operator=(const Iterator& other)
{
	if (this != &other)
		this->matrix_ptr = other.matrix_ptr,
		this->row = other.row,
		this->column = other.column;
	return *this;
}

template<typename T>
bool CycleMatrix<T>::Iterator::operator==(const Iterator& other) const
{
	return this->matrix_ptr == other.matrix_ptr && this->row == other.row && this->column == other.column;
}

template<typename T>
bool CycleMatrix<T>::Iterator::operator!=(const Iterator& other) const
{
	return !(*this == other);
}

template<typename T>
T& CycleMatrix<T>::Iterator::operator*()
{
	if (this->matrix_ptr == nullptr)
		throw std::exception("Can't dereference out of range CycleMatrix iterator");
	return this->matrix_ptr->at(this->row).at(this->column);
}

template<typename T>
typename CycleMatrix<T>::Iterator& CycleMatrix<T>::Iterator::operator++()
{
	if (this->matrix_ptr == nullptr)
		throw std::exception("Can't increment CycleMatrix iterator past end");
	this->column++;
	if (this->column >= this->matrix_ptr->at(this->row).size())
	{
		this->column = 0;
		this->row++;
		if (this->row >= this->matrix_ptr->size())
			this->matrix_ptr = nullptr;
	}
	return *this;
}

template<typename T>
typename CycleMatrix<T>::Iterator& CycleMatrix<T>::Iterator::operator--()
{
	if (this->matrix_ptr == nullptr)
		throw std::exception("Can't decrement CycleMatrix iterator before end");
	this->column--;
	if (this->column < 0)
	{
		this->column = this->matrix_ptr->at(this->row).size() - 1;
		this->row--;
		if (this->row < 0)
			throw std::exception("Can't decrement CycleMatrix iterator before begin");
	}
	return *this;
}

template<typename T>
typename CycleMatrix<T>::Iterator CycleMatrix<T>::Iterator::operator++(int)
{
	if (this->matrix_ptr == nullptr)
		throw std::exception("Can't increment CycleMatrix iterator past end");
	auto prev = this;
	this->column++;
	if (this->column >= this->matrix_ptr->at(this->row).size())
	{
		this->column = 0;
		this->row++;
		if (this->row >= this->matrix_ptr->size())
			this->matrix_ptr = nullptr;
	}
	return *prev;
}

template<typename T>
typename CycleMatrix<T>::Iterator CycleMatrix<T>::Iterator::operator--(int)
{
	if (this->matrix_ptr == nullptr)
		throw std::exception("Can't decrement CycleMatrix iterator before end");
	auto prev = this;
	this->column--;
	if (this->column < 0)
	{
		this->column = this->matrix_ptr->at(this->row).size() - 1;
		this->row--;
		if (this->row < 0)
			throw std::exception("Can't decrement CycleMatrix iterator before begin");
	}
	return *prev;
}

template<typename T>
std::pair<size_t, size_t> CycleMatrix<T>::Iterator::GetIndexes() const
{
	return {this->row, this->column};
}

template<typename T>
typename CycleMatrix<T>::BlockIterator& CycleMatrix<T>::BlockIterator::operator=(const BlockIterator& other)
{
	if (this != &other)
		this->matrix_ptr = other.matrix_ptr,
		this->row = other.row,
		this->column = other.column,
		this->move = other.move,
		this->block = other.block;
	return *this;
}

template<typename T>
bool CycleMatrix<T>::BlockIterator::operator==(const BlockIterator& other) const
{
	if (this->move == Move::End && other.move == Move::End)
		return true;
	return this->matrix_ptr == other.matrix_ptr && this->row == other.row && this->column == other.column && this->move == other.move;
}

template<typename T>
bool CycleMatrix<T>::BlockIterator::operator!=(const BlockIterator& other) const
{
	return !(*this == other);
}

template<typename T>
std::vector<T>& CycleMatrix<T>::BlockIterator::operator*()
{
	return this->block;
}

template<typename T>
typename CycleMatrix<T>::BlockIterator& CycleMatrix<T>::BlockIterator::operator++()
{
	switch (this->move)
	{
	case Move::Horizontal:
		if (this->column == this->matrix_ptr->size() - 1)
		{
			this->row++;
			this->column = 0;
			if (this->row == this->matrix_ptr->size())
			{
				this->row = 0;
				this->move = Move::Vertical;
			}
		}
		else
			this->column++;
		break;
	case Move::Vertical:
		if (this->row == this->matrix_ptr->size() - 1)
		{
			this->column++;
			this->row = 0;
			if (this->column == this->matrix_ptr->size())
			{
				this->column = 0;
				this->move = Move::MainDiagonal;
			}
		}
		else
			this->row++;
		break;
	case Move::MainDiagonal:
		if (this->column == this->matrix_ptr->size() - 1)
		{
			this->column = 0;
			if (this->row == this->matrix_ptr->size() - 2)
				this->move = Move::SideDiagonal;
			this->row = ToMatrixIndex(this->row + 2);
		}
		else
		{
			this->row = ToMatrixIndex(this->row + 1);
			this->column = ToMatrixIndex(this->column + 1);
		}
		break;
	case Move::SideDiagonal:
		if (this->row == this->matrix_ptr->size() - 1)
		{
			this->row = 0;
			if (this->column == 0)
				this->move = Move::End;
		}
		else
		{
			this->row = ToMatrixIndex(this->row + 1);
			this->column = ToMatrixIndex(this->column - 1);
		}
		break;
	case Move::End:
		throw std::exception("Can't increment CycleMatrix block_iterator past end");
	}
	if(this->move != Move::End)
		SetBlock(this->row, this->column);
	return *this;
}
