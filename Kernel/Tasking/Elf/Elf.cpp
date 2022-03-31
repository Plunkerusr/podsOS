#include "Elf.hpp"
#include "Elfstructs.hpp"

#include <FileSystem/VFS/VFS.hpp>
#include <Libkernel/KError.hpp>
#include <Memory/Region.hpp>
#include <Memory/vmm.hpp>

#include <Macaronlib/ABI/Syscalls.hpp>
#include <Macaronlib/Memory.hpp>
#include <Macaronlib/String.hpp>

namespace Kernel::Tasking {

using namespace Memory;

KErrorOr<Elf::ExecData> Elf::load_exec(const String& exec_path, uint32_t page_directory)
{
    auto& vfs = FileSystem::VFS::the();
    auto& vmm = VMM::the();

    auto buffer = vfs.read_entire_file(exec_path);
    if (!buffer) {
        return KError(ENOENT);
    }

    auto header = (ElfHeader*)buffer;

    if (
        header->ident[0] != 0x7f || header->ident[1] != 'E' || header->ident[2] != 'L' || header->ident[3] != 'F') {
        free(buffer);
        return KError(ENOEXEC);
    }

    auto ph_table = (ElfProgramHeader*)(buffer + header->phoff);

    vmm.set_page_directory(page_directory);

    ExecData exec_data;

    for (size_t i = 0; i < header->phnum; i++) {
        if (ph_table[i].type == static_cast<uint32_t>(ElfProgramHeaderType::LOAD)) {

            size_t pages_count = (ph_table[i].memsz + ph_table[i].vaddr % PAGE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;

            vmm.psized_allocate_space_from(
                page_directory,
                ph_table[i].vaddr / 4096,
                pages_count,
                Flags::User | Flags::Write | Flags::Present);

            exec_data.regions.push_back({
                .type = Region::Type::Allocated,
                .page = ph_table[i].vaddr / 4096,
                .pages = pages_count,
                .flags = Flags::User | Flags::Write | Flags::Present,
            });

            memcpy((void*)ph_table[i].vaddr, (void*)(buffer + ph_table[i].offset), ph_table[i].filesz);
            memset((void*)(ph_table[i].vaddr + ph_table[i].filesz), 0, ph_table[i].memsz - ph_table[i].filesz);
        }
    }

    exec_data.entry_point = (uint32_t)header->entry;
    free(buffer);
    return exec_data;
}

}