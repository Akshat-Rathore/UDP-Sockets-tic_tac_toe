#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>

static int count=0;

void clearBoard(char board[3][3])
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            board[r][c] = '.';
}

bool addMove(char board[3][3], char move[], char xo)
{
    int h=move[0]-'A';
    int v=move[1]-'1';
    int size = *(&move + 1) - move;
    if(size>2)
        return false;
    
    for(int i=0;i<2;i++)
    {
        char a =move[0];
        if(a == 'A' || a=='B'|| a=='C')
            {
                char b=move[1];
                if(b=='1' || b=='2' || b=='3')
                {
                    continue;
                }
                else{
                    return false;
                }
            }
        else{
            return false;
        }
    }
    if(board[h][v]=='.')
    {
        // printf("%c",xo);
        board[h][v]=xo;
        return true;
    }
    return false;
    
    
}

void showBoard(char board[3][3])
{
    printf("\n");
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            printf("%c ",board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

bool isWinner(char board[3][3], char xo)
{
    
    //horizontal checks
    for(int i=0;i<3;i++)
    {
        if(board[i][0]==board[i][1] && board[i][1]==board[i][2] )
        {
            if(xo==board[i][2])
                return true;
        }
    }
    //vertical checks
    for(int i=0;i<3;i++)
    {
        if(board[0][i]==board[1][i] && board[0][i]==board[2][i] )
        {
            if(xo==board[2][i])
                return true;
        }
    }
    //diagonal checks
    if(board[0][0]==board[1][1] && board[2][2]==board[1][1] && board[1][1]==xo)
        return true;
    if(board[2][0]==board[1][1] && board[0][2]==board[1][1] && board[1][1]==xo)
        return true;
    return false;
}

bool openListenerPort(const char ipv4Address[], int port){
    int socket_desc;
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ipv4Address);
    return (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0);
}

void server(int argc, char* argv[])
{
    if (!(argc==3))
    {
        return -1;
    }
    char board[3][3];
    clearBoard(board);
    char *ipv4 = malloc(strlen(argv[1]) + 1); 
    strcpy(ipv4,argv[1]);
    char *message = malloc(strlen(argv[2]) + 1); 
    strcpy(message,argv[2]);
    int socket_desc;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000];
    char smove[2],cmove[2];
    int client_struct_length = sizeof(client_addr);
    if(strcmp(message,"accept")!=0)
    {
        return -1;
    }
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    memset(smove, '\0', sizeof(smove));
    memset(cmove, '\0', sizeof(cmove));
    // Create UDP socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");
    
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr(ipv4);
    const char ipv4Address[]="127.0.0.1";
    
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
   
    
    printf("Waiting for invitation\n\n");
    
    
    if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
         (struct sockaddr*)&client_addr, &client_struct_length) < 0){
        printf("Couldn't receive\n");
        return -1;
    }
    
    printf("Invitation Recieved\n");

    while(count<=9)
    {
        
        
        printf("Enter your move(x):");
        gets(smove);
        // printf("%s",smove);
        while(!(addMove(board,smove,'x'))){
            printf("Illegal Move!\nEnter again(x):");
            gets(smove);
        }
        showBoard(board);
        strcpy(server_message, smove);
        if (sendto(socket_desc, server_message, strlen(server_message), 0,
         (struct sockaddr*)&client_addr, client_struct_length) < 0){
            printf("Can't send\n");
            return -1;
        }

        if(isWinner(board,'x'))
            {printf("You (x) won\n");
            return 0;}
        count++;
        if(count==9)
        {
            printf("Draw!\n");
            return 0;
        }
        if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
         (struct sockaddr*)&client_addr, &client_struct_length) < 0){
            printf("Couldn't receive\n");
            return -1;
        }
        memset(cmove, '\0', sizeof(cmove));
        strcpy(cmove, client_message);
        printf("Your opponent (o) moved to ");
        int c=0;
        while (cmove[c] != '\0') {
            printf("%c", cmove[c]);
            c++;
            if(c==2){
                break;
            }
        }
        
        if(addMove(board,cmove,'o'))
        {
            
            showBoard(board);
        }
        else{
            return -1;
        }
        if(isWinner(board,'o'))
            {printf("Your opponent (o) won\n");
            return 0;}

        count++;


        if(count==9){
            printf("Draw!\n");
            return 0;
        }

    }

    close(socket_desc);
}

void client(int argc, char* argv[])
{
    if (!(argc==3))
    {
        return -1;
    }
    char board[3][3];
    clearBoard(board);
    char *ipv4 = malloc(strlen(argv[1]) + 1); 
    strcpy(ipv4,argv[1]);
    char *message = malloc(strlen(argv[2]) + 1); 
    strcpy(message,argv[2]);

    if(strcmp(message,"invite")!=0)
    {
        return -1;
    }

    int socket_desc;
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];
    char smove[2],cmove[2];
    int server_struct_length = sizeof(server_addr);
    
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    memset(smove, '\0', sizeof(smove));
    memset(cmove, '\0', sizeof(cmove));
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    
    
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr(ipv4);

    if(sendto(socket_desc, message, strlen(message), 0,
         (struct sockaddr*)&server_addr, server_struct_length) < 0){
        printf("Unable to send message\n");
        return -1;
    }
    else{
        printf("Sending an invite\n");
    }
    
    while(count<=9)
    {

        if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
         (struct sockaddr*)&server_addr, &server_struct_length) < 0){
            printf("Error while receiving server's msg\n");
            return -1;
        }

        if(addMove(board,server_message,'x'))
        {
            printf("Your opponent (x) moved to %s\n",server_message);
            showBoard(board);
        }
        else{
            printf("error");
            return -1;
        }

        if(isWinner(board,'x'))
            {printf("Your opponent (x) won\n");
            return 0;}
        count++;

        if(count==9)
        {
            printf("Draw!\n");
            return 0;
        }


        printf("Enter your move(o):");
        gets(cmove);
        while(!(addMove(board,cmove,'o'))){
            printf("Illegal Move!\nEnter again(o):");
            gets(cmove);
        }
        showBoard(board);
        strcpy(client_message, cmove);
        if(sendto(socket_desc, client_message, strlen(client_message), 0,
         (struct sockaddr*)&server_addr, server_struct_length) < 0){
            printf("Unable to send message\n");
            return -1;
        }
        
        if(isWinner(board,'o'))
            {printf("You (o) won");
            return 0;}

        count++;


        if(count==9){
            printf("Draw!");
            return 0;
        }

    }

    close(socket_desc);
    
}

int main(int argc, char* argv[])
{
    if(argc!=3)
        return -1;
    if(strcmp(argv[2],"accept")==0)
        server(argc,argv);
    else if(strcmp(argv[2],"invite")==0)
        client(argc,argv);
    else
    printf("Wrong Parameters!");
    return 0;

}


