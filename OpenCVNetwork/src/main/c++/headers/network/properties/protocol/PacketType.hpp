//
// Created by dialight on 04.11.16.
//

#ifndef OPENCVCLIENT_PACKETTYPE_HPP
#define OPENCVCLIENT_PACKETTYPE_HPP

enum PacketType {
    //in
    GET_ALL_PROPERTIES,
    GET_PROPERTY,
    CHANGE_PROPERTY,
    RESET_PROPERTY,
    PING,
    //out
    ALL_PROPERTIES,
    ADD_PROPERTY,
    REMOVE_PROPERTY,
    PONG,
};

#endif //OPENCVCLIENT_PACKETTYPE_HPP
