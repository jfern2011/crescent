#include "SharedData.h"

Element::Element(const std::string& name)
	: _name(name)
{
}

Element::~Element()
{
}

std::string Element::get_name() const
{
	return _name;
}

DataAccountant::DataAccountant()
{
}

DataAccountant::~DataAccountant()
{
}

int DataAccountant::lookup(const std::string& name)
{
	std::string path = Util::trim_path(name);

	for (size_t i = 0; i < _elements.size(); i++)
	{
		if (_elements[i].first == path) return i;
	}

	return -1;
}

int DataAccountant::register_element(const std::string& path,
									 Handle<Element> element)
{
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

DataDirectory::DataDirectory(const std::string& path,
							 Handle<DataAccountant> accountant)
	: _accountant(accountant),
	  _path(Util::trim_path(path))
{
}

DataDirectory::~DataDirectory()
{
}

int DataDirectory::get_element_id(const std::string& name)
{
	AbortIfNot_2(_accountant, -1);
	return _accountant->lookup(_path + "/" + name);
}

void DataDirectory::get_elements(std::vector<std::string>& names) const
{
	names.clear();
	for (size_t i = 0; i < _elements.size(); i++)
	{
		names.push_back(_elements[i]->get_name());
	}
}

std::string DataDirectory::get_path() const
{
	return _path;
}

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

bool DataDirectory::_is_element(const std::string& _name)
{
	return get_element_id(_name) >= 0;
}


SharedData::SharedData() : _accountant(), _root()
{
	_root.reset(new DataDirectory("root", _accountant));
}

SharedData::~SharedData()
{
}

Handle<DataDirectory> SharedData::get_dir(const std::string& path)
{
	AbortIfNot_2(_root, _root);

	return _root->lookup(path);
}

int SharedData::lookup(const std::string& _name)
{
	std::vector<std::string> tokens;

	Util::split(_name, tokens, "/");

	std::string name = Util::trim(tokens.back());

	AbortIf_2(name.empty(), -1);

	tokens.pop_back();

	auto dir =
		_root->lookup(Util::build_string(tokens, "/"));

	AbortIfNot_2(dir, -1);

	return dir->get_element_id(name);
}

/**
 * Print the entire directory tree
 */
void SharedData::print() const
{
	_print(0, _root);
	std::fflush(stdout);
}

Handle<DataDirectory> SharedData::root()
{
	return _root;
}

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
		for ( int i = 0; i < level + 1; i++ )
			std::printf("    ");

		std::printf("|\n");
	}
}
