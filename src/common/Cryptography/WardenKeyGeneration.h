/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "CryptoGenerics.h"
#include "CryptoHash.h"

#ifndef _WARDEN_KEY_GENERATION_H
#define _WARDEN_KEY_GENERATION_H

class SHA1Randx
{
public:
    SHA1Randx(uint8* buff, uint32 size)
    {
        uint32 taken = size/2;

        Trinity::Crypto::SHA1 sha1;
        sha1.UpdateData(buff, taken);
        sha1.Finalize();

        memcpy(o1, sha1.GetDigest().data(), 20);

        Trinity::Crypto::SHA1 sha2;
        sha2.UpdateData(buff + taken, size - taken);
        sha2.Finalize();

        memcpy(o2, sha2.GetDigest().data(), 20);

        memset(o0, 0x00, 20);

        FillUp();
    }

    void Generate(uint8* buf, uint32 sz)
    {
        for (uint32 i = 0; i < sz; ++i)
        {
            if (taken == 20)
                FillUp();

            buf[i] = o0[taken];
            taken++;
        }
    }

private:
    void FillUp()
    {
        Trinity::Crypto::SHA1 sh;
        sh.UpdateData(o1, 20);
        sh.UpdateData(o0, 20);
        sh.UpdateData(o2, 20);
        sh.Finalize();

        memcpy(o0, sh.GetDigest().data(), 20);

        taken = 0;
    }
    
    uint32 taken;
    uint8 o0[20], o1[20], o2[20];
};

#endif
