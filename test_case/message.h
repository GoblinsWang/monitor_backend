#ifndef MESSAGE_H
#define MESSAGE_H
typedef struct
{
    int type;
    char data[1024];
} messageObject;

#define MSG_PING 1
#define MSG_PONG 2
#define MSG_TYPE1 11
#define MSG_TYPE2 21

#endif // MESSAGE_H