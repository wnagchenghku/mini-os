#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <zlib.h>
#include <assert.h>
#include <xen/grant_table.h>
//#include <xenstore.h>
//#include <pthread.h>

#include "xg_private.h"
#include "xc_dom.h"
#include "xc_bitops.h"
#include "xg_private.h"
#include "xg_save_restore.h"
//#include "libelf-private.h"
#include <xen/hvm/params.h>
#include "dev_xc_dom_elfdyn.h"
#define DT_LOPROC	0x70000000
#define DT_VERNEEDNUM	0x6fffffff
#define DT_ADDRRNGHI	0x6ffffeff

/* The entries in the .hash table always have a size of 32 bits.  */
typedef uint32_t Elf_Symndx;


/* Structure to describe a single list of scope elements.  The lookup
   functions get passed an array of pointers to such structures.  */
struct r_scope_elem {
  /* Array of maps for the scope.  */
  struct link_map **r_list;
  /* Number of entries in the scope.  */
  unsigned int r_nlist;
};

/*A data structure for a simple single linked list of strings.*/
struct libname_list
{
	char *name;/*Name requested(before search)*/
	struct libname_list *next; /*Link to next name for this object.*/
	int dont_free; /*Flag whether this element should be freed if the object is not
					entirely unloaded.*/
};
/*-------------------------------------------*/
/*define a map from label of relloc table to realaddr*/
Elf64_Addr sym_value[128][128];
int count[2];

/*define an array to .got.plt*/
Elf64_Addr r_got[10];
int got_num = 0;

/*--------------------------------------------*/

/*--------------------------------------------*/
/*define a link table(offset->reference of relloc tabel)*/
/*
struct lookup_relloc {
	Elf64_Addr offset;
	int label;
	struct lookup_relloc *next;
};
*/
/*define a heard point and current point!*/
struct lookup_relloc* head_lr, *current_lr;
/*--------------------------------------------*/


struct link_map {
    /* These first few members are part of the protocol with the debugger.
       This is the same format used in SVR4.  */

    Elf64_Addr l_addr;		/* Base (or delta?) domU address shared object is loaded at.  */
    char *l_name;		/* Absolute file name object was found in.  */
    Elf64_Dyn *l_ld;		/* Dynamic section of the shared object.  */
    Elf64_Half l_ldnum;		/* Number of dynamic segment entries.  */
	
    struct link_map *l_next, *l_prev; /* Chain of loaded objects.  */

    /* All following members are internal to the dynamic linker.
       They may change without notice.  */

    struct libname_list *l_libname; //for, e.g., libkylinx.so.1
	
    /* Indexed pointers to dynamic section.
       [0,DT_NUM) are indexed by the processor-independent tags.
       [DT_NUM,DT_NUM+DT_THISPROCNUM) are indexed by the tag minus DT_LOPROC.
       [DT_NUM+DT_THISPROCNUM,DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM) are
       indexed by DT_VERSIONTAGIDX(tagvalue).
       [DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM,
	DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM) are indexed by
       DT_EXTRATAGIDX(tagvalue).
       [DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM,
	DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM+DT_VALNUM) are
       indexed by DT_VALTAGIDX(tagvalue) and
       [DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM+DT_VALNUM,
	DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM+DT_VALNUM+DT_ADDRNUM)
       are indexed by DT_ADDRTAGIDX(tagvalue), see <elf.h>.  */
    Elf64_Dyn *l_info[DT_NUM];
	//we use elf to replace the following
	struct elf_binary *elf;
    //const Elf64_Phdr *l_phdr;	/* Pointer to program header table in domU.  */
    //Elf64_Addr l_entry;		/* Entry point location.  */
    //Elf64_Half l_phnum;		/* Number of program header entries.  */

    /* Array of DT_NEEDED dependencies and their dependencies, in
       dependency order for symbol lookup (with and without
       duplicates).  There is no entry before the dependencies have
       been loaded.  */
    struct r_scope_elem l_searchlist;

    /* Dependent object that first caused this object to be loaded.  */
    struct link_map *l_loader;

    /* Symbol hash table.*/ 
    Elf_Symndx l_nbuckets;
    Elf64_Word l_gnu_bitmask_idxbits;
    Elf64_Word l_gnu_shift;
    const Elf64_Addr *l_gnu_bitmask;
    union {
      const Elf64_Word *l_gnu_buckets;
      const Elf_Symndx *l_chain;
    };
    union {
      const Elf64_Word *l_gnu_chain_zero;
      const Elf_Symndx *l_buckets;
    };

    enum {			/* Where this object came from.  */
		lt_executable,		/* The main executable program.  */
		lt_library,		/* Library needed by main executable.  */
		lt_loaded		/* Extra run-time loaded shared object.  */
	} l_type:2;
    unsigned int l_relocated:1;	/* Nonzero if object's relocations done.  */
    //unsigned int l_global:1;	/* Nonzero if object in _dl_global_scope.  */
    unsigned int l_reserved:2;	/* Reserved for internal use.  */
    unsigned int l_removed:1;	/* Nozero if the object cannot be used anymore
				   since it is removed.  */
	unsigned int l_soname_added:1;/*Nonzero if the SONAME is for sure in the l_libname list.*/

    /* String specifying the path where this object was found.  */
    const char *l_origin;

    /* Start and finish of memory map for this object.  l_map_start
       need not be the same as l_addr.  */
    Elf64_Addr l_map_start, l_map_end;
    /* End of the executable part of the mapping.  */
    Elf64_Addr l_text_end;

    /* Default array for 'l_scope'.  */
    //struct r_scope_elem *l_scope_mem[4];
    /* Size of array allocated for 'l_scope'.  */
    //size_t l_scope_max;
    /* This is an array defining the lookup scope for this link map.
       There are initially at most three different scope lists.  */
    struct r_scope_elem *l_scope[4];

    /* List of object in order of the init and fini calls.  */
    struct link_map **l_initfini;

    struct {
      const Elf64_Sym *sym;
      int type_class;
      struct link_map *value;
      const Elf64_Sym *ret;
    } l_lookup_cache;

	unsigned long l_serial; //num in the list of _ns_loaded
};


typedef struct xc_dom_image dom_t;
typedef struct link_map map_t;

//if _ns_loaded == NULL, then new_map is the main map and set _ns_loaded = new_map
struct link_map *_ns_loaded = NULL; //for compatibility with kylinx (where named z_ns_loaded)
uint32_t _ns_nloaded = 0;

/*Convert string into hex*/
static inline int htoi(char s[])
{
    int n = 0;
    int i = 0;
    while (s[i] != '\0' && s[i] != '\n') {
        if (s[i] == '0') {
            if (s[i+1] == 'x' || s[i+1] == 'X')
                            i+=2;
        }
        if (s[i] >= '0' && s[i] <= '9') {
            n = n * 16 + (s[i] - '0');
        } else if (s[i] >= 'a' && s[i] <= 'f') {
            n = n * 16 + (s[i] - 'a') + 10;
        } else if (s[i] >= 'A' && s[i] <= 'F') {
            n = n * 16 + (s[i] - 'A') + 10;
        } else
            return -1;
        ++i;
 
    }
    return n;
}

/* Allocate a `struct link_map' for a new object being loaded,
and insert it into the _dl_loaded list.  */
static inline struct link_map *
new_map(dom_t *dom, char *realname, const char *libname, int type, map_t *loader)
{
	//size_t realname_len = strlen(realname) + 1;
	//size_t libname_len = strlen(libname) + 1;
	/* Counter for the scopes we have to handle.  */
	int idx = 0;
	map_t *new_map;
	struct libname_list *newname;

	DOMPRINTF_CALLED(dom->xch);
	if (!realname || !libname)
		return NULL;
	
	if (!(new_map = xc_dom_malloc(dom, sizeof(*new_map))))
		return NULL;	
/*
	if (!(new_map->l_name = xc_dom_malloc(dom, realname_len)))
		return NULL;
*/
	if (!(newname  = xc_dom_malloc(dom, sizeof(*newname))))
		return NULL;
//	memmove(new_map->l_name, realname, realname_len);
	new_map->l_name = realname;
	//new_map->l_libname = newname = (char *)memcpy(newname+1, libname, libname_len);
	newname->name = libname;
	//newname->next = NULL;

	new_map->l_libname = newname;
	//newname->name = (char *)memcpy(newname+1, libname, libname_len);
	/*newname->next = NULL; we use calloc therefore not necessary.*/
	newname->dont_free = 1;
	//new_map->l_libname = newname;
	
	new_map->l_type = type;
	new_map->l_loader = loader;

	if (_ns_loaded != NULL) //Add the global scope.  
		new_map->l_scope[idx++] = &_ns_loaded->l_searchlist;
	//if env->_ns_loaded == NULL, then new_map is the _ns_loaded,
	//and we'll set _ns_loaded to _ns_loaded in _dl_add_to_namespace_list()

	/* If we have no loader the new object acts as it.  */
	if (loader == NULL) //usually happens if it is _ns_loaded
		loader = new_map;
	else /* Determine the local scope.  */		
		while (loader->l_loader != NULL)
			loader = loader->l_loader;

	/* Insert the scope if it isn't the global scope we already added.  */
	if (idx == 0 || &loader->l_searchlist != new_map->l_scope[0]) {
		if (idx != 0) {
			new_map->l_scope[1] = new_map->l_scope[0];
			idx = 0;
		}
		new_map->l_scope[idx] = &loader->l_searchlist;
	}

	//new_map->l_local_scope[0] = &new_map->l_searchlist;

	/* Don't try to find the origin for the main map which has the name "".  */
	//we ignore l_origin now, which specifys the absolute path of object

	return new_map;
}


