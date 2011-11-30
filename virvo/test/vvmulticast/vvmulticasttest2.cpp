// Virvo - Virtual Reality Volume Rendering
// Copyright (C) 2010 University of Cologne
// Contact: Jurgen P. Schulze, jschulze@ucsd.edu
//
// This file is part of Virvo.
//
// Virvo is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library (see license.txt); if not, write to the
// Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <pthread.h>

#include "vvsocket.h"
#include "vvsocketio.h"
#include "vvmulticast.h"
#include "vvclock.h"
#include "vvtoolshed.h"
#include "vvdebugmsg.h"

using namespace std;

//-------------------------------------------------------------------
// Threads for parallel TCP-Communication
//-------------------------------------------------------------------
class tcpConnection
{
public:
  tcpConnection(string addr, uchar* data, int size)
  {
    _addr = addr;
    _data = data;
    _size = size;
    _done = false;

    // socket connection
    int delim = addr.find_first_of(":");
    string port = addr.substr(delim+1);
    addr = addr.substr(0, delim);
    _socket = new vvSocketIO(atoi(port.c_str()), addr.c_str(), vvSocket::VV_TCP);

    if(vvSocket::VV_OK == _socket->init())
    {
      _status = true;
      _return = pthread_create( &_thread, NULL, threadMain, this);
    }
    else
    {
      _status = false;
    }
  }

  ~tcpConnection()
  {
    delete _socket;
  }

  static void* threadMain(void* attrib )
  {
    tcpConnection *obj = static_cast<tcpConnection*>(attrib);

    // send data via socket
    if(vvSocket::VV_OK != obj->_socket->write32(obj->_size))
    {
      cout << "Error occured in transfer to " << obj->_addr << "!" << endl;
      return NULL;
    }

    if(vvSocket::VV_OK == obj->_socket->write_data(obj->_data, obj->_size))
    {
      cout << "transfer to " << obj->_addr << " complete!" << endl;
    }
    else
    {
      cout << "Error occured in transfer to " << obj->_addr << endl;
    }

    obj->_done = true;
    return NULL;
  }

  bool       _status;
  bool       _done;
  string     _addr;
  uchar     *_data;
  uint       _size;
  int        _return;
  pthread_t  _thread;
  vvSocketIO *_socket;
};

uchar* generateData(const int size)
{
  // init data to be sent
  cout << "Prepare " << size << " Bytes of random data to be send/checked..." << flush;
  uchar* bar = new uchar[size];

  srand(123456);
  for(int i=0; i<size; i++)
  {
    uchar num = rand() % 256;
    bar[i] = num;
  }
  cout << "done!" << endl;
  return bar;
}

