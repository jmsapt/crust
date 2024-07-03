#include <arpa/inet.h>
#include <bits/types/struct_iovec.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "protocol.h"
#define UDP_BUFFER_LEN 4096

// Write resource record into buffer, returning the total size in bytes of the
// resource record
static size_t write_buffer_rr(uint8_t *buffer, struct ResourceRecordPacket rr) {
    size_t length = 0;
    memcpy(&buffer[length], &rr.type, sizeof(rr.type));
    length += sizeof(rr.type);

    memcpy(&buffer[length], &rr.value_length, sizeof(rr.value_length));
    length += sizeof(rr.value_length);

    memcpy(&buffer[length], rr.value, rr.value_length);
    length += rr.value_length;

    return length;
}

static size_t receive_buffer_rr(uint8_t *buffer,
                                struct ResourceRecordPacket *rr) {
    size_t num_bytes = 0;

    memcpy(&rr->type, &buffer[num_bytes], sizeof(rr->type));
    num_bytes += sizeof(rr->type);

    memcpy(&rr->value_length, &buffer[num_bytes], sizeof(rr->value_length));
    num_bytes += sizeof(rr->value_length);

    strncpy(rr->value, (const char *)&buffer[num_bytes], rr->value_length);
    num_bytes += rr->value_length;

    // // insert null terminator (for string operations)
    // rr->value[rr->value_length] = '\0';

    return num_bytes;
}

int send_query(int sockfd, struct sockaddr *addr, struct QueryPacket *query) {
    uint8_t buffer[UDP_BUFFER_LEN];
    socklen_t addrlen = sizeof(*addr);

    // write header
    size_t length = 0;
    memcpy(&buffer[length], &query->qid, sizeof(query->qid));
    length += sizeof(query->qid);

    // write body
    length += write_buffer_rr(&buffer[length], query->question);
    return sendto(sockfd, buffer, length, 0, addr, addrlen);
}

int receive_query(int sockfd, struct sockaddr *addr,
                  struct QueryPacket *query) {
    uint8_t buffer[UDP_BUFFER_LEN];
    socklen_t addrlen = sizeof(*addr);
    size_t num_bytes = 0;

    // recv entire packet, before structuring it
    recvfrom(sockfd, buffer, UDP_BUFFER_LEN, 0, addr, &addrlen);

    // recv header
    memcpy(&query->qid, &buffer[num_bytes], sizeof(query->qid));
    num_bytes += sizeof(query->qid);

    // recv body
    num_bytes += receive_buffer_rr(&buffer[num_bytes], &query->question);

    return num_bytes;
}

int send_response(int sockfd, struct sockaddr *addr,
                  struct ResponsePacket *response) {
    uint8_t buffer[UDP_BUFFER_LEN];
    socklen_t addrlen = sizeof(*addr);
    size_t num_bytes = 0;

    // write header
    size_t length = 0;
    // qid
    memcpy(&buffer[length], &response->qid, sizeof(response->qid));
    length += sizeof(response->qid);
    // num answers
    memcpy(&buffer[length], &response->qid, sizeof(response->qid));
    length += sizeof(response->qid);
    // num authority
    memcpy(&buffer[length], &response->qid, sizeof(response->qid));
    length += sizeof(response->qid);
    // num additional
    memcpy(&buffer[length], &response->qid, sizeof(response->qid));
    length += sizeof(response->qid);

    // write body
    // write question & flush buffer
    length += write_buffer_rr(&buffer[length], response->question);
    sendto(sockfd, buffer, length, 0, addr, addrlen);
    num_bytes += length;

    // write answers & flush buffer
    for (int i = 0, length = 0; i < response->num_answer; i++)
        length += write_buffer_rr(&buffer[length], response->answer[i]);
    sendto(sockfd, buffer, length, 0, addr, addrlen);
    num_bytes += length;

    // write authority & flush buffer
    for (int i = 0, length = 0; i < response->num_authority; i++)
        length += write_buffer_rr(&buffer[length], response->authority[i]);
    sendto(sockfd, buffer, length, 0, addr, addrlen);
    num_bytes += length;

    // write additional & flush buffer
    for (int i = 0, length = 0; i < response->num_additional; i++)
        length += write_buffer_rr(&buffer[length], response->additional[i]);
    sendto(sockfd, buffer, length, 0, addr, addrlen);
    num_bytes += length;

    return num_bytes;
}

// Returns num bytes of packet on success, <0 on timeout
int receive_repsonse(int sockfd, struct sockaddr *addr,
                     struct ResponsePacket *response) {
    uint8_t buffer[UDP_BUFFER_LEN];
    socklen_t addrlen = sizeof(*addr);
    size_t num_bytes = 0;

    // recv entire packet, before structuring it
    if (recvfrom(sockfd, buffer, UDP_BUFFER_LEN, 0, addr, &addrlen) <= 0)
        return -1;

    // recv header
    memcpy(&response->qid, &buffer[num_bytes], sizeof(response->qid));
    num_bytes += sizeof(response->qid);

    memcpy(&response->num_answer, &buffer[num_bytes],
           sizeof(response->num_answer));
    num_bytes += sizeof(response->num_answer);

    memcpy(&response->num_authority, &buffer[num_bytes],
           sizeof(response->num_authority));
    num_bytes += sizeof(response->num_authority);

    memcpy(&response->num_additional, &buffer[num_bytes],
           sizeof(response->num_additional));
    num_bytes += sizeof(response->num_additional);

    // recv body
    // recv question
    num_bytes += receive_buffer_rr(&buffer[num_bytes], &response->question);

    // recv answers
    for (int i = 0; i < response->num_answer; i++)
        num_bytes +=
            receive_buffer_rr(&buffer[num_bytes], &response->answer[i]);

    // recv authority
    for (int i = 0; i < response->num_authority; i++)
        num_bytes +=
            receive_buffer_rr(&buffer[num_bytes], &response->authority[i]);

    // recv additional
    for (int i = 0; i < response->num_additional; i++)
        num_bytes +=
            receive_buffer_rr(&buffer[num_bytes], &response->additional[i]);

    return num_bytes;
}

enum ResourceRecordType rr_type_from_str(const char *str) {
    if (strcmp(str, "A") == 0)
        return Address;
    if (strcmp(str, "CNAME") == 0)
        return CName;
    if (strcmp(str, "NS") == 0)
        return NameSever;

    return None;
}