/* Add the new link_map NEW to the end of the namespace list.  */
static inline void
add_to_namespace_list (dom_t *dom, map_t *new_map)
{
	map_t *l; 

	DOMPRINTF_CALLED(dom->xch);
	if (_ns_loaded != NULL) {
		l = _ns_loaded;
		while (l->l_next != NULL)
			l = l->l_next;
		
		new_map->l_prev = l;
		new_map->l_next = NULL;   /* Would be necessary but we use calloc.  */
		l->l_next = new_map;
	}
	else
    	_ns_loaded = new_map; //this is the main_map
	
	new_map->l_serial = _ns_nloaded;
  	++_ns_nloaded;
}

static inline int  //__attribute__ ((unused, always_inline))
get_dynamic_info(dom_t *dom, map_t *map)
{
	Elf64_Dyn *dyn = map->l_ld;
	Elf64_Dyn **info;
	Elf64_Addr delta = 0;
	const struct elf_binary *elf = map->elf;

/*
	printf("--------------------------------------------------elf->pstart = %p\n", elf->pstart);
	printf("--------------------------------------------------elf->pend = %p\n", elf->pend);
	printf("--------------------------------------------------elf->dest = %p\n", elf->dest);
*/

	//printf("--------------elf->reloc_offset = %p\n", elf->reloc_offset);
	DOMPRINTF_CALLED(dom->xch);
	if (dyn == NULL)
		goto err;
	info = map->l_info;
	
	while (dyn->d_tag != DT_NULL)
	{
		if ((Elf64_Word)dyn->d_tag < DT_NUM)
			info[dyn->d_tag] = dyn;
		/*
		else if (dyn->d_tag >= DT_LOPROC && dyn->d_tag < DT_LOPROC+16)
			info[dyn->d_tag-DT_LOPROC+DT_NUM] = dyn;
		else if ((Elf64_Word)(DT_VERNEEDNUM-(dyn->d_tag))< 16)
			info[(DT_NUM + 16 + (DT_VERNEEDNUM - (dyn->d_tag)))] = dyn;
		else if ((Elf64_Word)(DT_ADDRRNGHI-(dyn->d_tag))<16)
			info[(DT_NUM + 32 + (DT_ADDRRNGHI-(dyn->d_tag)))] = dyn;
		else{
			++dyn;
			continue;
		}
		*/
		++dyn;
	}
	//elf_get_ptr() turns guest pa to host va.
	//so we directly compute the delta
	delta = (uint32_t)elf->dest - elf->pstart;
	//printf("---------------------.plt.got = %p\n", info[DT_PLTGOT]->d_un.d_ptr);
//	printf("---------------------delta = %p\n", delta);
//	printf("---------------------virt_offset = %p\n", dom->parms.virt_offset);
	//printf("--------dom0_kernel = %p\n", elf->dest[0]);
	if (delta) {
		if (info[DT_HASH] != NULL) 
			info[DT_HASH]->d_un.d_ptr += delta;				      
		if (info[DT_PLTGOT] != NULL){
			r_got[got_num] = info[DT_PLTGOT]->d_un.d_ptr;
			info[DT_PLTGOT]->d_un.d_ptr += delta;	
		}
		if (info[DT_STRTAB] != NULL) 
			info[DT_STRTAB]->d_un.d_ptr += delta;				      
		if (info[DT_SYMTAB] != NULL) 
			info[DT_SYMTAB]->d_un.d_ptr += delta;				      
		if (info[DT_RELA] != NULL) 
			info[DT_RELA]->d_un.d_ptr += delta;				      
		if (info[DT_REL] != NULL) 
			info[DT_REL]->d_un.d_ptr += delta;				      
		if (info[DT_JMPREL] != NULL) 
			info[DT_JMPREL]->d_un.d_ptr += delta;
	}

	//printf("------------r_got[%d] = %p\n", got_num, r_got[got_num]);

	if (info[DT_PLTREL] != NULL) {
		assert (info[DT_PLTREL]->d_un.d_val == DT_REL
			|| info[DT_PLTREL]->d_un.d_val == DT_RELA);	
	}
	if (info[DT_RELA] != NULL)
		assert (info[DT_RELAENT]->d_un.d_val == sizeof(Elf64_Rela));

	if (info[DT_REL] != NULL)
		assert (info[DT_RELENT]->d_un.d_val == sizeof(Elf64_Rel));

	return 0;

err:
	return -1;
}

static inline void
setup_hash (const dom_t *dom, map_t *map)
{
	Elf_Symndx *hash;
	Elf64_Dyn **info = map->l_info;
	
	DOMPRINTF_CALLED(dom->xch);
	if (!info[DT_HASH]) 
		return;

	hash = (void *) (info[DT_HASH]->d_un.d_ptr);

	map->l_nbuckets = *hash++;
	/* Skip nchain.  */
	hash++;
	map->l_buckets = hash;
	hash += map->l_nbuckets;
	map->l_chain = hash;

	//DOMPRINTF("%s: l_nbuckets = 0x%x, l_buckets = 0x%p, l_chain = 0x%p", __FUNCTION__,
	//			map->l_nbuckets, (void *)map->l_buckets, (void *)map->l_chain);
}

struct map_list {
	int done;			/* Nonzero if this map was processed.  */
	map_t *map;	/* The data.  */
	struct map_list *next;		/* Elements for normal list.  */
};

typedef struct map_list map_list_t;

//zym: preload() seems to have different meaning with preloads
static inline void 
preload (dom_t *dom, map_t *m, map_list_t *known, unsigned int *nlist)
{
	DOMPRINTF_CALLED(dom->xch);
	known[*nlist].done = 0;
	known[*nlist].map = m;
	known[*nlist].next = &known[*nlist + 1];

	++*nlist;
	/* We use `l_reserved' as a mark bit to detect objects we have
	already put in the search list and avoid adding duplicate
	elements later in the list.  */
	m->l_reserved = 1;
}

//zym: TODO: this func should recgonize several dst names in #str#, i guess
//dst: dynamic sring tokens
static inline const char *
recognize_dst(struct link_map *l, const char *str)
{									      
	return str; 
}
/*
#ifndef ELF_MAGIC
#define ELF_MAGIC 0x464C457FU	//"\x7FELF" in little endian
#endif
*/

static inline void elf_load_syms(struct elf_binary *elf)
{
    elf_ehdr *sym_ehdr;
    unsigned long sz;
    char *maxva, *symbase, *symtab_addr;
    elf_shdr *shdr;
    int i, type;

    if ( !elf->bsd_symtab_pstart )
        return;

#define elf_hdr_elm(_elf, _hdr, _elm, _val)     \
do {                                            \
    if ( elf_64bit(_elf) )                      \
        (_hdr)->e64._elm = _val;                \
    else                                        \
        (_hdr)->e32._elm = _val;                \
} while ( 0 )

    symbase = elf_get_ptr(elf, elf->bsd_symtab_pstart);
    symtab_addr = maxva = symbase + sizeof(uint32_t);

    /* Set up Elf header. */
    sym_ehdr = (elf_ehdr *)symtab_addr;
    sz = elf_uval(elf, elf->ehdr, e_ehsize);
    memcpy(sym_ehdr, elf->ehdr, sz);
    maxva += sz; /* no round up */

    elf_hdr_elm(elf, sym_ehdr, e_phoff, 0);
    elf_hdr_elm(elf, sym_ehdr, e_shoff, elf_uval(elf, elf->ehdr, e_ehsize));
    elf_hdr_elm(elf, sym_ehdr, e_phentsize, 0);
    elf_hdr_elm(elf, sym_ehdr, e_phnum, 0);

    /* Copy Elf section headers. */
    shdr = (elf_shdr *)maxva;
    sz = elf_shdr_count(elf) * elf_uval(elf, elf->ehdr, e_shentsize);
    memcpy(shdr, elf->image + elf_uval(elf, elf->ehdr, e_shoff), sz);
    maxva = (char *)(long)elf_round_up(elf, (long)maxva + sz);

    for ( i = 0; i < elf_shdr_count(elf); i++ )
    {
        type = elf_uval(elf, shdr, sh_type);
        if ( (type == SHT_STRTAB) || (type == SHT_SYMTAB) )
        {
             elf_msg(elf, "%s: shdr %i at 0x%p -> 0x%p\n", __func__, i,
                     elf_section_start(elf, shdr), maxva);
             sz = elf_uval(elf, shdr, sh_size);
             memcpy(maxva, elf_section_start(elf, shdr), sz);
             /* Mangled to be based on ELF header location. */
             elf_hdr_elm(elf, shdr, sh_offset, maxva - symtab_addr);
             maxva = (char *)(long)elf_round_up(elf, (long)maxva + sz);
        }
        shdr = (elf_shdr *)((long)shdr +
                            (long)elf_uval(elf, elf->ehdr, e_shentsize));
    }

    /* Write down the actual sym size. */
    *(uint32_t *)symbase = maxva - symtab_addr;

#undef elf_ehdr_elm
}


