
#include <librealsense2/rs.hpp>
#include <iostream>
#include <iomanip>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#include <osc/OscOutboundPacketStream.h>
#include <ip/UdpSocket.h>

#include <string>
#include <fstream>
#include <iostream>

#define OUTPUT_BUFFER_SIZE 1024

std::string address;
std::string port;

void readConfig(char* fn) {
	std::fstream fs;
	fs.open(fn, std::fstream::in);

	if (!fs.is_open()) {
		std::cout << "Could not open configuration file " << fn << "\n";
	}

	std::string line;
	while (getline(fs, line)) {
		std::stringstream ss(line);
		std::string str;
		ss >> str;
		if (str == "[port]") {
			ss >> port;
		}
		else if (str == "[address]") {
			ss >> address;
		}
	}
}


int main(int argc, char * argv[]) try {

	readConfig("config.txt");
	UdpTransmitSocket transmitSocket(IpEndpointName(address.c_str(), atoi(port.c_str())));
	char buffer[OUTPUT_BUFFER_SIZE];


    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    
	// Create a configuration for configuring the pipeline with a non default profile
    rs2::config cfg;
    
	// Add pose stream
    cfg.enable_stream(RS2_STREAM_POSE, RS2_FORMAT_6DOF);

	// Start pipeline with chosen configuration
    pipe.start(cfg);

    // Main loop
    while (true) {

        // Wait for the next set of frames from the camera
        auto frames = pipe.wait_for_frames();

        // Get a frame from the pose stream
        auto f = frames.first_or_default(RS2_STREAM_POSE);

		// Cast the frame to pose_frame and get its data
        auto pose_data = f.as<rs2::pose_frame>().get_pose_data();

        // Print the x, y, z values of the translation, relative to initial position
        std::cout	<< "\r"
					<< "Device State: "
					<< std::setprecision(3) 
					<< std::fixed 
					<< pose_data.translation.x 
					<< " "
					<< pose_data.translation.y
					<< " " 
					<< pose_data.translation.z 
					// << " (meters)"
				    << " "
					<< pose_data.rotation.w
					<< " "
					<< pose_data.rotation.x
					<< " "
					<< pose_data.rotation.y
					<< " "
					<< pose_data.rotation.z;

		osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
		p << osc::BeginBundleImmediate
			<< osc::BeginMessage("/realsense")
			<< pose_data.translation.x
			<< pose_data.translation.y
			<< pose_data.translation.z
			<< pose_data.rotation.w
			<< pose_data.rotation.x
			<< pose_data.rotation.y
			<< pose_data.rotation.z
			<< osc::EndMessage
			<< osc::EndBundle;
		transmitSocket.Send(p.Data(), p.Size());

	}

    return EXIT_SUCCESS;
}

catch (const rs2::error & e) {
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}

catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
