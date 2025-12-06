#include "CFramework.h"

const int ReadCount{ 15000 };
const char* NPC_Name{ "NPC" };

// 0x20
struct alignas(0x20) entity {
    uint64_t address;
    uint64_t junk[3];
};

struct entitylist_t {
    entity entity[ReadCount]{};
};

void CFramework::UpdateList()
{
    while (g_ApplicationActive)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));

        // Read EntityList
        const uintptr_t EntityList = m.m_dwClientBaseAddr + offset::dwEntityList;
        auto list_addr = m.Read<uintptr_t>(EntityList);

        if (list_addr == NULL)
            continue;

        // Get Local
        CEntity tmpLocal = CEntity();
        uintptr_t pLocalPlayer = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset::dwLocalPlayer);

        if (pLocalPlayer != NULL)
        {
            tmpLocal.m_address = pLocalPlayer;

            if (!tmpLocal.Update())
                continue;

            tmpLocal.UpdateStatic();
        }

        auto list = m.Read<entitylist_t>(EntityList);

        std::vector<CEntity> tmpEntityList;
        std::vector<std::string> tmpSpectator;

        for (int i = 0; i < ReadCount; i++)
        {
            // 無効なポインタではないか、Localでははいか.
            if (list.entity[i].address != NULL && list.entity[i].address != tmpLocal.m_address)
            {
                // SignifierNameを取得。エンティティの種類別にある固有の名前みたいなもの.
                char SignifierName[32]{};
                const uintptr_t sig_name_addr = m.Read<uintptr_t>(list.entity[i].address + offset::m_iSignifierName);

                if (sig_name_addr != NULL)
                {
                    // SignifierNameを取得.
                    m.ReadString(sig_name_addr, SignifierName, sizeof(SignifierName));

                    // プレイヤー / ダミー.
                    if (strcmp(SignifierName, "player") == 0 || g.ESP_NPC && strcmp(SignifierName, "npc_dummie") == 0)
                    {
                        // 格納用にCEntityインスタンスを作り、情報を格納してあげる.
                        CEntity p = CEntity();
                        p.m_address = list.entity[i].address;
                        p.m_iSignifierName = SignifierName;
                        p.UpdateStatic(); // 静的な情報を取得する.

                        if (strcmp(SignifierName, "npc_dummie") == 0)
                            strcpy_s(p.m_szName, NPC_Name);
                           
                        // 観戦中だったら
                        if (strcmp(SignifierName, "player") == 0 && p.IsSpectator()) {
                            std::string result = p.m_szName;
                            result += p.GetObservingTarget(EntityList).m_address == tmpLocal.m_address ? "[ * ]" : "";
                            tmpSpectator.push_back(result);
                        }
                        else if (!p.IsDead()) {
                            tmpEntityList.push_back(p); // 生きてたら
                        } 
                    }
                }
            }
        }

        std::lock_guard<std::mutex> lock(mtx);
        m_CLocal = tmpLocal;
        m_CEntityList = tmpEntityList;
        m_CSpectatorList = tmpSpectator;
    }
}