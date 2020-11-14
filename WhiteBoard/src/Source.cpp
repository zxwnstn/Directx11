#include <cstdlib>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace std;

class session
{
public:
	session(boost::asio::io_service& io_service)
		//boost 1.66이후 (Ubuntu 18.10 이후) 버전의 경우 io_context를 사용
		//session(boost::asio::io_context& io_service)
		: socket_(io_service)
	{
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		//client로부터 연결됨
		cout << "connected" << endl;
		//client로부터 비동기 read 실행
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			boost::bind(&session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

private:
	void handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
	{
		if (!error)
		{
			cout << data_ << endl;
		}
		else
		{
			delete this;
		}
	}

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
};

class server
{
public:
	server(boost::asio::io_service& io_service, short port)
		//boost 1.66이후 (Ubuntu 18.10 이후) 버전의 경우 io_context를 사용
		//server(boost::asio::io_context& io_service, short port)
		: io_service_(io_service),
		//PORT 번호 등록
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		session* new_session = new session(io_service_);
		//client로부터 접속될 때 까지 대기한다.
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}

	//client로부터 접속이 되었을 때 해당 handler 함수를 실행한다.
	void handle_accept(session* new_session,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_session->start();
		}
		else
		{
			delete new_session;
		}
		//client로부터 접속이 끊겼을 대 다시 대기한다.
		start_accept();
	}

	boost::asio::io_service& io_service_;
	//boost 1.66이후 (Ubuntu 18.10 이후) 버전의 경우 io_context를 사용
	//boost::asio::io_context &io_service_;
	tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: async_tcp_echo_server <port>\n";
			return 1;
		}

		boost::asio::io_service io_service;
		//boost 1.66이후 (Ubuntu 18.10 이후) 버전의 경우 io_context를 사용
		//boost::asio::io_context io_service;

		server s(io_service, atoi(argv[1]));
		//asio 통신을 시작한다.
		io_service.run();
	}
	catch (exception& e)
	{
		cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}


