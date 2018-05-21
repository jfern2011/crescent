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

DataDirectory& DataDirectory::lookup(const std::string& path)
{
	std::vector<std::string> tokens;

	Util::split(path, tokens, "/");

	int id = _is_dir(path);
	if (id < 0)
		return *this;

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
		_directories.push_back(DataDirectory(_path + "/" + name,
											 _accountant));
	}

	return _directories[id];
}

int DataDirectory::_is_dir(const std::string& _name)
{
	std::string path =
		_path + "/" + Util::trim(_name);

	for (size_t i = 0; i < _directories.size(); i++)
	{
		if (_directories[i]._path == path) return i;
	}

	return -1;
}
