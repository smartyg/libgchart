/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * GchartMap.hpp
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

#ifndef __GCHART_MAP_HPP__
#define __GCHART_MAP_HPP__

#include <map>

#include "GchartMapBase.hpp"

template<class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
class GchartMap : public GchartMapBase<Key, T, Compare, Allocator> {
private:
	std::map<Key, T, Compare, Allocator> _map;

public:
	// types
	using key_type               = typename std::map<Key, T, Compare, Allocator>::key_type;
	using mapped_type            = typename std::map<Key, T, Compare, Allocator>::mapped_type;
	using value_type             = typename std::map<Key, T, Compare, Allocator>::value_type;
	using key_compare            = typename std::map<Key, T, Compare, Allocator>::key_compare;
	using allocator_type         = typename std::map<Key, T, Compare, Allocator>::allocator_type;
	using pointer                = typename std::map<Key, T, Compare, Allocator>::pointer;
	using const_pointer          = typename std::map<Key, T, Compare, Allocator>::const_pointer;
	using reference              = typename std::map<Key, T, Compare, Allocator>::reference;
	using const_reference        = typename std::map<Key, T, Compare, Allocator>::const_reference;
	using size_type              = typename std::map<Key, T, Compare, Allocator>::size_type;
	using difference_type        = typename std::map<Key, T, Compare, Allocator>::difference_type;
	using iterator               = typename std::map<Key, T, Compare, Allocator>::iterator;
	using const_iterator         = typename std::map<Key, T, Compare, Allocator>::const_iterator;
	using reverse_iterator       = typename std::map<Key, T, Compare, Allocator>::reverse_iterator;
	using const_reverse_iterator = typename std::map<Key, T, Compare, Allocator>::const_reverse_iterator;

	// construct/copy/destroy
	GchartMap (void) : GchartMap (Compare()) { }
	explicit GchartMap (const Compare& comp, const Allocator& alloc = Allocator()) { this->_map (comp, alloc); }
	template<class InputIt>GchartMap (InputIt first, InputIt last, const Compare& comp = Compare(), const Allocator& alloc = Allocator()) { this->_map (first, last, comp, alloc); }
	//GchartMap (const GchartMap& x) { this->_map (x); }
	//GchartMap (GchartMap&& x);
	explicit GchartMap (const Allocator& alloc) { this->_map (alloc); }
	//GchartMap (const GchartMap&, const Allocator& alloc);
	//GchartMap (GchartMap&&, const Allocator&);
	GchartMap (std::initializer_list<value_type> il, const Compare& comp = Compare(), const Allocator& alloc = Allocator()) { this->_map (il, comp, alloc); }
	template<class InputIt>GchartMap (InputIt first, InputIt last, const Allocator& alloc) : GchartMap (first, last, Compare(), alloc) { }
	GchartMap (std::initializer_list<value_type> il, const Allocator& alloc) { this->_map (il, alloc); }
	~GchartMap (void) { }
	//GchartMap& operator= (const GchartMap& x);
	//GchartMap& operator= (GchartMap&& x) noexcept(std::allocator_traits<Allocator>::is_always_equal::value && is_nothrow_move_assignable_v<Compare>);
	//GchartMap& operator= (std::initializer_list<value_type>);
	allocator_type get_allocator (void) const noexcept { return this->_map.get_allocator (); }

	// iterators
	iterator               begin (void) noexcept override { return this->_map.begin (); }
	const_iterator         begin (void) const noexcept override { return this->_map.begin (); }
	iterator               end (void) noexcept override { return this->_map.end (); }
	const_iterator         end (void) const noexcept override { return this->_map.end (); }

	reverse_iterator       rbegin (void) noexcept override { return this->_map.rbegin (); }
	const_reverse_iterator rbegin (void) const noexcept override { return this->_map.rbegin (); }
	reverse_iterator       rend (void) noexcept override { return this->_map.rend (); }
	const_reverse_iterator rend (void) const noexcept override { return this->_map.rend (); }

