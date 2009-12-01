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

#ifndef _VVGLTOOLS_H_
#define _VVGLTOOLS_H_

#include "vvexport.h"
#include "vvvecmath.h"

//============================================================================
// Class Definitions
//============================================================================

/** Collection of OpenGL raleted tools.
    Consists of static helper functions which are project independent.
    @author Juergen Schulze-Doebold
*/
class VIRVOEXPORT vvGLTools
{
  public:
    enum DisplayStyle                             /// string display style for extensions display
    {
      CONSECUTIVE = 0,                            ///< using entire line length
      ONE_BY_ONE  = 1                             ///< one extension per line
    };
    static void printGLError(const char*);
    static bool isGLextensionSupported(const char*);
    static void displayOpenGLextensions(DisplayStyle);
    static void checkOpenGLextensions();
    static void draw(vvVector3* vec);
};
#endif

//============================================================================
// End of File
//============================================================================
