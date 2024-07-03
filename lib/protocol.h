#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>

#define RR_BUFFER_LEN 128

enum ResourceRecordType {
    None = 0,
    Address,
    CName,
    NameSever,
};

struct ResourceRecordPacket {
    enum ResourceRecordType type;
    uint16_t value_length;
    char value[RR_BUFFER_LEN];
};

struct QueryPacket {
    // -- header --
    uint16_t qid;
    // -- body --
    struct ResourceRecordPacket question;
};

struct ResponsePacket {
    // -- header --
    uint16_t qid;
    // number of resource records to expect
    uint16_t num_answer;
    uint16_t num_authority;
    uint16_t num_additional;
    // -- body --
    struct ResourceRecordPacket question;
    struct ResourceRecordPacket answer[RR_BUFFER_LEN];
    struct ResourceRecordPacket authority[RR_BUFFER_LEN];
    struct ResourceRecordPacket additional[RR_BUFFER_LEN];
};

int send_query(int sockfd, struct sockaddr *addr, struct QueryPacket *query);
int receive_query(int sockfd, struct sockaddr *addr, struct QueryPacket *query);

int send_response(int sockfd, struct sockaddr *addr,
                  struct ResponsePacket *response);
int receive_repsonse(int sockfd, struct sockaddr *addr,
                     struct ResponsePacket *response);

enum ResourceRecordType rr_type_from_str(const char *str);