static inline int elf_load_dynamic(struct elf_binary *elf, char *filename)
{
	const elf_phdr *phdr;
	uint64_t i, count, paddr, offset, filesz, memsz;
	char *dest;
	char *maddr;
	int fd = -1;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		goto err;
	count = elf_uval(elf, elf->ehdr, e_phnum);
	for (i=0; i<count; i++)
	{
		phdr = elf_phdr_by_index(elf, i);
		if (!elf_phdr_is_loadable(elf, phdr))
			continue;
		paddr = elf_uval(elf, phdr, p_paddr);
		offset = elf_uval(elf, phdr, p_offset);
		filesz = elf_uval(elf, phdr, p_filesz);
		memsz = elf_uval(elf, phdr, p_memsz);
		dest = elf_get_ptr(elf, paddr);
		elf_msg(elf, "%s: phdr %" PRIu64 " at 0x%p -> 0x%p\n",
				                __func__, i, dest, dest + filesz);
		maddr = (char *)mmap(NULL, memsz, PROT_READ|PROT_EXEC, MAP_SHARED, fd, offset);
		dest = maddr;
		memset(dest+filesz, 0, memsz-filesz);
	}
	close(fd);
	elf_load_syms(elf);
	return 0;
err:
	if (fd != -1)
		close(fd);
	return -1;
}


static inline int
parse_elf_so_map(dom_t *dom, map_t *l, void *blob, size_t size, char *filename)
{
    struct elf_binary *elf;
    int rc;
	struct xc_dom_seg dyn_seg; //we dont save it after this func, ok?
	size_t seg_size;
	uint32_t e_phnum;
	uint32_t p_type, p_paddr, p_memsz, p_filesz, p_offset;	
	
	const elf_phdr *phdr;
    uint32_t i;	

	DOMPRINTF_CALLED(dom->xch);
	assert(blob);
	elf = xc_dom_malloc(dom, sizeof(*elf));
	l->elf = elf;
	//init static info (->elf), dyn info (->l_info) is handled in get_dynamic_info()
	//elf->sym_tab is a `shdr', we have to use elf_section_start(shdr) to get the sec addr
	rc = elf_init(elf, blob, size);  
	xc_elf_set_logfile(dom->xch, elf, 1);
    if ( rc != 0 ) {
        xc_dom_panic(dom->xch, XC_INVALID_KERNEL, "%s: corrupted ELF image",
                     __FUNCTION__);
        return rc;
    }

    /* Find the section-header strings table. */
    if ( elf->sec_strtab == NULL ) {
        xc_dom_panic(dom->xch, XC_INVALID_KERNEL, "%s: ELF image"
                     " has no shstrtab", __FUNCTION__);
        return -EINVAL;
    }

    /* parse binary and get xen meta info */
    elf_parse_binary(elf);
	seg_size = elf->pend - elf->pstart;
	//dyn_seg.vstart = elf->pstart + dom->parms.virt_offset;
	//dyn_seg.vend = elf->pend + dom->parms.virt_offset;

    /* load dyn seg. use 0 to tell it make dyn_seg.vstart = virt_alloc_end */
	//after this func, dyn_seg.vstart and .vend is set to the real vaddr in domU 
    if ( xc_dom_alloc_segment(dom, &dyn_seg, "dynamic", 0, seg_size) != 0 )
		return -1;
	
    elf->dest = xc_dom_seg_to_ptr(dom, &dyn_seg);
	/*
	printf("+++++++++++elf->pend = %p\n", elf->pend);
	printf("+++++++++++elf->pstart = %p\n", elf->pstart);
	printf("+++++++++++elf->dest = %p\n", elf->pstart);
*/
   // rc = elf_load_dynamic(elf, filename);
	rc = elf_load_binary(elf);
    if ( rc < 0 ) {
        DOMPRINTF("%s: failed to load elf binary", __FUNCTION__);
        return rc;
    }
	//l_addr is the delta between its real va and `planned' va
	l->l_addr = dyn_seg.vstart - (l->elf->pstart + dom->parms.virt_offset);
	DOMPRINTF("%s: l->l_addr = 0x%x", __FUNCTION__, l->l_addr);
	
	//should we do this?
	if (dom->kernel_seg.vend != dyn_seg.vstart) {
		DOMPRINTF("ERROR: kernel_seg.vend != dyn_seg.vstart");
		return -1;
	}
	dom->kernel_seg.vend = dyn_seg.vend;
	
	e_phnum = elf_uval(elf, elf->ehdr, e_phnum);
	for (i = 0; i < e_phnum; i++) {
		phdr = elf_phdr_by_index(elf, i);
		p_type = elf_uval(elf, phdr, p_type);
		if (p_type != PT_DYNAMIC) 
			continue;

		p_paddr = elf_uval(elf, phdr, p_paddr);
		p_offset = elf_uval(elf, phdr, p_offset);
		p_filesz = elf_uval(elf, phdr, p_filesz);
		p_memsz = elf_uval(elf, phdr, p_memsz);

		l->l_ld = (Elf64_Dyn *)elf_get_ptr(elf, p_paddr);
		l->l_ldnum = ((Elf64_Phdr *)phdr)->p_memsz / sizeof(Elf64_Dyn);
		
		DOMPRINTF("%s: l_ld = 0x%p, l_ldnum = %d", __FUNCTION__, 
				l->l_ld, l->l_ldnum);
		break;
	}
	
	return 0;
}

static inline map_t *
map_object_from_bin(dom_t *dom, const char *libname, char *realname, 
		map_t *loader, int l_type, char *binary, size_t size)
{
	map_t *l = NULL;
	void *blob = (void *)binary;

	DOMPRINTF_CALLED(dom->xch);
	/* Enter the new object in the list of loaded objects.  */
	l = new_map(dom, realname, libname, l_type, loader);
	if (l == NULL) {
		DOMPRINTF("cannot create shared object descriptor");
		goto err;
	}

	if (parse_elf_so_map(dom, l, blob, size, realname))
		goto err;
	
	got_num++;
	get_dynamic_info(dom, l);

/*	DOMPRINTF("\n dynamic: %08x, base: 0x%08x, size: 0x%x,\n entry: 0x%08x,  phdr: 0x%08x,  phnum: 0x%x\n\n",
		(Elf64_Addr) l->l_ld, l->l_addr, l->l_map_end - l->l_map_start,
		l->l_entry, (Elf64_Addr)l->l_phdr, l->l_phnum);
*/
	/* Set up the symbol hash table.  */
	setup_hash(dom, l);

 	/* If this object has DT_SYMBOLIC set modify now its scope.  We don't
     have to do this for the main map.  */

	/* Remember whether this object must be initialized first.  */

	add_to_namespace_list(dom, l);

	return l;
err:
	return NULL;
}

static inline int dl_name_match_p(dom_t *dom, const char *realname, const char *name, const map_t *map)
{
	if (strcmp(realname, map->l_name) == 0)
		return 1;
	struct libname_list *runp = map->l_libname;

	while (runp != NULL)
	{
		if (strcmp(name, runp->name) == 0)
			return 1;
		else
			runp = runp->next;
	}
	return 0;
}

/*Add 'name' to the list of names for a particular shared object.
 * 'name' is expected to have been allocated with malloc and will be
 * freed if the shared object already has this name.
 * return false if the object already had this name.*/
static inline void add_name_to_object(dom_t *dom, map_t *l, const char *name)
{
	struct libname_list *lnp, *lastp;
	struct libname_list *newname;
	size_t name_len;

	lastp = NULL;
	for (lnp=l->l_libname; lnp!=NULL; lastp=lnp, lnp=lnp->next)
		if (strcmp(name, lnp->name) == 0)
			return;
	name_len = strlen(name) + 1;
	newname = (struct libname_list *)xc_dom_malloc(dom, sizeof(*newname));
	if (newname == NULL){
		printf("cannot allocate name record");
		return;
	}
	/*The object should have a libname set from _dl_new_object.*/
	assert(lastp != NULL);

	newname->name = (char *)memcpy(newname+1, name, name_len);
	newname->next = NULL;
	newname->dont_free = 0;
	lastp->next = newname;
}

static xen_pfn_t dom_gnttab_setup(xc_interface *xch, domid_t domid)
{
	gnttab_setup_table_t setup;
    DECLARE_HYPERCALL_BUFFER(xen_pfn_t, gmfnp);
    int rc;
    xen_pfn_t gmfn;

    gmfnp = xc_hypercall_buffer_alloc(xch, gmfnp, sizeof(*gmfnp));
    if (gmfnp == NULL)
        return -1;

    setup.dom = domid;
    setup.nr_frames = 1;
    set_xen_guest_handle(setup.frame_list, gmfnp);
    setup.status = 0;

    rc = xc_gnttab_op(xch, GNTTABOP_setup_table, &setup, sizeof(setup), 1);
    gmfn = *gmfnp;
    xc_hypercall_buffer_free(xch, gmfnp);

    if ( rc != 0 || setup.status != GNTST_okay )
    {
        xc_dom_panic(xch, XC_INTERNAL_ERROR,
                     "%s: failed to setup domU grant table "
                     "[errno=%d, status=%" PRId16 "]\n",
                     __FUNCTION__, rc != 0 ? errno : 0, setup.status);
        return -1;
    }

    return gmfn;
}

