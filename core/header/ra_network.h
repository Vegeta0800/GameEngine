
struct Data
{
	bool left, right;
	bool shoot;
};

class Network
{
public:
	void InitializeClient();
	void InitializeHost();

	void RecieveHost();
	void SendHost();

	void RecieveClient();
	void SendClient();

	void SetAddr(sockaddr_in addr);

private:
	SOCKET opponentSocket;
	sockaddr_in opponentAddr;

	bool host;
};