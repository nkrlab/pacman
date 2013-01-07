// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifdef POSIX
#include <termios.h>
#endif

#include <arpa/inet.h>
#include <json_spirit.h>
#include <boost/asio.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <deque>
#include <string>
#include <utility>
#include <vector>

#include "src/account_messages.pb.h"
#include "src/app_messages_for_client.pb.h"
#include "src/mutex_deque.h"
#include "src/pacman.h"
#include "src/pacman_app.h"
#include "src/pacman_constants.h"
#include "src/pacman_network.h"
#include "src/pacman_render.h"
#include "src/pacman_util.h"

#include "src/util/archive.h"
#include "src/util/json_archive.h"


using boost::asio::ip::tcp;

class PacmanProtocolHandler;


namespace {

boost::asio::io_service *io_service;
tcp::resolver *resolver;
tcp::resolver::query *query;
tcp::resolver::iterator *iterator;
PacmanProtocolHandler *handler = NULL;
boost::thread *boost_thread = NULL;

typedef std::pair<int64_t, int64_t> IntPair;


Uuid session_uuid = { { 0U, 0U, 0U, 0U,
                        0U, 0U, 0U, 0U,
                        0U, 0U, 0U, 0U,
                        0U, 0U, 0U, 0U } };


void PackStringToBuffer(const std::string &strarg,
                        boost::shared_ptr<unsigned char> *arg_buffer,
                        int *arg_length) {
  size_t length_size = sizeof(uint32_t);
  size_t uuid_size = Uuid::static_size();

  uint32_t payload_length = uuid_size + strarg.size();
  uint32_t packed_length = htonl(payload_length);

  *arg_length = length_size + payload_length;

  boost::shared_ptr<unsigned char> buffer(new unsigned char[*arg_length]);
  *arg_buffer = buffer;
  ::memcpy(arg_buffer->get(), &packed_length, sizeof(uint32_t));
  ::memcpy(arg_buffer->get() + length_size, session_uuid.data, uuid_size);
  ::memcpy(arg_buffer->get() + length_size + uuid_size, strarg.c_str(),
           strarg.size());
}

}  // end of anonymous namespace

class PacmanProtocolHandler {
 public:
  PacmanProtocolHandler(boost::asio::io_service *io_service,
                        tcp::resolver::iterator endpoint_iterator)
    : io_service_(io_service), socket_(*io_service) {
    ConnectStart(endpoint_iterator);
    current_buffer_index_ = 0;
    buffer_size_ = 65536;
    read_buffer_ = new char[buffer_size_];
  }
  ~PacmanProtocolHandler() {
  }

  // pass the write data to the do_write function via the io service
  // in the other thread
  void Write(boost::shared_ptr<unsigned char> arg_buffer, int arg_length) {
    io_service_->post(boost::bind(&PacmanProtocolHandler::DoWrite,
                                  this, arg_buffer, arg_length));
  }

  void HandlingAllReceivedPacket() {
    while (deque_.Contains()) {
      const std::string &str = deque_.Front();
      UpdateFromSerializedBuffer(str);
      deque_.Pop();
    }
  }

  // call the do_close function via the io service in the other thread
  void Close() {
    io_service_->post(boost::bind(&PacmanProtocolHandler::DoClose, this));
  }

 private:
  void ConnectStart(tcp::resolver::iterator endpoint_iterator) {
    // asynchronously connect a socket to the specified remote endpoint and
    // call connect_complete when it completes or fails
    tcp::endpoint endpoint = *endpoint_iterator;
    socket_.async_connect(endpoint,
        boost::bind(&PacmanProtocolHandler::ConnectComplete,
                    this,
                    boost::asio::placeholders::error,
                    ++endpoint_iterator));
  }

  void ConnectComplete(const boost::system::error_code& error,
                       tcp::resolver::iterator endpoint_iterator) {
    // the connection to the server has now completed or failed and
    // returned an error
    if (!error) {
      // success, so start waiting for read data
      ReadStart();
    } else if (endpoint_iterator != tcp::resolver::iterator()) {
      // failed, so wait for another connection event
      socket_.close();
      ConnectStart(endpoint_iterator);
    }
  }

