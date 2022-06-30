/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * GchartMapBase.hpp
 * Copyright (C) Martijn Goedhart 2022 <goedhart.martijn@gmail.com>
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GCHART_MAPBASE_HPP__
#define __GCHART_MAPBASE_HPP__

#include <iterator>
#include <map>

template<class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
class GchartMapBase {
public:
	// types
	using key_type               = Key;
	using mapped_type            = T;
	using value_type             = std::pair<const Key, T>;
	using key_compare            = Compare;
	using allocator_type         = Allocator;
	using pointer                = typename std::allocator_traits<Allocator>::pointer;
	using const_pointer          = typename std::allocator_traits<Allocator>::const_pointer;
	using reference              = value_type&;
	using const_reference        = const value_type&;
	using size_type              = std::size_t;
	using difference_type        = typename std::map<Key, T, Compare, Allocator>::difference_type;
	using iterator               = typename std::map<Key, T, Compare, Allocator>::iterator;
	using const_iterator         = typename std::map<Key, T, Compare, Allocator>::const_iterator;
	using reverse_iterator       = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	//using node_type              = /* unspecified */;
	//using insert_return_type     = /*insert-return-type*/<iterator, node_type>;

	class value_compare {
		friend class GchartMapBase;

	protected:
		Compare comp;
		value_compare(Compare c) : comp(c) {}

	public:
		bool operator() (const value_type& x, const value_type& y) const {
			return comp (x.first, y.first);
		}
	};
/*
	// construct/copy/destroy
	GchartMapBase (void) : GchartMapBase (Compare()) { }
	explicit GchartMapBase (const Compare& comp, const Allocator& = Allocator());
	template<class InputIt>GchartMapBase (InputIt first, InputIt last, const Compare& comp = Compare(), const Allocator& = Allocator());
	GchartMapBase (const GchartMapBase& x);
	GchartMapBase (GchartMapBase&& x);
	explicit GchartMapBase (const Allocator&);
	GchartMapBase (const GchartMapBase&, const Allocator&);
	GchartMapBase (GchartMapBase&&, const Allocator&);
	GchartMapBase (std::initializer_list<value_type>, const Compare& = Compare(), const Allocator& = Allocator());
	template<class InputIt>GchartMapBase (InputIt first, InputIt last, const Allocator& a) : GchartMapBase (first, last, Compare(), a) { }
	GchartMapBase (std::initializer_list<value_type> il, const Allocator& a)	: GchartMapBase(il, Compare(), a) { }
	~GchartMapBase (void);
	GchartMapBase& operator= (const GchartMapBase& x);
	GchartMapBase& operator= (GchartMapBase&& x) noexcept(std::allocator_traits<Allocator>::is_always_equal::value && is_nothrow_move_assignable_v<Compare>);
	GchartMapBase& operator= (std::initializer_list<value_type>);
	allocator_type get_allocator (void) const noexcept;
*/
	// iterators
	virtual iterator               begin(void) noexcept = 0;
	virtual const_iterator         begin(void) const noexcept = 0;
	virtual iterator               end(void) noexcept = 0;
	virtual const_iterator         end(void) const noexcept = 0;

	virtual reverse_iterator       rbegin(void) noexcept = 0;
	virtual const_reverse_iterator rbegin(void) const noexcept = 0;
	virtual reverse_iterator       rend(void) noexcept = 0;
	virtual const_reverse_iterator rend(void) const noexcept = 0;

