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

#ifndef SF_WARDEN_WIN_H
#define SF_WARDEN_WIN_H

#include <map>
#include "Cryptography/ARC4.h"
#include "Cryptography/BigNumber.h"
#include "ByteBuffer.h"
#include "Warden.h"

class WorldSession;

class WardenWin : public Warden
{
    enum class State
    {
        Normal,
        EndSceneRequestStep1,
        EndSceneRequestStep2,
        EndSceneRequestStep3,
        EndSceneRequestStep4,
    };

    public:
        WardenWin();
        ~WardenWin();

        void Init(WorldSession* session, BigNumber* K) override;
        void RequestHash() override;
        void HandleHashResult(ByteBuffer &buff) override;
        void RequestData(WardenRequestContext* context = nullptr) override;
        void HandleData(ByteBuffer &buff) override;

        void Disable() override;

        bool GetEndSceneAddress(int32& addr) override;
        void DoCustomMemCheck(uint32 addr, uint32 len, ObjectGuid guid = ObjectGuid::Empty);
        void HandleEndSceneResult(ByteBuffer& buff, WardenCheck* check);
        void ContinueCheck();

    private:
        uint32 _serverTicks;
        std::list<uint16> _otherChecksTodo;
        std::list<uint16> _memChecksTodo;
        std::list<std::pair<WardenCheck, WardenCheckResult>> _currentChecks;

        State _state = State::Normal;
        bool _endSceneCaptured = false;
        int32 _endSceneAddress = 0;
};

#endif
