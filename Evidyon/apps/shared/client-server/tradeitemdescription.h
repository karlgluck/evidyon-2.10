#ifndef __TRADEITEMDESCRIPTION_H__
#define __TRADEITEMDESCRIPTION_H__



namespace Evidyon {

  
#pragma pack(push, 1)

// Used to define items in the bazaar, in player-to-player trade,
// that a merchant has for sale
struct TradeItemDescription {
  unsigned __reserved :  8; // will be used for item special properties
  unsigned quantity   :  8;
  unsigned type       : 16;
};

#pragma pack(pop)


}

#endif