  void ReadStart(void) {
    // Start an asynchronous read and call read_complete when it completes or
    // fails
    socket_.async_read_some(boost::asio::buffer(read_msg_, kMaxReadLength),
        boost::bind(&PacmanProtocolHandler::ReadComplete,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void UnpackString() {
    while (current_buffer_index_ > sizeof(uint32_t)) {
      int packed_length;
      ::memcpy(&packed_length, read_buffer_, sizeof(uint32_t));
      size_t payload_length = ntohl(packed_length);
      if (payload_length > (buffer_size_/2)) {
        // payload length가 buffer 크기의 절반보다 크다면 비정상으로 간주함.
        assert(false);
        return;
      }
      if ((payload_length + sizeof(uint32_t)) > current_buffer_index_) {
        // payload를 완벽하게 decoding할 수 없으므로 나중에 다시 시도함.
        return;
      }

      Uuid uuid;
      ::memcpy(uuid.data, read_buffer_ + sizeof(uint32_t),
               Uuid::static_size());

      if (uuid != session_uuid) {
        session_uuid = uuid;
      }

      const char *kBuffer = read_buffer_ + sizeof(uint32_t) +
                            Uuid::static_size();
      const int kLength = payload_length -
                          Uuid::static_size();
      deque_.Push(std::string(kBuffer, kLength));

      int length = payload_length + sizeof(uint32_t);
      ::memmove(read_buffer_, read_buffer_ + length,
                current_buffer_index_ - length);
      current_buffer_index_ -= length;
    }
  }

  void ReadComplete(const boost::system::error_code& error,
                    size_t bytes_transferred) {
    // the asynchronous read operation has now completed or failed and
    // returned an error
    if (!error) {
      // 복사하기 전에 크기 체크함.
      const int kAdditionalRequiredSize = bytes_transferred -
          (buffer_size_-current_buffer_index_);
      if (kAdditionalRequiredSize > 0) {
        // 크기가 부족하다면 큰 버퍼를 만들고
        char* new_buffer = new char[buffer_size_ +
                                    kAdditionalRequiredSize];
        // 새 버퍼로 복사한 후
        ::memcpy(new_buffer, read_buffer_, buffer_size_);
        // 기존 버퍼를 지우고
        delete[] read_buffer_;
        // 새로운 버퍼로 바꾼 후
        read_buffer_ = new_buffer;
        // 크기를 갱신한다.
        buffer_size_ += static_cast<size_t>(kAdditionalRequiredSize);
      }
      ::memcpy(&read_buffer_[current_buffer_index_], read_msg_,
               bytes_transferred);
      current_buffer_index_ += bytes_transferred;

      // read completed, so process the data
      UnpackString();

      // start waiting for another asynchronous read again
      ReadStart();
    } else {
      DoClose();
    }
  }

  void DoWrite(boost::shared_ptr<unsigned char> arg_buffer,
                int arg_length) {
    boost::asio::write(socket_,
        boost::asio::buffer(arg_buffer.get(), arg_length));
  }

  void DoClose() {
    socket_.close();
    if (read_buffer_ != NULL) {
      delete[] read_buffer_;
      read_buffer_ = NULL;
    }
  }

 private:
  enum { kMaxReadLength = 512 };
  // the main IO service that runs this connection
  boost::asio::io_service *io_service_;
  // the socket this instance is connected to
  tcp::socket socket_;
  // data read from the socket
  char read_msg_[kMaxReadLength];
  // data buffer
  char *read_buffer_;
  size_t buffer_size_;
  size_t current_buffer_index_;

  // Message Queue
  fun::util::MutexDeque<std::string> deque_;
};


void NetworkInitialize() {
  io_service = new boost::asio::io_service();
  resolver = new tcp::resolver(*io_service);
  query = new tcp::resolver::query("localhost", "8012");
  iterator = new tcp::resolver::iterator(resolver->resolve(*query));

  // define an instance of the main class of this program
  handler = new PacmanProtocolHandler(io_service, *iterator);

  // run the IO service as a separate thread, so the main thread can block
  // on standard input
  boost_thread = new boost::thread(boost::bind(&boost::asio::io_service::run,
                                               io_service));
}


void SendMessage(const kPacketType kType, const char *string1,
                 const char *string2) {
  // set account message type
  ClientAccountMessage ca_msg;
  switch (kType) {
    case kLogin: {
      ca_msg.set_type(ClientAccountMessage::kAccountLoginRequest);

      // account
      AccountLoginRequest *login = ca_msg.mutable_login();
      login->set_account_id(string1);
      login->set_auth_key(string2);
    }
    break;
  case kMakeRoomGameStart: {
      ca_msg.set_type(ClientAccountMessage::kClientAppMessage);

      // set make room game start type
      ClientAppMessage *msg = ca_msg.mutable_app_message();
      msg->SetExtension(client_message_type,
                        ClientAppMessageType::kMakeRoomGameStart);

      // set level number
      MakeRoomGameStart *room = msg->MutableExtension(make_room_game_start);
      room->set_name(string1);
    }
    break;
  case kLoadLevel:  // handling on other function with different arguments
  case kPacmanMove:
  case kLogout:
  case kShowRoomList:
  case kGameEndLeaveRoom:
    assert(false);
  break;
  }

  // send
  std::string data;
  boost::shared_ptr<unsigned char> buffer;
  int buffer_size;
  ca_msg.SerializeToString(&data);
  PackStringToBuffer(data, &buffer, &buffer_size);
  handler->Write(buffer, buffer_size);
}


void SendMessage(const kPacketType kType, const int value) {
  // set account message type
  ClientAccountMessage ca_msg;
  switch (kType) {
    case kLogin:  // handling on other function with different arguments
    case kMakeRoomGameStart:
      assert(false);
    break;
    case kGameEndLeaveRoom: {
      ca_msg.set_type(ClientAccountMessage::kClientAppMessage);

      // set request tick type
      ClientAppMessage *msg = ca_msg.mutable_app_message();
      msg->SetExtension(client_message_type,
                        ClientAppMessageType::kGameEndLeaveRoom);
    }
    break;
    case kLoadLevel: {
      ca_msg.set_type(ClientAccountMessage::kClientAppMessage);

      // set load level type
      ClientAppMessage *msg = ca_msg.mutable_app_message();
      msg->SetExtension(client_message_type,
                        ClientAppMessageType::kLoadLevel);

      // set level number
      LoadLevel *level = msg->MutableExtension(load_level);
      level->set_level_number(value);
    }
    break;
    case kPacmanMove: {
      ca_msg.set_type(ClientAccountMessage::kClientAppMessage);

      // set pacman move type
      ClientAppMessage *msg = ca_msg.mutable_app_message();
      msg->SetExtension(client_message_type,
                        ClientAppMessageType::kPacmanMove);

      // set pacman direction name
      PacmanMove *move = msg->MutableExtension(pacman_move);
      move->set_pacman_direction(value);
    }
    break;
    case kLogout: {
      ca_msg.set_type(ClientAccountMessage::kAccountLogoutRequest);
    }
    break;
    case kShowRoomList: {
      ca_msg.set_type(ClientAccountMessage::kClientAppMessage);

      // set request tick type
      ClientAppMessage *msg = ca_msg.mutable_app_message();
      msg->SetExtension(client_message_type,
                        ClientAppMessageType::kShowRoomList);
    }
    break;
  }

  // send
  std::string data;
  boost::shared_ptr<unsigned char> buffer;
  int buffer_size;
  ca_msg.SerializeToString(&data);
  PackStringToBuffer(data, &buffer, &buffer_size);
  handler->Write(buffer, buffer_size);
}


void NetworkTerminate() {
  // close the giving_tree client connection
  handler->Close();

  // wait for the IO service thread to close
  boost_thread->join();

  delete boost_thread;
  delete handler;
  delete iterator;
  delete query;
  delete resolver;
  delete io_service;
}


void HandlingReceivedPacket() {
  handler->HandlingAllReceivedPacket();
}