static inline map_t * 
map_object (dom_t *dom, map_t *loader, const char *libname, int type, char *rpath)
{
	char *realname;
	size_t libname_len;
	size_t rpath_len;
	//size_t realname_len;
	//struct link_map *l;
	struct link_map *l;
	char *binary;
	size_t size = 0;

	DOMPRINTF_CALLED(dom->xch);
	//assert(loader);
	rpath_len = strlen(rpath);
	
	if (strchr (libname, '/') != NULL) {		
		DOMPRINTF("%s: for now we don't expect '/' in object names!", __FUNCTION__);
		goto err;
	}

	libname_len = strlen(libname);  
	realname = (char *)xc_dom_malloc(dom, rpath_len + libname_len + 2);
	memmove(realname, rpath, rpath_len);
	//for case that rpath may ended without `/'
	if (realname[rpath_len - 1] != '/')
		realname[rpath_len] = '/';
	//realname is xc_dom_malloc-ed, so alreadly inited with 0
	memmove(realname + strlen(realname), libname, libname_len + 1);
	
	DOMPRINTF("realname = %s", realname);

	/*Look for this name among those already loaded.*/
	for (l=_ns_loaded; l; l=l->l_next)
	{
		/*If the requested name matches the soname of a loaded object,
		 * use that object. Elide this check for names that have not yet been opened.*/
		if (!dl_name_match_p(dom, realname, libname, l))// if no match
		{
			const char *soname;
			if (l->l_info[DT_SONAME] == NULL)
				continue;
			soname = ((const char *)l->l_info[DT_STRTAB]->d_un.d_ptr + l->l_info[DT_SONAME]->d_un.d_val);
			if (strcmp(libname, soname) != 0)
				continue;
			/*We have a match on a new name---cache it.*/
			add_name_to_object(dom, l, soname);
			l->l_soname_added = 1;
		}
		/*We have a match*/
		return l;
	}
	/*map shared library into memory.*/
	binary = (char *) xc_dom_malloc_filemap(dom, realname, &size, 0);
	if (!binary)
		goto err;
/*
	gnttab_setup_table_t setup_op;
	setup_op.dom = DOMID_SELF;
	printf("domid = %d\n", DOMID_SELF);
	setup_op.nr_frames = 1;
	setup_op.frame_list = gnttab;

	HYPERVISOR_grant_table_op(GNTTABOP_setup_table, &setup_op, 0);
*/	
	xen_pfn_t gnttab_gmfn;
	grant_entry_v1_t *gnttab;

	gnttab_gmfn = dom_gnttab_setup(dom->xch, DOMID_SELF);
	if (gnttab_gmfn == -1)
		return -1;

	gnttab = xc_map_foreign_range(dom->xch, DOMID_SELF, PAGE_SIZE, PROT_READ|PROT_WRITE, gnttab_gmfn);

	gnttab[0].flags = GTF_permit_access;
	gnttab[0].domid = 1;
	gnttab[0].frame = ((unsigned long)binary >> 12);

	return map_object_from_bin(dom, libname, realname, loader, type, binary, size);

err:	
	return NULL;
}

static inline int
load_objects(dom_t *dom, map_t *map, map_list_t *known)
{
	unsigned int nlist = 0;
	map_list_t *runp, *tail;
	/* Object name.  */
	const char *libname = NULL;
	map_t *l, **needed;
	unsigned int nneeded;
	size_t malloc_size;
	map_list_t *newp;
	map_t **l_initfini;
	int type;
	map_t *dep;

	DOMPRINTF_CALLED(dom->xch);
	/* First load MAP itself.  */
	preload(dom, map, known, &nlist);

	/* Terminate the lists.  */
	known[nlist - 1].next = NULL;

	/* Pointer to last unique object.  */
	tail = &known[nlist - 1];

	/* Process each element of the search list, loading each of its
	auxiliary objects and immediate dependencies.  Auxiliary objects
	will be added in the list before the object itself and
	dependencies will be appended to the list as we step through it.
	This produces a flat, ordered list that represents a
	breadth-first search of the dependency tree.

	The whole process is complicated by the fact that we better
	should use alloca for the temporary list elements.  But using
	alloca means we cannot use recursive function calls.  */
	for (runp = known; runp; ) {
		l = runp->map;
		//struct link_map *temp_map_array[MAX_LINK_MAP];
		needed = NULL;
		nneeded = 0;

		/* Unless otherwise stated, this object is handled.  */
		runp->done = 1;

		/*Allocate a temporary record to contain the references to the
		dependencies of this object.*/
		//zym: we dont use needed[] to handle #map# (the root), which will 
		//be handled outsied the outer loop, for (runp = known; runp; )
		if (l->l_searchlist.r_list == NULL && l->l_initfini == NULL
				&& l != map && l->l_ldnum > 0) 
		{	// this is the max size needed...
			malloc_size = l->l_ldnum * sizeof(map_t *);
			needed = (map_t **) xc_dom_malloc(dom, malloc_size);
			if (!needed)
				goto err;
		}
			

		if (l->l_info[DT_NEEDED]) { //|| l->l_info[AUXTAG] || l->l_info[FILTERTAG]
			const char *strtab = (const void *) (l->l_info[DT_STRTAB]->d_un.d_ptr);
			const Elf64_Dyn *d;
			char *realrpath = (char *)(l->l_info[DT_STRTAB]->d_un.d_ptr) + l->l_info[DT_RPATH]->d_un.d_val;
			int i = 1;
			
			char *token;
			char *prpath[255];
			char *delim = ":";
			token = strtok(realrpath, delim);
			prpath[0] = token;
			while ((token = strtok(NULL, delim)) != NULL){
				prpath[i++] = token;
			}

			for (d = l->l_ld; d->d_tag != DT_NULL; ++d) {
				if (d->d_tag == DT_NEEDED) {
					/* Recognize DSTs.  *///dst: dynamic sring tokens
					//what if there are multi dsts?
					if ( !(libname = recognize_dst(l, strtab + d->d_un.d_val)) )
						continue;

					type = l->l_type == lt_executable ? lt_library : l->l_type;
					/* Map in the needed object.  */
					dep = map_object(dom, l, libname, type, prpath[0]);
					if (!dep)
						goto err;
					
					/* We use `l_reserved' as a mark bit to detect objects we have
					already put in the search list and avoid adding duplicate
					elements later in the list.  */
					if (!dep->l_reserved) {
						newp = (map_list_t *) xc_dom_malloc(dom, sizeof(map_list_t));
						if (!newp)
							goto err;
						newp->map = dep;
						newp->done = 0;
						newp->next = NULL;
						tail->next = newp;
						tail = newp;						
						++nlist; //assert(++nlist < MAX_LINK_MAP);
						/* Set the mark bit that says it's already in the list.  */
						dep->l_reserved = 1;
					}

					if (needed != NULL)
						needed[nneeded++] = dep;
				}
			}
		}

		if (needed != NULL) {
			/* Terminate the list of dependencies and store the array address.  
			zym: the following malloc a ptr array for (2 * nneeded + 1) struct link_map 
			ptrs, copy twice #strangely#, and make l->l_initfini point to the array */
			//assert(nneeded < MAX_LINK_MAP); 
			needed[nneeded++] = NULL;
			malloc_size = sizeof(map_t *) * (2 * nneeded + 1);
			l_initfini = (map_t **) xc_dom_malloc(dom, malloc_size);
			
			l_initfini[0] = l;
			memmove (&l_initfini[1], needed, nneeded * sizeof needed[0]);
			memmove (&l_initfini[nneeded + 1], l_initfini, nneeded * sizeof needed[0]);
			//if needed[] has 3 elements: map1, map2, null, then z_initfini_array 
			//has 7 elements: l, map1, map2, null, l, map1, map2. 
			l->l_initfini = l_initfini;
			//l->z_initfini_num = 2 * nneeded + 1;
		}

		/* If we have no auxiliary objects just go on to the next map.  */
		if (runp->done)
			do { runp = runp->next;
			} while (runp != NULL && runp->done);
	}

	return nlist;
err:
	return -1;
}

static inline void adjust_dep_order(struct link_map *map, unsigned int nlist)
{
	/* We can skip looking for the binary itself which is at the front
	of the search list.  */
	int i = 1;
	uint16_t seen[nlist];
	map_t *thisp, **runp;
	uint16_t this_seen;
	unsigned int k;
	
	memset (seen, 0, nlist * sizeof (seen[0]));
	while (1) {
		/* Keep track of which object we looked at this round.  */
		++seen[i];
		thisp = map->l_initfini[i];

		/* Find the last object in the list for which the current one is
		a dependency and move the current object behind the object
		with the dependency.  */
		k = nlist - 1;
		while (k > i) {
			runp = map->l_initfini[k]->l_initfini;
			if (runp != NULL)
				/* Look through the dependencies of the object.  */
				while (*runp != NULL)
					if (*runp++ == thisp) {
						/* Move the current object to the back past the last
						object with it as the dependency.  */
						memmove(&map->l_initfini[i], 
						  &map->l_initfini[i + 1],
						  (k - i) * sizeof (map->l_initfini[0]));
						
						map->l_initfini[k] = thisp;

						if (seen[i + 1] > nlist - i) {
							++i;
							goto next_clear;
						}

						this_seen = seen[i];
						memmove(&seen[i], &seen[i + 1], (k - i) * sizeof (seen[0]));
						seen[k] = this_seen;

						goto next;
					}

					--k;
		}

		if (++i == nlist)
			break;
next_clear:
		memset(&seen[i], 0, (nlist - i) * sizeof (seen[0]));

next:
		;
	}
}	

