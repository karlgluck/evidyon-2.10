// This file defines settings defining how clients are managed by the
// server.  This information is potentially used in several places
// to optimize network usage.  For example, the SlowSync message is
// set several times per second, but only to certain groups of clients
// at a time.  If this were not the case, the network usage would be
// very bursty.  However, this complicates SlowSync since there are
// flags in those messages that need to be rotated on a per-sync per-
// group basis.
#ifndef __CLIENTCONFIG_H__
#define __CLIENTCONFIG_H__


namespace Evidyon {
namespace ClientConfig {
static const int SYNC_GROUPS = 5;
}
}

#endif