/**
 * Copyright 2013-2015 Seagate Technology LLC.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at
 * https://mozilla.org/MP:/2.0/.
 *
 * This program is distributed in the hope that it will be useful,
 * but is provided AS-IS, WITHOUT ANY WARRANTY; including without
 * the implied warranty of MERCHANTABILITY, NON-INFRINGEMENT or
 * FITNESS FOR A PARTICULAR PURPOSE. See the Mozilla Public
 * License for more details.
 *
 * See www.openkinetic.org for more project information
 */

#ifndef KINETIC_CPP_CLIENT_MESSAGE_STREAM_H_
#define KINETIC_CPP_CLIENT_MESSAGE_STREAM_H_

#include "google/protobuf/message.h"
#include "openssl/ssl.h"

#include "byte_stream.h"
#include "common.h"
#include "incoming_value.h"

namespace kinetic {

class MessageStreamInterface {
    public:
    typedef enum {
        MessageStreamReadStatus_SUCCESS,
        MessageStreamReadStatus_INTERNAL_ERROR,
        MessageStreamReadStatus_TOO_LARGE
    } MessageStreamReadStatus;

    virtual ~MessageStreamInterface() {}
    virtual MessageStreamReadStatus ReadMessage(::google::protobuf::Message *message,
        IncomingValueInterface** value) = 0;
    virtual int WriteMessage(const ::google::protobuf::Message &message,
        const OutgoingValueInterface& value, int* err) = 0;
};

class MessageStream : public MessageStreamInterface {
    public:
    explicit MessageStream(uint32_t max_message_size_bytes, ByteStreamInterface *byte_stream);
    ~MessageStream();
    MessageStreamReadStatus ReadMessage(::google::protobuf::Message *message,
        IncomingValueInterface** value);
    int WriteMessage(const ::google::protobuf::Message &message,
        const OutgoingValueInterface& value, int* err);

    private:
    bool ReadHeader(uint32_t *message_size, uint32_t *value_size);
    bool WriteHeader(uint32_t message_size, uint32_t value_size);
    uint32_t max_message_size_bytes_;
    ByteStreamInterface *byte_stream_;
    DISALLOW_COPY_AND_ASSIGN(MessageStream);
};

class MessageStreamFactoryInterface {
    public:
    virtual bool NewMessageStream(int fd, bool use_ssl, SSL *ssl, uint32_t max_message_size_bytes,
        MessageStreamInterface **message_stream) = 0;
    virtual ~MessageStreamFactoryInterface() {}
};

class MessageStreamFactory : public MessageStreamFactoryInterface {
    public:
    MessageStreamFactory(SSL_CTX *ssl_context, IncomingValueFactoryInterface &value_factory);
    bool NewMessageStream(int fd, bool use_ssl, SSL *ssl, uint32_t max_message_size_bytes,
        MessageStreamInterface **message_stream);
    ~MessageStreamFactory();

    private:
    SSL_CTX *ssl_context_;
    SSL *ssl_;
    bool ssl_created_;
    IncomingValueFactoryInterface &value_factory_;
    DISALLOW_COPY_AND_ASSIGN(MessageStreamFactory);
};

} // namespace kinetic

#endif  // KINETIC_CPP_CLIENT_MESSAGE_STREAM_H_
