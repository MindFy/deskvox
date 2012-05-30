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

#ifndef _VV_RESOURCEMANAGER_H_
#define _VV_RESOURCEMANAGER_H_

#ifdef HAVE_CONFIG_H
#include "vvconfig.h"
#endif

#include <pthread.h>
#include <vector>

#include <virvo/vvinttypes.h>

#include <virvo/vvbonjour/vvbonjourentry.h>

// forward declarations
class vvServer;
class vvTcpSocket;

#include <iostream> //temp

/**
 * Class for resouce manager
 *
 * vserver extension for managing additional resources on the network
 *
 * It is capable to detect vservers by its own using bonjour or uses
 * uses a provided list of adresses or ip-ranges to connect with them
 * directly
 *
 * @author Stavros Delisavas (stavros.delisavas@uni-koeln.de)
 */
class vvResourceManager
{
private:
  struct vvRequest
  {
//    vvRequest(uint i)
//    {
//      _id = i;
//    };

//    uint _id;
    int _priority;
    int _type;
    int _requirements;
    vvTcpSocket *_sock;
  };

  struct vvResource
  {
  public:
    vvResource()
    {
      _upToDate = true;
      _server = NULL;
    }

    void numGPUsUp()
    {
      _numGPUs++;
    }

    void numGPUsDown()
    {
      _numGPUs--;
    }

    ushort getGPUs()
    {
      return _numGPUs;
    }

    bool   _upToDate;
    ushort _numCPUs;
    uint   _gpuMemSize;
    uint   _cpuMemSize;
  #ifdef HAVE_BONJOUR
    vvBonjourEntry _bonjourEntry;
  #endif
    vvServer *_server;

  private:
    ushort _numGPUs;
  };

  struct vvJob
  {
    vvTcpSocket *_requestSock;
    vvResource  *_resource;
  };

public:
  static vvResourceManager *inst;
  static void exitCallback(void*)
  {
    inst->exitLocalCallback();
  }


  /**
    Creates a resource manager connected with server
    \param server if set, resource manager also uses this server running locally
    */
  vvResourceManager(vvServer *server = NULL);
  ~vvResourceManager();

  void addJob(vvTcpSocket* sock);         ///< thread-savely adds new connection to the job list
private:
  bool initNextJob();                     ///< thread-savely check for waiting jobs and free resources and pair if possible

  std::vector<vvRequest*>  _requests;
  std::vector<vvResource*> _resources;
  pthread_mutex_t _jobsMutex;
  pthread_mutex_t _resourcesMutex;

  // static thread functions
  static void * updateResources(void *param);
  static void * checkWaitingQueue(void *param);
  static void * processJob(void *param);
  pthread_t               _threadUR;
  pthread_t               _threadCWQ;
  std::vector<pthread_t>  _threadsPJ;

  void exitLocalCallback();
};

#endif // _VV_RESOURCEMANAGER_H_

// vim: sw=2:expandtab:softtabstop=2:ts=2:cino=\:0g0t0