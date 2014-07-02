#include "h.h"

#define    COUNTER      10000
#define    PERMS        0660

#define  KEY001    1390415

struct data001
{
  int     id_no;
  char    phone_no[24];
  int     should_pay;
  int     favour_fee;
  int     talk_fee;
  int     times;
  int     duration;
  char    bill_begin[24];
  char    bill_end[24];
  int     fee;
};

