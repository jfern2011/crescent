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
	for ( size_t i = 0; i < _elements.size(); i++ )
	{
		if (_elements[i]->get_name() == name)
			return i;
	}

	return -1;
}

int DataAccountant::register_element(Handle<Element> element)
{
	int id = lookup(element->get_name());

	if (id >= 0) return id;

	id = _elements.size();

	_elements.push_back(element);
	return id;
}

DataDirectory::DataDirectory(const std::string& path,
							 Handle<DataAccountant> accountant)
	: _accountant(accountant), _path(path)
{
	if (_path.back() == '/')
		_path.pop_back();
}

DataDirectory::~DataDirectory()
{
}

int DataDirectory::get_element_id(const std::string& name)
{
	return _accountant->lookup(_path + "/" + name);
}

Handle<DataDirectory> DataDirectory::lookup(const std::string& path)
{
	std::vector<std::string> tokens;

	Util::split(path, tokens, "/");

	int id = _is_dir(tokens[0]);
	if (id < 0)
		return Handle<DataDirectory>();

	if (tokens.size() > 1)
	{
		tokens.erase(tokens.begin());

		std::string subpath =
			Util::build_string(tokens, "/");

		return lookup(subpath);
	}
	else
		return _directories[id];
}

DataDirectory& DataDirectory::subdir(const std::string& _name)
{
	const std::string name = Util::trim(_name);
	int id = _is_dir(name);

	if (id < 0)
	{
		id = _directories.size();

		Handle<DataDirectory> dir(new DataDirectory(_path + "/" + name,
													_accountant));
		_directories.push_back(dir);
	}

	return *_directories[id];
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
	return _root->lookup(path);
}

int SharedData::lookup(const std::string& _name)
{
	std::vector<std::string> tokens;

	Util::split(_name, tokens, "/");

	std::string name = Util::trim(tokens.back());

	if (name.empty()) return -1;

	tokens.pop_back();

	auto dir =
		_root->lookup(Util::build_string(tokens, "/"));

	if (!dir) return -1;

	return dir->get_element_id(name);
}

/**
 * Print the entire directory tree
 */
void SharedData::print() const
{
	// TODO
}

DataDirectory& SharedData::root()
{
	return *_root;
}
