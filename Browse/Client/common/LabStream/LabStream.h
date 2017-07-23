//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Handles communication with LabStreamingLayer.

#ifndef LABSTREAM_H_
#define LABSTREAM_H_

#include "externals/liblsl/include/lsl_cpp.h"
#include <thread>
#include <mutex>
#include <string>
#include <vector>

// Forward declaration
/*
namespace lsl
{
	class stream_info;
	class stream_outlet;
}*/

class LabStream
{
public:

	// Constructor
	LabStream(std::string streamOutputName, std::string streamOutputSourceId, std::string streamInputName);

	// Destructor
	virtual ~LabStream();

	// Send event
	void Send(std::string data);

	// Poll received events (clears events)
	std::vector<std::string> Poll();

private:

	// Private copy / asignment constructors
	LabStream(const LabStream&) {}
	LabStream& operator = (const LabStream &) { return *this; }

	// Members
	std::unique_ptr<std::thread> _upReceiverThread;
	std::unique_ptr<lsl::stream_info> _upStreamInfo;
	std::unique_ptr<lsl::stream_outlet> _upStreamOutlet;
	std::mutex _inputMutex;
	std::vector<std::string> _inputBuffer;
};

#endif // LABSTREAM_H_