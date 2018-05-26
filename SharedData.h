#pragma once

#include <memory>
#include <string>
#include <vector>
#include <typeinfo>
#include <utility>

#include "crescent.h"
#include "str_util.h"

class Element
{

public:

	Element(const std::string& name);

	virtual ~Element();

	std::string get_name() const;

protected:

	std::string _name;
};

template <typename T>
class DataElement : public Element
{

public:

	DataElement(const std::string& name)
		: DataElement(name, 0)
	{
	}

	DataElement(const std::string& name, T value)
		: Element(name), _value(value)
	{
	}

	~DataElement()
	{
	}

	T get() const
	{
		return _value;
	}

	void set(const T& value)
	{
		_value = value;
	}

private:

	T _value;
};

class DataAccountant
{

public:

	DataAccountant();

	~DataAccountant();

	template <typename T>
	DataElement<T>& load(int id)
	{
		auto element =
			std::dynamic_pointer_cast<DataElement<T>>(_elements[id]);

		return *element;
	}

	int lookup(const std::string& name);

	int register_element(Handle<Element> element);

private:

	std::vector< Handle<Element> >
		_elements;
};

class DataDirectory
{

public:

	DataDirectory(const std::string& path,
				  Handle<DataAccountant> accountant);

	~DataDirectory();

	template <typename T>
	int create_element(const std::string& _name)
	{
		std::string name = Util::trim(_name);

		if (name.empty()) return -1;

		if (_is_element(_name))
			return get_element_id(_name);

		Handle<Element> elem(new DataElement<T>(name));

		int id = _accountant.register_element(_path + "/" + name);
		if (!elem || id < 0) return -1;

		_elements.push_back(std::move(elem));
		return id;
	}

	int get_element_id(const std::string& name);

	Handle<DataDirectory> lookup(const std::string& path);

	DataDirectory& subdir(const std::string& name);

private:

	int _is_dir(const std::string& _name);

	bool _is_element(const std::string& _name);

	Handle<DataAccountant> _accountant;

	std::vector< Handle<DataDirectory> >
		_directories;

	std::vector< Handle<Element> >
		_elements;

	/**
	 * The path which should backtrack up
	 * to the root
	 */
	std::string _path;
};

class SharedData
{

public:

	SharedData();

	~SharedData();

	template <typename T>
	int create(const std::string& _name)
	{
		std::vector<std::string> tokens;

		Util::split(_name, tokens, "/");

		std::string name = Util::trim(tokens.back());

		if (name.empty()) return -1;

		tokens.pop_back();

		DataDirectory& dir = *_root;

		for (size_t i = 0; i < tokens.size(); i++)
		{
			dir = dir.subdir(tokens[i]);
		}

		return dir.create_element<T>(name);
	}

	Handle<DataDirectory> get_dir(const std::string& path);

	int lookup(const std::string& _name);

	void print() const;

	DataDirectory& root();

	template <typename T>
	DataElement<T> & operator[](int id)
	{
		return _accountant->load<T>(id);
	}

	template <typename T>
	DataElement<T>& operator[](const std::string& name)
	{
		const int id = _accountant->lookup(path);
		return _accountant->load<T>(id);
	}

private:

	Handle<DataAccountant> _accountant;

	Handle<DataDirectory> _root;
};
