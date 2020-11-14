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
		//boost 1.66���� (Ubuntu 18.10 ����) ������ ��� io_context�� ���
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
		//client�κ��� �����
		cout << "connected" << endl;
		//client�κ��� �񵿱� read ����
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
		//boost 1.66���� (Ubuntu 18.10 ����) ������ ��� io_context�� ���
		//server(boost::asio::io_context& io_service, short port)
		: io_service_(io_service),
		//PORT ��ȣ ���
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		session* new_session = new session(io_service_);
		//client�κ��� ���ӵ� �� ���� ����Ѵ�.
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}

	//client�κ��� ������ �Ǿ��� �� �ش� handler �Լ��� �����Ѵ�.
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
		//client�κ��� ������ ������ �� �ٽ� ����Ѵ�.
		start_accept();
	}

	boost::asio::io_service& io_service_;
	//boost 1.66���� (Ubuntu 18.10 ����) ������ ��� io_context�� ���
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
		//boost 1.66���� (Ubuntu 18.10 ����) ������ ��� io_context�� ���
		//boost::asio::io_context io_service;

		server s(io_service, atoi(argv[1]));
		//asio ����� �����Ѵ�.
		io_service.run();
	}
	catch (exception& e)
	{
		cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}