/* Call _dl_map_object on the dependencies of MAP, and set up
   MAP->l_searchlist.  PRELOADS points to a vector of NPRELOADS previously
   loaded objects that will be inserted into MAP->l_searchlist after MAP
   but before its dependencies.  */
//zym: currently this is invoked ONLY by main_map
static inline int
map_object_deps (dom_t *dom, map_t *map)
{
	map_list_t *known, *runp;
	unsigned int nlist = 0;
	map_t **l_initfini;

	DOMPRINTF_CALLED(dom->xch);

	// 1st for #map# (here is the main_map), 2nd for tail
	known = (map_list_t *)xc_dom_malloc(dom, sizeof(*known) * 2); 
	if (!known)
		goto err;
	
	nlist = load_objects(dom, map, known);
	if (nlist == -1)
		goto err;
	
	/* zym: now we omit this case. ('map' is the input argument, i.e. the root map) 
	map->l_initfini != NULL means this object was previously loaded as a 
	dependency and we have a separate l_initfini list.  We don't need it anymore.  */
	if (map->l_initfini)
		goto err;

	/* Store the search list we built in the object.  It will be used for
	searches in the scope of this object.  */
	l_initfini = (map_t **) xc_dom_malloc(dom, sizeof(map_t *) * (2 * nlist + 1));
	if (!l_initfini)
		goto err;
	
	map->l_searchlist.r_list = &l_initfini[nlist + 1];
	map->l_searchlist.r_nlist = nlist;     
	for (nlist = 0, runp = known; runp; runp = runp->next) {
		map->l_searchlist.r_list[nlist++] = runp->map;
		/* Now clear all the mark bits (we set in the objects on the search list
		to avoid duplicates), so the next call starts fresh.  */
		runp->map->l_reserved = 0;
	 }

	assert(map->l_searchlist.r_list[0] == map);

	//r_list may change in the above f**king code
	/* Sort the initializer list to take dependencies into account.  The binary
     itself will always be initialize last.  */
	memmove(l_initfini, map->l_searchlist.r_list, nlist * sizeof(map_t *));
	map->l_initfini = l_initfini;

	if (nlist > 1)
		adjust_dep_order(map, nlist);

	

	return 0;

err:
	return -1;
}

#define D_PTR(map, i) (map)->i->d_un.d_ptr

static inline void
machine_rel_relative (dom_t *dom, Elf64_Addr l_addr, const Elf64_Rel *reloc, 
		void *const reloc_addr_arg)
{
	Elf64_Addr *const reloc_addr = xc_dom_vaddr_to_ptr(dom, (uint32_t)reloc_addr_arg);

	DOMPRINTF_CALLED(dom->xch);
	//the following condition is necessary in glibc since l_addr's type is ElfW(Addr).
	//but here it is actually can be omitted...
	if (ELF64_R_TYPE(reloc->r_info) == R_386_RELATIVE) {
    		*(Elf64_Addr *) reloc_addr = (Elf64_Addr)l_addr;
	} 
}

struct sym_val {
    const Elf64_Sym *s;
    struct link_map *m;
};

static uint_fast32_t
calc_new_hash (const char *s)
{
  uint_fast32_t h = 5381;
  for (unsigned char c = *s; c != '\0'; c = *++s)
    h = h * 33 + c;
  return h & 0xffffffff;
}

/* This is the hashing function specified by the ELF ABI.  It is highly
   optimized for the PII processors.  Though it will run on i586 it
   would be much slower than the generic C implementation.  So don't
   use it.  */
static inline unsigned int
/* This is the hashing function specified by the ELF ABI.  In the
   first five operations no overflow is possible so we optimized it a
   bit.  */
kelf_hash(const char *name_arg)
{
  const unsigned char *name = (const unsigned char *) name_arg;
  unsigned long int hash = *name;
  if (hash != 0 && name[1] != '\0')
    {
      hash = (hash << 4) + name[1];
      if (name[2] != '\0')
	{
	  hash = (hash << 4) + name[2];
	  if (name[3] != '\0')
	    {
	      hash = (hash << 4) + name[3];
	      if (name[4] != '\0')
		{
		  hash = (hash << 4) + name[4];
		  name += 5;
		  while (*name != '\0')
		    {
		      unsigned long int hi;
		      hash = (hash << 4) + *name++;
		      hi = hash & 0xf0000000;

		      /* The algorithm specified in the ELF ABI is as
			 follows:

			 if (hi != 0)
			   hash ^= hi >> 24;

			 hash &= ~hi;

			 But the following is equivalent and a lot
			 faster, especially on modern processors.  */

		      hash ^= hi >> 24;
		    }

		  /* Second part of the modified formula.  This
		     operation can be lifted outside the loop.  */
		  hash &= 0x0fffffff;
		}
	    }
	}
    }
  return hash;
}


/* Ignore all but STT_NOTYPE, STT_OBJECT, STT_FUNC,
STT_COMMON, STT_TLS, and STT_GNU_IFUNC since these are no
code/data definitions.  */
#define ALLOWED_STT \
	((1 << STT_NOTYPE) | (1 << STT_OBJECT) | (1 << STT_FUNC) \
	 | (1 << STT_COMMON) | (1 << STT_TLS) | (1 << STT_GNU_IFUNC))

/* Nested routine to check whether the symbol matches.  */
//zym: now we should have no version or verstab at all
static inline const Elf64_Sym *
check_sym_match (dom_t *dom, const Elf64_Sym *sym, const Elf64_Sym *ref, 
		const char *strtab, const char *undef_name, int type_class)
{
	unsigned int stt = ELF64_ST_TYPE(sym->st_info);
	if ( (sym->st_value == 0 && stt != STT_TLS)
			|| (type_class & (sym->st_shndx == SHN_UNDEF))) //__builtin_expect 0
		return NULL;

	 
	if ( ((1 << stt) & ALLOWED_STT) == 0) //__builtin_expect 0
		return NULL;

	//DOMPRINTF("st_name = %s; undef = %s", strtab + sym->st_name, undef_name);
	if (sym != ref && strcmp(strtab + sym->st_name, undef_name))
		/* Not the symbol we are looking for.  */
		return NULL;

	/* There cannot be another entry for this symbol so stop here.  */
	return sym;
}

/* Inner part of the lookup functions.  We return a value > 0 if we
   found the symbol, the value 0 if nothing is found and < 0 if
   something bad happened.  */
static int
do_lookup_x (dom_t *dom, const char *undef_name, uint_fast32_t new_hash,
		unsigned long int *old_hash, const Elf64_Sym *ref,
		struct sym_val *result, struct r_scope_elem *scope, size_t i,
		int flags, map_t *skip, int type_class, map_t *undef_map)
{
	/* Make sure we read the value before proceeding.  Otherwise we
	might use r_list pointing to the initial scope and r_nlist being
	the value after a resize.  That is the only path in dl-open.c not
	protected by GSCOPE.  A read barrier here might be to expensive.  */
	//__asm volatile ("" : "+r" (n), "+m" (scope->r_list));
	map_t **list = scope->r_list;
	Elf_Symndx symidx;
	map_t *map;
	const Elf64_Sym *symtab;
	const char *strtab;
	const Elf64_Sym *sym;
	const Elf64_Addr *bitmask;

	DOMPRINTF_CALLED(dom->xch);
	do {
		/* These variables are used in the nested function.  */		
		map = list[i];
		/* Here come the extra test needed for `_dl_lookup_symbol_skip'.  */
		if (map == skip)
			continue;

		/* Don't search the executable when resolving a copy reloc.  */
		if ((type_class & ELF_RTYPE_CLASS_COPY) && map->l_type == lt_executable)
			continue;

		/* Do not look into objects which are going to be removed.  */
		if (map->l_removed)
			continue;

		/* If the hash table is empty there is nothing to do here.  */
		if (map->l_nbuckets == 0)
			continue;

		/* The tables for this map.  */
		symtab = (const void *) D_PTR(map, l_info[DT_SYMTAB]);
		strtab = (const void *) D_PTR(map, l_info[DT_STRTAB]);
		bitmask = map->l_gnu_bitmask;
		
		if (bitmask != NULL) { //__builtin_expect: 1
			xc_dom_panic(dom->xch, XC_INTERNAL_ERROR,"%s: gnu-style hash not supported yet", __FUNCTION__); //zym
		}
		else { //if (bitmask != NULL)
			if (*old_hash == 0xffffffff)
				*old_hash = kelf_hash(undef_name);

		  	/* Use the old SysV-style hash table.  Search the appropriate
		     hash bucket in this object's symbol table for a definition
		     for the same symbol name.  */
			symidx = map->l_buckets[*old_hash % map->l_nbuckets];
			for (; symidx != STN_UNDEF; symidx = map->l_chain[symidx]) {
				sym = check_sym_match(dom, &symtab[symidx], ref, 
									strtab, undef_name, type_class);

				if (sym != NULL)
					goto found_it;
			}
		}
      	/* If we have seen exactly one versioned symbol while we are
	 	looking for an unversioned symbol and the version is not the
	 	default version we still accept this symbol since there are
	 	no possible ambiguities.  */
      	sym = NULL;

		if (sym != NULL) {
found_it:	DOMPRINTF("found sym! str: %s, value: %08x\n", 
					strtab + sym->st_name, map->l_addr + sym->st_value);
			switch (ELF64_ST_BIND(sym->st_info)) { //__builtin_expect: STB_GLOBAL
			case STB_WEAK:
				/*
				xc_dom_panic(dom->xch, XC_INTERNAL_ERROR,
						"%s: case STB_WEAK not supported yet", __FUNCTION__); //zym
				break;
				*/
				/* Weak definition.  Use this value if we don't find another.  */
				if (! result->s){
					result->s = sym;
					result->m = (struct link_map *) map;
				}
				return 1;
				
			case STB_GLOBAL:
				/* Global definition.  Just what we need.  */
				result->s = sym;
				result->m = (map_t *) map;
				return 1;

			default:
				/* Local symbols are ignored.  */
				break;
			}
		}		
	}
	while (++i < scope->r_nlist);

	/* We have not found anything until now.  */
	return 0;
}

