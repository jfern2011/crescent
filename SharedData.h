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
			std::dynamic_pointer_cast<DataElement<T>> _elements[id];

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

		if (name.empty() || _is_element(name))
			return -1;

		Handle<Element> elem(new DataElement<T>(name));

		int id = _accountant.register_element(_path + "/" + name);
		if (!elem || id < 0) return -1;

		_elements.push_back(std::move(elem));
		return id;
	}

	int get_element_id(const std::string& name);

	DataDirectory& lookup(const std::string& path);

	DataDirectory& subdir(const std::string& name);

private:

	bool _is_dir(const std::string& path);

	Handle<DataAccountant> _accountant;

	std::vector<DataDirectory>
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

	int create(const std::string& name);

	int lookup(const std::string& name);

	template<T>
	const DataElement<T>& operator[](int id) const;

	template <T>
	DataElement<T>& operator[](int id);

	template<T>
	const DataElement<T>& operator[](const std::string& path) const;

	template <T>
	DataElement<T>& operator[](const std::string& path);

private:

	DataDirectory & _root;
};
