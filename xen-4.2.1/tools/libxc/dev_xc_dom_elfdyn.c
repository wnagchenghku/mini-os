#include <inttypes.h>

#include "xg_private.h"
#include "xc_dom.h"
#include "xc_bitops.h"
#include "xg_private.h"
#include "xg_save_restore.h"
#include "dev_xc_dom_elfdyn.h"

struct link_map {
    Elf32_Addr l_addr;		/* Base (or delta?) domU address shared object is loaded at.  */
    Elf32_Dyn *l_ld;		/* Dynamic section of the shared object.  */
    Elf32_Half l_ldnum;		/* Number of dynamic segment entries.  */
};

typedef struct link_map map_t;

//if _ns_loaded == NULL, then new_map is the main map and set _ns_loaded = new_map
struct link_map *_ns_loaded = NULL;

/* Allocate a `struct link_map' for a new object being loaded,
and insert it into the _dl_loaded list.  */
static inline struct link_map *
new_map(dom_t *dom, map_t *loader)
{
	map_t *new_map;

	DOMPRINTF_CALLED(dom->xch);
	
	if (!(new_map = xc_dom_malloc(dom, sizeof(*new_map))))
		return NULL;

	return new_map;
}

int xc_dom_elf_dyn(struct xc_dom_image *dom)
{
	const elf_phdr *phdr;
	Elf32_Phdr *ph;
	uint32_t i;

	uint32_t e_phnum;
	uint32_t p_type, p_paddr, p_memsz, p_filesz, p_offset;

	map_t *main_map = NULL;

	struct elf_binary *elf = dom->private_loader;

	DOMPRINTF_CALLED(dom->xch);

	main_map = new_map(dom, NULL);

	e_phnum = elf_uval(elf, elf->ehdr, e_phnum);
	/*Scan the program header table for the dynamic section.*/
	for (i = 0; i < e_phnum; i++) {
		phdr = elf_phdr_by_index(elf, i);
		p_type = elf_uval(elf, phdr, p_type);
		ph = (Elf32_Phdr *)phdr;
		switch (p_type)
		{
			case PT_PHDR:
				/*Find out the load address.*/
				main_map->l_addr = dom->kernel_seg.vstart - dom->parms.virt_kstart;
				DOMPRINTF("%s:main_map->l_addr = 0x%x", __FUNCTION__, main_map->l_addr);
				break;
			case PT_DYNAMIC:
				p_paddr = elf_uval(elf, phdr, p_paddr);
				p_offset = elf_uval(elf, phdr, p_offset);
				p_filesz = elf_uval(elf, phdr, p_filesz);
				p_memsz = elf_uval(elf, phdr, p_memsz);

				main_map->l_ld = (Elf32_Dyn *)elf_get_ptr(elf, p_paddr);
				main_map->l_ldnum = ph->p_memsz/sizeof(Elf32_Dyn);
				DOMPRINTF("%s:l_ld = 0x%p, l_ldnum = %d", __FUNCTION__, main_map->l_ld, main_map->l_ldnum);
				break;
		}
	}

	return 0;
}
