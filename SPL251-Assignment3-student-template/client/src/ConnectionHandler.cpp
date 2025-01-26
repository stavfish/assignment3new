#include "../include/ConnectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ConnectionHandler::ConnectionHandler(string host, short port) : host_(host), port_(port), io_service_(),
                                                                socket_(io_service_) {}

ConnectionHandler::~ConnectionHandler() {
	close();
}

bool ConnectionHandler::connect() {
	std::cout << "Starting connect to "
	          << host_ << ":" << port_ << std::endl;
	try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
	}
	catch (std::exception &e) {
		std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t totalBytesRead = 0; // Keep track of the total bytes read
    boost::system::error_code error;

    try {
        while (totalBytesRead < bytesToRead) { // Read until the required number of bytes is read
            size_t bytesRead = socket_.read_some(
                boost::asio::buffer(bytes + totalBytesRead, bytesToRead - totalBytesRead), error);

            if (error) {
                if (error == boost::asio::error::eof) {
                    // EOF encountered: log it and return true if all bytes were read
                    std::cerr << "Connection closed by the server (EOF)." << std::endl;
                    return totalBytesRead == bytesToRead; // Success if all required bytes are already read
                } else {
                    // Handle other errors as fatal
                    throw boost::system::system_error(error);
                }
            }

            totalBytesRead += bytesRead; // Update the total number of bytes read

            // Debug log
            std::cerr << "Bytes read: " << bytesRead 
                      << ", Total read: " << totalBytesRead 
                      << "/" << bytesToRead << std::endl;
        }
    } catch (const std::exception& e) {
        // Log exceptions other than EOF
        std::cerr << "getBytes failed (Error: " << e.what() << ")" << std::endl;
        return false;
    }

    return true; // Successfully read all required bytes
}


bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
	int tmp = 0;
	boost::system::error_code error;
	try {
		while (!error && bytesToWrite > tmp) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
		}
		if (error)
			throw boost::system::system_error(error);
	} catch (std::exception &e) {
		std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::getLine(std::string &line) {
	return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string &line) {
	return sendFrameAscii(line, '\n');
}


bool ConnectionHandler::getFrameAscii(std::string &frame, char delimiter) {
	char ch;
	// Stop when we encounter the null character.
	// Notice that the null character is not appended to the frame string.
	try {
		do {
			if (!getBytes(&ch, 1)) {
				return false;
			}
			if (ch != '\0')
				frame.append(1, ch);
		} while (delimiter != ch);
	} catch (std::exception &e) {
		std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::sendFrameAscii(const std::string &frame, char delimiter) {
	bool result = sendBytes(frame.c_str(), frame.length());
	if (!result) return false;
	return sendBytes(&delimiter, 1);
}

// Close down the connection properly.
void ConnectionHandler::close() {
	try {
		socket_.close();
	} catch (...) {
		std::cout << "closing failed: connection already closed" << std::endl;
	}
}
