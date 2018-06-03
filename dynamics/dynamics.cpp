// dynamics.cpp : Defines the entry point for the console application.
//

#include "Matrix.h"
#include "SharedData.h"

class SharedData_ut
{

public:

	SharedData_ut()
	{
	}

	~SharedData_ut()
	{
	}

	bool test_DataElement()
	{
		DataElement<int> element1("int_element");
		DataElement<std::string> element2("string_element");

		element1.set(42);
		element2.set("hello");

		std::printf("element 1 = %d, element 2 = '%s'\n",
					element1.get(), element2.get().c_str());

		std::fflush(stdout);
		return true;
	}

	/*
		root/
		|
		 -- orbital/
			|   - element
			|
			 -- sun/
			|   - r_eci.0
			|   - r_eci.1
			|   - r_eci.2
			|
			 -- earth/
				|
				 -- subdir/
					|   - element2
	 */
	bool test_DataDirectory()
	{
		Handle<DataAccountant> accountant(new DataAccountant());

		DataDirectory root("root", accountant);

		auto orbital_dir = root.subdir("orbital");
		AbortIfNot_2(orbital_dir, false);

		auto sun_dir = orbital_dir->subdir("sun");
		AbortIfNot_2(sun_dir, false);

		auto earth_dir = orbital_dir->subdir("earth");
		AbortIfNot_2(earth_dir, false);

		auto earth_subdir = earth_dir->subdir("subdir");
		AbortIfNot_2(earth_subdir, false);

		int element_id = orbital_dir->create_element<int>("element");
		AbortIf_2(element_id < 0, false);

		int r_eci_0_id = sun_dir->create_element<double>("r_eci.0");
		AbortIf_2(r_eci_0_id < 0, false);

		int r_eci_1_id = sun_dir->create_element<double>("r_eci.1");
		AbortIf_2(r_eci_1_id < 0, false);

		int r_eci_2_id = sun_dir->create_element<double>("r_eci.2");
		AbortIf_2(r_eci_2_id < 0, false);

		int element2_id = earth_subdir->create_element<bool>("element2");
		AbortIf_2(element2_id < 0, false);

		std::printf("\nelement_id = %d\n", element_id);
		std::printf("r_eci_0_id = %d\n", r_eci_0_id);
		std::printf("r_eci_1_id = %d\n", r_eci_1_id);
		std::printf("r_eci_2_id = %d\n", r_eci_2_id);
		std::printf("element2_id = %d\n\n", element2_id);

		auto print = [](std::vector<std::string>& names)
		{
			for (auto iter = names.begin(), end = names.end();
				iter != end; ++iter)
			{
				std::printf("%s\n", iter->c_str());
			}
		};

		std::vector<std::string> names;
		root.get_elements(names);

		std::printf("----------------------------------------\n");

		std::printf("%s/ elements:\n", root.get_path().c_str());
		print(names);

		root.get_subdirs(names);
		std::printf("%s/ subdirectories:\n", root.get_path().c_str());
		print(names);

		std::printf("----------------------------------------\n");

		orbital_dir->get_elements(names);
		std::printf("%s/ elements:\n", orbital_dir->get_path().c_str());
		print(names);

		orbital_dir->get_subdirs(names);
		std::printf("%s/ subdirectories:\n", orbital_dir->get_path().c_str());
		print(names);

		std::printf("----------------------------------------\n");

		sun_dir->get_elements(names);
		std::printf("%s/ elements:\n", sun_dir->get_path().c_str());
		print(names);

		sun_dir->get_subdirs(names);
		std::printf("%s/ subdirectories:\n", sun_dir->get_path().c_str());
		print(names);

		std::printf("----------------------------------------\n");

		earth_dir->get_elements(names);
		std::printf("%s/ elements:\n", earth_dir->get_path().c_str());
		print(names);

		earth_dir->get_subdirs(names);
		std::printf("%s/ subdirectories:\n", earth_dir->get_path().c_str());
		print(names);

		std::printf("----------------------------------------\n");

		earth_subdir->get_elements(names);
		std::printf("%s/ elements:\n", earth_subdir->get_path().c_str());
		print(names);

		earth_subdir->get_subdirs(names);
		std::printf("%s/ subdirectories:\n", earth_subdir->get_path().c_str());
		print(names);

		std::printf("----------------------------------------\n");

		auto dir = root.lookup("orbital/earth/subdir");
		AbortIfNot_2(dir, false);
		std::printf("lookup() = %s\n", dir->get_path().c_str());

		dir = root.lookup("orbital/earth/");
		AbortIfNot_2(dir, false);
		std::printf("lookup() = %s\n", dir->get_path().c_str());

		dir = root.lookup("/orbital/");
		AbortIfNot_2(dir, false);
		std::printf("lookup() = %s\n", dir->get_path().c_str());

		/*
		 * check the accountant:
		 */
		AbortIf_2(accountant->lookup(
			"root/orbital/element") != element_id, false);

		AbortIf_2(accountant->lookup(
			"root/orbital/sun/r_eci.0") != r_eci_0_id, false);

		AbortIf_2(accountant->lookup(
			"root/orbital/sun/r_eci.1") != r_eci_1_id, false);

		AbortIf_2(accountant->lookup(
			"root/orbital/sun/r_eci.2") != r_eci_2_id, false);

		AbortIf_2(accountant->lookup(
			"root/orbital/earth/subdir/element2") != element2_id, false);

		accountant->load<int>(element_id)->set(123);
		accountant->load<double>(r_eci_0_id)->set(2.71828182846);
		accountant->load<double>(r_eci_1_id)->set(3.14159265359);
		accountant->load<double>(r_eci_2_id)->set(1.61803398875);
		accountant->load<bool>(element2_id)->set(true);

		auto element_data = accountant->load<int>(element_id);
		auto r_eci_0_data = accountant->load<double>(r_eci_0_id);
		auto r_eci_1_data = accountant->load<double>(r_eci_1_id);
		auto r_eci_2_data = accountant->load<double>(r_eci_2_id);
		auto element2_data = accountant->load<bool>(element2_id);

		std::printf("\nelement  = %d\n", element_data->get());
		std::printf("r_eci_0  = %f\n", r_eci_0_data->get());
		std::printf("r_eci_1  = %f\n", r_eci_1_data->get());
		std::printf("r_eci_2  = %f\n", r_eci_2_data->get());
		std::printf("element2 = %s\n",
			element2_data.get() ? "true" : "false");

		std::fflush(stdout);
		return true;
	}

	bool test_SharedData()
	{
		SharedData shared;
		//int id = shared.create("root/orbital/earth/subdir/element2");
		//AbortIf_2(id < 0, false);

		//AbortIfNot_2(shared.lookup())
		return true;
	}
};

int main(int argc, char** argv)
{
	SharedData_ut test;
	test.test_DataElement();
	test.test_DataDirectory();

    return 0;
}
