#include "CFramework.h"

const int ReadCount = 15000;
static const char* NPC_Name = "NPC";

// 0x20
struct alignas(32) Entity {
    uint64_t address;
    uint64_t junk[3];
};

struct entitylist_t {
    Entity entity[ReadCount]{};
};

std::vector<CEntity> CFramework::GetEntityList() {
    std::lock_guard<std::mutex> lock(ent_list_mutex);
    return m_vecEntityList;
}

std::vector<std::string> CFramework::GetSpectatorList() {
    std::lock_guard<std::mutex> lock(spec_list_mutex);
    return m_vecSpectatorList;
}

std::vector<uintptr_t> CFramework::GetViewModelList() {
    std::lock_guard<std::mutex> lock(vmodel_list_mutex);
    return m_vecViewModelList;
}

void CFramework::UpdateList()
{
    while (g_ApplicationActive)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(333));

        CEntity local = CEntity();

        // Read EntityList
        auto list_addr = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset::dwEntityList);

        if (list_addr == NULL)
            continue;

        // Gte Local
        local.m_address = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset::dwLocalPlayer);

        if (!local.Update())
            continue;

        local.UpdateStatic();

        auto list = m.Read<entitylist_t>(m.m_dwClientBaseAddr + offset::dwEntityList);
        entitylist_t* pList = &list;

        std::vector<CEntity> vec_entitylist;
        std::vector<std::string> vec_spectatorlist;
        std::vector<uintptr_t> vec_viewmodellist;

        for (int i = 0; i < ReadCount; i++)
        {
            // �����ȃ|�C���^�ł͂Ȃ����ALocal�ł͂͂���.
            if (pList->entity[i].address != NULL && pList->entity[i].address != local.m_address)
            {
                // SignifierName���擾�B�G���e�B�e�B�̎�ޕʂɂ���ŗL�̖��O�݂����Ȃ���.
                char SignifierName[32]{};
                const uintptr_t sig_name_addr = m.Read<uintptr_t>(pList->entity[i].address + offset::m_iSignifierName);

                if (sig_name_addr != NULL)
                {
                    // SignifierName���擾.
                    m.ReadString(sig_name_addr, SignifierName, sizeof(SignifierName));

                    // ViewModel
                    if (strcmp(SignifierName, "viewmodel") == 0) {
                        vec_viewmodellist.push_back(pList->entity[i].address);
                        continue;
                    }

                    // �v���C���[ / �_�~�[.
                    if (strcmp(SignifierName, "player") == 0 || g.ESP_NPC && strcmp(SignifierName, "npc_dummie") == 0)
                    {
                        // �i�[�p��CEntity�C���X�^���X�����A�����i�[���Ă�����.
                        CEntity p = CEntity();
                        p.m_address = pList->entity[i].address;
                        p.m_iSignifierName = SignifierName;
                        p.UpdateStatic(); // �ÓI�ȏ����擾����.

                        if (strcmp(SignifierName, "npc_dummie") == 0)
                            strcpy_s(p.m_szName, NPC_Name);
                           
                        // �ϐ풆��������
                        if (strcmp(SignifierName, "player") == 0 && p.IsSpectator())
                            vec_spectatorlist.push_back(p.m_szName);
                        else if (!p.IsDead()) // �����Ă���.
                            vec_entitylist.push_back(p);
                    }
                }
            }
        }

        std::lock_guard<std::mutex> elock(ent_list_mutex);
        std::lock_guard<std::mutex> slock(spec_list_mutex);
        std::lock_guard<std::mutex> vlock(vmodel_list_mutex);
        m_vecEntityList = vec_entitylist;
        m_vecSpectatorList = vec_spectatorlist;
        m_vecViewModelList = vec_viewmodellist;
    }
}