static inline map_t *
lookup_symbol_x(dom_t *dom, const char *undef_name, map_t *undef_map,
		const Elf64_Sym **ref, struct r_scope_elem *symbol_scope[],
		int type_class, int flags, map_t *skip_map)
{
	const uint_fast32_t new_hash = calc_new_hash(undef_name);
	unsigned long int old_hash = 0xffffffff;
	struct sym_val current_value = { NULL, NULL };
	struct r_scope_elem **scope = symbol_scope;
	size_t i = 0;
	size_t start;

	DOMPRINTF_CALLED(dom->xch);
	DOMPRINTF("undef_name: %s", undef_name);
	/* No other flag than DL_LOOKUP_ADD_DEPENDENCY or DL_LOOKUP_GSCOPE_LOCK
	is allowed if we look up a versioned symbol.  */

	if (skip_map != NULL)
		/* Search the relevant loaded objects for a definition.  */
		while ((*scope)->r_list[i] != skip_map)
			++i;

	/* Search the relevant loaded objects for a definition.  */
	for (start = i; *scope != NULL; start = 0, ++scope) {
		int res = do_lookup_x(dom, undef_name, new_hash, &old_hash, *ref, &current_value, *scope, start, flags,
				skip_map, type_class, undef_map);
		
		if (res > 0)
			break;

		if (res < 0 && skip_map == NULL) {
			*ref = NULL;
			return NULL;
		}
	}

	if (current_value.s == NULL) { 
		*ref = NULL;
		return NULL;
	}

	if (current_value.m->l_type == lt_loaded) //__builtin_expect = 0
	//n.b. glibc does `add_dependency()', and panic if failed...
		xc_dom_panic(dom->xch, XC_INTERNAL_ERROR,
				"%s: m->l_type == lt_loaded, not supported yet", __FUNCTION__); //zym

	/* The object is used.  
	if (current_value.m->l_used == 0) //__builtin_expect 0
		current_value.m->l_used = 1;
*/
	*ref = current_value.s;
	return current_value.m;
}

static inline map_t *
resolve_map(dom_t *dom, const Elf64_Sym **ref, unsigned int r_type, 
		map_t *l, struct r_scope_elem *scope[])
{
	map_t *_lr;
	const char *strtab;
	int _tc;

	DOMPRINTF_CALLED(dom->xch);
	if (ELF64_ST_BIND((*ref)->st_info) == STB_LOCAL) {
		return l;
	}
	else if ((*ref) == l->l_lookup_cache.sym 
			&& elf_machine_type_class(r_type) == l->l_lookup_cache.type_class) {
		(*ref) = l->l_lookup_cache.ret;
		return l->l_lookup_cache.value;
	}
	else { 
		strtab = (const void *)l->l_info[DT_STRTAB]->d_un.d_ptr;								      
		_tc = elf_machine_type_class(r_type);			     
		l->l_lookup_cache.type_class = _tc;			      
		l->l_lookup_cache.sym = (*ref);				      
		_lr = lookup_symbol_x(dom, strtab + (*ref)->st_name, l, ref, scope, 
				_tc, DL_LOOKUP_ADD_DEPENDENCY, NULL);      
		l->l_lookup_cache.ret = (*ref);				      
		l->l_lookup_cache.value = _lr; 
		return _lr;
	}
}

/*Based on function name, We can find really relloc addr */
// static inline Elf64_Addr update_lookup_lazy_binding(dom_t *dom, map_t *map, char *fun_name, int type ,struct r_scope_elem *scope[])
// {
// 	Elf64_Sym *sym;
// 	Elf64_Addr value;
// 	map_t *sym_map;

// 	const Elf64_Sym *const symtab = (const void*)D_PTR(map, l_info[DT_SYMTAB]);
// 	const char *strtab = (const void *)D_PTR(map, l_info[DT_STRTAB]);
	
// 	sym = symtab;
// 	while (sym != NULL){
// 		if (!strcmp(strtab[sym->st_name], fun_name))
// 			break;
// 		sym++;
// 	}

// 	sym_map = resolve_map(dom, &sym, type, map, scope);
// 	value = (sym_map == NULL)? 0:(Elf64_Addr)(sym_map->l_addr) + sym->st_value;
	
// 	return value;
// }

/*Read addr from the xenstore!*/
// static inline Elf64_Addr update_readaddr_xenstore(dom_t *dom)
// {
// 	struct xs_handle *xs;
// 	xs_transaction_t xth;
// 	char path[128];
// 	char *value;
// 	int len;
// 	/*step 1: Get domid of the guest os*/
// 	int domid = dom->guest_domid;

// 	/*step 2: Read /local/domain/domid/console/addr*/
// 	xs = xs_daemon_open();
// 	if (xs == NULL){
// 		printf("Could not connect to xenstore!\n");
// 		exit(1);
// 	}

// 	xth = xs_transaction_start(xs);
// 	if (xth == 0){
// 		printf("Could not start xaction xenstore!\n");
// 		exit(1);
// 	}
	
// 	snprintf(path, sizeof(path), "/local/domain/%d/console/addr", domid);

// 	value = xs_read(xs, xth, path, &len);

// 	xs_transaction_end(xs, xth, 1);
// 	if (xth == 0){
// 		printf("Could not end xaction xenstore!\n");
// 		exit(1);
// 	}
// 	xs_daemon_close(xs);
	
// 	return htoi(value);
// }

/*Check out a list of struct lookup_relloc*/
/*
static inline int update_lookup_relloc(dom_t *dom, Elf64_Addr offset)
{
	for (current_lr = head_lr; current_lr != NULL; current_lr = current_lr->next)
	{
		if (current_lr->offset == offset)
			return current_lr->label;
	}
	return -1;
}
*/
/*Based on relloc table, Get really relocate address*/
/*issue: need map(module ID)*/
// static inline Elf64_Addr update_relloc_address(dom_t *dom, map_t *map, int label, struct r_scope_elem *scope[])
// {
// 	Elf64_Rel *reloc;
// 	Elf64_Sym *sym;
// 	int r_type;
// 	map_t *sym_map;
// 	Elf64_Addr value;

// 	/*step 1: Get address of relloc table!*/
// 	const Elf64_Rel *r = D_PTR((map), l_info[DT_JMPREL]);
	
// 	/*step 2: Get address of symtab!*/
// 	const Elf64_Sym *const symtab = (const void *)D_PTR(map, l_info[DT_SYMTAB]);

// 	/*step 3: Get sym and relloc */
// 	reloc = r + label;
// 	sym = &symtab[ELF64_R_SYM(reloc->r_info)];
	
// 	r_type = ELF64_R_TYPE(reloc->r_info);

// 	sym_map = resolve_map(dom, &sym, r_type, map, scope);
// 	value = (sym_map == NULL)?0:(Elf64_Addr)(sym_map->l_addr) + sym->st_value;

// 	return value;
// }

/*Write realaddr into xenstore!*/
/*static inline void update_write_xenstore(dom_t *dom, Elf64_Addr realaddr)
{
	struct xs_handle *xs;
	char path[128];
	int error;
	char value[12];
	
	xs = xs_daemon_open();
	if (xs == NULL) {
		printf("Could not connect to xenstore!\n");
		exit(1);
	}

	snprintf(path, sizeof(path), "/local/domain/%d/console/realaddr", dom->guest_domid);
	sprintf(value, "%p", realaddr);
	error = xs_write(xs, XBT_NULL, path, value, strlen(value));
	if (error == 0){
		printf("Cound not write key-value in xenstore!\n");
		exit(1);
	}

	xs_daemon_close(xs);
}*/

/*listen xenstore (xs_watch)*/
/*static inline void update_watch_xenstore(dom_t *dom, map_t *map, struct r_scope_elem *scope[])
{
	struct xs_handle *xs;
	char path[128];
	int err;
	int fd;
	fd_set set;
	char **vec;
	unsigned int num;
	Elf64_Addr offset, realaddr;
	struct timeval tv;
	int label;

	xs = xs_daemon_open();
	if (xs == NULL) {
		printf("Could not connect to xenstore!\n");
		exit(1);
	}

	snsprintf(path, sizeof(path), "/local/domain/%d/console", dom->guest_domid);
	
	err = xs_watch(xs, path, "addr");
	if (err == 0) {
		printf("Error in setting watch on mytoken in %s\n", path);
	}
	fd = xs_fileno(xs);
	while (1) {
		FD_ZERO(&set);
		FD_SET(fd, &set);
		fflush(stdout);
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		if (select(fd+1, &set, NULL, NULL, &tv) > 0 && FD_ISSET(fd, &set)){
			vec = xs_read_watch(xs, &num);
			if (!vec) {
				printf("Error on watch firing!\n");
				exit(1);
			}
			offset = htoi(vec[XS_WATCH_TOKEN]);
			label = update_lookup_relloc(dom, offset);
			realaddr = update_relloc_address(dom, map, label, scope);
			update_write_xenstore(dom, realaddr);
		}
	}

	xs_daemon_close(xs);
}*/