int main(int argc, char** argv)
{
  // don't forget arguments
  if(argc < 2)
  {
    cout << "Start with -sender or -receiver" << endl;
    return 0;
  }

  // ----------------------------------------------------------------
  // Sender
  // ----------------------------------------------------------------
  if(strcmp("-sender", argv[1])== 0)
  {
    cout << "Sender-Mode" << endl;
    cout << "###########" << endl;
    cout << endl;

    // Number of Bytes to be send/received
    uint count = 1024;
    if(NULL != argv[2] && 3 <= argc)
    {
      count = atoi(argv[2]);
    }
    else
    {
      cout << "No number of bytes given. (Default: " << count << ")" << endl;
    }

    uchar* bar = generateData(count);

    // timeout
    double sendTimeout;
    if(NULL != argv[3] && 4 <= argc)
    {
      sendTimeout = atof(argv[3]);
      cout << "Timeout set to " << sendTimeout << endl;
    }
    else
    {
      cout << "No timeout given. (Default: no timeout)" << endl;
      sendTimeout = -1.0;
    }

    double startTime = vvClock::getTime();

    // Send to all servers
    cout << "Try sending to receivers via TCP-Connection!" << endl;
    std::vector<tcpConnection*> servers;
    for(int i=0; i<argc; i++)
    {
      if(0 == strcmp("-server", argv[i]))
      {
        servers.push_back(new tcpConnection(argv[i+1], bar, count));
        if(false == servers.back()->_status)
        {
          return 1;
        }
        i++;
      }
    }
    bool done = false;
    while(!done)
    {
      vvToolshed::sleep(1);

      done = true;
      for(unsigned int i = 0;i<servers.size(); i++)
      {
        if(false == servers[i]->_done)
        {
          done = false;
          break;
        }
      }
    }
    cout << "TCP-Transfers complete!" << endl << endl;
    cout << "Time needed: " << vvClock::getTime() - startTime << endl;

    cout << endl << endl;

    cout << "Receiver ready?" << endl;
    string tmp;
    cin >> tmp;

    // init Multicaster
    vvMulticast foo = vvMulticast("224.1.2.3", 50096, vvMulticast::VV_SENDER, vvMulticast::VV_VVSOCKET);

    cout << "Sending " << count << " Bytes of random numbers..." << flush;
    startTime = vvClock::getTime();
    int written = foo.write(reinterpret_cast<const unsigned char*>(bar), count);
    cout << "sendBytes = " << written << endl;
    cout << "sendTimeout = " << sendTimeout << endl;
    for(unsigned int i = 0;i<servers.size() && count>0; i++)
    {
      char *multidone = new char[5];
      servers[i]->_socket->read_string(multidone, 5);
      if(strcmp("done!", multidone) == 0)
        continue;
      else
        cout << "Server did not finish!"<< endl;

      delete[] multidone;
    }
    cout << "done!" << endl;

    if(written < 0)
      cout << "Error occured!" << endl;
    else
      cout << "Successfully sent " << count << " Bytes!" << endl;

    cout << "Time needed: " << vvClock::getTime() - startTime << endl;
    cout << endl;

    delete[] bar;

    return 0;
  }

  // ----------------------------------------------------------------
  // Receiver
  // ----------------------------------------------------------------
  if(strcmp("-receiver", argv[1])== 0)
  {
    cout << "Receiver-Mode" << endl;
    cout << "#############" << endl;
    cout << endl;

    // timeout
    double receiveTimeout;
    if(NULL != argv[3] && 4 <= argc)
    {
      receiveTimeout = atof(argv[3]);
    }
    else
    {
      cout << "No timeout given. (Default: no timeout)" << endl;
      receiveTimeout = -1.0;
    }

    vvMulticast foo = vvMulticast("224.1.2.3", 50096, vvMulticast::VV_RECEIVER, vvMulticast::VV_VVSOCKET);

    cout << "Waiting for incoming data on TCP..." << endl;

    vvSocketIO recSocket = vvSocketIO(31050, vvSocket::VV_TCP);
    recSocket.init();
    uint tcpSize = recSocket.read32();
    cout << "Expecting " << tcpSize << "Byes of data." << endl;
    uchar* bartcp = new uchar[tcpSize];
    if(vvSocket::VV_OK == recSocket.read_data(bartcp, tcpSize))
    {
      cout << "Successfully received " << tcpSize << "Bytes." << endl;
    }
    else
    {
      cout << "Error for TCP-transfer!" << endl;
    }
    cout << endl;

    uchar* bar = generateData(tcpSize);

    cout << "Waiting for incoming data over Multicasting..." << endl;

    uchar* bartext = new uchar[tcpSize];
    int read = foo.read(bartext, tcpSize);

    // Tell sender, that we are done
    recSocket.write_string("done!");

    if(read >= 0)
    {
      cout << "Received: " << read << endl;
      cout << "Check data for differences...    ";
      for(int i=0; i<tcpSize;i++)
      {
        if(bar[i] != bartext[i])
        {
          cout << "Failed: Differences found!" << endl;
          cout << "i:         " << i           << endl
               << "checkdata: " << bar[i]      << endl
               << "multicast: " << bartext[i]  << endl
               << "tcpdata:   " << bartcp[i]   << endl;
          break;
        }
        else if(i % 1024 == 0)
        {
          cout << "\r" << flush;
          cout << "Check data for differences..." << int(100 * float(i)/float(tcpSize)) << "%" << flush;
        }
      }
      cout << endl;
    }
    else
      cout << "Timeout reached or no data received!" << endl;
    cout << endl;

    delete[] bar;
    delete[] bartext;
    delete[] bartcp;
    return 0;
  }

  cout << "Nothing done..." << endl;

  return 1;
}
