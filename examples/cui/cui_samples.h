#include <iostream>

#include <thread>
#include <future>
#include <chrono>

#include <stlext/cui/progress_viewer.hpp>
#include <stlext/cui/progress_watcher.hpp>
#include <stlext/cui/noecho.hpp>
#include <stlext/cui/prompt.hpp>

#include "../record.h"

static void progress_viewer_example()
{
	using namespace std;
	using std::chrono::milliseconds;

	/// This example illustrates how to use and customize 
	/// stdx::progress_viewer class for rendering progress bar
	/// in a single-threaded application.

	// Create a progress viewer with range 0...1000 
	// progress bar width 32 chars and a special format.
	// Possible output may look like this:
	// [###                      ] 21% (210/1000) speed: 9 eps ETA: 7 sec
	stdx::progress_viewer<uint64_t> progress(0, 1000, "] %p% (%c/%t) speed: %s eps ETA: %e sec", '#', 32);
	// execute log running computations
	for (size_t i = 0; i <= 1000; i++)
	{
		// Show our progress bar
		// Note that you must ensure that no other thread or function
		// DOES NOT use std::cout in any way to escape clobbering output 
		// of progress bar!
		cout << '[' << progress(i); 
		
		// simulate work -- sleep for 100 milliseconds
		this_thread::sleep_for(milliseconds(10));
	}
	// finally print newline and flush stream
	cout << endl;
}





static void progress_watcher_example()
{
	using namespace std;
	using std::chrono::seconds;

	/// This example illustrates how to use and customize 
	/// stdx::progress_watcher class for rendering progress bar
	/// in a multi-threaded application.

	// Define our problem dimensions
	const size_t lower = 0;
	const size_t upper = 10000;

	// Create a watcher 
	// Every second watcher will update textual representation
	// of current progress using specified format, the length
	// of progress bar is 32 and filling character is '#'
	// Possible output may look like this:
	// [###                    ] 21% (210/1000) speed: 31 eps ETA: 7 sec
	stdx::progress_watcher<uint64_t> watcher(seconds(1), lower, upper, " %p% (%c/%t) speed: %s eps ETA: %e sec", '#', 32);

	// Define a lambda that will be executed in a thread.
	// This lambda provides actual computations.
	// Note that watcher is captured by reference, another way
	// is to use std::ref(watcher) if worker is a free function or class method.
	auto worker = [&watcher](size_t tid, uint64_t lower, uint64_t upper)
	{
		using namespace std;
		using std::chrono::milliseconds;

		// imbue watcher that this thread was started
		++watcher;

		// execute log running computations
		// in a [lower, upper) subrange 
		for (size_t i = lower; i < upper; ++i) {
			if ((i & 0xF) == 0) { 
				// Update progress of current thread
				// N.B. Note that because watcher knows nothing
				// about particular partition of problem we
				// need to update progress in terms of processed
				// items rather than currently processing item index
				watcher[tid] = (i - lower);
			}
			// simulate work -- sleep for 60 milliseconds
			this_thread::sleep_for(milliseconds(30));
		}

		// Update progress of current thread
		watcher[tid] = (upper - lower); 

		// imbue watcher that this thread was finished
		--watcher;
	};

	// get number of logical cores
	size_t nthreads = thread::hardware_concurrency();
	if (nthreads < 2) {
		throw std::runtime_error("insufficent concurrency");
	}

	vector< future<void> > futures;
	
	nthreads -= 1;

	// divide problem into chunks
	size_t chunk_size = upper / nthreads;
	size_t tid = 0;
	for (; tid < nthreads; tid++) 
	{
		// evaluate subrange boundaries
		auto first = tid * chunk_size; 
		auto last = (tid + 1) * chunk_size;
		cout << "launching thread #" << tid << ": [" << first << "..." << last << ']' << endl;

		// start dedicated thread
		futures.emplace_back( std::async(launch::async, worker, tid, first, last) );
	}

	// process the reminder
	if ((upper % nthreads) != 0) {
		auto first = tid * chunk_size;
		auto last = upper;
		cout << "launching thread #" << tid << ": [" << first << "..." << last << ']' << endl;

		// start dedicated thread
		futures.emplace_back( std::async(launch::async, worker, tid, first, last) );
	}

	// Now start watching of overall progress while 
	// render the current state of progress bar.
	// At this point main thread will be blocked
	// until all workers process their chunks of
	// problem.
	watcher(cout);

	// Finally join all threads
	for (auto it = futures.begin(); it != futures.end(); ++it)
		it->get();
	
}




static void prompt_noecho_example()
{
	using namespace std;
	/// This exmample show how to use 
	/// stdx::noecho stream manipulator
	/// and prompt_for() functions

	string text;
	cout << "Please, enter some text: ";
	// disable echo on input
	cin >> stdx::noecho;
	getline(cin, text);
	// enable echo on input
	cin >> stdx::echo;
	cout << "\nYou entered: " << text << endl;


	record rec;
	// prompting for input
	if (!stdx::prompt_for("Enter user ID:     ", rec.id)) {
		cerr << "Incorret input" << endl;
		return;
	}
	if (!stdx::prompt_for("Enter user rating: ", rec.rating)) {
		cerr << "Incorret input" << endl;
		return;
	}
	if (!stdx::prompt_for("Enter user name:   ", rec.name)) {
		cerr << "Incorret input" << endl;
		return;
	}
	
	cout << rec  << '\n' << endl;
}



