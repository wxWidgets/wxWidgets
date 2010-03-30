/** file Array.cpp
* This file is defining a kind of tool for simulating std::vector
* for using wx on OS which are not supporting the STL
* @author foldink (foldink@gmail.com)
* @date 26-February-2010
*/

#include <string.h>

#include "Platform.h"
#include "Scintilla.h"
#include "Selection.h"
#include "Array.h"

SelectionRange& ArrayIterator::operator*()
{
	if ( m_idx >= m_parent->size() )
		throw "Error access out of bounds";

	return (*m_parent)[m_idx];
}

ArrayIterator ArrayIterator::operator++(int)
{
	ArrayIterator ret(*this);
	ret.m_idx++;
	return ret;
}

ArrayIterator& ArrayIterator::operator++()
{
	m_idx++;
	return (*this);
}

ArrayIterator ArrayIterator::operator--(int)
{
	if ( m_idx == 0 )
		throw "Error access out of bounds";

	ArrayIterator ret(*this);
	ret.m_idx--;
	return ret;
}

ArrayIterator& ArrayIterator::operator--()
{
	if ( m_idx == 0 )


		m_idx--;
	return (*this);
}

ArrayIterator ArrayIterator::operator+(size_t idx)
{
	ArrayIterator ret(*this);
	ret.m_idx += idx;
	return ret;
}

ArrayIterator ArrayIterator::operator-(size_t idx)
{
	if ( idx > m_idx )
		throw "Error access out of bounds";

	ArrayIterator ret(*this);
	ret.m_idx -= idx;
	return ret;
}

ArrayIterator& ArrayIterator::operator+=(size_t idx)
{
	m_idx += idx;
	return (*this);
}

ArrayIterator& ArrayIterator::operator-=(size_t idx)
{
	if ( idx > m_idx )
		throw "Error access out of bounds";

	m_idx -= idx;
	return (*this);
}

bool ArrayIterator::operator!=(const ArrayIterator& rhs)
{
	if ( m_parent == rhs.m_parent && m_idx == rhs.m_idx )
		return false;

	return true;
}

bool ArrayIterator::operator==(const ArrayIterator& rhs)
{
	if ( m_parent == rhs.m_parent && m_idx == rhs.m_idx )
		return true;

	return false;
}

Array::Array( size_t len ):
		m_data(0L),
		m_size(0)
{
	if ( len == 0 )
		return;

	m_data = new SelectionRange[len];
	m_size = len;
}

Array::Array( size_t len , const SelectionRange& val ):
		m_data(0L),
		m_size(0)
{
	if ( len == 0 )
		return;

	m_data = new SelectionRange[len];
	m_size = len;

	for ( size_t i = 0; i < len ; ++i )
		*(m_data + i) = val;
}

Array::Array( const Array& rhs ):
		m_data(0L),
		m_size(0)
{
	if ( rhs.m_size == 0 )
		return;

	m_data = new SelectionRange[rhs.m_size];
	m_size = rhs.m_size;
	memcpy( m_data , rhs.m_data , m_size*sizeof(SelectionRange) );
}

Array::~Array()
{
	if ( m_data )
		delete m_data;
}

Array& Array::operator=( const Array& rhs )
{
	if ( m_data )
		delete m_data;

	m_data = 0L;
	m_size = 0;

	if ( rhs.m_size == 0 )
		return (*this);

	m_data = new SelectionRange[rhs.m_size];
	m_size = rhs.m_size;
	memcpy( m_data , rhs.m_data , m_size*sizeof(SelectionRange) );

	return (*this);
}

void Array::reserve( size_t len )
{
	if ( len <= m_size || len == 0 )
		return;

	SelectionRange* data = 0L;
	data = new SelectionRange[len];

	if ( m_data ) {
		memcpy( data , m_data , m_size*sizeof(SelectionRange) );
		delete m_data;
		m_data = data;
	} else
		m_data = data;
}

void Array::push_back( const SelectionRange& val )
{
	if ( !m_data ) {
		m_data = new SelectionRange[1];
		*m_data = val;
		m_size = 1;
		return;
	}

	SelectionRange* data = 0L;
	data = new SelectionRange[m_size+1];
	memcpy( data , m_data , m_size*sizeof(SelectionRange) );
	m_data = data;
	*(m_data+m_size) = val;
	m_size++;
}

void Array::pop_back( )
{
	if ( !m_data )
		throw "Error access out of bounds";

	m_size--;
	SelectionRange* data = 0L;

	if ( m_size > 0 ) {
		data = new SelectionRange[m_size];
		memcpy( data , m_data , m_size*sizeof(SelectionRange) );
	}

	delete m_data;
	m_data = data;
}

