all:
	protoc --cpp_out=src --proto_path=src src/account_messages.proto
	protoc --cpp_out=src --proto_path=src src/pacman_client_messages.proto
	protoc --cpp_out=src --proto_path=src src/pacman_server_messages.proto
	protoc --cpp_out=src --proto_path=src src/app_messages_for_client.proto

	g++ 	src/pacman_app.cc \
		src/pacman_render.cc \
		src/pacman_network.cc \
		src/account_messages.pb.cc \
		src/app_messages_for_client.pb.cc \
		src/pacman_client_messages.pb.cc \
		src/pacman_server_messages.pb.cc \
		-I/usr/include -I. \
		-lboost_thread -lboost_system -lprotobuf -ljson_spirit \
		-W -Wall -o pacman -lncurses -g

clean:	
	rm -f pacman