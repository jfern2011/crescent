#ifndef __INIT_H__
#define __INIT_H__

#include "config.h"
#include "time.h"
#include "spice.h"

bool initialize(const Time::Epoch& epoch, const Spice::Interface& spice);

#endif // __INIT_H__
