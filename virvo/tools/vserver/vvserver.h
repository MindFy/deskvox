// Virvo - Virtual Reality Volume Rendering
// Copyright (C) 1999-2003 University of Stuttgart, 2004-2005 Brown University
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

#ifndef _VV_SERVER_H_
#define _VV_SERVER_H_

#ifdef HAVE_CONFIG_H
#include "vvconfig.h"
#endif

#ifdef HAVE_BONJOUR
#include <virvo/vvbonjour/vvbonjourregistrar.h>
#endif

class vvTcpSocket;
class vvResource;

/**
 * Virvo Server main class.
 *
 * Server unit for remote rendering.
 *
 * Options:
 *  -port:    set port, else default will be used.
 *  -mode:    choose server mode (default: server only)
 *  -bonjour: Use bonjour to broadcast this service (default: off)
 *  -debug:   set debug level, else default will be used.
 *
 * @author Juergen Schulze (schulze@cs.brown.de)
 * @author Stavros Delisavas (stavros.delisavas@uni-koeln.de)
 */
class vvServer
{
public:
  enum ServerRequest
  {
    STATUS,
    RENDERING
  };

  enum ServerMode
  {
    SERVER,
    RM,
    RM_WITH_SERVER
  };

  struct vvServerThreadArgs
  {
    vvTcpSocket *_sock;
    void(*_exitFunc)(void*);
  };

  vvServer();
  ~vvServer();

  /** Main Virvo server routine.
    @param argc,argv command line arguments
    @return 0 if the program finished ok, 1 if an error occurred
  */
  int run(int argc, char** argv);

  void handleClient(vvTcpSocket *sock);           ///< Creates a thread handling the Client
  static void * handleClientThread(void *param);

  static void exitCallback(void*)
  {
  }

private:
  void displayHelpInfo();                         ///< Display command usage help on the command line.
  bool parseCommandLine(int argc, char *argv[]);  ///< Parse command line arguments.
  bool serverLoop();
#ifdef HAVE_BONJOUR
  DNSServiceErrorType registerToBonjour();
  void unregisterFromBonjour();
#endif

  static const int DEFAULTSIZE;   ///< default window size (width and height) in pixels
  static const int DEFAULT_PORT;  ///< default port for socket connections
  unsigned short   _port;         ///< port the server renderer uses to listen for incoming connections
  ServerMode       _sm;           ///< indicating current server mode (default: single server)
  bool             _useBonjour;   ///< indicating the use of bonjour

#ifdef HAVE_BONJOUR
  vvBonjourRegistrar _registrar;  ///< Bonjour registrar used by registerToBonjour() and unregisterFromBonjour()
#endif
};

#endif // _VV_SERVER_H_

//===================================================================
// End of File
//===================================================================
// vim: sw=2:expandtab:softtabstop=2:ts=2:cino=\:0g0t0