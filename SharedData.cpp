#include "SharedData.h"

/**
 * Constructor
 *
 * @param[in] name The name of this element
 */
Element::Element(const std::string& name)
	: _name(name)
{
}

/**
 * Destructor
 */
Element::~Element()
{
}

/**
 * Get the name of this element
 *
 * @return The element name
 */
std::string Element::get_name() const
{
	return _name;
}

/**
* Get the type of this element
*
* @return The element type
*/
std::string Element::get_type() const
{
	return _type;
}

/**
 * Constructor
 */
DataAccountant::DataAccountant()
{
}

/**
 * Destructor
 */
DataAccountant::~DataAccountant()
{
}

/**
 * Get a registered data element by ID
 *
 * @param [in] id  The unique ID returned by \ref register_element()
 *
 * @return A shared_ptr to the element
 */
Handle<Element> DataAccountant::get_element(int id)
{
	Handle<Element> e;
	AbortIfNot_2((size_t)id < _elements.size(), e);

	e = _elements[id].second;
	return e;
}

/**
 * Look up a previously registered data element
 *
 * @param[in] path The full path to the data element, which includes
 *                 the path used to register the element
 *
 * @return The element ID, or -1 if not found
 */
int DataAccountant::lookup(const std::string& path)
{
	std::string _path = Util::trim_path(path);

	for (size_t i = 0; i < _elements.size(); i++)
	{
		if (_elements[i].first == _path) return i;
	}

	return -1;
}

/**
 * Register a new shared data element
 *
 * @param [in] path    Path to the directory containing this element
 * @param [in] element The element to register
 *
 * @return A unique ID with which to look up this element,
 *         or -1 on error
 */
int DataAccountant::register_element(const std::string& path,
									 Handle<Element> element)
{
	AbortIfNot_2(element, -1);

	const std::string prefix =
		Util::trim_path(path) + "/" + element->get_name();

	int id = lookup(prefix);
	if (id >= 0)
		return id;
	else
		id = _elements.size();

	_elements.push_back(
		std::make_pair(prefix, element));

	return id;
}

/**
 * Constructor
 *
 * @param[in] path       The full path to this directory
 * @param[in] accountant The DataAccountant which manages all created
 *                       data elements. Note that the same
 *                       DataAccountant may be shared across multiple
 *                       directories
 */
DataDirectory::DataDirectory(const std::string& path,
							 Handle<DataAccountant> accountant)
	: _accountant(accountant),
	  _directories(),
	  _elements(),
	  _path(Util::trim_path(path))
{
}

/**
 * Destructor
 */
DataDirectory::~DataDirectory()
{
}

/**
 * Get the ID of an element in this directory with the given name
 *
 * @param[in] name The name of this element
 *
 * @return The element ID, or -1 on error
 */
int DataDirectory::get_element_id(const std::string& name)
{
	AbortIfNot_2(_accountant, -1);
	return _accountant->lookup(_path + "/" + name);
}

/**
 * Get a list of all shared data elements in this directory
 *
 * @param[out] names The names of all elements
 */
void DataDirectory::get_elements(std::vector<std::string>& names) const
{
	names.clear();
	for (size_t i = 0; i < _elements.size(); i++)
	{
		names.push_back(_elements[i]->get_name());
	}
}

/**
 * Get the full path to this directory
 *
 * @return The path
 */
std::string DataDirectory::get_path() const
{
	return _path;
}

/**
 * Get a list of all subdirectories within this directory
 *
 * @param[out] names The names of all subdirectories
 */
void DataDirectory::get_subdirs(std::vector<std::string>& names) const
{
	names.clear();
	for (size_t i = 0; i < _directories.size(); i++)
	{
		const auto& dir = _directories[i];

		std::vector<std::string> tokens;

		Util::split(dir->get_path(), tokens, "/");

		std::string name = Util::trim(tokens.back());

		names.push_back(name);
	}
}

/**
 * Look up a subdirectory with the given path
 *
 * @param [in] path  The path to the directory relative to this one
 *
 * @return A shared_ptr to the desired DataDirectory
 */
Handle<DataDirectory> DataDirectory::lookup(const std::string& path)
{
	std::vector<std::string> tokens;

	Util::split(path, tokens, "/");

	int id = _is_dir(tokens[0]);
	AbortIf(id < 0, Handle<DataDirectory>(),
			"path = %s", path.c_str());

	if (tokens.size() > 1)
	{
		tokens.erase(tokens.begin());

		std::string subpath =
			Util::build_string(tokens, "/");

		auto dir = _directories[id]->lookup(subpath);
		AbortIfNot(dir, dir, "path = %s",
			path.c_str());

		return dir;
	}
	else
		return _directories[id];
}

