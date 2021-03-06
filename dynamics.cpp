// dynamics.cpp : Defines the entry point for the console application.
//

#include <cstdlib>

#include "Vector.h"
#include "SharedData.h"
#include "Simulation.h"
#include "Verbosity.h"

namespace Crescent
{
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
			Handle<DataAccountant> accountant(new DataAccountant());

			SharedData shared(accountant);
			std::string fullpath = "root/orbital/earth/subdir/element2";

			int id = shared.create<int>(fullpath);
			AbortIf_2(id < 0, false);

			AbortIfNot_2(shared.lookup(fullpath) == id, false);

			shared.load<int>(id) = 345;

			AbortIf_2(shared.load<int>(id) != shared.load<int>(fullpath),
				false);

			std::printf("\nshared[%d] = %d\n\n",
				id, shared.load<int>(id));

			shared.create<int>("root/orbital/earth/r_eci.0");
			shared.create<int>("root/orbital/earth/r_eci.1");
			shared.create<int>("root/orbital/earth/r_eci.2");
			shared.create<int>("root/orbital/sun/r_eci.0");
			shared.create<int>("root/orbital/sun/r_eci.1");
			shared.create<int>("root/orbital/sun/r_eci.2");
			shared.create<int>("root/orbital/sun/subdir/blah");
			shared.create<int>("root/blah");
			shared.create<int>("root/orbital/mars/r_eci.0");
			shared.create<int>("root/orbital/mars/r_eci.1");
			shared.create<int>("root/orbital/mars/r_eci.2");

			shared.print();
			return true;
		}
	};

	class Matrix_ut
	{

	public:

		bool run1()
		{
			double data[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

			Matrix<2, 4> mat1(data);
			Matrix<2, 4> mat2(mat1);

			std::printf("mat1 = \n");
			mat1.print();

			std::printf("mat2 = \n");
			mat2.print();

			AbortIfNot_2(mat1 == mat2, false);

			auto sum = mat1 + mat2;
			std::printf("mat1 + mat2 = \n");
			sum.print();

			auto mat3 = mat1.transpose();
			std::printf("mat3 = \n");
			mat3.print();

			auto prod = mat1 * mat3;
			std::printf("mat1 * mat3 = \n");
			prod.print();

			auto diff = mat2 - mat1;
			std::printf("mat2 - mat1 = \n");
			diff.print();

			auto scaled = mat1 * 5;
			std::printf("mat1 * 5 = \n");
			scaled.print();

			scaled = 5 * mat1;
			std::printf("5 * mat1 = \n");
			scaled.print();

			auto div = mat1 / 2;
			std::printf("mat1 / 2 = \n");
			div.print();

			auto sub = mat3.sub<2, 2>(1, 0);
			std::printf("sub = \n");
			sub.print();

			std::fflush(stdout);
			return true;
		}

		bool run2()
		{
			double data[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

			Matrix<1, 8> mat1(data);
			std::printf("mat1 = \n");
			mat1.print();

			auto mat2 = mat1.transpose();
			std::printf("mat2 = \n");
			mat2.print();

			auto mat3 = mat2 * mat1;
			std::printf("mat3 = \n");
			mat3.print();

			mat3 *= mat3;

			std::printf("mat3 *= mat3: \n");
			mat3.print();

			std::fflush(stdout);
			return true;
		}

		bool run3()
		{
			double data[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			Vector<3> accel(data);
			Vector<6> m_i(data);
			Vector<6> m_j(data + 3);

			std::printf("accel = \n");
			accel.print();

			std::printf("m_i = \n");
			m_i.print();

			std::printf("m_j = \n");
			m_j.print();

			accel.zeroify();
			std::printf("accel  = \n");
			accel.print();

			Vector<3> r_ji = m_i.sub<3>(0) - m_j.sub<3>(0);
			std::printf("r_ji = \n");
			r_ji.print();

			const double norm = r_ji.norm();
			std::printf("norm(r_ji) = %f\n", norm);

			accel = 1.0 / pow(norm, 3) * r_ji;
			std::printf("accel = \n");
			accel.print();

			std::fflush(stdout);
			return true;
		}
	};
}

bool create_cmdline_options(CommandLineOptions& options)
{
	AbortIfNot_2(options.add<std::string>(
			"masses_config", "masses", "Path to the masses config file." ),
		false);

	AbortIfNot_2(options.add<std::string>(
		"ephem_config", "ephemeris", "Path to the ephemeris config file."),
		false);

	AbortIfNot_2(options.add<std::string>(
		"telem_config", "telemetry", "Path to the telemetry config file."),
		false);

	AbortIfNot_2(options.add<int>(
		"verbosity", 1, "Verbosity level: 0=quiet 1=terse 2=verbose 3=debug"),
		false);

	AbortIfNot_2(options.add<bool>(
		"disable_telemetry", false, "Disable telemetry output."), false);

	AbortIfNot_2(options.add<bool>(
		"help", false, "Print this help message."), false);

	AbortIfNot_2(options.add<bool>(
		"run_once", false,  "Run the simulation for 1 cycle."), false);

	AbortIfNot_2(options.add<double>(
		"duration", 60.0, "The simulation duration (seconds)"), false);

	AbortIfNot_2(options.add<bool>(
		"realtime", false, "Run in real-time mode."), false);

	return true;
}

bool go(int argc, char** argv)
{
	CommandLineOptions opts;
	AbortIfNot_2(create_cmdline_options(opts), false);

	CommandLine cmd(opts);
	AbortIfNot_2(cmd.parse(argc, argv),
		false);

	bool needs_help;
	AbortIfNot_2( cmd.get( "help", needs_help), false);

	if (needs_help)
	{
		opts.print(argv[0]);
		return true;
	}

	int verbosity;
	AbortIfNot_2(cmd.get("verbosity", verbosity),
		false);

	Crescent::Verbosity::set(verbosity);

	bool run_once;
	AbortIfNot_2(cmd.get("run_once", run_once), false);

	double end_time;
	AbortIfNot_2(cmd.get("duration", end_time), false);

	AbortIf_2(end_time < 0.0, false);

	Crescent::Simulation sim;
	AbortIfNot_2(sim.init(cmd), false);

	Crescent::int64 t_end = 0;
	if (!run_once)
		t_end = Crescent::int64(end_time * 100);

	if (Crescent::Verbosity::level >= Crescent::verbose)
	{
		std::printf( "duration = %lld steps.\n", t_end );
		sim.shared->print();
	}
	
	AbortIfNot_2(sim.go(t_end), false);

	return true;
}

int main(int argc, char** argv)
{
	/*
	SharedData_ut test;
	test.test_DataElement();
	test.test_DataDirectory();
	test.test_SharedData();
	*/

	/*
	Matrix_ut test;
	test.run1();
	test.run2();
	test.run3();
	*/

	AbortIfNot_2(go(argc, argv), EXIT_FAILURE);

    return EXIT_SUCCESS;
}
