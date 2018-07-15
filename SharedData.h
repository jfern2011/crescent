#pragma once

#include <memory>
#include <string>
#include <vector>
#include <typeinfo>
#include <utility>

#include "abort.h"
#include "crescent.h"
#include "str_util.h"
#include "traits.h"

namespace Crescent
{
	/**
	 * Represents a shared data element
	 */
	class Element
	{

	public:

		Element(const std::string& name);

		virtual ~Element();

		std::string get_name() const;

		std::string get_type() const;

	protected:

		/**
		 * The name of this element
		 */
		std::string _name;

		/**
		 * The type of this element
		 */
		std::string _type;
	};

	/**
	 * Represents a typed shared data element
	 */
	template <typename T>
	class DataElement : public Element
	{

	public:

		/**
		 * Constructor (1)
		 *
		 * @param[in] name The name of this element
		 */
		DataElement(const std::string& name)
			: DataElement(name, T())
		{
		}

		/**
		 * Constructor (2)
		 *
		 * @param[in] name  The name of this element
		 * @param[in] value Its initial value
		 */
		DataElement(const std::string& name, T value)
			: Element(name), _value(value)
		{
			if (Util::is_bool<T>::value)
				_type = "bool";
			else if (Util::is_char<T>::value)
				_type = "char";
			else if (Util::is_int16<T>::value)
				_type = "int16";
			else if (Util::is_int32<T>::value)
				_type = "int32";
			else if (Util::is_int64<T>::value)
				_type = "int64";
			else if (Util::is_uchar<T>::value)
				_type = "uchar";
			else if (Util::is_uint16<T>::value)
				_type = "uint16";
			else if (Util::is_uint32<T>::value)
				_type = "uint32";
			else if (Util::is_uint64<T>::value)
				_type = "uint64";
			else if (Util::is_float<T>::value)
				_type = "float";
			else if (Util::is_double<T>::value)
				_type = "double";
		}

		/**
		 * Destructor
		 */
		~DataElement()
		{
		}

		/**
		 * Get a reference to the current value of this element
		 *
		 * @return The current value
		 */
		T& get()
		{
			return _value;
		}

		/**
		 * Get the current value of this element
		 *
		 * @return The current value
		 */
		T get() const
		{
			return _value;
		}

		/**
		 * Update this element with a fresh value
		 *
		 * @param[in] value The new value
		 */
		void set(const T& value)
		{
			_value = value;
		}

	private:

		/**
		 * The internal value
		 */
		T _value;
	};

	/**
	 * Maintains a record of all shared data elements that were
	 * created. Each element is associated with a unique ID
	 * with which it can be accessed. Elements are created by
	 * using \ref register_element()
	 */
	class DataAccountant
	{

	public:

		DataAccountant();

		~DataAccountant();

		Handle<Element> get_element(int id);

		/**
		 * Load a shared data element
		 *
		 * @param[in] id The unique ID to look up the element
		 *
		 * @return A shared_ptr to the element
		 */
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

		int lookup(const std::string& path);

		int register_element(const std::string& path,
			Handle<Element> element);

	private:

		/**
		 * Maps from element path -> element
		 */
		using str_elem_p =
			std::pair< std::string, Handle<Element> >;

		/**
		 * The record of created elements
		 */
		std::vector<str_elem_p>
			_elements;
	};

	/**
	 * @class DataDirectory
	 *
	 * Represents a "directory" within a larger data structure that
	 * resembles a file system. Normally, each software component creates
	 * its own DataDirectory which serves as its personal workspace and
	 * is the location of its shared data. Once created, this data may be
	 * accessed by downstream components that require it as input. Any
	 * software component may reach into other directories to pull shared
	 * data needed to perform its computations. The paths to these data
	 * elements are built using the "/" delimiter
	 */
	class DataDirectory
	{

	public:

		DataDirectory(const std::string& path,
			Handle<DataAccountant> accountant);

		~DataDirectory();

		/**
		 * Create a new data element in this directory
		 *
		 * @param[in] _name The element name
		 *
		 * @return A unique ID by which to access this element, or
		 *         -1 on error
		 */
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

		/**
		 * Get the element in this directory with the given name
		 *
		 * @param[in] name The name of the element
		 *
		 * @return A shared_ptr to the element
		 */
		template <typename T>
		Handle<DataElement<T>> get_element(const std::string& name)
		{
			return get_element<T>(get_element_id(name));
		}

		/**
		 * Get the element in this directory with the given ID
		 *
		 * @param[in] id The unique ID of the element
		 *
		 * @return A shared_ptr to the element
		 */
		template <typename T>
		Handle<DataElement<T>> get_element(int id)
		{
			return _accountant->load<T>(id);
		}

		int get_element_id(const std::string& name);

		void get_elements(std::vector<std::string>& names) const;

		std::string get_path() const;

		void get_subdirs(std::vector<std::string>& names) const;

		/**
		 * Get a reference to the element in this directory
		 * with the given ID
		 *
		 * @tparam T The type of element returned
		 *
		 * @param[in] id An ID returned by create_element()
		 *
		 * @return The data element
		 */
		template <typename T>
		T& load(int id)
		{
			return _accountant->load<T>(id)->get();
		}

		/**
		 * Get a reference to the element in this directory
		 * with the given name
		 *
		 * @tparam T The type of element returned
		 *
		 * @param[in] _name The name of the element
		 *
		 * @return The data element
		 */
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

		/**
		 * Keeps track of all created data elements
		 */
		Handle<DataAccountant> _accountant;

		/**
		 * Our subdirectories
		 */
		std::vector< Handle<DataDirectory> >
			_directories;

		/**
		 * All data elements created here
		 */
		std::vector< Handle<Element> >
			_elements;

		/**
		 * The path which should backtrack up
		 * to the root
		 */
		std::string _path;
	};

	/**
	 * @class SharedData
	 *
	 * A simple data storage engine which allows different modules
	 * to share data with each other by reading/writing to data
	 * elements that reside within a data structure that resembles
	 * a file system. The underlying implementation is provided
	 * by \ref DataDirectory
	 */
	class SharedData
	{

	public:

		SharedData(Handle<DataAccountant> accountant);

		~SharedData();

		/**
		 * Create a new data element
		 *
		 * @param[in] _name The name of the element. This can also include
		 *                  a path to the element, with directories
		 *                  delimited by "/", e.g. path/to/name. Note that
		 *                  if such a path does not exist, it will be
		 *                  created
		 *
		 * @return A unique ID by which to reference this element,
		 *         or -1 on error
		 */
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

		std::string get_type(int id);

		int lookup(const std::string& _name);

		void print() const;

		Handle<DataDirectory> root();

		/**
		 * Get a reference to the element with the given ID
		 *
		 * @tparam T The type of element returned
		 *
		 * @param[in] id An ID returned by \ref create()
		 *
		 * @return The data element
		 */
		template <typename T>
		T& load(int id)
		{
			return _accountant->load<T>(id)->get();
		}

		/**
		* Get a reference to the element with the given name
		*
		* @tparam T The type of element returned
		*
		* @param[in] name The name of the element
		*
		* @return The data element
		*/
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

		/**
		 * Keeps track of all created elements
		 */
		Handle<DataAccountant> _accountant;

		/**
		 * The root of the data directory tree
		 */
		Handle<DataDirectory> _root;
	};
}