	virtual const_iterator         cbegin(void) const noexcept = 0;
	virtual const_iterator         cend(void) const noexcept = 0;
	virtual const_reverse_iterator crbegin(void) const noexcept = 0;
	virtual const_reverse_iterator crend(void) const noexcept = 0;
/*
	// capacity
	virtual [[nodiscard]] bool empty (void) const noexcept = 0;
	virtual size_type size (void) const noexcept = 0;
	virtual size_type max_size (void) const noexcept = 0;

	// element access
	virtual mapped_type& operator[] (const key_type& x) = 0;
	virtual mapped_type& operator[] (key_type&& x) = 0;
	virtual mapped_type&       at (const key_type& x) = 0;
	virtual const mapped_type& at (const key_type& x) const = 0;

	// modifiers
	virtual template<class... Args> pair<iterator, bool> emplace (Args&&... args) = 0;
	virtual template<class... Args> iterator emplace_hint (const_iterator position, Args&&... args) = 0;
	virtual pair<iterator, bool> insert (const value_type& x) = 0;
	virtual pair<iterator, bool> insert (value_type&& x) = 0;
	virtual template<class P> pair<iterator, bool> insert (P&& x) = 0;
	virtual iterator insert (const_iterator position, const value_type& x) = 0;
	virtual iterator insert (const_iterator position, value_type&& x) = 0;
	virtual template<class P> iterator insert (const_iterator position, P&&) = 0;
	virtual template<class InputIt>	void insert (InputIt first, InputIt last) = 0;
	virtual void insert (std::initializer_list<value_type>) = 0;

	virtual node_type extract (const_iterator position) = 0;
	virtual node_type extract (const key_type& x) = 0;
	virtual template<class K> node_type extract (K&& x) = 0;
	virtual insert_return_type insert (node_type&& nh) = 0;
	virtual iterator           insert (const_iterator hint, node_type&& nh) = 0;

	virtual template<class... Args> pair<iterator, bool> try_emplace (const key_type& k, Args&&... args) = 0;
	virtual template<class... Args> pair<iterator, bool> try_emplace (key_type&& k, Args&&... args) = 0;
	virtual template<class... Args>	iterator try_emplace (const_iterator hint, const key_type& k, Args&&... args) = 0;
	virtual template<class... Args>	iterator try_emplace (const_iterator hint, key_type&& k, Args&&... args) = 0;
	virtual template<class M> pair<iterator, bool> insert_or_assign (const key_type& k, M&& obj) = 0;
	virtual template<class M> pair<iterator, bool> insert_or_assign (key_type&& k, M&& obj) = 0;
	virtual template<class M> iterator insert_or_assign (const_iterator hint, const key_type& k, M&& obj) = 0;
	virtual template<class M> iterator insert_or_assign (const_iterator hint, key_type&& k, M&& obj) = 0;

	virtual iterator  erase (iterator position) = 0;
	virtual iterator  erase (const_iterator position) = 0;
	virtual size_type erase (const key_type& x) = 0;
	virtual template<class K> size_type erase (K&& x) = 0;
	virtual iterator  erase (const_iterator first, const_iterator last) = 0;
	//virtual void      swap (GchartMapBase&) noexcept(allocator_traits<Allocator>::is_always_equal::value &&	is_nothrow_swappable_v<Compare>) = 0;
	virtual void      clear (void) noexcept = 0;

	virtual template<class C2> void merge (GchartMapBase<Key, T, C2, Allocator>& source) = 0;
	virtual template<class C2> void merge (GchartMapBase<Key, T, C2, Allocator>&& source) = 0;
	virtual template<class C2> void merge (map<Key, T, C2, Allocator>& source) = 0;
	virtual template<class C2> void merge (map<Key, T, C2, Allocator>&& source) = 0;
	virtual template<class C2> void merge (multimap<Key, T, C2, Allocator>& source) = 0;
	virtual template<class C2> void merge (multimap<Key, T, C2, Allocator>&& source) = 0;

	// observers
	virtual key_compare key_comp (void) const = 0;
	virtual value_compare value_comp (void) const = 0;

	// map operations
	virtual iterator       find (const key_type& x) = 0;
	virtual const_iterator find (const key_type& x) const = 0;
	virtual template<class K> iterator       find (const K& x) = 0;
	virtual template<class K> const_iterator find (const K& x) const = 0;

	virtual size_type      count (const key_type& x) const = 0;
	virtual template<class K> size_type count (const K& x) const = 0;

	virtual iterator       lower_bound (const key_type& x) = 0;
	virtual const_iterator lower_bound (const key_type& x) const = 0;
	virtual template<class K> iterator       lower_bound (const K& x) = 0;
	virtual template<class K> const_iterator lower_bound (const K& x) const = 0;

	virtual iterator       upper_bound (const key_type& x) = 0;
	virtual const_iterator upper_bound (const key_type& x) const = 0;
	virtual template<class K> iterator       upper_bound (const K& x) = 0;
	virtual template<class K> const_iterator upper_bound (const K& x) const = 0;

	virtual pair<iterator, iterator>               equal_range (const key_type& x) = 0;
	virtual pair<const_iterator, const_iterator>   equal_range (const key_type& x) const = 0;
	virtual template<class K> pair<iterator, iterator>             equal_range (const K& x) = 0;
	virtual template<class K> pair<const_iterator, const_iterator> equal_range (const K& x) const = 0;
	*/
};

#endif /* __GCHART_MAPBASE_HPP__ */
