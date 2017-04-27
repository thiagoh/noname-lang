//
// See copyright.h for copyright notice and limitation of liability
// and disclaimer of warranty provisions.
//
#include "copyright.h"

#ifndef NONAME_IO_H
#define NONAME_IO_H

//
// noname files include this header to use the standard library's
// IO streams.  
//

//By default use the ANSI standard iostream, etc.
#ifndef  NONAME_USE_OLD_HEADERS

# include <iostream>

using std::ostream;
using std::cout;
using std::cerr;
using std::endl;

# include <fstream>

using std::ofstream;

# include <iomanip>

using std::oct;
using std::dec;
using std::setw;
using std::setfill;

//Including the entire std namespace doesn't work well because of conflicts
//between e.g. std::plus and the plus AST node.
//using namespace std;

#else  
// NONAME_USE_OLD_HEADERS is defined

// I haven't actually tested this, but it ought to work
# include <iostream.h>
# include <fstream.h>
# include <iomanip.h>

#endif // NONAME_USE_OLD_HEADERS


#endif //NONAME_IO_H
