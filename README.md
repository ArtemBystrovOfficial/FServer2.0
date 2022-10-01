# FServer2.0 (Cross-platform CMake) <img src ="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white"/>

## Platforms:

<span>
<img src ="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black"/>
<img src ="https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=apple&logoColor=white"/>
<img src ="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white"/>
</span>

## About
**FServer-library** - this library will allow users to `quickly`, `conveniently` and `practically` create their own servers together with the client
Please read documentation of new version
___
Use the library with `maximum` simplicity all you need to communicate with clients a couple of lines of code and something else
```C++
struct Pocket
{
  std::string commad, msg;
}

Server <Pocket> sv("192.128.234.122",1228);
sv.start();

sv << std::pair Out{ Pocket{"Ok","Hello everybody"}, FType::ALL };

sv.stop();
```
## Documentation
Everything you need to know before learning the methods <br/>

- fid - unique id of any connection, all fid start with 1
- gid - group id, you can get new group and return id like discriptor, collect fids
- _Pocket - template of your struct or type for send to other, use this for main logic of app
___

The library is so simple that you don't have to think about the connections and operation of your server, use the commands that have been selected specifically for fast and efficient work. <br/>

Unlike the previous version, the full documentation of which can be read here: <br/>

:spiral_notepad:
[***FServer_doc.docx***](https://docs.google.com/document/d/1XeIXLJ9op7A7yuGaVC4ZtFfJ6ryyomB7/edit?usp=sharing&ouid=114316868734239124935&rtpof=true&sd=true)

The library was written completely rewritten, now it is not possible to send files, but it is possible to use an adapter. Also, there is no more asynchrony, which could cause questions. <br/>

Now your packets will be received as they should, you can absolutely not worry about crashes on the server. <br/>

Library use ASIO and googletest.


## Base methods
### FServer
```C++
	/////////////////////////
	// start and stop listen
	/////////////////////////
				
				  // start your server, now all clients can connect to you
				void start();
				  // stop accept new connections, but other resume do it
				  // delete class for stop all process all exit from programm
				void stop();

	//////////////////
	//	operators
	/////////////////
	
	        // new very easy variant to send your pockets in one stroke with a lot of options,
	        // read documentations
	template < class _Pock >
	friend FServer<_Pock>& operator << (FServer<_Pock>& fserver, const _Out<_Pock>& out);
		// new very easy variant to send your pockets in one stroke with a lot of options,
		//read documentations
	template < class _Pock >
	friend FServer<_Pock>& operator >> (FServer<_Pock>& fserver, std::pair<_Pock,int> & in);

	/////////////////
	//	groups	
	/////////////////
				   // return descriptor of gid, use him to next operations 
				 int newGroup(); 
				   // return descriptor of gid, use him to next operations 
				   // you can make group already with some fids
				 int newGroup(const std::vector<int> & vec_fids);
				   // return false if group doesn't exist or fid exist in group
				bool addToGroup(int gid, int fid);
				   // return false if group doesn't exist or fid exist in group
				bool deleteFromGroup(int gid, int fid);
				   // find all groups by fid
	std::vector<int> findGroupsByFid(int fid);
				   // return list of fids
	std::vector<int> getGroupList(int gid);
				   // return all gids
	std::vector<int> getGroups();

	/////////////////
	// server status
	////////////////
				   // is server listening
				bool isWorking();
				   // is current fid online
				bool isFidOnline(int fid);
				   // list of online users
	std::vector<int> getOnlineList();
				   // return last fid
				int  getMaxFid();
				   // return list Ip of online users
	std::vector < std::pair  < int,std::string > >	
				     getOnlineIpList();
				   // return list of Banned Ip fid -> string(IP)
std::set<std::string>getBanListIp();
				   

	/////////////////			
	// server health
	/////////////////
				   // refresh all sockets who closed not by safety exit ( Recomended to use sometimes )
				void refreshOnline();
				   // reserve to next versions
				void setOptions(int opt);
				   // reserve to next versions
				bool isServerCrash();

	//////////////////
	//server commands
	//////////////////
					// safety disconnect any fid
				bool disconnect(int fid);
					// ban by ip any fid
				void banIp(const std::string & ip);
					// unban by ip any fid
				bool unbanIp(const std::string& ip);
					// return ip by fid if fid not online return ""
				std::string getIpByFid(int fid);

```
_____
### FClient
```C++
	/////////////////////////
	// connect and disconnect
	/////////////////////////

				//return false if bad ip or already connect
			bool connect();
				//return false if not connected
			bool disconect();

	//////////////////
	//	operators
	/////////////////

				// new very easy variant to send your pockets in one stroke with a lot of options,
				// read documentations
			template < class _Pock >
			friend FClient<_Pock>& operator << (FClient<_Pock>& fserver, const _Pock &);
				// new very easy variant to send your pockets in one stroke with a lot of options,
				// read documentations
			template < class _Pock >
			friend FClient<_Pock>& operator >> 
      (FClient<_Pock>& fserver, std::pair <_Pock, typename ReciverSingle<_Pock>::f_error> &);

	/////////////////
	// client status
	////////////////
	
				// return false if client not connected
			bool isConnected();
				// return true if your ip is banned
			bool isBanned();

	//////////////////
	// client health
	//////////////////		

				// if disconnect unsafety, you must refresh by hands
			bool refreshConnection();
```
## Installation
Download repository and include this code to CMake this <br/>
Correlate with your project variables <br/>
```CMake
project(NAME)

include_directories("FServer2_0/source/Client")
include_directories("FServer2_0/source/Server")
include_directories("FServer2_0/libs/asio-1.24.0/include")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(FServer FServer.cpp)
add_executable(FClient FClient.cpp)

target_compile_features(FServer PRIVATE cxx_std_17)
target_compile_features(FClient PRIVATE cxx_std_17)

add_subdirectory(FServer2_0)

target_link_libraries(FServer PRIVATE libFS)
target_link_libraries(FClient PRIVATE libFC)

set(CMAKE_CXX_FLAGS "-pthread")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
```
## Examples of code (Oldest FServer1.0)
- :open_file_folder:
  - :file_folder: [Example 1 (Easy classic server with reg)](https://drive.google.com/drive/folders/1oQyTh73ImCRDRsXq-KhkqRJYRacEwZ3Y?usp=sharing)
  - :file_folder: [Example 2 (Work with files)](https://drive.google.com/drive/folders/1j_NFBW2qQFidXhFTgdaZ7HtJpGm6oNiK?usp=sharing)
  - :file_folder: [Example 3 (Work with HTTP)](https://drive.google.com/drive/folders/1cRHKnJTdg3zkKcDWph3qbWwQp9RB-_c4?usp=sharing)
## Cheme

![plan](https://user-images.githubusercontent.com/92841151/193421180-cd62006d-728a-485f-b5d0-bd137113d293.png)

## Feedback
:email: bystrov.official.one@gmail.com

![logo](https://user-images.githubusercontent.com/92841151/163685103-54875fdc-2b7d-4e54-b73e-6564479622c6.png)
