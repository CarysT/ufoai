/*
 Copyright (C) 2001-2006, William Joseph.
 All Rights Reserved.

 This file is part of GtkRadiant.

 GtkRadiant is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GtkRadiant is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GtkRadiant; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#if !defined(INCLUDED_FS_FILESYSTEM_H)
#define INCLUDED_FS_FILESYSTEM_H

#include "string/string.h"
#include "os/path.h"

#include <string>
#include <map>

inline unsigned int path_get_depth (const std::string& path)
{
	unsigned int depth = 0;
	const char* cstr = path.c_str();
	while (cstr != 0 && cstr[0] != '\0') {
		cstr = strchr(cstr, '/');
		if (cstr != 0) {
			++cstr;
		}
		++depth;
	}
	return depth;
}

/// \brief A generic unix-style file-system which maps paths to files and directories.
/// Provides average O(log n) find and insert methods.
/// \param file_type The data type which represents a file.
template<typename file_type>
class GenericFileSystem
{
		class Path
		{
				std::string m_path;
				unsigned int m_depth;
			public:
				Path (const std::string& path) :
					m_path(path), m_depth(path_get_depth(*this))
				{
				}
				Path (const std::string& start, std::size_t length) :
					m_path(start, length), m_depth(path_get_depth(m_path))
				{
				}
				bool operator< (const Path& other) const
				{
					return string_less_nocase(*this, other);
				}
				unsigned int depth () const
				{
					return m_depth;
				}
				operator const std::string&() const
				{
					return m_path;
				}
		};

		class Entry
		{
				file_type* m_file;
			public:
				Entry () :
					m_file(0)
				{
				}
				Entry (file_type* file) :
					m_file(file)
				{
				}
				file_type* file () const
				{
					return m_file;
				}
				bool is_directory () const
				{
					return file() == 0;
				}
		};

		typedef std::map<Path, Entry> Entries;
		Entries m_entries;

	public:
		typedef typename Entries::iterator iterator;
		typedef typename Entries::value_type value_type;
		typedef Entry entry_type;

		iterator begin ()
		{
			return m_entries.begin();
		}
		iterator end ()
		{
			return m_entries.end();
		}

		/// \brief Returns the file at \p path.
		/// Creates all directories below \p path if they do not exist.
		/// O(log n) on average.
		entry_type& operator[] (const Path& path)
		{
			std::vector<std::string> vec;
			std::string cleaned("");
			os::getDirectoryComponentsFromPath(path, vec);

			for (std::vector<std::string>::const_iterator i = vec.begin(); i != vec.end(); ++i) {
				cleaned += DirectoryCleaned(*i);
				// And insert it as directory (NULL)
				m_entries.insert(value_type(Path(cleaned), Entry(NULL)));
			}

			return m_entries[path];
		}

		/// \brief Returns the file at \p path or end() if not found.
		iterator find (const Path& path)
		{
			return m_entries.find(path);
		}

		iterator begin (const std::string& root)
		{
			if (root.empty()) {
				return m_entries.begin();
			}
			iterator i = m_entries.find(root);
			if (i == m_entries.end()) {
				return i;
			}
			return ++i;
		}

		/// \brief Performs a depth-first traversal of the file-system subtree rooted at \p root.
		/// Traverses the entire tree if \p root is "".
		/// Calls \p visitor.file() with the path to each file relative to the filesystem root.
		/// Calls \p visitor.directory() with the path to each directory relative to the filesystem root.
		template<typename visitor_type>
		void traverse (visitor_type visitor, const std::string& root)
		{
			unsigned int start_depth = path_get_depth(root);
			unsigned int skip_depth = 0;
			for (iterator i = begin(root); i != end() && i->first.depth() > start_depth; ++i) {
				if (i->first.depth() == skip_depth) {
					skip_depth = 0;
				}
				if (skip_depth == 0) {
					if (!i->second.is_directory()) {
						visitor.file(i->first);
					} else if (visitor.directory(i->first, i->first.depth() - start_depth)) {
						skip_depth = i->first.depth();
					}
				}
			}
		}
};

#endif