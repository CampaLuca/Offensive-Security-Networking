// configuration
#define MAX_ALIVE_CONNECTIONS 5
#define MAX_ALIVE_TIME 3*60 // 3 minutes

struct Client{
    char name[100];
	int sockID;
	struct sockaddr_in clientAddr;
    int start_time;
    int quit_flag;
};