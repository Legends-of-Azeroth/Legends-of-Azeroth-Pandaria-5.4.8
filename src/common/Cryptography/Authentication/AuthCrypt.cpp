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

#include <cstring>
#include "AuthCrypt.h"
#include "Cryptography/HMAC.h"

AuthCrypt::AuthCrypt() :
    _initialized(false)
{ }

void AuthCrypt::Init(SessionKey const& K)
{
    uint8 ServerEncryptionKey[] = { 0x08, 0xF1, 0x95, 0x9F, 0x47, 0xE5, 0xD2, 0xDB, 0xA1, 0x3D, 0x77, 0x8F, 0x3F, 0x3E, 0xE7, 0x00 };
    _serverEncrypt.Init(Trinity::Crypto::HMAC_SHA1::GetDigestOf(ServerEncryptionKey, K));

    uint8 ServerDecryptionKey[] = { 0x40, 0xAA, 0xD3, 0x92, 0x26, 0x71, 0x43, 0x47, 0x3A, 0x31, 0x08, 0xA6, 0xE7, 0xDC, 0x98, 0x2A };
    _clientDecrypt.Init(Trinity::Crypto::HMAC_SHA1::GetDigestOf(ServerDecryptionKey, K));

    std::array<uint8, 1024> syncBuf;
    _serverEncrypt.UpdateData(syncBuf);
    _clientDecrypt.UpdateData(syncBuf);

    _initialized = true;
}

void AuthCrypt::DecryptRecv(uint8 *data, size_t len)
{
    ASSERT(_initialized);
    _clientDecrypt.UpdateData(data, len);
}

void AuthCrypt::EncryptSend(uint8 *data, size_t len)
{
    ASSERT(_initialized);
    _serverEncrypt.UpdateData(data, len);
}

