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

        // 同クラス内のスナップショット用変数に代入する為の関数
        CGameDataSnapshot next;

        // Read EntityList
        const uintptr_t EntityList = m.m_dwClientBaseAddr + offset::dwEntityList;
        auto list_addr = m.Read<uintptr_t>(EntityList);

        if (list_addr == NULL)
            continue;

        // Local
        uintptr_t pLocalPlayer = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset::dwLocalPlayer);

        if (pLocalPlayer != NULL)
        {
			next.m_localplayer.m_address = pLocalPlayer;

            if (!next.m_localplayer.Update())
                continue;

            next.m_localplayer.UpdateStatic();
        }

        entitylist_t list{};

        // ちゃんと意図したサイズを読み取れているかを確認
        if (!m.SafeRead<entitylist_t>(EntityList, list))
            continue;

        for (int i = 0; i < ReadCount; i++)
        {
            // 無効なポインタではないか、Localでははいか.
            if (list.entity[i].address != NULL && list.entity[i].address != next.m_localplayer.m_address)
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
						CEntity p{ list.entity[i].address };
                        p.m_iSignifierName = SignifierName;
                        p.UpdateStatic(); // 静的な情報を取得する.

                        if (strcmp(SignifierName, "npc_dummie") == 0)
                            strcpy_s(p.m_szName, NPC_Name);
                           
                        // 観戦中だったら
                        if (strcmp(SignifierName, "player") == 0 && p.IsSpectator()) {
                            std::string result = p.m_szName;
                            result += p.GetObservingTarget(EntityList).m_address == next.m_localplayer.m_address ? "[ * ]" : "";
                            next.m_spectatorlist.push_back(result);
                        }
                        else if (!p.IsDead()) {
                            next.m_entitylist.push_back(p); // 生きてたら
                        } 
                    }
                }
            }
        }

        std::lock_guard<std::mutex> lock(m_lock);
        m_NextGameDataSnapshot = next;
    }
}