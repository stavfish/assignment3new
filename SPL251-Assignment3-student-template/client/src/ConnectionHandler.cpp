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
    size_t bytesRead = 0;
    boost::system::error_code error;

    try {
        while (bytesRead < bytesToRead) {
            size_t chunk = socket_.read_some(
                boost::asio::buffer(bytes + bytesRead, bytesToRead - bytesRead), error);

            if (error) {
                if (error.message() == "End of file") {
                    return true;
                } else {
                    // Handle other errors
                    throw boost::system::system_error(error);
                }
            }

            bytesRead += chunk;
        }
    } catch (const std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ")" << std::endl;
        return false;
    }

    return true; // Successfully read all requested bytes
}






bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;

    try {
        while (!error && bytesToWrite > tmp) {
            size_t bytesWritten = socket_.write_some(
                boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);

            tmp += bytesWritten;
        }

        if (error) {
            throw boost::system::system_error(error);
        }
    } catch (const std::exception &e) {
        std::cerr << "Send failed (Error: " << e.what() << ")" << std::endl;
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
    try {
        while (true) {
            if (!getBytes(&ch, 1)) {
                std::cerr << "Connection closed or error while reading frame." << std::endl;
                return false; 
            }

            frame += ch;

            if (ch == delimiter) {
                break;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "getFrameAscii failed (Error: " << e.what() << ")" << std::endl;
        return false;
    }

    return true; 
}




bool ConnectionHandler::sendFrameAscii(const std::string &frame, char delimiter) {

    bool result = sendBytes(frame.c_str(), frame.length());
    if (!result) {
        std::cerr << "Failed to send frame content." << std::endl;
        return false;
    }
    return sendBytes(&delimiter, 0);
}

void ConnectionHandler::close() {
	try {
		socket_.close();
	} catch (...) {
		std::cout << "closing failed: connection already closed" << std::endl;
	}
}
