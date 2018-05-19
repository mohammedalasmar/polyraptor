//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_RQTAHOERENOFAMILY_H
#define __INET_RQTAHOERENOFAMILY_H

#include "inet/common/INETDefs.h"

#include "RQBaseAlg.h"

namespace inet {

namespace raptorq {

/**
 * State variables for RQTahoeRenoFamily.
 */
class INET_API RQTahoeRenoFamilyStateVariables : public RQBaseAlgStateVariables
{
  public:
    RQTahoeRenoFamilyStateVariables();
    virtual std::string info() const override;
    virtual std::string detailedInfo() const override;
    virtual void setSendQueueLimit(uint32 newLimit);

    uint32 ssthresh;    ///< slow start threshold
};

/**
 * Provides utility functions to implement RQTahoe, RQReno and RQNewReno.
 * (RQVegas should inherit from RQBaseAlg instead of this one.)
 */
class INET_API RQTahoeRenoFamily : public RQBaseAlg
{
  protected:
    RQTahoeRenoFamilyStateVariables *& state;    // alias to RQAlgorithm's 'state'

  public:
    /** Ctor */
    RQTahoeRenoFamily();
};

} // namespace tcp

} // namespace inet

#endif // ifndef __INET_RQTAHOERENOFAMILY_H

