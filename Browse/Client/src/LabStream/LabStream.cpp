//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "LabStream.h"
#include "src/Setup.h"
#include "externals/liblsl/include/lsl_cpp.h"
#include <iostream>
#include <chrono>

LabStream::LabStream()
{
    // Setting up sending
    _upStreamInfo = std::unique_ptr<lsl::stream_info>(
        new lsl::stream_info(
            setup::LAB_STREAM_OUTPUT_NAME,
            "Markers",
            1,
            lsl::IRREGULAR_RATE,
            lsl::cf_int32,
            setup::LAB_STREAM_OUTPUT_SOURCE_ID));
    _upStreamOutlet = std::unique_ptr<lsl::stream_outlet>(new lsl::stream_outlet(*(_upStreamInfo.get())));

    // Setting up receiving
    std::mutex* pMutex = &_inputMutex; // pointer to mutex because reference of private member is not allowed
    std::vector<std::string>* pBuffer = &_inputBuffer; // same for buffer itself
    _upReceiverThread = std::unique_ptr<std::thread>(new std::thread([pMutex, pBuffer]() // pass copies of pointers to members
    {
        // Receive data
        bool connected = false;
        std::vector<lsl::stream_info> streamInfos;
		std::unique_ptr<lsl::stream_inlet> upStreamInlet;
        std::vector<std::string> localBuffer;
        while(true)
        {
            // Build up connection if necessary
            if(!connected)
            {
                streamInfos.clear();
                while (streamInfos.empty())
                {
                    streamInfos = lsl::resolve_stream("name", setup::LAB_STREAM_INPUT_NAME); // search for stream with certain name
                    std::this_thread::sleep_for(std::chrono::seconds(1)); // do it every second
                }

                // Take first stream you can find
                upStreamInlet = std::unique_ptr<lsl::stream_inlet>(new lsl::stream_inlet(streamInfos[0]));
                connected = true;
            }

            // Listen for input
            try
            {
				upStreamInlet->pull_sample(localBuffer);
            }
            catch(lsl::lost_error e)
            {
                // Connection lost, do connection again
                connected = false;
                continue;
            }

            // Write it to shared memory
            pMutex->lock();
            (*pBuffer).insert(pBuffer->end(), localBuffer.begin(), localBuffer.end()); // append values to shared buffer
            pMutex->unlock();

            // Clear local buffer
            localBuffer.clear();
        }
    }));
}

LabStream::~LabStream()
{
    // Nothing to do
}

void LabStream::Send(std::string data)
{
    _upStreamOutlet->push_sample(&data);
}

std::vector<std::string> LabStream::Poll()
{
    // Used as return value
    std::vector<std::string> input;

    // Read values from thread
    _inputMutex.lock();
    input = _inputBuffer;
    _inputBuffer.clear(); // clear shared buffer
    _inputMutex.unlock();

    // Return received values
    return input;
}
