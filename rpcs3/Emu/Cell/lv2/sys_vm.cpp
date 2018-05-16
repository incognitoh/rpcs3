#include "stdafx.h"
#include "sys_vm.h"
#include "sys_memory.h"



logs::channel sys_vm("sys_vm");

error_code sys_vm_memory_map(u32 vsize, u32 psize, u32 cid, u64 flag, u64 policy, vm::ptr<u32> addr)
{
	sys_vm.error("sys_vm_memory_map(vsize=0x%x, psize=0x%x, cid=0x%x, flags=0x%llx, policy=0x%llx, addr=*0x%x)", vsize, psize, cid, flag, policy, addr);

	if (!vsize || !psize || vsize % 0x2000000 || vsize > 0x10000000 || psize > 0x10000000 || policy != SYS_VM_POLICY_AUTO_RECOMMENDED)
	{
		return CELL_EINVAL;
	}

	if (cid != SYS_MEMORY_CONTAINER_ID_INVALID && !idm::check<lv2_memory_container>(cid))
	{
		return CELL_ESRCH;
	}

	// Look for unmapped space (roughly)
	for (u32 found = 0x60000000; found <= 0xB0000000; found += 0x2000000)
	{
		// Try to map
		if (const auto area = vm::map(found, 0x10000000, flag))
		{
			// Alloc all memory (shall not fail)
			verify(HERE), area->alloc(0x10000000);

			const auto vm_blocks = fxm::get_always<sys_vm_t>();
			vm_blocks->m_blocks.emplace(found, std::make_shared<sys_vm_block_t>(sys_vm_block_t{ cid, psize }));

			if (cid == SYS_MEMORY_CONTAINER_ID_INVALID)
			{
				const auto dct = fxm::get_always<lv2_memory_container>();
				if (!dct->take(psize))
				{
					return CELL_ENOMEM;
				}
			}
			else
			{
				//todo
			}

			// Write a pointer for the allocated memory
			*addr = found;
			return CELL_OK;
		}
	}

	return CELL_ENOMEM;
}

error_code sys_vm_memory_map_different(u32 vsize, u32 psize, u32 cid, u64 flag, u64 policy, vm::ptr<u32> addr)
{
	sys_vm.warning("sys_vm_memory_map_different(vsize=0x%x, psize=0x%x, cid=0x%x, flags=0x%llx, policy=0x%llx, addr=*0x%x)", vsize, psize, cid, flag, policy, addr);
	// TODO: if needed implement different way to map memory, unconfirmed.

	return sys_vm_memory_map(vsize, psize, cid, flag, policy, addr);
}

error_code sys_vm_unmap(u32 addr)
{
	sys_vm.warning("sys_vm_unmap(addr=0x%x)", addr);

	if (!vm::unmap(addr))
	{
		return CELL_EINVAL;
	}

	const auto vm_blocks = fxm::get_always<sys_vm_t>();
	const auto block = vm_blocks->m_blocks[addr];

	fxm::get_always<lv2_memory_container>()->used -= block->psize;

	vm_blocks->m_blocks.erase(addr);

	return CELL_OK;
}

error_code sys_vm_append_memory(u32 addr, u32 size)
{
	sys_vm.warning("sys_vm_append_memory(addr=0x%x, size=0x%x)", addr, size);

	if (!size || size % 0x100000)
	{
		return CELL_EINVAL;
	}

	const auto vm_blocks = fxm::get_always<sys_vm_t>();
	const auto block = vm_blocks->m_blocks[addr];

	if (!block)
	{
		return CELL_EINVAL;
	}

	if (block->container == SYS_MEMORY_CONTAINER_ID_INVALID)
	{
		const auto dct = fxm::get_always<lv2_memory_container>();
		if (!dct->take(size))
		{
			return CELL_ENOMEM;
		}
	}
	else
	{
		//todo
	}

	block->psize += size;

	return CELL_OK;
}

error_code sys_vm_return_memory(u32 addr, u32 size)
{
	sys_vm.warning("sys_vm_return_memory(addr=0x%x, size=0x%x)", addr, size);

	if (!size || size % 0x100000)
	{
		return CELL_EINVAL;
	}

	const auto vm_blocks = fxm::get_always<sys_vm_t>();
	const auto block = vm_blocks->m_blocks[addr];

	if (!block)
	{
		return CELL_EINVAL;
	}

	if (block->psize < 0x100000 + size)
	{
		return CELL_EBUSY;
	}

	if (block->container == SYS_MEMORY_CONTAINER_ID_INVALID)
	{
		fxm::get_always<lv2_memory_container>()->used -= size;
	}
	else
	{
		//todo
	}

	block->psize -= size;

	return CELL_OK;
}

error_code sys_vm_lock(u32 addr, u32 size)
{
	sys_vm.warning("sys_vm_lock(addr=0x%x, size=0x%x)", addr, size);

	return CELL_OK;
}

error_code sys_vm_unlock(u32 addr, u32 size)
{
	sys_vm.warning("sys_vm_unlock(addr=0x%x, size=0x%x)", addr, size);

	return CELL_OK;
}

error_code sys_vm_touch(u32 addr, u32 size)
{
	sys_vm.warning("sys_vm_touch(addr=0x%x, size=0x%x)", addr, size);

	return CELL_OK;
}

error_code sys_vm_flush(u32 addr, u32 size)
{
	sys_vm.warning("sys_vm_flush(addr=0x%x, size=0x%x)", addr, size);

	return CELL_OK;
}

error_code sys_vm_invalidate(u32 addr, u32 size)
{
	sys_vm.warning("sys_vm_invalidate(addr=0x%x, size=0x%x)", addr, size);

	std::memset(vm::base(addr), 0, size);
	return CELL_OK;
}

error_code sys_vm_store(u32 addr, u32 size)
{
	sys_vm.warning("sys_vm_store(addr=0x%x, size=0x%x)", addr, size);

	return CELL_OK;
}

error_code sys_vm_sync(u32 addr, u32 size)
{
	sys_vm.warning("sys_vm_sync(addr=0x%x, size=0x%x)", addr, size);

	return CELL_OK;
}

error_code sys_vm_test(u32 addr, u32 size, vm::ptr<u64> result)
{
	sys_vm.warning("sys_vm_test(addr=0x%x, size=0x%x, result=*0x%x)", addr, size, result);

	*result = SYS_VM_STATE_ON_MEMORY;

	return CELL_OK;
}

error_code sys_vm_get_statistics(u32 addr, vm::ptr<sys_vm_statistics_t> stat)
{
	sys_vm.warning("sys_vm_get_statistics(addr=0x%x, stat=*0x%x)", addr, stat);

	const auto vm_blocks = fxm::get_always<sys_vm_t>();
	if (const auto block = vm_blocks->m_blocks[addr])
	{
		stat->page_fault_ppu = 0;
		stat->page_fault_spu = 0;
		stat->page_in = 0;
		stat->page_out = 0;
		stat->pmem_total = block->psize;
		stat->pmem_used = 0;
		stat->timestamp = 0;

		return CELL_OK;
	}

	return CELL_EFAULT;
}