void Array::insert( ArrayIterator it , SelectionRange& val )
{
	size_t idx = it.GetIdx();
	SelectionRange* data = 0L;
	if (!m_data) {
		m_size = idx+1;
		m_data = new SelectionRange[m_size];
	} else if ( idx >= m_size ) {
		data = new SelectionRange[idx+1];
		memcpy( data , m_data , m_size*sizeof(SelectionRange) );
		m_size = idx+1;
		delete m_data;
		m_data = data;
	} else if ( idx == 0 ) {
		data = new SelectionRange[m_size+1];
		memcpy( data + 1 , m_data , m_size*sizeof(SelectionRange) );
		delete m_data;
		m_size++;
		m_data = data;
	} else {
		data = new SelectionRange[m_size+1];
		memcpy( data , m_data , idx*sizeof(SelectionRange) );
		memcpy( data + idx + 1 , m_data + idx , (m_size-idx)*sizeof(SelectionRange) );
		delete m_data;
		m_size++;
		m_data = data;
	}

	*(m_data + idx) = val;
}

void Array::erase( ArrayIterator it )
{
	size_t idx = it.GetIdx();
	if (!m_data || idx >= m_size )
		throw "Error access out of bounds";

	SelectionRange* data = 0L;
	if ( m_size == 1 ) {
		delete m_data;
		m_data = 0L;
		return;
	} else if ( idx == 0) {
		data = new SelectionRange[m_size-1];
		memcpy( data , m_data + 1 , (m_size-1)*sizeof(SelectionRange) );
		delete m_data;
		m_size--;
		m_data = data;
	} else {
		data = new SelectionRange[m_size-1];
		memcpy( data , m_data , (idx)*sizeof(SelectionRange) );
		memcpy( data + idx , m_data + idx + 1 , (m_size-idx-1)*sizeof(SelectionRange) );
		delete m_data;
		m_size--;
		m_data = data;
	}
}

void Array::clear( )
{
	if ( !m_data )
		return;

	delete m_data;
	m_data = 0L;
	m_size = 0;
}

void Array::resize( size_t n , SelectionRange val )
{
	if ( (n == 0 && !m_data) || n <= m_size )
		return;

	if ( n == 0 ) {
		delete m_data;
		m_data = 0L;
		m_size = 0;
	}

	SelectionRange* data = 0L;
	data = new SelectionRange[n];
	memcpy( data , m_data , m_size*sizeof(SelectionRange) );
	delete m_data;
	m_data = data;

	for ( size_t i = m_size ; i < n ; ++i )
		*( m_data + i ) = val;

	m_size = n;
}

bool Array::operator==(const Array& rhs)
{
	if ( m_size != rhs.m_size )
		return false;

	if ( m_size == 0 && rhs.m_size == 0 )
		return true;

	for ( size_t i = 0 ; i < m_size ; ++i )
		if ( ! ((*(m_data +i)) == (*(rhs.m_data+i))) )
			return false;

	return true;
}

ArrayIterator Array::begin()
{
	ArrayIterator ret(this,0);
	return ret;
}

ArrayIterator Array::end()
{
	ArrayIterator ret(this,m_size);
	return ret;
}


int partition(Array &array, int top, int bottom)
{
	SelectionRange x = array[top];
	int i = top - 1;
	int j = bottom + 1;
	SelectionRange temp;
	do {
		do {
			j--;
		} while (x < array[j]);

		do {
			i++;
		} while (array[j] < x);

		if (i < j) {
			temp = array[i];
			array[i] = array[j];
			array[j] = temp;
		}
	} while (i < j);
	return j;           // returns middle subscript
}

void quicksort(Array &num, int top, int bottom)
{
	// top = subscript of beginning of array
	// bottom = subscript of end of array

	int middle;
	if (top < bottom) {
		middle = partition(num, top, bottom);
		quicksort(num, top, middle);   // sort first section
		quicksort(num, middle+1, bottom);    // sort second section
	}
	return;
}

void ArraySort( ArrayIterator start , ArrayIterator finish )
{
	if ( start.m_parent != finish.m_parent )
		throw "Invalid iterators parent mismatch";
	
	if ( start.m_idx >= finish.m_idx-1 )
		throw "Invalid iterators are refering to bad values";
		
	quicksort( *(start.m_parent) , start.m_idx , finish.m_idx-1 );
}