/* Perform the relocation specified by RELOC and SYM (which is fully resolved).
   MAP is the object containing the reloc.  */
static inline void 
machine_rel(dom_t *dom, map_t *map, const Elf64_Rel *reloc, const Elf64_Sym *sym, void *const reloc_addr_arg, struct r_scope_elem *scope[])
{	
	//reloc_addr_arg is guest addr, and reloc_addr is host addr const Elf64_Sym *sym, 
	Elf64_Addr *const reloc_addr = xc_dom_vaddr_to_ptr(dom, (Elf64_Addr)reloc_addr_arg);

	//Elf64_Addr *const reloc_addr = reloc_addr_arg;
	const unsigned int r_type = ELF64_R_TYPE(reloc->r_info);
	map_t *sym_map;
	Elf64_Addr value;

	DOMPRINTF_CALLED(dom->xch);

	if (r_type == R_386_RELATIVE) {
		*reloc_addr += map->l_addr;
    	}
	else if (r_type == R_386_NONE) {
		return;
	}
  	else {
		sym_map = resolve_map(dom, &sym, r_type, map, scope);

		value = (sym_map == NULL) ? 0 : (Elf64_Addr)(sym_map->l_addr) + sym->st_value;

		if (sym != NULL	&& ELF64_ST_TYPE(sym->st_info) == STT_GNU_IFUNC && sym->st_shndx != SHN_UNDEF)
			value = ((Elf64_Addr(*) (void)) value) ();

		switch (r_type) {
		case R_386_GLOB_DAT:
		case R_386_JUMP_SLOT:
	  		*reloc_addr = value;
			break;
			
		case R_386_TLS_DTPMOD32:
		case R_386_TLS_DTPOFF32:
		case R_386_TLS_DESC:
		case R_386_TLS_TPOFF32:
			if (sym_map != NULL) {
				DOMPRINTF("%s: case %d not supported yet", __FUNCTION__, r_type);
				//reloc_addr = sym_map->l_tls_modid;
			}
			break;
			
		case R_386_32:
			*reloc_addr +=  value;
			break;	
			
		case R_386_PC32:
			*reloc_addr += (value - (Elf64_Addr) reloc_addr);
			break;

		case R_386_IRELATIVE:
			value = map->l_addr + *reloc_addr;
			value = ((Elf64_Addr (*) (void)) value) ();
			*reloc_addr = value;
			break;

		default:
			//_dl_reloc_bad_type (map, r_type, 0);
			DOMPRINTF("%s: bad type = %d", __FUNCTION__, r_type);
	  		break;
		}
	}
}

/*---------------------------------------------------------------------------------*/
/*Let r_offset point to .kylinx.got table*/
static inline void machine_rel_init_kylinx_got(dom_t *dom, map_t *map, const Elf64_Rel *reloc, const Elf64_Sym *sym, void *const reloc_addr_arg, struct r_scope_elem *scope[], int moduleID)
{
	/*Step 1: convert r_offset into reloc_addr_arg in dom0*/
	Elf64_Addr *const reloc_addr = xc_dom_vaddr_to_ptr(dom, (Elf64_Addr)reloc_addr_arg);
	Elf64_Addr *got = (void *)D_PTR(map, l_info[DT_PLTGOT]);
	const unsigned int r_type = ELF64_R_TYPE(reloc->r_info);
	map_t *sym_map;
	Elf64_Addr value;

	/*step 2: Declare a struct */
	//struct kylinx kl;

	/*test elf->dest*/
	const struct elf_binary *elf = map->elf;
	/*
	printf("--------dom0_kernel = %p\n", elf->dest[0]);
	printf("--------dom0_kernel = %p\n", elf->dest[1]);
	printf("--------dom0_kernel = %p\n", elf->dest[2]);
	*/
	/*set got[1]*/
	got[1] = (Elf64_Addr)moduleID;
	//printf("-------------got[1] = %p", got[1]);
	sym_map = resolve_map(dom, &sym, r_type, map, scope);
	value = (sym_map == NULL)?0:(Elf64_Addr)(sym_map->l_addr) + sym->st_value;
	/*keep value in sym_value*/
	sym_value[moduleID][count[moduleID]] = value;
	Elf64_Addr *strtab = (Elf64_Addr)D_PTR(map, l_info[DT_STRTAB]);
	/*
	if (moduleID == 0){
		printf("--------sym = %s\n", strtab + sym->st_name);	
		printf("---------------sym_value[%d][%d] = %p\n", moduleID, count[moduleID], sym_value[moduleID][count[moduleID]]);
	}
	*/
	/*
	printf("---------------value = %p\n", value);
	*/
	//*reloc_addr = value;
	//printf("---------------sym_value[%d][%d] = %p\n", moduleID, count[moduleID], sym_value[moduleID][count[moduleID]]);
//void (*plt[1024])(void *);
	/*
	kl.func = value;
	kl.r_offset = reloc->r_offset;
	kylinx_got[count] = kl;
	*/
	/*step 3: assign kl to reloc_addr*/
	//*reloc_addr = kylinx_got[count].func;
	/*
	printf("-----------------------*reloc_addr = %p\n", *reloc_addr);
	printf("-----------------------reloc_addr = %p\n", reloc_addr);
	printf("-----------------------got[0] = %p\n", got[0]);
	printf("-----------------------got[1] = %p\n", got[1]);
	printf("-----------------------got[2] = %p\n", got[2]);
	printf("-----------------------got[3] = %p\n", got[3]);
	printf("-----------------------got = %p\n", got);
	printf("-----------------------r_offset = %p\n", reloc->r_offset);
	printf("-----------------------reloc_addr_arg = %p\n", reloc_addr_arg);

	printf("---------------------------------------------------------\n");
	printf("-------------------------------elf_note_vstart = %p\n", dom->parms.elf_note_start);
	printf("-------------------------------elf_note_vend = %p\n", dom->parms.elf_note_end);
	*/
}

/*
void watch_map(dom_t *dom)
{
	//step 1: listen /local/domain/domid/console/map
	struct xs_handle *xs;
	xs_transaction_t xth;
	char path[128];
	char *value;
	unsigned int len;

	xs = xs_daemon_open();
	if (xs == NULL){
		printf("Could not connect to xenstore!\n");
		exit(1);
	}
	xth = xs_transaction_start(xs);
	if (xth == 0) {
		printf("Could not start xaction xenstore!\n");
		exit(1);
	}

	snprintf(path, sizeof(path), "/local/domain/%d/console/map", dom->guest_domid);
	printf("---------path = %s\n", path);
	while (1) {
		value = xs_read(xs, xth, path, &len);
		if (value == NULL)
			continue;
		break;
	}

	xs_transaction_end(xs, xth, 1);
	if (xth == 0) {
		printf("Could not end xaction xenstore!\n");
		exit(1);
	}

	//step 2: write realaddr
				
	xs_daemon_close(xs);
}
*/
/*---------------------------------------------------------------------------------*/

/* Perform the relocations in MAP on the running program image as specified
   by RELTAG, SZTAG.  If LAZY is nonzero, this is the first pass on PLT
   relocations; they should be set up to call _dl_runtime_resolve, rather
   than fully resolved now.  */
static inline void //__attribute__ ((always_inline))
dynamic_do_Rel(dom_t *dom, map_t *map, Elf64_Addr reladdr, Elf64_Addr relsize, struct r_scope_elem *scope[], int moduleID)
{
	//unsigned long id_addr;
	int ret;
  	const Elf64_Rel *r = (const void *) reladdr;
  	const Elf64_Rel *end = (const void *) (reladdr + relsize);
	/*rellocate to plt table, count stands for a lable of plt*/
  	Elf64_Addr l_addr = map->l_addr; //elf_get_ptr(map->elf, 0); 
	count[moduleID] = 0;

  	/* We never bind lazily during ld.so bootstrap.  Unfortunately gcc is
     not clever enough to see through all the function calls to realize that.  */
//	Elf64_Addr *strtab = (Elf64_Addr)D_PTR(map, l_info[DT_STRTAB]);
//	const Elf64_Rel *relative = r;
	const Elf64_Sym *const symtab = (const void*)D_PTR(map, l_info[DT_SYMTAB]);
	DOMPRINTF_CALLED(dom->xch);
//	r += nrelative;

/*	if (l_addr != 0)
		for (; relative < r; ++relative) //this handles the first nrelative Elf64_Rela
			machine_rel_relative(dom, l_addr, relative, (void *)(l_addr + relative->r_offset));&symtab[ELF64_R_SYM(r->r_info)],
*/
/*---------------------------------------------------------------------------------*/
	for (; r < end; ++r){
		/*step 1:add a node for a list of head_lr*/
		/*current_lr = (struct lookup_relloc *)malloc(sizeof(struct lookup_relloc));
		current_lr->offset = r->r_offset;
		current_lr->label = count;
		current_lr->next = NULL;
		current_lr = current_lr->next;
		*/
		
		/*step 2: Let r_offset point to an item of .kylinx.got*/
		machine_rel_init_kylinx_got(dom, map, r, &symtab[ELF64_R_SYM(r->r_info)],(void *)(l_addr + r->r_offset), scope, moduleID);
		count[moduleID]++;
	}
/*---------------------------------------------------------------------------------*/
	/*Create a thread and use I/O select*/
	/*
	ret = pthread_create(&id_addr, NULL, (void *)watch_map, dom);
	if (ret != 0)
	{
		printf("create pthread error!\n");
	}
	*/
}