	const_iterator         cbegin (void) const noexcept override { return this->_map.cbegin (); }
	const_iterator         cend (void) const noexcept override { return this->_map.cend (); }
	const_reverse_iterator crbegin (void) const noexcept override { return this->_map.crbegin (); }
	const_reverse_iterator crend (void) const noexcept override { return this->_map.crend (); }
/*
	// capacity
	[[nodiscard]] bool empty (void) const noexcept override { return this->_map.empty (); }
	size_type size (void) const noexcept override { return this->_map.size (); }
	size_type max_size (void) const noexcept override { return this->_map.max_size (); }

	// element access
	mapped_type& operator[](const key_type& x) override { return this->_map[x]; }
	mapped_type& operator[](key_type&& x) override { return this->_map[x]; }
	mapped_type&       at(const key_type& x) override { return this->_map.at (x); }
	const mapped_type& at(const key_type& x) const override { return this->_map.at (x); }

	// modifiers
	template<class... Args> pair<iterator, bool> emplace (Args&&... args) override { return this->_map.emplace (args); }
	template<class... Args> iterator emplace_hint (const_iterator position, Args&&... args) override { return this->_map.emplace_hint (position, args); }
	pair<iterator, bool> insert (const value_type& x) override { return this->_map.insert (x); }
	pair<iterator, bool> insert (value_type&& x) override { return this->_map.insert (x); }
	template<class P> pair<iterator, bool> insert (P&& x) override { return this->_map.insert (x); }
	iterator insert (const_iterator position, const value_type& x) override { return this->_map.insert (); }
	iterator insert (const_iterator position, value_type&& x) override { return this->_map.insert (); }
	template<class P> iterator insert (const_iterator position, P&& x) override { return this->_map.insert (position, x); }
	template<class InputIt>	void insert (InputIt first, InputIt last) override { return this->_map.insert (first, last); }
	//void insert (std::initializer_list<value_type>) override { return this->_map.insert (); }

	node_type extract (const_iterator position) override { return this->_map.extract (position); }
	node_type extract (const key_type& x) override { return this->_map.extract (x); }
	template<class K> node_type extract (K&& x) override { return this->_map.extract (x); }
	insert_return_type insert (node_type&& nh) override { return this->_map.insert (nh); }
	iterator           insert (const_iterator hint, node_type&& nh) override { return this->_map.insert (hint, nh); }

	template<class... Args> pair<iterator, bool> try_emplace (const key_type& k, Args&&... args) override { return this->_map.try_emplace (k, args); }
	template<class... Args> pair<iterator, bool> try_emplace (key_type&& k, Args&&... args) override { return this->_map.try_emplace (k, args); }
	template<class... Args>	iterator try_emplace (const_iterator hint, const key_type& k, Args&&... args) override { return this->_map.try_emplace (hint, k, args); }
	template<class... Args>	iterator try_emplace (const_iterator hint, key_type&& k, Args&&... args) override { return this->_map.try_emplace (hint, k, args); }
	template<class M> pair<iterator, bool> insert_or_assign (const key_type& k, M&& obj) override { return this->_map.insert_or_assign (k, obj); }
	template<class M> pair<iterator, bool> insert_or_assign (key_type&& k, M&& obj) override { return this->_map.insert_or_assign (k, obj); }
	template<class M> iterator insert_or_assign (const_iterator hint, const key_type& k, M&& obj) override { return this->_map.insert_or_assign (hint, k, obj); }
	template<class M> iterator insert_or_assign (const_iterator hint, key_type&& k, M&& obj) override { return this->_map.insert_or_assign (hint, k, obj); }

	iterator  erase (iterator position) override { return this->_map.erase (position); }
	iterator  erase (const_iterator position) override { return this->_map.erase (position); }
	size_type erase (const key_type& x) override { return this->_map.erase (x); }
	template<class K> size_type erase (K&& x) override { return this->_map.erase (x); }
	iterator  erase (const_iterator first, const_iterator last) override { return this->_map.erase (first, last); }
	void      clear (void) noexcept override { return this->_map.clear (); }

	template<class C2> void merge (GchartMap<Key, T, C2, Allocator>& source) override { return this->_map.merge (source); }
	template<class C2> void merge (GchartMap<Key, T, C2, Allocator>&& source) override { return this->_map.merge (source); }
	template<class C2> void merge (map<Key, T, C2, Allocator>& source) override { return this->_map.merge (source); }
	template<class C2> void merge (map<Key, T, C2, Allocator>&& source) override { return this->_map.merge (source); }
	template<class C2> void merge (multimap<Key, T, C2, Allocator>& source) override { return this->_map.merge (source); }
	template<class C2> void merge (multimap<Key, T, C2, Allocator>&& source) override { return this->_map.merge (source); }

	// observers
	key_compare key_comp (void) const override { return this->_map.key_comp (); }
	value_compare value_comp (void) const override { return this->_map.value_comp (); }

	// map operations
	iterator       find (const key_type& x) override { return this->_map.find (x); }
	const_iterator find (const key_type& x) const override { return this->_map.find (x); }
	template<class K> iterator       find (const K& x) override { return this->_map.find (x); }
	template<class K> const_iterator find (const K& x) const override { return this->_map.find (x); }

	size_type      count (const key_type& x) const override { return this->_map.count (x); }
	template<class K> size_type count (const K& x) const override { return this->_map.count (x); }

	iterator       lower_bound (const key_type& x) override { return this->_map.lower_bound (x); }
	const_iterator lower_bound (const key_type& x) const override { return this->_map.lower_bound (x); }
	template<class K> iterator       lower_bound (const K& x) override { return this->_map.lower_bound (x); }
	template<class K> const_iterator lower_bound (const K& x) const override { return this->_map.lower_bound (x); }

	iterator       upper_bound (const key_type& x) override { return this->_map.upper_bound (x); }
	const_iterator upper_bound (const key_type& x) const override { return this->_map.upper_bound (x); }
	template<class K> iterator       upper_bound (const K& x) override { return this->_map.upper_bound (x); }
	template<class K> const_iterator upper_bound (const K& x) const override { return this->_map.upper_bound (x); }

	pair<iterator, iterator>               equal_range (const key_type& x) override { return this->_map.equal_range (x); }
	pair<const_iterator, const_iterator>   equal_range (const key_type& x) const override { return this->_map.equal_range (x); }
	template<class K> pair<iterator, iterator>             equal_range (const K& x) override { return this->_map.equal_range (x); }
	template<class K> pair<const_iterator, const_iterator> equal_range (const K& x) const override { return this->_map.equal_range (x); }
	*/
};

#endif /* __GCHART_MAP_HPP__ */
