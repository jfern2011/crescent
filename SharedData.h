#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <typeinfo>
#include <utility>

#include "abort.h"
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
		: DataElement(name, T())
	{
	}

	DataElement(const std::string& name, T value)
		: Element(name), _value(value)
	{
	}

	~DataElement()
	{
	}

	T& get()
	{
		return _value;
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
	Handle<DataElement<T>> load(int id)
	{
		Handle<DataElement<T>> element;

		AbortIfNot_2((size_t)id < _elements.size(),
			element);

		element =
			std::dynamic_pointer_cast<DataElement<T>>(_elements[id].second);

		return element;
	}

	int lookup(const std::string& name);

	int register_element(const std::string& path,
						 Handle<Element> element);

private:

	using str_elem_p =
		std::pair< std::string, Handle<Element> >;

	std::vector<str_elem_p>
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

		AbortIf_2(name.empty(), -1);

		if (_is_element(name))
			return get_element_id(name);

		Handle<Element> elem(new DataElement<T>(name));
		AbortIfNot_2(elem, -1);

		int id = _accountant->register_element(_path, elem);
		AbortIf_2(id < 0, -1);

		_elements.push_back(std::move(elem));

		return id;
	}

	template <typename T>
	Handle<DataElement<T>> get_element(const std::string& name)
	{
		return get_element<T>(get_element_id(name));
	}

	template <typename T>
	Handle<DataElement<T>> get_element(int id)
	{
		return _accountant->load<T>(id);
	}

	int get_element_id(const std::string& name);

	void get_elements(std::vector<std::string>& names) const;

	std::string get_path() const;

	void get_subdirs (std::vector<std::string>& names) const;

	template <typename T>
	T& load(int id)
	{
		return _accountant->load<T>(id)->get();
	}

	template <typename T>
	T& load(const std::string& _name)
	{
		std::string name = Util::trim(_name);

		AbortIf_2(name.empty(), -1);

		if (_is_element(name))
		{
			auto element = _accountant->load<T>(get_element_id(name));

			if (element)
				return element->get();
		}

		Abort(true, T(), "unable to find '%s'",
			name.c_str());
	}

	Handle<DataDirectory>
		lookup(const std::string& path);

	Handle<DataDirectory>
		subdir(const std::string& name);

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

	SharedData(Handle<DataAccountant> accountant);

	~SharedData();

	template <typename T>
	int create(const std::string& _name)
	{
		std::vector<std::string> tokens;

		Util::split(_name, tokens, "/");

		std::string name = Util::trim(tokens.back());

		AbortIf_2(name.empty(), -1);

		tokens.pop_back();

		Handle<DataDirectory> dir = _root;

		for (size_t i = 0; i < tokens.size(); i++)
		{
			dir = dir->subdir(tokens[i]);
			AbortIfNot_2(dir, false);
		}

		return dir->create_element<T>(name);
	}

	Handle<DataDirectory> get_dir(const std::string& path);

	int lookup(const std::string& _name);

	void print() const;

	Handle<DataDirectory> root();

	template <typename T>
	T& load(int id)
	{
		return _accountant->load<T>(id)->get();
	}

	template <typename T>
	T& load(const std::string& name)
	{
		const int id =
			_accountant->lookup(_root->get_path() + "/" + name);
		return _accountant->load<T>(id)->get();
	}

private:

	void _print(int level,
				Handle<DataDirectory> dir) const;

	Handle<DataAccountant> _accountant;

	Handle<DataDirectory> _root;
};
