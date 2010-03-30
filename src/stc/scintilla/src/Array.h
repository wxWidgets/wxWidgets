/** file Array.h
* This file is defining a kind of tool for simulating std::vector
* for using wx on OS which are not supporting the STL
* @author foldink (foldink@gmail.com)
* @date 26-February-2010
*/

#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
/* defines */
class Array;
class ArrayIterator;
//struct SelectionRange;

/**
 * @class ArrayIterator
 * @brief This class is used to simulate an iterator in the array
 * @author foldink (foldink@gmail.com)
 * @date 26-February-2010
 */
class ArrayIterator
{
	friend class Array;
	
public :
	/** ctor 
	 * @param parent, parented array to store positions in
	 */
	ArrayIterator( Array* parent , size_t idx ):
		m_parent(parent),
		m_idx(idx) {};
	
	/** dtor */
	virtual ~ArrayIterator()
	{
		m_parent = 0L;
	}
	
	/** copy ctor */
	ArrayIterator(const ArrayIterator& rhs):
		m_parent(rhs.m_parent),
		m_idx(rhs.m_idx) { }
	
	/***********************************************************************
	* OPERATOR OVERLOADS
	**********************************************************************/
	/** the equal operator is used to overload copy for the array */
	ArrayIterator& operator=(const ArrayIterator& rhs)
	{
		m_parent = rhs.m_parent;
		m_idx = rhs.m_idx;
		
		return (*this);
	}
	
	/** Smart pointer part overload as it is in standard templates arrays */
	SelectionRange& operator*();
	
	/** a++ operator */
	ArrayIterator operator++(int);
	
	/** ++a operator */
	ArrayIterator& operator++();
	
	/** a-- operator */
	ArrayIterator operator--(int);
	
	/** --a operator */
	ArrayIterator& operator--();
	
	/** a + int operator */
	ArrayIterator operator+(size_t idx);
	
	/** a - int operator */
	ArrayIterator operator-(size_t idx);
	
	/** a += int operator */
	ArrayIterator& operator+=(size_t idx);
	
	/** a -= int operator */
	ArrayIterator& operator-=(size_t idx);
	
	/** != operator */
	bool operator!=(const ArrayIterator& rhs);
	
	/** == operator */
	bool operator==(const ArrayIterator& rhs);
	
	/** Set the idx if needed */
	void SetIdx(size_t idx) {m_idx = idx;};
	/** Get the idx if needed */
	size_t GetIdx() {return m_idx;};
	
	Array* m_parent;
	size_t m_idx;
};

/** 
 * @class Array
 * @brief simple array of SelectionRange
 * @brief This class is used to simulate an iterator in the array
 * @author foldink (foldink@gmail.com)
 * @date 26-February-2010
 */
class Array
{
	friend class ArrayIterator;
	
public :
	/** default ctor taking the len in input */ 
	Array( size_t len = 0 );
	
	/** ctor with len and initial value */
	Array( size_t len , const SelectionRange& val );
	
	/** copy ctor */
	Array( const Array& rhs );
	
	/** dtor */
	virtual ~Array();
	
	/** operator= */
	Array& operator=( const Array& rhs );
	
	/** operator[] */
	SelectionRange& operator[]( size_t idx )
	{
		if( idx >= m_size )
			throw "Error access to vector range out of bounds";
		
		return *(m_data + idx);
	}
	
	/** operator[] */
	const SelectionRange& operator[]( size_t idx ) const
	{
		if( idx >= m_size )
			throw "Error access to vector range out of bounds";
		
		return *(m_data + idx);
	}
	
	/** Get the size */
	size_t size() const {return m_size;};
	
	/** check if the array is empty or not */
	bool empty() const {return (m_size == 0);};
	
	/** reserve */
	void reserve( size_t len );
	
	/** front */
	SelectionRange& front() { 
		if( !m_data )
			throw "Trying to access to an uninitialized array";
			
		return (*m_data);
	}
	
	/** back */
	SelectionRange& back() {
		if( !m_data )
			throw "Trying to access to an uninitialized array";
			
		return *(m_data + m_size-1);
	}
	
	/** push back */
	void push_back( const SelectionRange& val );
	
	/** pop back */
	void pop_back( );
	
	/** insert before the given iterator position */
	void insert( ArrayIterator it , SelectionRange& val );
	
	/** erase */
	void erase( ArrayIterator it );
	
	/** clear */
	void clear( );
	
	/** resize */
	void resize( size_t n , SelectionRange val = SelectionRange() );
	
	/** == operator */
	bool operator==(const Array& rhs);
	
	/** begin */
	ArrayIterator begin();
	
	/** end */
	ArrayIterator end();

private :
	SelectionRange* m_data;
	size_t m_size;
};

/** 
 * @brief Sorting method for a replace in Editor.cxx
 * @author foldink (foldink@gmail.com)
 * @date 26-February-2010
 */
extern void ArraySort( ArrayIterator start , ArrayIterator finish );

#endif