static inline void
relocate_object_rel(dom_t *dom, map_t *map, struct r_scope_elem *scope[], int moduleID)
{
    	struct { 
		Elf64_Addr start, size;					      
	}ranges[2] = { { 0, 0}, { 0, 0} };			      

	DOMPRINTF_CALLED(dom->xch);
	if (map->l_relocated) {
		DOMPRINTF("object (%s) already relocated", map->l_name);
		return;
	}
	assert(map->l_info[DT_TEXTREL] == NULL);
	
	/* Do the actual relocation of the object's GOT and other data.  */									      
    	if (map->l_info[DT_REL]) {									      
			ranges[0].start = D_PTR(map, l_info[DT_REL]);		      
			ranges[0].size = map->l_info[DT_RELSZ]->d_un.d_val;
	//	dynamic_do_Rel(dom, map, ranges[0].start, ranges[0].size, scope);
/*
		const Elf64_Rel *r = (const void *) ranges[0].start;
		const Elf64_Rel *end = (const void *) (ranges[0].start + ranges[0].size);
		const Elf64_Sym *const symtab = (const void*)D_PTR(map, l_info[DT_SYMTAB]);
		for (; r < end; ++r){
			Elf64_Addr *reloc_addr =  xc_dom_vaddr_to_ptr(dom, (Elf64_Addr)(r->r_offset));
			const unsigned int r_type = ELF64_R_TYPE(r->r_info);
			map_t *sym_map;
			Elf64_Addr value;

			DOMPRINTF_CALLED(dom->xch);

			if (r_type == R_386_RELATIVE) {
				*(reloc_addr) += map->l_addr;
    			}
			else if (r_type == R_386_NONE) {
				return;
			} 
			else {
				const Elf64_Sym *sym = &symtab[ELF64_R_SYM(r->r_info)];
				sym_map = resolve_map(dom, &sym, r_type, map, scope);
				value = (sym_map == NULL) ? 0 : (Elf64_Addr)(sym_map->l_addr) + sym->st_value;

				if (sym != NULL	&& ELF64_ST_TYPE(sym->st_info) == STT_GNU_IFUNC && sym->st_shndx != SHN_UNDEF)
					value = ((Elf64_Addr(*) (void)) value) ();

				switch (r_type) {
				case R_386_GLOB_DAT:
	  				*reloc_addr = value;
					break;
			
				case R_386_TLS_DTPMOD32:
				case R_386_TLS_DTPOFF32:
				case R_386_TLS_DESC:
				case R_386_TLS_TPOFF32:
					if (sym_map != NULL) {
						DOMPRINTF("%s: case %d not supported yet", __FUNCTION__, r_type);
					}
					break;
			
				case R_386_32:
					*reloc_addr +=  value;
					break;	
			
				case R_386_PC32:
					*reloc_addr += (value - (Elf64_Addr) reloc_addr);
					break;

				case R_386_IRELATIVE:
					value = map->l_addr + *reloc_addr;
					value = ((Elf64_Addr (*) (void)) value) ();
					*reloc_addr = value;
					break;

				default:
					DOMPRINTF("%s: bad type = %d", __FUNCTION__, r_type);
	  				break;
				}	
			}
		}
*/
	}									      

	if (map->l_info[DT_PLTREL] && (map->l_info[DT_PLTREL]->d_un.d_val == DT_REL))									      
	{	
	 	ranges[1].start = D_PTR((map), l_info[DT_JMPREL]);		      
		ranges[1].size = (map)->l_info[DT_PLTRELSZ]->d_un.d_val;
		dynamic_do_Rel(dom, map, ranges[1].start, ranges[1].size, scope, moduleID); 
		/*							      
		if (ranges[0].start + ranges[0].size == (start + size)){
		//	ranges[0].size -= size;
			if (ranges[0].start < start)
			{
				ranges[0].size -= size;	
			} else {
				ranges[1].start = start;					      
				ranges[1].size = size;					      
				ranges[1].lazy = 0;	
				ranges[1].size -= ranges[0].size;
			}

		}
		if (ranges[0].start >= (start + size) || (ranges[0].start + ranges[0].size) <= start) {						      
			ranges[1].start = start;					      
			ranges[1].size = size;					      
			ranges[1].lazy = 0;					      
	  	}
		if (ranges[0].size == 0) {								      
	    	ranges[1].start = start;					      
	    	ranges[1].size = size;					      
	    	ranges[1].lazy = 0;					      
	  	} else {								      		      
			ranges[0].size += size;					      
		}
		*/
	}
	/* Mark the object so we know this work has been done.  */
	map->l_relocated = 1;

}

int xc_dom_elf_dyn(struct xc_dom_image *dom)
{
	const elf_phdr *phdr;
	Elf64_Phdr *ph;
    	uint32_t i;	
	uint32_t e_phnum;
	uint32_t p_type, p_paddr, p_memsz, p_filesz, p_offset;	
	map_t *main_map = NULL, *l;
	//Elf64_Addr mapstart;
	//Elf64_Addr allocend;

	struct elf_binary *elf = dom->private_loader;

	//unsigned long mstart = pfn_to_mfn(elf->pstart);
	//unsigned long mend = pfn_to_mfn(elf->pend);
	//printf("----------------mstart = %x--------------mend = %x\n", mstart, mend);


	DOMPRINTF_CALLED(dom->xch);

	main_map = new_map(dom, "", "", lt_executable, NULL);
	main_map->elf = elf;
	//we use l_addr to adjust domU virt addr 
	//so l_addr = delta between the seg's real va and its planned va (virt_kstart = pstart + virt_offset)
	//main_map->l_addr = dom->kernel_seg.vstart - dom->parms.virt_kstart; //we need a delta
	//DOMPRINTF("%s: main_map->l_addr = 0x%x", __FUNCTION__, main_map->l_addr);
	
	e_phnum = elf_uval(elf, elf->ehdr, e_phnum);
	/*Scan the program header table for the dynamic section.*/
	for (i = 0; i < e_phnum; i++) {
		phdr = elf_phdr_by_index(elf, i);
		p_type = elf_uval(elf, phdr, p_type);
		ph = (Elf64_Phdr *)phdr;
		switch (p_type)
		{
			case PT_PHDR:
				/*Find out the load address.*/
				//main_map->l_addr = (Elf64_Addr)elf->dest - elf->pstart - dom->parms.virt_offset;
				main_map->l_addr = dom->kernel_seg.vstart - dom->parms.virt_kstart;
				DOMPRINTF("%s:main_map->l_addr = 0x%x", __FUNCTION__, main_map->l_addr);
				break;
			case PT_DYNAMIC:
				p_paddr = elf_uval(elf, phdr, p_paddr);
				p_offset = elf_uval(elf, phdr, p_offset);
				p_filesz = elf_uval(elf, phdr, p_filesz);
				p_memsz = elf_uval(elf, phdr, p_memsz);
				/*This tells us where to find the dynamic section,
				 which tells us everything we need to do.				
				*/
				main_map->l_ld = (Elf64_Dyn *)elf_get_ptr(elf, p_paddr);
				main_map->l_ldnum = ph->p_memsz/sizeof(Elf64_Dyn);
				DOMPRINTF("%s:l_ld = 0x%p, l_ldnum = %d", __FUNCTION__, main_map->l_ld, main_map->l_ldnum);
				break;
	//		case PT_LOAD:
				/*Remember where the main program starts in memory.*/
	/*			mapstart = main_map->l_addr + ph->p_vaddr;
				if (main_map->l_map_start > mapstart)
					main_map->l_map_start = mapstart;
	*/			
				/*Also where it ends.*/
	/*			allocend = main_map->l_addr + ph->p_vaddr + ph->p_memsz;
				if (main_map->l_map_end < allocend)
					main_map->l_map_end = allocend;
				if ((ph->p_flags & PF_X) && allocend > main_map->l_text_end)
					main_map->l_text_end = allocend;
				break;
	*/
		}
	}

	if (!main_map->l_ld) //no dynamic sections
		return 0;

	add_to_namespace_list(dom, main_map);
	if (get_dynamic_info(dom, main_map))
		goto err;

	setup_hash(dom, main_map);
	
	if (map_object_deps(dom, main_map))
		goto err;

	//zym: currently we dont handle GNU libs, of course
/*	i = ADDRIDX(DT_GNU_LIBLIST);
	assert(main_map->l_info[index] == NULL);
*/
	i = main_map->l_searchlist.r_nlist;
	while (i-- > 0) {
		l = main_map->l_initfini[i];
		//for now we assume no rel exists in elf64
		relocate_object_rel(dom, l, l->l_scope, i);
	}
	
	return 0;

err:
	return -1;
}


