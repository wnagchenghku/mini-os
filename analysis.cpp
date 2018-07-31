typedef struct elf64_phdr {
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off p_offset;		/* Segment file offset */
  Elf64_Addr p_vaddr;		/* Segment virtual address */
  Elf64_Addr p_paddr;		/* Segment physical address */

  // main_map->l_ld = (Elf32_Dyn *)elf_get_ptr(elf, p_paddr);
  // elf_get_ptr() turns guest pa to host va.

  // Actually point to an array of Elf32_Dyn

  // Each Elf32_Dyn has a tag, e.g., DT_SYMTAB, DT_PLTGOT, DT_HASH

  Elf64_Xword p_filesz;		/* Segment size in file */
  Elf64_Xword p_memsz;		/* Segment size in memory */
  Elf64_Xword p_align;		/* Segment alignment, file & memory */
} Elf64_Phdr;

/* Dynamic structure */
typedef struct {
	Elf32_Sword	d_tag;		/* controls meaning of d_val */
	union {
		Elf32_Word	d_val;	/* Multiple meanings - see d_tag */
		Elf32_Addr	d_ptr;	/* program virtual address */
	} d_un;
} Elf32_Dyn;

struct elf_binary {
    /* elf binary */

    /* loaded to */
    char *dest;
    uint64_t pstart;
    uint64_t pend;
    uint64_t reloc_offset;
};

// dom0_kernel = %p\n", elf->dest[0]

map_object
{
	/*Look for this name among those already loaded.*/
	for (l=_ns_loaded; l; l=l->l_next)
	{
		/*If the requested name matches the soname of a loaded object,
		 * use that object. Elide this check for names that have not yet been opened.*/
		if (!dl_name_match_p(dom, realname, libname, l))// if no match
		{
		}
		/*We have a match*/
		return l;
	}
	/*map shared library into memory.*/
}


map_object_from_bin()
{
	/* Enter the new object in the list of loaded objects.  */

	
}