/**
 * Get a subdirectory
 *
 * @param[in] _name The name of the desired subdirectory. If it does
 *                  not exist, it will be created
 *
 * @return A shared_ptr to the subdirectory
 */
Handle<DataDirectory> DataDirectory::subdir(const std::string& _name)
{
	const std::string name = Util::trim(_name);
	int id = _is_dir(name);

	if (id < 0)
	{
		AbortIfNot_2(_accountant, Handle<DataDirectory>());

		id = _directories.size();

		Handle<DataDirectory> dir(new DataDirectory(_path + "/" + name,
													_accountant));
		AbortIfNot_2(dir, dir);
			_directories.push_back(dir);
	}

	return _directories[id];
}

/**
 * Check if a subdirectory exists
 *
 * @param[in] _name The name of the subdirectory to check for
 *
 * @return True if it exists
 */
int DataDirectory::_is_dir(const std::string& _name)
{
	std::string path =
		_path + "/" + Util::trim(_name);

	for (size_t i = 0; i < _directories.size(); i++)
	{
		if (_directories[i]->_path == path) return i;
	}

	return -1;
}

/**
* Check if a shared data element exists in this directory
*
* @param [in] _name  The name of the element to check for
*
* @return True if it exists
*/
bool DataDirectory::_is_element(const std::string& _name)
{
	return get_element_id(_name) >= 0;
}

/**
 * Constructor
 *
 * @param[in] accountant The DataAccountant which will manage
 *                       all created data elements
 */
SharedData::SharedData(Handle<DataAccountant> accountant)
	: _accountant(accountant),
	  _root(new DataDirectory("root", accountant))
{
}

/**
 * Destructor
 */
SharedData::~SharedData()
{
}

/**
 * Get a directory with the given path relative to the root
 *
 * @param[in] path The path to the desired directory
 *
 * @return a shared_ptr to the directory
 */
Handle<DataDirectory> SharedData::get_dir(const std::string& path)
{
	AbortIfNot_2(_root, _root);

	return _root->lookup(path);
}

/**
 * Get the type of the data element with the given ID
 *
 * @param[in] id The unique ID returned by \ref create()
 *
 * @return The data type
 */
std::string SharedData::get_type(int id)
{
	auto e = _accountant->get_element(id);
	AbortIfNot_2(e, "");

	return e->get_type();
}

/**
 * Look up a data element
 *
 * @param [in] _name  The full path to the data element
 *
 * @return The element ID, or -1 if not found
 */
int SharedData::lookup(const std::string& _name)
{
	std::vector<std::string> tokens;

	Util::split(_name, tokens, "/");

	std::string name = Util::trim(tokens.back());

	AbortIf_2(name.empty(), -1);

	tokens.pop_back();

	Handle<DataDirectory> dir = _root;

	if (dir && tokens.size())
	{
		dir = _root->lookup(Util::build_string(tokens, "/"));
	}

	AbortIfNot_2(dir, -1);

	return
		dir->get_element_id(name);
}

/**
 * Print the entire directory tree to standard output
 */
void SharedData::print() const
{
	_print(0, _root);
	std::fflush(stdout);
}

/**
 * Get the root directory
 *
 * @return A shared_ptr to the root directory
 */
Handle<DataDirectory> SharedData::root()
{
	return _root;
}

/**
 * Print a sub-directory tree
 *
 * @param[in] level The depth of \a dir within the tree
 * @param[in] dir   Print the sub-tree from here
 */
void SharedData::_print(int level,
						Handle<DataDirectory> dir) const
{
	std::vector<std::string> tokens;
	Util::split(dir->get_path(), tokens, "/");

	const std::string name = tokens.back();

	for (int i = 0; i < level; i++)
		std::printf("    ");

	std::printf(" -- %s/\n", name.c_str());

	dir->get_elements(tokens);

	for (size_t i = 0; i < tokens.size(); i++)
	{
		for (int i = 0; i < level+1; i++)
			std::printf("    ");

		std::printf("|   - %s\n", tokens[i].c_str());
	}

	dir->get_subdirs(tokens);

	if (!tokens.empty())
	{
		for ( size_t i = 0; i < tokens.size(); i++ )
		{
			for (int i = 0; i < level + 1; i++)
				std::printf("    ");

			std::printf("|\n");

			_print(level + 1, dir->subdir(tokens[i]));
		}
	}
}
