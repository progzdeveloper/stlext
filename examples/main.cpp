#include <stlext/options/command_line.hpp>

#include "rt.h"

#include "algorithm/majelem_sample.h"
#include "cui/cui_samples.h"

#include "iostreams/codecs.h"
#include "iostreams/getline.h"
#include "iostreams/byteview.h"
#include "iostreams/iomanips.h"
#include "iostreams/utilities.h"

void setup_cmdline(stdx::command_line& cmdline)
{
	cmdline.emplace("filter=", "filter examples by name using regular expression");
	cmdline.emplace("interactive=", std::string("yes"), "switch to interactive mode");
	cmdline.emplace("list", "list all available examples");
	cmdline.emplace(stdx::option::help());
}

int main(int argc, char* argv[])
{
	using namespace std;

	sample_map samples;
	_EXAMPLE(samples, majority_element, "illustrate how to use and customize stdx::majority_element() algorithm");
	_EXAMPLE(samples, is_majority_element, "illustrate how to use and customize stdx::is_majority_element() algorithm");
	_EXAMPLE(samples, progress_viewer, "illustrate how to use progress_viewer class to show progress of long-running computations");
	_EXAMPLE(samples, progress_watcher, "illustrate how to use progress_watcher class to show progress of concurrently executing long-running computations"); 
	_EXAMPLE(samples, prompt_noecho, "illustrate how to use stdx::noecho manipulator and prompt_for() functions");
	_EXAMPLE(samples, codecs, "illustrate how to use various codes (Base16/Base64) to encode/decode data");
	_EXAMPLE(samples, byte_view, "illustrate how to use byte_viewer class to display byte data in a various forms");
	_EXAMPLE(samples, getline, "illustrate how to use stdx::getline() with stdx::is_any_of() predicates to split input stream using custom separators");
	_EXAMPLE(samples, iomanips, "illustrate how to use I/O manipulators extensions provided by STLEXT library");
	_EXAMPLE(samples, ioutility, "illustrate how to use I/O utilities provided by STLEXT library");

	stdx::command_line cmdline(argc, argv);
	setup_cmdline(cmdline);

	try {
		cmdline();
		string pattern = cmdline["filter"].as<string>();
		unique_ptr<filter> act;
		if (cmdline.count("list") > 0) {
			act.reset(new enumerator(pattern));
		} else {
			act.reset(new executer(pattern, cmdline["interactive"].as<bool>()));
		}
		cout << endl;
		for_each(samples.begin(), samples.end(), std::ref(*act));
	}
	catch (std::exception& e) {
		cerr << "fatal error: " << e.what() << endl;
	}
	catch (...) {
		cerr << "fatal error: unknown error" << endl;
	}
	
	return 0;
}

